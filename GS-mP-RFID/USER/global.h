#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
   GTA-GXU01 实训管理器主控板
          2018-9-18 
					by: 富宝东					
  
	引脚使用：
	LED:  RFID  LED  PB5 ;
        RS232 LED  PB4 ;	
        RS485 LED  PB3 ;		
	
	BEEP: pc4
	
	串口：RS485(接设故盒)        UART3   PB10 PB11
	      RS232(接ANDROID主机)   UART2   PA2  PA3
				打印输出 ，调试        UART1   PA9  PA10  

  RFID 13.56M   CR95HF  CR95V5: 
	      IRQ_IN            PC8
	      IRQ_OUT           PC9
	      CR95HF_NSS        PB12
	      CR95HF_CLK        PB13
	      CR95HF_MOSI       PB15
	      CR95HF_MISO       PB14
				CR95HF_INTERFACE  PC6
				
				
****************************************/

#include "stdio.h"	
#include "sys.h" 
#include <ctype.h>
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h" 
#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "timer.h" 


#pragma pack(1)
typedef  struct
{   
	u16   dest_dev_num;     //目标设备号
	u8    dest_addr[4];     //目标地址
	u16   source_dev_num;   //源设备号
	u8    source_addr[4];   //源地址 
	u8    TYPE_NUM ;        //数据格式号
	u8    data_len ;        //数据长度	 
}
TRAN_D_struct;    //在头文件前



 


#include "usart_cfg.h"  
#include "usart3_rs485.h"  
 


#include "CR95HF.h" 

#include "lib_ConfigManager.h"
#include "hw_config.h"
#include "drv_interrupt.h"
#include "lib_iso14443Apcd.h"
#include "lib_iso18092pcd.h"
#include "lib_iso14443Bpcd.h"

 

#include "stmflash.h"


#include  "fuc_595.h"



#define UID_BASE              0x1FFFF7E8U  
 
   
 
  /*
设备种类号	终端安卓机	20
	实训管理器主板	21
	电源管理器主板	22
	16路2态3A设故板	23
	16路5态1A设故板	24
	8路2态16A设故板	25
	
	集控中心板     26
	集控安卓机     27
*/

  #define  DTN_TER_RIFD          19      //设备号 
  #define  DTN_android          20      //设备号 

 
 

 #define  L_R1C4    0X80
 #define  L_R1C3    0X40
 #define  L_R1C2    0X20
 #define  L_R1C1    0X10
 
 #define  L_R2C4    0X08
 #define  L_R2C3    0X04
 #define  L_R2C2    0X02
 #define  L_R2C1    0X01
 
 #define  L_R3C4    0X80
 #define  L_R3C3    0X40
 #define  L_R3C2    0X20
 #define  L_R3C1    0X10
 
 #define  L_R4C4    0X08
 #define  L_R4C3    0X04
 #define  L_R4C2    0X02
 #define  L_R4C1    0X01
 
extern  uint32_t MCU_UID[3];

extern ISO14443B_CARD 	ISO14443B_Card;
extern ISO14443A_CARD 	ISO14443A_Card;
extern uint8_t TagUID[16];
extern FELICA_CARD 	FELICA_Card;
 
extern  u16  motor_ck_time ;
extern  u16 alarm_time ;

extern  u32 ANDROID_DEST_ADDR ;
 
extern  u8            m_mode ;
extern  u8            search_addr_flg ;

extern  u8            LED_DAT[2];
extern  u16           join_LED_S ;
 
extern  u16           send_urt_time ;

#endif
