/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     shakeMS
 * @Author:     RanHongLiang
 * @Date:       2019-07-12 10:19:01
 * @Description: 
 * ————震动传感器模块
 * ---------------------------------------------------------------------------*/

#ifndef SHAKE_MS_H
#define SHAKE_MS_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

#define shake_DATA	PBin(8);

typedef struct{

	bool VAL;
}shakeMS_MEAS;

extern osThreadId 	 tid_shakeMS_Thread;
extern osPoolId  	 shakeMS_pool;
extern osMessageQId  MsgBox_shakeMS;
extern osMessageQId  MsgBox_MTshakeMS;
extern osMessageQId  MsgBox_DPshakeMS;

void shakeMS_Init(void);
void shakeMS_Thread(const void *argument);
void shakeMSThread_Active(void);

#endif

