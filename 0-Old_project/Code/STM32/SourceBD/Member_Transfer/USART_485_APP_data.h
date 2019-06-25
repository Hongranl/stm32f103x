#ifndef _USART_485_APP_DATA_H_
#define _USART_485_APP_DATA_H_


#include "stm32f10x.h"
#include "delay.h"
#include "Driver_USART.h"
#include "IO_Map.h"

#define con485_2			PAout(4)

#define  datsDev485_2_TX	Driver_USART2.Send
#define  datsDev485_2_RX	Driver_USART2.Receive

#define  DIR_485_2_Receive()  con485_2 = 0;  
#define  DIR_485_2_Send()     con485_2 = 1;  

void USART2_Init(void);

#endif
