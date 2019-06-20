/*************************************
名称：物联网通讯扩展板
时间：2019-05-14
作者：冉宏亮
概要：
****************************************/
#include "global.h"


 
u32  ANDROID_DEST_ADDR = 0;

u16  motor_ck_time = 0;

u16  alarm_time = 0;

u8   global_mode = 0;

extern u8 gb_dat[18];

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
void HAL_GetUID(uint32_t *UID);
void  IR_send_waveform( u8 CAP_IR_num  ) ;
extern void TIM2_Int_Init(u16 arr,u16 psc);


void  Init(void) ;

void cnt_delay(u16 num)
{
  u16 i,j;
  for(i=0;i<num;i++)
    for(j=0;j<0x800;j++);
}


int main(void)
{
    	TRAN_D_struct   TRAN_info1;
	 u8   dat[100] = {1,2,3,4,5,6,7,8,9,10,11};
	 u8 KEY_VLUE = 255;
	 
	Init();
	
	
	
	printf("this is a test\n");
	while (1)
		{

					 

			if (KEY2 == 0)
			{
				
				while(KEY2 == 0)
					{
					delay_ms(20);
					}
				KEY_VLUE = !KEY_VLUE;
					
			}


			if(KEY_VLUE == 1)
			{
				
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

				zigbee_SD_load_buf(0xAAAA ,0xBBBB , &TRAN_info1, dat ,TRAN_info1.data_len ); 
				
				
				
				KEY_VLUE = !KEY_VLUE;
			}
			
			
			zigbee_uart_fuc() ;
			zigbee_dma_send_buf(); 
			
			android_uart_fuc() ;
			android_dma_send_buf(); 
			delay_ms(10);
			 
		}
}



void  Init(void)
{
	
	delay_init();	        	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	HAL_GetUID(MCU_UID);     //获得单片机唯一码

	TIM2_Int_Init(9,72);
	   
	//LCD144_Init();
	IO_Init();

	uart1_init(115200);
	//uart2_init(115200);
	uart3_init(115200);
	
	
	
}

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





