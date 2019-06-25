#include "led.h"
#include "delay.h"
 
#include  "global.h"
 
 
 
 
//初始化PB5和PE5为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟

 
	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //使能AFIO功能的时钟(勿忘！)
		
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);   //进行重映射
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //进行重映射
	

 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_1| GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5| GPIO_Pin_6;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					  
 
  MR_595=0;
  MR_595=1;


 	RS485_con=0; //485
	RS485_con=0; //485
	
	
}
 

void  BEEP_INIT(void) 
{
	
  
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB端口时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟
  
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_15;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOC, &GPIO_InitStructure);					  

 
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
  BEEP_OUT=0;
  LED_RFID=0;

  RS485_LED=0;

	
}	

 




