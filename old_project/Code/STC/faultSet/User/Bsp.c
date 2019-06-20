#include "Bsp.h"

void ADDRGET_Init(void){		//硬件地址读取前初始化

	GPIO_InitTypeDef gpio_addr;
	
	gpio_addr.Mode = GPIO_PullUp;
	gpio_addr.Pin  = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	
	GPIO_Inilize(GPIO_P3,&gpio_addr);
}

u8 Localaddr_get(void){		//硬件地址读取

	u8 addr = 0;
	
	addr |= P3 >> 7;
	addr |= P3 >> 6;
	addr |= P3 >> 5;
	addr |= P3 >> 4;
	
	return addr;
}

u8 LocalBDType_get(void){	//设故板本地类型获取

	u8 type = 0;
	
	EEPROM_read_n(0,&type,1);
	return type;
}

void Bsp_Init(void){

	ADDRGET_Init(); 
	USART485Trans_Init();
	USARTDebug_Init();
	GPIO_SPI_Init();
	GPIO_BEEP_Init();
}