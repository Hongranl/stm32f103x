#ifndef DELAY_H
#define DELAY_H

#define osObjectsPublic                     // define objects in main module

#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions

#define delay_ms 				osDelay

void bsp_delayinit(void);

void delay_us(uint32_t _ulDelayTime);

#endif
