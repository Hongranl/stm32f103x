#ifndef TEMP_MS_H
#define TEMP_MS_H


#include "ALL_Includes.h"


typedef struct{

	float temp;
	float hum;
}tempMS_MEAS;


tempMS_MEAS tempMS_Thread( u8*);


/***********************************************************************
 * CONSTANTS
 */
#define SHT_NOACK       0
#define SHT_ACK         1

#define SHT_MODE_TEMP   0
#define SHT_MODE_HUMI   1

//adr  command  r/w
#define SHT_STATUS_REG_W        0x06   //000   0011    0
#define SHT_STATUS_REG_R        0x07   //000   0011    1
#define SHT_MEASURE_TEMP        0x03   //000   0001    1
#define SHT_MEASURE_HUMI        0x05   //000   0010    1
#define SHT_RESET               0x1e

/***********************************************************************
 * MACROS
 */

typedef  uint8_t uint8;
typedef  uint16_t uint16;
typedef  u32 uint32;

typedef union
{
	uint8_t sensor_dat[4];
	struct{
		uint16_t temp;
		uint16_t hum;
	}hum_temp;
}result_t;
#define SHT_data_Pin GPIO_PIN_2
#define SHT_sck_Pin  GPIO_PIN_3


#define SHT_RD_DATA()  GPIO_ReadInputPin(GPIOD,GPIO_PIN_2)//(GPIOD->IDR & SHT_data_Pin )// == 0 ) ? 0 : 1 )//GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_7)
#define SHT_DATA(level)  (level==0)?(GPIOD->ODR &= ~SHT_data_Pin  ):( GPIOD->ODR |= SHT_data_Pin )//(PBout(7)=level)
#define SHT_DATA_OUT  (SHT_WInit())
#define SHT_DATA_IN  (SHT_RInit())
#define SHT_SCK(level)  (level==0)?(GPIOD->ODR &= ~SHT_sck_Pin  ):( GPIOD->ODR |= SHT_sck_Pin )//(PBout(6)=level)

#define udelay()	Delay(5)//2-50uS∂ºø…“‘

#endif

