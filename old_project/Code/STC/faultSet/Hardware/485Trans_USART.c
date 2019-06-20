#include "485Trans_USART.h"

extern u8 BD_addr,BD_type;	 //设故板地址及类型

const u8 flgCfmHead = FRAME_HEAD;				//帧头标记
const u8 frameCfmTail[2] = {0x0d,0x0a};		//帧尾死不变
u8 frameCfmHead[4] = {0};							//帧头由帧头标记、设故板专用通信地址、本地设故板地址、本地设故板类型组成

extern u8 xdata UART485_BUF[COM_RX2_Lenth];	//485串口数据缓存

u8 COREDATS[COREDATS_SIZE] = {0};		//帧核数据缓存

void USART485Trans_Init(void){	//485通讯接口初始化

	GPIO_InitTypeDef gpio_485;
	COMx_InitDefine  UART485Port;
	
	gpio_485.Mode = GPIO_OUT_PP;
	gpio_485.Pin  = GPIO_Pin_2;
	GPIO_Inilize(GPIO_P1,&gpio_485);
	
	UART485Port.UART_Mode = UART_8bit_BRTx;
	UART485Port.UART_BRT_Use = BRT_Timer2;
	UART485Port.UART_BaudRate = 38400UL;
	UART485Port.Morecommunicate = DISABLE;
	UART485Port.UART_RxEnable = ENABLE;
	UART485Port.BaudRateDouble = DISABLE;
	UART485Port.UART_Interrupt = ENABLE;
	UART485Port.UART_Polity = PolityHigh;
	UART485Port.UART_P_SW = UART2_SW_P10_P11; 
	UART485Port.UART_RXD_TXD_Short = DISABLE;
	USART_Configuration(USART2,&UART485Port);
	
	RD485; //初始化开启接受使能
}

void frameInsAccept(void){	//数据验收

	u8 loop,corelen;
	
	frameCfmHead[0] = flgCfmHead;	 //帧头填装
	frameCfmHead[1] = COM_ADDR;	  
	frameCfmHead[2] = BD_addr;
	frameCfmHead[3] = BD_type;
	
	for(loop = 0; loop < COM_RX2_Lenth; loop ++){
		
		if(!memcmp(frameCfmHead,&UART485_BUF[loop],2)){	//帧头标记及设故板专用通信地址校验/*第一层*/
	
			if(!memcmp(&frameCfmHead[2],&UART485_BUF[loop + 3],2)){ //本地设故板地址及本地设故板地址校验/*第二层*/  帧头标记后第三位开始
			
				corelen 	= UART485_BUF[loop + 2];	//帧头标记后第二位表示帧核长度
				if(!memcmp(frameCfmTail,&UART485_BUF[loop + corelen + 2],2)){	//帧尾校验，帧头标记 + 设故板专用通信地址1位 + 帧核长1位 + 下一位（越过当前位）/*第三层*/
				
					memset(COREDATS,0,COREDATS_SIZE * sizeof(u8));			//帧核缓存清零
					memcpy(COREDATS,&UART485_BUF[loop + 5],corelen - 2);	//帧头后第五位为去掉地址后帧核头字节，以此为开端复制帧核数据到缓存
					memset(&UART485_BUF[loop],0,loop + corelen + 5);		//历史数据废弃清零  5：帧头2 + 帧核长1 + 帧尾2
				}				
			}
		}
	}
}



