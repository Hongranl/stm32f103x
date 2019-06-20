#include "shakeMS.h"//震动传感器检测驱动的进程函数

extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明

osThreadId tid_shakeMS_Thread;
osThreadDef(shakeMS_Thread,osPriorityNormal,1,512);
			 
osPoolId  shakeMS_pool;								 
osPoolDef(shakeMS_pool, 10, shakeMS_MEAS);                  // 内存池定义
osMessageQId  MsgBox_shakeMS;
osMessageQDef(MsgBox_shakeMS, 2, &shakeMS_MEAS);            // 消息队列定义，用于模块进程向无线通讯进程
osMessageQId  MsgBox_MTshakeMS;
osMessageQDef(MsgBox_MTshakeMS, 2, &shakeMS_MEAS);          // 消息队列定义,用于无线通讯进程向模块进程
osMessageQId  MsgBox_DPshakeMS;
osMessageQDef(MsgBox_DPshakeMS, 2, &shakeMS_MEAS);          // 消息队列定义，用于模块进程向显示模块进程

void shakeDIO_Init(void){

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);		
}

void shakeMS_Init(void){

	shakeDIO_Init();
}

void shakeMS_Thread(const void *argument){

	osEvent  evt;
    osStatus status;	
	
	const bool UPLOAD_MODE = false;	//1：数据变化时才上传 0：周期定时上传
	
	const uint8_t upldPeriod = 5;	//数据上传周期因数（UPLOAD_MODE = false 时有效）
	
	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;

	const uint8_t dpSize = 30;
	const uint8_t dpPeriod = 40;
	
	static uint8_t Pcnt = 0;
	char disp[dpSize];
	
	shakeMS_MEAS	sensorData;
	static shakeMS_MEAS Data_temp = {1};
	static shakeMS_MEAS Data_tempDP = {1};
	
	shakeMS_MEAS *mptr = NULL;
	shakeMS_MEAS *rptr = NULL;
	
	for(;;){
		
	/***********************本地进程数据接收***************************************************/
	//传感器构件数据仅作上传，接收数据功能保留，暂时不用
		evt = osMessageGet(MsgBox_MTshakeMS, 100);
		if (evt.status == osEventMessage) {		//等待消息指令
			
			rptr = evt.value.p;
			/*自定义本地进程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
			

			do{status = osPoolFree(shakeMS_pool, rptr);}while(status != osOK);	//内存释放
			rptr = NULL;
		}

	/***********************驱动数据采集*****************************************************/
		sensorData.VAL = shake_DATA;		//数据采集
		
		if(!UPLOAD_MODE){	//选择上传触发模式
		
			if(UPLDcnt < upldPeriod)UPLDcnt ++;
			else{
			
				UPLDcnt = 0;
				UPLD_EN = true;
			}
		}else{
		
			if(Data_temp.VAL != sensorData.VAL){	//数据推送（数据更替时才触发）
				
				Data_temp.VAL = sensorData.VAL;
				UPLD_EN = true;
			}
		}

	/***********************进程数据推送*****************************************************/		
		if(UPLD_EN){
			
			UPLD_EN = false;
			gb_Exmod_key = true;
			gb_databuff[0] = sensorData.VAL;
			osDelay(10);
		}
		
		if(Data_tempDP.VAL != sensorData.VAL){	//数据推送（数据更替时才触发）
		
			Data_tempDP.VAL = sensorData.VAL;
			gb_Exmod_key = true;
			gb_databuff[0] = sensorData.VAL;
			do{mptr = (shakeMS_MEAS *)osPoolCAlloc(shakeMS_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
			mptr->VAL = sensorData.VAL;
			osMessagePut(MsgBox_DPshakeMS, (uint32_t)mptr, 100);
			osDelay(10);
		}
	/***********************Debug_log*********************************************************/		
		if(Pcnt < dpPeriod){osDelay(10);Pcnt ++;}
		else{
		
			Pcnt = 0;
			memset(disp,0,dpSize * sizeof(char));
			sprintf(disp,"\n\ris firing now? : %d\n\r", Data_temp.VAL);			
			Driver_USART1.Send(disp,strlen(disp));
			osDelay(20);
		}
		osDelay(10);
	}
}

void shakeMSThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		shakeMS_pool   = osPoolCreate(osPool(shakeMS_pool));	//创建内存池
		MsgBox_shakeMS 	= osMessageCreate(osMessageQ(MsgBox_shakeMS), NULL);   //创建消息队列
		MsgBox_MTshakeMS = osMessageCreate(osMessageQ(MsgBox_MTshakeMS), NULL);//创建消息队列
		MsgBox_DPshakeMS = osMessageCreate(osMessageQ(MsgBox_DPshakeMS), NULL);//创建消息队列
		
		memInit_flg = true;
	}

	shakeMS_Init();
	tid_shakeMS_Thread = osThreadCreate(osThread(shakeMS_Thread),NULL);
}
