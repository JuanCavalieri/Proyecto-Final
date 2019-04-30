#include "DSK6713_AIC23.h"
#include "dsk6713_dip.h"
#include "Inicializaciones.h"
#include "SD.h"
#include "Codec.h"

//-------------- Vectores de se�ales ----------------

#pragma DATA_SECTION(sweep, ".EXT_RAM")
#pragma DATA_SECTION(left_ch, ".EXT_RAM")
#pragma DATA_SECTION(right_ch, ".EXT_RAM")
#pragma DATA_SECTION(twiddles, ".EXT_RAM")

Vector sweep, left_ch, right_ch;
Vector *sweep_ptr = &sweep;
Vector *left_ch_ptr = &left_ch;
Vector *right_ch_ptr = &right_ch;
Complex twiddles[MUESTRAS/2];

//-------------- Variables auxiliares ---------------

volatile unsigned int cuentas = 0;
int puls_levantados = 1;
int modo_anterior = 0;
int reproduciendo = 0;
int grabo_ruido = 0;
int j = 0;

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

	Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true, MUESTRAS);
	Twiddle_init(twiddles, MUESTRAS);

	//-------------- Cargar Sweep desde la SD -----------

	SD_init();
	//if (existe sweep_corregido en SD){
	//	Load_sweep(sweep_ptr);
	//}

	//-------------- Bucle infinito ------------------------

	while(1) {

		//---------- Modo 1 --------------------------------
		if(!DSK6713_DIP_get(0) && puls_levantados){
			Codec_init();
			Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true, MUESTRAS);
			Generate_sweep(sweep_ptr);

			Play_codec(ON);
			while(reproduciendo){};

			Corregir_RespFrec(sweep_ptr, left_ch_ptr, twiddles, MUESTRAS); //Agregar en esta funcion la asignacion a la variable de longitud de muestras utiles

			puls_levantados = 0;
			modo_anterior = 1;
		}

		//---------- Modo 2 --------------------------------
		if(!DSK6713_DIP_get(1) && puls_levantados){
			if(modo_anterior == 0 || modo_anterior == 2 || modo_anterior == 3){
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, false, true, true, MUESTRAS);
			}else{
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true, MUESTRAS);
				SD_init();
				//if (existe sweep_corregido en SD){
					//	Load_sweep(sweep_ptr);
					//}
			}
			Codec_init();

			grabo_ruido = 1;
			Play_codec(ON);
			while(reproduciendo){};
			grabo_ruido = 0;
			//Medir_ruido();

			Play_codec(ON);
			while(reproduciendo){};
			//Medir_rms_del_sweep();
			//Ajustar_nivel_del_sweep();

			puls_levantados = 0;
			modo_anterior = 2;
		}

		//---------- Modo 3 --------------------------------
		if(!DSK6713_DIP_get(2) && puls_levantados){
			if(modo_anterior == 0 || modo_anterior == 2 || modo_anterior == 3){
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, false, true, true, MUESTRAS);
			}else{
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true, MUESTRAS);
				SD_init();
				//if (existe sweep_corregido en SD){
					//	Load_sweep(sweep_ptr);
					//}
			}
			Codec_init();

			Play_codec(ON);
			while(reproduciendo){};
			Obtener_RI(sweep_ptr, left_ch_ptr, right_ch_ptr, twiddles, MUESTRAS);

			puls_levantados = 0;
			modo_anterior = 3;
		}

		//---------- Modo 4 --------------------------------
		if(!DSK6713_DIP_get(3) && puls_levantados){
			SD_init();
			if(modo_anterior == 1)
				Save_sweep(sweep_ptr);

			if(modo_anterior == 3)
				Save_RI(left_ch_ptr, right_ch_ptr); //Terminar de definir la longitud de la RI a guardar
													//Resolver el tema de porder guardar varias RI con diferentes nombres

			puls_levantados = 0;
			modo_anterior = 4;
		}

		//Espera a que todos los pulsadores esten levantados para poder entrar en otro modo
		if((DSK6713_DIP_get(0) && DSK6713_DIP_get(1) && DSK6713_DIP_get(2) && DSK6713_DIP_get(3)) == 1)
			puls_levantados = 1;
	}
}

interrupt void c_int11(){
	if(j < sweep.muestras_utiles){

		if(grabo_ruido)
			Codec_out(0);
		else
			Codec_out((short)sweep.samples[j].real);

		Codec_data.sample = Codec_in();
		left_ch.samples[j].real = (float)Codec_data.channel[LEFT];
		right_ch.samples[j].real = (float)Codec_data.channel[RIGHT];

		j++;
	}else{
		j = 0;
		Play_codec(OFF);
	}

}

interrupt void c_int10(){ //Interrumpe cada 1 micro segundo
	IRQ_clear(TimerEventId);
	if(cuentas != 0)
		cuentas--;
}
