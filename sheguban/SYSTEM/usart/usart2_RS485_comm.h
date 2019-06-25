#ifndef  __USART2_RS485_H
#define  __USART2_RS485_H

#include "stdio.h"	
#include "sys.h" 
#include <ctype.h>
 

extern  void  Rs485_COMM_uart_timeout_fution(void);
 
extern  void  Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

extern  void Rs485_COMM_uart_fuc(void) ;
   
extern  void  Rs485_COMM_dma_send_buf(void);

extern void  Rs485_COMM_SD_load_buf2(  u8 * urt_buf ,u8  len   );

#endif


