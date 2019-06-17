#include "timer.h"
#include "led.h"
#include "key.h"
#include "usart_cfg.h"
#include "stm32f10x_tim.h"
 
 
 
 
//通用定时器3中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM2_Int_Init(u16 arr,u16 psc)
{
  	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	NVIC_InitTypeDef           NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器


	TIM_Cmd(TIM2, ENABLE);  //使能TIMx					 
}


 
void  ENTER_critical_offtim2(void)
{ 

	TIM_Cmd(TIM2, DISABLE); 

}

void  EXTI_critical_offtim2(void)
{ 

	TIM_Cmd(TIM2, ENABLE); 

}



//定时器3中断服务程序
 
void TIM2_IRQHandler(void)   //TIM3中断
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM3更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //清除TIMx更新中断标志 
 
		RS485_V_I_timeout_fution();  //1ms  ONCE 
		Rs485_COMM_uart_timeout_fution();

		alarm_time++;
		SystemCnt++;									//系统计时++
		if(BeepCnt>=SystemCnt)
		{
			if(SystemCnt%2==0)	BEEP_VALUE() = 1;
			else				BEEP_VALUE() = 0;
		}

		if(SystemCnt%2000==0)	TimeOutFlag=TRUE;		//2s到了
		 (send_urt_time==0)?(send_urt_time=send_urt_time):(send_urt_time--);
	}
}

/***************************************************************************************************************
*函数名：		TIM6_Init
*描述：		TIM6初始化——用于系统时间计时_1ms
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
//void TIM6_Init(void)
//{
//	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;	
//	NVIC_InitTypeDef NVIC_InitStruct;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
//	
//	//定时器TIM6初始化
//	TIM_BaseInitStruct.TIM_Prescaler = 72-1;//预分频值	72分频，频率为1MHz
//	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
//	TIM_BaseInitStruct.TIM_Period = 1000-1;//自动重装载值 溢出时间为 1ms(1KHz) 
//	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频因子 = 1;不分频
//	TIM_TimeBaseInit(TIM6,&TIM_BaseInitStruct);

//	//中断分组
//	NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;//中断名
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;//抢占优先级
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;//响应优先级
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
//	
//	//清除中断标志位
//	TIM_ClearFlag(TIM6,TIM_FLAG_Update);
//	//TIM1中断设置
//	TIM_ITConfig(TIM6,TIM_IT_Update, ENABLE);
//	//向上溢出中断
//	
//	/*4、开启定时器*/
//	TIM_Cmd(TIM6,ENABLE);//使能TIM6
//}

/***************************************************************************************************************
*函数名：		TIM1_Init
*描述：		TIM1初始化——用作HC_SR04测距模块的DATA引脚，输入捕获，需同时调用HC_SR04Gpio_Init()函数
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
void TIM1_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	TIM_ICInitTypeDef TIM_ICInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);
	//TIM1时钟使能
	
	TIM_DeInit(TIM1);//重设为缺省值
		
	TIM_BaseInitStruct.TIM_Prescaler = 72-1;//预分频值	72分频，频率为1MHz
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_BaseInitStruct.TIM_Period = 1000-1;//自动重装载值 溢出时间为 1ms(1KHz) 
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频因子 = 1;不分频
	TIM_TimeBaseInit(TIM1,&TIM_BaseInitStruct);

	TIM_ICInitStruct.TIM_Channel = TIM_Channel_4;//CH4
	TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;//上升沿捕获
	TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI;//对应IC4
	TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;//每个边沿执行一次
	TIM_ICInitStruct.TIM_ICFilter = 0x00;//不滤波
	TIM_ICInit(TIM1, &TIM_ICInitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn;//中断名
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn;//中断名
	NVIC_Init(&NVIC_InitStruct);
	
	//清除中断标志位
	TIM_ClearFlag(TIM1,TIM_IT_CC4);
	TIM_ClearFlag(TIM1,TIM_IT_Update);
	//TIM1中断设置
	TIM_ITConfig(TIM1,TIM_IT_CC4,ENABLE);
	TIM_ITConfig(TIM1,TIM_IT_Update,ENABLE);
	//向上溢出中断和比较中断
	
	/*4、开启定时器*/
	TIM_Cmd(TIM1,DISABLE);//失能TIM1
}


