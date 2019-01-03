/*
 * Funciones.c
 *
 *  Created on: 20/05/2018
 *      Author: JuaniPC
 */
#include "DSK6713_AIC23.h"	//codec-DSK support file
#include "math.h"
#include "Funciones.h"
#include "ff.h"
#include "diskio.h"
#include "integer.h"

short sweep_signal[240000];
extern Uint32 CodecEventId;

union {  //Union para el manejo de los datos de entrada y salida del codec
	Uint32 uint;
	short channel[2];
	} AIC_data;


/*
FRESULT f_openwavefile(FIL* fp, const TCHAR* path, BYTE mode){
	unsigned int bw;

	WaveFile.ChunkID = 0x46464952;
	WaveFile.ChunkSize = 0x00075324;
	WaveFile.Format = 0x45564157;
	WaveFile.SubChunk1ID = 0x20746d66;
	WaveFile.SubChunk1Size = 0x00000010;
	WaveFile.AudioFormat = 0x0001;
	WaveFile.NumChannels = 0x0001;
	WaveFile.SampleRate = 0x0000AC44;
	WaveFile.ByteRate = 0x00015888;
	WaveFile.BlockAlign = 0x0002;
	WaveFile.BitsPerSample = 0x0010;
	WaveFile.SubChunk2ID = 0x61746164;
	WaveFile.SubChunk2Size = 0x00075300;

	if(f_open(fp, path, mode) == FR_OK){
		f_write(fp, &WaveFile.ChunkID, 4, &bw);
		f_write(fp, &WaveFile.ChunkSize, 4, &bw);
		f_write(fp, &WaveFile.Format, 4, &bw);
		f_write(fp, &WaveFile.SubChunk1ID, 4, &bw);
		f_write(fp, &WaveFile.SubChunk1Size, 4, &bw);
		f_write(fp, &WaveFile.AudioFormat, 2, &bw);
		f_write(fp, &WaveFile.NumChannels, 2, &bw);
		f_write(fp, &WaveFile.SampleRate, 4, &bw);
		f_write(fp, &WaveFile.ByteRate, 4, &bw);
		f_write(fp, &WaveFile.BlockAlign, 2, &bw);
		f_write(fp, &WaveFile.BitsPerSample, 2, &bw);
		f_write(fp, &WaveFile.SubChunk2ID, 4, &bw);
		f_write(fp, &WaveFile.SubChunk2Size, 4, &bw);
	}
	return FR_OK;
}
*/
void Sweep(){  //Genera la señal seno de exitacion del recinto
	int i;
	float x = 0.0;

	for(i=0;i<240000;i++){
			sweep_signal[i] = (short)1000.0*sin(2.0*PI*20.0*exp(x/240000.0));
			x += 7.0;
		}
}

void Output_data(short out_data){  //Saca un dato por el codec
	short CHANNEL_data;

	AIC_data.uint = 0;                 //Limpia la union de datos
	AIC_data.uint = out_data;          //32-bit data -->data structure

	//The existing interface defaults to right channel. To default instead to the
	//left channel and use output_sample(short), left and right channels are swapped
	//In main source program use LEFT 0 and RIGHT 1 (opposite of what is used here)
	CHANNEL_data = AIC_data.channel[RIGHT]; 			//swap left and right channels
	AIC_data.channel[RIGHT] = AIC_data.channel[LEFT];
	AIC_data.channel[LEFT] = CHANNEL_data;
	MCBSP_write(DSK6713_AIC23_DATAHANDLE,AIC_data.uint);//write/output data
}

Uint32 Input_data(){  //Lee un dato de entrada al codec
	short CHANNEL_data;

	AIC_data.uint = MCBSP_read(DSK6713_AIC23_DATAHANDLE); //Lee el dato

//Swapping left and right channels (see comments in output_sample())
	CHANNEL_data = AIC_data.channel[RIGHT]; 			//swap left and right channel
	AIC_data.channel[RIGHT] = AIC_data.channel[LEFT];
	AIC_data.channel[LEFT] = CHANNEL_data;

	return(AIC_data.uint);
}

void play_codec(Uint8 estado){  //Prende o apaga la reproduccion del codec
	if(estado == 1){
		IRQ_enable(CodecEventId); //Habilita las interrupciones de transmision
		Output_data(0); //Comienza a interrumpir el MSPB sacando una muestra
	}else{
		IRQ_reset(CodecEventId); //Deshabilita y limpia la bandera de la interrupcion
	}
}
