#include "System_init.h"

void System_init(void)
{
		bsp_delayinit();
		Relay_Init();	
		delay_us(50000);
		USART1_Init();
		USART2_Init();	
}
