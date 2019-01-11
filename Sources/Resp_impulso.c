/*
 * Resp_impulso.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */
#include "math.h"
#include "Resp_impulso.h"

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

	for(i = 0; i < lenght/2; i++){

		twiddles[i].real = (float)cos(2*PI*i/lenght);
		twiddles[i].imag = (float)-sin(2*PI*i/lenght);
	}
}

void Normalize(Complex *sweep, int lenght){

	int i;
	float norm;

	norm = 1.0/lenght;

	for(i = 0;i < lenght; i++){

		sweep[i].real *= norm;
		sweep[i].imag *= norm;
	}
}

void Bit_reversal(Complex *sweep, unsigned int lenght){

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
            swap(forward, rev, sweep);
            nodd = nmin1 ^ forward;              // compute the bitwise negations
            noddrev = nmin1 ^ rev;
            swap(nodd, noddrev, sweep);       // swap bitwise-negated pairs
        }

        nodd = forward ^ 1;                      // compute the odd values from the even
        noddrev = rev ^ halfn;
        swap(nodd, noddrev, sweep);           // swap odd unconditionally
    }
    // end of the bitreverse permutation loop
}

static inline void swap(unsigned int forward, unsigned int rev, Complex *sweep){

	float temp;

    temp = sweep[forward].real;
    sweep[forward].real = sweep[rev].real;
    sweep[rev].real = temp;

    temp = sweep[forward].imag;
    sweep[forward].imag = sweep[rev].imag;
    sweep[rev].imag = temp;
}

void fft(Complex *sweep, Complex *twiddles, int lenght){

	unsigned int even, odd, span, log, rootindex;    // indexes
    float temp;
    log=0;

    for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){         // iterate over the dual nodes

            odd |= span;                    // iterate over odd blocks only
            even = odd ^ span;              // even part of the dual node pair

            temp = sweep[even].real + sweep[odd].real;
            sweep[odd].real = sweep[even].real - sweep[odd].real;
            sweep[even].real = temp;

            temp = sweep[even].imag + sweep[odd].imag;
            sweep[odd].imag = sweep[even].imag - sweep[odd].imag;
            sweep[even].imag = temp;

            rootindex = (even<<log) & (lenght-1); // find root of unity index
            if(rootindex){                    // skip rootindex[0] (has an identity)

                temp = twiddles[rootindex].real * sweep[odd].real - twiddles[rootindex].imag * sweep[odd].imag;
                sweep[odd].imag = twiddles[rootindex].real * sweep[odd].imag + twiddles[rootindex].imag * sweep[odd].real;
                sweep[odd].real = temp;
            }

        } // end of loop over n

     } // end of loop over FFT stages

    Bit_reversal(sweep, lenght);
}

void ifft(Complex *sweep, Complex *twiddles, int lenght){

	unsigned int even, odd, span, log, rootindex;    // indexes
    float temp;
    log=0;

    for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){         // iterate over the dual nodes

            odd |= span;                    // iterate over odd blocks only
            even = odd ^ span;              // even part of the dual node pair

            temp = sweep[even].real + sweep[odd].real;
            sweep[odd].real = sweep[even].real - sweep[odd].real;
            sweep[even].real = temp;

            temp = sweep[even].imag + sweep[odd].imag;
            sweep[odd].imag = sweep[even].imag - sweep[odd].imag;
            sweep[even].imag = temp;

            rootindex = (even<<log) & (lenght-1); // find root of unity index
            if(rootindex){                    // skip rootindex[0] (has an identity)

                temp = twiddles[rootindex].real * sweep[odd].real - (-twiddles[rootindex].imag) * sweep[odd].imag;
                sweep[odd].imag = twiddles[rootindex].real * sweep[odd].imag + (-twiddles[rootindex].imag) * sweep[odd].real;
                sweep[odd].real = temp;
            }

        } // end of loop over n

     } // end of loop over FFT stages

    Bit_reversal(sweep, lenght);
    Normalize(sweep, lenght);
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
