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

void RelaysoutletCM_Init(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
}

void RelaysoutletCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	u8 outlet1_vlue = 0;
	u8 outlet2_vlue = 0;
	u8 UPLD_cnt;
	const u8 UPLD_period = 5;

	RelaysoutletCM_MEAS actuatorData;	//本地输入量
	static RelaysoutletCM_MEAS  Data_temp   = {0};	//下行数据输入量同步对比缓存
	static RelaysoutletCM_MEAS  Data_tempDP = {0};	//本地输入量显示数据对比缓存
	
	RelaysoutletCM_MEAS *mptr = NULL;
	RelaysoutletCM_MEAS *rptr = NULL;
	
	for(;;){
	
		evt = osMessageGet( MsgBox_MTRelaysoutletCM, 100);
		if (evt.status == osEventMessage){		//等待消息指令
			
			rptr = evt.value.p;
			/*自定义本地线程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
			
			actuatorData.relay_con = rptr->relay_con;

			do{status = osPoolFree(RelaysoutletCM_pool, rptr);}while(status != osOK);	//内存释放
			rptr = NULL;
		}
		
		if(Data_temp.relay_con != actuatorData.relay_con){
		
			Data_temp.relay_con = actuatorData.relay_con;
			
			outlet1 = (Data_temp.relay_con >> 0) & 0x01;
			outlet2 = (Data_temp.relay_con >> 1) & 0x01;
		}
	
		if(Data_tempDP.relay_con != actuatorData.relay_con){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_DPRelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		if(UPLD_cnt < UPLD_period)
			UPLD_cnt ++;	//数据定时上传
		else{
		
			UPLD_cnt = 0;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_RelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}
		if(key1_vlue == 0)
		{
			osDelay(25);
			if(key1_vlue == 0)
				{
					outlet1_vlue =!outlet1_vlue;
					
					
				}
				
				
		}
		if(key2_vlue == 0)
		{
			osDelay(25);
			if(key2_vlue == 0)
				{
					outlet2_vlue =!outlet2_vlue;
					
				}
				
				
		}
		
		actuatorData.relay_con = outlet1_vlue | (outlet2_vlue<<1) ;
		
	}
}

void RelaysoutletCMThread_Active(void){

	RelaysoutletCM_Init();
	RelaysoutletCM_pool   = osPoolCreate(osPool(RelaysoutletCM_pool));	//创建内存池
	MsgBox_RelaysoutletCM 	= osMessageCreate(osMessageQ(MsgBox_RelaysoutletCM), NULL);	//创建消息队列
	MsgBox_MTRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_MTRelaysoutletCM), NULL);//创建消息队列
	MsgBox_DPRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_DPRelaysoutletCM), NULL);//创建消息队列
	
	tid_RelaysoutletCM_Thread = osThreadCreate(osThread(RelaysoutletCM_Thread),NULL);
}
