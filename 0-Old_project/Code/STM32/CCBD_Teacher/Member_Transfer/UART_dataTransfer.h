#ifndef __DATATRANSFER_H_
#define __DATATRANSFER_H_

#include "Driver_USART.h"
#include "IO_Map.h"
#include "delay.h"

#include "stdio.h"
#include "string.h"

#define con485			PAout(12)

#define	comToAndroid	Driver_USART1
#define	comToDev485		Driver_USART2
#define	comToRFID		Driver_USART3
#define	comToZigbee		Driver_USART5

#define datsAndroidTX	Driver_USART1.Send
#define datsAndroidRX	Driver_USART1.Receive
#define datsDev485TX	Driver_USART2.Send
#define datsDev485RX	Driver_USART2.Receive
#define datsRFIDTX		Driver_USART3.Send
#define datsRFIDRX		Driver_USART3.Receive
#define datsZigbeeTX	Driver_USART5.Send
#define datsZigbeeRX	Driver_USART5.Receive

#define	FRAME_SHELLSIZE	 10

#define	FRAME_SIZEAndrod	 100
#define	FRAME_SIZEDev485	 100
#define	FRAME_SIZERFID		 100
#define	FRAME_SIZEZigbee	 100

#define	COREDATS_SIZEAndrod	 FRAME_SIZEAndrod - FRAME_SHELLSIZE
#define	COREDATS_SIZEDev485	 FRAME_SIZEDev485 - FRAME_SHELLSIZE
#define	COREDATS_SIZERFID	 FRAME_SIZERFID	  - FRAME_SHELLSIZE
#define	COREDATS_SIZEZigbee	 FRAME_SIZEZigbee - FRAME_SHELLSIZE

#define comObj_Androd	0xA1
#define comObj_Dev485	0xA2
#define comObj_DevRFID	0xA3
#define comObj_Zigbee	0xA4

void 	USART1_toAndroid_Init(void);
void 	USART2_toDev485_Init(void);
void 	USART3_toRFID_Init(void);
void 	USART5_toZigbee_Init(void);

void com1ToAndroid_Thread	(const void *argument);
void com2ToDev485_Thread	(const void *argument);
void com3ToRFID_Thread		(const void *argument);
void com5ToZigbee_Thread	(const void *argument);

void communicationActive(uint8_t comObj);
#endif

