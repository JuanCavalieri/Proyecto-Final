/*
 * Timer_Config.h
 *
 *  Created on: 22/05/2018
 *      Author: JuaniPC
 */

#ifndef TIMER_CONFIG_H_
#define TIMER_CONFIG_H_

TIMER_Config Timer_Config = {
		TIMER_FMKS(CTL, INVINP, DEFAULT)      |
		TIMER_FMKS(CTL, CLKSRC, CPUOVR4)      |
		TIMER_FMKS(CTL, CP, PULSE)            |
		TIMER_FMKS(CTL, HLD, YES)             |
		TIMER_FMKS(CTL, GO, YES)              |
		TIMER_FMKS(CTL, PWID, TWO)            |
		TIMER_FMKS(CTL, DATIN, 0)             |
		TIMER_FMKS(CTL, DATOUT, 0)            |
		TIMER_FMKS(CTL, INVOUT, NO)           |
		TIMER_FMKS(CTL, FUNC, GPIO),

		TIMER_PRD_PRD_OF(45), //Cuentas necesarias para que cuente 1 microsegundo a 90Mhz/2

		TIMER_CNT_CNT_OF(0)
};


#endif /* TIMER_CONFIG_H_ */
