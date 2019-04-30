/*
 * SD.h
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#ifndef SD_H_
#define SD_H_

#include "Resp_impulso.h"

void Load_sweep(Vector *sweep);
void Save_sweep(Vector *sweep);
void Save_RI(Vector *left_ch, Vector *right_ch);

#endif /* SD_H_ */
