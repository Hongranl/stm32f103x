//             // CMSIS RTOS header file
#include "RSART_MCU.h"
/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
extern ARM_DRIVER_USART Driver_USART2;


osThreadId tid_Thread_usart_app;
osThreadDef(Thread_usart_app,osPriorityNormal,1,512);
			 
osPoolId  usart_app_pool;								 
osPoolDef(usart_app_pool, 2, sourceCM_MEAS);                  // 内存池定义
osMessageQId  usart_app_fireMS;
osMessageQDef(usart_app_fireMS, 2, &sourceCM_MEAS);            // 消息队列定义，用于模块进程向无线通讯进程

void USART2_callback(uint32_t event){

	;
}

void USART2_Init(void){
	
//	GPIO_InitTypeDef GPIO_InitStructure;

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
	
	Driver_USART2.Send("i'm usart2 for Dev485_OK\r\n", 26);
	osDelay(100);
	
} 
void Thread_usart_app(const void *argument)
{
//	const u8 frameDatatrans_totlen = 100;	//帧缓存限长
//	
//	u8 dataTrans_RXBUF[frameDatatrans_totlen] = {0};	//接收帧缓存
	
	for(;;){
		
//		datsDev485RX(dataTrans_RXBUF,frameDatatrans_totlen);	//接收指令长度数据
//		if(dataTrans_RXBUF[0]==0x0a)
//		{
//				con_485_TX;	
//				datsDev485TX("OK\r\n", 4);
//			
//				osDelay(100);
//				con_485_RX;
//				dataTrans_RXBUF[0]=0;			
//		}
	}
}

void Thread_USRAT_data(void){

	USART2_Init();	
	usart_app_pool   = osPoolCreate(osPool(usart_app_pool));	//创建内存池
	usart_app_fireMS = osMessageCreate(osMessageQ(usart_app_fireMS), NULL);   //创建消息队列
	tid_Thread_usart_app = osThreadCreate(osThread(Thread_usart_app),NULL);   //激活进程
}


