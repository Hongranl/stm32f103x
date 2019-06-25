#ifndef _BEEP_H__
#define _BEEP_H__

#include "config.h"
#include "GPIO.h"
#include "delay.h"

#define	BEEP	P17

void GPIO_BEEP_Init(void);
void Beep_time(u8 timea,u8 timeb);

#endif