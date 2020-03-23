/*
 * SD.c
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#include <stdio.h>
#include "ff.h"
#include "sd.h"

union
{	// Union para el encabezado del archivo .WAV
	char header[44];
	struct wav_file
	{
		unsigned int chunk_id;			//Escribe 'RIFF' (BE)
		unsigned int chunk_size; 		//=36 + SubChunk2Size  (LE)
		unsigned int format; 			//Escribe 'WAVE' (BE)

		unsigned int sub_chunk1_id; 		//Escribe 'fmt ' (BE)
		unsigned int sub_chunk1_size; 	//Vale 16 para PCM (LE)
		unsigned short audio_format; 	//Para PCM vale 1, otros valores indican algun grado de compresion (LE)
		unsigned short num_channels; 	//Cantidad de canales (1 = Mono, 2 = Estereo, etc) (LE)
		unsigned int sample_rate; 		//Frecuencia de muestreo (44100Hz = 0xAC44) (LE)
		unsigned int byte_rate; 			//= SampleRate * NumChannels * BitsPerSample/8 (LE)
		unsigned short block_align; 		//= NumChannels * BitsPerSample/8 (LE)
		unsigned short bits_per_sample; 	//Cantidad de bits por sample (8bits = 8, 16bits = 16, etc) (LE)

		unsigned int sub_chunk2_id; 		//Escribe 'data' (BE)
		unsigned int sub_chunk2_size; 	//= NumSamples * NumChannels * BitsPerSample/8 (LE)
	} wav_file;
}wav_header;

union
{	// Union auxiliar para cargar la respuesta al impulso en la SD
	short data[2];
	struct channels
	{
		short right;
		short left;
	} channels;
}sample;

int save_respuesta_impulso (vector *left_ch, vector *right_ch, int n_medicion)
{
	/* Guarda la respuesta al impulso obtenida por el algoritmo en la tarjeta SD.
	 *
	 * Args:
	 * 		*left_ch, *right_ch: Punteros a los vectores con los canales izq y der de la señal a guardar.
	 * 		n_medicion: Entero indicando el numero de medicion realizada en el mismo recinto para dar nombre al archivo WAV.
	 * Return:
	 * 		int indicando si hubo algun error durante el guardado. Si no hubo errores devuelve 0.
	 * */

	FATFS fat_fs;
	FIL fil;
	UINT bytes;
	TCHAR nombre[50];
	unsigned int i, size;

	size = (left_ch->muestras_utiles + right_ch->muestras_utiles) * 2;

	wav_header.wav_file.chunk_id = 0x46464952;
	wav_header.wav_file.chunk_size = 36 + size;
	wav_header.wav_file.format = 0x45564157;

	wav_header.wav_file.sub_chunk1_id = 0x20746d66;
	wav_header.wav_file.sub_chunk1_size = 0x00000010;
	wav_header.wav_file.audio_format = 0x0001;
	wav_header.wav_file.num_channels = 0x0002;
	wav_header.wav_file.sample_rate = 0x0000AC44;
	wav_header.wav_file.byte_rate = 0x0002B110;
	wav_header.wav_file.block_align = 0x0004;
	wav_header.wav_file.bits_per_sample = 0x0010;

	wav_header.wav_file.sub_chunk2_id = 0x61746164;
	wav_header.wav_file.sub_chunk2_size = size;

	sprintf(nombre, "IR%d.wav", n_medicion);

	f_mount(&fat_fs, "", 0);

	if (f_open(&fil, nombre, FA_WRITE | FA_OPEN_ALWAYS) != FR_OK)
	{
		return 1;
	}

	if (f_write(&fil, &wav_header.header, 44, &bytes) != FR_OK)
	{
		return 1;
	}

	for (i = 0; i < left_ch->muestras_utiles; i++)
	{

		sample.channels.left = (short)(CONST_CONVER * left_ch->samples[i].real);
		sample.channels.right = (short)(CONST_CONVER * right_ch->samples[i].real);

		if (f_write(&fil, &sample.data, 4, &bytes) != FR_OK)
		{
			return 1;

		}
	}

	f_close(&fil);
	return 0;
}
