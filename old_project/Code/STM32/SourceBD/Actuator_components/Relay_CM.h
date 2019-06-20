#ifndef _RELAY_CM_H_
#define _RELAY_CM_H_

#include "stm32f10x.h"

#define Relay_OFF PAout(12) = 0;//继电器关
#define Relay_ON  PAout(12) = 1;//继电器开

#define Blue_LED_OFF PBout(9) = 1; //开合检测指示关
#define Blue_LED_ON  PBout(9) = 0; //开合检测指示开

void Relay_Init(void);
 
#endif

