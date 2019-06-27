/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     Main
 * @Author:     RanHongLiang
 * @Date:       2019-06-26 10:11:40
 * @Description: 
 *————智能实训台底板程序
 *	使用CMSIS-RTOS，一共有三个大线程：
 *	第一个是wirelessThread，用于无线传输线程，用作传输和接收数据。
 * 	第二个是LCD144Disp_Thread，用于1.44寸LCD显示屏，显示扩展板类型和ID，无线扩展板类型和ID，以及部分数据。
 *	第三个MoudleDEC_Thread，用于扩展板的切换线程。
 *	注：keyMboard_Thread和tipsLED_Thread是用于按键线程以及LED和蜂鸣器线程。
 *---------------------------------------------------------------------------*/
#define osObjectsPublic   // define objects in main module
#include "includes.h"//文件内包括用于内核激活，以及Bsp初始化和不可中断进程启动；

/*---------------------------------------------------------------------------
 *
 * @Description:主函数，整个程序的入口。
 * @Param:      void*,用于外部传参
 * @Return:     int 用于线程退出返回
 *---------------------------------------------------------------------------*/
int main (void const *argument)
{
	
	osKernelInitialize ();                    	// initialize CMSIS-RTOS
	
	// initialize peripherals here

	// create 'thread' functions that start executing,
	// example: tid_name = osThreadCreate (osThread(name), NULL);
	
	BSP_Init();////文件内包括延时函数初始化及Debug串口输出初始化；
	
	wirelessThread_Active(1);
	
	MoudleDEC_Init();//模块检测进程
	
	LCD144Disp_Active();//1.44寸屏显示驱动进程函数；
	
	keyMboardActive();//底板按键检测进程函数及其按键回调函数；
	
	tipsLEDActive();//led提示灯及无源蜂鸣器的驱动函数；

	osKernelStart ();                         	// start thread execution 
}
