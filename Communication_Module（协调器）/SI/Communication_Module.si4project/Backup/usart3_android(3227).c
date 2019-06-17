#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"
#include "global.h"

  
void   Android_UART_RX_load_buf(u8* buf,u8 len,u8  transid);

void  Android_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid);
 
 
void  Android_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

 
void  Android_CANCEL_SD_buf(u8 transid);
	


void  Android_uart_fuc(void) ;

u8 Android_msg_process(u8 *tab ,u8  m_len  ) ; 

void  Android_dma_send_buf(void); 


void  Android_uart_timeout_fution(void);

u8    Android_CHECK_BUF_VALID(u8 *p) ;

void  Android_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen);
void  Android_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen);

uint8_t Android_crc8(uint8_t *buf,uint8_t len);
 
 
 
void Android_StrToHex(u8 *pbDest, u8 *pbSrc, int nLen)
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

 
void Android_HexToStr(u8 *pbDest, u8 *pbSrc, int nLen)
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




u8  Android_CHECK_BUF_VALID(u8 *p) 
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
uint8_t Android_crc8(uint8_t *buf,uint8_t len)
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
void USART3_IRQHandler(void)                	//串口1中断服务程序
{   
	u8	 transid=0,crc=0;
	u32 i,Data_Len=0;

	if(USART_GetITStatus(USART3, USART_IT_IDLE) != RESET) //判断是否是空闲标志
	{ 
		DMA_ClearFlag( DMA1_FLAG_GL3 );           // 清DMA标志位
		USART_ClearITPendingBit(USART3, USART_IT_IDLE);  //清除空闲标志
		i = USART3->SR;  //清除空闲标志
		i = USART3->DR; //清除空闲标志

		DMA_Cmd(Android_DMA_RX_CH, DISABLE);// 禁用  避免出错
		Data_Len=UART_RX_LEN- DMA_GetCurrDataCounter(Android_DMA_RX_CH);  

		// LED_232=~LED_232;
		if(Data_Len<UART_RX_LEN&&Data_Len>= 21  ) 
		{  
			i= 17+UART3_android_st.RXcun[16];
			if(  UART3_android_st.RXcun[0] ==0xAA&&UART3_android_st.RXcun[1] ==0xAA && UART3_android_st.RXcun[ i ]==0XBB&&UART3_android_st.RXcun[ i+1 ]==0XBB )
			{
				crc=Android_crc8( (u8*) &UART3_android_st.RXcun[ 0 ], i+2);

				if(  UART3_android_st.RXcun[ i+2 ]  ==crc  )//判断CRC
				{
					//如果接收到数据
					transid =   UART3_android_st.RXcun[ 2 ] ;
					Android_UART_RX_load_buf(UART3_android_st.RXcun,i+3,transid);     //载入到RX  FIFO   
				}
			}
		} 
		else if(Data_Len<UART_RX_LEN&&Data_Len>= 7  )
		{
			if( UART3_android_st.RXcun[0] ==0xCC&&UART3_android_st.RXcun[1] ==0xCC&&UART3_android_st.RXcun[ 4 ]==0XDD&&UART3_android_st.RXcun[ 5 ]==0XDD )
			{
				if(  UART3_android_st.RXcun[ 6 ]  ==Android_crc8(  &UART3_android_st.RXcun[ 0 ], 6)  )//判断CRC
				{
					//如果接受到ACK   
					transid =   UART3_android_st.RXcun[ 2 ] ;
					Android_CANCEL_SD_buf(transid);  
				}
			}
		}					 
		memset(UART3_android_st.RXcun, 0,i+2);

		Android_DMA_RX_CH->CNDTR=UART_RX_LEN; //  重新赋值计数值，必须大于等于最大

		DMA_Cmd(Android_DMA_RX_CH, ENABLE);//
	} 		   
} 


/************************************************* 
*串口1 DMA 中断
*/  
void  DMA1_Channel2_IRQHandler(void) 
{     
	if(DMA_GetITStatus( DMA1_IT_TC2) != RESET) // 
	{   
		UART3_android_st.ALL_tc_flg=TRUE; 

		DMA_Cmd(Android_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送

		DMA_ClearITPendingBit(DMA1_FLAG_GL2);   // 

		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET){};//等待发送结束

//		LED_232 =~LED_232;
	}
}

 

/************************************************* 
*串口1   超时处理函数
*/
void  Android_uart_timeout_fution(void) 
{  
	UART3_android_st.ACK_TIMEOUT_js++; 
}
 

 /************************************************* 
   接收载入缓存
*/ 	 
void   Android_UART_RX_load_buf(u8* buf,u8 len,u8  transid)
{ 
	u8  i=0,cun_rx_lun;

	cun_rx_lun=UART3_android_st.uart_Rx_lun;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{
		if( Android_CHECK_BUF_VALID(UART3_android_st.RXBuff[cun_rx_lun]) == FALSE)//未使用的BUFF
		{
			memcpy((u8*)UART3_android_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
			break;
		}
		(cun_rx_lun>=USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
	} 

	// test   
	Android_ACK_load_buf(0XCCCC,0XDDDD,0,transid);	
}
 

/*****************ACK 载入*************************/
void  Android_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid)
{     
	u8  i = 0 ;
	u8  crc = 0 ;

	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( Android_CHECK_BUF_VALID(UART3_android_st.ACKBuff[i]) == FALSE)//如果没有数据
		{
			UART3_android_st.ACKBuff[i][0] = hd>>8;
			UART3_android_st.ACKBuff[i][1] = hd ;
			UART3_android_st.ACKBuff[i][2] = transid ;
			UART3_android_st.ACKBuff[i][3] = t_status;
			UART3_android_st.ACKBuff[i][4] = tail>>8 ;
			UART3_android_st.ACKBuff[i][5] = tail  ;

			crc=Android_crc8(&UART3_android_st.ACKBuff[i][0],6); 

			UART3_android_st.ACKBuff[i][6]= crc  ; 

			break;
		}
	}			    		 
}



/****************收到返回后 消除FIFO************************/
void Android_CANCEL_SD_buf(u8 transid) 
{
	u8  i=0 ;

	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( Android_CHECK_BUF_VALID(UART3_android_st.SDBuff[i])==TRUE)//如果没有数据
		{   
			if(transid==UART3_android_st.SDBuff[i][2])
			{
				if( i== UART3_android_st.uart_SD_lun )// 
				{
					UART3_android_st.ACK_RX_OK_flg=TRUE;  //设置ACK 接收标志位 OK
					UART3_android_st.ACK_TIMEOUT_js=0;

					//fbd
					UART3_android_st.ACK_TIMEOUT_SUM	=0;//这条不及时清0 会有累计计数，导致有数据还没发就删除了

					memset(UART3_android_st.SDBuff[i], FALSE, 10 );//  清除前几位，会被判定为无效	
					//  memset(UART3_android_st.SDBuff[i]+5, 5, 5 );//  清除前几位，会被判定为无效	
				}
				break;
			} 
		} 
	}
}

 
///************************************************* 
//*串口3  DMA 发送 
//*/ 
void  Android_dma_send_buf(void)
{   
	u8  i=0,cun_sd_lun;

	//  if( UART3_android_st.ALL_tc_flg==TRUE )//DMA传送完毕
	{
		//检测出ACK缓存里是否有数据   ACK 优先发送
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{      
			if(Android_CHECK_BUF_VALID(UART3_android_st.ACKBuff[i])==TRUE)//如果有数据
			{   
				memset(UART3_android_st.SDcun, FALSE,7);				

				memcpy((u8*)UART3_android_st.SDcun , (u8*)UART3_android_st.ACKBuff[i], 7 ); //把数据COPY到 	 

				memset(UART3_android_st.ACKBuff[i], FALSE,7); 						

				MYDMA_Config( Android_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_android_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 

				MYDMATX_Enable(Android_DMA_TX_CH,7);		
				USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //使能串口3的DMA发送 接收 
				UART3_android_st.ALL_tc_flg=FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 

				return;
			} 
		}

		if(UART3_android_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART3_android_st.ACK_RX_OK_flg ==FALSE  )//超时进入或者收到应答进入
		{
			return;
		}		

		disable_all_uart_interupt();

		UART3_android_st.ACK_RX_OK_flg=FALSE;			
		UART3_android_st.ACK_TIMEOUT_js=0;		

		//没有ACK时 发送数据
		cun_sd_lun= UART3_android_st.uart_SD_lun;
		for(i=0;i<USART_TXRX_PK_NUM ;i++)
		{  
			if( Android_CHECK_BUF_VALID(UART3_android_st.SDBuff[cun_sd_lun])==TRUE)//如果有数据
			{
				if( cun_sd_lun== UART3_android_st.uart_SD_lun  )
				{
					UART3_android_st.ACK_TIMEOUT_SUM++;
					if(UART3_android_st.ACK_TIMEOUT_SUM> RE_SEND_SUM)
					{  
						UART3_android_st.ACK_TIMEOUT_SUM=0; 

						memset(UART3_android_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
						(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
						continue;
					}
				}		

				memset(UART3_android_st.SDcun, FALSE,10);			

				memcpy((u8*)UART3_android_st.SDcun , (u8*)UART3_android_st.SDBuff[cun_sd_lun], UART3_android_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 

				MYDMA_Config( Android_DMA_TX_CH,(u32)&USART3->DR,(u32)UART3_android_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 

				MYDMATX_Enable(Android_DMA_TX_CH, UART3_android_st.SDBuff_len[cun_sd_lun] );		
				USART_DMACmd(USART3,USART_DMAReq_Tx,ENABLE); //使能串口3的DMA发送 接收 
				UART3_android_st.uart_SD_lun =cun_sd_lun; // 暂存
				UART3_android_st.ALL_tc_flg=FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 

				UART3_android_st.ACK_RX_OK_flg=FALSE;			
				UART3_android_st.ACK_TIMEOUT_js=0;			

				break;
			}
			(cun_sd_lun>=USART_TXRX_PK_NUM-1)?(cun_sd_lun=0):(cun_sd_lun++);	
		}
		enable_all_uart_interupt();	
		//	DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		//  USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
		LED2 = 0;
	} 
}



/*****************发送载入BUF**************************/
void  Android_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len )
{     
	u8  i=0,cun_sd_lun;
	u8  crc=0 ;

	if(len >UART_SD_LEN ) 
	{
		return; 
	}
	disable_all_uart_interupt();	
	//USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
	//DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun = UART3_android_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{  
		if( Android_CHECK_BUF_VALID(UART3_android_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			UART3_android_st.SDBuff[cun_sd_lun][0] = d_head;//包头 
			UART3_android_st.SDBuff[cun_sd_lun][1] = d_head>>8;//包头
			UART3_android_st.SDBuff[cun_sd_lun][2] = UART3_android_st.txtransid;

			memcpy(&UART3_android_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART3_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART3_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len] = d_tail;   //包尾
			UART3_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+1] = d_tail>>8;

			crc= Android_crc8(&UART3_android_st.SDBuff[cun_sd_lun][0],3+sizeof(TRAN_D_struct)+len+2);

			UART3_android_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)+len+2] = crc;

			UART3_android_st.SDBuff_len[cun_sd_lun] = 3 + sizeof(TRAN_D_struct) + len + 2 + 1 ;


			UART3_android_st.txtransid++;	//序号++			
			break;
		}
		(cun_sd_lun>=USART_TXRX_PK_NUM -1)?(cun_sd_lun=0):(cun_sd_lun++);	
	}
	enable_all_uart_interupt();	
	// DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	// USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
}


void Android_uart_fuc(void) 
{
	u8  i=0 ;

	// USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);  
	// DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,DISABLE); 
	//   disable_all_uart_interupt();	

	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{
		if( Android_CHECK_BUF_VALID(UART3_android_st.RXBuff[i])==TRUE)  
		{
			
			if( Android_msg_process( UART3_android_st.RXBuff[i],  UART3_android_st.RXBuff_len[i]  ) )
			{ 
				memset(UART3_android_st.RXBuff[i], FALSE,10);
			} 
			break;
		}
	} 
	//   enable_all_uart_interupt();	
	// DMA_ITConfig(Android_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	// USART_ITConfig(USART3, USART_IT_IDLE, ENABLE); 
}


u8 Android_msg_process(u8 *tab ,u8  m_len  )  
{
	TRAN_D_struct   TRAN_info1 ;
	u8   fuc_dat[UART_RX_LEN];
	u8    dat[UART_RX_LEN];

	memcpy(&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
	memcpy(fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	  


	TRAN_info1.source_dev_num=(TRAN_info1.source_dev_num<<8|TRAN_info1.source_dev_num>>8);

	TRAN_info1.dest_dev_num=(TRAN_info1.dest_dev_num<<8|TRAN_info1.dest_dev_num>>8);


	if(TRAN_info1.TYPE_NUM==4)
	{
		switch(  fuc_dat[0]<<8|fuc_dat[1]  ) 
		{
			case  0x0001:    // //允许入网开关， 倒计时 
			{
				join_LED_S=fuc_dat[2]*2 ;

				ZIG1_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len ); 


				TRAN_info1.dest_dev_num= 0;

				TRAN_info1.dest_addr[0]=0x00;
				TRAN_info1.dest_addr[1]=0x00;
				TRAN_info1.dest_addr[2]=0x00;
				TRAN_info1.dest_addr[3]=0x00;

				TRAN_info1.source_dev_num= 0;

				TRAN_info1.source_addr[0]=0;
				TRAN_info1.source_addr[1]=0;
				TRAN_info1.source_addr[2]=0;
				TRAN_info1.source_addr[3]=0;

				TRAN_info1.TYPE_NUM= 4   ;   //第4类
				TRAN_info1.data_len= 2   ; 

				dat[0]=0;
				dat[1]=1;
				printf("this is Rx preadth\n");
				Android_SD_load_buf( 0xAAAA  ,0XBBBB ,&TRAN_info1 , dat , TRAN_info1.data_len  ); 

			} break;
			
			case  0x0002:   //删除设备 2字节设备号+4字节地址+8字节zigbee地址
			{
				ZIG1_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );
			} break; 
			
			case  0x0004:    //上传设备
			{
			
			}	break;
			
			case  0x0005:    //控制设备
			{
				ZIG1_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );
			} break;
			
			case  0x0006:    //查询设备
			{
				ZIG1_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );	
			} break;
			
			case  0x0008:    // 学习捕获红外遥控波形  CMD  0X0008 + 存储号 （最大50个） 
			{
				m_mode = 1;//进入捕获红外

				join_LED_S = 60*2 ;
				IR_CAP1.CAP_IR_WAVEFORM_TIME = 60*1000;         //等待60S  
				IR_CAP1.CAP_IR_store_num = fuc_dat[2];
				IR_CAP_START_FLG = 0;

			}break;
			
			case  0x0009:    // 发送 捕获红外遥控波形   CMD  0X0009 + 存储号 （最大50个） 
			{
				m_mode = 2;//  
				IR_CAP1.CAP_IR_store_num = fuc_dat[2]; 
			} break;
			
			case  0x000A:    // 控制翻转屏控制板  CMD 0X000a ，加2字节 （第一字节1.显示器开机，2.显示器关机；第二字节1.翻转屏上升2.翻转屏下降 3.翻转屏停止  ） 
			{
				//RS485_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );
			} break;		
			
			case  0x000b:    //  
			{
				//RS485_SD_load_buf(0xAAAA ,0XBBBB , &TRAN_info1, fuc_dat ,TRAN_info1.data_len );
			} break;

			default:  break; 
		}
	}
	return  1;
}

