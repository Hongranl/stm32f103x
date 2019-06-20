#ifndef  __USART2_ANDROID_H
#define  __USART2_ANDROID_H
 
#include "global.h"



extern void  	android_CANCEL_SD_buf(u8 transid);
extern void  	android_dma_send_buf(void);
extern void   	android_timeout_fution(void) ; 
extern u8  		android_CHECK_BUF_VALID(u8 *p) ;
extern void 	android_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
extern void 	android_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);
extern uint8_t 	android_crc8(uint8_t *buf,uint8_t len);
extern void 	android_uart_fuc(void);
extern  void  	android_RX_load_buf(u8* buf,u8 len,u8  transid); //接收载入缓存
void 			android_uart_fuc(void);    //接收到数据后处理函数
u8  			android_msg_process(u8 *tab);  
extern void  	android_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );



 
extern TRAN_D_struct   android_TRAN_info  ;




#endif





