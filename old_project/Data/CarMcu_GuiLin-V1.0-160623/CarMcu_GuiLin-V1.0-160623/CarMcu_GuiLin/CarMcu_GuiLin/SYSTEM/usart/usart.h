#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

  
void uart1_init(u32 bound);
void uart1_send_buf(u8 * string, u16 len);


#endif


