#include <Key&Tips.h>//底板按键检测进程函数及其按键回调函数；
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern ARM_DRIVER_USART Driver_USART1;								//设备驱动库串口一设备声明

osThreadId tid_keyMboard_Thread;										//按键监测主进程ID
osThreadDef(keyMboard_Thread,osPriorityAboveNormal,1,2048);	//按键监测主进程定义

typedef void (* funkeyThread)(funKeyInit key_Init,Obj_keyStatus *orgKeyStatus,funKeyScan key_Scan,Obj_eventKey keyEvent,const char *Tips_head);

/***按键外设初始化***/
void keyInit(void){	

	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC,ENABLE);		//时钟分配
	
	

	//按键 KEY 1~8声明
	GPIO_InitStructure.GPIO_Pin = 
			GPIO_Pin_0|
			GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
			GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
			GPIO_Pin_7; //端口属性分配
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

	//继电器 1 声明	
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8;	//端口属性分配
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		
	//LED 7、8声明 继电器 6~8 声明	
	GPIO_InitStructure.GPIO_Pin = 
			GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_13|
			GPIO_Pin_14|GPIO_Pin_15;	//端口属性分配
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);

	//LED 1~6声明 继电器 2~5 声明
	GPIO_InitStructure.GPIO_Pin = 
		GPIO_Pin_0|
		GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|
		GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|
		GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;	//端口属性分配
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	LED1 = LED2 = LED3=LED4=LED5=LED6=LED7=LED8 = 1;
	Relay1 = Relay2 = Relay3=Relay4=Relay5=Relay6=Relay7=Relay8 = 0;

	
	GPIO_InitStructure.GPIO_Pin = 
		GPIO_Pin_4 ; 				//底板指示灯
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//最高输出速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;			
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	PBout(4) = 0;
	 
	
}

/***按键键值读取***/
 uint16_t keyScan(void){

	if(!K1)return KEY_VALUE_1;		//键值1
	if(!K2)return KEY_VALUE_2;		//键值2
	return KEY_NULL;				//无按键
}

/***按键检测状态机***/
static uint16_t getKey(Obj_keyStatus *orgKeyStatus,funKeyScan keyScan){

	static	uint16_t s_u16KeyState 		= KEY_STATE_INIT;		//状态机检测状态，初始化状态
	static	uint16_t	s_u16LastKey		= KEY_NULL;			//保留历史按键键值	
	static	uint8_t	KeyKeepComfirm		= 0;					//长按后确认保持 确认所用时长计时
	static	uint16_t	s_u16KeyTimeCount	= 0;				//长按时长定义（用来对KEY_TICK进行计数，根据这个计数值来确认是否属于长按）
				uint16_t keyTemp				= KEY_NULL;		/*十六进制第一位：按键状态；	第二位：保持计数值；		第三位：键值；		第四位：连按计数值*/
	
	static	uint32_t osTick_last			= 0xffff0000;		//对osTick进行记录，用于与下一次osTick进行对比获取间隔（此间隔用于判断按键是否属于连续按下）
	
	keyTemp = keyScan();		//获取键值
	
	switch(s_u16KeyState){	//获取状态机状态
	
		case KEY_STATE_INIT:	//初始化状态
			
				if(orgKeyStatus->keyCTflg){	//检测上一次是否为连按
				
					if((osKernelSysTick() - osTick_last) > KEY_CONTINUE_PERIOD){	//上一次是连按则检测本次是否继续连按
					
						keyTemp	= s_u16LastKey & 0x00f0;	//本次不是连按，处理返回值为连按结束状态，同时连按标志位清零
						keyTemp |= KEY_CTOVER;
						orgKeyStatus->keyCTflg = 0;	
					}
				}
		
				if(KEY_NULL != keyTemp)s_u16KeyState = KEY_STATE_WOBBLE;	//检测到有按键，切换状态到抖动检测
				break;
		
		case KEY_STATE_WOBBLE:	//消抖状态检测
			
				s_u16KeyState = KEY_STATE_PRESS;	//确认按键，切换状态到短按检测
				break;
		
		case KEY_STATE_PRESS:	//短按状态检测
		
				if(KEY_NULL != keyTemp){		//按键是否弹起？
				
					s_u16LastKey 	= keyTemp;	//存储按键键值
					keyTemp 		  |= KEY_DOWN;	//按键状态确认为按下
					s_u16KeyState	= KEY_STATE_LONG;	//按键依然未弹起，切换状态到长按检测
				}else{
				
					s_u16KeyState	= KEY_STATE_INIT;	//检测为按键抖动，检测状态机返回初始化状态
				}
				break;
				
		case KEY_STATE_LONG:		//长按状态检测
				
				if(KEY_NULL != keyTemp){	//按键是否弹起？
					
					if(++s_u16KeyTimeCount > KEY_LONG_PERIOD){	//若按键依然未弹起，则根据长按时长进行计数继续确认是否为长按
					
						s_u16KeyTimeCount	= 0;			//确认长按，长按计数值清零
						orgKeyStatus->sKeyKeep				= 0;			//检测状态机切换状态前，对保持确认状态所需计数值提前清零，本计数值为长按后保持计数步长
						KeyKeepComfirm		= 0;			//检测状态机切换状态前，对保持确认状态所需计数值提前清零，本数值用于定义长按后多久进行保持计数
						keyTemp			  |= KEY_LONG;	//按键状态确认为长按
						s_u16KeyState		= KEY_STATE_KEEP;	//按键依然未弹起，切换状态到长按保持检测
						
						orgKeyStatus->keyOverFlg			= KEY_OVER_LONG;	//长按
					}else	orgKeyStatus->keyOverFlg	= KEY_OVER_SHORT;		//短按
				}else{
				
					s_u16KeyState	= KEY_STATE_RELEASE;	//检测确认为长按后按键弹起，切换状态到弹起检测
				}
				break;
				
		case KEY_STATE_KEEP:		//长按后保持状态检测
			
				if(KEY_NULL != keyTemp){		//按键是否弹起？
				
					if(++s_u16KeyTimeCount > KEY_KEEP_PERIOD){	//若按键依然未弹起，则根据长按时长进行计数继续确认是否为长按后继续保持
						
						s_u16KeyTimeCount	= 0;			//确认长按后继续保持，长按计数值清零
						if(KeyKeepComfirm < (KEY_COMFIRM + 3))KeyKeepComfirm++;			//检测是否到允许计数时刻
						if(orgKeyStatus->sKeyKeep < 15 && KeyKeepComfirm > KEY_COMFIRM)orgKeyStatus->sKeyKeep++; 	//检测到允许计数，执行长按后保持计数
						if(orgKeyStatus->sKeyKeep){		//检测到长按后保持计数值不为零，即确认按键状态为长按后继续保持，对返回值进行相应确认处理
						
							orgKeyStatus->keyOverFlg	 = KEY_OVER_KEEP;	//状态确认为长按后为保持
							keyTemp	|= orgKeyStatus->sKeyKeep << 8;		//保持计数数据左移8位放在十六进制keyTemp第二位
							keyTemp	|= KEY_KEEP;			//按键状态确认为长按后继续保持
						}		
					}
				}else{
				
					s_u16KeyState	= KEY_STATE_RELEASE;	//按键状态确认为长按后继续保持之后弹起，切换状态到弹起检测
				}
				break;
				
		case KEY_STATE_RELEASE:	//弹起状态检测
				
				s_u16LastKey |= KEY_UP;	//存储按键状态
				keyTemp		  = s_u16LastKey;	//按键状态确认为弹起
				s_u16KeyState = KEY_STATE_RECORD;	//切换状态到按键连按记录
				break;
		
		case KEY_STATE_RECORD:	//按键连按记录状态检测

				if((osKernelSysTick() - osTick_last) < KEY_CONTINUE_PERIOD){	//若两次按键弹起时间间隔小于规定值，则判断为连按
					
					orgKeyStatus->sKeyCount++;	//连按计数		
				}else{
					
					orgKeyStatus->sKeyCount = 0;	//连按断开，计数清零
				}
				
				if(orgKeyStatus->sKeyCount){		//若连按次数不为零，即确认为按键连按，对返回值进行相应处理
													
					orgKeyStatus->keyCTflg	= 1;	//打开连按标志
					keyTemp	= s_u16LastKey & 0x00f0;	//提取键值
					keyTemp	|=	KEY_CONTINUE;				//确认为按键连按
					if(orgKeyStatus->sKeyCount < 15)keyTemp += orgKeyStatus->sKeyCount;	//连按计数数据放在十六进制keyTemp第四位（最低位）
				}
				
				s_u16KeyState	= KEY_STATE_INIT;		//检测状态机返回初始状态
				osTick_last	 	= osKernelSysTick();	//记录osTick，留作下次连按检测对比
				break;
		
		default:break;
	}
	return keyTemp;	//返回按键状态和键值
}

/*按键初始化函数，按键状态缓存结构体，按键扫描函数，按键触发事件函数表，按键提示信息头*/
void key_Thread(	funKeyInit 		key_Init,		
					Obj_keyStatus 	*orgKeyStatus,
					funKeyScan 		key_Scan,		
					Obj_eventKey 	keyEvent,		
					const char 		*Tips_head	){
	
/***按键调试（串口1反馈调试信息）****/
	static uint16_t keyVal;						//按键状态事件
	static uint8_t	key_temp;					//按键键值缓冲
	static uint8_t	kCount;						//按键计数值变量，长按保持计数和连按计数使用同一个变量，因为两个状态不会同时发生
	static uint8_t	kCount_rec;			//历史计数值保存
	
	static osThreadId ID_Temp;					//当前进程ID缓存
	static osEvent evt;
	static uint8_t KEY_DEBUG_FLG = 0;

	const	 uint8_t	tipsLen = 80;		//Tips打印字符串长度
	static char	key_tempDisp;
	static char	kCountDisp;
	static char	kCount_recDisp;
	static char	tips[tipsLen];					//Tips字符串
	
	key_Init();

	for(;;){
		
		keyVal = getKey(orgKeyStatus,key_Scan);    //获取键值
		
		ID_Temp = osThreadGetId();
		evt = osSignalWait (KEY_DEBUG_OFF, 1);		 //获取Debug_log输出权限信号
		if (evt.value.signals == KEY_DEBUG_OFF){
		
			KEY_DEBUG_FLG = 0;
			osSignalClear(ID_Temp ,KEY_DEBUG_OFF);
		}else{
		
			evt = osSignalWait (KEY_DEBUG_ON, 1);
			if (evt.value.signals == KEY_DEBUG_ON){
			
				KEY_DEBUG_FLG = 1;
				osSignalClear(ID_Temp ,KEY_DEBUG_ON);
			}		
		}  
		
if(KEY_DEBUG_FLG){
	
		memset(tips,0,tipsLen*sizeof(char));	//每轮Tips打印后清空
		strcat(tips,"Tips:");						//Tips标识
		strcat(tips,Tips_head);
		strcat(tips,"-");
}
/*------------------------------------------------------------------------------------------------------------------------------*/		
		switch(keyVal & 0xf000){
		
			case KEY_LONG		:	
				
					key_temp = (uint8_t)((keyVal & 0x00f0) >> 4);
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/
					strcat(tips,"按键");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);
					strcat(tips,"长按\r\n");	
					Driver_USART1.Send(tips,strlen(tips));	
}/***/
					break;
					
			case KEY_KEEP		:
				
					kCount		= (uint8_t)((keyVal & 0x0f00) >> 8);  //获取计数值
					kCount_rec	= kCount;
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/					
					strcat(tips,"按键");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);
					strcat(tips,"长按保持，保持计数：");
					kCountDisp = kCount + '0';
					strcat(tips,(const char*)&kCountDisp);	
					strcat(tips,"\r\n");	
					Driver_USART1.Send(tips,strlen(tips));	
}/***/		
					break;
					
			case KEY_DOWN		:
				
					key_temp = (uint8_t)((keyVal & 0x00f0) >> 4);
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/				
					strcat(tips,"按键");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);	
					strcat(tips,"按下\r\n");	
					Driver_USART1.Send(tips,strlen(tips));
}/***/			
					break;
					
			case KEY_UP			:
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/			
					strcat(tips,"按键");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);	
}/***/						
					switch(orgKeyStatus->keyOverFlg){

							case KEY_OVER_SHORT		:	
								
								   if(keyEvent.funKeySHORT[key_temp])keyEvent.funKeySHORT[key_temp]();		//按键事件触发，先检测触发事件是否创建，没创建则不进行触发
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/								
									strcat(tips,"短按后弹起\r\n");	
									Driver_USART1.Send(tips,strlen(tips));
									orgKeyStatus->keyOverFlg = 0;
}/***/							
									break;

							case KEY_OVER_LONG		:
								
									if(keyEvent.funKeyLONG[key_temp])keyEvent.funKeyLONG[key_temp]();
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/								
									strcat(tips,"长按后弹起\r\n");	
									Driver_USART1.Send(tips,strlen(tips));
									orgKeyStatus->keyOverFlg = 0;
}/***/							
									break;

							case KEY_OVER_KEEP		:	
								
									if(keyEvent.funKeyKEEP[key_temp][kCount_rec])keyEvent.funKeyKEEP[key_temp][kCount_rec]();
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/									
									strcat(tips,"长按后保持");
									kCount_recDisp = kCount_rec + '0';
									strcat(tips,(const char*)&kCount_recDisp);
									strcat(tips,"次计数后结束\r\n");
									Driver_USART1.Send(tips,strlen(tips));
									kCount_rec = 0;
}/***/							
									break;			
							default:break;
						}
						break;
					
			case KEY_CONTINUE	:
				
					kCount 		= (uint8_t)((keyVal & 0x000f) >> 0);	//获取计数值
					kCount_rec	= kCount + 1;
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/	
					strcat(tips,"按键");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);	
					strcat(tips,"连按，连按计数：");	
					kCountDisp = kCount + '0';
					strcat(tips,(const char*)&kCountDisp);	
					strcat(tips,"\r\n");	
					Driver_USART1.Send(tips,strlen(tips));		
}/***/			
					break;
					
			case KEY_CTOVER	:
				
					if(keyEvent.funKeyCONTINUE[key_temp][kCount_rec])keyEvent.funKeyCONTINUE[key_temp][kCount_rec]();
if(KEY_DEBUG_FLG){/*Debug_log输出使能*/					
					strcat(tips,"按键");	
					key_tempDisp = key_temp + '0';
					strcat(tips,(const char*)&key_tempDisp);
					strcat(tips,"连按");
					kCount_recDisp = kCount_rec + '0';
					strcat(tips,(const char*)&kCount_recDisp);
					strcat(tips,"次后结束\r\n");
					Driver_USART1.Send(tips,strlen(tips));
}/***/	
					kCount_rec = 0;				
					break;
					
			default:break;
		}
		osDelay(KEY_TICK);
	}
}

void eventK23(void){
	
	;
}

void eventK24(void){

	;
}

void eventK25(void){

	switch(Moudle_GTA.Extension_ID){
	
		case MID_EXEC_DEVPWM:{
		
		}break;
		
		default:break;
	}
}
u8 key_scan(void)
{
	u16 i = 0;
	u8 buff=0;
		
		if(KEY1 == 0)
		{
			delay_ms(10);
			while(KEY1 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			
			if(i > 1 && i < 20)//单次按键
			{

				(LED1 = !LED1);
				Relay1 = !LED1;
				

			}
			else if(i > 145 && KEY2 == 0)//长按 3S
			{
			
					led2_status = led2_b_flash;
					osSignalSet(tid_USARTWireless_Thread, 0x03);
			}
		}
		else if(KEY2 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY2 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 10)//单次按键
			{

				LED2 = !LED2;
				 Relay2 = !Relay2;

			}
			else if(i > 145 && KEY1 == 0)//长按 3S
			{
			
					led2_status = led2_b_flash;
					osSignalSet(tid_USARTWireless_Thread, 0x03);
			}
		}
		
		else if(KEY3 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY3 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//单次按键
			{

				LED3 = !LED3;
				 Relay3 = !Relay3;

			}
			
		}
		
		else if(KEY4 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY4 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//单次按键
			{

				LED4 = !LED4;
				 Relay4 = !Relay4;

			}
			
		}
		else if(KEY5 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY5 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//单次按键
			{

				LED5 = !LED5;
				 Relay5 = !Relay5;

			}
			
		}
		else if(KEY6 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY6 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//单次按键
			{

				LED6 = !LED6;
				Relay6 = !LED6;

			}
			
		}
		else if(KEY7 == 0)
		{
			i = 0;
			delay_ms(10);
			while(KEY7 == 0 && i < 150 )
			{
				delay_ms(20);
				i++;
				}
			if(i > 1 && i < 80)//单次按键
			{

				LED7 = !LED7;
				Relay7 = !LED7;

			}
			
		}
		else if(KEY8 == 0)
		{
				i = 0;
				delay_ms(10);
				while(KEY8 == 0 && i < 150 )
				{
					delay_ms(20);
					i++;
					}
				if(i > 1 && i < 80)//单次按键
				{

					LED8 = !LED8;
					Relay8 = !LED8;
				}
			
		}
		buff =  Relay1;
		buff = (buff<<1)+Relay2;
		buff = (buff<<1)+Relay3;
 		buff = (buff<<1)+Relay4;
 		buff = (buff<<1)+Relay5;
 		buff = (buff<<1)+Relay6;
 		buff = (buff<<1)+Relay7;
 		buff = (buff<<1)+Relay8;
		
	 return buff;
}
/***按键监测主进程***/
void keyMboard_Thread(const void *argument){
	u8 relay_status=0;
	u8 relay_status_pre=0xff;
	unsigned int time_out_count = 0xffff;
	//临时使用按键程序
	while(1)
	{
		relay_status = key_scan();
		gb_databuff[0] = relay_status;

		
		if(relay_status_pre == relay_status)
		{
			
			;
		}
		else
		{
			gb_Exmod_key = true;
			
			osSignalSet(tid_USARTWireless_Thread, 0x03);
			relay_status_pre = relay_status;
		}

		if(!time_out_count--)
		{
			time_out_count = 0xffff;
			gb_Exmod_key = true;
			
			osSignalSet(tid_USARTWireless_Thread, 0x03);
		}

		delay_ms(20);
	}

	

}
	
void keyMboardActive(void){
	keyInit();
	tid_keyMboard_Thread = osThreadCreate(osThread(keyMboard_Thread),NULL);
}



