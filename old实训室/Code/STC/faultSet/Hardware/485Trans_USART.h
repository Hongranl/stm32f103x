#ifndef _485TRANS_USART__
#define _485TRANS_USART__

#include "string.h"
#include "stdio.h"

#include "config.h"
#include "USART.h"
#include "GPIO.h"
#include "delay.h"

#define	FRAME_HEAD		0x7E
#define	COM_ADDR			0x1B

#define	UART485_BUF		RX2_Buffer
#define	COREDATS_SIZE	16

#define	DIR485	P12
#define	RD485		P12 = 0
#define	WR485		P12 = 1

extern u8 COREDATS[COREDATS_SIZE];

void USART485Trans_Init(void);
void frameInsAccept(void);

#endif