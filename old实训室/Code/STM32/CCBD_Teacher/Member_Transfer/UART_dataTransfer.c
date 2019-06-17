#include "UART_dataTransfer.h"

extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART2;
extern ARM_DRIVER_USART Driver_USART3;
extern ARM_DRIVER_USART Driver_USART5;

osThreadId tid_com1ToAndroid_Thread;
osThreadId tid_com2ToDev485_Thread;
osThreadId tid_com3ToRFID_Thread;
osThreadId tid_com5ToZigbee_Thread;

osThreadDef(com1ToAndroid_Thread,osPriorityNormal,	1,	1024);
osThreadDef(com2ToDev485_Thread,osPriorityNormal,	1,	1024);
osThreadDef(com3ToRFID_Thread,osPriorityNormal,		1,	1024);
osThreadDef(com5ToZigbee_Thread,osPriorityNormal,	1,	1024);

void USART1_callback(uint32_t event){

	;
}

void USART2_callback(uint32_t event){

	;
}

void USART3_callback(uint32_t event){

	;
}

void USART5_callback(uint32_t event){

	;
}

void USART1_toAndroid_Init(void){

    /*Initialize the USART driver */
    Driver_USART1.Initialize(USART1_callback);
    /*Power up the USART peripheral */
    Driver_USART1.PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1 |
                          ARM_USART_FLOW_CONTROL_NONE, 115200);

    /* Enable Receiver and Transmitter lines */
    Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
    Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);
	
    Driver_USART1.Send("i'm usart1 for Android\r\n", 25);
}

void USART2_toDev485_Init(void){
	
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
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
                          ARM_USART_FLOW_CONTROL_NONE, 115200);

    /* Enable Receiver and Transmitter lines */
    Driver_USART2.Control (ARM_USART_CONTROL_TX, 1);
    Driver_USART2.Control (ARM_USART_CONTROL_RX, 1);
	
	con485 = 1;
	
    Driver_USART2.Send("i'm usart2 for Dev485\r\n", 24);
}

void USART3_toRFID_Init(void){

    /*Initialize the USART driver */
    Driver_USART3.Initialize(USART3_callback);
    /*Power up the USART peripheral */
    Driver_USART3.PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    Driver_USART3.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1 |
                          ARM_USART_FLOW_CONTROL_NONE, 19200);

    /* Enable Receiver and Transmitter lines */
    Driver_USART3.Control (ARM_USART_CONTROL_TX, 1);
    Driver_USART3.Control (ARM_USART_CONTROL_RX, 1);
	
    Driver_USART3.Send("i'm usart3 for RFID\r\n", 22);
}

void USART5_toZigbee_Init(void){

    /*Initialize the USART driver */
    Driver_USART5.Initialize(USART5_callback);
    /*Power up the USART peripheral */
    Driver_USART5.PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    Driver_USART5.Control(ARM_USART_MODE_ASYNCHRONOUS |
                          ARM_USART_DATA_BITS_8 |
                          ARM_USART_PARITY_NONE |
                          ARM_USART_STOP_BITS_1 |
                          ARM_USART_FLOW_CONTROL_NONE, 115200);

    /* Enable Receiver and Transmitter lines */
    Driver_USART5.Control (ARM_USART_CONTROL_TX, 1);
    Driver_USART5.Control (ARM_USART_CONTROL_RX, 1);
	
    Driver_USART5.Send("i'm usart5 for Zigbee\r\n", 24);
}

void com1ToAndroid_Thread(const void *argument){

	const	uint8_t frame_head = 0x7e;
	const uint8_t frame_tail[2] = {0x0d,0x0a};
	uint8_t dats_type;
	uint8_t dats_length;
	uint8_t FRAMEBUF_Android[FRAME_SIZEAndrod];
	uint8_t COREDATS_Android[COREDATS_SIZEAndrod];
	uint8_t DATSBUF_TX[FRAME_SIZEAndrod];
	
	for(;;){
	
		datsAndroidTX("adr",3);
		osDelay(1000);
	}
}

void com2ToDev485_Thread(const void *argument){
	
	const uint8_t frame_head = 0x7e;
	const uint8_t frame_tail[2] = {0x0d,0x0a};
	uint8_t dev_addr;
	uint8_t dats_length;
	uint8_t FRAMEBUF_Dev485[FRAME_SIZEDev485];
	uint8_t COREDATS_Dev485[COREDATS_SIZEDev485];
	uint8_t DATSBUF_TX[FRAME_SIZEAndrod];
	
	for(;;){
	
		datsDev485TX("485",3);
		osDelay(1000);
	}
}

void com3ToRFID_Thread(const void *argument){
	
	const uint8_t frame_head[2] = {0x55,0xaa};
	uint8_t frame_tail[2];
	uint8_t FRAMEBUF_RFID[FRAME_SIZERFID];
	uint8_t COREDATS_RFID[COREDATS_SIZERFID];
	uint8_t DATSBUF_TX[FRAME_SIZEAndrod];
	
	for(;;){
	
		memset(FRAMEBUF_RFID,0,FRAME_SIZERFID * sizeof(uint8_t));
		osDelay(10);
		datsRFIDRX(FRAMEBUF_RFID,20);
		osDelay(20);
		datsAndroidTX(FRAMEBUF_RFID,strlen((const char *)FRAMEBUF_RFID));
		osDelay(20);
	}
}

void com5ToZigbee_Thread(const void *argument){

//	const uint8_t frame_head
//	const uint8_t frame_tail
	uint8_t FRAMEBUF_Zigbee[FRAME_SIZEZigbee];
	uint8_t COREDATS_Zigbee[COREDATS_SIZEZigbee];
	uint8_t DATSBUF_TX[FRAME_SIZEAndrod];
	
	for(;;){
	
		datsZigbeeTX("zig",3);
		osDelay(1000);
	}
}

void communicationActive(uint8_t comObj){

	switch(comObj){
	
		case comObj_Androd:		USART1_toAndroid_Init();
								tid_com1ToAndroid_Thread = osThreadCreate(osThread(com1ToAndroid_Thread),NULL);
								break;
		
		case comObj_Dev485:		USART2_toDev485_Init();
								tid_com2ToDev485_Thread = osThreadCreate(osThread(com2ToDev485_Thread),NULL);
								break;
				
		case comObj_DevRFID:	USART3_toRFID_Init();
								tid_com3ToRFID_Thread = osThreadCreate(osThread(com3ToRFID_Thread),NULL);
								break;
				
		case comObj_Zigbee:		USART5_toZigbee_Init();
								tid_com5ToZigbee_Thread = osThreadCreate(osThread(com5ToZigbee_Thread),NULL);
								break;
		
				   default:		break;
	}	
}
