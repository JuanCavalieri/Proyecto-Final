/*
 * Funciones.h
 *
 *  Created on: 20/05/2018
 *      Author: JuaniPC
 */

#ifndef FUNCIONES_H_
#define FUNCIONES_H_

#pragma DATA_SECTION(sweep_signal,".EXT_RAM")

#define PI 3.14159265359
#define LEFT  1 //Costantes para definir los canales de audio
#define RIGHT 0 //de entrada y de salida del codec

#define ON 1  //Constantes para arrancar o frenar
#define OFF 0 //la reproduccion en el codec

extern short sweep_signal[240000]; //Arreglo donde se almacena el sweep senoidal
//extern WaveFile Wavefile;

void Sweep(void);
void Output_data(short out_data);
Uint32 Input_data(void);
void play_codec(Uint8 estado);
//FRESULT f_openwavefile(FIL* fp, const TCHAR* path, BYTE mode);

#endif /* FUNCIONES_H_ */
