/*
 * Runtime_errors.h
 *
 *  Created on: 25/05/2019
 *      Author: Juani
 */

#ifndef RUNTIME_ERRORS_H_
#define RUNTIME_ERRORS_H_

/* --------- Funciones implementadas --------- */

void checkError (int state)
{
	if (state == 1)
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

void recording (int state)
{
	if (state == 1)
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

void ledsInMode (int mode)
{
	DSK6713_LED_off(0);
	DSK6713_LED_off(1);
	DSK6713_LED_off(2);
	DSK6713_LED_off(3);

	if (mode)
	{
		DSK6713_LED_on(0);
	}
	if (mode == 2)
	{
		DSK6713_LED_on(1);
	}
	if (mode == 3)
	{
		DSK6713_LED_on(2);
	}
	if (mode == 4)
	{
		DSK6713_LED_on(3);
	}
}

#endif /* RUNTIME_ERRORS_H_ */
