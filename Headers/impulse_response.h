/*
 * Resp_impulso.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef RESP_IMPULSO_H_
#define RESP_IMPULSO_H_

#define SIGNAL_LENGHT	524288 		// Longitud de los vectores de las señales

typedef struct{		// Estructura que define un numero complejo
	float real;
	float imag;
} complex;

typedef struct{		// Estructura que define un vector de señal
	complex samples[SIGNAL_LENGHT];
	int muestras_utiles;
} vector;


/* --------- Funciones implementadas --------- */

void resetVectors (int cant, ... );

void twiddleInit (complex *twiddles);

void generateSweep (vector *signal);

void ambientNoise (vector *left_ch, vector *right_ch, complex *twiddles, float *rms_ambiente);

void sweepCorrection (vector *sweep, vector *left_ch, vector *right_ch, complex *twiddles, float *rms_ambiente, float *correc_db);

void getImpulseResponse (vector *sweep, vector *left_ch, vector *right_ch, complex *twiddles);

#endif /* RESP_IMPULSO_H_ */
