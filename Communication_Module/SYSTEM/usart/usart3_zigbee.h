 
#ifndef  __USART3_ZIGBEE_H
#define  __USART3_ZIGBEE_H

#include "global.h"	
 

 


extern  void  zigbee_uart_timeout_fution(void);
 
extern  void  zigbee_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

extern  void zigbee_uart_fuc(void) ;
   
extern  void  zigbee_dma_send_buf(void);

extern  u8 zigbee_CHECK_BUF_VALID(u8 *p);


#endif




