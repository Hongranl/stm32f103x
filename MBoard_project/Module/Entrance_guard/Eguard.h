/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     Eguard
 * @Author:     RanHongLiang
 * @Date:       2019-06-27 09:03:43
 * @Description: 
 *————
 *---------------------------------------------------------------------------*/

#ifndef	_EGUARD_H_
#define	_EGUARD_H_

#include "fingerID.h"
#include "rfID.h"
#include "kBoard.h"
#include "doorLock.h"

#include "IO_Map.h"
#include "stm32f10x.h"

#include "Tips.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"
#include "delay.h"
#include <dataTrans_USART.h>
/****线程特定数据，结构体*****/
typedef struct{

	u8 CMD;
	u8 DAT;
	u8 rfidDAT[4];
	u8 Psd[9];
}EGUARD_MEAS;

extern osPoolId  	 EGUD_pool;	
extern osMessageQId  MsgBox_EGUD;							 
extern osMessageQId  MsgBox_MTEGUD_FID;
extern osMessageQId  MsgBox_MTEGUD_DLOCK;
extern osMessageQId  MsgBox_DPEGUD;

void Eguard_Active(void);
void Eguard_Terminate(void);

#endif

