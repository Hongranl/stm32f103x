#ifndef _BSP_H__
#define _BSP_H__

#include "eeprom.h"

#include "SPI_faultSet.h"
#include "485Trans_USART.h"
#include "Debug.h"
#include "Beep.h"

void ADDRGET_Init(void);
u8 Localaddr_get(void);
u8 LocalBDType_get(void);
void Bsp_Init(void);

#endif