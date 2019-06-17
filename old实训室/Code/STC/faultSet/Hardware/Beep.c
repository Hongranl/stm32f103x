#include "Beep.h"

void GPIO_BEEP_Init(void){

	GPIO_InitTypeDef gpio_BEEP;
	
	gpio_BEEP.Mode = GPIO_OUT_PP;
	gpio_BEEP.Pin  = GPIO_Pin_7;
	
	GPIO_Inilize(GPIO_P1,&gpio_BEEP);
	
	BEEP = 0;
}

void Beep_time(u8 timea,u8 timeb){

	BEEP = 1;
	delay_ms(timea);
	BEEP = 0;
	delay_ms(timeb);
}