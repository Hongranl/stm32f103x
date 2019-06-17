#include "MotorDriver.h"

u16 TimerPeriod = 0;

/**********************************************************************
舵机控制
**********************************************************************/
void SeverPWMInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	// TIM3_CH3 - PB0;  
	
	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);


 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);		
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		

	/* TIM1 Configuration ---------------------------------------------------
	The objective is to generate 7 PWM signal at 1KHz:
	- TIM1_Period = (SystemCoreClock / 1000) - 1
	- ChannelxPulse = DutyCycle * (TIM1_Period - 1) / 100
	----------------------------------------------------------------------- */
	/* Compute the value to be set in ARR regiter to generate signal frequency at 1 Khz */
	  TimerPeriod = (SystemCoreClock / 10000 ) - 1;

	  /* Compute CCR2 value to generate a duty cycle at 37.5%  for channel 2 and 2N */
	  //Channel2Pulse = (uint16_t) (((uint32_t) 50 * (Timer1Period - 1)) / 100);
	  /* Compute CCR4 value to generate a duty cycle at 12.5%  for channel 4 */
	 // Channel4Pulse = (uint16_t) (((uint32_t) 10 * (Timer1Period- 1)) / 100);

	/* Time Base configuration */
	TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
	TIM_TimeBaseStructure.TIM_Prescaler = 199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

	/* Channel 1, 2,3 and 4 Configuration in PWM mode */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;

	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);

	TIM_Cmd(TIM3, ENABLE);
	SetSever_PWMDuty(75);
}

// 设置舵机 PWM 占空比
void SetSever_PWMDuty(u16 Pulse)
{
	u16 ChannelPulse;

	if(Pulse<25)Pulse = 25;
	else if(Pulse>125)Pulse = 125;
	ChannelPulse = (u16) (((u32) Pulse * (TimerPeriod - 1)) / 1000);
	TIM_SetCompare3(TIM3,ChannelPulse);
}


/**********************************************************************
电机控制
**********************************************************************/
void Motor_Init(void)
{
	//PA8    CH1
	//PA11   CH4
	
	TIM_TimeBaseInitTypeDef  	TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  	TIM_OCInitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE); 
	
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	TIM_TimeBaseStructure.TIM_Period = 999; //999
	TIM_TimeBaseStructure.TIM_Prescaler = 71; //71
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x0;
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 

	TIM_OC1Init(TIM1, &TIM_OCInitStructure);  	
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

	TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);  	
	TIM_OC4PreloadConfig(TIM1, TIM_OCPreload_Enable); 

 	TIM_ARRPreloadConfig(TIM1, ENABLE);               
	TIM_Cmd(TIM1, ENABLE);  	
	TIM_CtrlPWMOutputs(TIM1, ENABLE);	
	
	SetMotorF_PWMDuty(999);  

}

// 设置Motor Front PWM 占空比
void SetMotorF_PWMDuty(u16 Pulse)
{
	TIM_SetCompare1(TIM1,Pulse);
	TIM_SetCompare4(TIM1,0);
}

// 设置Motor Back PWM 占空比
void SetMotorB_PWMDuty(u16 Pulse)
{
	TIM_SetCompare4(TIM1,Pulse);
	TIM_SetCompare1(TIM1,0);
}

