 
#include "global.h"
	/*
	引脚使用：
  LED:     LED_EEROR  PA12 ; 	
	         LED_COMM  PA15 ;
	
	LED_595_ST   PB0
	LED_595_SH   PB1
	LED_595_MR   PB10
	LED_595_DS   PB11
	
	BEEP: PB9
	
	串口：RS485      UART1   PA9 PA10    con_pin1  PA11
	   
	TYPE_SW1  PC13  
  TYPE_SW2  PC14  
	TYPE_SW3  PC15 
	
	ST_595    PB3
	SH_595    PB4
	MR_595    PB5
	DS_595    PB6
	 
	MODE_SW3    PA8
	MODE_SW4    PB7
	MODE_SW5    PB8
	
	
	ADDRX1_1=PA4
	ADDRX1_2=PA5
	ADDRX1_4=PA6
	ADDRX1_8=PA7
	
	ADDRX10_1=PB12
	ADDRX10_2=PB13
	ADDRX10_4=PB14
	ADDRX10_8=PB15	
	*/ 
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE);	 //使能PB端口时钟

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);  //使能AFIO功能的时钟(勿忘！)

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);   //进行重映射  //595相关脚


	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);   //进行重映射

	//LED
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12|GPIO_Pin_15;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);					  

	//beep
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_9   ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);				

	/*
	TYPE_SW1  PC13  
	TYPE_SW2  PC14  
	TYPE_SW3  PC15 
	*/
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15    ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 // 
	GPIO_Init(GPIOC, &GPIO_InitStructure);		

	/*ADDRX1_1=PA4
	ADDRX1_2=PA5
	ADDRX1_4=PA6
	ADDRX1_8=PA7*/

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4| GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7  ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 // 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	// #define  MODE_SW3       PAin(8)//
	// #define  MODE_SW4       PBin(7)//
	// #define  MODE_SW5       PBin(8)//
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 // 
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8|GPIO_Pin_7 ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 // 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	 

	/*ADDRX10_1=PB12
	ADDRX10_2=PB13
	ADDRX10_4=PB14
	ADDRX10_8=PB15*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12| GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15    ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_IPU; 		 // 
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

	//RS485 CON   PA11
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);		

	/*
	LED_595_ST   PB0
	LED_595_SH   PB1
	LED_595_MR   PB10
	LED_595_DS   PB11 */


	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0| GPIO_Pin_1| GPIO_Pin_10|GPIO_Pin_11 ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/*
	ST_595    PB3
	SH_595    PB4
	MR_595    PB5
	DS_595    PB6  */

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_3| GPIO_Pin_4| GPIO_Pin_5|GPIO_Pin_6 ;	  //  端口配置
	GPIO_InitStructure.GPIO_Mode =  GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);


	//595复位
	MR_595 = 0;  
	delay_ms(1);	
	LED_595_MR = 0; 
	MR_595 = 1;   
	LED_595_MR = 1;

	ST_595 = 1;
	delay_ms(1);
	LED_595_ST = 1;
	SH_595 = 1;
	LED_595_SH = 1;  

	RS485_CON1 = 0;// 

	BEEP = 0;  

	LED_EEROR = 1;
	LED_COMM = 0;
}
 
 
 

void BSP_init(void)
{	
	LED_Init();  
}

