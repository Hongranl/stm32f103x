#ifndef __CX_H
#define __CX_H	 
#include "stm32f10x.h"

extern u16 speedOvertimer;
extern u16 curSpeed;
extern u8 Traffic_red;

void SpeedDet_Init(void);
void Tim2_Init(void);
void TIM3_Configuration(u16 arr,u16 psc);
void Tim4_Init(void);
void Traffic_PA1_Init(void);


void Tim5_Init(void);
void Ultra_PC0_Init(void);

void Ultra_Ranging(void);

#endif

