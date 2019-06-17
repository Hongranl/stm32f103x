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

u8  RS485_V_I__msg_process(char *tab);  



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
	u8  i = 0;

	for(i=0;i<3;i++)
	{
		if(  p[i] != 0  )
		{
			return TRUE;
		} 
	}
	return FALSE;
}


/*********************************************************************
 *  crc
 */
uint8_t RS485_V_I_crc8(uint8_t *buf,uint8_t len)
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

//注释，为以后增加从机考虑，倾向于把此函数结构改为switch-case嵌套
//根据地址码区分第一层case，再根据功能码区分第二层case
//——hongxiang.liu 2019.3.26
 void USART1_IRQHandler(void)           	     	//串口3中断服务程序
 {  
	u32 i=0,Data_Len = 0;

	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) //判断是否是空闲标志
	{ 
		DMA_ClearFlag( DMA1_FLAG_GL5 );           	// 清DMA标志位
		USART_ClearITPendingBit(USART1, USART_IT_IDLE);  //清除空闲标志
		i = USART1->SR;  //清除空闲标志
		i = USART1->DR; //清除空闲标志

		DMA_Cmd(UART1_DMA_RX_CH, DISABLE);			// 禁用  避免出错
		Data_Len = UART_RX_LEN- DMA_GetCurrDataCounter(UART1_DMA_RX_CH);  
        
		if(Data_Len==15   ) 
		{  
			if(UART1_RS485_V_I_st.RXcun[0] == 0x01 && UART1_RS485_V_I_st.RXcun[1] == 0x03 && UART1_RS485_V_I_st.RXcun[2] == 0x0a )
			{
				My_vol = UART1_RS485_V_I_st.RXcun[3] << 8 | UART1_RS485_V_I_st.RXcun[4];   //电压值
				My_curr = UART1_RS485_V_I_st.RXcun[5] << 8 | UART1_RS485_V_I_st.RXcun[6];     //电流值
				My_watt = UART1_RS485_V_I_st.RXcun[7] << 8 | UART1_RS485_V_I_st.RXcun[8];    //功率
				My_powr = UART1_RS485_V_I_st.RXcun[9] << 24 | UART1_RS485_V_I_st.RXcun[10] << 16 | UART1_RS485_V_I_st.RXcun[11] << 8 | UART1_RS485_V_I_st.RXcun[12] ;//电能
				if( My_vol > 100 )
				{
					POWER_220V_FLG = 1; 
				}
				//	LED1=~LED1; 
			}
		} 
		else if(Data_Len == 7)						//风速计传感器回传风速查询应答帧字节数
		{
			if(UART1_RS485_V_I_st.RXcun[0]==Anemograph_Addr && UART1_RS485_V_I_st.RXcun[1]==0x03 && UART1_RS485_V_I_st.RXcun[2]==2)	//地址为风速计地址且功能码为0x03且字节数为2
			{
				WindSpeed = (UART1_RS485_V_I_st.RXcun[3]<<8|UART1_RS485_V_I_st.RXcun[4])/10.0;		//计算风速值
			}
		}
		else if(Data_Len == 9)						//土壤温湿度传感器回传温度+湿度应答帧字数——stm32默认只发送温度+湿度的2有效数据问询帧
		{
			if(UART1_RS485_V_I_st.RXcun[0]==Soil_Detector_Addr && UART1_RS485_V_I_st.RXcun[1]==0x03 && UART1_RS485_V_I_st.RXcun[2]==0x04)	//地址为土壤传感器地址且功能码为0x03且字节数为4
			{
				SoilHum  = (UART1_RS485_V_I_st.RXcun[3]<<8|UART1_RS485_V_I_st.RXcun[4])/10;			//计算土壤湿度值
				SoilTemp = (UART1_RS485_V_I_st.RXcun[5]<<8|UART1_RS485_V_I_st.RXcun[6])/10;			//计算土壤温度值
				///////////****************备注：零度以下温度是补码形式，须在增加原码转换公式！！！！！！！！！——hongxiang.liu****************///////////////
			}
		}

		memset(UART1_RS485_V_I_st.RXcun, 0,15);		//清空接受数组

		UART1_DMA_RX_CH -> CNDTR = UART_RX_LEN; 	//重新赋值计数值，必须大于等于最大

		DMA_Cmd(UART1_DMA_RX_CH, ENABLE);			//重新开启DMA传输			
	}  			 
}


/*****************DMA 发送完中断************************/
void  DMA1_Channel4_IRQHandler(void) 
{    
	if(DMA_GetITStatus( DMA1_IT_TC4) != RESET) // 
	{   
		UART1_RS485_V_I_st.ALL_tc_flg = TRUE;   
		DMA_Cmd(UART1_DMA_TX_CH, DISABLE);// 禁用  避免出错 或发送
		DMA_ClearITPendingBit(DMA1_FLAG_GL4);   //DMA1_FLAG_GL2

		while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET){};//等待发送结束
		RS485_CON1 = 0;	//接收
		RS485_CON1 = 0;	//接收
	}
}
 
/************************************************* 
*串口3   超时处理函数
*/
void   RS485_V_I_timeout_fution(void) 
{  
	UART1_RS485_V_I_st.ACK_TIMEOUT_js++;
}
 
 
  

///************************************************* 
//*串口3  DMA 发送 
//*/ 
void  RS485_V_I_dma_send_buf(void)
{   
	u8  i=0,cun_sd_lun;
	
	if( UART1_RS485_V_I_st.ALL_tc_flg == TRUE )//DMA传送完毕
	{
		if(UART1_RS485_V_I_st.ACK_TIMEOUT_js < TIMEOUT_SUM_2 && UART1_RS485_V_I_st.ACK_RX_OK_flg == FALSE  )//  超时未到时间，而且 应答标志未收到OK   继续等待
		{ 
			return;
		}			 
		UART1_RS485_V_I_st.ACK_RX_OK_flg = FALSE;			
		UART1_RS485_V_I_st.ACK_TIMEOUT_js = 0;	

		USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  
		DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE);  
				 	 
		//没有ACK时 发送数据
		cun_sd_lun = UART1_RS485_V_I_st.uart_SD_lun;
		for(i=0; i<USART_TXRX_PK_NUM ; i++)
		{  
			if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == TRUE)//如果有数据
			{
				if( cun_sd_lun == UART1_RS485_V_I_st.uart_SD_lun  )
				{
					UART1_RS485_V_I_st.ACK_TIMEOUT_SUM ++;
					if(UART1_RS485_V_I_st.ACK_TIMEOUT_SUM > RE_SEND_SUM_2)
					{  
						UART1_RS485_V_I_st.ACK_TIMEOUT_SUM = 0; 

						memset(UART1_RS485_V_I_st.SDBuff[cun_sd_lun], FALSE,10 ); //重发了，还是没有回应，则清空
						(cun_sd_lun >= USART_TXRX_PK_NUM-1)?(cun_sd_lun = 0):(cun_sd_lun++);	
						continue;
					}
				}								 
				memset(UART1_RS485_V_I_st.SDcun, FALSE,10);			

				memcpy((u8*)UART1_RS485_V_I_st.SDcun , (u8*)UART1_RS485_V_I_st.SDBuff[cun_sd_lun], UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] ); //把数据COPY到 	 


				RS485_CON1 = 1;//485  发送使能
				RS485_CON1 = 1;//485  发送使能
				MYDMA_Config( UART1_DMA_TX_CH,(u32)&USART1->DR,(u32)UART1_RS485_V_I_st.SDcun,UART_SD_LEN,DMA_DIR_PeripheralDST,DMA_Mode_Normal);//DMA1通道4,外设为串口1,存储器为UART1_SDBuff,长度USART1_SD_LEN. 

				MYDMATX_Enable(UART1_DMA_TX_CH, UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] );		

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
//注释：Uart1发送modbus协议，直接调用此函数即可，自带缓存——hongxiang.liu
void  RS485_V_I_SD_load_buf2( u8 * urt_buf ,u8 len )
{     
	u8 i=0, cun_sd_lun;

	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);  //临界点
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,DISABLE); 

	cun_sd_lun = UART1_RS485_V_I_st.uart_SD_lun ;
	for(i=0; i<USART_TXRX_PK_NUM; i++)
	{  
		if( RS485_V_I_CHECK_BUF_VALID(UART1_RS485_V_I_st.SDBuff[cun_sd_lun]) == FALSE)//如果没有数据
		{ 
			memcpy((u8*)&UART1_RS485_V_I_st.SDBuff[cun_sd_lun][0], urt_buf,len);

			UART1_RS485_V_I_st.SDBuff_len[cun_sd_lun] = len;

			break;
		}
		(cun_sd_lun >= USART_TXRX_PK_NUM-1)?(cun_sd_lun = 0):(cun_sd_lun ++);	
	}
	DMA_ITConfig(UART1_DMA_TX_CH,DMA_IT_TC,ENABLE);  //临界点
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); 
}

