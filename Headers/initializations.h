/*
 * Inicializaciones.h
 *
 *  Created on: 20/05/2018
 *      Author: JuaniPC
 */

#ifndef INICIALIZACIONES_H_
#define INICIALIZACIONES_H_

//extern DSK673_AIC23_CodecHandle hAIC23_handle;
extern TIMER_Handle Timer_Handler;

extern unsigned int CodecEventId, TimerEventId, SPIEventId;

void codecOpen(void);
void codecClose(void);
void sdOpen(void);
void sdClose(void);
void timerInit(void);
void timerInterrupInit(void);
void codecInterruptInit(void);

void DSK6713_LED_init(void);
void DSK6713_DIP_init(void);
void DSK6713_init(void);
void DSK6713_LED_toggle(Uint32);

#endif /* INICIALIZACIONES_H_ */
