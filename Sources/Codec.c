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

//---------------- Union para los datos del Codec ----------------

union{
	Uint32 sample;
	short channel[2];
} Codec_data;

void Codec_out(short sample){

	Codec_data.channel[0] = sample;
	Codec_data.channel[1] = sample;

	MCBSP_write(DSK6713_AIC23_DATAHANDLE, Codec_data.sample);
}

unsigned int Codec_in(void){

	return MCBSP_read(DSK6713_AIC23_DATAHANDLE);
}

void Play_codec(unsigned char estado){
	if(estado == 1){
		IRQ_enable(CodecEventId);
		ARRANCAR_CODEC;
	}else{
		IRQ_reset(CodecEventId);
	}
}

