#include "dataManager.h"

extern osMessageQId MsgBoxID_SBD_UTZigB;
extern osMessageQId MsgBoxID_SBD_UT485;

osThreadId tid_com1ToZigbee_Thread;
osThreadId tid_com3ToDev485_Thread;

osThreadDef(com1ToZigbee_Thread,osPriorityNormal,1,1024);
osThreadDef(com3ToDev485_Thread,osPriorityNormal,1,1024);

static void USART1_callback(uint32_t event){

	;
}

static void USART3_callback(uint32_t event){

	;
}

static void USART1_toZigbee_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

    /*Initialize the USART driver */
    Driver_USART1.Initialize(USART1_callback);
    /*Power up the USART peripheral */
    Driver_USART1.PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1 |
                          ARM_USART_FLOW_CONTROL_NONE, 115200);

    /* Enable Receiver and Transmitter lines */
    Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
    Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);
	
    Driver_USART1.Send("i'm usart1 for Zigebee\r\n", 25);
}

static void USART3_toDev485_Init(void){

    /*Initialize the USART driver */
    Driver_USART3.Initialize(USART3_callback);
    /*Power up the USART peripheral */
    Driver_USART3.PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    Driver_USART3.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1 |
                          ARM_USART_FLOW_CONTROL_NONE, 115200);

    /* Enable Receiver and Transmitter lines */
    Driver_USART3.Control (ARM_USART_CONTROL_TX, 1);
    Driver_USART3.Control (ARM_USART_CONTROL_RX, 1);
	
	con485 = 1;
	
    Driver_USART3.Send("i'm usart3 for Dev485\r\n", 22);
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

int memloc(u8 str2[],u8 num_s2,u8 str1[],u8 num_s1)
{
	int la = num_s1;
	int i, j;
	int lb = num_s2;
	for(i = 0; i < lb; i ++)
	{
		for(j = 0; j < la && i + j < lb && str1[j] == str2[i + j]; j ++);
		if(j == la)return i;
	}
	return -1;
}

u8 XOR_CHECK(u8 buf[],u8 length){

	u8 loop = 0;
	u8 valXOR = buf[0];
	
	for(loop = 1;loop < length;loop ++)valXOR ^= buf[loop];
	
	return valXOR;
}

u8 ZigB_TXFrameLoad(u8 frame[],u8 cmd[],u8 cmdLen,u8 dats[],u8 datsLen){		

	const u8 frameHead = DTZIGB_FRAME_HEAD;	//ZNP,SOF帧头
	u8 xor_check = datsLen;					//异或校验，帧尾
	u8 loop = 0;
	u8 ptr = 0;
	
	frame[ptr ++] = frameHead;
	frame[ptr ++] = datsLen;
	
	memcpy(&frame[ptr],cmd,cmdLen);
	ptr += cmdLen;
	for(loop = 0;loop < cmdLen;loop ++)xor_check ^= cmd[loop];

	memcpy(&frame[ptr],dats,datsLen);
	ptr += datsLen;
	for(loop = 0;loop < datsLen;loop ++)xor_check ^= dats[loop];	
	
	frame[ptr ++] = xor_check;
	
	return ptr;
}

bool ZigB_datsRX(SourceBD_MEAS *data_RX,u8 recvTime){

	const u8 datsRXLen = 100;
	u8 datsRX_BUF[datsRXLen] = {0};
	
	const u8 cmdRX_Num = 2;
	const u8 cmdRX_Len = 2;
	const u8 cmdRX[cmdRX_Num][cmdRX_Len] = {	//指令表
	
		{0x0A,0x0A},	//指令0：	继电器数据传输
		{0x0A,0x0B},	//指令1:	待定
	};
	
	u8 *ptr = NULL;
	u8 loop = 0;
	
	static ARM_DRIVER_USART *USARTdrv = &Driver_USART1;
	
	USARTdrv->Receive(datsRX_BUF,datsRXLen);
	osDelay(recvTime);
	USARTdrv->Control(ARM_USART_ABORT_RECEIVE,0);
	
	for(loop = 0;loop < cmdRX_Num;loop ++){
	
		ptr = memmem(datsRX_BUF,datsRXLen,(u8*)cmdRX[loop],cmdRX_Len);
		
		if(ptr != NULL){
	
			switch(loop){
			
				case 0:{
				
					u8 XOR_VAL = *(ptr + *(ptr - 1) + 2);
					
					if(DTZIGB_FRAME_HEAD == *(ptr - 2) &&				//帧头校验
					   XOR_VAL == XOR_CHECK(ptr - 1,*(ptr - 1) + 3) &&	//帧尾异或校验
					   *(ptr + cmdRX_Len + 0) == SBD_LocalAddr_main &&	//本地地址校验
					   *(ptr + cmdRX_Len + 1) == SBO_LocalAddr_ext){	//本地扩展地址校验
					
						data_RX->role 		= roleUTZigBDNLoad;
						data_RX->datsType 	= datsRelayCon;
						memcpy(data_RX->dats.datsRelayCon.conArryDats,ptr + cmdRX_Len + 2,2);
						   
						return true;
					}else{
					
						datsRX_BUF[memloc((u8 *)datsRX_BUF,datsRXLen,(u8 *)cmdRX[loop],2)] = 0xFF;  //非指定格式则主动污染本段使其不同，继续查询本段后部
						loop --;	//原段信息向后复查
					}
				}break;
				
				case 1:{
				
					
				}break;
				
				default:break;
			}
		}
	}
	
	return false;
}

bool ZigB_datsTX(u8		UART_OBJ,
				 u8		cmd[2],	
				 u8		datsTX[],
				 u8 	datsTXLen,
				 bool	responseIF){
					
	const u8 ASR_datsDstLen = 3;
	const u8 ASR_datsDst[ASR_datsDstLen] = {0x03,0x02,0x03};	//远端接受确认，响应返回数据

	const uint8_t datsTransLen = 100;
	uint8_t buf_datsTX[datsTransLen] = {0};
	uint8_t buf_datsRX[datsTransLen] = {0};
	uint8_t datsRX_Len = 0;
	
	buf_datsRX[0] = DTZIGB_FRAME_HEAD;
	
	datsRX_Len = ZigB_TXFrameLoad(buf_datsTX,cmd,2,datsTX,datsTXLen);
	datsZigbeeTX(buf_datsTX,datsRX_Len);
	osDelay(20);
	
//	if(responseIF){		//是否接收确认响应
//	
//		datsZigbeeRX(buf_datsRX,datsTransLen);
//	}
}

static void com1ToZigbee_Thread(const void *argument){
	
	SourceBD_MEAS *local_datsRX = (SourceBD_MEAS *) osPoolAlloc(memid_SourceBD_pool);	//数据接收缓存
	
	for(;;){
		
		if(true == ZigB_datsRX(local_datsRX,50) && local_datsRX->role == roleUTZigBDNLoad){
		
			switch(local_datsRX->datsType){
			
				case datsRelayCon:{
				
					SourceBD_MEAS *mptr = (SourceBD_MEAS *) osPoolAlloc(memid_SourceBD_pool);
					
					mptr->role 		= roleUTZigBDNLoad;
					mptr->datsType 	= datsRelayCon;
					memcpy(mptr->dats.datsRelayCon.conArryDats,local_datsRX->dats.datsRelayCon.conArryDats,2);
					
					osMessagePut(MsgBoxID_SBD_Relay, (uint32_t)mptr, 100);
				}break;
				
				default:break;
			}
		}
	
//		datsZigbeeTX("abc",3);
//		osDelay(1000);
	}
}

static void com3ToDev485_Thread(const void *argument){

	for(;;){
	
		datsDev485TX("abc",3);
		osDelay(1000);
	}
}

void communicationActive(uint8_t comObj){

	switch(comObj){
		
		case comObj_Dev485:		USART1_toZigbee_Init();
								tid_com1ToZigbee_Thread = osThreadCreate(osThread(com1ToZigbee_Thread),NULL);
								break;
				
		case comObj_Zigbee:		USART3_toDev485_Init();
								tid_com3ToDev485_Thread = osThreadCreate(osThread(com3ToDev485_Thread),NULL);
								break;
		
				   default:		break;
	}	
}



