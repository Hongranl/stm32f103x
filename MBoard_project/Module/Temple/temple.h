/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     temple
 * @Author:     RanHongLiang
 * @Date:       2019-07-12 15:41:35
 * @Description: 
 * ————温度测量模块
 * ---------------------------------------------------------------------------*/

#ifndef FIRE_MS_H
#define FIRE_MS_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

typedef struct{

	bool VAL;
}templeMS_MEAS;

extern osThreadId 	 tid_templeMS_Thread;
extern osPoolId  	 templeMS_pool;
extern osMessageQId  MsgBox_templeMS;
extern osMessageQId  MsgBox_MTtempleMS;
extern osMessageQId  MsgBox_DPtempleMS;

void templeMS_Init(void);
void templeMS_Thread(const void *argument);
void templeMSThread_Active(void);

#endif

