/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-07-15 16:26:43
 * @Description: 
 * ————无线通讯协调器，用于接收下位机信息后转发给安卓
 * 	入网等待指示灯，接收指示灯
 * 
 * 实训台子设备->Zigbee-R==>Zigbee-C->协调器->安卓主控
 * 
 * ---------------------------------------------------------------------------*/


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

/*---------------------------------------------------------------------------
 * @Description:获取STM32板载ID
 * @Param:      UID，ID存放指针
 * @Return:     无
 *---------------------------------------------------------------------------*/
void HAL_GetUID(uint32_t *UID);
/*---------------------------------------------------------------------------
 * @Description:定时器2 初始化函数声明为外部函数
 * @Param:      arr：自动重装值，psc：时钟预分频数
 * @Return:     无
 *---------------------------------------------------------------------------*/
extern void TIM2_Int_Init(u16 arr,u16 psc);
/*---------------------------------------------------------------------------
 * @Description:整体初始化
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
void  Init(void) ;

/*---------------------------------------------------------------------------
 * @Description:程序入口，功能按键2功能为允许入网，zigbee数据转发
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
int main(void)
{
    	TRAN_D_struct   TRAN_info1;
	 u8   dat[100] = {1,2,3,4,5,6,7,8,9,10,11};
	 u8 KEY_VLUE = 255;
	 
	Init();
	
	
	
	//printf("this is a test\n");
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


/*---------------------------------------------------------------------------
 * @Description:模块初始化，串口，时钟总线，中断，IO
 * @Param:      无
 * @Return:     无
 *---------------------------------------------------------------------------*/
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
/*---------------------------------------------------------------------------
 * @Description:为空检测
 * @Param:      数组
 * @Return:     有/无
 *---------------------------------------------------------------------------*/
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


/*---------------------------------------------------------------------------
 * @Description:STM32出厂自带ID
 * @Param:      ID存放指针
 * @Return:     无
 *---------------------------------------------------------------------------*/
void HAL_GetUID(uint32_t *UID)
{
  UID[0] = (uint32_t)(READ_REG(*((uint32_t *)UID_BASE)));
  UID[1] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 4U))));
  UID[2] = (uint32_t)(READ_REG(*((uint32_t *)(UID_BASE + 8U))));
} 





