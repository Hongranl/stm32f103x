#ifndef _USART_485_VAW_DATA_H_
#define _USART_485_VAW_DATA_H_


#include "stm32f10x.h"
#include "delay.h"
#include "Driver_USART.h"
#include "IO_Map.h"

#define con485			PAout(8)

#define  datsDev485_1_TX	 Driver_USART1.Send
#define  datsDev485_1_RX	 Driver_USART1.Receive

#define  DIR_485_1_Receive()  con485 = 0;  
#define  DIR_485_1_Send()     con485 = 1;  

void USART1_Init(void);

#endif
