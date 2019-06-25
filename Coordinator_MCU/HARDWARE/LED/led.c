#include "led.h"
#include "delay.h"
 
#include  "global.h"
 
 
 
 
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB端口时钟


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //使能AFIO功能的时钟(勿忘！)

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);   //进行重映射
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //进行重映射


	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1|  GPIO_Pin_5|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_12;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOC, &GPIO_InitStructure);			 


	ZIGBEE_RST = 1;

	LED_232 = 0;
	LED_RFID = 0;
	
}
 

void  BEEP_INIT(void) 
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_13;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);					  


	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	JDQ_PIN = 0;

	LED_HW = 1;

	RS485_con = 0; //485
	RS485_con = 0; //485
}	


void  EXTI_START(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB端口时钟

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		  
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	IR_TX = 0;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource6);

	EXTI_InitStructure.EXTI_Line = EXTI_Line6;	//KEY2
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	 	//根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//使能按键WK_UP所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1;	//抢占优先级2， 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x1;					//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;								//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 

}


