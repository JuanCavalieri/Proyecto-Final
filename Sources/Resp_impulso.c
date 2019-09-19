/*
 * Resp_impulso.c
 *
 *  Created on: 04/01/2019
 *      Author: Juani
 */
#include <stdarg.h>
#include "math.h"
#include "Resp_impulso.h"
#include "filtros.h"

#define FS 44100.0 				// Frecuencia de muestreo de las señales de audio [Hz]
#define DURACION_SWEEP 5.0 		// Tiempo de duracion de la señal de exitacion [s]
#define FSTART 20.0 			// Frecuencia inicial de la señal generada [Hz]
#define FEND 22000.0 			// Frecuencia final de la señal generada [Hz]
#define DURACION_FADEIN 0.379 	// Tiempo de duracion del fadein de la señal generada [s]
#define DURACION_FADEOUT 0.2 	// Tiempo de duracion del fadeout de la señal generada [s]

#define MUESTRAS_SWEEP FS*DURACION_SWEEP	// Cantidad de muestras que tiene la señal generada

/* Se declaran en el main.c para evitar conflictos con
 * la sentencia #pragma DATA_SECTION(). Sino estas variables deberian
 * ser parte de la libreria creada para este proyecto
 *
Vector sweep, left_ch, right_ch;
Vector *sweep_ptr = &sweep;
Vector *left_ch_ptr = &left_ch;
Vector *right_ch_ptr = &right_ch;
Complex twiddles[MUESTRAS/2];
*/

void Vectores_reset(int cant, ... ){

	/* Permite resetear los valores de todas las componentes de los vectores de señal que se pasan como argumentos.
	 *
	 * Args:
	 * 		cant: Cantidad de vectores que se desean resetear
	 * 		El resto de parametros son los punteros a los vectores que se desean resetear
	 * */

	int i;
	Vector *vector;
	va_list pa;

	va_start(pa, cant);

	while(cant--){

		vector = va_arg(pa, Vector*);
	    for(i = 0; i < MUESTRAS; i++){

	    	vector->samples[i].real = 0.0;
	        vector->samples[i].imag = 0.0;
	    }
	}

	va_end(pa);
}

void Twiddle_init(Complex *twiddles){

	/* Inicializa las constantes Twiddle que se utilizan en las funciones fft y ifft.
	 *
	 * Args:
	 * 		*twiddles: Puntero al array de numeros complejos donde se almacenaran las constantes
	 * */

	int i;
	double temp;

	for(i = 0; i < MUESTRAS/2; i++){

		temp = 2*PI*i/MUESTRAS;
		twiddles[i].real = (float)cos(temp);
		twiddles[i].imag = (float)-sin(temp);
	}
}

void _normalize(Vector *signal){

	/* Aplica la normalizacion 1/N a los valores devueltos por la funcion de ifft.
	 *
	 * Args:
	 * 		*signal: Puntero al vector señal al que se le aplicara la normalizacion
	 * */

	int i;
	float norm;

	norm = 1.0/(float)MUESTRAS;

	for(i = 0;i < MUESTRAS; i++){

		signal->samples[i].real *= norm;
		signal->samples[i].imag *= norm;
	}
}

void _bit_reversal(Vector *signal, unsigned int lenght){

	/* Algoritmo de bit reversal requerido en las funciones de fft y ifft.
	 *
	 * Args:
	 * 		*signal: Puntero al vector señal al que se le aplicara la reversion.
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

    for(i=quartn; i; i--){

		nodd = ~i;
		for(zeros=0; nodd&1; zeros++) nodd >>= 1;
		forward ^= 2 << zeros;
		rev ^= quartn >> zeros;

			if(forward<rev){
				_swap(forward, rev, signal);
				nodd = nmin1 ^ forward;
				noddrev = nmin1 ^ rev;
				_swap(nodd, noddrev, signal);
			}

			nodd = forward ^ 1;
			noddrev = rev ^ halfn;
			_swap(nodd, noddrev, signal);
    }
}

static inline void _swap(unsigned int forward, unsigned int rev, Vector *signal){

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

void _fft(Vector *signal, Complex *twiddles, int lenght){

	/* Realiza la fft (Fast Fourier Transform) de longitud length sobre la señal en signal en dominio tiempo.
	 * Almacena el resultado, en dominio frecuencial, en el mismo vector signal. La funcion presupone que el
	 * vector signal tiene longitud length y que si la señal util es mas corta que el propio vector signal,
	 * el resto del mismo vale 0.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la señal a transformar.
	 * 		*twiddles: Puntero al vector con las constantes twiddle.
	 * 		length: Longitud de la transformada.
	 * */

	unsigned int even, odd, span, log, rootindex;
	float temp;
	log=0;

	for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){

            odd |= span;
            even = odd ^ span;

            temp = signal->samples[even].real + signal->samples[odd].real;
            signal->samples[odd].real = signal->samples[even].real - signal->samples[odd].real;
            signal->samples[even].real = temp;

            temp = signal->samples[even].imag + signal->samples[odd].imag;
            signal->samples[odd].imag = signal->samples[even].imag - signal->samples[odd].imag;
            signal->samples[even].imag = temp;

            rootindex = (even<<log) & (lenght-1);
            if(rootindex){

                temp = twiddles[rootindex].real * signal->samples[odd].real - twiddles[rootindex].imag * signal->samples[odd].imag;
                signal->samples[odd].imag = twiddles[rootindex].real * signal->samples[odd].imag + twiddles[rootindex].imag * signal->samples[odd].real;
                signal->samples[odd].real = temp;
            }
        }
     }

    _bit_reversal(signal, lenght);
}

void _ifft(Vector *signal, Complex *twiddles, int lenght){

	/* Realiza la ifft (Inverse Fast Fourier Transform) de longitud length sobre la señal en signal en dominio frecuencial.
	 * Almacena el resultado, en dominio tiempo, en el mismo vector signal. La funcion presupone que el
	 * vector signal tiene longitud length.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la señal a antitransformar.
	 * 		*twiddles: Puntero al vector con las constantes twiddle.
	 * 		length: Longitud de la transformada.
	 * */

	unsigned int even, odd, span, log, rootindex;
    float temp;
    log=0;

    for(span=lenght>>1; span; span>>=1, log++){

        for(odd=span; odd<lenght; odd++){

            odd |= span;
            even = odd ^ span;

            temp = signal->samples[even].real + signal->samples[odd].real;
            signal->samples[odd].real = signal->samples[even].real - signal->samples[odd].real;
            signal->samples[even].real = temp;

            temp = signal->samples[even].imag + signal->samples[odd].imag;
            signal->samples[odd].imag = signal->samples[even].imag - signal->samples[odd].imag;
            signal->samples[even].imag = temp;

            rootindex = (even<<log) & (lenght-1);
            if(rootindex){

                temp = twiddles[rootindex].real * signal->samples[odd].real - (-twiddles[rootindex].imag) * signal->samples[odd].imag;
                signal->samples[odd].imag = twiddles[rootindex].real * signal->samples[odd].imag + (-twiddles[rootindex].imag) * signal->samples[odd].real;
                signal->samples[odd].real = temp;
            }
        }
     }

    _bit_reversal(signal, lenght);
    _normalize(signal);
}

void Generate_sweep(Vector *signal){

	/* Genera una señal senoidal de frecuencia variable que comienza en FSTART y termina en FEND,
	 * con una duracion igual a DURACION_SWEEP que se almacena en el vector signal segun la formula
	 * presente en [], referenciado en la bibliografia del informe del presente trabajo. Ademas, se
	 * le aplica a la misma un efecto de fade in de duracion DURACION_FADEIN y otro de fade out de
	 * duracion DURACION_FADEOUT.
	 *
	 * Args:
	 * 		*signal: Puntero al vector donde almacenar la señal generada.
	 * */

	int i;
	double wstart = 2*PI*FSTART;
	double wend = 2*PI*FEND;
	double t = 0;
	double temp1 = log(wend/wstart);
	double temp2 = wstart*DURACION_SWEEP/temp1;
	double alpha_fadein = -log(0.001)/(FS*DURACION_FADEIN);
	double alpha_fadeout = -log(0.001)/(FS*DURACION_FADEOUT);

	for(i = 0; i < MUESTRAS_SWEEP; i++){

		t = (double)i/FS;
		signal->samples[i].real = (float)(0.5 * sin(temp2 * (exp(temp1*t/DURACION_SWEEP) - 1)));

		if(i < 16714)
			signal->samples[i].real *= (float)(1 - exp(-alpha_fadein * i));

		if(i > 211679)
			signal->samples[i].real *= (float)(1 - exp(alpha_fadeout * (i - (MUESTRAS_SWEEP - 1))));

	}
	signal->muestras_utiles = MUESTRAS_SWEEP - 1;
}

void _promediar(Vector *record_lft, Vector *record_rgt){

	/* Realiza un promedio entre las muestras de las señales grabadas en toma estereo.
	 * Almacena el resultado en el primer vector pasado como argumento.
	 *
	 * Args:
	 * 		*record_lft: Puntero al vector con la señal del canal izquierdo.
	 * 		*record_rgt: Puntero al vector con la señal del canal derecho.
	 * */

	int i;

	for(i = 0; i < record_lft->muestras_utiles; i++ ){
		record_lft->samples[i].real = (record_lft->samples[i].real + record_rgt->samples[i].real) / 2.0;
	}
}

void _filtrar(Vector *signal, Vector *filtro){

	/* Realiza el filtrado entre la señal en signal y el filtro. El mismo se realiza en
	 * dominio frecuencial, por lo que ambos vectores deben contener los espectros de las
	 * señales correspondientes.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la señal a filtrar.
	 * 		*filtro: Puntero al vector con el filtro a aplicar.
	 * */

	int i;
	float temp;

	for(i = 0; i < signal->muestras_utiles; i++){

		temp = filtro->samples[i].real;
		filtro->samples[i].real = signal->samples[i].real*filtro->samples[i].real - signal->samples[i].imag*filtro->samples[i].imag;
		filtro->samples[i].imag = signal->samples[i].real*filtro->samples[i].imag + signal->samples[i].imag*temp;
	}
}

float _valor_rms(Vector *signal, int length){

	/* Calcula el valor rms de la señal en el vector signal. El valor length indica hasta
	 * que componente del vector se toma la señal para calcular su valor rms.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la señal a la cual calcular el valor rms.
	 * 		length: Longitud que abarca la señal util en el vector signal.
	 * 	Return:
	 * 		float con el valor rms de la señal de entrada.
	 * */

	int i;
	float aux = 0.0;

	for(i = 0; i <= length; i++){

		aux += signal->samples[i].real*signal->samples[i].real;
	}

	aux = (float)sqrt(aux/(float)length);
	return aux;
}

void Medir_RmsAmbiente(Vector *left_ch, Vector *right_ch, Complex *twiddles, float *max_rms, int *num_filtro){

	int i, j, cant_filtros, long_filtros;
	float rms_ruido;

	_promediar(left_ch, right_ch);		// Promedio los canales estereo para trabajar en mono
	_fft(left_ch, twiddles, MUESTRAS);	// Hago la fft del ruido ambiente

	cant_filtros = sizeof(filtros) / sizeof(filtros[0]);
	long_filtros = sizeof(filtros[0]) / sizeof(filtros[0][0]);

	for(i = 0; i < cant_filtros; i++){

		Vectores_reset(1, right_ch);

		for(j = 0; j < long_filtros; j++){

			right_ch->samples[j].real = filtros[i][j];
		}

		_fft(right_ch, twiddles, MUESTRAS);
		_filtrar(left_ch, right_ch, MUESTRAS);
		_ifft(right_ch, twiddles, MUESTRAS);

		rms_ruido = _valor_rms(right_ch, MUESTRAS_SWEEP);
		if(rms_ruido > max_rms){
			max_rms = rms_ruido;
			num_filtro = i;
		}
	}
	Vectores_reset(2, left_ch, right_ch);
}

void Ajustar_Sweep(Vector *sweep, Vector *left_ch, Vector *right_ch, Complex *twiddles, float rms_ambiente, int num_filtro){


}

void

void Corregir_RespFrec(Vector *signal, Vector *record, Vector *aux, Complex *twiddles){

	/* Corrige la señal de exitacion utilizada (signal) en funcion de la respuesta en
	 * frecuencia del sistema de sonido utilizado para las mediciones. La nueva señal
	 * de exitacion corregida se almacena en el mismo vector signal.
	 *
	 * Args:
	 * 		*signal: Puntero al vector con la señal de exitacion sin corregir.
	 * 		*record: Puntero al vector con el canal izquierdo de la señal grabada en la medicion.
	 * 		*aux: Puntero al vector con el canal derecho de la señal grabada en la medicion.
	 * 		*twiddles: Puntero al vector con las constantes twiddles.
	 * */

	int i;
	double energy = 0;
	double tau_start = 0.05;
	double tau_end = DURACION_SWEEP;
	double tau_g = tau_start;
	double previous_tau_g;
	double phi = 0;
	double previous_phi;
	double C = 0;
	double temp = 2*PI*FS/MUESTRAS;

	_promediar(record, aux);

	// Obtengo el espectro del sweep y de la grabacion
	_fft(signal, twiddles, MUESTRAS);
	_fft(record, twiddles, MUESTRAS);

	// Obtengo el espectro del sweep a sintetizar (en veces) y calculo el valor de la energia para la constante C
	for(i = 0; i < MUESTRAS; i++){

		record->samples[i].real = (float)((double)(signal->samples[i].real*signal->samples[i].real + signal->samples[i].imag*signal->samples[i].imag)/sqrt((double)(record->samples[i].real*record->samples[i].real + record->samples[i].imag*record->samples[i].imag)));
		if(i < MUESTRAS/2)
			energy += (double)(record->samples[i].real*record->samples[i].real);
	}

	// Calculo el valor de C
	C = (tau_end - tau_start)/energy;

	// Calculo el valor del retardo de grupo, la fase y sintetizo el espectro del barrido deseado
	for(i = 0; i < MUESTRAS; i++){

		if(i == 0){
			signal->samples[i].real = record->samples[i].real * (float)cos(phi);
			signal->samples[i].imag = record->samples[i].real * (float)sin(phi);
		} else if(i < MUESTRAS/2){
			tau_g = previous_tau_g + C*(double)(record->samples[i].real*record->samples[i].real);
			phi = previous_phi - temp*tau_g;
			signal->samples[i].real = record->samples[i].real * (float)cos(phi);
			signal->samples[i].imag = record->samples[i].real * (float)sin(phi);
		} else {
		    signal->samples[i].real = record->samples[i].real * (float)cos(phi);
			signal->samples[i].imag = record->samples[i].real * (float)sin(phi);
			tau_g = previous_tau_g - C*(double)(record->samples[i].real*record->samples[i].real);
			phi = previous_phi + temp*tau_g;
		}

		previous_tau_g = tau_g;
		previous_phi = phi;
	}

	// Paso el barrido a dominio temporal. De este vector hay que reproducir unicamente las muestras que ocupen el sweep
	_ifft(signal, twiddles, MUESTRAS);

	// Ajusto el nivel del sweep
	for(i = 0; i < MUESTRAS; i++){
		if(i < FS*(tau_start + tau_end))
			signal->samples[i].real *= 2.0;
		else
			signal->samples[i].real = 0.0;
	}

	signal->muestras_utiles = FS*(tau_start  + tau_end);
}

void Obtener_RI(Vector *sweep, Vector *left_ch, Vector *right_ch, Complex *twiddles){

	/* Obtiene la respuesta al impulso del recinto bajo medicion. La misma es almacenada
	 * en los vectores left_ch y right_ch de forma estereo.
	 *
	 * Args:
	 * 		*sweep: Puntero al vector con la señal de exitacion utilizada.
	 * 		*left_ch y *right_ch: Punteros a los vectores que contienen las señales grabadas en la medicion por la toma estereo.
	 * 		*twiddles: Puntero al vector con las constantes twiddle.
	 * */

	int i;
	float denominador, tempLeft, tempRight;

	_fft(sweep, twiddles, MUESTRAS);
	_fft(left_ch, twiddles, MUESTRAS);
	_fft(right_ch, twiddles, MUESTRAS);

	for(i = 0; i < MUESTRAS; i++){

		denominador = sweep->samples[i].real * sweep->samples[i].real + sweep->samples[i].imag * sweep->samples[i].imag;
		tempLeft = left_ch->samples[i].real;
		tempRight = right_ch->samples[i].real;

		left_ch->samples[i].real = (tempLeft * sweep->samples[i].real + left_ch->samples[i].imag * sweep->samples[i].imag) / denominador;
		left_ch->samples[i].imag = (left_ch->samples[i].imag * sweep->samples[i].real - tempLeft * sweep->samples[i].imag) / denominador;

		right_ch->samples[i].real = (tempRight * sweep->samples[i].real + right_ch->samples[i].imag * sweep->samples[i].imag) / denominador;
		right_ch->samples[i].imag = (right_ch->samples[i].imag * sweep->samples[i].real - tempRight * sweep->samples[i].imag) / denominador;
	}

	_ifft(sweep, twiddles, MUESTRAS);
	_ifft(left_ch, twiddles, MUESTRAS);
	_ifft(right_ch, twiddles, MUESTRAS);
}
