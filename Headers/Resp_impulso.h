/*
 * Resp_impulso.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef RESP_IMPULSO_H_
#define RESP_IMPULSO_H_

#include "stdbool.h"

#define MUESTRAS	524288 		// Longitud de los vectores de las señales
#define CONST_CONVER	32767	// Constante de conversion entre float32 y int16
#define PI	3.14159265358979


typedef struct{		// Estructura que define un numero complejo
	float real;
	float imag;
} Complex;

typedef struct{		// Estructura que define un vector de señal
	Complex samples[MUESTRAS];
	int muestras_utiles;
} Vector;


/* --------- Funciones implementadas --------- */

void Vectores_reset(int cant, ... );
void Twiddle_init(Complex *twiddles);
void _normalize(Vector *signal);
void _bit_reversal(Vector *signal, unsigned int lenght);
static inline void swap(unsigned int forward, unsigned int rev, Vector *signal);
void _fft(Vector *signal, Complex *twiddles, int lenght);
void _ifft(Vector *signal, Complex *twiddles, int lenght);
void Generate_sweep(Vector *signal);
void _promediar(Vector *record_lft, Vector *record_rgt);
void _filtrar(Vector *signal, Vector *filtro);
float _valor_rms(Vector *signal, int length);
void Medir_RmsAmbiente(Vector *left_ch, Vector *right_ch, Complex *twiddles, float *max_rms, int *num_filtro);
void Ajustar_Sweep(Vector *sweep, Vector *left_ch, Vector *right_ch, Complex *twiddles, float rms_ambiente, int num_filtro);
void Corregir_RespFrec(Vector *signal, Vector *record, Vector *aux, Complex *twiddles);
void Obtener_RI(Vector *sweep, Vector *left_ch, Vector *right_ch, Complex *twiddles);

#endif /* RESP_IMPULSO_H_ */
