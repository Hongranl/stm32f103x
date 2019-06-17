#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

#define LED0 	PBout(10)	
#define BEEP_ON() 	PAout(0)=0	
#define BEEP_OFF() 	PAout(0)=1	


void LED_Init(void);//≥ı ºªØ

#endif 
