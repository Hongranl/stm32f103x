#ifndef  __USART2_RS485_H
#define  __USART2_RS485_H
 
#include "global.h"


 


 

 
extern void  RS485_CANCEL_SD_buf(u8 transid);

 

extern void  RS485_dma_send_buf(void);

extern void   RS485_timeout_fution(void) ;
 
extern u8  RS485_CHECK_BUF_VALID(u8 *p) ;

extern void RS485_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
extern void RS485_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

 
extern uint8_t RS485_crc8(uint8_t *buf,uint8_t len);

extern void RS485_uart_fuc(void);
  

extern  void  RS485_RX_load_buf(u8* buf,u8 len,u8  transid); //接收载入缓存

void RS485_uart_fuc(void);    //接收到数据后处理函数

u8  RS485_msg_process(u8 *tab);  



 
extern TRAN_D_struct   RS485_TRAN_info  ;


extern void  RS485_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );


#endif





