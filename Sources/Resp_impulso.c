/*
 * Resp_impulso.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */
#include "math.h"
#include "Resp_impulso.h"

#define FS 44100.0 // Hz
#define DURACION_SWEEP 5.0 // segundos
#define FSTART 20.0 // Hz
#define FEND 22000.0 // Hz
#define DURACION_FADEIN 0.379 // segundos
#define DURACION_FADEOUT 0.2 // segundos

#define MUESTRAS_SWEEP FS*DURACION_SWEEP

void Vectores_reset(Complex *sweep, Complex *left_ch, Complex *right_ch, int lenght){

	int i;

	for(i = 0; i < lenght; i++){

		sweep[i].real = 0;
		sweep[i].imag = 0;
		left_ch[i].real = 0;
		left_ch[i].imag = 0;
		right_ch[i].real = 0;
		right_ch[i].imag = 0;
	}
}

void Twiddle_init(Complex *twiddles, int lenght){

	int i;
	double temp;

	for(i = 0; i < lenght/2; i++){

		temp = 2*PI*i/lenght;
		twiddles[i].real = (float)cos(temp);
		twiddles[i].imag = (float)-sin(temp);
	}
}

void Normalize(Complex *signal, int lenght){

	int i;
	float norm;

	norm = 1.0/(float)lenght;

	for(i = 0;i < lenght; i++){

		signal[i].real *= norm;
		signal[i].imag *= norm;
	}
}

void Bit_reversal(Complex *signal, unsigned int lenght){

    unsigned int i, forward, rev, zeros;
    unsigned int nodd, noddrev;        // to hold bitwise negated or odd values
    unsigned int halfn, quartn, nmin1;

    halfn = lenght>>1;            // frequently used 'constants'
    quartn = lenght>>2;
    nmin1 = lenght-1;

    forward = halfn;         // variable initialisations
    rev = 1;

    for(i=quartn; i; i--)    // start of bitreversed permutation loop, N/4 iterations
    {

     // Gray code generator for even values:

     nodd = ~i;                                  // counting ones is easier
     for(zeros=0; nodd&1; zeros++) nodd >>= 1;   // find trailing zero's in i
     forward ^= 2 << zeros;                      // toggle one bit of forward
     rev ^= quartn >> zeros;                     // toggle one bit of rev


        if(forward<rev)                          // swap even and ~even conditionally
        {
            swap(forward, rev, signal);
            nodd = nmin1 ^ forward;              // compute the bitwise negations
            noddrev = nmin1 ^ rev;
            swap(nodd, noddrev, signal);       // swap bitwise-negated pairs
        }

        nodd = forward ^ 1;                      // compute the odd values from the even
        noddrev = rev ^ halfn;
        swap(nodd, noddrev, signal);           // swap odd unconditionally
    }
    // end of the bitreverse permutation loop
}

static inline void swap(unsigned int forward, unsigned int rev, Complex *signal){

	float temp;

    temp = signal[forward].real;
    signal[forward].real = signal[rev].real;
    signal[rev].real = temp;

    temp = signal[forward].imag;
    signal[forward].imag = signal[rev].imag;
    signal[rev].imag = temp;
}

void fft(Complex *signal, Complex *twiddles, int lenght){

	unsigned int even, odd, span, log, rootindex;    // indexes
	float temp;
	log=0;

	for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){         // iterate over the dual nodes

            odd |= span;                    // iterate over odd blocks only
            even = odd ^ span;              // even part of the dual node pair

            temp = signal[even].real + signal[odd].real;
            signal[odd].real = signal[even].real - signal[odd].real;
            signal[even].real = temp;

            temp = signal[even].imag + signal[odd].imag;
            signal[odd].imag = signal[even].imag - signal[odd].imag;
            signal[even].imag = temp;

            rootindex = (even<<log) & (lenght-1); // find root of unity index
            if(rootindex){                    // skip rootindex[0] (has an identity)

                temp = twiddles[rootindex].real * signal[odd].real - twiddles[rootindex].imag * signal[odd].imag;
                signal[odd].imag = twiddles[rootindex].real * signal[odd].imag + twiddles[rootindex].imag * signal[odd].real;
                signal[odd].real = temp;
            }

        } // end of loop over n

     } // end of loop over FFT stages

    Bit_reversal(signal, lenght);
}

void ifft(Complex *signal, Complex *twiddles, int lenght){

	unsigned int even, odd, span, log, rootindex;    // indexes
    float temp;
    log=0;

    for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){         // iterate over the dual nodes

            odd |= span;                    // iterate over odd blocks only
            even = odd ^ span;              // even part of the dual node pair

            temp = signal[even].real + signal[odd].real;
            signal[odd].real = signal[even].real - signal[odd].real;
            signal[even].real = temp;

            temp = signal[even].imag + signal[odd].imag;
            signal[odd].imag = signal[even].imag - signal[odd].imag;
            signal[even].imag = temp;

            rootindex = (even<<log) & (lenght-1); // find root of unity index
            if(rootindex){                    // skip rootindex[0] (has an identity)

                temp = twiddles[rootindex].real * signal[odd].real - (-twiddles[rootindex].imag) * signal[odd].imag;
                signal[odd].imag = twiddles[rootindex].real * signal[odd].imag + (-twiddles[rootindex].imag) * signal[odd].real;
                signal[odd].real = temp;
            }

        } // end of loop over n

     } // end of loop over FFT stages

    Bit_reversal(signal, lenght);
    Normalize(signal, lenght);
}

void Generate_sweep(Complex *signal, int length){

	int i;
	double wstart = 2*PI*FSTART;
	double wend = 2*PI*FEND;
	double t = 0;
	double temp1 = log(wend/wstart);
	double temp2 = wstart*DURACION_SWEEP/temp1;
	double alpha_fadein = -log(0.001)/(FS*DURACION_FADEIN);
	double alpha_fadeout = -log(0.001)/(FS*DURACION_FADEOUT);

	for(i = 0; i < MUESTRAS_SWEEP; i++){

		t = (double)i/FS;
		signal[i].real = (float)(0.5 * sin(temp2 * (exp(temp1*t/DURACION_SWEEP) - 1)));

		if(i < 16714)
			signal[i].real *= (float)(1 - exp(-alpha_fadein * i));

		if(i > 211679)
			signal[i].real *= (float)(1 - exp(alpha_fadeout * (i - (MUESTRAS_SWEEP - 1))));

	}
}

void Corregir_RespFrec(Complex *signal, Complex *record, Complex *twiddles, int length){

	int i;
	double energy = 0;
	double tau_start = 0.25;
	double tau_end = DURACION_SWEEP;
	double tau_g = tau_start;
	double previous_tau_g;
	double phi = 0;
	double previous_phi;
	double C = 0;
	double temp = 2*PI*FS/MUESTRAS;


	// Obtengo el espectro del sweep y de la grabacion
	fft(signal, twiddles, length);
	fft(record, twiddles, length);

	// Obtengo el espectro del sweep a sintetizar (en veces) y calculo el valor de la energia para la constante C
	for(i = 0; i < length; i++){

		record[i].real = (float)((double)(signal[i].real*signal[i].real + signal[i].imag*signal[i].imag)/sqrt((double)(record[i].real*record[i].real + record[i].imag*record[i].imag)));
		if(i < length/2)
			energy += (double)(record[i].real*record[i].real);
	}

	// Calculo el valor de C
	C = (tau_end - tau_start)/energy;

	// Calculo el valor del retardo de grupo, la fase y sintetizo el espectro del barrido deseado
		for(i = 0; i < length; i++){

			if(i == 0){
				signal[i].real = record[i].real * (float)cos(phi);
				signal[i].imag = record[i].real * (float)sin(phi);
			} else if(i < length/2){
				tau_g = previous_tau_g + C*(double)(record[i].real*record[i].real);
				phi = previous_phi - temp*tau_g;
				signal[i].real = record[i].real * (float)cos(phi);
				signal[i].imag = record[i].real * (float)sin(phi);
			} else {
			    signal[i].real = record[i].real * (float)cos(phi);
				signal[i].imag = record[i].real * (float)sin(phi);
				tau_g = previous_tau_g - C*(double)(record[i].real*record[i].real);
				phi = previous_phi + temp*tau_g;
			}

			previous_tau_g = tau_g;
			previous_phi = phi;
		}

	// Paso el barrido a dominio temporal. De este vector hay que reproducir unicamente las muestras que ocupen el sweep
	ifft(signal, twiddles, length);

	// Ajusto el nivel del sweep multiplicando por 2
	for(i = 0; i < length; i++){
		signal[i].real *= 2.0;
	}
}

void Obtener_RI(Complex *sweep, Complex *left_ch, Complex *right_ch, Complex *twiddles, int lenght){

	int i;
	float denominador, tempLeft, tempRight;

	fft(sweep, twiddles, lenght);
	fft(left_ch, twiddles, lenght);
	fft(right_ch, twiddles, lenght);

	for(i = 0; i < lenght; i++){

		denominador = sweep[i].real * sweep[i].real + sweep[i].imag * sweep[i].imag;
		tempLeft = left_ch[i].real;
		tempRight = right_ch[i].real;

		left_ch[i].real = (tempLeft * sweep[i].real + left_ch[i].imag * sweep[i].imag) / denominador;
		left_ch[i].imag = (left_ch[i].imag * sweep[i].real - tempLeft * sweep[i].imag) / denominador;

		right_ch[i].real = (tempRight * sweep[i].real + right_ch[i].imag * sweep[i].imag) / denominador;
		right_ch[i].imag = (right_ch[i].imag * sweep[i].real - tempRight * sweep[i].imag) / denominador;
	}

	ifft(sweep, twiddles, lenght);
	ifft(left_ch, twiddles, lenght);
	ifft(right_ch, twiddles, lenght);
}
