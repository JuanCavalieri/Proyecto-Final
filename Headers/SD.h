/*
 * SD.h
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#ifndef SD_H_
#define SD_H_

#include "Resp_impulso.h"

/* --------- Funciones implementadas --------- */

int Load_sweep(Vector *sweep);
int Save_sweep(Vector *sweep);
int Save_RI(Vector *left_ch, Vector *right_ch, int n_medicion);

#endif /* SD_H_ */
