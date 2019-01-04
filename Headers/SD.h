/*
 * SD.h
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#ifndef SD_H_
#define SD_H_

#include "Resp_impulso.h"

void Load_sweep(Complex* sweep);
void Save_sweep(Complex* sweep, int size);
void Save_RI(Complex* left_ch, Complex* right_ch, int size);

#endif /* SD_H_ */
