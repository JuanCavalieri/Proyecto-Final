/*
 * Resp_impulso.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#include "Resp_impulso.h"

extern Complex sweep[MUESTRAS], left_ch[MUESTRAS], right_ch[MUESTRAS];

void Vectores_reset(int lenght){

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


