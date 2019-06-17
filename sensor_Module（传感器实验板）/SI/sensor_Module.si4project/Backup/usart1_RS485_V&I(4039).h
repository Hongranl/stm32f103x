#ifndef  __USART1_RS485_V_I_H
#define  __USART1_RS485_V_I_H
 
#include "global.h"	
 
 

extern void  RS485_V_I_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);  //应答载入缓存

extern void  RS485_V_I_RX_load_buf(u8* buf,u8 len,u8  transid); //接收载入缓存

extern void  RS485_V_I_CANCEL_SD_buf(u8 transid);           //删除发送缓存
 

extern void  RS485_V_I_dma_send_buf(void);             //DMA发送函数

extern void   RS485_V_I_timeout_fution(void) ;         //超时函数


extern void RS485_V_I_uart_fuc(void);    //接收到数据后处理函数

extern u8  RS485_V_I__msg_process(char *tab);  



extern u8  RS485_V_I_CHECK_BUF_VALID(u8 *p) ;    //检查数据不为空

extern void RS485_V_I_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
extern void RS485_V_I_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

 
extern uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len);       //校验 





extern TRAN_D_struct   RS485_V_I_TRAN_info  ;

extern void  RS485_V_I_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );   //发送载入缓存
 
void  RS485_V_I_SD_load_buf2(  u8 * urt_buf ,u8  len   );

#endif 

