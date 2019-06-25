 
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

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //使能AFIO功能的时钟(勿忘！)
		
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_NoJTRST, ENABLE);   //进行重映射
	 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //进行重映射
	
	
 // RSR485_CON
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_5;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					  

	 // LED  PA6
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
	
	//key
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_11|GPIO_Pin_12 |GPIO_Pin_15  ;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 //上拉输入
 GPIO_Init(GPIOA, &GPIO_InitStructure);				
	
 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14   ;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 //上拉输入
 GPIO_Init(GPIOB, &GPIO_InitStructure);		 

/* 
	ST_595    PB7
	SH_595    PB8
	MR_595    PB9
	DS_595    PB6  */

 GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6| GPIO_Pin_7| GPIO_Pin_8|GPIO_Pin_9 ;	  //  端口配置
 GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);

   
 MR_595 = 0;
 MR_595 = 1;
   
 JOIN_LED = 1;
 
}
  
 

void BSP_init(void)
{
	LED_Init();  
}

