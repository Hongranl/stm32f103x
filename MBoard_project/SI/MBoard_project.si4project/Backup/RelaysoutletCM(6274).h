#ifndef RELAYSoutlet_CM_H
#define RELAYSoutlet_CM_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

#define key1_vlue PBin(9)
#define key2_vlue PBin(10)

#define outlet1 PBout(12)
#define outlet2 PBout(13)




typedef struct{

	u8 relay_con;	//bit0 bit1 ÓÐÐ§
}RelaysoutletCM_MEAS;


extern osThreadId 	 tid_RelaysoutletCM_Thread;
extern osPoolId  	 RelaysoutletCM_pool;
extern osMessageQId  MsgBox_RelaysoutletCM;
extern osMessageQId  MsgBox_MTRelaysoutletCM;
extern osMessageQId  MsgBox_DPRelaysoutletCM;

void RelaysoutletCM_Init(void);
void RelaysoutletCM_Thread(const void *argument);
void RelaysoutletCMThread_Active(void);

#endif

