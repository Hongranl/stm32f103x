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
 #include "stm32f10x_iwdg.h"
 
#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "usart_cfg.h" 
#include "bsp_iwdg.h" 
#include "bsp_internal_flash.h"   


#include "timer.h" 

#include "usart1_RS485_V&I.h"  
#include "usart2_RS485_comm.h"  
 
  #define UID_BASE              0x1FFFF7E8U  

 /*
设备种类号	终端安卓机	20
	实训管理器主板	21
	电源管理器主板	22
	16路2态3A设故板	23
	16路5态1A设故板	24
	8路2态16A设故板	25
*/

 #define  DTN_android          20      //设备号 
// #define  DTN_main_manage      21    
 #define  DTN_power_manage     22
 //#define  DTN_16L_2T_3A        23
 //#define  DTN_16L_5T_1A        24
 //#define  DTN_8L_2T_16A        25
 
 





extern  u32      MY_DEST_ADDR ;
extern  uint32_t MCU_UID[3];
extern u16       My_vol; 
extern u16		   My_curr;  
extern u16       My_watt; 
extern u32		   My_powr;


extern u8     search_addr_flg ;
extern u16    alarm_time ; 
 
extern u16   send_urt_time ;

extern  u8   POWER_220V_FLG ;  //AC_CHECK


#endif
