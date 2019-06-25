#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 


#define  RS485_CON1       PBout(5)// 



#define  ST_595       PBout(7)//
#define  SH_595       PBout(8)//
#define  MR_595       PBout(9)//
#define  DS_595       PBout(6)//

 
#define  KEY1       PAin(4)//
#define  KEY2       PAin(7)//
#define  KEY3       PBin(0)//
#define  KEY4       PBin(1)//
#define  KEY5       PBin(12)//
#define  KEY6       PBin(13)//
#define  KEY7       PBin(14)//
#define  KEY8       PAin(8)//
#define  KEY9       PAin(11)//
#define  KEY10      PAin(12)//


#define  JOIN_KEY      PAin(15)//PAin(15)  2019.3.6 YQ

#define  JOIN_LED      PAout(6)//



void LED_Init(void);//≥ı ºªØ 
		 		
void BSP_init(void);

#endif
