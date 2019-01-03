#include "DSK6713_AIC23.h"	//codec-DSK support file
#include "Funciones.h"
#include "Inicializaciones.h"
#include "ff.h"

volatile unsigned int cuentas = 0;
int j = 0;

FATFS FatFs;
FIL Fil;
FRESULT resultado;
unsigned int bw;

extern MCBSP_Handle SPI_Handler;

struct{  //Estructura con el encabezado del archivo .WAV
		unsigned int ChunkID; //Escribe 'RIFF' (BE)
		unsigned int ChunkSize; //=36 + SubChunk2Size (36 + 480000 = 480036) (LE)
		unsigned int Format; //Escribe 'WAVE' (BE)

		unsigned int SubChunk1ID; //Escribe 'fmt ' (BE)
		unsigned int SubChunk1Size; //Vale 16 para PCM (LE)
		unsigned short AudioFormat; //Para PCM vale 1, otros valores indican algun grado de compresion (LE)
		unsigned short NumChannels; //Cantidad de canales (1 = Mono, 2 = Estereo, etc) (LE)
		unsigned int SampleRate; //Frecuencia de muestreo (44100Hz = 0xAC44) (LE)
		unsigned int ByteRate; //= SampleRate * NumChannels * BitsPerSample/8 (LE)
		unsigned short BlockAlign; //= NumChannels * BitsPerSample/8 (LE)
		unsigned short BitsPerSample; //Cantidad de bits por sample (8bits = 8, 16bits = 16, etc) (LE)

		unsigned int SubChunk2ID; //Escribe 'data' (BE)
		unsigned int SubChunk2Size; //= NumSamples * NumChannels * BitsPerSample/8 (240000*1*2=480000) (LE)
	} WaveFile;

void main(){

	IRQ_globalDisable(); //Deshabilita todas las interrupciones
	DSK6713_init(); //Inicializa DSK-EMIF,PLL
	Timer_init(); //Inicializa el Timer
	MCBSP_SPI_init(); //Inicializa el MCBSP como SPI
	Interrup_init(); //Inicializa las interrupciones de

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

	Sweep();

	f_mount(&FatFs, "", 0);

	resultado = f_open(&Fil, "sweep.wav", FA_WRITE | FA_OPEN_ALWAYS);

	resultado = f_write(&Fil, &WaveFile, 44, &bw);

	for(j = 0;j < 240000;j++){
		resultado = f_write(&Fil, &sweep_signal[j], 2, &bw);
	}

	f_close(&Fil);

	MCBSP_FSET(PCR1, CLKXP, 0);
	MCBSP_FSET(PCR1, CLKXP, 1);

	while(1) {}
}

interrupt void c_int11(){         //interrupt service routine
    Output_data(sweep_signal[j]);
    if(j == 240000){
    	j = 0;
    }else{
    	j++;
    }
}

interrupt void c_int10(){ //Interrumpe cada 1 micro segundo
	IRQ_clear(TimerEventId);
	if(cuentas != 0)
		cuentas--;
}
