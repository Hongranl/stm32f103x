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
#include "stdbool.h"
#include "sys.h" 
#include <ctype.h>
#include <string.h>
#include "stm32f10x.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_adc.h"

#include "bit_band.h"
#include "led.h"
#include "delay.h"
#include "key.h"
#include "dma.h"
#include "sys.h"
#include "usart_cfg.h" 
#include "timer.h" 
#include "usart1_RS485_V&I.h"  
#include "usart2_RS485_comm.h"  
#include "driver_iic.h"
#include "adc.h"
#include "peripheral.h"
#include "SHT1X.h"
#include "LCD_144.h"
#include "tempMS.h"

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

/*********宏定义声明**********/
#define Fan_AdcIndex 				(0)
#define HD136_AdcIndex				(1)
#define RPC18_AdcIndex				(2)

/*Modbus地址码宏定义*/
#define Soil_Detector_Addr			(0x03)			//土壤温湿度
#define Anemograph_Addr				(0x04)			//风速计

/*****************************/

/*********结构体声明**********/
typedef struct
{
	int Temperature;//转换的温度
	u16 Humidity;   //转换的湿度
    u16 DEW;
    float T_Result;
    float H_Result;
	u8 CRC8_Temperature; //校验码
	u8 CRC8_Humidity;  //校验码
}SHT1x;
/*****************************/


/********变量外部声明*********/
extern uint64_t SystemCnt;		//系统运行时间(ms)
extern bool TimeOutFlag;		//10ms到达标志位
extern u8  Tim1CaptureState;	//TIM1捕获状态位  		最高位bit7-成功捕获 次高位bit6-已经捕获到上升沿 bit5~bit0-高电平后溢出次数
extern u16 Tim1CaptureCount;	//TIM1捕获计数值
extern bool HcSr04StartF;		//测距传感器开始的标志
extern u8  SwitchType;			//8421编码开关键值
extern u32 AdcBuf[3];			//保持ADC采集值
extern u16 PwmSpeed;			//调速风扇转速
extern bool PwmPowerFlag;		//风扇开启标志
extern SHT1x sht1x; 			//定义传感器结构体
extern float SoilTemp;			//土壤温度
extern float SoilHum;			//土壤湿度
extern float WindSpeed;			//风速值
extern bool  Key1Flag;			//KEY1按键值
extern bool  Key2Flag;			//KEY2按键值
extern uint64_t  BeepCnt;		//蜂鸣器开启计时

extern u16 humi_val, temp_val;	/**/
extern float humi_val_real; 	//SHT11 温湿度相关变量
extern float temp_val_real;		/**/
extern float dew_point;			/**/

/*****************************/


#endif
