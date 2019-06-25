#include "sys.h"
#include "usart_cfg.h"
#include "dma.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "string.h"
#include "stdio.h"
#include "timer.h"

#include "bsp_internal_flash.h" 

  
void   Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8  transid);

void   Rs485_COMM_ACK_load_buf(u16  hd, u16  tail,u8 t_status , u8 transid); 
 
void   Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct  * TRAN_info , u8 * urt_buf ,u8  len );

	
void Rs485_COMM_uart_fuc(void) ;

u8  Rs485_COMM_msg_process(char *tab); 
 
void  Rs485_COMM_dma_send_buf(void); 

void  Rs485_COMM_uart_timeout_fution(void);

u8  Rs485_COMM_CHECK_BUF_VALID(u8 *p) ;

uint8_t Rs485_COMM_crc8(uint8_t *buf,uint8_t len);

void  Rs485_COMM_SD_load_buf2( u8 * urt_buf ,u8 len );

extern u8 gb_is_DMA_send_OK ;
extern u8  gb_time_count_send;
extern u8 gb_J_status ;


u8  Rs485_COMM_CHECK_BUF_VALID(u8 *p) 
{  
	u8 i = 0;
	
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
uint8_t Rs485_COMM_crc8(uint8_t *buf,uint8_t len)
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
*串口1中断程序
*/
void USART2_IRQHandler(void)                	//串口1中断服务程序
{  
	u8	transid = 0,crc = 0;
	u32 i, Data_Len = 0;
	gb_is_DMA_send_OK = 0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) //判断是否是空闲标志
	{ 
		DMA_ClearFlag( DMA1_FLAG_GL6 );           // 清DMA标志位
		USART_ClearITPendingBit(USART2, USART_IT_IDLE);  //清除空闲标志

		i = USART2->SR;  //清除空闲标志
		i = USART2->DR; //清除空闲标志

		DMA_Cmd(RS485_COMM_DMA_RX_CH, DISABLE);// 禁用  避免出错
		Data_Len=UART_RX_LEN- DMA_GetCurrDataCounter(RS485_COMM_DMA_RX_CH);  

		//LED1=!LED1; 
		send_urt_time=50;

		if(Data_Len<UART_RX_LEN&&Data_Len >= 21 ) 
		{  
			if( UART2_RS485_COMM_st.RXcun[0] ==0xAA&&UART2_RS485_COMM_st.RXcun[1] ==0xAA)
			{
				i= 17+UART2_RS485_COMM_st.RXcun[16];
				if( UART2_RS485_COMM_st.RXcun[ i ]==0XBB&&UART2_RS485_COMM_st.RXcun[ i+1 ]==0XBB )
				{
					crc=Rs485_COMM_crc8( (u8*) &UART2_RS485_COMM_st.RXcun[ 0 ], i+2);

					if(  UART2_RS485_COMM_st.RXcun[ i+2 ]  ==crc  )//判断CRC
					{
						//如果接收到数据
						transid =   UART2_RS485_COMM_st.RXcun[ 2 ] ;
						Rs485_COMM_UART_RX_load_buf(UART2_RS485_COMM_st.RXcun,i+3,transid);     //载入到RX  FIFO   
						   //LED1=~LED1; 
					}
					else
					{
						//LED2 = 0;
					}
				}
			}	
		} 
		memset(UART2_RS485_COMM_st.RXcun, 0,i+2);

		RS485_COMM_DMA_RX_CH->CNDTR=UART_RX_LEN; //  重新赋值计数值，必须大于等于最大

		DMA_Cmd(RS485_COMM_DMA_RX_CH, ENABLE);//
	} 		

	gb_is_DMA_send_OK = 1;
} 
  
 
/************************************************* 
*串口1 DMA 中断
*/  
void  DMA1_Channel7_IRQHandler(void) 
{   
	if(DMA_GetITStatus( DMA1_IT_TC7) != RESET) // 
	{  
		LED1 = !LED1;
		//LED2 = 1; 	
		gb_time_count_send = 0;
		gb_is_DMA_send_OK = 0;

		
		UART2_RS485_COMM_st.ALL_tc_flg = TRUE; 

		DMA_Cmd(RS485_COMM_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送

		DMA_ClearITPendingBit(DMA1_FLAG_GL7);   //DMA1_FLAG_GL7

		while(USART_GetFlagStatus(USART2,USART_FLAG_TC) != SET ) {};//等待发送结束
		gb_is_DMA_send_OK = 1;
		//LED1 = !LED1;   //  输出 
		RS485_CON2 = 0; //接收 
		RS485_CON2 = 0; //接收 
	} 
}

 

/************************************************* 
*串口1   超时处理函数
*/
void Rs485_COMM_uart_timeout_fution(void) 
{  
	UART2_RS485_COMM_st.ACK_TIMEOUT_js++; 
}
 

 /************************************************* 
   接收载入缓存
*/ 	 
void Rs485_COMM_UART_RX_load_buf(u8* buf,u8 len,u8 transid)
{ 
	u8 i=0,cun_rx_lun;

	cun_rx_lun=UART2_RS485_COMM_st.uart_Rx_lun;
	for(i=0;i<USART_TXRX_PK_NUM ;i++)
	{
		if( Rs485_COMM_CHECK_BUF_VALID(UART2_RS485_COMM_st.RXBuff[cun_rx_lun]) == FALSE)//未使用的BUFF
		{
			memcpy((u8*)UART2_RS485_COMM_st.RXBuff[cun_rx_lun], (u8*)buf,len); 
			break;
		}
		(cun_rx_lun >= USART_TXRX_PK_NUM-1)?(cun_rx_lun=0):(cun_rx_lun++);
	} 
}

 
 
///************************************************* 
//*串口3  DMA 发送 
//*/ 
void  Rs485_COMM_dma_send_buf(void)
{   
	u8  i=0, cun_sd_lun;

	if( UART2_RS485_COMM_st.ALL_tc_flg == TRUE )//DMA传送完毕
	{
		if(UART2_RS485_COMM_st.ACK_TIMEOUT_js<TIMEOUT_SUM && UART2_RS485_COMM_st.ACK_RX_OK_flg == FALSE  )//超时进入或者收到应答进入
		{ 
			//LED2 =0;
			return;
		}			 
		UART2_RS485_COMM_st.ACK_RX_OK_flg = FALSE;			
		UART2_RS485_COMM_st.ACK_TIMEOUT_js = 0;		

		USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,DISABLE);

		//没有ACK时 发送数据
		cun_sd_lun = UART2_RS485_COMM_st.uart_SD_lun;
		for(i=0; i<USART_TXRX_PK_NUM; i++)
		{  
			if( Rs485_COMM_CHECK_BUF_VALID(UART2_RS485_COMM_st.SDBuff[cun_sd_lun]) == TRUE)//如果有数据
			{
				if( cun_sd_lun == UART2_RS485_COMM_st.uart_SD_lun  )
				{
					UART2_RS485_COMM_st.ACK_TIMEOUT_SUM ++;
					if(UART2_RS485_COMM_st.ACK_TIMEOUT_SUM > RE_SEND_SUM)
					{  
						UART2_RS485_COMM_st.ACK_TIMEOUT_SUM = 0; 
						memset(UART2_RS485_COMM_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
						(cun_sd_lun >= USART_TXRX_PK_NUM - 1)?(cun_sd_lun = 0):(cun_sd_lun ++);	
						continue;
					}
				}		
				memset(UART2_RS485_COMM_st.SDcun, FALSE,10);			

				memcpy((u8*)UART2_RS485_COMM_st.SDcun , (u8*)UART2_RS485_COMM_st.SDBuff[cun_sd_lun], UART2_RS485_COMM_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 

				//	LED1 = !LED1;   //  输出
				RS485_CON2 = 1;	
				RS485_CON2 = 1;	

				MYDMA_Config( RS485_COMM_DMA_TX_CH,(u32)&USART2->DR,(u32)UART2_RS485_COMM_st.SDcun,UART_SD_LEN ,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 

				MYDMATX_Enable(RS485_COMM_DMA_TX_CH, UART2_RS485_COMM_st.SDBuff_len[cun_sd_lun] );		

				UART2_RS485_COMM_st.uart_SD_lun = cun_sd_lun; // 暂存
				UART2_RS485_COMM_st.ALL_tc_flg = FALSE;//如果在这里 有要传送的 就是FLASE  不然直接显示TRUE  然后uart_dma_send_buf里可以开启 				 

				UART2_RS485_COMM_st.ACK_RX_OK_flg = FALSE;			
				UART2_RS485_COMM_st.ACK_TIMEOUT_js = 0;			

				break;
			}
			(cun_sd_lun >= USART_TXRX_PK_NUM - 1)?(cun_sd_lun = 0):(cun_sd_lun ++);	
		}
		DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,ENABLE); 
		USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 
	} 
}

 /*****************发送载入BUF**************************/
void  Rs485_COMM_SD_load_buf2( u8 * urt_buf ,u8 len )
{     
	u8 i=0, cun_sd_lun;

	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun=  UART2_RS485_COMM_st.uart_SD_lun ;
	for(i=0;i<USART_TXRX_PK_NUM  ;i++)
	{  
		if( Rs485_COMM_CHECK_BUF_VALID(UART2_RS485_COMM_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			memcpy((u8*)&UART2_RS485_COMM_st.SDBuff[cun_sd_lun][0], urt_buf,len);

			UART2_RS485_COMM_st.SDBuff_len[cun_sd_lun] = len;					
			break;
		}
		(cun_sd_lun >= USART_TXRX_PK_NUM -1)?(cun_sd_lun = 0):(cun_sd_lun ++);	
	}
	DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 
}


/*****************发送载入BUF**************************/
void  Rs485_COMM_SD_load_buf(u16 d_head,u16 d_tail, TRAN_D_struct * TRAN_info , u8 * urt_buf ,u8 len )
{     
	u8  i=0, cun_sd_lun;
	u8  crc=0 ,z_c = 0;

	if(len > UART_SD_LEN ) 
	{
		return; 
	}
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun = UART2_RS485_COMM_st.uart_SD_lun ;
	for(i=0; i < USART_TXRX_PK_NUM ; i++)
	{  
		if( Rs485_COMM_CHECK_BUF_VALID(UART2_RS485_COMM_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			UART2_RS485_COMM_st.SDBuff[cun_sd_lun][0] = d_head;//包头 
			UART2_RS485_COMM_st.SDBuff[cun_sd_lun][1] = d_head >> 8;//包头
			UART2_RS485_COMM_st.SDBuff[cun_sd_lun][2] = UART2_RS485_COMM_st.txtransid;

			memcpy(&UART2_RS485_COMM_st.SDBuff[cun_sd_lun][3], (u8*)TRAN_info,sizeof(TRAN_D_struct));
			memcpy(&UART2_RS485_COMM_st.SDBuff[cun_sd_lun][3+sizeof(TRAN_D_struct)],urt_buf,len);

			UART2_RS485_COMM_st.SDBuff[cun_sd_lun][ 3 + sizeof(TRAN_D_struct) + len ] = d_tail;   //包尾
			UART2_RS485_COMM_st.SDBuff[cun_sd_lun][ 3 + sizeof(TRAN_D_struct) + len + 1 ] = d_tail >> 8;

			z_c = 3 + sizeof(TRAN_D_struct) + len + 2;
			crc = Rs485_COMM_crc8(&UART2_RS485_COMM_st.SDBuff[cun_sd_lun][0],z_c);

			UART2_RS485_COMM_st.SDBuff[cun_sd_lun][z_c] = crc;

			UART2_RS485_COMM_st.SDBuff_len[cun_sd_lun] = z_c+1    ;

			UART2_RS485_COMM_st.txtransid++;	//序号++		
			
			break;
		}
		(cun_sd_lun >= USART_TXRX_PK_NUM - 1)?(cun_sd_lun = 0):(cun_sd_lun ++);	
	}
	DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 
}


void Rs485_COMM_uart_fuc(void) 
{
	u8 i = 0;

	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);  
	DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	for(i=0; i < USART_TXRX_PK_NUM ; i++)
	{
		if( Rs485_COMM_CHECK_BUF_VALID(UART2_RS485_COMM_st.RXBuff[i]) == TRUE)  
		{
			if( Rs485_COMM_msg_process((char*)UART2_RS485_COMM_st.RXBuff[i]) )
			{ 
				memset(UART2_RS485_COMM_st.RXBuff[i], FALSE,10);
			} 
			break;
		}
	} 
	DMA_ITConfig(RS485_COMM_DMA_TX_CH,DMA_IT_TC,ENABLE); 
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE); 		
}


u8 Rs485_COMM_msg_process(char *tab)  
{
	TRAN_D_struct   TRAN_info1,TRAN_info2; 
	u8   fuc_dat[UART_RX_LEN];
	u8   dat1[UART_RX_LEN];

	memcpy((u8*)&TRAN_info1 , (u8*)tab+3,sizeof(TRAN_D_struct));
	memcpy((u8*)fuc_dat , (u8*)tab+17,TRAN_info1.data_len);	  

	TRAN_info1.source_dev_num = ( TRAN_info1.source_dev_num << 8 | TRAN_info1.source_dev_num >> 8 );

	TRAN_info1.dest_dev_num = ( TRAN_info1.dest_dev_num << 8 | TRAN_info1.dest_dev_num >> 8 );

	if(TRAN_info1.TYPE_NUM == 1)
	{
		switch(  fuc_dat[0]<<8|fuc_dat[1]  ) 
		{
			case  0x0000:     //查询版本号
			{
			
			}	break;
			
			case  0x0001:    //获得下一级  设备识别号 和地址 
			{
			
			}	break;
			
			case  0x0002:   //获得上一级  设备识别号 和地址 
			{
				if(TRAN_info1.source_dev_num == DTN_android) //识别号为 DTN_main_manage 返回的
				{
					search_addr_flg = 1;
				}
			} break;
			
			default:  break; 
		}	 
	}
	else if( TRAN_info1.TYPE_NUM == 2 )
	{
		switch( fuc_dat[0] << 8 | fuc_dat[1] ) 
		{
			case  0x0000:     //查询版本号
			{
			
			} break;
			
			case  0x0001:    // 
			{
			
			}	break;
			
			case  0x0002:     //查询电源管理器 电压值和电流值 CMD 0X0002  反馈 电压值2字节 电流值2字节， 瓦特2字节，电能4字节
			{
				if( TRAN_info1.source_dev_num == DTN_android) //识别号为 DTN_main_manage  被查询
				{  
					TRAN_info2.dest_dev_num = ( DTN_android << 8 | DTN_android >> 8 );

					TRAN_info2.dest_addr[0] =0;
					TRAN_info2.dest_addr[1] =0;
					TRAN_info2.dest_addr[2] = 0x00;
					TRAN_info2.dest_addr[3] = 0x00;

					TRAN_info2.source_dev_num = ( DTN_power_manage << 8 | DTN_power_manage >> 8 );

					TRAN_info2.source_addr[0] = (u8)MCU_UID[0];
					TRAN_info2.source_addr[1] = (u8)(MCU_UID[0]>>8);
					TRAN_info2.source_addr[2] = (u8)(MCU_UID[0]>>16);
					TRAN_info2.source_addr[3] = (u8)(MCU_UID[0]>>24);

					TRAN_info2.TYPE_NUM = 2;   //第2类
					TRAN_info2.data_len = 2 + 10;

					dat1[0] = 00;  
					dat1[1] = 02;   //   查询电源管理器 电压值和电流值 CMD 0X0002

					dat1[2] = My_vol >> 8; 	 
					dat1[3] = My_vol; 
					dat1[4] = My_curr >> 8; 	 
					dat1[5] = My_curr; 	 
					dat1[6] = My_watt >> 8; 	 
					dat1[7] = My_watt;
					dat1[8] = My_powr >> 24; 	 
					dat1[9] = My_powr >> 16; 	 
					dat1[10] = My_powr >> 8; 	 
					dat1[11] = My_powr;  																
					//485
					Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info2 , dat1 ,TRAN_info2.data_len ); 
				}
			} break;
			
			case  0x0003:
			{	
				// 开关继电器  CMD 0X0003    1开和 0关     
				// 收到命令后，反馈 一个字节表示 1开和 0关，2没有电					 
				if( TRAN_info1.source_dev_num == DTN_android)//  
				{
					TRAN_info2.dest_dev_num = ( DTN_android << 8 | DTN_android >> 8 );

					TRAN_info2.dest_addr[0] = (u8) (DTN_android << 8) ;
					TRAN_info2.dest_addr[1] = (u8) DTN_android >> 8 ;
					TRAN_info2.dest_addr[2] = 0x00;
					TRAN_info2.dest_addr[3] = 0x00;

					TRAN_info2.source_dev_num = ( DTN_power_manage << 8 | DTN_power_manage >> 8 );

					TRAN_info2.source_addr[0] = (u8)MCU_UID[0];
					TRAN_info2.source_addr[1] = (u8)(MCU_UID[0] >> 8);
					TRAN_info2.source_addr[2] = (u8)(MCU_UID[0] >> 16);
					TRAN_info2.source_addr[3] = (u8)(MCU_UID[0] >> 24);

					TRAN_info2.TYPE_NUM = 2;   //第2类
					TRAN_info2.data_len = 2 + 1;

					dat1[0] = 00;  
					dat1[1] = 03;   //  

					//POWER_220V_FLG = 1;//test
					// //除去（AC_CHECK）电源开启检查
					//if( POWER_220V_FLG == 1 )
					
					if( fuc_dat[2] == 0 )
					{
						AC_con = 0;
						gb_J_status = 0;
						dat1[2] = 0;
						Write_Flash_J_status(0);
					}
					else
					{
						AC_con = 1; 
						gb_J_status = 0;
						dat1[2] = 1;
						Write_Flash_J_status(1);
					}
					
					
					//1开和 0关 2没电
					//485
					Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info2 , dat1 ,TRAN_info2.data_len ); 
				} 
			}break;

			case  0x0004: 
			{
			
			} break;
			
			default:  break; 
		}
	}
	else
	{

	}
	return  1;
 }
