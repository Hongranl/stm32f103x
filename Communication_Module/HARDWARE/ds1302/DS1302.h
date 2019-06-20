#ifndef __DS1302_H
#define __DS1302_H


#include "stdio.h"	
#include "sys.h" 
#include <ctype.h>
 


#define   DS1302_SCLK(x)      (x?GPIO_SetBits(GPIOE,GPIO_Pin_14):GPIO_ResetBits(GPIOE,GPIO_Pin_14))

#define   DS1302_OUTPUT(x)    (x?GPIO_SetBits(GPIOE,GPIO_Pin_13):GPIO_ResetBits(GPIOE,GPIO_Pin_13))

#define   DS1302_CE(x)        (x?GPIO_SetBits(GPIOE,GPIO_Pin_12):GPIO_ResetBits(GPIOE,GPIO_Pin_12))
 
#define   DS1302_INPUT(x)     (GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_13))
 


typedef   struct
{
   u8  hour;
	 u8  min;
	 u8  sec;
	 u16 w_year;
	 u8  w_month;
	 u8  w_date;
	 u8  w_week; 
}DS_TIME_T;

extern DS_TIME_T   DS_TIME;

#define  ds1302_Charge()    WriteOneWord_DS1302(0x90,0xa4)
#define  ENWrite()          WriteOneWord_DS1302(0x8e,0x00)
#define  DISWrite()         WriteOneWord_DS1302(0x8e,0x80)




extern u8     ChangeToHex(u8 dat);
extern u8     ChangeToDec(u8 dat);
extern void   DS1302_IO_Init(void);
extern void   WriteOneChar_DS1302(u8  m);	
extern u8     ReadOneChar_1302(void);
extern u8     ReadOneWord_1302(u8  address);
extern void   WriteOneWord_DS1302(u8 address,u8  shuju);
extern void   Settime_ds1302(u16 year,u8 month,u8 day ,u8 hour,u8 minute,u8 sec);
extern void   Gettime_ds1302(void);




#endif

