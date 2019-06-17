
/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		my_define.c
描述   :      存放全局变量定义
作者   :      hongxiang.liu
版本   :      V1.0
修改   :  
完成日期:		2019.3.22
************************************************************************************************************************/
/*************HEADS*********/
#include "global.h"
/***************************/

/****** 全局变量声明 ******/
uint64_t SystemCnt;		//系统运行时间(ms)
bool TimeOutFlag;		//10ms到达标志位
u8  Tim1CaptureState;	//TIM1捕获状态位  		最高位bit7-成功捕获 次高位bit6-已经捕获到上升沿 bit5~bit0-高电平后溢出次数
u16	Tim1CaptureCount;	//TIM1捕获计数值
bool HcSr04StartF;		//测距传感器开始的标志
u8  SwitchType;			//8421编码开关键值
u32 AdcBuf[3];			//保持ADC采集值
u16 PwmSpeed;			//调速风扇转速
bool PwmPowerFlag;		//风扇开启标志
SHT1x sht1x; 			//定义传感器结构体
float SoilTemp;			//土壤温度
float SoilHum;			//土壤湿度
float WindSpeed;		//风速值
bool  Key1Flag;			//KEY1按键值
bool  Key2Flag;			//KEY2按键值
uint64_t  BeepCnt;			//蜂鸣器开启计时

u16 humi_val,temp_val;		/**/
float humi_val_real = 0.0; 	//SHT11 温湿度相关变量
float temp_val_real = 0.0;	/**/
float dew_point = 0.0;		/**/
/**************************/




