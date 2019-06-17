 
#ifndef  __USART2_wifi_H
#define  __USART2_wifi_H

#include "global.h"	
 

 


extern  void  Android_uart_timeout_fution(void);
 
extern  void  Android_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

extern  void Android_uart_fuc(void) ;
   
extern  void  Android_dma_send_buf(void);



#endif




