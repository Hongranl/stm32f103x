#ifndef __USART_H
#define __USART_H
 
 
	
#ifndef  TRUE
#define  TRUE   1
#endif

#ifndef  FALSE
#define  FALSE  0
#endif	
	
#ifndef  FAIL
#define  FAIL     1
#endif

#ifndef  SUCCESS
#define  SUCCESS  0
#endif	
	
	
	
#define    TIMEOUT_SUM     			 100    //超时20MS
#define    RE_SEND_SUM      			 1      //重发两次

#define    TIMEOUT_SUM_2  		         100    //超时20MS
#define    RE_SEND_SUM_2  			 1      //重发两次

#define    ALL_ACK_LEN 				 10  	//所有ACK长度不超过10
 
#define    UART_SD_LEN  			 80  	// 
#define    UART_RX_LEN  			 80	//  
			
#define    USART_TXRX_PK_NUM  	                 1  	//


 
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
 


typedef  struct
{ 
	 u8   txtransid;        //序号
	 u8   ACK_TIMEOUT_SUM ;//超时次数
	 u16  ACK_TIMEOUT_js;//超时计数	
   
	 u8   ACK_RX_OK_flg;   //ACK 接受到OK
	 u8   ALL_tc_flg;

	 u8    uart_SD_lun;     //发送轮询
   u8    uart_Rx_lun;    //接收轮询
	
	 
	 u8   SDcun[UART_SD_LEN] ;
   u8   RXcun[UART_RX_LEN] ;
	
	 u8   SDBuff_len[USART_TXRX_PK_NUM]  ;
   u8   RXBuff_len[USART_TXRX_PK_NUM]  ;
	 
   u8   SDBuff[USART_TXRX_PK_NUM][UART_SD_LEN] ;
   u8   RXBuff[USART_TXRX_PK_NUM][UART_RX_LEN] ;
   u8   ACKBuff[USART_TXRX_PK_NUM][ALL_ACK_LEN] ;
	 
}
Uart_struct;

 
 
extern   Uart_struct    UART1_zigbee_st; 
 

extern  u8       Rs485_COMM_CHECK_BUF_VALID(u8 *p);
extern  uint8_t  Rs485_COMM_crc8(uint8_t *buf,uint8_t len);
extern  void    Rs485_COMM_uart_timeout_fution(void) ;
extern  void    Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid);
extern  void    Rs485_COMM_dma_send_buf(void);
extern  void    Rs485_COMM_SD_load_buf2(  u8 * urt_buf ,u8  len   );
extern  void    Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );
extern  void    Rs485_COMM_uart_fuc(void) ;
extern  u8       Rs485_COMM_msg_process(u8 *tab);  
 

extern  u16    send_urt_time ;



#endif


