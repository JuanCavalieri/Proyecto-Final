/*
 * Runtime_errors.h
 *
 *  Created on: 25/05/2019
 *      Author: Juani
 */

#ifndef RUNTIME_ERRORS_H_
#define RUNTIME_ERRORS_H_

/* --------- Funciones implementadas --------- */

void check_error (int estado)
{
	if (estado == 1)
	{
		DSK6713_LED_on(0);
		DSK6713_LED_on(1);
		DSK6713_LED_on(2);
		DSK6713_LED_on(3);
	}
	else
	{
		DSK6713_LED_off(0);
		DSK6713_LED_off(1);
		DSK6713_LED_off(2);
		DSK6713_LED_off(3);
	}
}

void leds_grabando (int estado)
{
	if (estado == 1)
	{
		DSK6713_LED_on(0);
		DSK6713_LED_on(1);
		DSK6713_LED_on(2);
		DSK6713_LED_on(3);
	}
	else
	{
		DSK6713_LED_off(0);
		DSK6713_LED_off(1);
		DSK6713_LED_off(2);
		DSK6713_LED_off(3);
	}
}

void leds_modo (int modo)
{
	DSK6713_LED_off(0);
	DSK6713_LED_off(1);
	DSK6713_LED_off(2);
	DSK6713_LED_off(3);

	if (modo == 1)
	{
		DSK6713_LED_on(0);
	}
	if (modo == 2)
	{
		DSK6713_LED_on(1);
	}
	if (modo == 3)
	{
		DSK6713_LED_on(2);
	}
	if (modo == 4)
	{
		DSK6713_LED_on(3);
	}
}

#endif /* RUNTIME_ERRORS_H_ */
