#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "global.h"
 
 
 
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	//初始化WIFI_RESET-->GPIOD.12   上拉输入
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);//使能PORTD时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12  ; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOD 12 13

}

 
