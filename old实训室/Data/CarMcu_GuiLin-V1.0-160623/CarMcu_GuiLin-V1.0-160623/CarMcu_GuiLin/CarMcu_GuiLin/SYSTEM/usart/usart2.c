#include "sys.h"
#include "usart2.h"	  
#include "includes.h"

u8 uart2_rx_buf[UART_PACKAGE_MAX+1][UART_RX_MAX_LEN+1];
u8 uart2_package_ptr;
u8 uart2_rx_ptr;


// wifi/zigbee
void uart2_init(u32 bound)
{
	u8 i;
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	

	USART_DeInit(USART2);  
	
	//USART1_TX   PA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure); //初始化PA2

	//USART1_RX	  PA.3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);  //初始化PA3


	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口
	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器

	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断
	USART_Cmd(USART2, ENABLE);                    //使能串口 
	

	uart2_package_ptr = 0;
	uart2_rx_ptr = 0;
	for(i=0; i<UART_PACKAGE_MAX; i++)
	{
		uart2_rx_buf[i][UART_RX_MAX_LEN] = 0x00;
	}	
}



void USART2_IRQHandler(void)                
{
	u8 rxtemp;
	u8 crc1,crc2;
	u16 crc;
	
	
	OSIntEnter();
	
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) 
	{
		rxtemp =USART_ReceiveData(USART2);	
		
		if(uart2_rx_ptr==0)
		{
			if(rxtemp == 0xAA)      // PC下行命令aa  55   
			{
				uart2_rx_buf[uart2_package_ptr][uart2_rx_ptr++] = rxtemp;  
			}
		}
		else if(uart2_rx_ptr==1)
		{
			if((uart2_rx_buf[uart2_package_ptr][0] == 0xAA)&&(rxtemp == 0x55))
			{
				uart2_rx_buf[uart2_package_ptr][uart2_rx_ptr++] = rxtemp;   
			}
			else
			{
				uart2_rx_ptr = 0;
			} 
		}
		else
		{
			uart2_rx_buf[uart2_package_ptr][uart2_rx_ptr++] = rxtemp; 
						
			if(uart2_rx_ptr >= uart2_rx_buf[uart2_package_ptr][3]+6)
			{
				crc = Crc16Code(&uart2_rx_buf[uart2_package_ptr][2],uart2_rx_buf[uart2_package_ptr][3]+2);  // crc       
	                    	crc1 =  crc;
	                    	crc2 =  crc>>8;
	                     	if((crc1 == uart2_rx_buf[uart2_package_ptr][uart2_rx_ptr-2]) &&(crc2 == uart2_rx_buf[uart2_package_ptr][uart2_rx_ptr-1]))
	                     	{
	                     		uart2_rx_buf[uart2_package_ptr][UART_RX_MAX_LEN] = uart2_rx_ptr;
					uart2_package_ptr++;
				} 
				uart2_rx_ptr = 0;
			}           							
		}

		if(uart2_rx_ptr > UART_RX_MAX_LEN)uart2_rx_ptr = 0;
		if(uart2_package_ptr > UART_PACKAGE_MAX)uart2_package_ptr=0;	
	} 

	OSIntExit();
}


void uart2_send_buf(u8 * string, u16 len)
{
	u16 i;
	
	for(i=0;i<len;i++)
	{
		while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET);
		USART_SendData(USART2, *string++);	
	}
}




