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
	 
		rand_seed++;
		//真的随机
		(ACCIDENTAL_time[0]==0)?(ACCIDENTAL_time[0]=ACCIDENTAL_time[0]):(ACCIDENTAL_time[0]--);
		(ACCIDENTAL_time[1]==0)?(ACCIDENTAL_time[1]=ACCIDENTAL_time[1]):(ACCIDENTAL_time[1]--);
		(ACCIDENTAL_time[2]==0)?(ACCIDENTAL_time[2]=ACCIDENTAL_time[2]):(ACCIDENTAL_time[2]--);
		(ACCIDENTAL_time[3]==0)?(ACCIDENTAL_time[3]=ACCIDENTAL_time[3]):(ACCIDENTAL_time[3]--);			
		(ACCIDENTAL_time[4]==0)?(ACCIDENTAL_time[4]=ACCIDENTAL_time[4]):(ACCIDENTAL_time[4]--);
		(ACCIDENTAL_time[5]==0)?(ACCIDENTAL_time[5]=ACCIDENTAL_time[5]):(ACCIDENTAL_time[5]--);
		(ACCIDENTAL_time[6]==0)?(ACCIDENTAL_time[6]=ACCIDENTAL_time[6]):(ACCIDENTAL_time[6]--);
		(ACCIDENTAL_time[7]==0)?(ACCIDENTAL_time[7]=ACCIDENTAL_time[7]):(ACCIDENTAL_time[7]--);
		(ACCIDENTAL_time[8]==0)?(ACCIDENTAL_time[8]=ACCIDENTAL_time[8]):(ACCIDENTAL_time[8]--);
		(ACCIDENTAL_time[9]==0)?(ACCIDENTAL_time[9]=ACCIDENTAL_time[9]):(ACCIDENTAL_time[9]--);
		(ACCIDENTAL_time[10]==0)?(ACCIDENTAL_time[10]=ACCIDENTAL_time[10]):(ACCIDENTAL_time[10]--);
		(ACCIDENTAL_time[11]==0)?(ACCIDENTAL_time[11]=ACCIDENTAL_time[11]):(ACCIDENTAL_time[11]--);
		(ACCIDENTAL_time[12]==0)?(ACCIDENTAL_time[12]=ACCIDENTAL_time[12]):(ACCIDENTAL_time[12]--);
		(ACCIDENTAL_time[13]==0)?(ACCIDENTAL_time[13]=ACCIDENTAL_time[13]):(ACCIDENTAL_time[13]--);			
		(ACCIDENTAL_time[14]==0)?(ACCIDENTAL_time[14]=ACCIDENTAL_time[14]):(ACCIDENTAL_time[14]--);
		(ACCIDENTAL_time[15]==0)?(ACCIDENTAL_time[15]=ACCIDENTAL_time[15]):(ACCIDENTAL_time[15]--);

		(send_urt_time==0)?(send_urt_time=send_urt_time):(send_urt_time--);

		alarm_time++;
	}
}





