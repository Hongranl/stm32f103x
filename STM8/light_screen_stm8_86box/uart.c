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
u8       Rs485_COMM_msg_process(char *tab);  

void send_uart_data(u8*  da, u16 len   );

/*********************************************
函数功能： 串口发送
输入参数： da: 数据  u16 len:长度
输出参数： 无
备    注： 
*********************************************/
void send_uart_data(u8*  da, u16 len   )
{
  u16  i=0;

  for(i=0;i<len;i++)
  {
    while (!(UART1->SR & 0x80));
    UART1->DR = da[i];
    /* Wait transmission is completed */
    while (!(UART1->SR & 0x80)); 
  }
}

/*********************************************
函数功能： 检查缓存命令是否有效
输入参数： 
输出参数： 
备    注： 
*********************************************/
u8  Rs485_COMM_CHECK_BUF_VALID(u8 *p) 
{  
  u8  i = 0;

  for(i=0;i<3;i++)   //接收缓存中的命令连续3个字节不等于0
  {
    if( p[i] != 0 )
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

      crc = (crc >> 1) ^ 0x8C;

      else

      crc >>= 1;
    }
  }
  return crc;
}

/************************************************* 
*串口1   超时处理函数
*/
void  Rs485_COMM_uart_timeout_fution(void) 
{  
  UART1_zigbee_st.ACK_TIMEOUT_js ++; 
}

 /************************************************* 
   接收载入缓存
*/ 	 
void   Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
  u8  i = 0, cun_rx_lun;

  cun_rx_lun = UART1_zigbee_st.uart_Rx_lun;
  
  for(i = 0; i < USART_TXRX_PK_NUM ; i ++)
  {
    if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[cun_rx_lun]) == FALSE )//未使用的BUFF
    {
      memcpy((u8*)UART1_zigbee_st.RXBuff[cun_rx_lun], (u8*)buf,len ); 
      break;
    }
	//及时发送不缓存，既是
	cun_rx_lun = 0;
    //( cun_rx_lun >= (USART_TXRX_PK_NUM - 1) ) ? ( cun_rx_lun = 0 ) : ( cun_rx_lun ++ );
  }     
}

/*****************发送载入BUF**************************/
//   Rs485_COMM_SD_load_buf   ( 0xAAAA,  0xBBBB,    &TRAN_info1 ,                dat ,        TRAN_info1.data_len ); 

void  Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len )
{     
  u8 i = 0,cun_sd_lun;
  u8 crc = 0 ,z_c = 0;


  if( len > UART_SD_LEN ) 
  {
    return; 
  }
  
  disableInterrupts(); 
 
  cun_sd_lun = UART1_zigbee_st.uart_SD_lun ;
  
  for( i = 0; i < USART_TXRX_PK_NUM ; i++ )
  {  
    if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
    { 
      UART1_zigbee_st.SDBuff[cun_sd_lun][0] = d_head;             //包头 
      UART1_zigbee_st.SDBuff[cun_sd_lun][1] = d_head >> 8;        //包头
      
      UART1_zigbee_st.SDBuff[cun_sd_lun][2] = UART1_zigbee_st.txtransid;        //序号

      memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info, sizeof( TRAN_D_struct ));     //目标设备种类识别号  到  数据长度
      
      memcpy(&UART1_zigbee_st.SDBuff[cun_sd_lun][ 3 + sizeof(TRAN_D_struct) ], urt_buf, len);      //载入数据（数据格式中的：实际数据）

      UART1_zigbee_st.SDBuff[cun_sd_lun][ 3 + sizeof(TRAN_D_struct) + len ] = d_tail;              //包尾
      UART1_zigbee_st.SDBuff[cun_sd_lun][ 3 + sizeof(TRAN_D_struct) + len + 1 ] = d_tail >> 8;     //

      z_c = 3 + sizeof(TRAN_D_struct) + len + 2;
      crc = Rs485_COMM_crc8(&UART1_zigbee_st.SDBuff[cun_sd_lun][0],z_c);      
      UART1_zigbee_st.SDBuff[cun_sd_lun][z_c] = crc;            // CRC校验

      UART1_zigbee_st.SDBuff_len[cun_sd_lun] = z_c + 1;

      UART1_zigbee_st.txtransid ++;	//序号++		

     
      //直接发送
      send_uart_data( UART1_zigbee_st.SDBuff[cun_sd_lun],UART1_zigbee_st.SDBuff_len[cun_sd_lun] ) ; 

      memset( UART1_zigbee_st.SDBuff[cun_sd_lun], FALSE, 10 );
        
      break;
    }
	//及时发送不缓存，既是
	cun_sd_lun = 0;
    //( cun_sd_lun >= USART_TXRX_PK_NUM - 1 ) ? ( cun_sd_lun = 0) : ( cun_sd_lun++ );	
  }
  enableInterrupts(); 
}


/***  RS485命令处理 ***/

 void Rs485_COMM_uart_fuc(void) 
{
  u8 i = 0 ;

  for(i = 0; i < USART_TXRX_PK_NUM ; i++)  //统计接收的命令
  {
    if( Rs485_COMM_CHECK_BUF_VALID(UART1_zigbee_st.RXBuff[i]) == TRUE)  // 检查缓存中的命令是否有效
    {
      if( Rs485_COMM_msg_process((char*)UART1_zigbee_st.RXBuff[i]) )  // 接收指令的处理
      { 
        memset(UART1_zigbee_st.RXBuff[i], FALSE,10);   // 清空接收缓存
      }  
      break;
    }
  }  			
}


/***** 接收指令操作 *****/


u8 Rs485_COMM_msg_process(char *tab)  
{
  u8  crc = 0, i = 0;
  TRAN_D_struct   TRAN_info2 ;
  u8 fuc_dat[UART_RX_LEN];
  u8 dat[UART_RX_LEN];
  u8 lu[3];

  i= 17 + tab[16];
  if( i < UART_RX_LEN - 17 &&  tab[0] == 0xAA && tab[1] == 0xAA && tab[i] == 0XBB && tab[i+1] == 0XBB )  //判断命令的帧头、帧尾、长度
  {
    crc = Rs485_COMM_crc8( (u8*) &tab[ 0 ], i+2);
    if( tab[ i+2 ]  != crc  )  //判断CRC
    {
      return 1;
    }
  }
  else 
  {  
    return 1;
  }
   
  //有效命令
  memcpy( &TRAN_info2,(u8*)tab + 3,sizeof( TRAN_D_struct ) );
  memcpy( fuc_dat,(u8*)tab + 17,TRAN_info2.data_len );	  //取出命令中的“实际数据”

  TRAN_info2.source_dev_num = ( TRAN_info2.source_dev_num << 8 | TRAN_info2.source_dev_num >> 8 );  //还原源地址

  TRAN_info2.dest_dev_num = ( TRAN_info2.dest_dev_num << 8 | TRAN_info2.dest_dev_num >> 8 );  //还源目标地址

  dat[0] = 0; 

  dat[2] = 0; // 

  dat[4] = (u8) MCU_UID[0];
  dat[5] = (u8)(MCU_UID[0] >> 8);
  dat[6] = (u8)(MCU_UID[0] >> 16);
  dat[7] = (u8)(MCU_UID[0] >> 24); 

  memset( dat+8,0,8  ); 
    
  if( TRAN_info2.TYPE_NUM == 3 )  //判断命令类型是否为 类型3 
  { 
    switch( fuc_dat[0] << 8 | fuc_dat[1] )  // CMD
    {
      case  notify_net_status:  //查询入网状态
      {
        join_flg = fuc_dat[2]; 

        if(join_flg)
        {
          dat[1]= notify_net_status;
                           
          LED1_PD4_Pin_OUT(0); 
          LED3_PB5_Pin_OUT(0);  
          LED5_PB4_Pin_OUT(0);
                 
          LED2_SWIM_PD1_Pin_OUT(0);  
          LED4_PA1_Pin_OUT(0);
          LED6_PA2_Pin_OUT(0);
                
          switch( program_num  )
          {
            case 1://默认是幕帘程序
            {
              dat[3] = DTN_curtain; // 
              TRAN_info1.source_dev_num = ( DTN_curtain << 8 | DTN_curtain >> 8 );     

              TRAN_info1.data_len = 16 + 1   ;

              dat[8+8] = curtain_status; //1个字节，1上 0下，2暂停          
            }break; 
          
            case 2://2路86灯面板
            {
              dat[3] = DTN_86_LIGHT_2; // 
              TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8 );    
              TRAN_info1.data_len = 16 + 3  ;

              dat[8+8] = 2; //1 2 3  表示路数

              dat[8+8+1] = LIGHT_1_STATUS;  
              dat[8+8+2] = LIGHT_3_STATUS;  
            }break; 
            
            case 3://3路86灯面板
            {
              dat[3] = DTN_86_LIGHT_3; // 
              TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 ); 
              TRAN_info1.data_len = 16 + 4  ;

              dat[8+8] = 3; //1 2 3  表示路数

              dat[8+8+1] = LIGHT_1_STATUS;  
              dat[8+8+2] = LIGHT_2_STATUS;  
              dat[8+8+3] = LIGHT_3_STATUS;  	  
            }break;
          
            default:break;
          } 
        
          Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1, dat ,TRAN_info1.data_len );	
                
          for(i=0;i<5;i++)
          { 
            LED1_PD4_Pin_OUT(1); 
            LED3_PB5_Pin_OUT(1);  
            LED5_PB4_Pin_OUT(1);
            
            LED_time = 300;
            while( LED_time != 0 ); 
            
            
            LED1_PD4_Pin_OUT(0); 
            LED3_PB5_Pin_OUT(0);  
            LED5_PB4_Pin_OUT(0);
            
            LED_time = 300;
            while( LED_time != 0 );
          }
        
          Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );
        }
        else
        {
          LED1_PD4_Pin_OUT(0); 
          LED3_PB5_Pin_OUT(0);  
          LED5_PB4_Pin_OUT(0);

          LED2_SWIM_PD1_Pin_OUT(0);  
          LED4_PA1_Pin_OUT(0);
          LED6_PA2_Pin_OUT(0);

          for(i=0;i<5;i++)
          {
            LED2_SWIM_PD1_Pin_OUT(1);  
            LED4_PA1_Pin_OUT(1);
            LED6_PA2_Pin_OUT(1); 
            
            LED_time = 300;
            while( LED_time != 0 );
            
            
            LED2_SWIM_PD1_Pin_OUT(0);  
            LED4_PA1_Pin_OUT(0);
            LED6_PA2_Pin_OUT(0); 
            
            LED_time = 300;
            while( LED_time != 0 );
          }             
        }
      }break; 
      
      case  control_info:       //控制设备
      {      
        dat[1] = upload_info; //
             
        switch( program_num )
        {
          case 1://默认是幕帘程序
          {
            if(( fuc_dat[2] << 8 | fuc_dat[3] ) == DTN_curtain && TRAN_info1.data_len == 17 )
            {
              if( fuc_dat[16] == 0 )
              {
                alarm_time = 0;
                motor_ck_time = 1500;
                
                sampling_time = 0;

                LED1_PD4_Pin_OUT(1); 
                LED3_PB5_Pin_OUT(1);  
                LED5_PB4_Pin_OUT(0);

                LED2_SWIM_PD1_Pin_OUT(0);  
                LED4_PA1_Pin_OUT(0);
                LED6_PA2_Pin_OUT(1);


                JDQ_WINDOW_DOWN();
                curtain_status = 0;
              }
              else if( fuc_dat[16] == 1 )
              {
                alarm_time = 0;
                motor_ck_time = 1500;

                sampling_time = 0;

                LED1_PD4_Pin_OUT(0); 
                LED3_PB5_Pin_OUT(1);  
                LED5_PB4_Pin_OUT(1);

                LED2_SWIM_PD1_Pin_OUT(1);  
                LED4_PA1_Pin_OUT(0);
                LED6_PA2_Pin_OUT(0);
                             
                JDQ_WINDOW_UP();
                curtain_status = 1;
              }
              else if( fuc_dat[16] == 2 )
              {
                LED1_PD4_Pin_OUT(1); 
                LED3_PB5_Pin_OUT(0);  
                LED5_PB4_Pin_OUT(1);

                LED2_SWIM_PD1_Pin_OUT(0);  
                LED4_PA1_Pin_OUT(1);
                LED6_PA2_Pin_OUT(0);


                JDQ_WINDOW_PAUSE(); 
                curtain_status = 2;
              }
              else
              {
                
              }
                            
              TRAN_info1.data_len = 17;

              TRAN_info1.source_dev_num = ( DTN_curtain << 8 | DTN_curtain >> 8);

              dat[3] = 51; // 

              dat[8+8] = curtain_status; //1个字节，1上 0下，2暂停

              //  Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   ); 
            } 
          }break;
        
        
          case 2://2路86灯面板
            
          case 3://3路86灯面板
          {
            if(( fuc_dat[2] << 8 | fuc_dat[3]) == DTN_86_LIGHT_2 || (fuc_dat[2] << 8 | fuc_dat[3]) == DTN_86_LIGHT_3 )
            {
              lu[0] = fuc_dat[17];

              if( program_num == 2 )
              {
                lu[1] = 0xff;
                lu[2] = fuc_dat[18];
              }
              
              if( program_num == 3)
              {
                lu[1] = fuc_dat[18];
                lu[2] = fuc_dat[19];
              }
              
              
              if( lu[0] == 0 )
              {
                LIGHT_1_STATUS = 0;
                
                KEY_LEFT_BLUE(1); 
                KEY_LEFT_RED(0);  
                
                RLY1_OUT(RLY_OFF) ; 
              }
              else
              {
                LIGHT_1_STATUS = 1;
                
                KEY_LEFT_BLUE(0); 
                KEY_LEFT_RED(1);  
                
                RLY1_OUT(RLY_ON);
              }

              if( program_num != 2 )
              {
                if( lu[1] == 0 )
                {
                  LIGHT_2_STATUS = 0;
                  
                  KEY_MID_BLUE(1); 
                  KEY_MID_RED(0);
                  
                  RLY2_OUT(RLY_OFF); 
                }
                else
                {
                  LIGHT_2_STATUS = 1;
                  
                  KEY_MID_BLUE(0); 
                  KEY_MID_RED(1);
                  
                  RLY2_OUT(RLY_ON);
                }
              }

              if( lu[2] == 0 )
              {
                LIGHT_3_STATUS = 0;
                
                KEY_RIGHT_BLUE(1); 
                KEY_RIGHT_RED(0); 
                
                RLY3_OUT(RLY_OFF);
              }
              else
              {
                LIGHT_3_STATUS = 1;
                
                KEY_RIGHT_BLUE(0); 
                KEY_RIGHT_RED(1); 
                
                RLY3_OUT(RLY_ON);
              }
              

              if( program_num == 2)
              {
                TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8 );
                TRAN_info1.data_len = 16 + 3  ;

                dat[3] = DTN_86_LIGHT_2; // 

                dat[8+8] = 2; //1 2 3  表示路数

                dat[8+8+1] = LIGHT_1_STATUS;  
                dat[8+8+2] = LIGHT_3_STATUS;  
              }
              
              
              if( program_num == 3)
              {
                TRAN_info1.source_dev_num = ( DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8 );
                TRAN_info1.data_len = 16 + 4  ;

                dat[3] = DTN_86_LIGHT_3; // 

                dat[8+8] = 3; //1 2 3  表示路数

                dat[8+8+1] = LIGHT_1_STATUS;  
                dat[8+8+2] = LIGHT_2_STATUS;  
                dat[8+8+3] = LIGHT_3_STATUS; 
              } 
            } 
          }break;
          
        default: break;
      }  
      
      Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB,&TRAN_info1,dat,TRAN_info1.data_len ); 
    }break; 
    
      case query_dev_info :    //收到查询命令  返回
      {    
        dat[1] = upload_info; //

        switch( program_num )
        {
          case 1://默认是幕帘程序
          {
            if( fuc_dat[16] == 1 )  //查询第一类设备信息
            {
              TRAN_info1.data_len = 17   ;

              dat[3] = 51; // 

              dat[8+8] = curtain_status; //1个字节，1上 0下，2暂停
            }
          }break;
          
          case 2://2路86灯面板
          {
            dat[3] = DTN_86_LIGHT_2; // 
            TRAN_info1.source_dev_num = ( DTN_86_LIGHT_2 << 8 | DTN_86_LIGHT_2 >> 8);    
            TRAN_info1.data_len = 16 + 3  ;

            dat[8+8] = 2; //1 2 3  表示路数

            dat[8+8+1] = LIGHT_1_STATUS;  
            dat[8+8+2] = LIGHT_3_STATUS;  
          }break;  
          
          case 3://3路86灯面板
          {
            dat[3] = DTN_86_LIGHT_3; // 
            TRAN_info1.source_dev_num = (DTN_86_LIGHT_3 << 8 | DTN_86_LIGHT_3 >> 8); 
            TRAN_info1.data_len = 16 + 4  ;

            dat[8+8] = 3; //1 2 3  表示路数

            dat[8+8+1] = LIGHT_1_STATUS;  
            dat[8+8+2] = LIGHT_2_STATUS;  
            dat[8+8+3] = LIGHT_3_STATUS; 
          }break;
      
          default:break;
        }
        
        Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB,&TRAN_info1,dat,TRAN_info1.data_len );
      
      }break;
        
    default:  break; 
    }
  } 
  return  1;
}










