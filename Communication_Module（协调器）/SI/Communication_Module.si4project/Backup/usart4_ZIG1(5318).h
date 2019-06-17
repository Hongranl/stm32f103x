#ifndef  __USART1_ZIG1_H
#define  __USART1_ZIG1_H

#include "global.h"




extern  void  ZIG1_uart_timeout_fution(void);
 
extern  void  ZIG1_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

extern  void  ZIG1_uart_fuc(void) ;
   
extern  void  ZIG1_dma_send_buf(void);



#endif







