#include "ALL_Includes.h"

Uart_struct    UART1_zigbee_st; 

u16    send_urt_time=0;

u8       Rs485_COMM_CHECK_BUF_VALID(u8 *p);
uint8_t  Rs485_COMM_crc8(uint8_t *buf,uint8_t len);
void    Rs485_COMM_uart_timeout_fution(void) ;
void    Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid);
void    Rs485_COMM_dma_send_buf(void);
void    Rs485_COMM_SD_load_buf2(  u8 * urt_buf ,u8  len   );
void    Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );
void    Rs485_COMM_uart_fuc(void) ;
u8       Rs485_COMM_msg_process(u8 *tab);  

void send_uart_data(u8*  da, u16 len   );

/*********************************************
函数功能： 
输入参数： 
输出参数： 
备    注： 
*********************************************/
void send_uart_data(u8*  da, u16 len   )
{
  u16  i=0;

  RS485_DIR_PD4_Pin_OUT(1);
  RS485_DIR_PD4_Pin_OUT(1);
  
  for(i=0;i<len;i++)
  {
    while (!(UART1->SR & 0x80));
    UART1->DR = da[i];
    /* Wait transmission is completed */
    while (!(UART1->SR & 0x80)); 
  }
  
  Delay (2500);
  RS485_DIR_PD4_Pin_OUT(0);
  RS485_DIR_PD4_Pin_OUT(0);
}

u8  Rs485_COMM_CHECK_BUF_VALID(u8 *p) 
{  
  u8  i=0;

  for(i=0;i<3;i++)
  {
    if(  p[i] !=0  )
    {
      return TRUE;
    } 
  }
  return FALSE;
}


/*********************************************************************
 *  crc
 */
uint8_t Rs485_COMM_crc8(uint8_t *buf,uint8_t len)
{  
  uint8_t crc;
  uint8_t i;

  crc = 0;

  while(len--) 
  {
    crc ^= *buf++;

    for(i = 0; i < 8; i++) 
    {
      if(crc & 0x01)
      {
        crc = (crc >> 1) ^ 0x8C;
      }
      else
      {
        crc >>= 1;
      }
    }
  }
  return crc;
}

/************************************************* 
*串口1   超时处理函数
*/
void  Rs485_COMM_uart_timeout_fution(void) 
{  
  UART1_zigbee_st.ACK_TIMEOUT_js++; 
}
 


 /************************************************* 
   接收载入缓存
*/ 	 
void   Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
  u8  i=0,cun_rx_lun;

  cun_rx_lun=UART1_zigbee_st.uart_Rx_lun;
  
  for(i=0;i<USART_TXRX_PK_NUM ;i++)
  {
    if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[cun_rx_lun])==FALSE)//未使用的BUFF
    {
      memcpy((u8*)UART1_zigbee_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
      break;
    }
    (cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
  }     
}


/*****************发送载入BUF**************************/
void  Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len )
{     
  u8  i = 0,cun_sd_lun;
  u8  crc = 0 ,z_c = 0;

  if( len > UART_SD_LEN ) 
  {
    return; 
  }

  disableInterrupts(); 
 
  cun_sd_lun = UART1_zigbee_st.uart_SD_lun ;
  
  for(i=0; i < USART_TXRX_PK_NUM ; i++)
  {  
    if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
    { 
      UART1_zigbee_st.SDBuff[cun_sd_lun][0] = d_head;//包头 
      UART1_zigbee_st.SDBuff[cun_sd_lun][1] = d_head >> 8;//包头
      UART1_zigbee_st.SDBuff[cun_sd_lun][2] = UART1_zigbee_st.txtransid;

      memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
      memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

      UART1_zigbee_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len] = d_tail;   //包尾
      UART1_zigbee_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1] = d_tail >> 8;

      z_c = 3 + sizeof(TRAN_D_struct) + len + 2;
      crc = Rs485_COMM_crc8(&UART1_zigbee_st.SDBuff[cun_sd_lun][0],z_c);

      UART1_zigbee_st.SDBuff[cun_sd_lun][z_c] = crc;

      UART1_zigbee_st.SDBuff_len[cun_sd_lun] = z_c + 1;

      UART1_zigbee_st.txtransid++;	//序号++		


      //直接发送
      send_uart_data(UART1_zigbee_st.SDBuff[cun_sd_lun] , UART1_zigbee_st.SDBuff_len[cun_sd_lun] ) ; 

      memset(UART1_zigbee_st.SDBuff[cun_sd_lun], FALSE, 10 );

      break;
    }
    (cun_sd_lun >= USART_TXRX_PK_NUM -1)?(cun_sd_lun = 0):(cun_sd_lun++);	
  }
  enableInterrupts(); 
}


 void Rs485_COMM_uart_fuc(void) 
{
  u8  i = 0 ;

  for(i = 0; i < USART_TXRX_PK_NUM ; i++)
  {
    if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[i]) == TRUE )  
    {
      if( Rs485_COMM_msg_process(UART1_zigbee_st.RXBuff[i]) )
      { 
        memset(UART1_zigbee_st.RXBuff[i], FALSE,10);
      } 
      break;
    }
  }
}


u8 Rs485_COMM_msg_process(u8 *tab)  
{
  u8  crc = 0,i = 0;
  TRAN_D_struct   TRAN_info1 ;
  u8    fuc_dat[UART_RX_LEN];
  // u8    dat[UART_RX_LEN];
  // u16   abc=0;

  i = 17 + tab[16];
  
  if(( i < UART_RX_LEN - 17 ) && ( tab[0] == 0xAA ) && ( tab[1] == 0xAA ) && ( tab[i] == 0xBB ) && ( tab[i+1] == 0xBB ) )
  {
    crc=Rs485_COMM_crc8((u8*) &tab[ 0 ], i+2);

    if( tab[ i+2 ]  != crc  )//判断CRC
    {
      return 1;
    }
  }
  else 
  {  
    return 1;
  }

  memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
  memcpy(fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	  

  TRAN_info1.source_dev_num = ( TRAN_info1.source_dev_num << 8 | TRAN_info1.source_dev_num >> 8 );

  TRAN_info1.dest_dev_num = ( TRAN_info1.dest_dev_num << 8 | TRAN_info1.dest_dev_num >> 8 );

  if( TRAN_info1.TYPE_NUM == 4 )
  { 
    switch(  fuc_dat[0]<<8|fuc_dat[1]  ) 
    {
      case  0x0a:     //控制翻转屏
      
      /*  if( fuc_dat[2]==screen_on)//执行，显示器开机  
      { 
        screen_sw_mode  =screen_on ;  

        dat_595|= RLY1_595Pin; //取电
        write_595( &dat_595 ,1 ); 

        screen_relay_on_time=1000;//1S 后开屏

      }
      if( fuc_dat[2] == screen_off  )//执行，显示器关机
      { 
        screen_sw_mode=screen_off; 
        //关闭屏 
        dat_595&=~SCREEN_ON_595Pin; //0  是开
        write_595( &dat_595 ,1 );
        Delay (100);
        dat_595|= SCREEN_ON_595Pin; //1  是 
        write_595( &dat_595 ,1 );

        //等待3s 后断电
        screen_relay_off_time=1000;//3S后 继电器关 

      } */
         
      if( fuc_dat[3] == motor_up )//翻转屏上升
      {
        //dat_595 &= ~RLY2_595Pin;  
        //dat_595 &= ~RLY3_595Pin; 
        //write_595( &dat_595 ,1 );
        //Delay(50000); 
        //Delay(50000); 
        //Delay(50000); 
        //dat_595 |= RLY3_595Pin; 
        //dat_595 &=~RLY2_595Pin;
       // dat_595 &=~LED2_595Pin; // 
       // dat_595 |= LED3_595Pin; //
       // write_595( &dat_595 ,1 );

        //motor_status = motor_up; 
       // motor_running_status = motor_up;
        UART_RX_CMD = UART_RX_CMD_motor_up;
        
      }
      
      if( fuc_dat[3] == motor_down )//翻转屏下降 
      {
       // dat_595 &=~RLY2_595Pin;  
       // dat_595 &=~RLY3_595Pin; 
        
       // write_595( &dat_595 ,1 );
        
       // Delay(50000); 
       // Delay(50000); 
       // Delay(50000); 
        
       // dat_595 |= RLY2_595Pin; 
       // dat_595 &=~RLY3_595Pin; 
        
       // dat_595 &=~LED3_595Pin; // 
       // dat_595 |= LED2_595Pin; //
        
       // write_595( &dat_595 ,1 );
        
       // motor_status = motor_down;
      //  motor_running_status = motor_down;
        UART_RX_CMD = UART_RX_CMD_motor_down;
      }
      
      if( fuc_dat[3] == motor_stop )//翻转屏停止 
      {
        //dat_595 &=~RLY2_595Pin;  
        //dat_595 &=~RLY3_595Pin; 
        //dat_595 |=  LED2_595Pin; // 
       // dat_595 |=  LED3_595Pin; // 
        //write_595( &dat_595 ,1 );
        //motor_status = motor_stop;  
       // motor_running_status = motor_stop; 
       UART_RX_CMD = UART_RX_CMD_motor_stop; 
        
      }   
      break; 
      default:  break; 
     }
  } 
  return  1;
}


