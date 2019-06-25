#ifndef _USART_COMMUNICATION_H_
#define _USART_COMMUNICATION_H_

#include "USART_485_VAW_data.h"
#include "USART_485_APP_data.h"
#include "Driver_USART.h"
#include "Relay_CM.h"

extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART2;

void USART_Communication_485(void);

#endif
