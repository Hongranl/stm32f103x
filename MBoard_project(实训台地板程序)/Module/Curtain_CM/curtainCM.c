/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     curtainCM
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 21:02:41
 * @Description: 
 *————
 *---------------------------------------------------------------------------*/

#include "curtainCM.h"//窗帘控制驱动进程函数；

static curtainCM_MEAS curtainATTR;

extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明

osThreadId tid_curtainCM_Thread;
osThreadDef(curtainCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  curtainCM_pool;								 
osPoolDef(curtainCM_pool, 10, curtainCM_MEAS);                  // 内存池定义
osMessageQId  MsgBox_curtainCM;
osMessageQDef(MsgBox_curtainCM, 2, &curtainCM_MEAS);            // 消息队列定义，用于模块线程向无线通讯线程
osMessageQId  MsgBox_MTcurtainCM;
osMessageQDef(MsgBox_MTcurtainCM, 2, &curtainCM_MEAS);          // 消息队列定义,用于无线通讯线程向模块线程
osMessageQId  MsgBox_DPcurtainCM;
osMessageQDef(MsgBox_DPcurtainCM, 2, &curtainCM_MEAS);          // 消息队列定义，用于模块线程向显示模块线程

void curtainCM_ioInit(void){

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE );	                
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10| GPIO_Pin_11;	//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12| GPIO_Pin_13| GPIO_Pin_14;	//输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	
	curtconSTP = 1;	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
}

void curtainCM_Init(void){

	curtainCM_ioInit();
}

void curtainCM_Thread(const void *argument){
	
	osEvent  evt;
    osStatus status;
	
	const uint8_t dpSize = 50;
	const uint8_t dpPeriod = 40;
	char  disp[dpSize];
	uint8_t Pcnt;
	uint8_t Kcnt;

	uint8_t Maxcnt = 0;
	
	
	static curtainCM_MEAS actuatorData;
	static curtainCM_MEAS Data_DPtemp;
	
	curtainCM_MEAS *mptr = NULL;
	curtainCM_MEAS *rptr = NULL;
	
	for(;;){
	
		evt = osMessageGet(MsgBox_MTcurtainCM, 100);
		if (evt.status == osEventMessage){		//等待消息指令
			
			rptr = evt.value.p;
			/*自定义本地线程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
			
			actuatorData.valACT = rptr->valACT;
			
			do{status = osPoolFree(curtainCM_pool, rptr);}while(status != osOK);	//内存释放
			rptr = NULL;
		}


		
			if(!curtKeySTP){			
			
			Kcnt = 100;
			while(!curtKeySTP && Kcnt)
				{
					osDelay(20);Kcnt --;
				}
			actuatorData.valACT  = CMD_CURTSTP;
			}
			else if(!curtKeyUP){			
				
				Kcnt = 100;
				while(!curtKeyUP && Kcnt){
					osDelay(20);Kcnt --;
				}
				actuatorData.valACT  = CMD_CURTUP;
			}
			else if(!curtKeyDN){			
				
				Kcnt = 100;
				while(!curtKeyDN && Kcnt){
					osDelay(20);Kcnt --;
				}
				actuatorData.valACT  = CMD_CURTDN;
			}
		
		
		 
		if(Data_DPtemp.valACT != actuatorData.valACT)
			switch(actuatorData.valACT){
			
				case CMD_CURTUP:	curtconSTP = 1;curtconDN = 0; delay_ms(20);curtconUP  = 1;delay_ms(50);	break;
				
				case CMD_CURTSTP:	
					{
						curtconSTP = 0; 
						
								curtconUP = 0;
								curtconDN = 0;
							
						delay_ms(100);	
						//curtconSTP = 1;
						
						break;

					}
					
				case CMD_CURTDN:	curtconSTP = 1;curtconUP = 0; delay_ms(20);	curtconDN  = 1;	delay_ms(50);break;
				
				default: 
					curtconUP = 0;
					curtconDN = 0;
					curtconSTP = 1;

				break;
				
			}
			
			if(actuatorData.valACT == CMD_CURTDN ||  
				actuatorData.valACT == CMD_CURTUP)
			{
				if(curtainENABLE == 1)
				{
					delay_ms(11);
					if(curtainENABLE == 1)//仍未启动
						Maxcnt++;
					else
						Maxcnt = 0;
					if(Maxcnt > 5)
					{
						actuatorData.valACT  = CMD_CURTmax;
						curtconUP = 0;
						curtconDN = 0;
						
						Maxcnt = 0;
					}
				}
					
			}
		
		if(Data_DPtemp.valACT != actuatorData.valACT){
		
			Data_DPtemp.valACT = actuatorData.valACT;
			gb_databuff[0] = actuatorData.valACT;
			do{mptr = (curtainCM_MEAS *)osPoolCAlloc(curtainCM_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
			mptr->valACT = actuatorData.valACT;
			osMessagePut(MsgBox_DPcurtainCM, (uint32_t)mptr, 100);
			osDelay(10);
		}
		
	
		
		
		if(Pcnt < dpPeriod){
			
			osDelay(20);
			Pcnt ++;
		}else{
			
			Pcnt = 0;
			gb_Exmod_key = true;
			gb_databuff[0] = actuatorData.valACT;
			memset(disp,0,dpSize * sizeof(char));
			sprintf(disp,"★--------------☆\n 窗帘状态 : %d\n",curtainATTR.valACT);
			Driver_USART1.Send(disp,strlen(disp));
			osDelay(20);
		}
		
		
		
		osDelay(10);
	}
}

void curtainCMThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		curtainCM_pool   = osPoolCreate(osPool(curtainCM_pool));	//创建内存池
		MsgBox_curtainCM 	= osMessageCreate(osMessageQ(MsgBox_curtainCM), NULL);   //创建消息队列
		MsgBox_MTcurtainCM = osMessageCreate(osMessageQ(MsgBox_MTcurtainCM), NULL);//创建消息队列
		MsgBox_DPcurtainCM = osMessageCreate(osMessageQ(MsgBox_DPcurtainCM), NULL);//创建消息队列
		
		memInit_flg = true;
	}

	curtainCM_Init();
	tid_curtainCM_Thread = osThreadCreate(osThread(curtainCM_Thread),NULL);
}
