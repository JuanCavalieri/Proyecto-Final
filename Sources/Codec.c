/*
 * Codec.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#include "Codec.h"
#include "DSK6713_AIC23.h"

#define ARRANCAR_CODEC	Codec_out(0);
extern Uint32 CodecEventId;
extern int reproduciendo;

union{	// Union para los datos del Codec
	Uint32 sample;
	short channel[2];
} Codec_data;

void Codec_out(short sample){

	/* Reproduce en el codec el valor de sample.
	 *
	 * Args:
	 * 		sample: Valor de la muestra de la señal a reproducir por el codec.
	 * */

	Codec_data.channel[0] = sample;
	Codec_data.channel[1] = sample;

	MCBSP_write(DSK6713_AIC23_DATAHANDLE, Codec_data.sample);
}

unsigned int Codec_in(void){

	/* Lee una muestra tomada por el codec.
	 *
	 * Return:
	 * 		unsigned int con el valor de los dos canales del codec de 16 bits cada uno.
	 * */

	return MCBSP_read(DSK6713_AIC23_DATAHANDLE);
}

void Play_codec(unsigned char estado){

	/* Activa o desactiva las interrupciones que hacen que el codec comienze a transmitir datos.
	 *
	 * Args:
	 * 		estado: Indica si el codec reproduce (1) o no (0).
	 * */

	if(estado == 1){
		IRQ_enable(CodecEventId);
		reproduciendo = 1;
		ARRANCAR_CODEC;
	}else{
		IRQ_reset(CodecEventId);
		reproduciendo = 0;
	}
}

