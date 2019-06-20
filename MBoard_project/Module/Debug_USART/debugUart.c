/*---------------------------------------------------------------------------
 *
 * Copyright (C),2014-2019, guoshun Tech. Co., Ltd.
 *
 * @Project:    智能实训台项目
 * @Version:    V 0.2 
 * @Module:     debugUart
 * @Author:     RanHongLiang
 * @Date:       2019-06-19 21:55:03
 * @Description: 
 *	调试模块，可用于调试输出
 *	注：未使用
 *---------------------------------------------------------------------------*/

#include "debugUart.h"//调试信息串口输出进程；

const uint8_t TestCMD_PAGSIZE = 20;
const uint8_t TestCMD_MAXSIZE = 80;

const char *TestCMD[TestCMD_PAGSIZE] = {

	"公示底板按键调试信息",
	"隐藏底板按键调试信息",
	"公示红外转发扩展板按键调试信息",
	"隐藏红外转发扩展板按键调试信息",
};

const char *TestREP[TestCMD_PAGSIZE] = {

	"底板按键调试信息已公示\r\n",
	"底板按键调试信息已隐藏\r\n",
	"红外转发扩展板按键调试信息已公示\r\n",
	"红外转发扩展板按键调试信息已隐藏\r\n",
};

funDebug funDebugTab[] = {funDB_keyMB_ON,funDB_keyMB_OFF,funDB_keyIFR_ON,funDB_keyIFR_OFF};

extern ARM_DRIVER_USART Driver_USART1;

extern osThreadId tid_keyMboard_Thread;	//声明主板按键任务ID，便于传递信息调试使能信号
extern osThreadId tid_keyIFR_Thread;	//声明红外转发扩展板按键任务ID，便于传递信息调试使能信号

osThreadId tid_USARTDebug_Thread;
osThreadDef(USARTDebug_Thread,osPriorityNormal,1,1024);

void funDB_keyMB_ON(void){  // 开启主板按键Debug_log

	osSignalSet (tid_keyMboard_Thread, KEY_DEBUG_ON);
}

void funDB_keyMB_OFF(void){ // 关闭主板按键Debug_log

	osSignalSet (tid_keyMboard_Thread, KEY_DEBUG_OFF);
}

void funDB_keyIFR_ON(void){  // 开启红外转发扩展板按键Debug_log

	osSignalSet (tid_keyIFR_Thread, KEY_DEBUG_ON);
}

void funDB_keyIFR_OFF(void){ // 关闭红外转发扩展板按键Debug_log

	osSignalSet (tid_keyIFR_Thread, KEY_DEBUG_OFF);
}
/**************************************
* @description: 初始化 Driver_USART1
* @param:  void
* @return: void
************************************/
void USART1Debug_Init(void){

	/*Initialize the USART driver */
	Driver_USART1.Initialize(myUSART1_callback);
	/*Power up the USART peripheral */
	Driver_USART1.PowerControl(ARM_POWER_FULL);
	/*Configure the USART to 4800 Bits/sec */
	Driver_USART1.Control(ARM_USART_MODE_ASYNCHRONOUS |
									ARM_USART_DATA_BITS_8 |
									ARM_USART_PARITY_NONE |
									ARM_USART_STOP_BITS_1 |
							ARM_USART_FLOW_CONTROL_NONE, 115200);

	/* Enable Receiver and Transmitter lines */
	Driver_USART1.Control (ARM_USART_CONTROL_TX, 1);
	Driver_USART1.Control (ARM_USART_CONTROL_RX, 1);

	Driver_USART1.Send("i'm usart1 for debug log\r\n", 26);
}
/**************************************
* @description: 串口事件回调，发送完成、接受完成、溢出、超时事件触发
* @param:  void
* @return: void
************************************/
void myUSART1_callback(uint32_t event)
{
;
}
/**************************************
* @description:模块线程函数，未使用
* @param：创建时设定值->argument
* @return: 
************************************/
void USARTDebug_Thread(const void *argument){

	const uint8_t cmdsize = TestCMD_MAXSIZE;
	char cmd[cmdsize] = "abc";
	uint8_t loop;
	
	osSignalWait(USARTDEBUG_THREAD_EN,osWaitForever);		//等待线程使能信号
	osSignalClear(tid_USARTDebug_Thread,USARTDEBUG_THREAD_EN);	
	
	for(;;){

		osDelay(10);													//必需延时，防乱序
		Driver_USART1.Receive(cmd,TestCMD_MAXSIZE);
		
		for(loop = 0; loop < 4; loop ++){
		
			if(strstr(cmd,TestCMD[loop])){							//子串比较
			//if(!strcmp(TestCMD[loop],cmd)){						//全等比较
			
				osDelay(10);
					
				funDebugTab[loop]();
				Driver_USART1.Send((char*)(TestREP[loop]),strlen((char*)(TestREP[loop])));
				memset(cmd,0,cmdsize*sizeof(char));
			}
		}
	}
}
/**************************************
* @description:模块初始化函数 
* @param 
* @return: 
************************************/
void USART_debugInit(void){

	USART1Debug_Init();
}
/**************************************
* @description: 线程创建接口函数
* @param：
* @return: 
************************************/
void debugThread_Active(void){
	
	USART_debugInit();
	tid_USARTDebug_Thread = osThreadCreate(osThread(USARTDebug_Thread),NULL);
}

