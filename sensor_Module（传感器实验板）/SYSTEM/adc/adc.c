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
//void Adc_Init(void)
//{
//	GPIO_InitTypeDef GPIO_InitStruct;
//	ADC_InitTypeDef ADC_InitStruct;
////	DMA_InitTypeDef DMA_InitStruct;
//
//	/*1.打开时钟*/
//	RCC_APB2PeriphClockCmd(HD_136_PERIPH|RCC_APB2Periph_ADC1|RCC_AHBPeriph_DMA1, ENABLE);
//	//使能GPIOA、ADC1和DMA1的时钟
//
//	/*2.GPIO配置*/
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
//	GPIO_InitStruct.GPIO_Pin  = FAN_TAC_PIN|HD_136_PIN|RP_C18_3_PIN;
//	GPIO_Init(HD_136_PORT, &GPIO_InitStruct);
//	//初始化PA5、PA6和PA7——模拟输入模式					//PA5-FAN_TAC(风扇) PA6 HD_136(水位) PA7 RP_C18_3(压力)
//
//	/*3.ADC配置*/
//	ADC_DeInit(ADC1);	//复位ADC1
//	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
//	//独立模式
//	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
//	//扫描模式
//	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;
//	//单次转换模式
//	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
//	//软件触发
//	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
//	//数据右对齐
//	ADC_InitStruct.ADC_NbrOfChannel = 3;
//	//通道数量3
//	ADC_Init(ADC1, &ADC_InitStruct);
//	//初始化ADC1
//	
//	/*4.配置规则通道采样顺序和时间 PA5-Channel_5 PA6-Channel_6 PA7-Channel_7*/
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_55Cycles5);
//	//通道5，第一个采样目标，周期55.5
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 2, ADC_SampleTime_55Cycles5);
//	//通道6，第二个采样目标，周期55.5
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 3, ADC_SampleTime_55Cycles5);
//	//通道7，第三个采样目标，周期55.5
//
//	/*5.开启ADC及校准*/
//	ADC_Cmd(ADC1,ENABLE);
//	//使能指定的ADC1
//	ADC_ResetCalibration(ADC1);
//	//使能复位校准
//	while(ADC_GetResetCalibrationStatus(ADC1));
//	//等待复位校准结束
//	ADC_StartCalibration(ADC1);
//	//开启ADC校准
//	while(ADC_GetCalibrationStatus(ADC1));
//	//等待ADC校准结束
//
//	return;
//
////	/*5.DMA配置*/
////	DMA_DeInit(DMA1_Channel1);//DMA1的通道1复位
//
////	DMA_InitStruct.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;
////	//DMA外设基地址
////	DMA_InitStruct.DMA_MemoryBaseAddr = (u32)AdcBuf;
////	//DMA内存基地址
////	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
////	//DMA数据传输方向——外设作为数据来源
////	DMA_InitStruct.DMA_BufferSize = 3;
////	//传输缓存大小（HD_136 RP_C18_3）
////	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
////	//外设地址递增关闭
////	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
////	//内存地址递增打开
////	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
////	//外设数据大小-半字（16位）
////	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
////	//内存数据大小
////	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
////	//循环模式
////	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
////	//通道的优先级——高
////	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
////	//不设置为内存到内存传输
////	DMA_Init(DMA1_Channel1,&DMA_InitStruct);
////	//根据参数初始化DMA1得通道1
////		
////	ADC_DMACmd(ADC1,ENABLE);		//使能ADC的DMA请求
////	DMA_Cmd(DMA1_Channel1,ENABLE);	//使能ADC1_DMA所指示的通道
//	//ADC_SoftwareStartConvCmd(ADC1, ENABLE);\
//	//ADC_GetConversionValue(ADC1);
//	// /*6.DMA中断设置*/
//	// NVIC_InitTypeDef NVIC_InitStruct;
//	// NVIC_InitStruct.NVIC_IRQChannel = DMA1_Channel1_IRQn;//中断通道
//	// NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;//抢占优先级
//	// NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//响应优先级
//	// NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	// NVIC_Init(&NVIC_InitStruct);
//	
//	// DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);//开启DMA的传输完成中断
//}
void Adc_Init(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M


	//PC0 1 作为模拟通道输入引脚  ADC12_IN8                       

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能
}

//获得ADC值
//ch:通道值 
uint16_t Get_Adc(uint8_t ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

uint16_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
	u32 temp_val=0;
	uint8_t t;
	
	for(t=0;t<times;t++)
	{
		temp_val += Get_Adc(ch);
		delay_ms(2);
	}
	return temp_val / times;
} 	


