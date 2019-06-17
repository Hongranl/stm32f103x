#ifndef __GLOBAL_H
#define __GLOBAL_H 


/*************************************
				
				
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

#include "usart1.h"
#include "usmart.h"
#include "LCD_144.h"
#include "GUI_144.h"









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



#pragma pack(push,1)
typedef  struct
{  
	u32   CAP_IR_WAVEFORM_TIME;
	u32   CAP_IR_store_num;
	u32   IR_tab_js ;

	u16   IR_tab_len ;  //长度 
	u16   IR_tab[1024];  //获取的帧数据
}
IR_CAP_struct;    
#pragma pack(pop)


#include "usart_cfg.h" 
#include "usart3_android.h" 
#include "usart4_ZIG1.h" 
#include "usart2_rs485.h"  
#include "LCD_144.h"//显示驱动进程函数； 


#include "stmflash.h"


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

 

#define  DTN_JCZX_BOARD          	26
#define  DTN_JCZX_ANDROID        	27
#define  DTN_JCZX_fanzhuang       28
 

#define  shegu_tab_size       100 //存储设故板的编号和设备号，排列是两个字节
  
 
 

extern void  remote_cap_study( void  );

extern void IR_js( void );

extern  IR_CAP_struct   IR_CAP1;

 
extern  uint32_t MCU_UID[3];


extern uint8_t TagUID[16];

 
extern  u16  motor_ck_time ;
extern  u16 alarm_time ;

extern  u32 ANDROID_DEST_ADDR ;
 
extern  u8            m_mode ;
extern  u8            search_addr_flg ;

extern  u8            IR_CAP_START_FLG ;

extern  u16           join_LED_S ;
 
extern  u16           send_urt_time ;

extern  u8            JDQ_NOW_STATUS ;
extern u8             JDQ_WILL_STATUS ;
extern  u16           JDQ_time;

extern  u8            PC_KEY_FLAG;

#endif
