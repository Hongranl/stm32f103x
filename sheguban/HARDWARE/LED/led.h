#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
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
 	
#define  LED_EEROR       PAout(12)//  
#define  LED_COMM        PAout(15)// 

#define  BEEP            PBout(9)// 
  
#define  RS485_CON1      PAout(11)// 
 
 
#define  TYPE_SW1       PCin(13)//
#define  TYPE_SW2       PCin(14)//
#define  TYPE_SW3       PCin(15)//


#define  MODE_SW3       PAin(8)//
#define  MODE_SW4       PBin(7)//
#define  MODE_SW5       PBin(8)//


#define  ADDRX1_1       PAin(4)//
#define  ADDRX1_2       PAin(5)//
#define  ADDRX1_4       PAin(6)//
#define  ADDRX1_8       PAin(7)//

#define  ADDRX10_1       PBin(12)//
#define  ADDRX10_2       PBin(13)//
#define  ADDRX10_4       PBin(14)//
#define  ADDRX10_8       PBin(15)//


#define  ST_595       PBout(3)//
#define  SH_595       PBout(4)//
#define  MR_595       PBout(5)//
#define  DS_595       PBout(6)//

 
#define  LED_595_ST       PBout(0)//
#define  LED_595_SH       PBout(1)//
#define  LED_595_MR       PBout(10)//
#define  LED_595_DS       PBout(11)//

 


void LED_Init(void);//初始化 
		 		
void BSP_init(void);

#endif
