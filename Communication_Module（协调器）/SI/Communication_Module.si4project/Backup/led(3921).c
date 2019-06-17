#include "led.h"



void IO_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD|RCC_APB2Periph_GPIOE, ENABLE);//使能PA,PD,PE端口时钟
//配置输出端口	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_11 | GPIO_Pin_12;	//端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOA, &GPIO_InitStructure);					 //根据设定参数初始化
 GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_11);						 

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 ;				 //端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化
 GPIO_ResetBits(GPIOE,GPIO_Pin_5);	

//配置输入端口

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;	    		 //端口配置, 输入
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //输入
 GPIO_Init(GPIOD, &GPIO_InitStructure);	  				 //IO口速度为50MHz
 
}

 
