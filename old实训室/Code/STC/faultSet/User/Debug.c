#include "Debug.h"

void USARTDebug_Init(void){	//485通讯接口初始化

	COMx_InitDefine  UARTdebugPort;
	
	UARTdebugPort.UART_Mode = UART_8bit_BRTx;
	UARTdebugPort.UART_BRT_Use = BRT_Timer1;
	UARTdebugPort.UART_BaudRate = 115200UL;
	UARTdebugPort.Morecommunicate = DISABLE;
	UARTdebugPort.UART_RxEnable = ENABLE;
	UARTdebugPort.BaudRateDouble = DISABLE;
	UARTdebugPort.UART_Interrupt = ENABLE;
	UARTdebugPort.UART_Polity = PolityLow;
	UARTdebugPort.UART_P_SW = UART1_SW_P30_P31;
	UARTdebugPort.UART_RXD_TXD_Short = DISABLE;
	USART_Configuration(USART2,&UARTdebugPort);
}