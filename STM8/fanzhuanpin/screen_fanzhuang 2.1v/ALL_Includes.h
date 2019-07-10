

#ifndef  _ALL_Includes_H
#define  _ALL_Includes_H

#include "stm8s.h"
#include "stdio.h" 
#include <ctype.h>
#include <string.h> 
#include "uart.h" 



#ifndef BIT
#define BIT(x)	(1 << (x))
#endif



/* 用的595 
#define  LED_SCREEN_Pin   (1 << 3) 
#define  LED_UP_Pin       (1 << 3) 
#define  LED_DOWN_Pin     (1 << 3) 
*/


#define   KEY1_PA3_Pin   (1 << 3) 
#define   KEY2_PB5_Pin   (1 << 5) 
#define   KEY3_PB4_Pin   (1 << 4) 


#define   KEY1_PA3_Pin_RD    (GPIOA->IDR & KEY1_PA3_Pin ) 
#define   KEY2_PB5_Pin_RD    (GPIOB->IDR & KEY2_PB5_Pin )
#define   KEY3_PB4_Pin_RD    (GPIOB->IDR & KEY3_PB4_Pin )


#define   DNLIMIT_PC3_Pin   (1 << 3) 
#define   UPLIMIT_PC4_Pin   (1 << 4) 
#define   CLAMP_PC5_Pin     (1 << 5) 


#define   DNLIMIT_PC3_Pin_RD    (GPIOC->IDR & DNLIMIT_PC3_Pin ) 
#define   UPLIMIT_PC4_Pin_RD    (GPIOC->IDR & UPLIMIT_PC4_Pin )
#define   CLAMP_PC5_Pin_RD      (GPIOC->IDR & CLAMP_PC5_Pin )



#define   RS485_LED_PA2_Pin   (1 << 2)
#define   RS485_DIR_PD4_Pin   (1 << 4)
#define   RS485_DIR_PD4_Pin_OUT(a)     (a!=0)?( GPIOD->ODR |= RS485_DIR_PD4_Pin ):(  GPIOD->ODR &= ~RS485_DIR_PD4_Pin) 
#define   RS485_LED_PA2_Pin_OUT(a)     (a!=0)?( GPIOA->ODR |= RS485_LED_PA2_Pin ):(  GPIOA->ODR &= ~RS485_LED_PA2_Pin) 



#define   H595_DS_PD2_Pin   (1 << 2)
#define   H595_ST_PD3_Pin   (1 << 3)
#define   H595_SH_PA1_Pin   (1 << 1)

#define   H595_DS_PD2_Pin_OUT(a)     (a!=0)?( GPIOD->ODR |= H595_DS_PD2_Pin ):(  GPIOD->ODR &= ~H595_DS_PD2_Pin) 
#define   H595_ST_PD3_Pin_OUT(a)     (a!=0)?( GPIOD->ODR |= H595_ST_PD3_Pin ):(  GPIOD->ODR &= ~H595_ST_PD3_Pin) 
#define   H595_SH_PA1_Pin_OUT(a)     (a!=0)?( GPIOA->ODR |= H595_SH_PA1_Pin ):(  GPIOA->ODR &= ~H595_SH_PA1_Pin) 




#define   SCREEN_LED_R_PC6_Pin   (1 << 6) 
#define   SCREEN_LED_G_PC7_Pin   (1 << 7)
 
#define   SCREEN_LED_R_PC6_Pin_RD    (GPIOC->IDR & SCREEN_LED_R_PC6_Pin ) 
#define   SCREEN_LED_G_PC7_Pin_RD    (GPIOC->IDR & SCREEN_LED_G_PC7_Pin )

 
#define   SCREEN_ON_595Pin   (1 << 0)  
#define   RLY1_595Pin   (1 << 1) 
#define   RLY2_595Pin   (1 << 2) 
#define   RLY3_595Pin   (1 << 3) 

#define   LED1_595Pin   ((1 << 6)) 
#define   LED2_595Pin   ((1 << 5) )
#define   LED3_595Pin   ((1 << 4) )


#define   motor_up    1
#define   motor_down  2
#define   motor_stop  3

#define   motor_limit_up    4
#define   motor_limit_down  5


#define   screen_on     1
#define   screen_off    2 



  /*
设备种类号	终端安卓机	20
	实训管理器主板	21
	电源管理器主板	22
	16路2态3A设故板	23
	16路5态1A设故板	24
	8路2态16A设故板	25
	
	集控中心板     26
	集控安卓机     27
        翻转屏控制板   28
*/

 #define  DTN_JCZX_BOARD          26
 #define  DTN_JCZX_ANDROID        27
  
 #define  DTN_JCZX_fanzhuang       28

/*	
 typedef enum 
{ 
	join_sw=1,
	leave_net,
	notify_net_status,
	upload_info,
	control_info,
	query_dev_info

}dev_cmd ;	
*/


typedef enum
{
  Key_None = 0,
  Key_Down,
  Key_Up,
  Key_Screen
}Key;

typedef enum
{
  UART_RX_CMD_None = 0,
  UART_RX_CMD_screen_on ,
  UART_RX_CMD_screen_off ,
  UART_RX_CMD_motor_up ,
  UART_RX_CMD_motor_down ,
  UART_RX_CMD_motor_stop 
} UART_CMD;



extern void write_595( u8  *dat,int  len );
extern  void Delay(uint16_t nCount);

extern  u8    dat_595 ; 
extern  u8    motor_status ; 
extern  u8    screen_status ; 
extern  u8    screen_sw_mode ;
extern  u16   screen_relay_off_time ;
extern  u16   screen_relay_on_time ; 
extern  u16   screen_on_time ;

extern  u16   CLAMP_CK_TIM ;

extern   u8    flash_led_power ;

extern   u8    motor_running_status ;


extern  u8    Data_Len ;
 
extern  u8    uart_rx_flg ;

extern  u16   UART_NO_DAT_TIME ;

extern  u16   JDQ_TIME ;

extern  u8 UART_RX_CMD ;

#endif

