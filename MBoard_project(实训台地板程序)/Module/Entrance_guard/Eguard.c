/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     Eguard
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 22:00:15
 * @Description: 
 *————门禁系统各进程，进程初始化，消息队列初始化，数据结构内存池初始化
 *---------------------------------------------------------------------------*/

#include "Eguard.h"
#include "debugUart.h"
//门禁系统各进程，进程初始化，消息队列初始化，数据结构内存池初始化函数；
extern ARM_DRIVER_USART Driver_USART2;

extern osThreadId tid_fingerID_Thread;
extern osThreadId tid_rfID_Thread;
extern osThreadId tid_kBoard_Thread;
extern osThreadId tid_doorLock_Thread;
					 
osPoolId  EGUD_pool;								 
osPoolDef(EGUD_pool, 30, EGUARD_MEAS);                   // 内存池定义
osMessageQId  MsgBox_EGUD;		
osMessageQDef(MsgBox_EGUD, 5, &EGUARD_MEAS);             // 消息队列定义
osMessageQId  MsgBox_MTEGUD_FID;		
osMessageQDef(MsgBox_MTEGUD_FID, 5, &EGUARD_MEAS);       // 消息队列定义
osMessageQId  MsgBox_MTEGUD_DLOCK;		
osMessageQDef(MsgBox_MTEGUD_DLOCK, 5, &EGUARD_MEAS);     // 消息队列定义
osMessageQId  MsgBox_DPEGUD;		
osMessageQDef(MsgBox_DPEGUD, 10, &EGUARD_MEAS);          // 消息队列定义
/**************************************
* @description: 线程创建接口函数，电磁门、RFID-card、指纹、按键线程创建接口
* @param：
* @return: 
************************************/
void Eguard_Active(void){
	
	static bool memInit_flg = false;
	
	if(!memInit_flg){

		EGUD_pool 			= osPoolCreate(osPool(EGUD_pool));	//创建内存池
		MsgBox_EGUD 		= osMessageCreate(osMessageQ(MsgBox_EGUD), NULL);	//创建消息队列
		MsgBox_MTEGUD_FID 	= osMessageCreate(osMessageQ(MsgBox_MTEGUD_FID), NULL);	//创建消息队列
		MsgBox_MTEGUD_DLOCK = osMessageCreate(osMessageQ(MsgBox_MTEGUD_DLOCK), NULL);	//创建消息队列
		MsgBox_DPEGUD 		= osMessageCreate(osMessageQ(MsgBox_DPEGUD), NULL);	//创建消息队列
		memInit_flg = true;
	}
	
	fingerIDThread_Active();
	rfIDThread_Active();
	kBoardThread_Active();
	doorLockThread_Active();
}
/**************************************
* @description: 线程统一关闭接口
* @param：
* @return: 
************************************/
void Eguard_Terminate(void){

	osThreadTerminate(tid_fingerID_Thread);
	osThreadTerminate(tid_rfID_Thread);
	osThreadTerminate(tid_kBoard_Thread);
	osThreadTerminate(tid_doorLock_Thread);
}

