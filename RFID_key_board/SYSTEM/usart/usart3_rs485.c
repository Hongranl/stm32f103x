 
 #include "global.h"


TRAN_D_struct   RS485_TRAN_info  ;


void   store_shegu_info( u8  s_code  , u16  s_dev_num );

void  RS485_RX_load_buf(u8* buf,u8 len,u8  transid); //接收载入缓存

void  RS485_CANCEL_SD_buf(u8 transid);           //删除发送缓存
 
void  RS485_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );   //发送载入缓存
 
void  RS485_dma_send_buf(void);             //DMA发送函数

void   RS485_timeout_fution(void) ;         //超时函数


void RS485_uart_fuc(void);    //接收到数据后处理函数

u8  RS485__msg_process(u8 *tab);  


u8  RS485_CHECK_BUF_VALID(u8 *p) ;    //检查数据不为空


uint8_t RS485_crc8(uint8_t *buf,uint8_t len);       //校验


u8  RS485_CHECK_BUF_VALID(u8 *p)//不全空的话 视为有效
{  
	u8 i = 0;
	
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






uint8_t RS485_crc8(uint8_t *buf,uint8_t len)
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

void USART3_IRQHandler(void)                	//串口3中断服务程序
{  
	u8	 transid = 0,crc = 0;
	u32 i,Data_Len=0;

	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //判断是否是空闲标志
	{ 
		DMA_ClearFlag( DMA1_FLAG_GL3 );           // 清DMA标志位
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);  //清除空闲标志
		i = USART3->SR;  //清除空闲标志
		i = USART3->DR; //清除空闲标志

		DMA_Cmd(RS485_DMA_RX_CH, DISABLE);// 禁用  避免出错
		Data_Len = UART_RX_LEN - DMA_GetCurrDataCounter(RS485_DMA_RX_CH);  

		send_urt_time = 50;
		if(Data_Len < UART_RX_LEN ) 
		{  
			if(UART3_rs485_st.RXcun[0] == 0xAA && UART3_rs485_st.RXcun[1] == 0xAA )
			{
				i = 17+UART3_rs485_st.RXcun[16];
				if(UART3_rs485_st.RXcun[i] == 0xBB && UART3_rs485_st.RXcun[i+1] == 0xBB )
				{
					crc=RS485_crc8((u8*) &UART3_rs485_st.RXcun[0], i+2);
					if( UART3_rs485_st.RXcun[i+2] == crc )//判断CRC
					{
						//如果接收到数据
						transid = UART3_rs485_st.RXcun[2] ;
						RS485_RX_load_buf(UART3_rs485_st.RXcun,i+3,transid);     //载入到RX  FIFO   
					}
				}
			}
		} 				
		memset(UART3_rs485_st.RXcun, 0,i+2);

		RS485_DMA_RX_CH->CNDTR=UART_RX_LEN; //  重新赋值计数值，必须大于等于最大

		DMA_Cmd(RS485_DMA_RX_CH, ENABLE);//					
	}  			 
}


/*****************DMA 发送完中断************************/
void  DMA1_Channel2_IRQHandler(void) 
{      
	if(DMA_GetITStatus( DMA1_IT_TC2) != RESET) // 
	{   
		UART3_rs485_st.ALL_tc_flg = TRUE;   
		DMA_Cmd(RS485_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送
		DMA_ClearITPendingBit(DMA1_FLAG_GL2);   //DMA1_FLAG_GL2

		while(USART_GetFlagStatus(USART3,USART_FLAG_TC) != SET){};//等待发送结束

		RS485_LED	= ~RS485_LED;				
		RS485_con = 0; //接收 
		RS485_con = 0; //接收 
	}
}
 
/************************************************* 
*串口3   超时处理函数
*/
void   RS485_timeout_fution(void) 
{  
	UART3_rs485_st.ACK_TIMEOUT_js++;
}
 
 
 /*************************************************  
   接收载入缓存*/

void   RS485_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
	u8 i = 0,cun_rx_lun;

	cun_rx_lun = UART3_rs485_st.uart_Rx_lun;
	for(i=0; i<USART_TXRX_PK_NUM ; i++)
	{
		if( RS485_CHECK_BUF_VALID(UART3_rs485_st.RXBuff[cun_rx_lun]) == FALSE)//未使用的BUFF
		{
			memcpy((u8*)UART3_rs485_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
			break;
		}
		(cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
	} 
	// test   
	//  RS485_ACK_load_buf(0XCCCC,0XDDDD,0,transid);		
}
  	 
 
///************************************************* 
//*串口3  DMA 发送 
//*/ 
void  RS485_dma_send_buf(void)
{   
	u8 i=0, cun_sd_lun;
      
	if( UART3_rs485_st.ALL_tc_flg==TRUE )//DMA传送完毕
	{
		if(UART3_rs485_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART3_rs485_st.ACK_RX_OK_flg ==FALSE  )//  超时未到时间，而且 应答标志未收到OK   继续等待
		{
			return;
		}			 
		UART3_rs485_st.ACK_RX_OK_flg=FALSE;			
		UART3_rs485_st.ACK_TIMEOUT_js=0;	

		USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE);	 
		// disable_all_uart_interupt();
		//没有ACK时 发送数据
		cun_sd_lun= UART3_rs485_st.uart_SD_lun;
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{  
			if( RS485_CHECK_BUF_VALID(UART3_rs485_st.SDBuff[cun_sd_lun]) == TRUE)//如果有数据
			{
				memset(UART3_rs485_st.SDcun, FALSE,10);			
				memcpy((u8*)UART3_rs485_st.SDcun , (u8*)UART3_rs485_st.SDBuff[cun_sd_lun], UART3_rs485_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 
				memset(UART3_rs485_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
	
				RS485_con = 1; //485
				RS485_con = 1; //485
							
				MYDMA_Config( RS485_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_rs485_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 
  								 
				MYDMATX_Enable(RS485_DMA_TX_CH, UART3_rs485_st.SDBuff_len[cun_sd_lun] );		

				USART_Cmd(USART3, ENABLE); //使能串口   这个一定要加············	 
					 
				UART3_rs485_st.uart_SD_lun = cun_sd_lun; // 暂存
				//	UART3_rs485_st.ALL_tc_flg=FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 


				UART3_rs485_st.ACK_RX_OK_flg = FALSE;			
				UART3_rs485_st.ACK_TIMEOUT_js = 0;			
						 					
				break;
			}
			(cun_sd_lun >= USART_TXRX_PK_NUM-1)?(cun_sd_lun = 0):(cun_sd_lun++);	
		}
		//enable_all_uart_interupt();
		DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
	} 
}



/*****************发送载入BUF**************************/
void  RS485_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   )
{     
	u8 crc = 0, i = 0,cun_sd_lun;
 
	//u16  offset1=0,nx=0;
	// char buff[500];

	if( len > UART_SD_LEN) 
	{
		return; 
	}
				 
	//	 disable_all_uart_interupt();
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  //临界点
	DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun = UART3_rs485_st.uart_SD_lun ;
	for(i=0; i<USART_TXRX_PK_NUM ; i++)
	{  
		if( RS485_CHECK_BUF_VALID(UART3_rs485_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			UART3_rs485_st.SDBuff[cun_sd_lun][0] = d_head;//包头
			UART3_rs485_st.SDBuff[cun_sd_lun][1] = d_head >> 8;//包头
			UART3_rs485_st.SDBuff[cun_sd_lun][2] = UART3_rs485_st.txtransid;

			memcpy(&UART3_rs485_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len] = d_tail;   //包尾
			UART3_rs485_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1] = d_tail>>8;

			crc = RS485_crc8( &UART3_rs485_st.SDBuff[cun_sd_lun][0],3 + sizeof(TRAN_D_struct) + len + 2 );

			UART3_rs485_st.SDBuff[cun_sd_lun][ 3 + sizeof(TRAN_D_struct) + len + 2 ] = crc;

			UART3_rs485_st.SDBuff_len[cun_sd_lun] = 3 + sizeof(TRAN_D_struct) + len + 2 + 1 ;
																						
			/*
			sprintf(buff ,"from maincon to rs485 :"); 

			UART1_SD_load_buf((u8*)buff, strlen(buff));
			memset(buff,0,strlen(buff));

			for(nx=0;nx < UART3_rs485_st.SDBuff_len[cun_sd_lun];nx++ )
			{
			offset1+= sprintf(buff+offset1  ,"%02x+" ,UART3_rs485_st.SDBuff[cun_sd_lun][nx] ); 
			}
			UART1_SD_load_buf((u8*)buff, strlen(buff));  

			UART1_SD_load_buf( (u8*)"\r\n", strlen("\r\n")); 

			*/

			UART3_rs485_st.txtransid++;	//序号++	

			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}
	// enable_all_uart_interupt();				 
	DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE);  //临界点
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
}


void RS485_uart_fuc(void) 
{
	u8 i = 0 ;
	//  disable_all_uart_interupt();	
	// USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
	// DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	for(i=0; i<USART_TXRX_PK_NUM ; i++)
	{
		if( RS485_CHECK_BUF_VALID(UART3_rs485_st.RXBuff[i])==TRUE)  
		{
			if( RS485__msg_process( UART3_rs485_st.RXBuff[i]) )
			{ 
				memset(UART3_rs485_st.RXBuff[i], FALSE,10);
			} 
			break;
		}
	} 
	// enable_all_uart_interupt();	
	// DMA_ITConfig(RS485_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	// USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
}

u8 RS485__msg_process(u8 *tab)  
{   
	TRAN_D_struct   TRAN_info1,TRAN_info2;
	u8              fuc_dat[UART_RX_LEN];
	u8              dat[UART_RX_LEN];
	
	TRAN_info2.dest_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

	TRAN_info2.dest_addr[0] = 0x00;
	TRAN_info2.dest_addr[1] = 0x00;
	TRAN_info2.dest_addr[2] = 0x00;
	TRAN_info2.dest_addr[3] = 0x00;

	TRAN_info2.source_dev_num = ( DTN_JCZX_fanzhuang << 8 | DTN_JCZX_fanzhuang >> 8 );

	TRAN_info2.source_addr[0] = (u8)MCU_UID[0];
	TRAN_info2.source_addr[1] = (u8)(MCU_UID[0]>>8);
	TRAN_info2.source_addr[2] = (u8)(MCU_UID[0]>>16);
	TRAN_info2.source_addr[3] = (u8)(MCU_UID[0]>>24);

	TRAN_info2.TYPE_NUM = 4;
	TRAN_info2.data_len = 16 + 2;
 
	memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
	memcpy(fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	  

	TRAN_info1.source_dev_num = ( TRAN_info1.source_dev_num << 8 | TRAN_info1.source_dev_num >> 8 );

	TRAN_info1.dest_dev_num = ( TRAN_info1.dest_dev_num << 8 | TRAN_info1.dest_dev_num >> 8 );

	if( TRAN_info1.TYPE_NUM == 4 )
	{
		switch( fuc_dat[0] << 8 | fuc_dat[1] ) 
		{
			case  0x000b:     

			if( fuc_dat[2]==1  )
			{    
				key_tab[1] = 0;
				key_tab[0] = 1;
				LED_DAT[1] |= L_R1C1;
				LED_DAT[1] &= ~L_R1C2; 
			}

			if( fuc_dat[3] == 1  )
			{    
				key_tab[1] = 1;
				key_tab[0] = 0;
				LED_DAT[1] &= ~L_R1C1;
				LED_DAT[1] |= L_R1C2; 
			} 
			
			//判断第三个字节，是锁定的
			if( fuc_dat[4] == 0 )
			{    
				key_tab[2] = 0;
				LED_DAT[1] &= ~L_R1C3;
			}
			else
			{
				key_tab[2] = 1;   
				LED_DAT[1] |= L_R1C3;
			}
										
			if( fuc_dat[5] == 0 )
			{    
				key_tab[3] = 0;
				LED_DAT[1] &= ~L_R1C4;
			}
			else
			{
				key_tab[3] = 1;
				LED_DAT[1] |= L_R1C4;
			}
										
			if( fuc_dat[6] == 0 )
			{    
				key_tab[4]=0;
				LED_DAT[1]&=~L_R2C1;
			}
			else
			{
				key_tab[4]=1;
				LED_DAT[1]|= L_R2C1;
			}
										
			/*
			if( fuc_dat[7]==0  )//开关机
			{ 
				key_tab[5]=0;
				LED_DAT[1]&=~L_R2C2;
			}
			else if(fuc_dat[7]==2 )
			{
				key_tab[5]=2;//已经开机
				LED_DAT[1]|= L_R2C2;
			}
			else
			{
				key_tab[5]=1;  //正在开机
				LED_DAT[1]|= L_R2C2;
			}
			*/
			if( fuc_dat[7]==1  )//开关机
			{ 
				key_tab[5]=4;
				
			}
			
			if( fuc_dat[8]==0)
			{
				key_tab[6]=0;
				LED_DAT[1]&=~L_R2C3;
			}
			else 
			{	 
				key_tab[6]=1;
				LED_DAT[1]|= L_R2C3;
			}
											 
			LED_DAT[0]=0X00;
	 
			write_595_LED( LED_DAT,2);

			dat[0]=00;
			dat[1]=0x0b;

			memcpy( dat+2 , key_tab , 16 );
			RS485_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info2 , dat  ,TRAN_info1.data_len   ); 

			break; 
			
			case  0x000c: 
			if( fuc_dat[2] == 0 )//0是关闭
			{    
				key_tab[5] = 0;
				LED_DAT[1]&= ~L_R2C2;
			}
			
			if( fuc_dat[2] == 1  )//1是正在开
			{    
				key_tab[5] = 1;  //正在开机
			} 
			
			if( fuc_dat[2] == 2  )//2是已经开
			{    
				key_tab[5] = 2;//已经开机
				LED_DAT[1]|= L_R2C2;
			}
			
			LED_DAT[0] = 0X00;
			write_595_LED( LED_DAT,2);
			
			break;
										 
			default:  break; 
		}
	}
	return  1;
}
 
 

