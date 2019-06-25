#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"

  
void  ZIG1_UART_RX_load_buf(u8* buf,u8 len,u8  transid);

void  ZIG1_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);

void  ZIG1_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

void  ZIG1_CANCEL_SD_buf(u8 transid);

void  ZIG1_uart_fuc(void) ;

u8    ZIG1_msg_process(u8 *tab); 

void  ZIG1_dma_send_buf(void); 

void  ZIG1_uart_timeout_fution(void);

u8    ZIG1_CHECK_BUF_VALID(u8 *p) ;

void  ZIG1_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
void  ZIG1_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

uint8_t ZIG1_crc8(uint8_t *buf,uint8_t len);

 
void ZIG1_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen)
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

 
void ZIG1_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen)
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




u8 ZIG1_CHECK_BUF_VALID(u8 *p) 
{  
	u8 i=0;

	for(i=0;i<3;i++)
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
uint8_t ZIG1_crc8(uint8_t *buf,uint8_t len)
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
*串口1中断程序
*/
void UART4_IRQHandler(void)                	//串口1中断服务程序
{  
	u8	 transid=0,crc=0;
	u32 i,Data_Len=0;

	if(USART_GetITStatus(UART4, USART_IT_IDLE) != RESET) //判断是否是空闲标志
	{ 
		DMA_ClearFlag( DMA2_FLAG_GL3 );           // 清DMA标志位
		USART_ClearITPendingBit(UART4, USART_IT_IDLE);  //清除空闲标志
		i = UART4->SR;  //清除空闲标志
		i = UART4->DR; //清除空闲标志

		DMA_Cmd(ZIG1_DMA_RX_CH, DISABLE);// 禁用  避免出错
		Data_Len=UART_RX_LEN- DMA_GetCurrDataCounter(ZIG1_DMA_RX_CH);  

		// LED_485=~LED_485;
		if(Data_Len<UART_RX_LEN&&Data_Len>= 21  ) 
		{  
			i= 17+UART4_st.RXcun[16];
			if(  UART4_st.RXcun[0] ==0xAA&&UART4_st.RXcun[1] ==0xAA && UART4_st.RXcun[ i ]==0XBB&&UART4_st.RXcun[ i+1 ]==0XBB )
			{
				crc=ZIG1_crc8( (u8*) &UART4_st.RXcun[ 0 ], i+2);
				if(  UART4_st.RXcun[ i+2 ]  ==crc  )//判断CRC
				{
					//如果接收到数据
					transid =   UART4_st.RXcun[ 2 ] ;
					ZIG1_UART_RX_load_buf(UART4_st.RXcun,i+3,transid);     //载入到RX  FIFO   
				}
			}				
		} 
		else if( Data_Len<UART_RX_LEN&&Data_Len >= 7 )
		{
			if( UART4_st.RXcun[0] == 0xCC && UART4_st.RXcun[1] == 0xCC && UART4_st.RXcun[4] == 0XDD && UART4_st.RXcun[5] == 0XDD )
			{
				if( UART4_st.RXcun[6]  ==ZIG1_crc8(  &UART4_st.RXcun[0], 6) )//判断CRC
				{
					//如果接受到ACK   
					transid = UART4_st.RXcun[2] ;
					ZIG1_CANCEL_SD_buf(transid);  							      
				}
			}
		}
					   
		memset(UART4_st.RXcun, 0,i+2);

		ZIG1_DMA_RX_CH->CNDTR = UART_RX_LEN; //  重新赋值计数值，必须大于等于最大

		DMA_Cmd(ZIG1_DMA_RX_CH, ENABLE);//			
	} 		
} 
  
 


/************************************************* 
*串口1 DMA 中断
*/  
void  DMA2_Channel4_5_IRQHandler(void) 
{   
	if(DMA_GetITStatus( DMA2_FLAG_GL5) != RESET) // 
	{   
		UART4_st.ALL_tc_flg = TRUE; 

		DMA_Cmd(ZIG1_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送

		DMA_ClearITPendingBit(DMA2_FLAG_GL5);   

		// while(USART_GetFlagStatus(UART4,USART_FLAG_TC)!=SET){};//等待发送结束
		//  LED_485=~LED_485;
	}
}

 

/************************************************* 
*串口1   超时处理函数
*/
void  ZIG1_uart_timeout_fution(void) 
{  
	UART4_st.ACK_TIMEOUT_js++; 
}
 

 /************************************************* 
   接收载入缓存
*/ 	 
void   ZIG1_UART_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
	u8  i=0,cun_rx_lun;

	cun_rx_lun=UART4_st.uart_Rx_lun;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{
		if( ZIG1_CHECK_BUF_VALID(UART4_st.RXBuff[cun_rx_lun])==FALSE)//未使用的BUFF
		{
			memcpy((u8*)UART4_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
			break;
		}
		(cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
	} 

	// test   
	ZIG1_ACK_load_buf(0XCCCC,0XDDDD,0,transid);	
}
 

/*****************ACK 载入*************************/
void  ZIG1_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid)
{    
	u8  i = 0 ;
	u8  crc = 0 ;

	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( ZIG1_CHECK_BUF_VALID(UART4_st.ACKBuff[i]) == FALSE)//如果没有数据
		{
			UART4_st.ACKBuff[i][0] = hd>>8;
			UART4_st.ACKBuff[i][1] = hd ;
			UART4_st.ACKBuff[i][2] = transid ;
			UART4_st.ACKBuff[i][3] = t_status;
			UART4_st.ACKBuff[i][4] = tail>>8 ;
			UART4_st.ACKBuff[i][5] = tail  ;

			crc = ZIG1_crc8(&UART4_st.ACKBuff[i][0],6); 

			UART4_st.ACKBuff[i][6] = crc  ; 

			break;
		}
	}
}



/****************收到返回后 消除FIFO************************/
void ZIG1_CANCEL_SD_buf(u8 transid) 
{
	u8  i=0 ;

	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( ZIG1_CHECK_BUF_VALID(UART4_st.SDBuff[i])==TRUE)//如果没有数据
		{   
			if(transid == UART4_st.SDBuff[i][2])
			{
				if( i == UART4_st.uart_SD_lun )// 
				{
					UART4_st.ACK_RX_OK_flg = TRUE;  //设置ACK 接收标志位 OK
					UART4_st.ACK_TIMEOUT_js = 0;

					UART4_st.ACK_TIMEOUT_SUM = 0;//这条不及时清0 会有累计计数，导致有数据还没发就删除了

					memset(UART4_st.SDBuff[i], FALSE, 10 );//  清除前几位，会被判定为无效	
				}
				break;
			} 
		} 
	}
}

 
///************************************************* 
//*串口3  DMA 发送 
//*/ 
void  ZIG1_dma_send_buf(void)
{   
	u8  i=0,cun_sd_lun;

	if( UART4_st.ALL_tc_flg==TRUE )//DMA传送完毕
	{
		//检测出ACK缓存里是否有数据   ACK 优先发送
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{      
			if(ZIG1_CHECK_BUF_VALID(UART4_st.ACKBuff[i])==TRUE)//如果有数据
			{   

				memset(UART4_st.SDcun, FALSE,7);				

				memcpy((u8*)UART4_st.SDcun , (u8*)UART4_st.ACKBuff[i], 7 ); //把数据COPY到 	 

				memset(UART4_st.ACKBuff[i], FALSE,7); 						

				MYDMA_Config( ZIG1_DMA_TX_CH,(u32)&UART4->DR,(u32)UART4_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度UART4_SD_LEN. 

				MYDMATX_Enable(ZIG1_DMA_TX_CH,7);		

				UART4_st.ALL_tc_flg=FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 

				return;
			} 
		}
				 
		if(UART4_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART4_st.ACK_RX_OK_flg ==FALSE  )//超时进入或者收到应答进入
		{
			return;
		}	
		
		UART4_st.ACK_RX_OK_flg = FALSE;			
		UART4_st.ACK_TIMEOUT_js = 0;		

		disable_all_uart_interupt();	   

		//没有ACK时 发送数据
		cun_sd_lun= UART4_st.uart_SD_lun;
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{  
			if( ZIG1_CHECK_BUF_VALID(UART4_st.SDBuff[cun_sd_lun])==TRUE)//如果有数据
			{
				if( cun_sd_lun== UART4_st.uart_SD_lun  )
				{
					UART4_st.ACK_TIMEOUT_SUM++;
					if(UART4_st.ACK_TIMEOUT_SUM> RE_SEND_SUM2)
					{  
						UART4_st.ACK_TIMEOUT_SUM=0; 

						memset(UART4_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
						(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
						continue;
					}
				}		
										 
				memset(UART4_st.SDcun, FALSE,10);			

				memcpy((u8*)UART4_st.SDcun , (u8*)UART4_st.SDBuff[cun_sd_lun], UART4_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 

				MYDMA_Config( ZIG1_DMA_TX_CH,(u32)&UART4->DR,(u32)UART4_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度UART4_SD_LEN. 

				MYDMATX_Enable(ZIG1_DMA_TX_CH, UART4_st.SDBuff_len[cun_sd_lun] );		

				UART4_st.uart_SD_lun =cun_sd_lun; // 暂存
				UART4_st.ALL_tc_flg=FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 


				UART4_st.ACK_RX_OK_flg=FALSE;			
				UART4_st.ACK_TIMEOUT_js=0;			
													
				break;
			}
			(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
		}
		enable_all_uart_interupt();	
		//	DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		//  USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); 
	} 	 
}



/*****************发送载入BUF**************************/
void  ZIG1_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len )
{     
	u8  i=0,cun_sd_lun;
	u8  crc=0 ;

	if(len >UART_SD_LEN ) 
	{
		return; 
	}
	disable_all_uart_interupt();	
	//USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);  
	// DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun=  UART4_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{  
		if( ZIG1_CHECK_BUF_VALID(UART4_st.SDBuff[cun_sd_lun])==FALSE)//如果没有数据
		{ 
			UART4_st.SDBuff[cun_sd_lun][0]=d_head;//包头 
			UART4_st.SDBuff[cun_sd_lun][1]= d_head>>8;//包头
			UART4_st.SDBuff[cun_sd_lun][2]= UART4_st.txtransid;

			memcpy(&UART4_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART4_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART4_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len]= d_tail;   //包尾
			UART4_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1]= d_tail>>8;

			crc= ZIG1_crc8(&UART4_st.SDBuff[cun_sd_lun][0],3+sizeof(TRAN_D_struct)+len+2);

			UART4_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+2]=crc;

			UART4_st.SDBuff_len[cun_sd_lun]= 3+sizeof(TRAN_D_struct)+len+2  +1  ;


			UART4_st.txtransid++;	//序号++			
			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM -1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}

	enable_all_uart_interupt();	
	// DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	// USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); 
}


void ZIG1_uart_fuc(void) 
{
	u8  i=0 ;

	// USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);  
	// DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,DISABLE); 
	// disable_all_uart_interupt();	

	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{
		if( ZIG1_CHECK_BUF_VALID(UART4_st.RXBuff[i])==TRUE)  
		{
			if( ZIG1_msg_process( UART4_st.RXBuff[i]) )
			{ 
				memset(UART4_st.RXBuff[i], FALSE,10);
			} 

			break;
		}
	} 
	//  enable_all_uart_interupt();	
	// DMA_ITConfig(ZIG1_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	// USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); 
}



u8 ZIG1_msg_process(u8 *tab)  
{
	TRAN_D_struct   TRAN_info1 ;
	u8              fuc_dat[UART_RX_LEN]; 

	memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
	memcpy(fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	  

	TRAN_info1.source_dev_num = (TRAN_info1.source_dev_num<<8|TRAN_info1.source_dev_num>>8);

	TRAN_info1.dest_dev_num = (TRAN_info1.dest_dev_num<<8|TRAN_info1.dest_dev_num>>8);

	// if(TRAN_info1.TYPE_NUM==4)
	{
		Android_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );
	} 

	return  1;
}

 

