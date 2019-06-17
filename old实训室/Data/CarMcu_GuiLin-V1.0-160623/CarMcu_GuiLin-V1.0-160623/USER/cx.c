#include "stm32f10x.h"
#include "includes.h"
#include "usart.h"	  
#include "delay.h"

extern u8 uart1Rx_timerout;
extern u16 DisableTrafficTimer;

u8 Traffic_red;
u8 TrafficOverTimer;
u8 TrafficCnt;
u16 speedOvertimer;
u16 discRanging;
u16 disValue;
u16 curSpeed;

void TrafficDetect(void);

/********************************************************************/
// 系统定时器
/********************************************************************/
void TIM3_Configuration(u16 arr,u16 psc)
{
    	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能

	TIM_DeInit(TIM3);

	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; 						//自动重装载寄存器周期的值(定时时间)累计1个频率后产生个更新或者中断
	TIM_TimeBaseStructure.TIM_Prescaler =psc; 					//时钟预分频数 例如：时钟频率=72/(时钟预分频+1) 
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		//采样分频
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx					 
}

void TIM3_IRQHandler(void)   //TIM3中断 5 MS
{
 	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)  
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update  ); 
		if(uart1Rx_timerout)uart1Rx_timerout--;	
		TrafficDetect();	
	}
}


/********************************************************************/
// 车速检测
/********************************************************************/
void Tim2_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 65500;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	//TIM_Cmd(TIM2, DISABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void SpeedDet_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;	
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA,GPIO_PinSource7);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;				 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 
	GPIO_Init(GPIOA, &GPIO_InitStructure);		
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line7;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
			 						 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		 			
	NVIC_Init(&NVIC_InitStructure);	

	Tim2_Init();
}


void EXTI9_5_IRQHandler(void)
{
	u16 speedCnt;
	
	OSIntEnter();
	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7))
    	{
        	TIM_SetCounter(TIM2,0);
		speedOvertimer = 0;
    	}	
    	else
    	{
        	speedCnt = TIM_GetCounter(TIM2);
		if(speedCnt)   curSpeed = 100000/speedCnt*3;		
		speedOvertimer = 0;
    	}   
	
    	EXTI_ClearITPendingBit(EXTI_Line7); 
    	OSIntExit();
} 

/********************************************************************/
// 红绿灯检测
/********************************************************************/
void Tim4_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 65500;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

	TIM_Cmd(TIM4, ENABLE);
}

void Traffic_PA1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);
												
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	Tim4_Init();
	
}

void TrafficDetect(void)
{
	if(DisableTrafficTimer)DisableTrafficTimer--;
	
	TrafficOverTimer++;
	if(TrafficOverTimer > (500/5))	// 300ms 检测不到红灯
	{
		TrafficOverTimer = 0;
		TrafficCnt = 0;
		Traffic_red = 0;
	}
}

void EXTI1_IRQHandler(void)
{
	u16 count1;
	
	OSIntEnter();
	
	if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1))
	{
	     TIM_SetCounter(TIM4,0);
	}	
	else
	{
		count1 = TIM_GetCounter(TIM4);
		
		if(count1 <= 1200 && count1 >= 800 ) 	// 红灯
		{
			TrafficCnt++;
			if(TrafficCnt>10)
			{
				TrafficCnt = 0;
				TrafficOverTimer = 0;
				Traffic_red = 1; 
			}
		}
		
		/*
		else if(count1 <= 3500 && count1 > 1500)   // 绿灯
		{  
			TrafficCnt++;
			if(TrafficCnt>10)
			{
				TrafficCnt = 0;
				Traffic_red = 0; 
				TrafficOverTimer = 0;
			}
		}
		*/
	}
	
   	EXTI_ClearITPendingBit(EXTI_Line1); 	
    	OSIntExit();
} 

/********************************************************************/
// 超声波检测
/********************************************************************/

void Tim5_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 65000;
	TIM_TimeBaseStructure.TIM_Prescaler = 71;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);

	TIM_Cmd(TIM5, ENABLE);
}

void Ultra_PC0_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO,ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;       
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOC,GPIO_Pin_1);
										
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;       
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0); 
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	Tim5_Init();
}

// 超声波中断函数
void EXTI0_IRQHandler(void)
{
	OSIntEnter();
	
	if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_0))
	{
	     TIM_SetCounter(TIM5,0);
	}	
	else
	{
		disValue = TIM_GetCounter(TIM5)/6;
	}
	
	EXTI_ClearITPendingBit(EXTI_Line0); 
   	OSIntExit();
}

void Ultra_Ranging(void)
{
	u8 i;
	u32 Ultr_Temp = 0;	

	disValue = 0;
	for(i=0;i<5;i++)
	{
		GPIO_SetBits(GPIOC,GPIO_Pin_1);	
		delay_us(10);
		GPIO_ResetBits(GPIOC,GPIO_Pin_1);
		OSTimeDlyHMSM(0,0,0,60);  
		Ultr_Temp += disValue;
	}
	discRanging = Ultr_Temp/5;
}





