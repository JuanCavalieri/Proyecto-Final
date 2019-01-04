/*
 * SD.c
 *
 *  Created on: 03/01/2019
 *      Author: Juani
 */

#include "ff.h"

//------------ Variables para el manejo de archivos ----------------------------------

FATFS FatFs;
FIL Fil;

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

void Load_sweep(Complex* sweep){

	unsigned int i, cant_samples;
	int sample = 0;

	f_mount(&FatFs, "", 0);
	f_open(&Fil, "SWEEP.WAV", FA_READ | FA_OPEN_ALWAYS);
	f_read(&Fil, WavHeader.Header, 44, &bytes);

	cant_samples = WavHeader.WAVFile.SubChunk2Size / (WavHeader.WAVFile.NumChannels * WavHeader.WAVFile.BitsPerSample / 8);

	for(i = 0; i < cant_samples; i++){
		f_read(&Fil, &sample, 2, &bytes);
		sweep[i].real = (float)sample;
	}
}
