#ifndef RELAYSOUTLET_CM_H
#define RELAYSOUTLET_CM_H

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

#define key_vlue PBin(9)


#define outlet1 PBout(12)
#define outlet1_led PBout(13)





typedef struct{
	uint8 Current;
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

