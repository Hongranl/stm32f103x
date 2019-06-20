#include "key.h"
#include "sys.h" 
#include "delay.h"
#include "global.h"

/*内部变量*/
static uint64_t key1Count=0, key2Count=0;
 /*********/
/***************************************************************************************************************
*函数名：	Key_Init
*描述：		按键初始化		
*作者：		hongxiang.liu
*参数：		无
*返回值：	无
*编写日期：	2019.3.29 13:18
*版本记录：	
****************************************************************************************************************/
void Key_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStruct;

 	RCC_APB2PeriphClockCmd(KEY_1_PERIPH, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin  = KEY_1_PIN|KEY_2_PIN;
	GPIO_Init(KEY_1_PORT, &GPIO_InitStruct);

}
void dly_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStruct;
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);	

	PAout(15) = 0;
}

/***************************************************************************************************************
*函数名：		Key1Scan
*描述：		按键1扫描		
*作者：		hongxiang.liu
*参数：		无
*返回值：	bool 按键值
*编写日期：	2019.3.29 13:18
*版本记录：	
****************************************************************************************************************/
bool Key1Scan(void)
{
	static u8 key1_i=0;                  								//静态局部变量，用于计数	
	bool Key1Value = FALSE;
	
	if(KEY_1_VALUE() == KEY_VALUE_FALSE)
	{
		Key1Flag = FALSE;
	}
	if(key1_i >= 10)													//按键确实按下
	{
		key1_i = 0;
		Key1Flag = TRUE;		    									//确认按键事件有效
	}
	else 																//消抖函数
	{
		if(SystemCnt - key1Count >= 10)									//10ms后
		{
			if(KEY_1_VALUE() == KEY_VALUE_TRUE)     					//确认键值是有效状态
			{
				key1_i++;
			}
			else
			{
				key1_i=0;
			}
			key1Count = SystemCnt;										//保存计时数
		}	
	}
	Key1Value = Key1Flag;
	
	return Key1Value;
}

/***************************************************************************************************************
*函数名：		Key2Scan
*描述：		按键2扫描		
*作者：		hongxiang.liu
*参数：		无
*返回值：	bool 按键值
*编写日期：	2019.3.29 13:29
*版本记录：	
****************************************************************************************************************/
bool Key2Scan(void)
{
	static u8 key2_i=0;                  								//静态局部变量，用于计数	
	bool Key2Value = FALSE;

	if(KEY_2_VALUE() == KEY_VALUE_FALSE)
	{
		Key2Flag = FALSE;
	}
	if(key2_i >= 10)													//按键确实按下
	{
		Key2Flag = TRUE;		    									//确认按键事件有效
		key2_i = 0;
	}
	else 																//消抖函数
	{
		if(SystemCnt - key2Count >= 10)									//10ms后
		{
			if(KEY_2_VALUE() == KEY_VALUE_TRUE)     					//确认键值是有效状态
			{
				key2_i++;
			}
			else
			{
				key2_i=0;
			}
			key2Count = SystemCnt;										//保存计时数
		}	
	}
	Key2Value = Key2Flag;
	
	return Key2Value;
}

/***************************************************************************************************************
*函数名：	Beep_Init
*描述：		按键初始化		
*作者：		hongxiang.liu
*参数：		无
*返回值：	无
*编写日期：	2019.3.29 13:50
*版本记录：	
****************************************************************************************************************/
void Beep_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	RCC_APB2PeriphClockCmd(BEEP_PERIPH, ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = BEEP_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(BEEP_PORT ,&GPIO_InitStruct);

	BEEP_VALUE() = 0;
}

/***************************************************************************************************************
*函数名：	BeepOn
*描述：		开启蜂鸣器	
*作者：		hongxiang.liu
*参数：		u16 Cnt 蜂鸣时间(ms)
*返回值：	无
*编写日期：	2019.3.29 13:55
*版本记录：	
****************************************************************************************************************/
void BeepOn(u16 Cnt)
{
	BeepCnt = SystemCnt + Cnt;
	BEEP_VALUE() = 1;
}

/***************************************************************************************************************
*函数名：		CodeSwitch_Init
*描述：		8472编码开关初始化		
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*备注：		反码(低电平有效)
*完成日期：	2019.3.20
*版本记录：	
****************************************************************************************************************/
void CodeSwtich_Init(void)
{
	RCC_APB2PeriphClockCmd(TYPE_1_PERIPH, ENABLE);
	//时钟使能

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin  = TYPE_1_PIN|TYPE_2_PIN|TYPE_4_PIN|TYPE_8_PIN;
	GPIO_Init(TYPE_1_PORT, &GPIO_InitStruct);
	//初始化编码开关4个IO——上拉输入
}

/***************************************************************************************************************
*函数名：		GetSwitchType
*描述：		获取8472编码开关的键值
*作者：		hongxiang.liu
*参数：		无
*返回值：		u8 编码键值
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
u8 GetSwitchType(void)
{
	u8 value = 0;
	
	value = (!TYPE_8_VALUE)<<3|(!TYPE_4_VALUE)<<2|(!TYPE_2_VALUE)<<1|(!TYPE_1_VALUE);

	return value;
}

/***************************************************************************************************************
*函数名：	Fan_Init
*描述：		PWM调速风扇配置
*作者：		hongxiang.liu
*参数：		u16 speed 转速(0~3800)
*返回值：		无
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
void Fan_Init(u16 speed)
{
	if(speed>3800) speed=3800;		//阈值判断
	u16 TIM3Cnt = (u16)speed*5/19;
	//speed/3800 * 1000
	
	GPIO_InitTypeDef GPIO_InitStruct;

	/*1.IO口配置*/
	RCC_APB2PeriphClockCmd(FAN_PWM_PERIPH, ENABLE);
	//时钟使能
	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin  = FAN_PWM_PIN;
	GPIO_InitStruct.GPIO_Speed= GPIO_Speed_50MHz;
	GPIO_Init(FAN_PWM_PORT, &GPIO_InitStruct);
	//初始化编码开关4个IO——上拉输入

	/*2.TIM3配置*/	
	TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//TIM3时钟使能
	
	TIM_DeInit(TIM3);//重设为缺省值
	
//	//获取APB1时钟频率
//	RCC_ClocksTypeDef RCC_Clocks;
//	RCC_GetClocksFreq(&RCC_Clocks);
	
	TIM_BaseInitStruct.TIM_Prescaler = 72-1;//预分频值	72分频，频率为1MHz
	TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_BaseInitStruct.TIM_Period = 1000-1;//自动重装载值 溢出时间为 1ms(1KHz) 
	TIM_BaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;//时钟分频因子 = 1;不分频
	TIM_TimeBaseInit(TIM3,&TIM_BaseInitStruct);
	
	TIM_OCStructInit(&TIM_OCInitStruct);//设为缺省值
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_Toggle;//PWM1比较输出模式
	TIM_OCInitStruct.TIM_Pulse = TIM3Cnt;//占空比 
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;//有效电平为高电平
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;//输出状态使能
	TIM_OC2Init(TIM3,&TIM_OCInitStruct);//初始化TIM5的通道2
	TIM_ARRPreloadConfig(TIM3,ENABLE);//使能TIM5ARR上的预装载寄存器
	TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);//使能TIM5在CCR2上的预装载寄存器
	
	/*3、设置中断*/
	//中断分组
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = TIM3_IRQn;//中断名
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;//响应优先级
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	//清除中断标志位
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_IT_CC2);
	//TIM1中断设置
	TIM_ITConfig(TIM3,TIM_IT_Update, ENABLE);
	TIM_ITConfig(TIM3,TIM_IT_CC2,ENABLE);
	//向上溢出中断和比较中断
	
	/*4、开启定时器*/
	TIM_Cmd(TIM3,ENABLE);//使能TIM3
//	TIM_CtrlPWMOutputs(TIM3,ENABLE);//TIM3主输出(PWM)使能
}

/***************************************************************************************************************
*函数名：	FanPowerOn
*描述：		打开PWM调速风扇
*作者：		hongxiang.liu
*参数：		u16 speed 转速(0~3800) —— 如为0，则不改变定时器比较值
*返回值：	无
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
void FanPowerOn(u16 speed)
{
	if(speed!=0)			//如果形参为0，则不改变转速
	{
		//if(speed>3800) speed=3800;		//阈值判断
		u16 TIM3Cnt = (u16)speed;//*5/19
		//speed/3800 * 1000
		TIM3Cnt = TIM3Cnt < 999 ? TIM3Cnt : 999;
		TIM_SetCompare2(TIM3, TIM3Cnt);
	}
	PwmPowerFlag = TRUE;
	//风扇打开标志置位
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_IT_CC2);
	TIM_Cmd(TIM3,ENABLE);//使能TIM3
//	TIM_CtrlPWMOutputs(TIM3,ENABLE);//TIM3主输出(PWM)使能
}

/***************************************************************************************************************
*函数名：	ChangeFanSpeed
*描述：		PWM调速风扇转速
*作者：		hongxiang.liu
*参数：		u16 speed 转速(0~3800)
*返回值：	无
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
void ChangeFanSpeed(u16 speed)
{
	//if(speed>3800) speed=3800;		//阈值判断
	u16 TIM3Cnt = (u16)speed;//*5/19
	//speed/3800 * 1000
	TIM3Cnt = TIM3Cnt < 999 ? TIM3Cnt : 999;
	TIM_SetCompare2(TIM3, TIM3Cnt);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_IT_CC2);
}

/***************************************************************************************************************
*函数名：	FanPowerOff
*描述：		关闭PWM调速风扇
*作者：		hongxiang.liu
*参数：		无
*返回值：	无
*完成日期：	2019.3.22
*版本记录：	
****************************************************************************************************************/
void FanPowerOff(void)
{
	PwmPowerFlag = FALSE;
	//风扇打开标志清0
	
	TIM_Cmd(TIM3,DISABLE);//失能TIM3
//	TIM_CtrlPWMOutputs(TIM3,DISABLE);//TIM3主输出(PWM)使能

	FAN_PWM_OFF //注意此处为反极性输出
}

