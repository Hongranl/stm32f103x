/***********************************************************************************************************************
Copyright 2019 - 2027 深圳国泰安教育技术股份有限公司. All rights reserved.
文件名 :		adc.c
描述   :      adc驱动程序文件
作者   :      hongxiang.liu
版本   :      V1.0
修改   :  
完成日期:		2019.3.22
************************************************************************************************************************/

/*************HEADS**********/
#include "adc.h"
/***************************/

/* 全局变量 */
/****************/

/***************************************************************************************************************
*函数名：		Adc_Config
*描述：		ADC初始化		//PA5 PA6 PA7
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*完成日期：	2019.3.19
*版本记录：	
****************************************************************************************************************/
void Adc_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;

	/*1.打开时钟*/
	RCC_APB2PeriphClockCmd(HD_136_PERIPH|RCC_APB2Periph_ADC1|RCC_AHBPeriph_DMA1, ENABLE);
	//使能GPIOA、ADC1和DMA1的时钟

	/*2.GPIO配置*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin  = FAN_TAC_PIN|HD_136_PIN|RP_C18_3_PIN;
	GPIO_Init(HD_136_PORT, &GPIO_InitStruct);
	//初始化PA5、PA6和PA7——模拟输入模式					//PA5-FAN_TAC(风扇) PA6 HD_136(水位) PA7 RP_C18_3(压力)

	/*3.ADC配置*/
	ADC_DeInit(ADC1);	//复位ADC1
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	//独立模式
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	//扫描模式
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
	//单次转换模式
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	//软件触发
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	//数据右对齐
	ADC_InitStruct.ADC_NbrOfChannel = 3;
	//通道数量3
	ADC_Init(ADC1, &ADC_InitStruct);
	//初始化ADC1
	
	/*4.配置规则通道采样顺序和时间 PA5-Channel_5 PA6-Channel_6 PA7-Channel_7*/
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
	//通道5，第一个采样目标，周期55.5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
	//通道6，第二个采样目标，周期55.5
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
	//通道7，第三个采样目标，周期55.5

	/*5.开启ADC及校准*/
	ADC_Cmd(ADC1,ENABLE);
	//使能指定的ADC1
	ADC_ResetCalibration(ADC1);
	//使能复位校准
	while(ADC_GetResetCalibrationStatus(ADC1));
	//等待复位校准结束
	ADC_StartCalibration(ADC1);
	//开启ADC校准
	while(ADC_GetCalibrationStatus(ADC1));
	//等待ADC校准结束
	
	/*5.DMA配置*/
	DMA_DeInit(DMA1_Channel1);//DMA1的通道1复位

	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
	//DMA外设基地址
	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)AdcBuf;
	//DMA内存基地址
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	//DMA数据传输方向——外设作为数据来源
	DMA_InitStruct.DMA_BufferSize = 3;
	//传输缓存大小（HD_136 RP_C18_3）
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	//外设地址递增关闭
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	//内存地址递增打开
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	//外设数据大小-半字（16位）
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	//内存数据大小
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	//循环模式
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	//通道的优先级——高
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	//不设置为内存到内存传输
	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
	//根据参数初始化DMA1得通道1
		
	ADC_DMACmd(ADC1,ENABLE);		//使能ADC的DMA请求
	DMA_Cmd(DMA1_Channel1,ENABLE);	//使能ADC1_DMA所指示的通道
	
	// /*6.DMA中断设置*/
	// NVIC_InitTypeDef NVIC_InitStruct;
	// NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;//中断通道
	// NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
	// NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//响应优先级
	// NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	// NVIC_Init(&NVIC_InitStruct);
	
	// DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//开启DMA的传输完成中断
}



