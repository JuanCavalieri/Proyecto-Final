/*
 * Codec.h
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#ifndef CODEC_H_
#define CODEC_H_

#define LEFT  1 //Costantes para definir los canales de audio
#define RIGHT 0 //de entrada y de salida del codec

#define ON 1  //Constantes para arrancar o frenar
#define OFF 0 //la reproduccion en el codec

/* --------- Funciones implementadas --------- */

void Codec_out(short sample);
unsigned int Codec_in(void);
void Play_codec(unsigned char estado);

#endif /* CODEC_H_ */
