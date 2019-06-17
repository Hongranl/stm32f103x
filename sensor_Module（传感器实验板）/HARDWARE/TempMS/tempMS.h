#ifndef TEMP_MS_H
#define TEMP_MS_H

#include "delay.h"
#include "stm32f10x.h"
#include "sys.h"


#include "stdio.h"
#include "string.h"




typedef struct{

	float temp;
	float hum;
}tempMS_MEAS;



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

#define SHT_RD_DATA()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_3)
#define SHT_DATA(level)  (PBout(3)=level)
#define SHT_DATA_OUT  (SHT_WInit())
#define SHT_DATA_IN  (SHT_RInit())
#define SHT_SCK(level)  (PBout(4)=level)

#define udelay()	delay_us(1);

extern void reset(void);
extern void sht11_start(void);
extern void sendByte(uint8 value);
extern uint8 recvByte(uint8 ack);

extern void readTH(uint8 *pData, uint8 *pStartIndex);

extern void convert_shtxx(float * hum, float *temp);



#endif

