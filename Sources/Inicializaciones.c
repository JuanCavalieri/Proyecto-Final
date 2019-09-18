/*
 * Inicializaciones.c
 *
 *  Created on: 20/05/2018
 *      Author: JuaniPC
 */
#include "DSK6713_AIC23.h"
#include "dsk6713.h"
#include "Codec_Config.h"
#include "MCBSP1_to_Codec_Config.h"
#include "MCBSP0_to_SPI_Config.h"
#include "MCBSP1_to_GPIO_Config.h"
#include "Timer_Config.h"
#include "Inicializaciones.h"

DSK6713_AIC23_CodecHandle hAIC23_handle; 	//Handler para el Codec
MCBSP_Handle SPI_Handler; 					//Handler para la configuracion del MCBSP0 como SPI
MCBSP_Handle GPIO_Handler; 					//Handler para la configuracion del MCBSP1 como GPIO
TIMER_Handle Timer_Handler; 				//Handler para el Timer

Uint32 CodecEventId, TimerEventId, SPIEventId; //ID de eventos para las interrupciones

void Codec_init(void){
	DSK6713_rset(DSK6713_MISC, 0); //Modifico registro de CPLD para mandar el MCBSP al Codec

	hAIC23_handle = DSK6713_AIC23_openCodec(0, &Codec_Config); //Crea puntero al Codec

	DSK6713_AIC23_setFreq(hAIC23_handle, DSK6713_AIC23_FREQ_44KHZ);  //Setea frecuencia de muestreo
	DSK6713_AIC23_rset(hAIC23_handle, 0x0004, DSK6713_AIC23_INPUT_LINE); //Selecciona Line-in como entrada al Codec
	MCBSP_config(DSK6713_AIC23_DATAHANDLE,&MCBSP1_to_Codec_Config); //Configura el canal de datos al Codec

	MCBSP_start(DSK6713_AIC23_DATAHANDLE, MCBSP_XMIT_START | MCBSP_RCV_START |
		MCBSP_SRGR_START | MCBSP_SRGR_FRAMESYNC, 220); //Habilita la transmision de datos
}

void SD_init(void){
	DSK6713_rset(DSK6713_MISC, 3); //Modifico registro de CPLD para sacar el MCBSP por los perifericos

	SPI_Handler = MCBSP_open(MCBSP_DEV0, MCBSP_OPEN_RESET); //Abro el MCBSP0 para configurarlo como SPI
	GPIO_Handler = MCBSP_open(MCBSP_DEV1, MCBSP_OPEN_RESET); //Abro el MCBSP1 para configrarlo como GPIO

	//Configuracion para GPIO

	MCBSP_config(GPIO_Handler, &MCBSP1_to_GPIO_Config);

	//Configuracion para SPI
	//1ro) Setear XRST y RRST en registro PCR en 0 (ya esta hecho al abrir con el RESET)
	//2do) Configurar el resto de los registro manteniendo GRST (SPCR) en 0
	//3ro) Iniciar TODO, se puede usar la funcion MCBSP_start();

	MCBSP_config(SPI_Handler,&MCBSP0_to_SPI_Config);
	MCBSP_start(SPI_Handler, MCBSP_XMIT_START | MCBSP_RCV_START |
			MCBSP_SRGR_START, 220);
}

void Timer_init(void){
	Timer_Handler = TIMER_open(TIMER_DEV0,TIMER_OPEN_RESET); //Habilito el Timer0
	TIMER_config(Timer_Handler, &Timer_Config); //Configuro los registros del timer

	TIMER_start(Timer_Handler); //Arranca el Timer
	TIMER_pause(Timer_Handler); //Pause el Timer
	TIMER_setCount(Timer_Handler, 0); //Setea el contador en 0
}

void Interrup_init(void){
	CodecEventId = MCBSP_getXmtEventId(DSK6713_AIC23_codecdatahandle); //McBSP1 Xmit
	TimerEventId = TIMER_getEventId(Timer_Handler); //Timer

	IRQ_map(CodecEventId, 11); //Mapea el McBSP1 Xmit a INT11
	IRQ_map(TimerEventId, 10); //Mapea el Timer a INT10

	IRQ_reset(CodecEventId); //Deshabilita y limpia la interrupcion del codec INT 11
	IRQ_reset(TimerEventId); //Deshabilita y limpia la interrupcion del Timer INT10

	IRQ_globalEnable(); //Habilita todas las interrupciones
	IRQ_nmiEnable(); //Habilita las interrupciones NMI
}


