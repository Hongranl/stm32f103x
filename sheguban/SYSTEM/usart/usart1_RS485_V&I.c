 #include "global.h"


TRAN_D_struct   RS485_V_I_TRAN_info  ;



void  RS485_V_I_SD_load_buf2(  u8 * urt_buf ,u8  len   );


void  RS485_V_I_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);  //应答载入缓存

void  RS485_V_I_RX_load_buf(u8* buf,u8 len,u8  transid); //接收载入缓存

void  RS485_V_I_CANCEL_SD_buf(u8 transid);           //删除发送缓存
 
void  RS485_V_I_SD_load_buf( u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len   );   //发送载入缓存
 
void  RS485_V_I_dma_send_buf(void);             //DMA发送函数

void   RS485_V_I_timeout_fution(void) ;         //超时函数


void RS485_V_I_uart_fuc(void);    //接收到数据后处理函数


u8    RS485_V_I__msg_process(u8 *tab) ;  
 

u8  RS485_V_I_CHECK_BUF_VALID(u8 *p) ;    //检查数据不为空

void RS485_V_I_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
void RS485_V_I_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

 
uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len);       //校验





 
void RS485_V_I_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen)
{
	char h1,h2;
	u8  s1,s2;
	int i;

	for (i=0; i<nLen; i++)
	{
		h1 = pbSrc[2*i];
		h2 = pbSrc[2*i+1];

		s1 = toupper(h1) - 0x30;
		if (s1 > 9) 
		s1 -= 7;

		s2 = toupper(h2) - 0x30;
		if (s2 > 9) 
		s2 -= 7;

		pbDest[i] = s1*16 + s2;
	}
}

 
void RS485_V_I_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen)
{
	char	ddl,ddh;
	int i;

	for (i=0; i<nLen; i++)
	{
		ddh = 48 + pbSrc[i] / 16;
		ddl = 48 + pbSrc[i] % 16;
		if (ddh > 57) ddh = ddh + 7;
		if (ddl > 57) ddl = ddl + 7;
		pbDest[i*2] = ddh;
		pbDest[i*2+1] = ddl;
	}

	pbDest[nLen*2] = '\0';
}


u8  RS485_V_I_CHECK_BUF_VALID(u8 *p)//不全空的话 视为有效
{  
	u8  i=0;

	for(i=0;i<3;i++)
	{
		if( p[i] !=0 )
		{
			return TRUE;
		} 
	}
	return FALSE;
}


/*********************************************************************
 *  crc
 */
/*********************************************************************
 *  crc
 */
uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len)
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


void USART1_IRQHandler(void)                	//串口1中断服务程序
{  
	u8	 transid = 0,crc = 0;
	u32 i,Data_Len = 0;

	if(USART_GetITStatus( USART1, USART_IT_IDLE) != RESET ) //判断是否是空闲标志
	{ 
		DMA_ClearFlag( DMA1_FLAG_GL5 );           					// 清DMA标志位
		USART_ClearITPendingBit( USART1, USART_IT_IDLE );  	//清除空闲标志
		i = USART1->SR;  		//清除空闲标志
		i = USART1->DR; 		//清除空闲标志

		DMA_Cmd(UART1_DMA_RX_CH, DISABLE);// 禁用  避免出错
		Data_Len = UART_RX_LEN - DMA_GetCurrDataCounter(UART1_DMA_RX_CH);  

		send_urt_time = 50;

		if(Data_Len < UART_RX_LEN && Data_Len >= 21 ) 
		{  
			if((UART1_RS485_V_I_st.RXcun[0] == 0xAA ) && ( UART1_RS485_V_I_st.RXcun[1] == 0xAA )) //判断数据包的帧头
			{
				i = 17 + UART1_RS485_V_I_st.RXcun[16];
				if(( UART1_RS485_V_I_st.RXcun[i] == 0xBB ) && ( UART1_RS485_V_I_st.RXcun[i+1] == 0xBB )) //判断数据包的包尾
				{
					crc = RS485_V_I_crc8( (u8*) &UART1_RS485_V_I_st.RXcun[0], i+2);  //CRC
					if( UART1_RS485_V_I_st.RXcun[i+2] == crc )		//判断CRC
					{
						//如果接收到数据
						transid = UART1_RS485_V_I_st.RXcun[2] ;
						RS485_V_I_RX_load_buf(UART1_RS485_V_I_st.RXcun,i+3,transid);     //载入到RX  FIFO   
						LED_COMM =~LED_COMM; 
					}
				}
			}
		} 

		memset(UART1_RS485_V_I_st.RXcun, 0,i+2);  //清除

		UART1_DMA_RX_CH->CNDTR = UART_RX_LEN; //  重新赋值计数值，必须大于等于最大

		DMA_Cmd(UART1_DMA_RX_CH, ENABLE);//	
    
	}  			 
}


/*****************DMA 发送完中断************************/
void  DMA1_Channel4_IRQHandler(void) 
{   
	if(DMA_GetITStatus( DMA1_IT_TC4) != RESET) // 
	{   
		UART1_RS485_V_I_st.ALL_tc_flg=TRUE;   
		DMA_Cmd(UART1_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送
		DMA_ClearITPendingBit(DMA1_FLAG_GL4);   //DMA1_FLAG_GL2

		while(USART_GetFlagStatus(USART1,USART_FLAG_TC)!=SET){};//等待发送结束
		RS485_CON1 = 0;	//接收
		RS485_CON1 = 0;	//接收
		LED_COMM=~LED_COMM;
	}
}
 
/************************************************* 
*串口1   超时处理函数
*/
void   RS485_V_I_timeout_fution(void) 
{  
	UART1_RS485_V_I_st.ACK_TIMEOUT_js++;
}
 
 
 /*************************************************  
   接收载入缓存*/

void RS485_V_I_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
	u8  i=0,cun_rx_lun;

	cun_rx_lun = UART1_RS485_V_I_st.uart_Rx_lun;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.RXBuff[cun_rx_lun]) == FALSE)//未使用的BUFF
		{
			memcpy((u8*)UART1_RS485_V_I_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
			break;
		}
		(cun_rx_lun >= USART_TXRX_PK_NUM - 1)?(cun_rx_lun = 0):(cun_rx_lun++);
	} 
	// test   
	// RS485_ACK_load_buf(0XCCCC,0XDDDD,0,transid);		
}
  	 
  
/****************收到返回后 消除FIFO************************/
void RS485_V_I_CANCEL_SD_buf(u8 transid) 
{
	u8 i = 0;

	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[i]) == TRUE)//如果没有数据
		{   		     
			if(transid == UART1_RS485_V_I_st.SDBuff[i][2])
			{
				if( i == UART1_RS485_V_I_st.uart_SD_lun )// 
				{
					UART1_RS485_V_I_st.ACK_RX_OK_flg = TRUE;  //设置ACK 接收标志位 OK
					UART1_RS485_V_I_st.ACK_TIMEOUT_js = 0;
				}

				memset(UART1_RS485_V_I_st.SDBuff[i], FALSE, 10 );//  清除前几位，会被判定为无效	
				break;
			} 
		} 
	} 
}

 
///************************************************* 
//*串口1  DMA 发送 
//*/ 
void  RS485_V_I_dma_send_buf(void)
{   
	u8  i=0,cun_sd_lun;
      
	//if( UART1_RS485_V_I_st.ALL_tc_flg==TRUE )//DMA传送完毕
	{
		if( UART1_RS485_V_I_st.ACK_TIMEOUT_js<TIMEOUT_SUM_2 && UART1_RS485_V_I_st.ACK_RX_OK_flg == FALSE )//  超时未到时间，而且 应答标志未收到OK   继续等待
		{ 
			return;
		}			 
		UART1_RS485_V_I_st.ACK_RX_OK_flg = FALSE;			
		UART1_RS485_V_I_st.ACK_TIMEOUT_js = 0;	

		USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE);  


		//没有ACK时 发送数据
		cun_sd_lun = UART1_RS485_V_I_st.uart_SD_lun;
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{  
			if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == TRUE)//如果有数据
			{
				if( cun_sd_lun == UART1_RS485_V_I_st.uart_SD_lun  )
				{
					UART1_RS485_V_I_st.ACK_TIMEOUT_SUM++;
					if(UART1_RS485_V_I_st.ACK_TIMEOUT_SUM > RE_SEND_SUM_2)
					{  
						UART1_RS485_V_I_st.ACK_TIMEOUT_SUM = 0; 

						memset(UART1_RS485_V_I_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
						(cun_sd_lun >= USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
						continue;
					}
				}					
									 
				memset(UART1_RS485_V_I_st.SDcun, FALSE,10);			

				memcpy((u8*)UART1_RS485_V_I_st.SDcun , (u8*)UART1_RS485_V_I_st.SDBuff[cun_sd_lun], UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 

				RS485_CON1 = 1;		//485  发送使能
				RS485_CON1 = 1;		//485  发送使能 
				MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 

				MYDMATX_Enable(UART1_DMA_TX_CH, UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] );		

				USART_Cmd(USART1, ENABLE);                    //使能串口 

				UART1_RS485_V_I_st.uart_SD_lun = cun_sd_lun; // 暂存
				UART1_RS485_V_I_st.ALL_tc_flg = FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 

				UART1_RS485_V_I_st.ACK_RX_OK_flg = FALSE;			
				UART1_RS485_V_I_st.ACK_TIMEOUT_js = 0;			
										
				break;
			}
			(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
		}

		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 		 
	} 
}

/*****************发送载入BUF**************************/
void  RS485_V_I_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len )
{     
	u8  i=0,cun_sd_lun;
	u8  crc = 0 ,z_c = 0;
	 
	if( len > UART_SD_LEN ) 
	{
		return; 
	}
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  //临界点
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun = UART1_RS485_V_I_st.uart_SD_lun ;
	for(i = 0; i<USART_TXRX_PK_NUM ;i++)
	{  
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			UART1_RS485_V_I_st.SDBuff[cun_sd_lun][0] = d_head;//包头 
			UART1_RS485_V_I_st.SDBuff[cun_sd_lun][1] = d_head>>8;//包头
			UART1_RS485_V_I_st.SDBuff[cun_sd_lun][2] = UART1_RS485_V_I_st.txtransid;

			memcpy(&UART1_RS485_V_I_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART1_RS485_V_I_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART1_RS485_V_I_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len] = d_tail;   //包尾
			UART1_RS485_V_I_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1] = d_tail>>8;

			z_c = 3 + sizeof(TRAN_D_struct) + len + 2;
			crc = RS485_V_I_crc8(&UART1_RS485_V_I_st.SDBuff[cun_sd_lun][0],z_c);  //CRC8
									 
			UART1_RS485_V_I_st.SDBuff[cun_sd_lun][z_c] = crc;

			UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] = z_c + 1;

			UART1_RS485_V_I_st.txtransid++;	//序号++		

			break;
		}
		(cun_sd_lun >= USART_TXRX_PK_NUM -1)?(cun_sd_lun = 0):(cun_sd_lun++);	
	}
				 
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE);  //临界点
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 			 
}


/*****************发送载入BUF**************************/
void  RS485_V_I_SD_load_buf2( u8 * urt_buf ,u8 len )
{     
	u8 i=0,cun_sd_lun;
 
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  //临界点
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 


	cun_sd_lun =  UART1_RS485_V_I_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			memcpy((u8*)&UART1_RS485_V_I_st.SDBuff[cun_sd_lun][0], urt_buf,len);

			UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun]=len;

			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}
				
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE);  //临界点
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}

 
void RS485_V_I_uart_fuc(void) 
{
	u8 i = 0;

	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  //临界点
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 
    
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.RXBuff[i]) == TRUE)  
		{
			if( RS485_V_I__msg_process( UART1_RS485_V_I_st.RXBuff[i]) )
			{ 
				memset(UART1_RS485_V_I_st.RXBuff[i], FALSE,10);
			} 
			break;
		}
	} 

	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE);  //临界点
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}

 
//指令处理过程
u8  RS485_V_I__msg_process(u8 *tab)  
{
	TRAN_D_struct   TRAN_info1,TRAN_info2;
	u8   fuc_dat[UART_RX_LEN];
	u8   dat[UART_RX_LEN];

	memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
	memcpy(fuc_dat , (u8*)tab + 17,TRAN_info1.data_len);	  

	TRAN_info1.source_dev_num = (TRAN_info1.source_dev_num<<8|TRAN_info1.source_dev_num>>8); 	//源地址

	TRAN_info1.dest_dev_num = (TRAN_info1.dest_dev_num<<8|TRAN_info1.dest_dev_num>>8);  			//目标地址


	if(TRAN_info1.TYPE_NUM == 1)  //数据格式号
	{
		switch( fuc_dat[0] << 8 | fuc_dat[1] )   //CMD
		{
			case  0x0000:     //查询版本号
			{

			} break;
			
			case  0x0001:    //获得下一级  设备识别号 和地址 
			{
				
			} break;
			
			case  0x0002:   //获得上一级  设备识别号 和地址 
			{
				if(TRAN_info1.source_dev_num == DTN_android)  
				{ 
					search_addr_flg = 1;
				}							   
			} break;
			
			default:  break; 
		} 
	}
	else if(TRAN_info1.TYPE_NUM == 2)  //数据格式号
	{
		switch( fuc_dat[0]<<8|fuc_dat[1] ) 
		{
			case  0x0000:     //查询版本号
			{

			} break;
			
			case  0x0001:   
			{
			
			}	break;
			
			case  0x0002: 
			{
			
			}	break; 
			
			case  0x0004: //查收设故盒编码号
			{
				if( TRAN_info1.source_dev_num ==  DTN_16L_2T_3A|| TRAN_info1.source_dev_num == DTN_16L_5T_1A
																					||  TRAN_info1.source_dev_num == DTN_8L_2T_16A )
				{
					if( MY_CODE == fuc_dat[2])
					{
						LED_EEROR = 0;  //故障灯亮
					}  
					else
					{
						LED_EEROR = 1;
					}
				} 
			}break;
			
			case  0x0005:  //设置故障
			{				
				MY_CODE = read_code();   //拨码编号
				if((TRAN_info1.source_dev_num == DTN_android ) && ( TRAN_info1.dest_dev_num == MY_SWTYPE ) && ( fuc_dat[2] == MY_CODE ))//  
				{
					TRAN_info2.dest_dev_num = (DTN_android<<8|DTN_android>>8);

					TRAN_info2.dest_addr[0] = TRAN_info1.source_addr[0];
					TRAN_info2.dest_addr[1] = TRAN_info1.source_addr[1];
					TRAN_info2.dest_addr[2] = TRAN_info1.source_addr[2];
					TRAN_info2.dest_addr[3] = TRAN_info1.source_addr[3];

					TRAN_info2.source_dev_num = (MY_SWTYPE<<8|MY_SWTYPE>>8);

					TRAN_info2.source_addr[0] = (u8)MCU_UID[0];
					TRAN_info2.source_addr[1] = (u8)(MCU_UID[0]>>8);
					TRAN_info2.source_addr[2] = (u8)(MCU_UID[0]>>16);
					TRAN_info2.source_addr[3] = (u8)(MCU_UID[0]>>24);

					TRAN_info2.TYPE_NUM = 2;   //第2类
					TRAN_info2.data_len = 2 + 3 + 16;

					dat[0] = 00;  
					dat[1] = 06;   //  
					dat[2] = MY_CODE;  
					dat[3] = MY_SWTYPE>>8; 
					dat[4] = MY_SWTYPE ; 

					memcpy(MY_JDQ_TAB , &fuc_dat[5] , 16 );	

					memcpy( dat + 5 , MY_JDQ_TAB ,16 );	   

					//485
					RS485_V_I_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info2 , dat ,TRAN_info2.data_len ); 
					BEEP = 1;  
					delay_ms(200);
					BEEP = 0; 
					memset( ACCIDENTAL_time,0,16 ); 
				}   	
			}break; 
			
			
			case  0x0006: //查询故障
			{				
				MY_CODE = read_code();   //拨码编号
				if(( TRAN_info1.source_dev_num == DTN_android )&&( TRAN_info1.dest_dev_num == MY_SWTYPE )&&( fuc_dat[2] == MY_CODE ))// 
				{
					if( fuc_dat[2] == MY_CODE )
					{ 
						TRAN_info2.dest_dev_num = (DTN_android<<8|DTN_android>>8);

						TRAN_info2.dest_addr[0] = TRAN_info1.source_addr[0];
						TRAN_info2.dest_addr[1] = TRAN_info1.source_addr[1];
						TRAN_info2.dest_addr[2] = TRAN_info1.source_addr[2];
						TRAN_info2.dest_addr[3] = TRAN_info1.source_addr[3];

						TRAN_info2.source_dev_num = (MY_SWTYPE<<8|MY_SWTYPE>>8);

						TRAN_info2.source_addr[0] = (u8)MCU_UID[0];
						TRAN_info2.source_addr[1] = (u8)(MCU_UID[0]>>8);
						TRAN_info2.source_addr[2] = (u8)(MCU_UID[0]>>16);
						TRAN_info2.source_addr[3] = (u8)(MCU_UID[0]>>24);

						TRAN_info2.TYPE_NUM = 2;   //第2类
						TRAN_info2.data_len = 2 + 3 + 16;

						dat[0] = 00;  
						dat[1] = 06;   //  
						dat[2] = MY_CODE;  
						dat[3] = MY_SWTYPE >> 8; 
						dat[4] = MY_SWTYPE ; 

						memcpy( dat +5 , MY_JDQ_TAB , 16 );	   

						//485
						RS485_V_I_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info2 , dat ,TRAN_info2.data_len );  

						BEEP = 1;  
						delay_ms(200);
						BEEP = 0; 
					}
				}   	
			}break; 
			
			default:  break; 
		}	 
	}
	else
	{

	}
	
	return 1;
}



