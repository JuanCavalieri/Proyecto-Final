/*
 * Resp_impulso.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef RESP_IMPULSO_H_
#define RESP_IMPULSO_H_

#define MUESTRAS	524288

typedef struct{
	float real;
	float imag;
} Complex;

void Vectores_reset(int lenght);

#endif /* RESP_IMPULSO_H_ */
