#ifndef __RELAY_H_
#define __RELAY_H_

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#define relayDIN	PBout(6)
#define relayMR		PBout(9)
#define relaySTCP	PBout(7)
#define relaySHCP	PBout(8)

#define relayK0		PAin(4)
#define relayK1		PAin(7)
#define relayK2		PBin(0)
#define relayK3		PBin(1)
#define relayK4		PBin(12)
#define relayK5		PBin(13)
#define relayK6		PBin(14)
#define relayK7		PAin(8)
#define relayK8		PAin(11)
#define relayK9		PAin(12)

typedef struct{

	uint8_t conArryDats[2];
}RelayCon_datsType;

extern osThreadId tid_RelayCon_Thread;

void RelayCon_Init(void);
void RelayCon_Thread(const void *argument);
void RelayConThread_Active(void);

#endif
