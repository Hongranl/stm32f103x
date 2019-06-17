 
#include "global.h"
 /*
	引脚使用：
	LED:   LED  PB9 ; 	
	
 
	
	串口：RS485 (接电压电流计)    UART1   PA9 PA10    con_pin1  PA8
	      RS485(通信)            UART2   PA2  PA3    con_pin2  PA4
	  
  AC_CHECK  PA5    220V是否接通进来

  AC_con   PA12    220V 继电器控制	 
*/	
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟

	//BEEP
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				

	//LED
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_12|GPIO_Pin_0;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  

	//
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_8|GPIO_Pin_4 ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);				

	RS485_CON2 = 0;
	RS485_CON1 = 0;//电压电流计  一直开启

	LED1 = 0;  
	LED2 = 0;  
	AC_con = 0; 
	BEEP = 0;
}
 
 
void EXTI9_5_IRQHandler(void)
{
	if( AC_CHECK == 0 )	 // 
	{
		POWER_220V_FLG = 1;
		LED1 =! LED1; 
	}		 
	EXTI_ClearITPendingBit(EXTI_Line5);  //清除LINE4上的中断标志位  
}


void BSP_init(void)
{
	EXTI_InitTypeDef  EXTI_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟

	//AC_CHECK
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IN_FLOATING; 		 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	//GPIOA.5	  中断线以及中断初始化配置 上升沿触发 PA5  WK_UP
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource5); 

	EXTI_InitStructure.EXTI_Line=EXTI_Line5;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器


	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键WK_UP所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 

	LED_Init();  
}
