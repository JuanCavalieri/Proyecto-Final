/*
 * Inicializaciones.h
 *
 *  Created on: 20/05/2018
 *      Author: JuaniPC
 */

#ifndef INICIALIZACIONES_H_
#define INICIALIZACIONES_H_

extern DSK6713_AIC23_CodecHandle hAIC23_handle;
extern TIMER_Handle Timer_Handler;

extern unsigned int CodecEventId, TimerEventId, SPIEventId;

void Codec_open(void);
void Codec_close(void);
void SD_open(void);
void SD_close(void);
void Timer_init(void);
void Timer_interrup_init(void);
void Codec_interrupt_init(void);

void DSK6713_LED_init(void);
void DSK6713_DIP_init(void);
void DSK6713_init(void);
void DSK6713_LED_toggle(Uint32);

#endif /* INICIALIZACIONES_H_ */
