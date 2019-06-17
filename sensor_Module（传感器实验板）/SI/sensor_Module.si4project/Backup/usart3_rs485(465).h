#ifndef  __USART3_RS485_H
#define  __USART3_RS485_H
 
#include "global.h"


 


//extern void  RS485_ACK_load_buf(u8* hd,u8 transid);

extern void  RS485_RX_load_buf(u8* buf,u8 len,u8  transid);

extern void  RS485_CANCEL_SD_buf(u8 transid);

 

extern void  RS485_dma_send_buf(void);

extern void   RS485_timeout_fution(void) ;
 
extern u8  RS485_CHECK_BUF_VALID(u8 *p) ;

extern void RS485_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
extern void RS485_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

extern u8      RS485_rcvd_crc(char *buf, u8 *transid);
extern uint8_t RS485_crc8(uint8_t *buf,uint8_t len);

extern void RS485_uart_fuc(void);


#pragma pack(1)
typedef  struct
{   
   u16   dest_dev_num;     //目标设备号
	 u8    dest_addr[4];     //目标地址
	 u16   source_dev_num;   //源设备号
	 u8    source_addr[4];   //源地址 
	 u8    TYPE_NUM ;        //数据格式号
	 u8    data_len ;        //数据长度	 
}
TRAN_D_struct;  


extern TRAN_D_struct   RS485_TRAN_info  ;


extern void  RS485_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );


#endif





