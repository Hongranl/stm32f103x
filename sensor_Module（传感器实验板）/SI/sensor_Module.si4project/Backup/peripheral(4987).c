/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		peripheral.c
描述   :      外设传感器驱动程序文件
作者   :      hongxiang.liu
版本   :      V1.0
修改   :  
完成日期:		2019.3.25
************************************************************************************************************************/

/*************HEADS**********/
#include "peripheral.h"
/***************************/

/* 全局变量 */
/****************/

/**************************测距模块HC_SR04***************************/
/***************************************************************************************************************
*函数名：		HcSr04_Init
*描述：		测距传感器初始化			//输出：PA12   	输入：PA11
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*编写日期：	2019.3.25 09:40
*版本记录：	
****************************************************************************************************************/
void HcSr04_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(SR04_TRIG_PERIPH, ENABLE);
	//时钟使能

	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = SR04_TRIG_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(SR04_TRIG_PORT, &GPIO_InitStruct);
	//50MHz通用输出
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = SR04_ECHO_PIN;
	GPIO_Init(SR04_ECHO_PORT, &GPIO_InitStruct);
	//输入

	TIM1_Init();
	//输入捕获定时器配置初始化
}

/***************************************************************************************************************
*函数名：		HcSr04Start
*描述：		测距传感器发送开始			//输出：PA12   	输入：PA11
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*编写日期：	2019.3.25 13:12
*版本记录：	
****************************************************************************************************************/
void HcSr04Start(void)
{
	Tim1CaptureState=0;			//清除标志位的最高位，以便能够响应接下来的捕获中断和溢出中断，开启下一次的捕获
	HcSr04StartF = TRUE;		//开始标志置位
	SR04_TRIG_VALUE() = 1;		//输出高电平
	delay_us(20);				//延时20us
	SR04_TRIG_VALUE() = 0;		//输出低电平
	TIM_Cmd(TIM1,ENABLE);		//使能TIM1
}
void HcSr04_OFF(void)
{
	HcSr04StartF = FALSE;		//开始标志置位
	
	TIM_Cmd(TIM1,DISABLE);		//使能TIM1
}

/***************************************************************************************************************
*函数名：	GetHcSr04Value
*描述：		获取测距传感器
*作者：		hongxiang.liu
*参数：		无
*返回值：	u32 value——距离数值 (value==0，说明读取错误)
*编写日期：	2019.3.25 13:17
*版本记录：	
****************************************************************************************************************/
u32 GetHcSr04Value(void)
{
	u32 value = 0;
	
	if((Tim1CaptureState&0x80) == 0)	//标志位最高位不为1，说明测距没有完成完成
	{
		return value;
	}

	value = Tim1CaptureState&0x3F;	//获取溢出次数
	value *= 1000;					//得到溢出值对应的计数值(us)
	value += Tim1CaptureCount;		//再加上没有溢出的计数值(us)
	value = value*340/2/1000000;	//得到距离值  		l=t*340/2(m)
	
	TIM_Cmd(TIM1,DISABLE);			//失能TIM1
	Tim1CaptureState=0;				//清除标志位的最高位，以便能够响应接下来的捕获中断和溢出中断，开启下一次的捕获

	return value;
}

/**************************称重模块HX711***************************/
/***************************************************************************************************************
*函数名：		HX711_Init
*描述：		称重传感器初始化
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*编写日期：	2019.3.25 15:12
*版本记录：	
****************************************************************************************************************/
void HX711_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(HX711_SCK_PERIPH, ENABLE);
	//时钟使能
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = HX711_SCK_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(HX711_SCK_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin  = HX711_DAT_PIN;
	GPIO_Init(HX711_DAT_PORT, &GPIO_InitStruct);
}

/***************************************************************************************************************
*函数名：		Read_HX711
*描述：		读取称重传感器AD值
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*编写日期：	2019.3.25 15:42
*版本记录：	
****************************************************************************************************************/
u32 Read_HX711(void)             
{ 
	u32 HX711_value = 0; 
	u8 i = 0; 
	u32 count = 0xfff;
	GPIO_SetBits(HX711_DAT_PORT,HX711_DAT_PIN);			//DOUT=1 
	GPIO_ResetBits(HX711_SCK_PORT,HX711_SCK_PIN);		//SCK=0 
	while(GPIO_ReadInputDataBit(HX711_DAT_PORT,HX711_DAT_PIN))//DOUT=0  
	{
		if(count < 10)
		{
			return 0;
		}
		else
		{
			delay_us(1); 
		}
		count --;
	}
	for(i=0;i<24;i++) 
	{ 
	   GPIO_SetBits(HX711_SCK_PORT,HX711_SCK_PIN);		//SCK=1 
	   HX711_value = HX711_value<<1;					//左移
	   delay_us(1);  
	   GPIO_ResetBits(HX711_SCK_PORT,HX711_SCK_PIN);	//SCK=0 
	   if(GPIO_ReadInputDataBit(HX711_DAT_PORT,HX711_DAT_PIN))//DOUT=1 
	      HX711_value++; 
	   delay_us(1); 
	} 
	GPIO_SetBits(HX711_SCK_PORT,HX711_SCK_PIN); 
	delay_us(1); 
	GPIO_ResetBits(HX711_SCK_PORT,HX711_SCK_PIN); 
	delay_us(1);  
	return HX711_value;  
} 

/***************************************************************************************************************
*函数名：	Relay_Init
*描述：		继电器初始化
*作者：		hongxiang.liu
*参数：		无
*返回值：	无
*编写日期：	2019.3.29 11:29
*版本记录：	
****************************************************************************************************************/
void Relay_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(RELAY_PERIPH, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin  = RELAY_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_2MHz;

	GPIO_Init(RELAY_PORT, &GPIO_InitStruct);

	PAout(15) = 0;
}

