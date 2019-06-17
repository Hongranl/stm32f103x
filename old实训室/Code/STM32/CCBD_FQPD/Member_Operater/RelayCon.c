#include "dataManager.h"

extern osMessageQId MsgBoxID_SBD_Relay;
extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明

osThreadId tid_RelayCon_Thread;
osThreadDef(RelayCon_Thread,osPriorityNormal,1,1024);

static void RelayCon_spiInit(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	relayMR 	= 1;
	relaySTCP	= 0;
	relaySHCP	= 0;
	relayDIN	= 0;
}

static void RelayCon_keyInit(void){

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void RelayCon_Init(void){

	RelayCon_keyInit();
	RelayCon_spiInit();
}

void RelayDatsIn595(uint16_t Dats){

	uint8_t loop;
	
	relayMR	  = 0;
	osDelay(10);
	relayMR	  = 1;
	
	relaySTCP = 0;
	
	for(loop = 0;loop < 16;loop ++){
	
		relaySHCP	= 0;
		relayDIN	= Dats & 0x0001;
		relaySHCP	= 1;
		Dats	  	= Dats>>1;
	}
	
	relaySTCP = 1;
	osDelay(10);
	relaySTCP = 0;
}

u16 keyPsy_read(void){

	u16 keyVAL = 0;
	
	if(!relayK0)keyVAL |= 0x8000 >> 0;
	if(!relayK1)keyVAL |= 0x8000 >> 1;
	if(!relayK2)keyVAL |= 0x8000 >> 2;
	if(!relayK3)keyVAL |= 0x8000 >> 3;
	if(!relayK4)keyVAL |= 0x8000 >> 4;
	if(!relayK5)keyVAL |= 0x8000 >> 5;
	if(!relayK6)keyVAL |= 0x8000 >> 6;
	if(!relayK7)keyVAL |= 0x8000 >> 7;
	if(!relayK8)keyVAL |= 0x8000 >> 8;
	if(!relayK9)keyVAL |= 0x8000 >> 9;
	
	return keyVAL;
}

void RelayCon_Thread(const void *argument){
	
	osEvent  evt;
	
	u16 relay_TMP = 0;
	u16 relay_PSY = 0;
	
	u16 keyVal_A = 0;
	u16 keyVal_B = 0;
	bool keyTrig_FLG = false;	//按键触发标志
	bool keyRles_FLG = true;	//按键释放标志
	u8 key_KeepCount = 0;		//消抖计数
	const u8 keyKcount_DetPeriod = 1;		//非阻塞消抖时间，可调节按键滞留感
	
	for(;;){
		
		/*消息队列接收处理*/
		evt = osMessageGet(MsgBoxID_SBD_Relay, 1);  // wait for message
		if(evt.status == osEventMessage){
		
			SourceBD_MEAS *rptr = evt.value.p;
			if(rptr->datsType == datsRelayCon){
			
				switch(rptr->role){
				
					case roleUTZigBDNLoad:{
						
						relay_TMP = (((u16)rptr->dats.datsRelayCon.conArryDats[0] & 0x00FF) << 8) + (((u16)rptr->dats.datsRelayCon.conArryDats[1] & 0x00FF) << 0);
					}break;
					
					default:break;
				}
			}
			osPoolFree(memid_SourceBD_pool,rptr);  
		}
		
		/*物理按键处理*/
		keyVal_A = keyPsy_read();	//按键初态值
		if(keyVal_A){
		
			if(!keyTrig_FLG && keyRles_FLG){
			
				keyTrig_FLG = true;
			}
		}else{
		
			keyRles_FLG = true;
		}
		
		if(keyTrig_FLG == true && keyVal_A == keyPsy_read()){	//按键次态值确认
		
			if(key_KeepCount < keyKcount_DetPeriod)key_KeepCount ++;
			else{
				
				u8 	loop; 
				u16 temp;
				
				keyVal_B = keyPsy_read();
				keyVal_A &= keyVal_B;
				
				for(loop = 0;loop < 10;loop ++){
				
					temp = 0x8000 >> loop;
					if(temp & keyVal_A){
					
						if(temp & relay_TMP){	//取反
							
							relay_TMP &= ~(0x8000 >> loop);
						}else{
						
							relay_TMP |= 0x8000 >> loop;
						}
					}
				}
				
				keyTrig_FLG = false;
				keyRles_FLG = false;
				key_KeepCount = 0;
			}
		}else{
		
			keyTrig_FLG = false;
		}
		
		/*执行构件动作*/
		relay_PSY = relay_TMP;
		RelayDatsIn595(relay_PSY);
	}
}

void RelayConThread_Active(void){

	RelayCon_Init();
	tid_RelayCon_Thread = osThreadCreate(osThread(RelayCon_Thread),NULL);
}
