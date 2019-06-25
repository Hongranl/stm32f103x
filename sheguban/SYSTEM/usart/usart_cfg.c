#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"



Uart_struct    UART1_RS485_V_I_st; 
    
/*********************************************************************
 *  LOCAL  FUNC
 */
 void   uart1_init(u32 bound);	  //打印，调试  


//初始化IO 串口1 
//bound:波特率
void uart1_init(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA1传输

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA   ,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;									//波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;			//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;					//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;			//收发模式

	USART_Init(USART1, &USART_InitStructure); 									//初始化串口

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3 ;	//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);															//根据指定的参数初始化VIC寄存器


	NVIC_InitStructure.NVIC_IRQChannel =  DMA1_Channel4_IRQn; 	//一定要开启中断向量
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;		//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);															//根据指定的参数初始化VIC寄存器
	 
 
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);	// 
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);	// 
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);  		//传送 完成中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);  //串口1空闲中断 
	 
	 
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE); 		//使能串口1的DMA发送 接收  
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE); 


	MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 
	DMA_Cmd(UART1_DMA_TX_CH, DISABLE);		// 没数据发不要开启这条，一开启就会发出数据
	MYDMA_Config( UART1_DMA_RX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.RXcun,UART_RX_LEN,DMA_DIR_PeripheralSRC,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 
	DMA_Cmd(UART1_DMA_RX_CH, ENABLE);			// 
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC, ENABLE); //开DMA传送完中断 

	DMA_ClearFlag(DMA1_FLAG_GL4); 
	DMA_ClearFlag(DMA1_FLAG_GL5);    

	USART_Cmd(USART1, ENABLE);			//使能串口 

	memset((u8*)&UART1_RS485_V_I_st,FALSE,sizeof(UART1_RS485_V_I_st));	//标志位  全部为 FALSE

	UART1_RS485_V_I_st.ALL_tc_flg = TRUE;
}
 

