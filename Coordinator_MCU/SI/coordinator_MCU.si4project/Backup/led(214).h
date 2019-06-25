#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
 	

#define  RS485_con      PAout(4)// 

 #define  LED_HW      PBout(13)//  
#define  LED_232       PBout(4) 
#define  LED_RFID       PBout(3) 

 

#define  IR_RX       PCin(6) 
#define  IR_TX       PCout(7)


#define  ZIGBEE_RST      PCout(12)// 


#define  JDQ_PIN        PBout(12)// 

#define  PC_STATUS     PBin(7)//  

#define  join_key     PBin(1)//


void   LED_Init(void);//≥ı ºªØ
void   BEEP_INIT(void) ; 
void   EXTI_START(void); 


#endif
