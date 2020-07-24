/*
 * SD.h
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#ifndef SD_H_
#define SD_H_

#include "impulse_response.h"

#define CONST_CONVER	32767	// Constante de conversion entre float32 y int16

/* --------- Funciones implementadas --------- */

int saveImpulseResponse (vector *left_ch, vector *right_ch, int n_medicion);

#endif /* SD_H_ */
