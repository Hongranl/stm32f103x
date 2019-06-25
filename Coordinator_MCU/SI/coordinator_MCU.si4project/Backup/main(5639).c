/*************************************
  配套 集控中心主控板-GX-P1B-GXM01-V2.1
	杨青
	
****************************************/
#include "global.h"

 
u32  ANDROID_DEST_ADDR = 0;

u16  motor_ck_time = 0;

u16  alarm_time = 0;

u8   global_mode = 0;


uint32_t         	MCU_UID[3];

IR_CAP_struct    	IR_CAP1;

u8               	IR_CAP_START_FLG = 0;

u8              	m_mode = 0;

u16              	join_LED_S = 0;

u16              	send_urt_time = 0;

u8               	JDQ_NOW_STATUS = 0;
u8               	JDQ_WILL_STATUS = 0;

u16             	JDQ_time = 0;

u8              	PC_KEY_FLAG;


void IR_js( void);

void  remote_cap_study( void  );



u8  main_CHECK_BUF_VALID(u8 *p) 
{  
	u8 i = 0;

	for(i=0;i<3;i++)
	{
		if(  p[i] != 0  )
		{
			return TRUE;
		} 
	}
	return FALSE;
}



void HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 



void IR_js( void )
{
	IR_CAP1.IR_tab_js++;
} 

 

void  remote_cap_study( void  )
{				 
	u16 IR_tab2[1024];
	TRAN_D_struct   TRAN_info1 ;

	u8   dat[UART_RX_LEN];

	u32  i=0;

	if( IR_RX == 0 )
	{
		while(1)
		{
			IR_CAP1.IR_tab_js = 0;
			while( IR_RX == 0 )
			{
				if( IR_CAP1.IR_tab_js >= 20000  )//200MS
				{
					goto CAP_END;
				}
			};

			IR_CAP1.IR_tab[IR_CAP1.IR_tab_len] = IR_CAP1.IR_tab_js;
			IR_CAP1.IR_tab_len++;
			IR_CAP1.IR_tab_js = 0;
			while( IR_RX != 0  )
			{
				if( IR_CAP1.IR_tab_js >= 20000 )
				{
					goto CAP_END;
				}
			};

			IR_CAP1.IR_tab[IR_CAP1.IR_tab_len] = 0x8000 | IR_CAP1.IR_tab_js;
			IR_CAP1.IR_tab_len++;
			IR_CAP1.IR_tab_js = 0;
		} 
	}

CAP_END:	 
								
	if((IR_CAP1.IR_tab_len > 0 ) && (IR_CAP1.IR_tab_len <= 1024))
	{
		LED_HW=1;
		//存储   
		STMFLASH_Write( FLASH_SAVE_ADDR+  IR_CAP1.CAP_IR_store_num*1024   ,(u16*)&IR_CAP1.IR_tab[0] ,1024  );
		STMFLASH_Read (FLASH_SAVE_ADDR+  IR_CAP1.CAP_IR_store_num*1024  ,(u16*)&IR_tab2[0] ,1024  );

		if(memcmp(&IR_CAP1.IR_tab[0],&IR_tab2[0],1024 ) == 0)
		{		
			TRAN_info1.dest_addr[0] = 0x00;
			TRAN_info1.dest_addr[1] = 0x00;
			TRAN_info1.dest_addr[2] = 0x00;
			TRAN_info1.dest_addr[3] = 0x00;

			TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

			TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
			TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM = 4;   //第2类
			TRAN_info1.data_len = 4;

			dat[0] = 00;  
			dat[1] = 8;   // 上传自己的  编码号 设备号 
			dat[2] = IR_CAP1.CAP_IR_store_num;  
			dat[3] = 1;   //1 ok 0 failed

			Android_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );


			IR_CAP1.IR_tab_len = 0;  

			IR_CAP1.CAP_IR_WAVEFORM_TIME = 0;

			memset( (u8*)&IR_CAP1, 0,sizeof(IR_CAP_struct) );
		}
		else
		{ 
			TRAN_info1.dest_addr[0] = 0x00;
			TRAN_info1.dest_addr[1] = 0x00;
			TRAN_info1.dest_addr[2] = 0x00;
			TRAN_info1.dest_addr[3] = 0x00;

			TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

			TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
			TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

			TRAN_info1.TYPE_NUM = 4;   //第2类
			TRAN_info1.data_len = 4;

			dat[0] = 00;  
			dat[1] = 8;   // 上传自己的  编码号 设备号 
			dat[2] = IR_CAP1.CAP_IR_store_num;  
			dat[3] = 0;   //1 ok 0 failed

			Android_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );


			IR_CAP1.IR_tab_len = 0;  

			IR_CAP1.CAP_IR_WAVEFORM_TIME = 0;

			memset( (u8*)&IR_CAP1, 0,sizeof(IR_CAP_struct) );
		}											 
	}
	else 
	{
		IR_CAP1.IR_tab_len = 0;   
		memset( (u8*)&IR_CAP1, 0,sizeof(IR_CAP_struct) );
		//保存成功，发送给ANDROID 信息 
		TRAN_info1.dest_dev_num = ( DTN_JCZX_ANDROID << 8 | DTN_JCZX_ANDROID >> 8 );

		TRAN_info1.dest_addr[0] = 0x00;
		TRAN_info1.dest_addr[1] = 0x00;
		TRAN_info1.dest_addr[2] = 0x00;
		TRAN_info1.dest_addr[3] = 0x00;


		TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

		TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
		TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
		TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
		TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

		TRAN_info1.TYPE_NUM = 4;   //第2类
		TRAN_info1.data_len = 4;

		dat[0] = 00;  
		dat[1] = 8;   // 上传自己的  编码号 设备号 
		dat[2] = IR_CAP1.CAP_IR_store_num;  
		dat[3] = 0;   //1 ok 0 failed

		Android_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
	}
	
	IR_CAP_START_FLG = 0;
}



void  IR_send_waveform( u8 CAP_IR_num  ) 
{
	TRAN_D_struct   TRAN_info1 ; 
	u8    	dat[UART_RX_LEN];
	u16    	ir_buf[1024];
	u16     ir_sd_val = 0;
	u16   	i = 0;
	
	STMFLASH_Read(FLASH_SAVE_ADDR+ CAP_IR_num*1024,(u16*)ir_buf,1024 );
	
	if( ir_buf[0] == 0 )
	{
		//保存成功，发送给ANDROID 信息 
		TRAN_info1.dest_dev_num = ( DTN_JCZX_ANDROID << 8 | DTN_JCZX_ANDROID >> 8 );

		TRAN_info1.dest_addr[0] = 0x00;
		TRAN_info1.dest_addr[1] = 0x00;
		TRAN_info1.dest_addr[2] = 0x00;
		TRAN_info1.dest_addr[3] = 0x00;


		TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

		TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
		TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
		TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
		TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);


		TRAN_info1.TYPE_NUM = 4;   //第2类
		TRAN_info1.data_len = 4;

		dat[0] = 00;  
		dat[1] = 9;   // 上传自己的  编码号 设备号 
		dat[2] = CAP_IR_num;  
		dat[3] = 2;   //1 ok 0 failed 2 no study

		Android_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );

		return;
	}	
	
  for(i=0;i<1024;i++)
	{  
		LED_HW=~LED_HW;

		if(ir_buf[i]!=0 && ir_buf[i]!=0xffff)
		{
			if( ir_buf[i] & 0x8000 )//高电平  
			{
				ir_sd_val = ir_buf[i] & 0x7fff;

				TIM_Cmd(TIM8, DISABLE);  //使能TIM3
				// RS_485=1;
				// IR_TX=1; 
				// RS_485=0;//加了一个三极管 所以取非
				// IR_TX=0; 
				IR_CAP1.IR_tab_js = 0;  // 时间
				while(IR_CAP1.IR_tab_js < ir_sd_val);
			}
			else
			{
				ir_sd_val = ir_buf[i] & 0x7fff;

				TIM_Cmd( TIM8,ENABLE );  //使能TIM3
				// RS_485=0;
				// IR_TX=0;
				// RS_485=1;
				// IR_TX=1; 
				IR_CAP1.IR_tab_js = 0; 	  // 时间
				while(IR_CAP1.IR_tab_js < ir_sd_val);
			} 
		} 
		else 
		{     
			TIM_Cmd(TIM8, DISABLE);  //使能TIM3

			// RS_485=1;
			// IR_TX=1; 
			// RS_485=0;
			// IR_TX=0; 

			//保存成功，发送给ANDROID 信息 
			TRAN_info1.dest_dev_num = ( DTN_JCZX_ANDROID << 8 | DTN_JCZX_ANDROID >> 8 );
			 
			TRAN_info1.dest_addr[0] = 0x00;
			TRAN_info1.dest_addr[1] = 0x00;
			TRAN_info1.dest_addr[2] = 0x00;
			TRAN_info1.dest_addr[3] = 0x00;
				 
				 
			TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

			TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
			TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
			TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
			TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);
			 
			TRAN_info1.TYPE_NUM = 4;   //第2类
			TRAN_info1.data_len = 4;

			dat[0] = 00;  
			dat[1] = 9;   // 上传自己的  编码号 设备号 
			dat[2] = CAP_IR_num;  
			dat[3] = 1;   //1 ok 0 failed

			Android_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );

			break;
		}
	} 
}





//#define FLASH_SAVE_ADDR  0X08011800 				//设置FLASH 保存地址(必须为偶数)   70K后
//学习红外遥控器，捕获波形，2个字节表示一个高低位加时宽  如 0x81ff  0x8000 表示高位   1FF表示时间宽  大于8MS的表示帧结束
//采集2帧 如果相同即可。1帧就是1K字节，可以容纳512次高低电平，保存一帧。最大50帧，即占用FLASH  50K字节
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)
	{ 
		if(m_mode == 1)
		{
			IR_CAP_START_FLG = 1; 
		}	
		EXTI_ClearITPendingBit(EXTI_Line6);  //清除LINE6上的中断标志位 
	}      
}

 

int main(void)
{  
	TRAN_D_struct   TRAN_info1;

	uint8_t   dat[100] = {1,2,3,4,5,6,7,8,9,10,11};
	uint8_t   dat2[100] = {25,26,27,28,29,30,31,32,33,34,35,36,37};

	u8  M_tagfounds = 1;

	delay_init();	        	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	HAL_GetUID(MCU_UID);     //获得单片机唯一码

	LED_Init();			       //LED端口初始化 
	BEEP_INIT() ; 
	EXTI_START();


	uart4_init(115200);	//与ZIG1 串口交互 
	uart2_init(57600);
	uart3_init(57600);

	TIM2_Int_Init(9,72);//72M/72   1000Khz的计数频率，计数到9为10us 


	// TIM3_Int_Init(9,72);//72M/72   1000Khz的计数频率，计数到9为10us 

	TIM8_PWM_Init(1894,0);	 //不分频。PWM频率=72000 000/1894=38Khz


	// Interrupts_Config(); 
	//CR95HF_ConfigManager_HWInit();

	memset( (u8*)&IR_CAP1, 0,sizeof(IR_CAP_struct) );
    			 
	while(1)
	{  
		if(join_key == 0)
		{
			delay_ms(10);
			while(join_key == 0) {};

			TRAN_info1.dest_dev_num = 0;

			TRAN_info1.dest_addr[0] = 0x00;
			TRAN_info1.dest_addr[1] = 0x00;
			TRAN_info1.dest_addr[2] = 0x00;
			TRAN_info1.dest_addr[3] = 0x00;

			TRAN_info1.source_dev_num = 0;

			TRAN_info1.source_addr[0] = 0;
			TRAN_info1.source_addr[1] = 0;
			TRAN_info1.source_addr[2] = 0;
			TRAN_info1.source_addr[3] = 0;

			TRAN_info1.TYPE_NUM = 4;   //第4类
			TRAN_info1.data_len = 3; 

			dat[0] = 0;
			dat[1] = 1;
			dat[2] = 60;
			join_LED_S = 60 ;

			ZIG1_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len ); 
		}
		
		if( PC_KEY_FLAG )//被按下,状态要切换一个
		{
			if(JDQ_NOW_STATUS != JDQ_WILL_STATUS)
			{
				JDQ_PIN = 1;

				if( PC_STATUS == 0 )
				{
					JDQ_NOW_STATUS = 2;
				}
				else
				{
					JDQ_NOW_STATUS = 0;
				}
			}
			else
			{ 
				JDQ_PIN = 0; 
				PC_KEY_FLAG = 0; 

				TRAN_info1.dest_dev_num = 0;

				TRAN_info1.dest_addr[0] = 0x00;
				TRAN_info1.dest_addr[1] = 0x00;
				TRAN_info1.dest_addr[2] = 0x00;
				TRAN_info1.dest_addr[3] = 0x00;

				TRAN_info1.source_dev_num = ( DTN_JCZX_BOARD << 8 | DTN_JCZX_BOARD >> 8 );

				TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
				TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);


				TRAN_info1.TYPE_NUM = 4;   //第2类
				TRAN_info1.data_len = 3;

				dat[0] = 00;  
				dat[1] = 0x0c;   // 上传自己的  编码号 设备号 
				dat[2] = JDQ_NOW_STATUS;  

				RS485_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
				delay_ms(50);
				RS485_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
				delay_ms(50);
				RS485_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len );
			}		  
		}
		
		switch( m_mode )
		{
			case 0: 
			{
				// 	disable_all_uart_interupt(   ); 
				//  CR95HF_FUC(  M_tagfounds );    //刷卡的  drv95HF_SPIPollingCommand 
				//  (M_tagfounds>=TRACK_NFCTYPE5)?( M_tagfounds=TRACK_NFCTYPE1 ):( M_tagfounds*=2  );
				// enable_all_uart_interupt(   );


				ZIG1_uart_fuc() ;
				ZIG1_dma_send_buf();
				Android_uart_fuc() ;
				Android_dma_send_buf(); 

				RS485_uart_fuc();
				RS485_dma_send_buf();

			} break;
			
			case 1: //进入1模式 ，是学习红外模式，时间是60S
			{
				if(IR_CAP1.CAP_IR_WAVEFORM_TIME > 0)
				{
					if(IR_CAP_START_FLG == 1)
					{
						remote_cap_study();
					} 
				}
				else 
				{  
					IR_CAP1.CAP_IR_WAVEFORM_TIME = 0;
					IR_CAP_START_FLG = 0;
					m_mode = 0;
				}
			} break;
			
			case 2:  //发送 捕获红外遥控波形 
			{
				IR_send_waveform( IR_CAP1.CAP_IR_store_num );
				LED_HW = 1;
				m_mode = 0;
			} break;
				
			default: break; 
		} 
	}
}
