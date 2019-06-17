#ifndef __USART3_H
#define __USART3_H

#include "stdio.h"	
#include "sys.h" 

	  	

extern u8 uart3_rx_buf[UART_PACKAGE_MAX+1][UART_RX_MAX_LEN+1];
extern u8 uart3_package_ptr;
extern u8 uart3_rx_ptr;

void uart3_init(u32 bound);
void uart3_send_buf(u8 * string, u16 len);

#endif


