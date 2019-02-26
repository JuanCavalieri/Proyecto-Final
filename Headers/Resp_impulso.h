/*
 * Resp_impulso.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef RESP_IMPULSO_H_
#define RESP_IMPULSO_H_

#define MUESTRAS	524288
#define PI	3.14159265358979

typedef struct{
	float real;
	float imag;
} Complex;

void Vectores_reset(Complex *sweep, Complex *left_ch, Complex *right_ch, int lenght);
void Twiddle_init(Complex *twiddles, int lenght);
void Normalize(Complex *signal, int lenght);
void Bit_reversal(Complex *signal, unsigned int lenght);
static inline void swap(unsigned int forward, unsigned int rev, Complex *signal);
void fft(Complex *signal, Complex *twiddles, int lenght);
void ifft(Complex *signal, Complex *twiddles, int lenght);
void Obtener_RI(Complex *sweep, Complex *left_ch, Complex *right_ch, Complex *twiddles, int lenght);

#endif /* RESP_IMPULSO_H_ */
