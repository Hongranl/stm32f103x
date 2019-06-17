#ifndef __DATATRANSFER_H_
#define __DATATRANSFER_H_

#include "IO_Map.h"
#include "delay.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"

#define SBD_LocalAddr_main	0x0A
#define SBO_LocalAddr_ext	0x01

#define datsZigbeeTX	Driver_USART1.Send
#define datsZigbeeRX	Driver_USART1.Receive
#define datsDev485TX	Driver_USART3.Send
#define datsDev485RX	Driver_USART3.Receive

#define DTZIGB_FRAME_HEAD	0xFE
#define DT485_FRAME_HEAD	0x85

#define con485			PBout(5)

#define comObj_Dev485	0xA2
#define comObj_Zigbee	0xA4

extern ARM_DRIVER_USART Driver_USART1;
extern ARM_DRIVER_USART Driver_USART3;

extern osThreadId tid_com1ToZigbee_Thread;
extern osThreadId tid_com3ToDev485_Thread;

void com1ToZigbee_Thread(const void *argument);
void com3ToDev485_Thread(const void *argument);

void communicationActive(uint8_t comObj);

#endif

