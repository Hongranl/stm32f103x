#ifndef __LED_H
#define __LED_H	 
#include "sys.h"

//BEEP KEY LED

#define KEY2 PCin(8)//
#define KEY3 PCin(9)//

#define BEEP PBout(14)//

#define LED2 PDout(1)//
#define LED3 PDout(0)//








//MOD_LED MOD_REST MOD_CHECK

#define OK_LED1 PAout(0)// 
#define OK_LED2 PEout(5)// 
#define OK_LED3 PAout(11)//

#define REST1 PAout(1)// 
#define REST2 PEout(6)// 
#define REST3 PAout(12)//


#define Check1 PDin(10)// 
#define Check2 PDin(11)// 	
#define Check3 PDin(12)// 



void IO_Init(void);//≥ı ºªØ

		 				    
#endif
