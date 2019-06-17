#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
 	
#define  LED1       PBout(12)//  

#define  LED2       PBout(13)// 

#define  BEEP       PBout(7)//  

#define  Zigbee_REST       PAout(1)// 
#define  Led_Zigbee_OK       PAout(0)// 



#define  RS485_CON1       PAout(8)//
#define  RS485_CON2       PAout(4)//

#define  AC_con          PBout(0)//
 
#define  AC_CHECK        PAin(5)//


void LED_Init(void);//≥ı ºªØ 
		 		
void BSP_init(void);

#endif
