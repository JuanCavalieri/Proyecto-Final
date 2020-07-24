/*
 * Codec.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */

#include "codec.h"
#include "DSK6713_AIC23.h"

extern Uint32 CodecEventId;
extern volatile unsigned char reproduciendo;

union{	// Union para los datos del Codec
	Uint32 sample;
	short channel[2];
} codec_data;

void codecOut (
		short sample
)
{
	/* Reproduce en el codec el valor de sample.
	 *
	 * Args:
	 * 		sample: Valor de la muestra de la señal a reproducir por el codec.
	 * */

	codec_data.channel[0] = sample;
	codec_data.channel[1] = sample;

	MCBSP_write(DSK6713_AIC23_DATAHANDLE, codec_data.sample);
}

unsigned int codecIn (void)
{
	/* Lee una muestra tomada por el codec.
	 *
	 * Return:
	 * 		unsigned int con el valor de los dos canales del codec de 16 bits cada uno.
	 * */

	return MCBSP_read(DSK6713_AIC23_DATAHANDLE);
}

void playCodec (
		unsigned char state
)
{
	/* Activa o desactiva las interrupciones que hacen que el codec comienze a transmitir datos.
	 *
	 * Args:
	 * 		estado: Indica si el codec reproduce (1) o no (0).
	 * */

	if (state == 1)
	{
		IRQ_enable(CodecEventId);
		reproduciendo = 1;
		codecOut(0);
	}
	else
	{
		IRQ_reset(CodecEventId);
		reproduciendo = 0;
	}
}

