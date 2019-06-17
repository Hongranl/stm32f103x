
//             // CMSIS RTOS header file
#include "USART_485_APP_data.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
extern ARM_DRIVER_USART Driver_USART2;

#define  datsDev485_2_TX	Driver_USART2.Send
#define  datsDev485_2_RX	Driver_USART2.Receive

void USART2_callback(uint32_t event){

	;
}

void USART2_Init(void){
	
		GPIO_InitTypeDef GPIO_InitStructure;

		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		/*Initialize the USART driver */
		Driver_USART2.Initialize(USART2_callback);
		/*Power up the USART peripheral */
		Driver_USART2.PowerControl(ARM_POWER_FULL);
		/*Configure the USART to 115200 Bits/sec */
		Driver_USART2.Control(ARM_USART_MODE_ASYNCHRONOUS |
													ARM_USART_DATA_BITS_8 |
													ARM_USART_PARITY_NONE |
													ARM_USART_STOP_BITS_1 |
													ARM_USART_FLOW_CONTROL_NONE, 9600);

		/* Enable Receiver and Transmitter lines */
		Driver_USART2.Control (ARM_USART_CONTROL_TX, 1);
		Driver_USART2.Control (ARM_USART_CONTROL_RX, 1);
		
		DIR_485_2_Send();
		datsDev485_2_TX("i'm usart1 for Dev485_APP_OK\r\n", 30);
		delay_us(50000);
	
} 

