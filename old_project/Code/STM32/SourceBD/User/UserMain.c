#include "System_init.h"
#include "USART_Communication.h"

int main (void) 
{
		System_init();//启动系统初始化函数
		USART_Communication_485();	//485通讯处理函数
}
