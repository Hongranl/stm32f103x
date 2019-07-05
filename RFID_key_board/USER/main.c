/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训室项目
 * @Version:    V 0.2 
 * @Module:     main
 * @Author:     RanHongLiang
 * @Date:       2019-07-05 14:09:28
 * @Description: 
 * ————实训室讲台面板按键与读卡程序
 * ---------------------------------------------------------------------------*/


#include "global.h"

u32  ANDROID_DEST_ADDR = 0;

u16  motor_ck_time = 0;

u16  alarm_time = 0;

u8   global_mode = 0;


uint32_t         MCU_UID[3];

u8               LED_DAT[2];

u8               m_mode = 0;

u16              join_LED_S = 0;

u16              send_urt_time = 0;

u16              PC_ON_TIME = 0;
 

u8  main_CHECK_BUF_VALID(u8 *p) 
{  
	u8 i = 0;

	for(i=0;i<3;i++)
	{
		if( p[i] != 0 )
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
/*
 * 设置 IWDG 的超时时间
 * Tout = prv/40 * rlv (s)
 *      prv可以是[4,8,16,32,64,128,256]
 * prv:预分频器值，取值如下：
 *     @arg IWDG_Prescaler_4: IWDG prescaler set to 4
 *     @arg IWDG_Prescaler_8: IWDG prescaler set to 8
 *     @arg IWDG_Prescaler_16: IWDG prescaler set to 16
 *     @arg IWDG_Prescaler_32: IWDG prescaler set to 32
 *     @arg IWDG_Prescaler_64: IWDG prescaler set to 64
 *     @arg IWDG_Prescaler_128: IWDG prescaler set to 128
 *     @arg IWDG_Prescaler_256: IWDG prescaler set to 256
 *
 * rlv:预分频器值，取值范围为：0-0XFFF
 * 函数调用举例：
 * IWDG_Config(IWDG_Prescaler_64 ,625);  // IWDG 1s 超时溢出
 */

void IWDG_Config(uint8_t prv ,uint16_t rlv)
{	
	// 使能 预分频寄存器PR和重装载寄存器RLR可写
	IWDG_WriteAccessCmd( IWDG_WriteAccess_Enable );
	
	// 设置预分频器值
	IWDG_SetPrescaler( prv );
	
	// 设置重装载寄存器值
	IWDG_SetReload( rlv );
	
	// 把重装载寄存器的值放到计数器中
	IWDG_ReloadCounter();
	
	// 使能 IWDG
	IWDG_Enable();	
}

// 喂狗
void IWDG_Feed(void)
{
	// 把重装载寄存器的值放到计数器中，喂狗，防止IWDG复位
	// 当计数器的值减到0的时候会产生系统复位
	IWDG_ReloadCounter();
}

 int main(void)
{  
	TRAN_D_struct   TRAN_info1;

	u8 M_tagfounds = 1;

	delay_init();	        	 //延时函数初始化	  
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级

	HAL_GetUID(MCU_UID);     //获得单片机唯一码

	LED_Init();			       	//LED端口初始化 
	BEEP_INIT(); 
	KEY_Init(); 
	uart3_init(57600);
	
	TIM2_Int_Init(9,72);//72M/72   1000Khz的计数频率，计数到9为10us 

	LED_DAT[0] = 0x00;
	LED_DAT[1] = 0x00;
	write_595_LED( LED_DAT,2);
	
	IWDG_Config(IWDG_Prescaler_64,625);
	Interrupts_Config(); 

	CR95HF_ConfigManager_HWInit();
	
	while(1)
	{  
		// 	disable_all_uart_interupt(   ); 
		CR95HF_FUC( M_tagfounds );    //刷卡的  drv95HF_SPIPollingCommand 
		(M_tagfounds>=TRACK_NFCTYPE5)?( M_tagfounds=TRACK_NFCTYPE1 ):( M_tagfounds*=2  );
		// enable_all_uart_interupt(   );
		key_fuc();

		RS485_uart_fuc(); 
		RS485_dma_send_buf();
		IWDG_Feed();
		if( key_tab[5] == 1 )//=1 正在开机  ，LED闪
		{
			if(PC_ON_TIME >= 1000)
			{
				PC_ON_TIME = 0;

				if( LED_DAT[1] & L_R2C2 )
				{
					LED_DAT[1] &=~ L_R2C2;
				}
				else
				{
					LED_DAT[1] |= L_R2C2;
				}

				LED_DAT[0] = 0x00;

				write_595_LED( LED_DAT,2);
			}
		}
	}
}
