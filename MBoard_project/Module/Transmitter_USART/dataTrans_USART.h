/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     dataTrans_USART
 * @Author:     RanHongLiang
 * @Date:       2019-07-12 16:27:15
 * @Description: 
 * ————无线传输模块
 * ---------------------------------------------------------------------------*/

#ifndef DATATRANS_USART_H
#define DATATRANS_USART_H

#define osObjectsPublic                     // define objects in main module

#pragma pack(1)
typedef  struct
{   
	unsigned short   dest_dev_num;     //目标设备号
	unsigned char    dest_addr[4];     //目标地址
	unsigned short   source_dev_num;   //源设备号
	unsigned char    source_addr[4];   //源地址 
	unsigned char    TYPE_NUM ;        //数据格式号
	unsigned char    data_len ;        //数据长度	 
}
TRAN_D_struct;    //在头文件前



#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions
#include <dataTrans_USART.h>
#include "Driver_USART.h"
#include "string.h"

#include <Key&Tips.h>
#include "Moudle_DEC.h"

#include "Eguard.h"
#include "fireMS.h"
#include "gasMS.h"
#include "lightMS.h"
#include "pyroMS.h"
#include "analogMS.h"
#include "smokeMS.h"
#include "tempMS.h"
#include "shakeMS.h"
#include "Tips.h"


#include "infraTrans.h"

#define  WIRLESS_THREAD_EN	123

#define  datsTransCMD_UPLOAD	0x10
#define  datsTransCMD_DOWNLOAD	0x20



#define	 ABNORMAL_DAT	0xfe

extern uint8 gb_databuff[50];

extern volatile bool gb_Exmod_key;

extern osThreadId tid_USARTWireless_Thread;
	
void USART2Wireless_wifiESP8266Init(void);

void myUSART2_callback(uint32_t event);
void USART2Wirless_Init(void);
void USART_WirelessInit(void);
void USARTWireless_Thread(const void *argument);
void UsartRx_Thread(const void *argument);	

void wirelessThread_Active(uint8);

#endif
