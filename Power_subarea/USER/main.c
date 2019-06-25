/*************************************
   GTA-GXU01 电源分区管理
          2018-9-18 
					by: 富宝东					
  
	引脚使用：
 
****************************************/
#include  "global.h"

u32   MY_DEST_ADDR = 0;
 
u8    POWER_220V_FLG = 0;  //AC_CHECK
 
u16   My_vol; 
u16		My_curr;  
u16   My_watt; 
u32		My_powr;

//三相电源检测模块
u16    My_AC_frequency; 
u16		 My_three_phase_W_H16;  
u16    My_three_phase_W_L16; 
u16		 My_A_PHASE_I;
u16		 My_A_PHASE_U;
u16		 My_A_PHASE_W;

u16		 My_B_PHASE_I;
u16		 My_B_PHASE_U;
u16		 My_B_PHASE_W;

u16		 My_C_PHASE_I;
u16		 My_C_PHASE_U;
u16		 My_C_PHASE_W;

u8    search_addr_flg = 0;
u16   alarm_time = 0;
u16   send_urt_time = 0;

u16   join_key_time = 0;

u8    global_mode = 0;

uint32_t MCU_UID[3];

u16  JDQ_16BIT = 0;
u16  JDQ_16BIT_compare = 0;

u8   join_flg = 0;

uint16_t crc16_MODBUS(uint8_t *ptr, uint16_t len)
{
	uint8_t i;
	uint16_t crc = 0xffff;
 
	if (len == 0) {
		len = 1;
	}
	while (len--) {
		crc ^= *ptr;
		for (i = 0; i<8; i++)
		{
			if (crc & 1) {
				crc >>= 1;
				crc ^= 0xa001;
			}
			else {
				crc >>= 1;
			}
		}
		ptr++;
	}
	return(crc);
} 

//
void    HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 


void	KEY_sta_machine(void)
{    
	u16   i = 0, abc = 0;
	TRAN_D_struct   TRAN_info1 ;
	
	u8  dat[UART_RX_LEN];

	
	
	if(JOIN_KEY == 0)
	{
		delay_ms(2);
		if(JOIN_KEY == 0)
		{ 
			join_key_time = 0;
			while(JOIN_KEY == 0)
			{
				if( join_key_time >= 3500 )  //2019.3.6 YQ
				{    
					//长按5S后，触发一次入网或退网
					TRAN_info1.dest_dev_num = 0;

					TRAN_info1.dest_addr[0] = 0x00;
					TRAN_info1.dest_addr[1] = 0x00;
					TRAN_info1.dest_addr[2] = 0x00;
					TRAN_info1.dest_addr[3] = 0x00;


					TRAN_info1.source_dev_num = (DTN_POWER_SUBAREA << 8 | DTN_POWER_SUBAREA >> 8);

					TRAN_info1.source_addr[0] = (u8) MCU_UID[0];
					TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
					TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
					TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

					TRAN_info1.TYPE_NUM = 3 ;   //第一类
					TRAN_info1.data_len = 2 ;

					dat[0] = 0; 

					if( join_flg == 0 )  //如果长按5S  入网开关开，中间的灯闪5次
					{   
						join_flg = 1;
						dat[1] = join_sw; // //入网开关

						for(i=0;i<8;i++)
						{
							JOIN_LED = 0; 
							delay_ms(150);
							JOIN_LED = 1; 
							delay_ms(150);
						}
					}
					else    //如果长按5S  入网开关关，左右两边的灯闪5次
					{  
						join_flg=0;
						dat[1] = leave_net; //

						for(i=0;i<3;i++)
						{
							JOIN_LED = 0; 
							delay_ms(800);
							JOIN_LED = 1; 
							delay_ms(800);
						}	
					}
					Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );  

					join_key_time = 0;
				} 	
			}
		}
	}
	/* else
	{
	join_key_time = 0 ;

	}
	*/

	if( KEY1 == 0 )
	{
		delay_ms(5);
		while( KEY1 == 0 );
		if(JDQ_16BIT&0x0001)
		{
			JDQ_16BIT = JDQ_16BIT&0xfffe;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0001; 
		} 
	}
	
	if( KEY2 == 0 )
	{
		delay_ms(5);
		while( KEY2 == 0 );
		if(JDQ_16BIT&0x0002)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0002;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0002; 
		} 
	}
	
	if( KEY3 == 0 )
	{
		delay_ms(5);
		while( KEY3 == 0 );
		if(JDQ_16BIT&0x0004)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0004;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0004; 
		}  
	}
	
	if( KEY4 == 0 )
	{
		delay_ms(5);
		while( KEY4 == 0 );
		if(JDQ_16BIT&0x0008)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0008;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0008; 
		} 
	}
	
	if( KEY5 == 0 )
	{
		delay_ms(5);
		while( KEY5 == 0 );
		if(JDQ_16BIT&0x0010)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0010;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0010; 
		} 
	}
	
	if( KEY6 == 0 )
	{
		delay_ms(5);
		while( KEY6 == 0 );
		if(JDQ_16BIT&0x0020)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0020;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0020; 
		} 
	}

	if( KEY7 == 0 )
	{
		delay_ms(5);
		while( KEY7 == 0 );
		if(JDQ_16BIT&0x0040)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0040;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0040; 
		} 
	}
	
	if( KEY8 == 0 )
	{
		delay_ms(5);
		while( KEY8 == 0 );
		if(JDQ_16BIT&0x0080)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0080;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0080; 
		} 
	}
	
	if( KEY9== 0 )
	{
		delay_ms(5);
		while( KEY9 == 0 );
		if(JDQ_16BIT&0x0100)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0100;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0100; 
		} 
	}	 

	if( KEY10 == 0 )
	{
		delay_ms(5);
		while( KEY10 == 0 );
		if(JDQ_16BIT&0x0200)
		{
			JDQ_16BIT = JDQ_16BIT&~0x0200;
		}
		else 
		{
			JDQ_16BIT = JDQ_16BIT|0x0200; 
		}   
	}	

	abc = JDQ_16BIT;

	write_595_JDQ( (u8*)&abc, 2 );

	if( JDQ_16BIT_compare!=JDQ_16BIT )
	{
		JDQ_16BIT_compare=JDQ_16BIT;

		TRAN_info1.dest_dev_num= 0;

		TRAN_info1.dest_addr[0]=0x00;
		TRAN_info1.dest_addr[1]=0x00;
		TRAN_info1.dest_addr[2]=0x00;
		TRAN_info1.dest_addr[3]=0x00;


		TRAN_info1.source_dev_num= (DTN_POWER_SUBAREA<<8|DTN_POWER_SUBAREA>>8);

		TRAN_info1.source_addr[0]=(u8) MCU_UID[0];
		TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
		TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
		TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);

		TRAN_info1.TYPE_NUM = 3;   //第一类
		TRAN_info1.data_len = 18;

		dat[0] = 0; 
		dat[1] = upload_info; //

		dat[2] = 0; // 
		dat[3] = DTN_POWER_SUBAREA; // 
		dat[4] = (u8) MCU_UID[0];
		dat[5] = (u8)(MCU_UID[0]>>8);
		dat[6] = (u8)(MCU_UID[0]>>16);
		dat[7] = (u8)(MCU_UID[0]>>24); 

		memset( dat+8,0,8 );

		dat[8+8] = JDQ_16BIT;  
		dat[8+9] = JDQ_16BIT>>8; 
		Rs485_COMM_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );									
	}
}



int main(void)
{  
	static  u8 swa_flg=0;
	TRAN_D_struct   TRAN_info1;

	u8   dat[8]={ 01,03, 00 ,0x48, 00 ,05 ,05 ,0xdf }; 

	u8   dat1[8]={ 01,03, 00 ,0x48, 00 ,05 ,05 ,0xdf };
	u8   dat2[8]={ 01,03, 00 ,3, 00 ,77 ,0x00,0x00};

	u16  l = 0;
	 
	 
	delay_init();	        	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	BSP_init(); 

	// VAL_init();

	HAL_GetUID(MCU_UID);     //获得单片机唯一码

	uart1_init (115200);	//  
	uart3_init (9600);	//与交流电压电流计交互 波特率不一定是这个


	TIM2_Int_Init(9,7199);//72M/7200   10Khz的计数频率，计数到9为1ms 
	
     /*
			4.4.4 直流测量指令解析举例：
			（1）读取电参数指令（以模块地址为 0x01 举例）：
			发送数据：01 03 00 48 00 05 05 DF （读 0048 开始的 5 个寄存器）
			接收数据：01 03 0A 02 6C 11 D7 01 1B 01 02 03 04 DB 6F
			红色部分为相关电参数，分解出来的结果如下图：
			分解数据，得到的结果为：电压=0x026C/100=620/100=6.2V
			电流=0x11D7/1000=4567/1000=4.567A
			功率=0x011B/10=283/10=28.3W
			电能=0x01020304/100=16909060/100=169090.6Wh
			*/	
	 
	  
	//设备开机查询 上一级地址   
	alarm_time = 3000;
	//	search_addr_flg=0; 
	search_addr_flg = 1;  

	join_key_time = 5000;
	while(1)
	{  
		
		//JOIN_LED = 0;
		if(search_addr_flg == 0)//
		{ 
		/*
			 if( alarm_time>2000 )
			 {  alarm_time=0;
				 
			 TRAN_info1.dest_dev_num= (DTN_main_manage<<8|DTN_main_manage>>8);
				 
		 
			 TRAN_info1.dest_addr[0]=0x00;
			 TRAN_info1.dest_addr[1]=0x00;
			 TRAN_info1.dest_addr[2]=0x00;
			 TRAN_info1.dest_addr[3]=0x00;
		 
			 
			 TRAN_info1.source_dev_num= (DTN_power_manage<<8|DTN_power_manage>>8);
			 
			 TRAN_info1.source_addr[0]=(u8)MCU_UID[0];
			 TRAN_info1.source_addr[1]=(u8)(MCU_UID[0]>>8);
			 TRAN_info1.source_addr[2]=(u8)(MCU_UID[0]>>16);
			 TRAN_info1.source_addr[3]=(u8)(MCU_UID[0]>>24);
				
			 TRAN_info1.TYPE_NUM=1;   //第一类
			 TRAN_info1.data_len= 2   ;

			 dat[0]=00;  
			 dat[1]=02;   //获取上一级设备种类识别号和地址 CMD 0X0002   反馈 6个字节
				 
			 Rs485_COMM_SD_load_buf( 0xAAAA,0XBBBB,  &TRAN_info1 , dat  ,TRAN_info1.data_len   );  
					
			 }						 
			 */
		}
		else
		{
			if( alarm_time > 4000 )
			{  
				alarm_time = 0;
				if( swa_flg )
				{   
					swa_flg=0;
					RS485_V_I_SD_load_buf2( dat1 , 8 );  
					RS485_V_I_dma_send_buf();
				}
				else
				{  
					swa_flg = 1; 
					l = crc16_MODBUS(dat2, 6);
					dat2[6] = l;
					dat2[7] = l >> 8 ;
					RS485_V_I_SD_load_buf2( dat2 , 8 );  
					RS485_V_I_dma_send_buf();
				}
			} 
		}
		Rs485_COMM_uart_fuc() ;
		Rs485_COMM_dma_send_buf(); 

		KEY_sta_machine();
  }
}
