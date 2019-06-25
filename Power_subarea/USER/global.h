#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
   GTA-GXU01 实训管理器主控板
          2018-9-18 
					by: 富宝东					
  
	引脚使用：
	LED:   LED  PB9 ; 	
	
	BEEP: pc4
	
	串口：RS485 (接电压电流计)    UART1   PA9 PA10    con_pin1  PA8
	      RS485(通信)            UART2   PA2  PA3    con_pin2  PA4
	  
  AC_CHECK    220V是否接通进来

  AC_con      220V 继电器控制
				
****************************************/
 #include "stdio.h"	
#include "sys.h" 
#include <ctype.h>
#include <string.h>
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
 #include "stm32f10x_exti.h"
 
#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "usart_cfg.h" 

#include "timer.h" 

#include "usart3_RS485_V&I.h"  
#include "usart1_zigbee_comm.h"  
 

#include "fuc_595.h" 



#define UID_BASE              0x1FFFF7E8U  

 

 #define  DTN_motor_window          50      //设备号   电动窗帘
// #define  DTN_curtain               51      //设备号   电动幕帘
 #define  DTN_POWER_SUBAREA         52      //设备号   电源分区管理
 
  
	
 typedef enum 
{ 
	join_sw=1,
	leave_net,
	notify_net_status,
	upload_info,
	control_info,
	query_dev_info

}dev_cmd ;	

	
	

extern  u32      MY_DEST_ADDR ;
extern  uint32_t MCU_UID[3];
extern u16       My_vol; 
extern u16		   My_curr;  
extern u16       My_watt; 
extern u32		   My_powr;

extern u16     My_AC_frequency; 
extern u16		 My_three_phase_W_H16;  
extern u16     My_three_phase_W_L16; 
extern u16		 My_A_PHASE_I;
extern u16		 My_A_PHASE_U;
extern u16		 My_A_PHASE_W;

extern u16		 My_B_PHASE_I;
extern u16		 My_B_PHASE_U;
extern u16		 My_B_PHASE_W;

extern u16		 My_C_PHASE_I;
extern u16		 My_C_PHASE_U;
extern u16		 My_C_PHASE_W;



extern  u8     search_addr_flg ;
extern  u16    alarm_time ; 
 
extern  u16   join_key_time ;
 
extern  u16   send_urt_time ;

extern  u8   POWER_220V_FLG ;  //AC_CHECK

extern  u16   JDQ_16BIT ;

extern  u8   join_flg ;

extern  uint16_t crc16_MODBUS(uint8_t *ptr, uint16_t len);


#endif
