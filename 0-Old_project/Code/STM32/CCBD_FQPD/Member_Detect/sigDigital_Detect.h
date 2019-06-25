#ifndef __SIGANALDETECT_H_
#define __SIGANALDETECT_H_

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

typedef struct{

	bool digVAL[2];
}sigDig_Det_datsType;

extern osThreadId 	 tid_sigDig_Det_Thread;

void sigDig_Det_Init(void);
void sigDig_Det_Thread(const void *argument);
void sigDig_DetThread_Active(void);

#endif

