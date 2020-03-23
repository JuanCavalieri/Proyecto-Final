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

void codec_open(void);
void codec_close(void);
void sd_open(void);
void sd_close(void);
void timer_init(void);
void timer_interrup_init(void);
void codec_interrupt_init(void);

void DSK6713_LED_init(void);
void DSK6713_DIP_init(void);
void DSK6713_init(void);
void DSK6713_LED_toggle(Uint32);

#endif /* INICIALIZACIONES_H_ */
