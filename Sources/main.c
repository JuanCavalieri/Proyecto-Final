#include "DSK6713_AIC23.h"	//codec-DSK support file
#include "Funciones.h"
#include "Inicializaciones.h"


volatile unsigned int cuentas = 0;
int j = 0;


void main(){

	IRQ_globalDisable(); //Deshabilita todas las interrupciones
	DSK6713_init(); //Inicializa DSK-EMIF,PLL
	Timer_init(); //Inicializa el Timer
	Interrup_init(); //Inicializa las interrupciones de


	while(1) {}
}

interrupt void c_int11(){         //interrupt service routine
    Output_data(sweep_signal[j]);
    if(j == 240000){
    	j = 0;
    }else{
    	j++;
    }
}

interrupt void c_int10(){ //Interrumpe cada 1 micro segundo
	IRQ_clear(TimerEventId);
	if(cuentas != 0)
		cuentas--;
}
