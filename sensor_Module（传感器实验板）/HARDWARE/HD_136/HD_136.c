/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		HD-136.c
描述   :      HD-136液位传感器模块程序文件
作者   :      hongxiang.liu
版本   :      V1.0
修改   :  
完成日期:		2019.3.21
************************************************************************************************************************/

/*************HEADS**********/
#include "HD_136.h"
/***************************/

/* 全局变量声明 */
/****************/

/***************************************************************************************************************
*函数名：	HD_136_GpioConfig
*描述：		HD_136的GPIO配置		
			//--PA6
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*完成日期：	2019.3.19
*版本记录：	
****************************************************************************************************************/
void HD_136_GpioConfig(void)
{
	RCC_APB2PeriphClockCmd(HD_136_PERIPH, ENABLE);
	//打开GPIOA的时钟

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin  = HD_136_PIN;
	GPIO_Init(HD_136_PORT, &GPIO_InitStruct);
	//初始化HD_136的GPIO(模拟输入)
}

/***************************************************************************************************************
*函数名：		ADC1_Config
*描述：		ADC1配置		
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*完成日期：	2019.3.19
*版本记录：	
****************************************************************************************************************/
void ADC1_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	ADC_InitTypeDef ADC_InitStruct;
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	//独立模式
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;
	//非扫描模式
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	//单次转换模式
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//软件触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	//数据右对齐
	ADC_InitStruct.ADC_NbrOfChannel = 1;
	//通道数量
}

