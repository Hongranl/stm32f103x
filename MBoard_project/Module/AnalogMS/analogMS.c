/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     analogMS
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 21:01:22
 * @Description: 
 *————模拟信号采集模块
 *	即两路数字电流、两路数字电压，采集线程
 *---------------------------------------------------------------------------*/


#include "analogMS.h"//模拟信号即数字电表检测驱动进程函数；

extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明

osThreadId tid_analogMS_Thread;				//线程ID
osThreadDef(analogMS_Thread,osPriorityNormal,1,512);	//Create a Thread Definition with function, priority, and stack requirements.
			 
osPoolId  analogMS_pool;								 
osPoolDef(analogMS_pool, 10, analogMS_MEAS);                  // 内存池定义
osMessageQId  MsgBox_analogMS;
osMessageQDef(MsgBox_analogMS, 2, &analogMS_MEAS);            // 消息队列定义，用于模块进程向无线通讯进程
osMessageQId  MsgBox_MTanalogMS;
osMessageQDef(MsgBox_MTanalogMS, 2, &analogMS_MEAS);          // 消息队列定义,用于无线通讯进程向模块进程
osMessageQId  MsgBox_DPanalogMS;
osMessageQDef(MsgBox_DPanalogMS, 2, &analogMS_MEAS);          // 消息队列定义，用于模块进程向显示模块进程
/**************************************
* @description: 初始化GPIO->GPIOA (GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5) 
*		ADC1 对应0、1、4、5channel(Don't miss an N)
* @param:  void
* @return: void
************************************/
void analogMS_ADCInit(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M                     

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_4|GPIO_Pin_5;
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


}
/**************************************
* @description: 
* @param ：设置采集通道，ADCx->channel0~16，选择对应通道采集，即是 ch=0读取GPIOA->GPIO_Pin_0
* @return: 返回对应通道采集数值，  采用十二位精度，即MAX_Value=4096, 16bit->MAX_Value=65535,
*		12-bit configurable resolution -> 16-bit data 
************************************/
u16 analogGet_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
/**************************************
* @description: 
* @param：采集通道->ch，采集次数->times
* @return: 返回均值
************************************/
u16 analogGet_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val += analogGet_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 
/**************************************
* @description:模块初始化函数 
* @param 
* @return: 
************************************/
void analogMS_Init(void){

	analogMS_ADCInit();
}
/**************************************
* @description:模块线程函数，定时采集，定时触发上传信号
* @param：创建时设定值->argument
* @return: 
************************************/
void analogMS_Thread(const void *argument){

	osEvent  evt;
   	osStatus status;

	analogMS_MEAS sensorData;

	static analogMS_MEAS Data_tempDP = {1};
	
	analogMS_MEAS *mptr = NULL;
	analogMS_MEAS *rptr = NULL;
	
	for(;;){
		/*自定义本地进程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
		evt = osMessageGet(MsgBox_MTanalogMS, 100);
		if (evt.status == osEventMessage) {		//等待消息指令
			
			rptr = evt.value.p;
			
			//囧
			//---------------空--------------------

			do{status = osPoolFree(analogMS_pool, rptr);}while(status != osOK);	//内存释放
			rptr = NULL;
		}
		//----------------数据采集-------------------------
		sensorData.Ich1 = analogGet_Adc_Average(ADC_Channel_0,10);
		sensorData.Vch1 = analogGet_Adc_Average(ADC_Channel_1,10);
		sensorData.Ich2 = analogGet_Adc_Average(ADC_Channel_4,10);
		sensorData.Vch2 = analogGet_Adc_Average(ADC_Channel_5,10);

		//---------------有改变即上传-----------------------
		if(Data_tempDP.Ich1 != sensorData.Ich1 ||
		   Data_tempDP.Vch1 != sensorData.Vch1||
		   Data_tempDP.Ich2 != sensorData.Ich2 ||
		   Data_tempDP.Vch2 != sensorData.Vch2){
		
			Data_tempDP.Ich1 = sensorData.Ich1;
			Data_tempDP.Vch1 = sensorData.Vch1;
			Data_tempDP.Ich2 = sensorData.Ich2;
			Data_tempDP.Vch2 = sensorData.Vch2;
			//------------触发上传信号------------------
			gb_Exmod_key = true;
			memcpy(gb_databuff,&Data_tempDP.Vch1, 4);
			memcpy(gb_databuff+4,&Data_tempDP.Ich1, 4);	
			memcpy(gb_databuff+8,&Data_tempDP.Vch2, 4);
			memcpy(gb_databuff+12,&Data_tempDP.Ich2, 4);
			//------------显示改变消息推送----------------	
			do{mptr = (analogMS_MEAS *)osPoolCAlloc(analogMS_pool);}while(mptr == NULL);
			mptr->Ich1 = sensorData.Ich1;
			mptr->Vch1 = sensorData.Vch1;
			mptr->Ich2 = sensorData.Ich2;
			mptr->Vch2 = sensorData.Vch2;
			osMessagePut(MsgBox_DPanalogMS, (uint32_t)mptr, 100);
			osDelay(10);
		}
		
		
		osDelay(100);
	}
}
/**************************************
* @description: 线程创建接口函数
* @param：
* @return: 
************************************/
void analogMSThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		analogMS_pool   = osPoolCreate(osPool(analogMS_pool));	//创建内存池
		MsgBox_analogMS 	= osMessageCreate(osMessageQ(MsgBox_analogMS), NULL);   //创建消息队列
		MsgBox_MTanalogMS = osMessageCreate(osMessageQ(MsgBox_MTanalogMS), NULL);//创建消息队列
		MsgBox_DPanalogMS = osMessageCreate(osMessageQ(MsgBox_DPanalogMS), NULL);//创建消息队列
		
		memInit_flg = true;
	}

	analogMS_Init();
	tid_analogMS_Thread = osThreadCreate(osThread(analogMS_Thread),NULL);
}
