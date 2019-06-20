/*************************************************************
                             \(^o^)/
  Copyright (C), 2013-2020, ZheJiang University of Technology
  File name  : SHT10.h 
  Author     : ziye334    
  Version    : V1.0 
  Data       : 2014/3/10      
  Description: Digital temperature and humidity sensor driver code
  
*************************************************************/
#ifndef __SHT1X_H__
#define __SHT1X_H__
#include "global.h"

enum {TEMP, HUMI};

/* GPIO相关宏定义 */
#define SHT10_AHB2_CLK        RCC_APB2Periph_GPIOB
#define SHT10_DATA_PIN        GPIO_Pin_3
#define SHT10_SCK_PIN        GPIO_Pin_4
#define SHT10_DATA_PORT        GPIOB
#define SHT10_SCK_PORT        GPIOB

#define SHT10_DATA_H()        GPIO_SetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)                         //拉高DATA数据线
#define SHT10_DATA_L()        GPIO_ResetBits(SHT10_DATA_PORT, SHT10_DATA_PIN)                         //拉低DATA数据线
#define SHT10_DATA_R()        GPIO_ReadInputDataBit(SHT10_DATA_PORT, SHT10_DATA_PIN)         //读DATA数据线

#define SHT10_SCK_H()        GPIO_SetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)                                 //拉高SCK时钟线
#define SHT10_SCK_L()        GPIO_ResetBits(SHT10_SCK_PORT, SHT10_SCK_PIN)                         //拉低SCK时钟线

/* 传感器相关宏定义 */
#define        noACK        0
#define ACK                1
                                                                //addr  command         r/w
#define STATUS_REG_W        0x06        //000         0011          0          写状态寄存器
#define STATUS_REG_R        0x07        //000         0011          1          读状态寄存器
#define MEASURE_TEMP         0x03        //000         0001          1          测量温度
#define MEASURE_HUMI         0x05        //000         0010          1          测量湿度
#define SOFTRESET       0x1E        //000         1111          0          复位


void SHT1X_Config(void);
u8 SHT1X_GetValue(void);
void SHT10_ConReset(void);
u8 SHT10_SoftReset(void);
u8 SHT10_Measure(u16 *p_value, u8 *p_checksum, u8 mode);
void SHT10_Calculate(u16 t, u16 rh,float *p_temperature, float *p_humidity);
float SHT10_CalcuDewPoint(float t, float h);


#endif



///***********************************************************************************************************************
//Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
//文件名 :		SHT1X.h
//描述   :      SHT1X温湿度传感器模块程序头文件
//作者   :      hongxiang.liu
//版本   :      V1.0
//修改   :  
//完成日期:		2019.3.21
//************************************************************************************************************************/
//#ifndef _SHT1X_H_
//#define _SHT1X_H_

///*************HEADS**********/
//#include "global.h"
//#include "sys.h"
//#include "delay.h"
//#include <math.h>
///***************************/

///***********宏定义************/
//#define SCL GPIO_Pin_4
//#define SDA GPIO_Pin_3

//#define noACK 0             //??????
//#define ACK   1             //???????
//                            //adr  command  r/w 
//#define STATUS_REG_W 0x06   //000   0011    0 
//#define STATUS_REG_R 0x07   //000   0011    1 
//#define MEASURE_TEMP 0x03   //000   0001    1 
//#define MEASURE_HUMI 0x05   //000   0010    1 
//#define RESET        0x1e   //000   1111    0 

//#define SDA_IN()  {GPIOB->CRL&=0XFFFF0FFF;GPIOC->CRL|=4<<12;} //MODE3的15:12位值位10,对应的模式为输出模式
//#define SDA_OUT() {GPIOB->CRL&=0XFFFF0FFF;GPIOC->CRL|=3<<12;} //MODE3的15:12位值位00,对应的模式为输入模式

///*****************************/

///**********函数声明***********/
//void SHT1x_Init(void);
//void SHT1x_Config(void);
//void SHT1x_GetValue(void);
//void SDA_SET_OUT(void);
//void SDA_SET_IN(void);
//void SCL_Pin_OUT(u8 out);
//void SDA_Pin_OUT(u8 out);
//void SHT1x_Start(void);
//void SHT1x_Reset(void);
//char SHT1X_softreset(void);
//char SHT1X_write_statusreg(unsigned char value);
//char SHT1X_read_statusreg(unsigned char *p_value);
//u8 SHT1x_Measure(u16 *p_value, u8 *p_checksum, u8 Mode);
//u8 SHT1x_Write_Byte(u8 value);
//u8 SHT1x_Read_Byte(u8 dat);
//void SHT1X_Caculation(float *p_temperature,float *p_humidity );
//void SHT1X_Caculation1(float *p_temperature,float *p_humidity );
//float SHT1X_dewpoint(float h,float t);
//float SHT1X_dewpoint1(float t,float h); //露点
///*****************************/

//#endif

