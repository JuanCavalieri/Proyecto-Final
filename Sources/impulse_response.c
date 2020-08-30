/*
 * impulse_response.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */
#include <stdarg.h>
#include "math.h"
#include "impulse_response.h"
#include "filters.h"

#define FS 44100.0 				// Frecuencia de muestreo de las se�ales de audio [Hz]
#define SWEEP_DURATION 5.0 		// Tiempo de duracion de la se�al de exitacion [s]
#define FSTART 10.0 			// Frecuencia inicial de la se�al generada [Hz]
#define FEND 22000.0 			// Frecuencia final de la se�al generada [Hz]
#define FADEIN_DURATION 0.2 	// Tiempo de duracion del fadein de la se�al generada [s]
#define FADEOUT_DURATION 0.2 	// Tiempo de duracion del fadeout de la se�al generada [s]
#define PI	3.14159265358979

#define SWEEP_SAMPLES FS*SWEEP_DURATION	// Cantidad de muestras que tiene la se�al generada

/* Se declaran en el main.c para evitar conflictos con
 * la sentencia #pragma DATA_SECTION(). Sino estas variables deberian
 * ser parte de la libreria creada para este proyecto
 *
vector sweep, left_ch, right_ch;
complex twiddles[SIGNAL_LENGHT/2];
*/

static void normalize (
		vector *signal
)
{
	/* Aplica la normalizacion 1/N a los valores devueltos por la funcion de ifft.
	 *
	 * Args:
	 * 		*signal: Puntero al vector se�al al que se le aplicara la normalizacion
	 * */

	int i;
	float norm;

	norm = 1.0/(float)SIGNAL_LENGHT;

	for (i = 0; i < SIGNAL_LENGHT; i++)
	{
		signal->samples[i].real *= norm;
		signal->samples[i].imag *= norm;
	}
}

static inline void swap (
		unsigned int forward,
		unsigned int rev,
		vector *signal
)
{
	/* Funcion auxiliar necesaria para ejecutar _bit_reversal.
	 * */

	float temp;

    temp = signal->samples[forward].real;
    signal->samples[forward].real = signal->samples[rev].real;
    signal->samples[rev].real = temp;

    temp = signal->samples[forward].imag;
    signal->samples[forward].imag = signal->samples[rev].imag;
    signal->samples[rev].imag = temp;
}

static void bitReversal (
		vector *signal,
		unsigned int lenght
)
{
	/* Algoritmo de bit reversal requerido en las funciones de fft y ifft.
	 *
	 * Args:
	 * 		*signal: Puntero al vector se�al al que se le aplicara la reversion.
	 * 		length: Longitud del vector al que se le aplicara la reversion.
	 * */

    unsigned int i, forward, rev, zeros;
    unsigned int nodd, noddrev;
    unsigned int halfn, quartn, nmin1;

    halfn = lenght>>1;
    quartn = lenght>>2;
    nmin1 = lenght-1;

    forward = halfn;
    rev = 1;

    for (i=quartn; i; i--)
    {
		nodd = ~i;
		for (zeros=0; nodd&1; zeros++) nodd >>= 1;
		forward ^= 2 << zeros;
		rev ^= quartn >> zeros;

			if (forward<rev)
			{
				swap(forward, rev, signal);
				nodd = nmin1 ^ forward;
				noddrev = nmin1 ^ rev;
				swap(nodd, noddrev, signal);
			}

			nodd = forward ^ 1;
			noddrev = rev ^ halfn;
			swap(nodd, noddrev, signal);
    }
}

static void fft (
		vector *signal,
		complex *twiddles,
		int lenght
)
{
	/* Realiza la fft (Fast Fourier Transform) de longitud length sobre la se�al en signal en dominio tiempo.
	 * Almacena el resultado, en dominio frecuencial, en el mismo vector signal. La funcion presupone que el
	 * vector signal tiene longitud length y que si la se�al util es mas corta que el propio vector signal,
	 * el resto del mismo vale 0.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la se�al a transformar.
	 * 		*twiddles: Puntero al vector con las constantes twiddle.
	 * 		length: Longitud de la transformada.
	 * */

	unsigned int even, odd, span, log, rootindex;
	float temp;
	log=0;

	for (span=lenght>>1; span; span>>=1, log++)
	{
        for (odd=span; odd<lenght; odd++)
        {
            odd |= span;
            even = odd ^ span;

            temp = signal->samples[even].real + signal->samples[odd].real;
            signal->samples[odd].real = signal->samples[even].real - signal->samples[odd].real;
            signal->samples[even].real = temp;

            temp = signal->samples[even].imag + signal->samples[odd].imag;
            signal->samples[odd].imag = signal->samples[even].imag - signal->samples[odd].imag;
            signal->samples[even].imag = temp;

            rootindex = (even<<log) & (lenght-1);
            if (rootindex)
            {
                temp = twiddles[rootindex].real * signal->samples[odd].real - twiddles[rootindex].imag * signal->samples[odd].imag;
                signal->samples[odd].imag = twiddles[rootindex].real * signal->samples[odd].imag + twiddles[rootindex].imag * signal->samples[odd].real;
                signal->samples[odd].real = temp;
            }
        }
	}

    bitReversal(signal, lenght);
}

static void ifft (
		vector *signal,
		complex *twiddles,
		int lenght
)
{
	/* Realiza la ifft (Inverse Fast Fourier Transform) de longitud length sobre la se�al en signal en dominio frecuencial.
	 * Almacena el resultado, en dominio tiempo, en el mismo vector signal. La funcion presupone que el
	 * vector signal tiene longitud length.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la se�al a antitransformar.
	 * 		*twiddles: Puntero al vector con las constantes twiddle.
	 * 		length: Longitud de la transformada.
	 * */

	unsigned int even, odd, span, log, rootindex;
    float temp;
    log=0;

    for (span=lenght>>1; span; span>>=1, log++)
    {
        for (odd=span; odd<lenght; odd++)
        {
            odd |= span;
            even = odd ^ span;

            temp = signal->samples[even].real + signal->samples[odd].real;
            signal->samples[odd].real = signal->samples[even].real - signal->samples[odd].real;
            signal->samples[even].real = temp;

            temp = signal->samples[even].imag + signal->samples[odd].imag;
            signal->samples[odd].imag = signal->samples[even].imag - signal->samples[odd].imag;
            signal->samples[even].imag = temp;

            rootindex = (even<<log) & (lenght-1);
            if (rootindex)
            {
                temp = twiddles[rootindex].real * signal->samples[odd].real - (-twiddles[rootindex].imag) * signal->samples[odd].imag;
                signal->samples[odd].imag = twiddles[rootindex].real * signal->samples[odd].imag + (-twiddles[rootindex].imag) * signal->samples[odd].real;
                signal->samples[odd].real = temp;
            }
        }
    }

    bitReversal(signal, lenght);
    normalize(signal);
}

static void filter (
		vector *signal,
		vector *filtro
)
{
	/* Realiza el filtrado entre la se�al en signal y el filtro. El mismo se realiza en
	 * dominio frecuencial, por lo que ambos vectores deben contener los espectros de las
	 * se�ales correspondientes.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la se�al a filtrar.
	 * 		*filtro: Puntero al vector con el filtro a aplicar.
	 * */

	int i;
	float temp;

	for (i = 0; i < SIGNAL_LENGHT; i++)
	{
		temp = filtro->samples[i].real;
		filtro->samples[i].real = signal->samples[i].real*filtro->samples[i].real - signal->samples[i].imag*filtro->samples[i].imag;
		filtro->samples[i].imag = signal->samples[i].real*filtro->samples[i].imag + signal->samples[i].imag*temp;
	}
}

static float rmsValue (
		vector *signal,
		int length
)
{
	/* Calcula el valor rms de la se�al en el vector signal. El valor length indica hasta
	 * que componente del vector se toma la se�al para calcular su valor rms.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la se�al a la cual calcular el valor rms.
	 * 		length: Longitud que abarca la se�al util en el vector signal.
	 * 	Return:
	 * 		float con el valor rms de la se�al de entrada.
	 * */

	int i;
	float temp = 0.0;

	for (i = 0; i <= length; i++)
	{
		temp += signal->samples[i].real*signal->samples[i].real;
	}

	return (float)sqrt(temp/(float)length);
}

static void normalizeIR (
		vector *left_ch,
		vector *right_ch
)
{
	/* Normaliza la respuesta al impulso obtenida. La normalizacion se hace
	 * teniendo en cuenta ambos canales a la vez.
	 * 
	 * Args:
	 * 		*left_ch, *right_ch: Punteros a los vectores de las señales grabadas.
	 * */

	int i;
	float max_value = 0.0;
	float correction;

	for (i = 0; i < left_ch->muestras_utiles; i++)
	{
		if (left_ch->samples[i].real > max_value)
		{
			max_value = left_ch->samples[i].real;
		}

		if (right_ch->samples[i].real > max_value)
		{
			max_value = right_ch->samples[i].real;
		}
	}

	correction = 1.0/max_value;

	for (i = 0; i < left_ch->muestras_utiles; i++)
	{
		left_ch->samples[i].real = left_ch->samples[i].real * correction;
		right_ch->samples[i].real = right_ch->samples[i].real * correction;
	}
}

void resetVectors (int cant, ... )
{
	/* Permite resetear los valores de todas las componentes de los vectores de se�al que se pasan como argumentos.
	 *
	 * Args:
	 * 		cant: Cantidad de vectores que se desean resetear
	 * 		El resto de parametros son los punteros a los vectores que se desean resetear
	 * */

	int i;
	vector *vec;
	va_list pa;

	va_start(pa, cant);

	while (cant--)
	{
		vec = va_arg(pa, vector*);
	    for (i = 0; i < SIGNAL_LENGHT; i++)
	    {
	    	vec->samples[i].real = 0.0;
	        vec->samples[i].imag = 0.0;
	    }
	}

	va_end(pa);
}

void twiddleInit (
		complex *twiddles
)
{
	/* Inicializa las constantes Twiddle que se utilizan en las funciones fft y ifft.
	 *
	 * Args:
	 * 		*twiddles: Puntero al array de numeros complejos donde se almacenaran las constantes
	 * */

	int i;
	double temp;

	for (i = 0; i < SIGNAL_LENGHT/2; i++)
	{
		temp = 2*PI*i/SIGNAL_LENGHT;
		twiddles[i].real = (float)cos(temp);
		twiddles[i].imag = (float)-sin(temp);

	}
}

void generateSweep (
		vector *signal
)
{
	/* Genera una se�al senoidal de frecuencia variable que comienza en FSTART y termina en FEND,
	 * con una duracion igual a DURACION_SWEEP que se almacena en el vector signal segun la formula
	 * presente en [], referenciado en la bibliografia del informe del presente trabajo. Ademas, se
	 * le aplica a la misma un efecto de fade in de duracion DURACION_FADEIN y otro de fade out de
	 * duracion DURACION_FADEOUT.
	 *
	 * Args:
	 * 		*signal: Puntero al vector donde almacenar la se�al generada.
	 * */

	int i;
	double wstart = 2*PI*FSTART;
	double wend = 2*PI*FEND;
	double t = 0;
	double temp1 = log(wend/wstart);
	double temp2 = wstart*SWEEP_DURATION/temp1;
	double alpha_fadein = -log(0.001)/(FS*FADEIN_DURATION);
	double alpha_fadeout = -log(0.001)/(FS*FADEOUT_DURATION);

	for (i = 0; i < SWEEP_SAMPLES; i++)
	{
		t = (double)i/FS;
		signal->samples[i].real = (float)(0.1 * sin(temp2 * (exp(temp1*t/SWEEP_DURATION) - 1)));

		if (i < (int)FS * FADEIN_DURATION)
		{
			signal->samples[i].real *= (float)(1 - exp(-alpha_fadein * i));
		}

		if (i > (SWEEP_SAMPLES - (int)FS * FADEOUT_DURATION))
		{
			signal->samples[i].real *= (float)(1 - exp(alpha_fadeout * (i - (SWEEP_SAMPLES - 1))));
		}
	}
	signal->muestras_utiles = SWEEP_SAMPLES - 1;
}

void ambientNoise (
		vector *left_ch,
		vector *right_ch,
		complex *twiddles,
		float *rms_ambiente)
{
	/* Mide el ruido ambiente de la se�al estereo grabada por el sistema. Realiza un filtrado con los filtros
	 * del archivo filtros.h y obtiene la banda de frecuencia en la cual el valor rms de la se�al es mayor.
	 *
	 * Args:
	 * 		*left_ch, *right_ch: Punteros a los vectores con las se�ales grabadas
	 * 		*twiddles: Puntero al vector con las constantes twiddles
	 * 		*max_rms: Puntero a la variable para almacenar el maximo valor rms obtenido
	 * 		*num_filtro: Puntero a la variable para almacenar el numero de filtro con el cual se obtuvo el maximo rms
	 * */

	int i, j, cant_filtros, long_filtros;

	fft(left_ch, twiddles, SIGNAL_LENGHT);	// Hago la fft del ruido ambiente

	cant_filtros = sizeof(filtros) / sizeof(filtros[0]);
	long_filtros = sizeof(filtros[0]) / sizeof(filtros[0][0]);

	for (i = 0; i < cant_filtros; i++)
	{
		resetVectors(1, right_ch);

		for (j = 0; j < long_filtros; j++)
		{
			right_ch->samples[j].real = filtros[i][j];
		}

		fft(right_ch, twiddles, SIGNAL_LENGHT);
		filter(left_ch, right_ch);
		ifft(right_ch, twiddles, SIGNAL_LENGHT);

		rms_ambiente[i] = rmsValue(right_ch, SWEEP_SAMPLES);

	}
	resetVectors(2, left_ch, right_ch);
}

void sweepCorrection (
		vector *sweep,
		vector *left_ch,
		vector *right_ch,
		complex *twiddles,
		float *rms_ambiente,
		float *correc_db)
{
	/* Obtiene el valor rms de la se�al de exitacion grabada y verifica que la relacion se�al ruido sea mayor o igual a 60dB.
	 * En caso de que sea menor, modifica el nivel de la se�al de exitacion para lograr dicha SNR.
	 *
	 * Args:
	 * 		*sweep: Puntero al vector de se�al de exitacion
	 * 		*twiddles: Puntero al vector con las constantes twiddles
	 * 		max_rms: Valor de maximo rms de ruido ambiente obtenido previamente
	 * 		num_filtro: Numero de filtro con el que se obtuvo el maximo rms de ruido ambiente
	 * */

	int i, j, long_filtros, cant_filtros;
	float min_SNR = 1000.0, SNR = 0.0, correc_v;
	float rms_grabacion[7];

	cant_filtros = sizeof(filtros) / sizeof(filtros[0]);
	long_filtros = sizeof(filtros[0]) / sizeof(filtros[0][0]);

	// Obtengo espectro de sweep grabado
	fft(left_ch, twiddles, SIGNAL_LENGHT);

	for (i = 0; i < cant_filtros; i++)
	{
		resetVectors(1, right_ch);

		for (j = 0; j < long_filtros; j++)
		{
			right_ch->samples[j].real = filtros[i][j];
		}

		fft(right_ch, twiddles, SIGNAL_LENGHT);
		filter(left_ch, right_ch);
		ifft(right_ch, twiddles, SIGNAL_LENGHT);

		rms_grabacion[i] = rmsValue(right_ch, SWEEP_SAMPLES);

		SNR = 20.0 * log10((double)rms_grabacion[i] / rms_ambiente[i]);

		if (SNR < min_SNR)
		{
			min_SNR = SNR;
		}
	}

	// Si la minima SNR es menor a 35dB lo corrijo
	if (min_SNR < 35.0)
	{
		*correc_db = 35.0 - min_SNR;
		if (*correc_db > 20.0)
		{
			*correc_db = 20.0;
		}

		correc_v = (float)pow(10.0, *correc_db/20.0);

		for (j = 0; j < sweep->muestras_utiles; j++)
		{
			sweep->samples[j].real *= correc_v;
		}
	}
}

void getImpulseResponse (
		vector *sweep,
		vector *left_ch,
		vector *right_ch,
		complex *twiddles
)
{
	/* Obtiene la respuesta al impulso del recinto bajo medicion. La misma es almacenada
	 * en los vectores left_ch y right_ch de forma estereo.
	 *
	 * Args:
	 * 		*sweep: Puntero al vector con la se�al de exitacion utilizada.
	 * 		*left_ch y *right_ch: Punteros a los vectores que contienen las se�ales grabadas en la medicion por la toma estereo.
	 * 		*twiddles: Puntero al vector con las constantes twiddle.
	 * */

	int i;
	float denominador, temp_left, temp_right;

	fft(sweep, twiddles, SIGNAL_LENGHT);
	fft(left_ch, twiddles, SIGNAL_LENGHT);
	fft(right_ch, twiddles, SIGNAL_LENGHT);

	for (i = 0; i < SIGNAL_LENGHT; i++)
	{
		denominador = sweep->samples[i].real * sweep->samples[i].real + sweep->samples[i].imag * sweep->samples[i].imag;
		temp_left = left_ch->samples[i].real;
		temp_right = right_ch->samples[i].real;

		left_ch->samples[i].real = (temp_left * sweep->samples[i].real + left_ch->samples[i].imag * sweep->samples[i].imag) / denominador;
		left_ch->samples[i].imag = (left_ch->samples[i].imag * sweep->samples[i].real - temp_left * sweep->samples[i].imag) / denominador;

		right_ch->samples[i].real = (temp_right * sweep->samples[i].real + right_ch->samples[i].imag * sweep->samples[i].imag) / denominador;
		right_ch->samples[i].imag = (right_ch->samples[i].imag * sweep->samples[i].real - temp_right * sweep->samples[i].imag) / denominador;
	}

	ifft(sweep, twiddles, SIGNAL_LENGHT);
	ifft(left_ch, twiddles, SIGNAL_LENGHT);
	ifft(right_ch, twiddles, SIGNAL_LENGHT);

	left_ch->muestras_utiles = SIGNAL_LENGHT - SWEEP_SAMPLES;
	right_ch->muestras_utiles = SIGNAL_LENGHT - SWEEP_SAMPLES;

	normalizeIR(left_ch, right_ch);
}
