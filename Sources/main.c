#include "DSK6713_AIC23.h"
#include "dsk6713_dip.h"
#include "dsk6713_led.h"
#include "Inicializaciones.h"
#include "SD.h"
#include "Codec.h"
#include "Runtime_errors.h"

//-------------- Vectores de señales ----------------

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
int n_medicion = 0;
int estado = 0;
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

	Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true);
	Twiddle_init(twiddles);

	//-------------- Cargar Sweep desde la SD -----------

	SD_init();
	estado = Load_sweep(sweep_ptr);
	check_error(estado);

	//-------------- Bucle infinito ------------------------

	while(1) {

		//---------- Modo 1 --------------------------------
		if(!DSK6713_DIP_get(0) && puls_levantados){
			Codec_init();
			Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true);
			Generate_sweep(sweep_ptr);

			Play_codec(ON);
			while(reproduciendo){};

			Corregir_RespFrec(sweep_ptr, left_ch_ptr, twiddles);

			puls_levantados = 0;
			modo_anterior = 1;
		}

		//---------- Modo 2 --------------------------------
		if(!DSK6713_DIP_get(1) && puls_levantados){
			if(modo_anterior == 0 || modo_anterior == 2 || modo_anterior == 3){
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, false, true, true);
			}else{
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true);
				SD_init();
				estado = Load_sweep(sweep_ptr);
				check_error(estado);
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
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, false, true, true);
			}else{
				Vectores_reset(sweep_ptr, left_ch_ptr, right_ch_ptr, true, true, true);
				SD_init();
				estado = Load_sweep(sweep_ptr);
				check_error(estado);
			}
			Codec_init();

			Play_codec(ON);
			while(reproduciendo){};
			Obtener_RI(sweep_ptr, left_ch_ptr, right_ch_ptr, twiddles); //Falta guardar la cantidad de muestras utiles en los vetores de salida

			puls_levantados = 0;
			modo_anterior = 3;
		}

		//---------- Modo 4 --------------------------------
		if(!DSK6713_DIP_get(3) && puls_levantados){
			SD_init();
			if(modo_anterior == 1){
				estado = Save_sweep(sweep_ptr);
				check_error(estado);
			}

			if(modo_anterior == 3){
				estado = Save_RI(left_ch_ptr, right_ch_ptr, n_medicion); //Terminar de definir la longitud de la RI a guardar
				check_error(estado);
				n_medicion++;
			}

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
			Codec_out((short)(sweep.samples[j].real * CONST_CONVER));

		Codec_data.sample = Codec_in();
		left_ch.samples[j].real = ((float)Codec_data.channel[LEFT])/CONST_CONVER;
		right_ch.samples[j].real = ((float)Codec_data.channel[RIGHT])/CONST_CONVER;

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
