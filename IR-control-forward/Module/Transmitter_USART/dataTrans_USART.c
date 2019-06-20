#include <dataTrans_USART.h>//数据传输驱动进程函数，包括有wifi及zigbee；

extern ARM_DRIVER_USART Driver_USART4;

extern osThreadId tid_keyMboard_Thread;	//声明主板按键任务ID，便于传递信息调试使能信号

osThreadId tid_USARTWireless_Thread;
osThreadId tid_UsartRx_Thread;
uint8 gb_databuff[50];

static bool online = false;
static u16 sendcount = 0;
volatile bool gb_Exmod_key=false;
static bool RX_FLG = false; //有效数据获取标志
static uint8 recvbuff[100]={0};

osThreadDef(USARTWireless_Thread,osPriorityNormal,1,1024);
osThreadDef(UsartRx_Thread,osPriorityNormal,1,512);

const u8 dataTransFrameHead_size = 1;
const u8 dataTransFrameHead[dataTransFrameHead_size + 1] = {

	0x7f
};

const u8 dataTransFrameTail_size = 2;
const u8 dataTransFrameTail[dataTransFrameTail_size + 1] = {

	0x0D,0x0A
};
/*********************************************************************
 *  crc
 */
uint8_t TRAN_crc8(uint8_t *buf,uint8_t len)
{
	uint8_t crc;
	uint8_t i;

	crc = 0;

	while(len--) 
	{
		crc ^= *buf++;

		for(i = 0; i < 8; i++) 
		{
			if(crc & 0x01)
				crc = (crc >> 1) ^ 0x8C;
			else
				crc >>= 1;
		}
	}
	return crc;
}

void *memmem(void *start, unsigned int s_len, void *find,unsigned int f_len){
	
	char *p, *q;
	unsigned int len;
	p = start, q = find;
	len = 0;
	while((p - (char *)start + f_len) <= s_len){
			while(*p++ == *q++){
					len++;
					if(len == f_len)
							return(p - f_len);
			};
			q = find;
			len = 0;
	};
	return(NULL);
}

void USART2Wirless_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	/*Initialize the USART driver */
	Driver_USART4.Initialize(myUSART2_callback);
	/*Power up the USART peripheral */
	Driver_USART4.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART4.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART4.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART4.Control (ARM_USART_CONTROL_RX, 1);

//	Driver_USART4.Send("i'm usart2 for wireless datstransfor\r\n", 38);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);		//使能或者失能APB2外设时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 ;					//底板指示灯
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;			//最高输出速率50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;			//推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	MSGZigbee_rest = 1;
	
}


void myUSART2_callback(uint32_t event){

	uint32_t TX_mask;
	uint32_t RX_mask;
  	TX_mask =ARM_USART_EVENT_TRANSFER_COMPLETE |
	         ARM_USART_EVENT_SEND_COMPLETE     |
	         ARM_USART_EVENT_TX_COMPLETE; 
         	
     RX_mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
			   ARM_USART_EVENT_TRANSFER_COMPLETE |
			   ARM_USART_EVENT_RX_TIMEOUT;
  if (event & TX_mask) {
    /* Success: Wakeup Thread |
			   ARM_USART_EVENT_RX_TIMEOUT*/
    osSignalSet(tid_UsartRx_Thread, 0x01);//接收完成，发送完成signal
    osSignalSet(tid_USARTWireless_Thread, 0x01);
    
  }
  if (event & RX_mask) {
    /* Success: Wakeup Thread */
    osSignalSet(tid_UsartRx_Thread, 0x02);//接收完成，发送完成signal
    osSignalSet(tid_USARTWireless_Thread, 0x02);//接收完成，发送完成signal
  	}
}

bool ATCMD_INPUT(char *CMD,char *REPLY[2],u8 REPLY_LEN[2],u8 times,u16 timeDelay){
	
	const u8 dataLen = 100;
	u8 dataRXBUF[dataLen];
	u8 loop;
	u8 loopa;
	
	for(loop = 0;loop < times;loop ++){
	
		Driver_USART4.Send(CMD,strlen(CMD));
		osDelay(20);
		Driver_USART4.Receive(dataRXBUF,dataLen);
		osDelay(timeDelay);
		osDelay(20);
		
		for(loopa = 0;loopa < 2;loopa ++)if(memmem(dataRXBUF,dataLen,REPLY[loopa],REPLY_LEN[loopa]))return true;
	}return false;
}

void USART2Wireless_wifiESP8266Init(void){

	const u8 	InitCMDLen = 9;
	const u16 	timeTab_waitAnsr[InitCMDLen] = {
		
		100,
		100,
		100,
		100,
		100,
		5000,
		3000,
		200,
		200,
	};
	const char *wifiInit_reqCMD[InitCMDLen] = {
		"ATE0\r\n",
		"ATE0\r\n",
		"AT\r\n",
		"AT+CWMODE_DEF=1\r\n",
		"AT+CWDHCP_DEF=1,1\r\n",
		"AT+CWJAP_DEF=\"GTA2017\",\"88888888\"\r\n",
		"AT+CIPSTART=\"TCP\",\"10.2.8.54\",8085\r\n",		//台架一：31.26		台架二：31.27
		"AT+CIPMODE=1\r\n",
		"AT+CIPSEND\r\n"
	};
	const u8 REPLY_DATLENTAB[InitCMDLen][2] = {
		
		{2,2},
		{2,2},
		{2,2},
		{2,2},
		{2,2},
		{2,14},
		{2,6},
		{2,2},
		{1,1},
	};
	const char *wifiInit_REPLY[InitCMDLen][2] = {
		
		{"OK","OK"},
		{"OK","OK"},
		{"OK","OK"},
		{"OK","OK"},
		{"OK","OK"},
		{"OK","WIFI CONNECTED"},
		{"OK","CONNECT"},
		{"OK","OK"},
		{">",">"}
	};
	
	u8 loop;
	
	Driver_USART4.Send("+++",3);osDelay(100);
	Driver_USART4.Send("+++\r\n",5);osDelay(100);
	Driver_USART4.Send("+++",3);osDelay(100);
	Driver_USART4.Send("+++\r\n",5);osDelay(100);
	
	for(loop = 0;loop < InitCMDLen;loop ++)
		if(false == ATCMD_INPUT((char *)wifiInit_reqCMD[loop],
								(char **)wifiInit_REPLY[loop],
								(u8 *)REPLY_DATLENTAB[loop],
								3,
								timeTab_waitAnsr[loop])
								)loop = 0;
	
	beeps(6);
}

u8 Extension_IDCHG(u8 Addr_in){
	
	switch(Addr_in){
	
		case MID_EXEC_DEVPWM:	return 0x32;
			
		case MID_EXEC_CURTAIN:	return 0x31;
		
		case MID_EXEC_RELAYS:	return 0x30;
			
		case MID_EXEC_SPEAK:	return 0x24;
			
		case MID_SENSOR_FIRE:	return 0x11;
			
		case MID_SENSOR_PYRO:	return 0x12;
			
		case MID_SENSOR_SMOKE:	return 0x13;
			
		case MID_SENSOR_GAS:	return 0x14;
			
		case MID_SENSOR_TEMP:	return 0x22;
			
		case MID_SENSOR_LIGHT:	return 0x23;
			
		case MID_SENSOR_ANALOG:	return 0x21;
			
		case MID_EXEC_DEVIFR:	return 0x34;
			
		case MID_EXEC_SOURCE:	return 0x33;
			
		case MID_EGUARD:		return 0x0A;
		
		default:return 0;
	}
}

/*****************发送帧数据填装*****************/
//发送帧缓存，命令，模块地址，数据长度，核心数据包，核心数据包长
u16 dataTransFrameLoad_TX(u8 bufs[],u8 cmd,u8 Maddr,u8 dats[],u8 datslen){

	u16 memp;
	
	memp = 0;
	
	memcpy(&bufs[memp],dataTransFrameHead,dataTransFrameHead_size); //帧头填充
	memp += dataTransFrameHead_size;	//指针后推
	bufs[memp ++] = Moudle_GTA.Wirless_ID;
	bufs[memp ++] = cmd;
	bufs[memp ++] = Extension_IDCHG(Moudle_GTA.Extension_ID);
	bufs[memp ++] = datslen;
	memcpy(&bufs[memp],dats,datslen);
	memp += datslen;
	memcpy(&bufs[memp],dataTransFrameTail,dataTransFrameTail_size);
	memp += dataTransFrameTail_size;
	
	return memp;
}
void UsartRx_Thread(const void *argument	)
{
	uint8 recv_b[100]={0};

	delay_ms(200);
	MSGZigbee_rest = 0;
	delay_ms(10);
	MSGZigbee_rest = 1;
	
	
	
	while(1)
	{
		
		

		memset(recv_b,0,100);
		//osSignalWait(0x02, osWaitForever);
		
		Driver_USART4.Receive(recv_b,64);
		osSignalWait(0x02, osWaitForever);
		memcpy(recvbuff,recv_b,100);//放入缓存

		if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa  )
		{
			if( recvbuff[18] == 3)//查询CMD
			{
				if(recvbuff[19] == 0)
					online = false;
				else if(recvbuff[19] == 1)
					online = true;
			}
			else if( recvbuff[18] == 5)
			{
				RX_FLG = true;
				
			}

			
		}
		
		if(online)
				led2_status = led2_b;
			else
				led2_status = led2_g;
		
	}
	
}

//切换zigbee状态：入网 <----> 退网
LEDstatus zigbee_sw(void)
{
	uint8 sendbuff[100]={0};
	
	
	if(led2_status == led2_b_flash)
		{
			uint8 i = 0;
			for(;i < 200;i++ )
			{
				if (led2_status == led2_b_flash)
				{
					TRAN_D_struct joinTRAN;

					memset(sendbuff,0,100);
					

					memset(&joinTRAN,0,sizeof(joinTRAN));

					memset(sendbuff,0xAA,2);

					memset(sendbuff+17+2,0xBB,2);

					joinTRAN.TYPE_NUM = 0x03;//子模块stm32--->路由器zigbee

					joinTRAN.data_len = 2;

					sendbuff[2] = sendcount++;

					memcpy(sendbuff+3,&joinTRAN,sizeof(joinTRAN));
					
					if( online )//初始化查询，是否入网
					{
						sendbuff[17] = 0;
						sendbuff[18] = 2;
						
					}
					else 
					{
						sendbuff[17] = 0;
						sendbuff[18] = 1;
					}
					
					sendbuff[17+2+2] = TRAN_crc8(sendbuff,17+2+2);

					if( i == 3 || i == 20)//只发两次
					{
						Driver_USART4.Send(sendbuff,17+2+2+1);
						osSignalWait(0x01, osWaitForever);

							LED2_MSG_b = 1;
							delay_ms(500);
							LED2_MSG_b = 0;
					}

				
				}
				else if(led2_status == led2_b)
					return led2_status;
				else
					return led2_g;
				
				
				delay_ms(200);
				}
		}
	else
	{
		return led2_status;
	}

	
	return led2_g;
}
uint8 zigbee_updata(uint8 EXmodID)
{
	uint8 sendbuff[100]={0};
	
	TRAN_D_struct updataTRAN;

	memset(sendbuff,0,100);
	memset(&updataTRAN,0,sizeof(TRAN_D_struct));

	memset(sendbuff,0xAA,2);
	updataTRAN.TYPE_NUM = 0x03;//子模块stm32--->路由器zigbee
	sendbuff[2] = sendcount++;
	sendbuff[18] = 0x04;//CMD----->updata
	sendbuff[20] = EXmodID;
	switch (EXmodID)
		{
					
			case MID_EXEC_DEVIFR:{

						updataTRAN.data_len = 3+16;					
						memcpy(sendbuff+14+updataTRAN.data_len,gb_databuff, 3);//17+updataTRAN.data_len-1
						
					}break;			
			
			default:break;
		}

	
	//beeps(1);
	memcpy(sendbuff+3,&updataTRAN,sizeof(TRAN_D_struct));
	
	memset(sendbuff+17+updataTRAN.data_len,0xBB,2);

	sendbuff[19+updataTRAN.data_len] = TRAN_crc8(sendbuff,19+updataTRAN.data_len);//17+data_len+2
	Driver_USART4.Send(sendbuff,20+updataTRAN.data_len);//sendbuff,17+2+2+1
	osSignalWait(0x01, osWaitForever);


	
	
	
	return 0;
}

void USARTWireless_Thread(const void *argument){
	
//	osEvent  evt;
//    osStatus status;
//	
	
	
	const u8 frameDatatrans_totlen = 100;	//帧缓存限长
//	const u8 dats_BUFtemp_len = frameDatatrans_totlen - 20;	//核心数据包缓存限长
//	u8 dataTrans_TXBUF[frameDatatrans_totlen] = {0};  //发送帧缓存
	u8 dataTrans_RXBUF[frameDatatrans_totlen] = {0};	//接收帧缓存
//	u8 TXdats_BUFtemp[dats_BUFtemp_len] = {0};	//发送核心数据包缓存
//	u8 RXdats_BUFtemp[dats_BUFtemp_len] = {0};	//接收核心数据包缓存
//	u8 memp = 1;
	TRAN_D_struct recv_tranbuff;
	u8 datacount = 0;
	u8 datacount_last = 0;
	
	//osSignalWait(WIRLESS_THREAD_EN,osWaitForever);		//等待进程使能信号
	//osSignalClear(tid_USARTWireless_Thread,WIRLESS_THREAD_EN);
	

	for(;;){

		
		led2_status = zigbee_sw();
		
		if( online && gb_Exmod_key)//更新线程
		{	

			gb_Exmod_key = false;
			zigbee_updata(Moudle_GTA.Extension_ID);
		}
		
		
/*************↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓无线数据接收，处理推送至驱动级进程↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓*********************/
		if(RX_FLG){
			RX_FLG = false;


			//查找对比
			if(recvbuff[0] == 0xaa && recvbuff[1] == 0xaa)
			{
				uint8  crc=0xfe;
				memcpy(&recv_tranbuff,recvbuff+3,17);
				crc = TRAN_crc8(recvbuff,19+recv_tranbuff.data_len);//17+recv_tranbuff.data_len+2

				if(crc == recvbuff[19+recv_tranbuff.data_len])//17+recv_tranbuff.data_len+2
				{
					datacount = dataTrans_RXBUF[2];//存放包序号
					if(datacount != datacount_last)//过滤同包
					{
						datacount_last = datacount;
						
					}

				}
			}
			
			switch(Moudle_GTA.Extension_ID){	//数据接收
						case MID_EXEC_DEVIFR:{
						
								IFR_MEAS *mptr = NULL;
								
								do{mptr = (IFR_MEAS *)osPoolCAlloc(IFR_pool);}while(mptr == NULL);
								/*自定义数据处理↓↓↓↓↓↓↓↓↓↓↓↓*/
								
								mptr->Mod_addr = recvbuff[15+recv_tranbuff.data_len];
								mptr->VAL_KEY  = recvbuff[16+recv_tranbuff.data_len];
								
								osMessagePut(MsgBox_MTIFR, (uint32_t)mptr, osWaitForever);	//指令推送至模块驱动
								osDelay(100);
							}break; 					
						default:break;
					}
	}


//		LED2_MSG_r = 0;
//		delay_ms(500);
//		LED2_MSG_r = 1;
//		delay_ms(500);
		delay_ms(10);
}
}
void USART_WirelessInit(void){


	USART2Wirless_Init();
	

}

void wirelessThread_Active(uint8 status){
	
	if(status && led2_status == led2_r)
	{	
		led2_status = led2_g;
		USART_WirelessInit();
		tid_USARTWireless_Thread = osThreadCreate(osThread(USARTWireless_Thread),NULL);
		tid_UsartRx_Thread = osThreadCreate(osThread(UsartRx_Thread),NULL);
	}
	else if(status == 0)
	{
		led2_status = led2_r;
		osThreadTerminate(tid_USARTWireless_Thread);
		osThreadTerminate(tid_UsartRx_Thread);
	}
	
}
