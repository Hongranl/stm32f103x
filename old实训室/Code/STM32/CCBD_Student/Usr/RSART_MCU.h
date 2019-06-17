#ifndef _RSARTMCU_H_
#define _RSARTMCU_H_

#include "stm32f10x.h"
#include "cmsis_os.h"  
#include "Driver_USART.h"
#include "IO_Map.h"

#define con485			PBout(1)

#define datsDev485TX	Driver_USART3.Send
#define datsDev485RX	Driver_USART3.Receive
//#define  DIR485_Receive()    GPIO_ResetBits(GPIOB,GPIO_Pin_1)
//#define  DIR485_Send()       GPIO_SetBits(GPIOB,GPIO_Pin_1) 
typedef struct{

	uint8_t source_addr;
	
}sourceCM_MEAS;

void USART3_Init(void);
void Thread_usart_app(const void *argument);
void Thread_USRAT_data(void);

#endif

