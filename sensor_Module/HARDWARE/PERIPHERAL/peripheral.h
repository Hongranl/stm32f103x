/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		peripheral.c
描述   :      外设传感器驱动程序头文件
作者   :      hongxiang.liu
版本   :      V1.0
修改   :  
完成日期:		2019.3.25
************************************************************************************************************************/
#ifndef _PERIPHERAL_H_
#define _PERIPHERAL_H_

/*************HEADS**********/
#include "global.h"
#include "timer.h"
/***************************/

/**********宏定义***********/
//引脚定义
#define SR04_TRIG_PERIPH		RCC_APB2Periph_GPIOA
#define SR04_TRIG_PORT			GPIOA
#define SR04_TRIG_PIN			GPIO_Pin_12
#define SR04_TRIG_PINNUM		(12)
#define SR04_TRIG_VALUE()		(PAout(12))

#define SR04_ECHO_PERIPH		RCC_APB2Periph_GPIOA
#define SR04_ECHO_PORT			GPIOA
#define SR04_ECHO_PIN			GPIO_Pin_11
#define SR04_ECHO_PINNUM		(11)
#define SR04_ECHO_VALUE()		(PAin(11))

#define HX711_SCK_PERIPH		RCC_APB2Periph_GPIOB
#define HX711_SCK_PORT			GPIOB
#define HX711_SCK_PIN			GPIO_Pin_6
#define HX711_SCK_PINNUM		(6)
#define HX711_SCK_H()			(PBout(6)=1)
#define HX711_SCK_L()			(PBout(6)=0)

#define HX711_DAT_PERIPH		RCC_APB2Periph_GPIOB
#define HX711_DAT_PORT			GPIOB
#define HX711_DAT_PIN			GPIO_Pin_7
#define HX711_DAT_PINNUM		(7)
#define HX711_DAT_H()			(PBin(7)=1)
#define HX711_DAT_L()			(PBin(7)=0)
#define HX711_RD_SDA()			(PBin(7))

#define RELAY_PERIPH			RCC_APB2Periph_GPIOA
#define RELAY_PORT				GPIOA
#define RELAY_PIN				GPIO_Pin_15
#define RELAY_PINNUM			(15)
#define RELAY_On()				(PAout(15)=1)
#define RELAY_Off()				(PAout(15)=0)
/***************************/

/**********函数声明***********/
void HcSr04_Init(void);
void HcSr04Start(void);
void HcSr04_OFF(void);
u32 GetHcSr04Value(void);
void HX711_Init(void);
u32 Read_HX711(void);
void Relay_Init(void);

/*****************************/

#endif

