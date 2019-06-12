#include "RelaysoutletCM.h"//两路继电器驱动进程函数；

extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明

osThreadId tid_RelaysoutletCM_Thread;

osThreadDef(RelaysoutletCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  RelaysoutletCM_pool;								 
osPoolDef(RelaysoutletCM_pool, 10, RelaysoutletCM_MEAS);                  // 内存池定义
osMessageQId  MsgBox_RelaysoutletCM;
osMessageQDef(MsgBox_RelaysoutletCM, 2, &RelaysoutletCM_MEAS);            // 消息队列定义，用于模块线程向无线通讯线程
osMessageQId  MsgBox_MTRelaysoutletCM;
osMessageQDef(MsgBox_MTRelaysoutletCM, 2, &RelaysoutletCM_MEAS);          // 消息队列定义,用于无线通讯线程向模块线程
osMessageQId  MsgBox_DPRelaysoutletCM;
osMessageQDef(MsgBox_DPRelaysoutletCM, 2, &RelaysoutletCM_MEAS);          // 消息队列定义，用于模块线程向显示模块线程
static	unsigned char light1_vlue ;

void RelaysoutletCM_AIOinit(void){

	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE);	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M


	//PA0  作为模拟通道输入引脚  ADC12_IN8                       

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
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
//ch:通道值 0~3
uint16_t Get_Adc(uint8_t ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

uint32_t Get_Adc_Average(uint8_t ch,uint8_t times)
{
	u32 temp_val=0;
	uint32 buff =0;
	uint32 max_buff =0;
	uint8_t t;
	
	for(t=0;t<times;t++)
	{
		buff = ((Get_Adc(ch) *100)/ (41*3));
		
		if(buff > 2300)
			temp_val = buff -2300 ;
		else
			temp_val = 2300 - buff ;
	
		max_buff = max_buff > temp_val ? max_buff : temp_val;
		delay_ms(2);
	}
	return temp_val/times;
} 	

void RelaysoutletCM_Init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}

void RelaysoutletCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	u8 Kcnt = 0;
	u8 ADcnt = 0;
	uint32 AD_temp = 0;
	//u8 UPLD_cnt;
	//const u8 UPLD_period = 5;

	RelaysoutletCM_MEAS actuatorData;	//本地输入量
	
	static RelaysoutletCM_MEAS  Data_tempDP = {0};	//本地输入量显示数据对比缓存
    
	static RelaysoutletCM_MEAS *mptr = NULL;
	static RelaysoutletCM_MEAS *rptr = NULL;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	outlet1 = 0;
	outlet1_led = 1;
	light1_vlue = 0;
	for(;;){
	
		evt = osMessageGet( MsgBox_MTRelaysoutletCM, 100);
		if (evt.status == osEventMessage){		//等待消息指令
			
			rptr = evt.value.p;
			/*自定义本地线程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
			
			actuatorData.relay_con = rptr->relay_con;

			do{status = osPoolFree(RelaysoutletCM_pool, rptr);}while(status != osOK);	//内存释放
			rptr = NULL;
		}
		

		
	
		if(Data_tempDP.relay_con != actuatorData.relay_con
		  || Data_tempDP.Current != actuatorData.Current){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			Data_tempDP.Current 	= actuatorData.Current;
			
			outlet1 	= (Data_tempDP.relay_con >> 0) & 0x01;
			outlet1_led	= !(Data_tempDP.relay_con >> 0) & 0x01;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
			mptr->relay_con = actuatorData.relay_con;
			mptr->Current 	= actuatorData.Current;
			osMessagePut(MsgBox_DPRelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		
		if(key_vlue == 0)
		{
			Kcnt = 100;
			while(key1_vlue == 0 && Kcnt--)
				{
					osDelay(25);
					
				}
			light1_vlue =!light1_vlue;

				
		}
		
		
		actuatorData.relay_con = light1_vlue  ;
		if(ADcnt < 10)
		{
			AD_temp += Get_Adc_Average(0,10);//Vi (mv) =data* 3.3* 1000 /4096
			ADcnt++;
			}
		else
		{
			actuatorData.Current = AD_temp ;
			AD_temp = 0;
			ADcnt = 0;
			
		}
		
		
		
	}
}


void RelaysoutletCMThread_Active(void){

	RelaysCM_Init();
	RelaysoutletCM_AIOinit();
	RelaysoutletCM_pool   = osPoolCreate(osPool(RelaysoutletCM_pool));	//创建内存池
	MsgBox_RelaysoutletCM 	= osMessageCreate(osMessageQ(MsgBox_RelaysoutletCM), NULL);	//创建消息队列
	MsgBox_MTRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_MTRelaysoutletCM), NULL);//创建消息队列
	MsgBox_DPRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_DPRelaysoutletCM), NULL);//创建消息队列
	tid_RelaysoutletCM_Thread = osThreadCreate(osThread(RelaysoutletCM_Thread),NULL);
}
