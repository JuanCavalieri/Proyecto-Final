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

void MCBSP_Codec_init(void);
void MCBSP_SPI_init(void);
void Timer_init(void);
void Interrup_init(void);

void DSK6713_LED_init(void);
void DSK6713_DIP_init(void);
void DSK6713_init(void);
void DSK6713_LED_toggle(Uint32);

#endif /* INICIALIZACIONES_H_ */
