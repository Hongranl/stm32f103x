#ifndef	_MOUDLE_DEC_H_
#define	_MOUDLE_DEC_H_

#include "IO_Map.h"
#include "stm32f10x.h"

#include "stdio.h"
#include "string.h"
#include "Driver_USART.h"
#include "delay.h"

#include "infraTrans.h"
#include "Eguard.h"

#include "LCD_1.44.h"

#include "fireMS.h"
#include "gasMS.h"
#include "lightMS.h"
#include "pyroMS.h"
#include "analogMS.h"
#include "smokeMS.h"
#include "tempMS.h"
#include "shakeMS.h"


#include "pwmCM.h"
#include "curtainCM.h"
#include "sourceCM.h"
#include "speakCM.h"
#include "RelaysCM.h"
#include "RelayslightCM.h"
#include "RelaysoutletCM.h"



#include "debugUART.h"
#include <dataTrans_USART.h>

#define		MID_TRANS_Wifi		0x02
#define 	MID_TRANS_Zigbee	0x01
	
#define		MID_TRANS_Wifi		0x02//WIFI模块地址
#define 	MID_TRANS_Zigbee	0x01//Zigbee模块地址
//	


#define		MID_EXEC_SPEAK			0x41//语音模块
#define 	MID_EXEC_RELAYS			0x42//两路继电器模块
#define 	MID_EXEC_RELAYSLIGHT	0x44//两路灯光模块
#define		MID_EXEC_CURTAIN		0x45//窗帘模块
#define		MID_EXEC_DEVPWM			0x48//PWM灯光控制模块
#define		MID_EXEC_DEVPWM_FAN		0x49//PWM风扇控制模块
#define 	MID_EXEC_RELAYSOUTLET	0x4a//一路继电器模块（智能插座）


#define 	MID_EXEC_DEVIFR		0x81//红外学习模块
#define 	MID_EXEC_SOURCE		0x80//电源模块



#define 	MID_SENSOR_FIRE		0x01//火焰传感器
#define 	MID_SENSOR_PYRO		0x02//人体红外传感器
#define 	MID_SENSOR_SMOKE	0x03//粉尘传感器
#define 	MID_SENSOR_GAS		0x04//燃气传感器
#define 	MID_SENSOR_TEMP		0x05//温湿度传感器
#define 	MID_SENSOR_SHAKE    0x07//震动传感器
#define 	MID_SENSOR_LIGHT	0x09//光照强度传感器
#define 	MID_SENSOR_ANALOG	0x08//模拟信号采集板




#define 	MID_EGUARD			0x0A//门禁模块

#define		extension_FLG	0x80	//扩展模块是否在线标志位
#define		wireless_FLG	0x40	//无线通讯模块是否在线标志位
#define		LCD4_3_FLG		0x20	//显示模块是否在线标志位

#define		EVTSIG_MDEC_EN	123		//模块地址检测进程使能信号

#define		extension_IF	(!PDin(0))
#define		wireless_IF		(!PEin(5))   //无线模块在线检测
#define		LCD4_3_IF		(!PEin(4))

typedef struct M_attr{

	u8 Extension_ID;
	u8 Wirless_ID;
	
	u8 Alive;	//bit7:扩展模块,bit6:无线通信模块,bit5:显示模块
}Moudle_attr;

extern osThreadId tid_MBDEC_Thread;
extern Moudle_attr Moudle_GTA;

void MoudleDEC_Init(void);

void MBDEC_Thread(const void *argument);

#endif
