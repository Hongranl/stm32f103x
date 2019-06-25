#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
 	

#define  RS485_con      PBout(1)// 

#define  LED_RFID       PBout(5) 

#define  BEEP_OUT       PCout(13) 

 
#define  ST_595         PBout(5)//
#define  SH_595         PBout(4)//
#define  MR_595         PBout(3)//
#define  DS_595         PBout(6)//
 

#define  K_ROW1         PAout(0)//
#define  K_ROW2         PAout(1)//
#define  K_ROW3         PAout(2)//
#define  K_ROW4         PAout(3)//

#define  K_COL1         PAin(4)//
#define  K_COL2         PAin(5)//
#define  K_COL3         PAin(6)//
#define  K_COL4         PAin(7)//

#define  RS485_LED      PAout(12)//


void   LED_Init(void);//≥ı ºªØ
void   BEEP_INIT(void) ; 
void   EXTI_START(void); 


#endif
