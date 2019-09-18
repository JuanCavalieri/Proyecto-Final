/*
 * Runtime_errors.h
 *
 *  Created on: 25/05/2019
 *      Author: Juani
 */

#ifndef RUNTIME_ERRORS_H_
#define RUNTIME_ERRORS_H_

/* --------- Funciones implementadas --------- */

void check_error(int estado){

	DSK6713_LED_off(0);
	DSK6713_LED_off(1);
	DSK6713_LED_off(2);
	DSK6713_LED_off(3);

	// estado = 0 -> no hay errores
	if(estado == 0) return;

	// estado = 1 -> no existe archivo sweep_corregido.wav en la SD
	if(estado == 1){
		DSK6713_LED_on(0);
		DSK6713_LED_off(1);
		DSK6713_LED_off(2);
		DSK6713_LED_off(3);
	}

	// estado = 2 -> hubo algun problema abriendo o leyendo el archivo sweep_corregido.wav desde la SD
	if(estado == 2){
		DSK6713_LED_off(0);
		DSK6713_LED_on(1);
		DSK6713_LED_off(2);
		DSK6713_LED_off(3);
		while(1){};
	}

	// estado = 3 -> hubo algun problema guardando el archivo sweep_corregido.wav en la SD
	if(estado == 3){
		DSK6713_LED_off(0);
		DSK6713_LED_off(1);
		DSK6713_LED_on(2);
		DSK6713_LED_off(3);
		while(1){};
	}
}

#endif /* RUNTIME_ERRORS_H_ */
