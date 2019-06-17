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
	
	//u8 UPLD_cnt;
	//const u8 UPLD_period = 5;

	RelaysoutletCM_MEAS actuatorData;	//本地输入量
	static RelaysoutletCM_MEAS  Data_temp   = {0};	//下行数据输入量同步对比缓存
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
		
		if(Data_temp.relay_con != actuatorData.relay_con){
		
			Data_temp.relay_con = actuatorData.relay_con;
			
			outlet1 	= (Data_temp.relay_con >> 0) & 0x01;
			outlet1_led	= !(Data_temp.relay_con >> 0) & 0x01;
//			osDelay(10);
		}
	
		if(Data_tempDP.relay_con != actuatorData.relay_con){
		
			Data_tempDP.relay_con = actuatorData.relay_con;
			
			do{mptr = (RelaysoutletCM_MEAS *)osPoolCAlloc(RelaysoutletCM_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
			mptr->relay_con = actuatorData.relay_con;
			osMessagePut(MsgBox_DPRelaysoutletCM, (uint32_t)mptr, 100);
			osDelay(10);
		}

		
		if(key_vlue == 0)
		{
			osDelay(35);
			if(key_vlue == 0)
				{
					light1_vlue =!light1_vlue;
					
				}

				
		}
		
		
		actuatorData.relay_con = light1_vlue  ;

		osDelay(10);
	}
}


void RelaysoutletCMThread_Active(void){

	RelaysCM_Init();
	RelaysoutletCM_pool   = osPoolCreate(osPool(RelaysoutletCM_pool));	//创建内存池
	MsgBox_RelaysoutletCM 	= osMessageCreate(osMessageQ(MsgBox_RelaysoutletCM), NULL);	//创建消息队列
	MsgBox_MTRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_MTRelaysoutletCM), NULL);//创建消息队列
	MsgBox_DPRelaysoutletCM = osMessageCreate(osMessageQ(MsgBox_DPRelaysoutletCM), NULL);//创建消息队列
	tid_RelaysoutletCM_Thread = osThreadCreate(osThread(RelaysoutletCM_Thread),NULL);
}
