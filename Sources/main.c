#include "DSK6713_AIC23.h"
#include "dsk6713_dip.h"
#include "Inicializaciones.h"
#include "SD.h"
#include "Codec.h"

//-------------- Vectores de señales ----------------

#pragma DATA_SECTION(sweep, ".EXT_RAM")
#pragma DATA_SECTION(left_ch, ".EXT_RAM")
#pragma DATA_SECTION(right_ch, ".EXT_RAM")
#pragma DATA_SECTION(twiddles, ".EXT_RAM")

Complex sweep[MUESTRAS], left_ch[MUESTRAS], right_ch[MUESTRAS];
Complex twiddles[MUESTRAS/2];

//-------------- Variables auxiliares ---------------

volatile unsigned int cuentas = 0;
int j = 0;
int puls_levantados = 1;

extern union{
	Uint32 sample;
	short channel[2];
} Codec_data;

void main(){

	//-------------- Inicializaciones ----------------------

	IRQ_globalDisable();
	DSK6713_init();
	DSK6713_DIP_init();
	DSK6713_LED_init();
	Timer_init();
	Interrup_init();

	Vectores_reset(sweep, left_ch, right_ch, MUESTRAS);

	//-------------- Cargar Sweep desde la SD -----------

	SD_init();
	Load_sweep(sweep);

	//-------------- Bucle infinito ------------------------

	while(1) {

		//---------- Modo 1 --------------------------------
		if(!DSK6713_DIP_get(0) && puls_levantados){
			Codec_init();
			//Play_codec();
			//Corregir_resp_freq();
			SD_init();
			//Guardar_sweep();
			puls_levantados = 0;
		}

		//---------- Modo 2 --------------------------------
		if(!DSK6713_DIP_get(1) && puls_levantados){
			Codec_init();
			//Medir_ruido();
			//while(Corregir_nivel == False)
				//Play_test();
			puls_levantados = 0;
		}

		//---------- Modo 3 --------------------------------
		if(!DSK6713_DIP_get(2) && puls_levantados){
			Codec_init();
			//Play_codec();
			//Obtener_RI();
			puls_levantados = 0;
		}

		//---------- Modo 4 --------------------------------
		if(!DSK6713_DIP_get(3) && puls_levantados){
			SD_init();
			//Guardar_RI();
			puls_levantados = 0;
		}

		//Espera a que todos los pulsadores esten levantados para
		//poder entrar en otro modo
		if((DSK6713_DIP_get(0) && DSK6713_DIP_get(1) && DSK6713_DIP_get(2) && DSK6713_DIP_get(3)) == 1)
			puls_levantados = 1;
	}
}

interrupt void c_int11(){

   Codec_out((short)sweep[j].real);
   Codec_data.sample = Codec_in();
   left_ch[j].real = (float)Codec_data.channel[LEFT];
   right_ch[j].real = (float)Codec_data.channel[RIGHT];
}

interrupt void c_int10(){ //Interrumpe cada 1 micro segundo
	IRQ_clear(TimerEventId);
	if(cuentas != 0)
		cuentas--;
}
