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

/* Se declaran en el main.c para evitar conflictos con
 * la sentencia #pragma DATA_SECTION(). Sino estas variables deberian
 * ser parte de la libreria creada para este proyecto
 *
Vector sweep, left_ch, right_ch;
Vector *sweep_ptr = &sweep;
Vector *left_ch_ptr = &left_ch;
Vector *right_ch_ptr = &right_ch;
Complex twiddles[MUESTRAS/2];
*/

void Vectores_reset(Vector *sweep, Vector *left_ch, Vector *right_ch, bool sweep_rst, bool left_ch_rst, bool right_ch_rst){

	int i;

	for(i = 0; i < MUESTRAS; i++){

		if(sweep_rst){
			sweep->samples[i].real = 0;
			sweep->samples[i].imag = 0;
		}
		if(left_ch_rst){
			left_ch->samples[i].real = 0;
			left_ch->samples[i].imag = 0;
		}
		if(right_ch_rst){
			right_ch->samples[i].real = 0;
			right_ch->samples[i].imag = 0;
		}

	}
}

void Twiddle_init(Complex *twiddles){

	int i;
	double temp;

	for(i = 0; i < MUESTRAS/2; i++){

		temp = 2*PI*i/lenght;
		twiddles[i].real = (float)cos(temp);
		twiddles[i].imag = (float)-sin(temp);
	}
}

void Normalize(Vector *signal){

	int i;
	float norm;

	norm = 1.0/(float)MUESTRAS;

	for(i = 0;i < MUESTRAS; i++){

		signal->samples[i].real *= norm;
		signal->samples[i].imag *= norm;
	}
}

void Bit_reversal(Vector *signal, unsigned int lenght){

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

static inline void swap(unsigned int forward, unsigned int rev, Vector *signal){

	float temp;

    temp = signal->samples[forward].real;
    signal->samples[forward].real = signal->samples[rev].real;
    signal->samples[rev].real = temp;

    temp = signal->samples[forward].imag;
    signal->samples[forward].imag = signal->samples[rev].imag;
    signal->samples[rev].imag = temp;
}

void fft(Vector *signal, Complex *twiddles, int lenght){

	unsigned int even, odd, span, log, rootindex;    // indexes
	float temp;
	log=0;

	for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){         // iterate over the dual nodes

            odd |= span;                    // iterate over odd blocks only
            even = odd ^ span;              // even part of the dual node pair

            temp = signal->samples[even].real + signal->samples[odd].real;
            signal->samples[odd].real = signal->samples[even].real - signal->samples[odd].real;
            signal->samples[even].real = temp;

            temp = signal->samples[even].imag + signal->samples[odd].imag;
            signal->samples[odd].imag = signal->samples[even].imag - signal->samples[odd].imag;
            signal->samples[even].imag = temp;

            rootindex = (even<<log) & (lenght-1); // find root of unity index
            if(rootindex){                    // skip rootindex[0] (has an identity)

                temp = twiddles[rootindex].real * signal->samples[odd].real - twiddles[rootindex].imag * signal->samples[odd].imag;
                signal->samples[odd].imag = twiddles[rootindex].real * signal->samples[odd].imag + twiddles[rootindex].imag * signal->samples[odd].real;
                signal->samples[odd].real = temp;
            }

        } // end of loop over n

     } // end of loop over FFT stages

    Bit_reversal(signal, lenght);
}

void ifft(Vector *signal, Complex *twiddles, int lenght){

	unsigned int even, odd, span, log, rootindex;    // indexes
    float temp;
    log=0;

    for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){         // iterate over the dual nodes

            odd |= span;                    // iterate over odd blocks only
            even = odd ^ span;              // even part of the dual node pair

            temp = signal->samples[even].real + signal->samples[odd].real;
            signal->samples[odd].real = signal->samples[even].real - signal->samples[odd].real;
            signal->samples[even].real = temp;

            temp = signal->samples[even].imag + signal->samples[odd].imag;
            signal->samples[odd].imag = signal->samples[even].imag - signal->samples[odd].imag;
            signal->samples[even].imag = temp;

            rootindex = (even<<log) & (lenght-1); // find root of unity index
            if(rootindex){                    // skip rootindex[0] (has an identity)

                temp = twiddles[rootindex].real * signal->samples[odd].real - (-twiddles[rootindex].imag) * signal->samples[odd].imag;
                signal->samples[odd].imag = twiddles[rootindex].real * signal->samples[odd].imag + (-twiddles[rootindex].imag) * signal->samples[odd].real;
                signal->samples[odd].real = temp;
            }

        } // end of loop over n

     } // end of loop over FFT stages

    Bit_reversal(signal, lenght);
    Normalize(signal, lenght);
}

void Generate_sweep(Vector *signal){

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
		signal->samples[i].real = (float)(0.5 * sin(temp2 * (exp(temp1*t/DURACION_SWEEP) - 1)));

		if(i < 16714)
			signal->samples[i].real *= (float)(1 - exp(-alpha_fadein * i));

		if(i > 211679)
			signal->samples[i].real *= (float)(1 - exp(alpha_fadeout * (i - (MUESTRAS_SWEEP - 1))));

	}
	signal->muestras_utiles = MUESTRAS_SWEEP - 1;
}

void Corregir_RespFrec(Vector *signal, Vector *record, Complex *twiddles){

	int i;
	double energy = 0;
	double tau_start = 0.05;
	double tau_end = DURACION_SWEEP;
	double tau_g = tau_start;
	double previous_tau_g;
	double phi = 0;
	double previous_phi;
	double C = 0;
	double temp = 2*PI*FS/MUESTRAS;


	// Obtengo el espectro del sweep y de la grabacion
	fft(signal, twiddles, MUESTRAS);
	fft(record, twiddles, MUESTRAS);

	// Obtengo el espectro del sweep a sintetizar (en veces) y calculo el valor de la energia para la constante C
	for(i = 0; i < length; i++){

		record->samples[i].real = (float)((double)(signal->samples[i].real*signal->samples[i].real + signal->samples[i].imag*signal->samples[i].imag)/sqrt((double)(record->samples[i].real*record->samples[i].real + record->samples[i].imag*record->samples[i].imag)));
		if(i < MUESTRAS/2)
			energy += (double)(record->samples[i].real*record->samples[i].real);
	}

	// Calculo el valor de C
	C = (tau_end - tau_start)/energy;

	// Calculo el valor del retardo de grupo, la fase y sintetizo el espectro del barrido deseado
	for(i = 0; i < MUESTRAS; i++){

		if(i == 0){
			signal->samples[i].real = record->samples[i].real * (float)cos(phi);
			signal->samples[i].imag = record->samples[i].real * (float)sin(phi);
		} else if(i < MUESTRAS/2){
			tau_g = previous_tau_g + C*(double)(record->samples[i].real*record->samples[i].real);
			phi = previous_phi - temp*tau_g;
			signal->samples[i].real = record->samples[i].real * (float)cos(phi);
			signal->samples[i].imag = record->samples[i].real * (float)sin(phi);
		} else {
		    signal->samples[i].real = record->samples[i].real * (float)cos(phi);
			signal->samples[i].imag = record->samples[i].real * (float)sin(phi);
			tau_g = previous_tau_g - C*(double)(record->samples[i].real*record->samples[i].real);
			phi = previous_phi + temp*tau_g;
		}

		previous_tau_g = tau_g;
		previous_phi = phi;
	}

	// Paso el barrido a dominio temporal. De este vector hay que reproducir unicamente las muestras que ocupen el sweep
	ifft(signal, twiddles, MUESTRAS);

	// Ajusto el nivel del sweep
	for(i = 0; i < MUESTRAS; i++){
		if(i < FS*(tau_start + tau_end))
			signal->samples[i].real *= 2.0;
		else
			signal->samples[i].real = 0.0;
	}

	signal->muestras_utiles = FS*(tau_start  + tau_end);
}

void Obtener_RI(Vector *sweep, Vector *left_ch, Vector *right_ch, Complex *twiddles){

	int i;
	float denominador, tempLeft, tempRight;

	fft(sweep, twiddles, MUESTRAS);
	fft(left_ch, twiddles, MUESTRAS);
	fft(right_ch, twiddles, MUESTRAS);

	for(i = 0; i < MUESTRAS; i++){

		denominador = sweep->samples[i].real * sweep->samples[i].real + sweep->samples[i].imag * sweep->samples[i].imag;
		tempLeft = left_ch->samples[i].real;
		tempRight = right_ch->samples[i].real;

		left_ch->samples[i].real = (tempLeft * sweep->samples[i].real + left_ch->samples[i].imag * sweep->samples[i].imag) / denominador;
		left_ch->samples[i].imag = (left_ch->samples[i].imag * sweep->samples[i].real - tempLeft * sweep->samples[i].imag) / denominador;

		right_ch->samples[i].real = (tempRight * sweep->samples[i].real + right_ch->samples[i].imag * sweep->samples[i].imag) / denominador;
		right_ch->samples[i].imag = (right_ch->samples[i].imag * sweep->samples[i].real - tempRight * sweep->samples[i].imag) / denominador;
	}

	ifft(sweep, twiddles, MUESTRAS);
	ifft(left_ch, twiddles, MUESTRAS);
	ifft(right_ch, twiddles, MUESTRAS);
}
