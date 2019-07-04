/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     pwmCM
 * @Author:     RanHongLiang
 * @Date:       2019-07-04 15:58:52
 * @Description: 
 *————pwm定义以及部分声明
 *---------------------------------------------------------------------------*/

#ifndef __PWM_CM_H__
#define __PWM_CM_H__

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stmflash.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#include "debugUart.h"

#define	 EC11_SW		PBin(14)

#define  KNOB_A		PBin(12)
#define  KNOB_B		PBin(13)

#define  pwr_LED		PBout(6)


#define  pwmDevMID_unvarLight	0x0A
#define  pwmDevMID_varLight		0x0B
#define  pwmDevMID_unvarFan		0x0C

#define  SPECMD_pwmDevModADDR_CHG	0x0A
#define  SPECMD_pwmDevDATS_CHG		0x0B

extern uint8_t pwmDevMOUDLE_ID;

typedef struct{
	
	u8 speDPCMD;
	
	uint8_t Mod_addr;
	
	bool Switch;
	u8	 pwmVAL;
}pwmCM_MEAS;

typedef struct{

	uint8_t mADDR;
}pwmCM_kMSG;

extern pwmCM_MEAS lightAttr;

extern osThreadId 	 tid_pwmCM_Thread;
extern osPoolId  	 pwmCM_pool;
extern osMessageQId  MsgBox_pwmCM;
extern osMessageQId  MsgBox_MTpwmCM;
extern osMessageQId  MsgBox_DPpwmCM;

extern osPoolId  	 memID_pwmCMsigK_pool;	
extern osMessageQId  MsgBox_pwmCMsigK;

void pwmCM_Init(void);
void pwmThread(void const *argument);
void DC11detectA_Thread(const void *argument);
void DC11detectB_Thread(const void *argument);
void pwmCM_Thread(const void *argument);
void pwmCM_Terminate(void);
void pwmCMThread_Active(void);

#endif

