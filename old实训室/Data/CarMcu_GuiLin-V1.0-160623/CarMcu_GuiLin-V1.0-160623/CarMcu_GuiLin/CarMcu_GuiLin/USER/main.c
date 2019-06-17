#include "includes.h"
#include "LED.h"
#include "usart.h"
#include "delay.h"
#include "spi.h"
#include "lcd.h"
#include "cx.h"
#include "adc.h"
#include "MotorDriver.h"
#include "Follow.h"
#include "usart2.h"
#include "usart3.h"
#include "CarRoad.h" 
#include "Voice.h"

//************* 任务堆栈定义 *************************
#define START_TASK_PRIO      	4	 
#define START_STK_SIZE  		120
OS_STK START_TASK_STK[START_STK_SIZE];
void start_task(void *pdata);	

#define MOTOR_TASK_PRIO         5
#define MOTOR_STK_SIZE  		120
OS_STK MOTOR_TASK_STK[MOTOR_STK_SIZE];
void Motor_task(void *pdata);	

#define LCD_TASK_PRIO       	6
#define LCD_STK_SIZE  			120
OS_STK LCD_TASK_STK[LCD_STK_SIZE];
void Lcd_task(void *pdata);

#define UART_TASK_PRIO       	7
#define UART_STK_SIZE  			120
OS_STK UART_TASK_STK[UART_STK_SIZE];
void UartCom_task(void *pdata);


#define DIS_TASK_PRIO       	8
#define DIS_STK_SIZE  			120
OS_STK DIS_TASK_STK[DIS_STK_SIZE];
void Detect_task(void *pdata);


#define LED_TASK_PRIO       	9
#define LED_STK_SIZE  			120
OS_STK LED_TASK_STK[LED_STK_SIZE];
void Led_task(void *pdata);
//**********************************************************
void Delay(void)
{
	u32 i;
	 i= 0x7fffff;
	while(i--);
}

void RCC_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,  ENABLE);
}

int main(void)
{
	RCC_Config();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	
	uart1_init(9600);		// 语音模块
	uart2_init(19200);		//  wifi
	uart3_init(19200);		// rfid
	TIM3_Configuration(499,719);	// 1Khz的计数频率，计数到500为5ms  
	LED_Init(); 	
	Adc_Init();			// ADC 电压采样初始化
	SPI2_Init();		
	LCD_Init();			// LCD 初始化		 
	Motor_Init();			// 电机初始化
	SeverPWMInit();		// 舵机初始化
	CarRoadMsgInt();		// 小车路径初始化
	CarFollowInit();		// 寻迹初始化
	SpeedDet_Init();		// 速度检测初始化
	Traffic_PA1_Init();	//  交通检测初始化
  	Ultra_PC0_Init();		// 超声波初始化
	BEEP_ON();			// 上电beep 一声	
	Delay();
	BEEP_OFF();	
	
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );
	OSStart();
}
//----------------------------------------------------------------------------------------------------------
void start_task(void *pdata)
{
	pdata = pdata; 
	OSStatInit();		//初始化统计任务,这里会延时1秒左右	

	OSTaskCreate(Motor_task,(void *)0,(OS_STK*)&MOTOR_TASK_STK[MOTOR_STK_SIZE-1],MOTOR_TASK_PRIO);
	OSTaskCreate(UartCom_task,(void *)0,(OS_STK*)&UART_TASK_STK[UART_STK_SIZE-1],UART_TASK_PRIO);
	OSTaskCreate(Lcd_task,(void *)0,(OS_STK*)&LCD_TASK_STK[LCD_STK_SIZE-1],LCD_TASK_PRIO);	
	OSTaskCreate(Detect_task,(void *)0,(OS_STK*)&DIS_TASK_STK[DIS_STK_SIZE-1],DIS_TASK_PRIO);	
	OSTaskCreate(Led_task,(void *)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO);
	OSTaskSuspend(OS_PRIO_SELF);	
	
	while(1);
}
//----------------------------------------------------------------------------------------------------------
void Motor_task(void *pdata)
{
	while(1)
	{
		CarSpeedCtr();
		CarFollowCtr();
		OSTimeDlyHMSM(0,0,0,5);
	}
}


//----------------------------------------------------------------------------------------------------------
void UartCom_task(void *pdata)
{
	while(1)
	{
		Uart_PC_Handler();
		Uart_Rfid_Handler();
		OSTimeDlyHMSM(0,0,0,50);
	}
}
//----------------------------------------------------------------------------------------------------------
void Lcd_task(void *pdata)
{
	while(1)
	{
		LcdShowCarMsg();	
		OSTimeDlyHMSM(0,0,1,0);
	}
}
//-----------------------------------------------------------------------------------------------------------
void Detect_task(void *pdata)
{	
	while(1)
	{
		Ultra_Ranging();
		OSTimeDlyHMSM(0,0,0,50);//100
	}
}
//-------------------------------------------------------------------------------------
void Led_task(void *pdata)    //add 5-17
{
	while(1)
	{
		LED_Control();     
		OSTimeDlyHMSM(0,0,0,150);
	}
}



