/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		driver_i2c.h
描述   :      I2C驱动程序头文件
作者   :      hongxiang.liu
版本   :      V1.0
修改   :  
完成日期:		2019.3.21
************************************************************************************************************************/
#ifndef _DRIVER_IIC_H_
#define _DRIVER_IIC_H_

/*************HEADS**********/
#include "global.h"
/***************************/

/**********宏定义***********/
//引脚定义
#define SHT1X_DAT_PERIPH			RCC_APB2Periph_GPIOB
#define SHT1X_DAT_PORT				GPIOB
#define SHT1X_DAT_PIN				GPIO_Pin_3
#define SHT1X_DAT_PINNUM			(3)

#define SHT1X_SCK_PERIPH			RCC_APB2Periph_GPIOB
#define SHT1X_SCK_PORT				GPIOB
#define SHT1X_SCK_PIN				GPIO_Pin_4
#define SHT1X_SCK_PINNUM			(4)

//IO方向
#define IIC_SDA_IN()	{GPIOB->CRL&=~(0x0F<<(3*2));GPIOB->CRL|=0x08<<(3*4);}	//PB3(上下拉)输入模式
#define IIC_SDA_OUT()	{GPIOB->CRL&=~(0x0F<<(3*2));GPIOB->CRL|=0x03<<(3*4);}	//PB3(推挽)输出模式

#define IIC_SCL_H()		(PBout(4)=1)
#define IIC_SCL_L()		(PBout(4)=0)

#define IIC_SDA_H()		(PBout(3)=1)
#define IIC_SDA_L()		(PBout(3)=0)

#define IIC_RD_SDA()	(PBin(3))

#define IIC_ACK			(0)
#define IIC_NACK		(1)
/***************************/

/**********函数声明***********/
void Iic_GpioConfig(void);
void IicStart(void);
void IicStop(void);
u8 IicWaitAck(void);
void IicAck(void);
void IicNAck(void);
void IicSendByte(u8 Data);
u8 IicReceiveByte(u8 Ack);
/*****************************/

#endif

