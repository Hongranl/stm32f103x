#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
#include "global.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 

/*********宏定义**********/
#define KEY_1_PERIPH			RCC_APB2Periph_GPIOB
#define KEY_1_PORT				GPIOB
#define KEY_1_PIN				GPIO_Pin_0
#define KEY_1_PINNUM			(0)
#define KEY_1_VALUE()			(PBin(0))

#define KEY_2_PERIPH			RCC_APB2Periph_GPIOB
#define KEY_2_PORT				GPIOB
#define KEY_2_PIN				GPIO_Pin_1
#define KEY_2_PINNUM			(1)
#define KEY_2_VALUE()			(PBin(1))

#define KEY_VALUE_TRUE			(0)
#define KEY_VALUE_FALSE			(1)

#define BEEP_PERIPH				RCC_APB2Periph_GPIOB
#define BEEP_PORT				GPIOB
#define BEEP_PIN				GPIO_Pin_5
#define BEEP_PINNUM				(5)
#define BEEP_VALUE()			(PBout(5))

#define TYPE_1_PERIPH			RCC_APB2Periph_GPIOC
#define TYPE_1_PORT				GPIOC
#define TYPE_1_PIN				GPIO_Pin_0
#define TYPE_1_PINNUM			(0)
#define TYPE_1_VALUE			(PCin(0))

#define TYPE_2_PERIPH			RCC_APB2Periph_GPIOC
#define TYPE_2_PORT				GPIOC
#define TYPE_2_PIN				GPIO_Pin_1
#define TYPE_2_PINNUM			(1)
#define TYPE_2_VALUE			(PCin(1))

#define TYPE_4_PERIPH			RCC_APB2Periph_GPIOC
#define TYPE_4_PORT				GPIOC
#define TYPE_4_PIN				GPIO_Pin_2
#define TYPE_4_PINNUM			(2)
#define TYPE_4_VALUE			(PCin(2))

#define TYPE_8_PERIPH			RCC_APB2Periph_GPIOC
#define TYPE_8_PORT				GPIOC
#define TYPE_8_PIN				GPIO_Pin_3
#define TYPE_8_PINNUM			(3)
#define TYPE_8_VALUE			(PCin(3))

#define FAN_PWM_PERIPH			RCC_APB2Periph_GPIOA
#define FAN_PWM_PORT			GPIOA
#define FAN_PWM_PIN				GPIO_Pin_4
#define FAN_PWM_PINNUM			(4)
#define FAN_PWM_VALUE			(!PAout(4))
#define FAN_PWM_OFF			(PAout(4) = 1);
#define FAN_PWM_ON			(PAout(4) = 0);

/*************************/

/********函数声明*********/	
void Key_Init(void);
void dly_Init(void);
bool Key1Scan(void);
bool Key2Scan(void);
void Beep_Init(void);
void BeepOn(u16 Cnt);
void CodeSwtich_Init(void);
u8 GetSwitchType(void);
void Fan_Init(u16 speed);
void FanPowerOn(u16 speed);
void ChangeFanSpeed(u16 speed);
void FanPowerOff(void);
/*************************/

#endif
