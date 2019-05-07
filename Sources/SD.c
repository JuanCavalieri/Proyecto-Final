/*
 * SD.c
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

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
	int data[2];
	struct channels{
		int left;
		int right;
	} channels;
}Sample;

int Load_sweep(Vector *sweep){

	FATFS FatFs;
	FRESULT fr;
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

void Save_sweep(Vector *sweep){

	FATFS FatFs;
	FIL Fil;
	UINT bytes;
	unsigned int i, size;
	int sample = 0;

	size = sweep->muestras_utiles * 4;

	WavHeader.WAVFile.ChunkID = 0x46464952;
	WavHeader.WAVFile.ChunkSize = 36 + size;
	WavHeader.WAVFile.Format = 0x45564157;

	WavHeader.WAVFile.SubChunk1ID = 0x20746d66;
	WavHeader.WAVFile.SubChunk1Size = 0x00000010;
	WavHeader.WAVFile.AudioFormat = 0x0001;
	WavHeader.WAVFile.NumChannels = 0x0001;
	WavHeader.WAVFile.SampleRate = 0x0000AC44;
	WavHeader.WAVFile.ByteRate = 0x0002B110;
	WavHeader.WAVFile.BlockAlign = 0x0004;
	WavHeader.WAVFile.BitsPerSample = 0x0020;

	WavHeader.WAVFile.SubChunk2ID = 0x61746164;
	WavHeader.WAVFile.SubChunk2Size = size;

	f_mount(&FatFs, "", 0);
	f_open(&Fil, "SWEEP.WAV", FA_WRITE | FA_OPEN_ALWAYS);
	f_write(&Fil, &WavHeader.Header, 44, &bytes);

	for(i = 0; i < sweep->muestras_utiles; i++){

		sample = (int)sweep->samples[i].real;
		f_write(&Fil, &sample, 4, &bytes);
	}

	f_close(&Fil);
}

void Save_RI(Vector *left_ch, Vector *right_ch){

	FATFS FatFs;
	FIL Fil;
	UINT bytes;
	unsigned int i, size, cant_samples;

	size = (left_ch->muestras_utiles + right_ch->muestras_utiles) * 4;

	WavHeader.WAVFile.ChunkID = 0x46464952;
	WavHeader.WAVFile.ChunkSize = 36 + size;
	WavHeader.WAVFile.Format = 0x45564157;

	WavHeader.WAVFile.SubChunk1ID = 0x20746d66;
	WavHeader.WAVFile.SubChunk1Size = 0x00000010;
	WavHeader.WAVFile.AudioFormat = 0x0001;
	WavHeader.WAVFile.NumChannels = 0x0002;
	WavHeader.WAVFile.SampleRate = 0x0000AC44;
	WavHeader.WAVFile.ByteRate = 0x00056220;
	WavHeader.WAVFile.BlockAlign = 0x0008;
	WavHeader.WAVFile.BitsPerSample = 0x0020;

	WavHeader.WAVFile.SubChunk2ID = 0x61746164;
	WavHeader.WAVFile.SubChunk2Size = size;

	f_mount(&FatFs, "", 0);
	f_open(&Fil, "SWEEP.WAV", FA_WRITE | FA_OPEN_ALWAYS);
	f_write(&Fil, &WavHeader.Header, 44, &bytes);

	cant_samples = 1000; // CAMBIAR

	for(i = 0; i < cant_samples; i++){

		Sample.channels.left = (int)left_ch->samples[i].real;
		Sample.channels.right = (int)right_ch->samples[i].real;
		f_write(&Fil, &Sample.data, 8, &bytes);
	}

	f_close(&Fil);
}
