#include "sourceCM.h"    //智能电表驱动进程函数；

extern ARM_DRIVER_USART Driver_USART1;		//设备驱动库串口一设备声明

extern osThreadId tid_USARTDebug_Thread;

 uint32_t	 gb_power_kwh;
 uint16_t	 gb_power_v;
 uint32_t	 gb_power_A;

//extern datsTransCMD_FLAG1;
	
//默认RS485 9600B

osThreadId tid_sourceCM_Thread;
osThreadDef(sourceCM_Thread,osPriorityNormal,1,512);
			 
osPoolId  sourceCM_pool;								 
osPoolDef(sourceCM_pool, 10, sourceCM_MEAS);                  // 内存池定义
osMessageQId  MsgBox_sourceCM;
osMessageQDef(MsgBox_sourceCM, 2, &sourceCM_MEAS);            // 消息队列定义，用于模块线程向无线通讯线程
osMessageQId  MsgBox_MTsourceCM;
osMessageQDef(MsgBox_MTsourceCM, 2, &sourceCM_MEAS);          // 消息队列定义,用于无线通讯线程向模块线程
osMessageQId  MsgBox_DPsourceCM;
osMessageQDef(MsgBox_DPsourceCM, 2, &sourceCM_MEAS);          // 消息队列定义，用于模块线程向显示模块线程

//接收缓存区 	
u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
	

void sourceCM_ioInit(void){//端口初始化

	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE );	                

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	
}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(unsigned char  *buf,unsigned char  len)
{
	
	RS485_TX_EN=1;			//设置为发送模式
	osDelay(1);
  	Driver_USART1.Send(buf,len);	//数据发送
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET){};//等待发送结束

	RS485_TX_EN=0;
	RS485_TX_EN=0;	
		
 
				//设置为接收模式	
}

void SOURCE_USARTInitCallback(uint32_t event){

	;
}

void USART1source_Init(uint16 booter){

	/*Initialize the USART driver */
	Driver_USART1.Initialize(SOURCE_USARTInitCallback);
	/*Power up the USART peripheral */
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_EVEN |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, booter);

	/* Enable Receiver and Transmitter lines */
	Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);

	
}

void sourceCM_Init(void){
	
	//debug_Init();
	//uart_init(9600);
	//debug_Init();
	sourceCM_ioInit();
	//sourceCM_ADCInit();
}

unsigned char ADD_CHECK_8b(u8 dats [ ], u8 length)
{
	u8 loop;
	u8	result = 0;
	
	for(loop = 0;loop < length;loop ++){
	
		result += dats[loop];
	}
	return result;
}

unsigned char empty_id(unsigned char *id)
{
	char i = 0;
	for ( ; i  < 6; i++)
		{
			if(id[i] != 0xAA)
				return 0;
		}
	return 1;
}

unsigned char read_data2id(unsigned char *data,unsigned char *id)
{
	unsigned char i = 4;
	unsigned char add = 0;
	unsigned char len = data[13]+14;
	if(data[4] != 0x68 )
			return 0;
	
	for( i = 4; i  < len; i++)//data[4]->data[len-2]的和校验
		{
			
			add += data[i];
			
		}
	if(add == data[len])
	{
		for ( i = 0; i  < 6; i++)
		{
			id[i] = data[i+5];
		}
		return 1;
	}
	else
	{
		return 0;
	}
}
unsigned char bcd_to_hex(unsigned char data)
{
    unsigned char temp;

    temp = ((data>>4)*10 + (data&0x0f));
    return temp;
}


void sourceCM_Thread(const void *argument){

	osEvent  evt;
    osStatus status;
	unsigned char temp_id[6] 	= {0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA };
	unsigned char read_id[12] 	= {0x68 ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0xAA ,0x68 ,0x13 ,0x00 ,0xDF ,0x16};
	//FE FE FE FE 68 94 41 03 26 12 18 68 93 06 C7 74 36 59 45 4B EB 16
	unsigned char strpower_kwh[4]	= {0x33,0x33,0x34,0x33};//电能指令，单位kwh,其地址码每位都是需要加0x33
	//FE FE FE FE 68 94 41 03 26 12 18 68 91 08 33 33 34 33 59 33 33 33 50 16 
	unsigned char strpower_v[4]	= {0x33,0x32,0x34,0x35};//电压指令，单位V
	//FE FE FE FE 68 94 41 03 26 12 18 68 91 0A 33 32 34 35 36 56 33 33 33 33 B9 16 
	unsigned char strpower_a[4] 	= {0x33,0x32,0x35,0x35};//电流指令，单位A
	//FE FE FE FE 68 94 41 03 26 12 18 68 91 0D 33 32 35 35 BB 34 33 33 33 33 33 33 33 B9 16 
	unsigned char send_buff[24]	= {0x68 ,
									0,0,0,0,0,0,
									0x68,
									0x11,0x04,
									0x00,0x00,0x00,0x00,
									0x00,
									0x16};
		
	const bool UPLOAD_MODE = false;	//1：数据变化时才上传 0：周期定时上传
	
	const uint8_t upldPeriod = 5;	//数据上传周期因数（UPLOAD_MODE = false 时有效）
	
	static char flag=0;
	uint8_t UPLDcnt = 0;
	bool UPLD_EN = false;
	const uint8_t dpSize = 40;
	const uint32_t dpPeriod = 2000;

	uint16 enble_count = 0;
	
	static uint32_t Pcnt = 0;
	char disp[dpSize];
	char len_b = 0;
	sourceCM_MEAS actuatorData ={0,0,0,0,0,0};	//本地输入量
	//static sourceCM_MEAS Data_temp   = {1};	//下行数据输入量同步对比缓存
	static sourceCM_MEAS Data_tempDP = {1};	//本地输入量显示数据对比缓存
	
	sourceCM_MEAS *mptr = NULL;
	sourceCM_MEAS *rptr = NULL;


	osThreadTerminate(tid_USARTDebug_Thread);   //关闭debug，串口外设互斥
	USART1source_Init(2400);
	
	for(;;){
			float p=actuatorData.power_p/100.0;
			float v=actuatorData.power_v/10.0;
			float i=actuatorData.power_i/1000.0;

			if(enble_count > 5)
			{
				enble_count = 0;
				Driver_USART1.Uninitialize();
				USART1source_Init(9600);
			}
				evt = osMessageGet(MsgBox_MTsourceCM, 100);
				if (evt.status == osEventMessage){		//等待消息指令
					
					rptr = evt.value.p;
					/*自定义本地线程接收数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
					if (rptr->source_addr == datsTransCMD_FLAG1)
					{
						actuatorData.Switch = rptr->Switch;
						//actuatorData.anaVal	= rptr->anaVal;
						flag=1;
		
						//Data_temp.Switch = actuatorData.Switch; 	//缓存区数据同步，避免下行数据回发
						//Data_temp.anaVal = actuatorData.anaVal;
					}
		
					do{status = osPoolFree(sourceCM_pool, rptr);}while(status != osOK); //内存释放
					rptr = NULL;
					
					//soceRELAY = actuatorData.Switch;//将actuatorData的值附给PA0
					
		/*执行完后立即发一次状态*/do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//无线数据传输消息推送
					mptr->power_p = actuatorData.power_p;
					mptr->power_v = actuatorData.power_v;
					mptr->power_i = actuatorData.power_i;
					osMessagePut(MsgBox_sourceCM, (uint32_t)mptr, 50);
				}	
				
				//actuatorData.anaVal = soceGet_Adc_Max(4,100);//清0，ADC
				
				if(!UPLOAD_MODE){	//选择上传触发模式
				
					if(UPLDcnt < upldPeriod)UPLDcnt ++;
					else{
					
						UPLDcnt = 0;
						UPLD_EN = true;
					}
				}
				
				if(UPLD_EN){
					
					UPLD_EN = false;
					gb_Exmod_key = true;

					
					memcpy(gb_databuff, &(p), 4);
						memcpy(gb_databuff+4, &(v), 4);
						memcpy(gb_databuff+8, &(i), 4);
						
				}
				
				if( Data_tempDP.power_p != actuatorData.power_p || 
					Data_tempDP.power_v != actuatorData.power_v || 
					Data_tempDP.power_i != actuatorData.power_i)					
					{

 						Data_tempDP.power_p = actuatorData.power_p;
 						Data_tempDP.power_v = actuatorData.power_v;
 						Data_tempDP.power_i = actuatorData.power_i;

						gb_Exmod_key = true;
						memcpy(gb_databuff, &(p), 4);
						memcpy(gb_databuff+4, &(v), 4);
						memcpy(gb_databuff+12, &(i), 4);
						
						do{mptr = (sourceCM_MEAS *)osPoolCAlloc(sourceCM_pool);}while(mptr == NULL);	//1.44寸液晶显示消息推送
						if(flag==1){mptr->source_addr = datsTransCMD_FLAG1;}
						
						mptr->power_p = actuatorData.power_p;
 						mptr->power_v = actuatorData.power_v;
 						mptr->power_i = actuatorData.power_i;
						osMessagePut(MsgBox_DPsourceCM, (uint32_t)mptr, 100);
						osDelay(10);
						enble_count = 0;
				   }
			
				if(Pcnt < dpPeriod){osDelay(10);Pcnt ++;enble_count++;}
				else{
				
					Pcnt = 0;
					memset(disp,0,dpSize * sizeof(char));
					sprintf(disp,"★-------------☆\n\r开关状态 : %d\n模拟量：%d\n\n\r", actuatorData.Switch,actuatorData.anaVal);			
					//Driver_USART1.Send(disp,strlen(disp));
					osDelay(20);
				}
				
				osDelay(10);


	//未获取ID
		
		if( empty_id(temp_id) == 1  )
		{
			RS485_Send_Data(read_id, 12);
			
			Driver_USART1.Receive(RS485_RX_BUF,64);		
			osDelay(300);
			
			if(read_data2id(RS485_RX_BUF, temp_id) == 1 )
			{
				memcpy(temp_id, 	RS485_RX_BUF+5 , 6);
				memcpy(send_buff+1, RS485_RX_BUF+5 , 6);//给ID到发送缓存
				
				}
			osDelay(500);
		}
		else //if(Pcnt == 200)//以获取ID
		{

			
			//获取电能
			memcpy(send_buff+10,  strpower_kwh , 4);			
			send_buff[14] = ADD_CHECK_8b(send_buff,14);
			RS485_Send_Data(send_buff, 16);			
			
			Driver_USART1.Receive(RS485_RX_BUF,64);	
			osDelay(300);
			//重启线程！
			if(Driver_USART1.GetRxCount() < 10)
				Exmod_rest = false;

			
			//Driver_USART5.Send(RS485_RX_BUF,64);
			len_b = RS485_RX_BUF[13]+16;
			osDelay(20);
			
			if(RS485_RX_BUF[len_b-2] == ADD_CHECK_8b(RS485_RX_BUF+4,len_b-6) 
				&& RS485_RX_BUF[18] >= 0x33 && RS485_RX_BUF[19] >= 0x33 && RS485_RX_BUF[20] >= 0x33)
			{
				actuatorData.power_p= 	bcd_to_hex(RS485_RX_BUF[18] - 0x33)+
										bcd_to_hex(RS485_RX_BUF[19] - 0x33)*100+
										bcd_to_hex(RS485_RX_BUF[20] - 0x33)*10000+
										bcd_to_hex(RS485_RX_BUF[21] - 0x33)*1000000;
				
				
			}
			
		
			
			
			//获取电压
			memcpy(send_buff+10,  strpower_v, 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,14);
			RS485_Send_Data(send_buff, 16);
			
			Driver_USART1.Receive(RS485_RX_BUF,64);	
			osDelay(300);
			
			len_b = RS485_RX_BUF[13]+16;
			if(RS485_RX_BUF[len_b-2] == ADD_CHECK_8b(RS485_RX_BUF+4,len_b-6) 
				&& RS485_RX_BUF[18] >= 0x33 && RS485_RX_BUF[19] >= 0x33 && RS485_RX_BUF[20] >= 0x33)
				{
				actuatorData.power_v =	bcd_to_hex(RS485_RX_BUF[18] - 0x33)+
										bcd_to_hex(RS485_RX_BUF[19] - 0x33)*100+
										bcd_to_hex(RS485_RX_BUF[20] - 0x33)*10000+
										bcd_to_hex(RS485_RX_BUF[21] - 0x33)*1000000+
										bcd_to_hex(RS485_RX_BUF[22] - 0x33)*100000000;
				
			}
			//获取电流
			memcpy(send_buff+10,  strpower_a , 4);
			send_buff[14] = ADD_CHECK_8b(send_buff,14);
			RS485_Send_Data(send_buff, 16);
			
			
			Driver_USART1.Receive(RS485_RX_BUF,64);	
			osDelay(500);
			
			len_b = RS485_RX_BUF[13]+16;
			if(RS485_RX_BUF[len_b-2] == ADD_CHECK_8b(RS485_RX_BUF+4,len_b-6) 
				&& RS485_RX_BUF[18] >= 0x33 && RS485_RX_BUF[19] >= 0x33 && RS485_RX_BUF[20] >= 0x33)
			{
				actuatorData.power_i =	bcd_to_hex(RS485_RX_BUF[18] - 0x33)+
										bcd_to_hex(RS485_RX_BUF[19] - 0x33)*100+
										bcd_to_hex(RS485_RX_BUF[20] - 0x33)*10000+
										bcd_to_hex(RS485_RX_BUF[21] - 0x33)*1000000+
										bcd_to_hex(RS485_RX_BUF[22] - 0x33)*100000000;
				
			}
		}
		//memset(RS485_RX_BUF, 0, 64);//可有可无，串口缓存里面始终都在
		delay_ms(200);


	}
}

void sourceCMThread_Active(void){

	static bool memInit_flg = false;
	
	if(!memInit_flg){
	
		sourceCM_pool   = osPoolCreate(osPool(sourceCM_pool));	//创建内存池
		MsgBox_sourceCM = osMessageCreate(osMessageQ(MsgBox_sourceCM), NULL);   //创建消息队列
		MsgBox_MTsourceCM = osMessageCreate(osMessageQ(MsgBox_MTsourceCM), NULL);//创建消息队列
		MsgBox_DPsourceCM = osMessageCreate(osMessageQ(MsgBox_DPsourceCM), NULL);//创建消息队列
		
		memInit_flg = true;
	}

	sourceCM_Init();
	tid_sourceCM_Thread = osThreadCreate(osThread(sourceCM_Thread),NULL);
}
