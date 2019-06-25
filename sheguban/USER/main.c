/*************************************
        设故控制板
          2018-9-18 
					by: 富宝东					
  
	引脚使用：
  LED:     LED_EEROR  PA12 ; 	
	         LED_COMM  PA15 ;
	
	LED_595_ST   
	LED_595_SH
	LED_595_MR
	LED_595_DS
	
	BEEP: PB9
	
	串口：RS485      UART1   PA9 PA10    con_pin1  PA11
	   
	TYPE_SW1  PC13  
  TYPE_SW2  PC14  
	TYPE_SW3  PC15 
	
	ST_595    PB3
	SH_595    PB4
	MR_595    PB5
	DS_595    PB6
	 
	MODE_SW3    PA8
	MODE_SW4    PB7
	MODE_SW5    PB8
	
	
	ADDRX1_1=PA4
	ADDRX1_2=PA5
	ADDRX1_4=PA6
	ADDRX1_8=PA7
	
	ADDRX10_1=PB12
	ADDRX10_2=PB13
	ADDRX10_4=PB14
	ADDRX10_8=PB15	
	
****************************************/
#include  "global.h"

u32  	MY_DEST_ADDR = 0;
 
u16   My_vol; 
u16		My_curr;  
u16   My_watt; 
u32		My_powr;

u8  	search_addr_flg = 0;  //搜索地址标志
u16 	alarm_time = 0;				//报警时间
u32 	ACCIDENTAL_time[16];	
u16  	rand_seed = 0;
  
u8   	MY_JDQ_TAB[16];

u8   	MY_JDQ_con_TAB[8];

u8 		global_mode = 0;

u16   send_urt_time=0;

uint32_t MCU_UID[3];
u8       MY_CODE = 0;
u16      MY_SWTYPE = 0;

//
void HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 

u8 read_code( void )
{  
	u16  pa4_7 = 0,pb12_15 = 0;

	pa4_7 = GPIO_ReadInputData(GPIOA);
	pb12_15 = GPIO_ReadInputData(GPIOB);

	pa4_7 >>= 4;
	pa4_7 =~ pa4_7;
	pa4_7 &= 0x000f;
	pb12_15 >>= 12;
	pb12_15 =~ pb12_15;
	pb12_15 &= 0x000f;

	return  (u8)( pb12_15 * 10 + pa4_7 );
}

 
u8 read_SWTYPE( void )
{  
	u16 pc13_15 = 0;

	pc13_15 = GPIO_ReadInputData(GPIOC);  

	pc13_15 >>= 13;
	//pc13_15=~pc13_15;
	pc13_15 &= 0x000f;

	if( pc13_15 == 3 )
	{
		MY_SWTYPE = DTN_8L_2T_16A; 
	}
	else if( pc13_15 == 7 )
	{
		MY_SWTYPE = DTN_16L_2T_3A; 
	}
	else if( pc13_15 == 5 )
	{
		MY_SWTYPE = DTN_16L_5T_1A; 
	}		
	else
	{

	}
	return (u8)pc13_15;
}


  /*
	
	5态
     加了个非门
状态	A         B           C
0	    0         0           0    
1	    1         0           0       1
2	  0FF(0)     0N(1)       0FF(0)   2
3	    0         0           1       4
4   偶发  前4种情况 随机出现 时间也随机    1,2,4  
	 
  2态
状态	A         B           C
0	  0FF(0)     0FF(0)      0FF(0)
1	  0N(1)      0FF(0)      0FF(0)
   
 #define  OPEN               2    // 0 
 #define  CLOSE              0   //  1    
 
  
 #define   normal              0   
 #define  VIRTUAL_CONNECT      1
 #define  open circuit         2
 #define  VIRTUAL_EARTH        3
 #define  ACCIDENTAL           4 
	*/
u16  TransForm_JDQ_to_LED_VALUE(  u8  typ ,u8* JDQ_tab1  )
{     
	u8 MY_JDQ_TAB2[16];
	u16 ia = 0, i = 0, dd = 0;
	
	switch( typ )
	{
		case DTN_8L_2T_16A :    //0  OPEN  是亮 
		{
			memcpy( MY_JDQ_TAB2, MY_JDQ_TAB,16 );

			for(ia=0; ia<16; ia++)
			{
				if( MY_JDQ_TAB2[ia] == 0 )
				{
					MY_JDQ_TAB2[ia] = 1;
				}
				else 
				{
					MY_JDQ_TAB2[ia] = 0;
				}
			}
			
			for( i=0;i<8;i++ )
			{
				if( MY_JDQ_TAB2[i] == 0 )
				{
					dd|=1<<i;
				} 
			}  
		}break; 
		
		case DTN_16L_2T_3A :    //0  OPEN  是亮 
		{
			memcpy( MY_JDQ_TAB2, MY_JDQ_TAB,16 );

			for(ia=0;ia<16;ia++)
			{
				if( MY_JDQ_TAB2[ia] == 0)
				{
					MY_JDQ_TAB2[ia] = 1;
				}
				else 
				{
					MY_JDQ_TAB2[ia] = 0;
				}
			}

			for( i=0;i<16;i++ )
			{
				if( MY_JDQ_TAB2[i] == 0 )
				{
					dd|=1<<i;
				} 
			} 
		}break;
			
		case DTN_16L_5T_1A :      //这里是有区别的  0是正常，其他都是故障 
		{
			for( i=0;i<16;i++  )
			{
				if( MY_JDQ_TAB[i]!=0)
				{
					dd|=1<<i;
				} 
			}
		}break;
				   
		default:	break;		 
	}
	return dd;
}


u8  exchange_jdq_val_status( u8*  dat ,u8  tb1 ,u32* m_ACCIDENTAL_time )
{
	switch( tb1 )
	{
		case 1 :
		{
			tb1=1;
		} break;
		
		case 2 :
		{
			tb1=2;
		} break;
		
		case 3 :
		{
			tb1=4;
		} break;
		
		case 4 :		
		{	// tb1= ?;  偶发  随机
			if( *m_ACCIDENTAL_time ==0 )
			{
				srand(  rand_seed );
				*m_ACCIDENTAL_time =  rand()%180+1 ;
				*m_ACCIDENTAL_time *= 1000;

				do
				{
					tb1 = rand();
					tb1 = tb1 % 3 + 1 ;

					if(tb1 == 1)
					{
						tb1 = 1;
					}
					else if(tb1 == 2)
					{
						tb1 = 2;
					}
					else if(tb1 == 3)
					{
						tb1 = 4;
					}

					if(*dat==tb1)
					{
						continue;
					}  
					else
					{
						*dat = tb1;
						break;								 
					}
				} while(1);
			}
			return     *dat;
		} break;	
		
		default: 
		{
			tb1 = 0;  //正常
		} break;
	} 
	*dat	=	tb1;
	return *dat ;
}


void handle_jdq_fuc( void )  //MY_JDQ_TAB
{   
	u16     ia = 0;
	u8      JDQ_tab2[8],JDQ_tab3[8];
	static  u8  JDQ_tab1[16] ;
	u8      MY_JDQ_TAB2[16];

	switch( MY_SWTYPE )
	{
		case DTN_8L_2T_16A :   
		{
			//#define  OPEN               2    // 0 
			//#define  CLOSE              0   //  1  
									 
			memcpy( MY_JDQ_TAB2, MY_JDQ_TAB,16 );

//			for(ia=0;ia<16;ia++)
//			{
//				if( MY_JDQ_TAB2[ia] == 0)
//				{
//					MY_JDQ_TAB2[ia] = 1;
//				}
//				else   
//				{
//					MY_JDQ_TAB2[ia] = 0;
//				}
//			}
			
			JDQ_tab1[0]= (MY_JDQ_TAB2[ 0 ]&0x0f)|(MY_JDQ_TAB2[ 1 ]<<4); 
			JDQ_tab1[1]= (MY_JDQ_TAB2[ 2 ]&0x0f)|(MY_JDQ_TAB2[ 3 ]<<4); 
			JDQ_tab1[2]= (MY_JDQ_TAB2[ 4 ]&0x0f)|(MY_JDQ_TAB2[ 5 ]<<4);
			JDQ_tab1[3]= (MY_JDQ_TAB2[ 6 ]&0x0f)|(MY_JDQ_TAB2[ 7 ]<<4);

			JDQ_tab1[4]= (MY_JDQ_TAB2[ 8 ]&0x0f)|(MY_JDQ_TAB2[ 9 ]<<4);
			JDQ_tab1[5]= (MY_JDQ_TAB2[ 10 ]&0x0f)|(MY_JDQ_TAB2[ 11 ]<<4); 
			JDQ_tab1[6]= (MY_JDQ_TAB2[ 12 ]&0x0f)|(MY_JDQ_TAB2[ 13 ]<<4);
			JDQ_tab1[7]= (MY_JDQ_TAB2[ 14 ]&0x0f)|(MY_JDQ_TAB2[ 15 ]<<4);

			memcpy(JDQ_tab3 , JDQ_tab1 ,8 );

			write_595_JDQ( JDQ_tab1 , 8 );

		}break;
		
		case DTN_16L_2T_3A :   
		{
			//#define  OPEN               2    // 0 
			//#define  CLOSE              0   //  1  

			memcpy(MY_JDQ_TAB2, MY_JDQ_TAB,16  );

//			for(ia=0;ia<16;ia++)
//			{
//				if( MY_JDQ_TAB2[ia] == 0)
//				{
//					MY_JDQ_TAB2[ia] = 1;
//				}
//				else 
//				{
//					MY_JDQ_TAB2[ia] = 0;
//				}
//			}

			JDQ_tab1[0] = (MY_JDQ_TAB2[ 0 ]&0x0f)|(MY_JDQ_TAB2[ 1 ]<<4); 
			JDQ_tab1[1] = (MY_JDQ_TAB2[ 2 ]&0x0f)|(MY_JDQ_TAB2[ 3 ]<<4); 
			JDQ_tab1[2] = (MY_JDQ_TAB2[ 4 ]&0x0f)|(MY_JDQ_TAB2[ 5 ]<<4);
			JDQ_tab1[3] = (MY_JDQ_TAB2[ 6 ]&0x0f)|(MY_JDQ_TAB2[ 7 ]<<4);

			JDQ_tab1[4] = (MY_JDQ_TAB2[ 8 ]&0x0f)|(MY_JDQ_TAB2[ 9 ]<<4);
			JDQ_tab1[5] = (MY_JDQ_TAB2[ 10 ]&0x0f)|(MY_JDQ_TAB2[ 11 ]<<4); 
			JDQ_tab1[6] = (MY_JDQ_TAB2[ 12 ]&0x0f)|(MY_JDQ_TAB2[ 13 ]<<4);
			JDQ_tab1[7] = (MY_JDQ_TAB2[ 14 ]&0x0f)|(MY_JDQ_TAB2[ 15 ]<<4);

			memcpy(JDQ_tab3 , JDQ_tab1 ,8 );

			write_595_JDQ( JDQ_tab1 , 8 );

		}break;
		
		case DTN_16L_5T_1A :   
		{
			exchange_jdq_val_status( &JDQ_tab1[0] ,MY_JDQ_TAB[0], &ACCIDENTAL_time[0] ); 
			exchange_jdq_val_status( &JDQ_tab1[1] ,MY_JDQ_TAB[1], &ACCIDENTAL_time[1]); 
			exchange_jdq_val_status( &JDQ_tab1[2] ,MY_JDQ_TAB[2], &ACCIDENTAL_time[2]); 
			exchange_jdq_val_status( &JDQ_tab1[3] ,MY_JDQ_TAB[3], &ACCIDENTAL_time[3]); 		 
			exchange_jdq_val_status( &JDQ_tab1[4] ,MY_JDQ_TAB[4], &ACCIDENTAL_time[4]); 
			exchange_jdq_val_status( &JDQ_tab1[5] ,MY_JDQ_TAB[5], &ACCIDENTAL_time[5]); 
			exchange_jdq_val_status( &JDQ_tab1[6] ,MY_JDQ_TAB[6], &ACCIDENTAL_time[6]); 
			exchange_jdq_val_status( &JDQ_tab1[7] ,MY_JDQ_TAB[7], &ACCIDENTAL_time[7]); 	
			exchange_jdq_val_status( &JDQ_tab1[8] ,MY_JDQ_TAB[8], &ACCIDENTAL_time[8]); 
			exchange_jdq_val_status( &JDQ_tab1[9] ,MY_JDQ_TAB[9], &ACCIDENTAL_time[9]); 
			exchange_jdq_val_status( &JDQ_tab1[10] ,MY_JDQ_TAB[10], &ACCIDENTAL_time[10]); 
			exchange_jdq_val_status( &JDQ_tab1[11] ,MY_JDQ_TAB[11], &ACCIDENTAL_time[11]); 	
			exchange_jdq_val_status( &JDQ_tab1[12] ,MY_JDQ_TAB[12], &ACCIDENTAL_time[12]); 
			exchange_jdq_val_status( &JDQ_tab1[13] ,MY_JDQ_TAB[13], &ACCIDENTAL_time[13]); 
			exchange_jdq_val_status( &JDQ_tab1[14] ,MY_JDQ_TAB[14], &ACCIDENTAL_time[14]); 
			exchange_jdq_val_status( &JDQ_tab1[15] ,MY_JDQ_TAB[15], &ACCIDENTAL_time[15]); 	

			JDQ_tab2[0] = (JDQ_tab1[ 0 ]&0x0f)|(JDQ_tab1[ 1 ]<<4);  
			JDQ_tab2[1] = (JDQ_tab1[ 2 ]&0x0f)|(JDQ_tab1[ 3 ]<<4);  
			JDQ_tab2[2] = (JDQ_tab1[ 4 ]&0x0f)|(JDQ_tab1[ 5 ]<<4); 
			JDQ_tab2[3] = (JDQ_tab1[ 6 ]&0x0f)|(JDQ_tab1[ 7 ]<<4); 
			JDQ_tab2[4] = (JDQ_tab1[ 8 ]&0x0f)|(JDQ_tab1[ 9 ]<<4); 
			JDQ_tab2[5] = (JDQ_tab1[ 10 ]&0x0f)|(JDQ_tab1[ 11 ]<<4);  
			JDQ_tab2[6] = (JDQ_tab1[ 12 ]&0x0f)|(JDQ_tab1[ 13 ]<<4); 
			JDQ_tab2[7] = (JDQ_tab1[ 14 ]&0x0f)|(JDQ_tab1[ 15 ]<<4); 

			memcpy( JDQ_tab3,JDQ_tab2,8 );
			write_595_JDQ( JDQ_tab2 , 8 ); 
		}break;
		
		default:

		break;		 
	}
					    
	if( MODE_SW3 )     //控制故障显示灯的    0是关闭
	{
		ia = TransForm_JDQ_to_LED_VALUE( MY_SWTYPE , JDQ_tab3 );
		write_595_LED( (u8*)&ia ,2 );
	} 
	else
	{       
		ia = 0;
		write_595_LED( (u8*)&ia ,2 ); 
	}		  
}


int main(void)
{  
	u8 i = 0, dat[8] = { 01,03,00,0x48,00,05,05,0xdf }; 
	TRAN_D_struct   TRAN_info1; 

	delay_init();	        	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	BSP_init(); 

	HAL_GetUID(MCU_UID);     //获得单片机唯一码

	MY_CODE = read_code();   //拨码编号

	read_SWTYPE();  //获得自己的设故板类型

	switch( MY_SWTYPE )//默认的无故障
	{
		case DTN_8L_2T_16A : 
		case DTN_16L_2T_3A :
		{			
			for(i=0;i<16;i++)
			{
				MY_JDQ_TAB[i] = 0;
			}
			//write_595_JDQ( MY_JDQ_TAB , 16 );
		} break; 
		
		case DTN_16L_5T_1A : 
		{			
			for(i=0;i<16;i++)
			{
				MY_JDQ_TAB[i] = 0;
			}
			//write_595_JDQ( MY_JDQ_TAB , 16 );
		} break;
		
		default:

		break;		 
	}
	

	delay_ms(1000);

	uart1_init (57600);	//初始化串口

	TIM2_Int_Init(9,7199);//72M/7200   10Khz的计数频率，计数到9为1ms 


	//设备开机查询 上一级地址   
	alarm_time = 3000;
	search_addr_flg = 0;

	send_urt_time = 100;//开机不能不检查 ，就立马发数据
		 
	while(1)
	{ 
		if(search_addr_flg == 0)		//搜索地址标志
		{
			if( alarm_time > 1500 )
			{   
				alarm_time = 0;

				TRAN_info1.dest_dev_num = ( DTN_android << 8 | DTN_android >> 8 );

				TRAN_info1.dest_addr[0] = 0x00;
				TRAN_info1.dest_addr[1] = 0x00;
				TRAN_info1.dest_addr[2] = 0x00;
				TRAN_info1.dest_addr[3] = 0x00;

				TRAN_info1.source_dev_num = ( MY_SWTYPE << 8 | MY_SWTYPE >> 8 );

				TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
				TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM = 1;   //第一类
				TRAN_info1.data_len = 2;

				dat[0] = 00;  
				dat[1] = 02;   //获取上一级设备种类识别号和地址 CMD 0X0002   反馈 6个字节

				RS485_V_I_SD_load_buf( 0xAAAA,0xBBBB,  &TRAN_info1 , dat ,TRAN_info1.data_len );  
			}						 			   
		}
		else
		{
			MY_CODE = read_code();   //拨码编号

			if( alarm_time > 2000 )
			{  
				alarm_time = 0;	 
							 
				TRAN_info1.dest_dev_num = (DTN_android<<8|DTN_android>>8);

				TRAN_info1.dest_addr[0] = 0x00;
				TRAN_info1.dest_addr[1] = 0x00;
				TRAN_info1.dest_addr[2] = 0x00;
				TRAN_info1.dest_addr[3] = 0x00;

				TRAN_info1.source_dev_num = (MY_SWTYPE<<8|MY_SWTYPE>>8);

				TRAN_info1.source_addr[0] = (u8)MCU_UID[0];
				TRAN_info1.source_addr[1] = (u8)(MCU_UID[0]>>8);
				TRAN_info1.source_addr[2] = (u8)(MCU_UID[0]>>16);
				TRAN_info1.source_addr[3] = (u8)(MCU_UID[0]>>24);

				TRAN_info1.TYPE_NUM = 2;   //第2类
				TRAN_info1.data_len = 2 + 3;

				dat[0] = 00;  
				dat[1] = 04;   // 上传自己的  编码号 设备号 
				dat[2] = MY_CODE;  
				dat[3] = MY_SWTYPE >> 8; 
				dat[4] = MY_SWTYPE ; 
				RS485_V_I_SD_load_buf( 0xAAAA,0xBBBB, &TRAN_info1 , dat ,TRAN_info1.data_len );  
			}  
		} 
			 
		handle_jdq_fuc();  

		RS485_V_I_uart_fuc(); 

		if( send_urt_time == 0 )
		{
			RS485_V_I_dma_send_buf(); 
		}
	}
}
