#ifndef __SIGDIGIDETECT_H_
#define __SIGDIGIDETECT_H_

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

typedef struct{

	uint16_t anaVAL;
}sigAna_Det_datsType;

extern osThreadId tid_sigAna_Det_Thread;

void sigAna_Det_Init(void);
void sigAna_Det_Thread(const void *argument);
void sigAna_DetThread_Active(void);

#endif

