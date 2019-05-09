/*
 * SD.c
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#include <stdio.h>
#include "ff.h"
#include "SD.h"

//------------ Union para el encabezado del archivo .WAV -----------------------------

union{
	char Header[44];
	struct WAVFile{
		unsigned int ChunkID;			//Escribe 'RIFF' (BE)
		unsigned int ChunkSize; 		//=36 + SubChunk2Size  (LE)
		unsigned int Format; 			//Escribe 'WAVE' (BE)

		unsigned int SubChunk1ID; 		//Escribe 'fmt ' (BE)
		unsigned int SubChunk1Size; 	//Vale 16 para PCM (LE)
		unsigned short AudioFormat; 	//Para PCM vale 1, otros valores indican algun grado de compresion (LE)
		unsigned short NumChannels; 	//Cantidad de canales (1 = Mono, 2 = Estereo, etc) (LE)
		unsigned int SampleRate; 		//Frecuencia de muestreo (44100Hz = 0xAC44) (LE)
		unsigned int ByteRate; 			//= SampleRate * NumChannels * BitsPerSample/8 (LE)
		unsigned short BlockAlign; 		//= NumChannels * BitsPerSample/8 (LE)
		unsigned short BitsPerSample; 	//Cantidad de bits por sample (8bits = 8, 16bits = 16, etc) (LE)

		unsigned int SubChunk2ID; 		//Escribe 'data' (BE)
		unsigned int SubChunk2Size; 	//= NumSamples * NumChannels * BitsPerSample/8 (LE)
	} WAVFile;
}WavHeader;

union{
	short data[2];
	struct channels{
		short right;
		short left;
	} channels;
}Sample;

int Load_sweep(Vector *sweep){

	FATFS FatFs;
	FILINFO fno;
	FIL Fil;
	UINT bytes;
	unsigned int i, cant_samples;
	short sample = 0;

	if(f_stat("sweep_corregido.wav", &fno) == FR_NO_FILE) //Chequear que funcione con el nombre en minuscula
		return 1;

	f_mount(&FatFs, "", 0);

	if(f_open(&Fil, "sweep_corregido.wav", FA_READ | FA_OPEN_ALWAYS) != FR_OK)
		return 2;

	if(f_read(&Fil, WavHeader.Header, 44, &bytes) != FR_OK)
		return 2;

	cant_samples = WavHeader.WAVFile.SubChunk2Size / (WavHeader.WAVFile.NumChannels * WavHeader.WAVFile.BitsPerSample / 8);

	for(i = 0; i < cant_samples; i++){

		if(f_read(&Fil, &sample, 2, &bytes) != FR_OK)
			return 2;

		sweep->samples[i].real = ((float)sample)/CONST_CONVER;
	}

	f_close(&Fil);
	return 0;
}

int Save_sweep(Vector *sweep){

	FATFS FatFs;
	FILINFO fno;
	FIL Fil;
	UINT bytes;
	unsigned int i, size;
	int sample = 0;

	size = sweep->muestras_utiles * 2;

	WavHeader.WAVFile.ChunkID = 0x46464952;
	WavHeader.WAVFile.ChunkSize = 36 + size;
	WavHeader.WAVFile.Format = 0x45564157;

	WavHeader.WAVFile.SubChunk1ID = 0x20746d66;
	WavHeader.WAVFile.SubChunk1Size = 0x00000010;
	WavHeader.WAVFile.AudioFormat = 0x0001;
	WavHeader.WAVFile.NumChannels = 0x0001;
	WavHeader.WAVFile.SampleRate = 0x0000AC44;
	WavHeader.WAVFile.ByteRate = 0x00015888;
	WavHeader.WAVFile.BlockAlign = 0x0002;
	WavHeader.WAVFile.BitsPerSample = 0x0010;

	WavHeader.WAVFile.SubChunk2ID = 0x61746164;
	WavHeader.WAVFile.SubChunk2Size = size;

	if(f_stat("sweep_corregido.wav", &fno) != FR_OK){
		return 1;
	}else{if(f_unlink("sweep_corregido.wav") != FR_OK)
		return 1;
	}

	f_mount(&FatFs, "", 0);

	if(f_open(&Fil, "sweep_corregido.wav", FA_WRITE | FA_OPEN_ALWAYS) != FR_OK)
		return 1;

	if(f_write(&Fil, &WavHeader.Header, 44, &bytes) != FR_OK)
		return 1;

	for(i = 0; i < sweep->muestras_utiles; i++){

		sample = CONST_CONVER * (short)sweep->samples[i].real;
		if(f_write(&Fil, &sample, 2, &bytes) != FR_OK)
			return 1;
	}

	f_close(&Fil);
	return 0;
}

int Save_RI(Vector *left_ch, Vector *right_ch, int n_medicion){

	FATFS FatFs;
	FIL Fil;
	UINT bytes;
	TCHAR nombre[50];
	unsigned int i, size, cant_samples;

	size = (left_ch->muestras_utiles + right_ch->muestras_utiles) * 2;

	WavHeader.WAVFile.ChunkID = 0x46464952;
	WavHeader.WAVFile.ChunkSize = 36 + size;
	WavHeader.WAVFile.Format = 0x45564157;

	WavHeader.WAVFile.SubChunk1ID = 0x20746d66;
	WavHeader.WAVFile.SubChunk1Size = 0x00000010;
	WavHeader.WAVFile.AudioFormat = 0x0001;
	WavHeader.WAVFile.NumChannels = 0x0002;
	WavHeader.WAVFile.SampleRate = 0x0000AC44;
	WavHeader.WAVFile.ByteRate = 0x0002B110;
	WavHeader.WAVFile.BlockAlign = 0x0004;
	WavHeader.WAVFile.BitsPerSample = 0x0010;

	WavHeader.WAVFile.SubChunk2ID = 0x61746164;
	WavHeader.WAVFile.SubChunk2Size = size;

	sprintf(nombre, "IR%d.wav", n_medicion);

	f_mount(&FatFs, "", 0);
	if(f_open(&Fil, nombre, FA_WRITE | FA_OPEN_ALWAYS) != FR_OK)
		return 1;

	if(f_write(&Fil, &WavHeader.Header, 44, &bytes) != FR_OK)
		return 1;

	if(left_ch->muestras_utiles > right_ch->muestras_utiles)
		cant_samples = left_ch->muestras_utiles;
	else
		cant_samples = right_ch->muestras_utiles;

	for(i = 0; i < cant_samples; i++){

		Sample.channels.left = CONST_CONVER * (short)left_ch->samples[i].real;
		Sample.channels.right = CONST_CONVER * (short)right_ch->samples[i].real;
		if(f_write(&Fil, &Sample.data, 4, &bytes) != FR_OK)
			return 1;
	}

	f_close(&Fil);
	return 0;
}
