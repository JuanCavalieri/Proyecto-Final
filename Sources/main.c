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

volatile unsigned int cuentas = 0;	// Lleva las cuentas del timer
int puls_levantados = 1;			// Indica si todos los pulsadores de la placa estan levantados (1), caso contrario (0)
int modo_anterior = 0;				// Indica en cual de los 4 modos de funcionamiento se encontraba el programa anteriormente
int reproduciendo = 0;				// Indica si el codec esta reproduciendo (1) o no (0)
int grabo_ruido = 0;				// Indica si el algoritmo esta grabando el ruido ambiente (1) o no (0)
int num_medicion = 0;				// Indica el numero de medicion de la RI realizada
int num_filtro = 0;					// Indica el numero de filtro con el que se obtiene el maximo nivel de ruido ambiente
int estado = 0;						// Indica el estado actual del programa, si es distinto de 0 hubo algun error
int j = 0;							// Variable auxiliar para un loop for

float rms_ambiente = 0.0;					// Valor rms del ruido ambiente

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

	Vectores_reset(3, sweep_ptr, left_ch_ptr, right_ch_ptr);
	Twiddle_init(twiddles);

	//-------------- Cargar Sweep desde la SD -----------

	SD_init();
	estado = Load_sweep(sweep_ptr);
	check_error(estado);

	//-------------- Bucle infinito ------------------------

	while(1) {

		//---------- Modo 1: Correccion de la respuesta en frecuencia del sistema ---------------------------------------------------
		if(!DSK6713_DIP_get(0) && puls_levantados){
			Codec_init();
			Vectores_reset(3, sweep_ptr, left_ch_ptr, right_ch_ptr);
			Generate_sweep(sweep_ptr);

			Play_codec(ON);
			while(reproduciendo){};

			Corregir_RespFrec(sweep_ptr, left_ch_ptr, right_ch_ptr, twiddles);

			puls_levantados = 0;
			modo_anterior = 1;
		}

		//---------- Modo 2: Medicion del ruido ambiente y correccion de la amplitud de la exitacion --------------------------------
		if(!DSK6713_DIP_get(1) && puls_levantados){
			if(modo_anterior == 0 || modo_anterior == 2 || modo_anterior == 3){
				Vectores_reset(2, left_ch_ptr, right_ch_ptr);
			}else{
				Vectores_reset(3, sweep_ptr, left_ch_ptr, right_ch_ptr);
				SD_init();
				estado = Load_sweep(sweep_ptr);
				check_error(estado);
			}
			Codec_init();

			grabo_ruido = 1;
			Play_codec(ON);
			while(reproduciendo){};
			grabo_ruido = 0;

			Medir_RmsAmbiente(left_ch_ptr, right_ch_ptr, twiddles, &rms_ambiente, &num_filtro);

			Play_codec(ON);
			while(reproduciendo){};

			Ajustar_Sweep(sweep_ptr, left_ch_ptr, right_ch_ptr, twiddles, rms_ambiente, num_filtro);

			puls_levantados = 0;
			modo_anterior = 2;
		}

		//---------- Modo 3: Obtencion de la respuesta al impulso del recinto ------------------------------------------------------
		if(!DSK6713_DIP_get(2) && puls_levantados){
			if(modo_anterior == 0 || modo_anterior == 2 || modo_anterior == 3){
				Vectores_reset(2, left_ch_ptr, right_ch_ptr);
			}else{
				Vectores_reset(3, sweep_ptr, left_ch_ptr, right_ch_ptr);
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

		//---------- Modo 4: Guardado en la tarjeta de memoria ----------------------------------------------------------
		if(!DSK6713_DIP_get(3) && puls_levantados){
			SD_init();
			if(modo_anterior == 1){
				estado = Save_sweep(sweep_ptr);
				check_error(estado);
			}

			if(modo_anterior == 3){
				estado = Save_RI(left_ch_ptr, right_ch_ptr, num_medicion); //Terminar de definir la longitud de la RI a guardar
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
		left_ch.muestras_utiles = j;
		right_ch.muestras_utiles = j;
		j = 0;
		Play_codec(OFF);
	}

}

interrupt void c_int10(){ //Interrumpe cada 1 micro segundo
	IRQ_clear(TimerEventId);
	if(cuentas != 0)
		cuentas--;
}
