/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     debugUart
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 21:55:03
 * @Description: 
 *	调试模块，可用于调试输出
 *	注：未使用
 *---------------------------------------------------------------------------*/
#ifndef DEBUG_USART_H
#define DEBUG_USART_H

#define osObjectsPublic                     // define objects in main module

#include "stm32f10x.h"
#include "osObjects.h"                      // RTOS object definitions
#include <dataTrans_USART.h>
#include "Driver_USART.h"
#include "string.h"

#include <Key&Tips.h>

#include "Eguard.h"

#define	USARTDEBUG_THREAD_EN	123

typedef void (*funDebug)(void);

void USART1Debug_Init(void);

extern osThreadId tid_USARTDebug_Thread;

void funDB_keyMB_ON(void);
void funDB_keyMB_OFF(void);
void funDB_keyIFR_ON(void);
void funDB_keyIFR_OFF(void);

void myUSART1_callback(uint32_t event);
void USART1Debug_Init(void);
void USART_debugInit(void);
void USARTDebug_Thread(const void *argument);

void debugThread_Active(void);

#endif



