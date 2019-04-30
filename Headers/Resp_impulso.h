/*
 * Resp_impulso.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef RESP_IMPULSO_H_
#define RESP_IMPULSO_H_

#include "stdbool.h"

#define MUESTRAS	524288
#define PI	3.14159265358979

typedef struct{
	float real;
	float imag;
} Complex;

typedef struct{
	Complex samples[MUESTRAS];
	unsigned char reseteado;
	int muestras_utiles;
} Vector;

void Vectores_reset(Vector *sweep, Vector *left_ch, Vector *right_ch, bool sweep_rst, bool left_ch_rst, bool right_ch_rst, int lenght);
void Twiddle_init(Complex *twiddles, int lenght);
void Normalize(Vector *signal, int lenght);
void Bit_reversal(Vector *signal, unsigned int lenght);
static inline void swap(unsigned int forward, unsigned int rev, Vector *signal);
void fft(Vector *signal, Complex *twiddles, int lenght);
void ifft(Vector *signal, Complex *twiddles, int lenght);
void Generate_sweep(Vector *signal);
void Corregir_RespFrec(Vector *signal, Vector *record, Complex *twiddles, int length);
void Obtener_RI(Vector *sweep, Vector *left_ch, Vector *right_ch, Complex *twiddles, int lenght);

#endif /* RESP_IMPULSO_H_ */
