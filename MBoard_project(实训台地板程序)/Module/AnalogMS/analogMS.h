/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     analogMS
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 21:53:15
 * @Description: 
 *————
 *---------------------------------------------------------------------------*/

#ifndef ANALOG_MS_H
#define ANALOG_MS_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

/****线程特定数据，结构体*****/
typedef struct{				

	uint16_t Ich1;
	uint16_t Ich2;
	uint16_t Vch1;
	uint16_t Vch2;
}analogMS_MEAS;

extern osThreadId 	 tid_analogMS_Thread;
extern osPoolId  	 analogMS_pool;
extern osMessageQId  MsgBox_analogMS;
extern osMessageQId  MsgBox_MTanalogMS;
extern osMessageQId  MsgBox_DPanalogMS;

void analogMS_Init(void);
void analogMS_Thread(const void *argument);
void analogMSThread_Active(void);

#endif

