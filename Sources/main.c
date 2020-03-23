#include "DSK6713_AIC23.h"
#include "dsk6713_dip.h"
#include "dsk6713_led.h"
#include "inicializaciones.h"
#include "sd.h"
#include "codec.h"
#include "leds.h"

#define DELAY 44100*1

//-------------- Vectores de señales ----------------

#pragma DATA_SECTION(sweep, ".EXT_RAM")
#pragma DATA_SECTION(left_ch, ".EXT_RAM")
#pragma DATA_SECTION(right_ch, ".EXT_RAM")
#pragma DATA_SECTION(twiddles, ".EXT_RAM")

vector sweep, left_ch, right_ch;
complex twiddles[MUESTRAS/2];

//-------------- Variables auxiliares ---------------

volatile unsigned int cuentas = 0;			// Lleva las cuentas del timer
volatile unsigned char reproduciendo = 0;	// Indica si el codec esta reproduciendo (1) o no (0)
unsigned char puls_levantados = 1;			// Indica si todos los pulsadores de la placa estan levantados (1), caso contrario (0)
unsigned char modo_anterior = 0;			// Indica en cual de los 4 modos de funcionamiento se encontraba el programa anteriormente
unsigned char modo_actual = 0;				// Indica el modo actual de funcionamiento
unsigned char num_medicion = 0;				// Indica el numero de medicion de la RI realizada
unsigned char estado = 0;					// Indica el estado actual del programa, si es distinto de 0 hubo algun error
int long_grabacion = 0;						// Indica la cantidad de muestras que se grabaran
int j = 0;									// Variable auxiliar para un loop for
int i = 0;

float rms_ambiente[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};	// Valor rms del ruido ambiente
float correc_db = 0.0;

extern union{	// Union para los datos del Codec
	Uint32 sample;
	short channel[2];
} codec_data;

void main ()
{
	//-------------- Inicializaciones ----------------------

	IRQ_globalDisable();
	DSK6713_init();
	DSK6713_DIP_init();
	DSK6713_LED_init();
	timer_init();

	twiddle_init(twiddles);

	//-------------- Genero el sweep -----------

	vectores_reset(1, &sweep);
	generate_sweep(&sweep);

	//-------------- Bucle infinito ------------------------

	while (1)
	{
		/*-----------------------------------------*/
		/*                MODO 1                   */
		/* Medicion de ruido ambiente del recinto  */
		/*-----------------------------------------*/

		if (!DSK6713_DIP_get(0) && puls_levantados)
		{
			leds_grabando(ON);
			modo_actual = 1;
			long_grabacion = sweep.muestras_utiles;

			vectores_reset(2, &left_ch, &right_ch);

			codec_open();

			play_codec(ON);
			while (reproduciendo);

			codec_close();

			leds_grabando(OFF);

			medir_rms_ambiente(&left_ch, &right_ch, twiddles, rms_ambiente);

			leds_modo(1);
			puls_levantados = 0;
			modo_anterior = 1;
		}

		/*-----------------------------------------*/
		/*                MODO 2                   */
		/*   Medicion y ajuste del nivel de señal  */
		/*-----------------------------------------*/

		if (!DSK6713_DIP_get(1) && puls_levantados)
		{
			leds_grabando(ON);
			modo_actual = 2;
			long_grabacion = sweep.muestras_utiles;

			if (modo_anterior == 1 || modo_anterior == 3 || modo_anterior == 4)
			{
				generate_sweep(&sweep);
			}

			vectores_reset(2, &left_ch, &right_ch);

			codec_open();

			play_codec(ON);
			while (reproduciendo);

			codec_close();

			leds_grabando(OFF);

			ajustar_sweep(&sweep, &left_ch, &right_ch, twiddles, rms_ambiente, &correc_db);

			leds_modo(2);
			puls_levantados = 0;
			modo_anterior = 2;
		}

		/*-----------------------------------------*/
		/*                MODO 3                   */
		/*   Calculo de la respuesta al impulso    */
		/*-----------------------------------------*/

		if (!DSK6713_DIP_get(2) && puls_levantados)
		{
			leds_grabando(ON);
			modo_actual = 3;
			long_grabacion = MUESTRAS;

			if (modo_anterior == 1 || modo_anterior == 4)
			{
				generate_sweep(&sweep);
			}

			vectores_reset(2, &left_ch, &right_ch);

			codec_open();

			play_codec(ON);
			while (reproduciendo);

			codec_close();

			leds_grabando(OFF);

			obtener_respuesta_impulso(&sweep, &left_ch, &right_ch, twiddles); //Falta guardar la cantidad de muestras utiles en los vetores de salida

			leds_modo(3);
			puls_levantados = 0;
			modo_anterior = 3;
		}

		/*-----------------------------------------*/
		/*                MODO 4                   */
		/*    Guardado en la tarjeta de memoria    */
		/*-----------------------------------------*/

		if (!DSK6713_DIP_get(3) && puls_levantados)
		{
			modo_actual = 4;
			sd_open();

			if (modo_anterior == 3)
			{
				estado = save_respuesta_impulso(&left_ch, &right_ch, num_medicion); //Terminar de definir la longitud de la RI a guardar
				check_error(estado);
				num_medicion++;
			}

			sd_close();

			leds_modo(4);
			puls_levantados = 0;
			modo_anterior = 4;
		}

		//Espera a que todos los pulsadores esten levantados para poder entrar en otro modo

		if ((DSK6713_DIP_get(0) && DSK6713_DIP_get(1) && DSK6713_DIP_get(2) && DSK6713_DIP_get(3)) == 1)
		{
			puls_levantados = 1;
		}

	}
}

interrupt void c_int11 ()
{
	if (j < long_grabacion)
	{
		if (i < DELAY)
		{
			codec_out(0);
			codec_data.sample = codec_in();
			i++;
		}
		else
		{
			if (modo_actual == 1)
			{
				codec_out(0);
			}
			else
			{
				codec_out((short)(sweep.samples[j].real * CONST_CONVER));
			}

			codec_data.sample = codec_in();
			left_ch.samples[j].real = ((float)codec_data.channel[LEFT])/CONST_CONVER;
			right_ch.samples[j].real = ((float)codec_data.channel[RIGHT])/CONST_CONVER;

			j++;
		}
	}
	else
	{
		left_ch.muestras_utiles = j;
		right_ch.muestras_utiles = j;
		j = 0;
		i = 0;
		play_codec(OFF);
	}
}

interrupt void c_int10 () //Interrumpe cada 1 micro segundo
{
	IRQ_clear(TimerEventId);

	if (cuentas != 0)
	{
		cuentas--;
	}
}
