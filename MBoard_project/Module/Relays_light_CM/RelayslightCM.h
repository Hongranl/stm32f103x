/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    ???????
 * @Version:    V 0.2 
 * @Module:     RelayslightCM
 * @Author:     RanHongLiang
 * @Date:       2019-07-10 15:37:20
 * @Description: 
 * ————????
 * ---------------------------------------------------------------------------*/

#ifndef RELAYSLIGHT_CM_H
#define RELAYSLIGHT_CM_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"
#include "Tips.h"

#define key1_vlue PBin(9)
#define key2_vlue PBin(10)

#define light1 PBout(12)
#define light2 PBout(13)
#define outlet1_led PBout(13)




typedef struct{

	uint8 relay_con;	//bit0 bit1 ÓÐÐ§
}RelayslightCM_MEAS;


extern osThreadId 	 tid_RelayslightCM_Thread;
extern osPoolId  	 RelayslightCM_pool;
extern osMessageQId  MsgBox_RelayslightCM;
extern osMessageQId  MsgBox_MTRelayslightCM;
extern osMessageQId  MsgBox_DPRelayslightCM;

void RelayslightCM_Init(void);
void RelayslightCM_Thread(const void *argument);
void RelayslightCMThread_Active(void);

#endif

