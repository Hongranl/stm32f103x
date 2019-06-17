 
 #include "global.h"


TRAN_D_struct   android_TRAN_info  ;


void   store_shegu_info( u8  s_code  , u16  s_dev_num );

void  android_RX_load_buf(u8* buf,u8 len,u8  transid); //接收载入缓存

void  android_CANCEL_SD_buf(u8 transid);           //删除发送缓存
 
void  android_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );   //发送载入缓存
 
void  ANDROID_DMA_send_buf(void);             //DMA发送函数

void   android_timeout_fution(void) ;         //超时函数


void android_uart_fuc(void);    //接收到数据后处理函数

u8  android__msg_process(u8 *tab);  



u8  android_CHECK_BUF_VALID(u8 *p) ;    //检查数据不为空


uint8_t android_crc8(uint8_t *buf,uint8_t len);       //校验


u8       gb_dat[18]; 




u8  android_CHECK_BUF_VALID(u8 *p)//不全空的话 视为有效
{  u8  i=0;
	
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






uint8_t android_crc8(uint8_t *buf,uint8_t len)
{
	/*
  uint8_t i,l,crc;
  uint16_t init=0;
 


	for(l=0;l<len;l++)
  {
    init^=(buf[l]*0x100);
    for(i=0;i<8;i++)
    {
      if(init&0x8000)
        init^=0x8380;
      init*=2;
    }
  }
  crc = init/0x100;
	
	
  return crc;
	*/
	
uint8_t crc;
uint8_t i;
	
	crc = 0;

while(len--) {

crc ^= *buf++;

for(i = 0; i < 8; i++) {

if(crc & 0x01)

crc = (crc >> 1) ^ 0x8C;

else

crc >>= 1;

}

}

return crc;
	
	

}


 
 



 void USART2_IRQHandler(void)                	//串口3中断服务程序
 {  
  	 u8	 transid=0,crc=0;
     u32 i,Data_Len=0;
		
    if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //判断是否是空闲标志
    { 
			
			 DMA_ClearFlag( DMA1_FLAG_GL6 );           // 清DMA标志位
		   USART_ClearITPendingBit(USART2, USART_IT_IDLE);  //清除空闲标志
         i = USART2->SR;  //清除空闲标志
         i = USART2->DR; //清除空闲标志
			  
         DMA_Cmd(ANDROID_DMA_RX_CH, DISABLE);// 禁用  避免出错
         Data_Len=UART_RX_LEN- DMA_GetCurrDataCounter(ANDROID_DMA_RX_CH);  
        
  		  
			       send_urt_time=50;
						if(Data_Len<UART_RX_LEN ) 
					{  
						   if(UART2_android_st.RXcun[0] ==0xAA&&UART2_android_st.RXcun[1] ==0xAA )
							{
											i= 17+UART2_android_st.RXcun[16];
									if(UART2_android_st.RXcun[ i ]==0XBB&&UART2_android_st.RXcun[ i+1 ]==0XBB )
									{
											   crc=android_crc8( (u8*) &UART2_android_st.RXcun[ 0 ], i+2);
										 
											 if(  UART2_android_st.RXcun[ i+2 ]  ==crc  )//判断CRC
											 {
												   //如果接收到数据
												     transid =   UART2_android_st.RXcun[ 2 ] ;
														android_RX_load_buf(UART2_android_st.RXcun,i+3,transid);     //载入到RX  FIFO   
												    
											 }
											 
									}
							   
							}
					} 
				 
					
          memset(UART2_android_st.RXcun, 0,i+2);
				
       
					 ANDROID_DMA_RX_CH->CNDTR=UART_RX_LEN; //  重新赋值计数值，必须大于等于最大
					
			   	DMA_Cmd(ANDROID_DMA_RX_CH, ENABLE);//
					
    }  		
		 
 }


/*****************DMA 发送完中断************************/
void  DMA1_Channel7_IRQHandler(void) 
{   
	   
	 if(DMA_GetITStatus( DMA1_IT_TC7) != RESET) // 
    {   
			   UART2_android_st.ALL_tc_flg=TRUE;   
        DMA_Cmd(ANDROID_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送
				 DMA_ClearITPendingBit(DMA1_FLAG_GL7);   //DMA1_FLAG_GL2
			
			
			while(USART_GetFlagStatus(USART2,USART_FLAG_TC)!=SET){};//等待发送结束
		 											
			
		}

}
 
/************************************************* 
*串口3   超时处理函数
*/
void   android_timeout_fution(void) 
{  
     UART2_android_st.ACK_TIMEOUT_js++;
}
 
 
 /*************************************************  
   接收载入缓存*/

void   android_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
      u8  i=0,cun_rx_lun;
									 
	     cun_rx_lun=UART2_android_st.uart_Rx_lun;
			for(i=0;i<USART_TXRX_PK_NUM ;i++)
			{
		    if( android_CHECK_BUF_VALID(UART2_android_st.RXBuff[cun_rx_lun])==FALSE)//未使用的BUFF
					{
				   memcpy((u8*)UART2_android_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
				    
			 
                 break;
					}
			 (cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
			} 
		 	
	     // test   
      android_ACK_load_buf(0XCCCC,0XDDDD,0,transid);		
    
}
  	 
 
///************************************************* 
//*串口3  DMA 发送 
//*/ 
void  android_dma_send_buf(void)
{   u8  i=0,cun_sd_lun;
      
	
	  //if( UART2_android_st.ALL_tc_flg==TRUE )//DMA传送完毕
	{     
     	 if(UART2_android_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART2_android_st.ACK_RX_OK_flg ==FALSE  )//  超时未到时间，而且 应答标志未收到OK   继续等待
    	{ 
    		return;
  		}			 
		UART2_android_st.ACK_RX_OK_flg=FALSE;			
		UART2_android_st.ACK_TIMEOUT_js=0;	

		USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(ANDROID_DMA_TX_CH,DMA_IT_TC,DISABLE);	 
		// disable_all_uart_interupt();
		//没有ACK时 发送数据
		 cun_sd_lun= UART2_android_st.uart_SD_lun;
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{  
			  if( android_CHECK_BUF_VALID(UART2_android_st.SDBuff[cun_sd_lun])==TRUE)//如果有数据
				 {
						 
									
								 
									 
					  	memset(UART2_android_st.SDcun, FALSE,10);			
								
	      				memcpy((u8*)UART2_android_st.SDcun , (u8*)UART2_android_st.SDBuff[cun_sd_lun], UART2_android_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 					    
								
						memset(UART2_android_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
								
						MYDMA_Config( ANDROID_DMA_TX_CH,(u32)&USART2->DR,(u32)UART2_android_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 
								 
		      			MYDMATX_Enable(ANDROID_DMA_TX_CH, UART2_android_st.SDBuff_len[cun_sd_lun] );		
								
						USART_Cmd(USART2, ENABLE); //使能串口   这个一定要加············	 
							 
							 
						UART2_android_st.uart_SD_lun =cun_sd_lun; // 暂存
					//	UART2_android_st.ALL_tc_flg=FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 

				
					 	UART2_android_st.ACK_RX_OK_flg=FALSE;			
						UART2_android_st.ACK_TIMEOUT_js=0;			
									
	   					
						break;
					}
				 (cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
		}
     //enable_all_uart_interupt();
        DMA_ITConfig(ANDROID_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 
				 
		 } 
}



/*****************发送载入BUF**************************/
void  android_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   )
{     u8  crc=0, i=0,cun_sd_lun;
 
	   
      if( len >UART_SD_LEN) 
			 {
			   return; 
			 }
				 
		disable_all_uart_interupt();	
	
	cun_sd_lun = UART2_android_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( zigbee_CHECK_BUF_VALID(UART2_android_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			UART2_android_st.SDBuff[cun_sd_lun][0] = d_head;//包头 
			UART2_android_st.SDBuff[cun_sd_lun][1] = d_head>>8;//包头
			UART2_android_st.SDBuff[cun_sd_lun][2] = UART2_android_st.txtransid;

			memcpy(&UART2_android_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART2_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART2_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len] = d_tail;   //包尾
			UART2_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1] = d_tail>>8;

			crc= zigbee_crc8(&UART2_android_st.SDBuff[cun_sd_lun][0],3+sizeof(TRAN_D_struct)+len+2);

			UART2_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+2] = crc;

			UART2_android_st.SDBuff_len[cun_sd_lun] = 3 + sizeof(TRAN_D_struct) + len + 2 + 1 ;


			UART2_android_st.txtransid++;	//序号++			
			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM -1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}
	enable_all_uart_interupt();	
}


void android_uart_fuc(void) 
{

   u8  i=0 ;
	
	
	    //  disable_all_uart_interupt();	
     // USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
      //DMA_ITConfig(ANDROID_DMA_TX_CH,DMA_IT_TC,DISABLE); 
   
    
      for(i=0;i<USART_TXRX_PK_NUM ;i++)
      {
                     if( android_CHECK_BUF_VALID(UART2_android_st.RXBuff[i])==TRUE)  
                      {
                        
                        
                      if( android__msg_process( UART2_android_st.RXBuff[i]) )
                      { 
                        memset(UART2_android_st.RXBuff[i], FALSE,10);
                      } 
                      
                           break;
                      }
 
      } 
	     // enable_all_uart_interupt();	
      //DMA_ITConfig(ANDROID_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	   // USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 

}




 u8  android__msg_process(u8 *tab)  
 {
     
   TRAN_D_struct   TRAN_info1;
   u8              fuc_dat[UART_RX_LEN];
 	// u8              dat[UART_RX_LEN];
	 
	 memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
   memcpy(fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	
   memcpy(gb_dat , fuc_dat,18);
	 
	 TRAN_info1.source_dev_num=(TRAN_info1.source_dev_num<<8|TRAN_info1.source_dev_num>>8);
	 
	 TRAN_info1.dest_dev_num=(TRAN_info1.dest_dev_num<<8|TRAN_info1.dest_dev_num>>8);
	 
	  
	   if(TRAN_info1.TYPE_NUM==4)
		 {
		   //转发给zigbee
		   zigbee_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );
		   printf("安卓--->转发---->zigbee\n");
			LED2 = ~LED2;	
			delay_ms(100);
			LED2 = ~LED2;

	   }
		 
		  
   
    return  1;
 }

 
 
 

