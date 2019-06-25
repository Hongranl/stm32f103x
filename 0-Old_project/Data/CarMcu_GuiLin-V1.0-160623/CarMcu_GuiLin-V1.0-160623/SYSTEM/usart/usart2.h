#ifndef __USART2_H
#define __USART2_H

#include "stdio.h"	
#include "sys.h" 


	  	
extern u8 uart2_rx_buf[UART_PACKAGE_MAX+1][UART_RX_MAX_LEN+1];
extern u8 uart2_package_ptr;
extern u8 uart2_rx_ptr;

void uart2_init(u32 bound);
void uart2_send_buf(u8 * string, u16 len);

#endif


