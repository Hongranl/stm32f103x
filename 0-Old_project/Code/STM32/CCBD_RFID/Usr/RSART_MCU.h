#ifndef _RSARTMCU_H_
#define _RSARTMCU_H_

#include "stm32f10x.h"
#include "cmsis_os.h"  
#include "Driver_USART.h"
#include "IO_Map.h"

typedef struct{

	uint8_t source_addr;
	
}sourceCM_MEAS;

void USART2_Init(void);
void Thread_usart_app(const void *argument);
void Thread_USRAT_data(void);

#endif

