/*
 * Resp_impulso.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef RESP_IMPULSO_H_
#define RESP_IMPULSO_H_

#define MUESTRAS	524288 		// Longitud de los vectores de las señales
#define CONST_CONVER	32767	// Constante de conversion entre float32 y int16

typedef struct{		// Estructura que define un numero complejo
	float real;
	float imag;
} complex;

typedef struct{		// Estructura que define un vector de señal
	complex samples[MUESTRAS];
	int muestras_utiles;
} vector;


/* --------- Funciones implementadas --------- */

void vectores_reset (int cant, ... );

void twiddle_init (complex *twiddles);

void generate_sweep (vector *signal);

void medir_rms_ambiente (vector *left_ch, vector *right_ch, complex *twiddles, float *rms_ambiente);

void ajustar_sweep (vector *sweep, vector *left_ch, vector *right_ch, complex *twiddles, float *rms_ambiente, float *correc_db);

void obtener_respuesta_impulso (vector *sweep, vector *left_ch, vector *right_ch, complex *twiddles);

#endif /* RESP_IMPULSO_H_ */
