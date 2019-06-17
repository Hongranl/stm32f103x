/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 

#include "stm32f10x_exti.h"// fbd add
#include "drv_interrupt.h"// fbd add


volatile bool uDataReady 							= false;
volatile bool RF_DataExpected 					= false;
volatile bool RF_DataReady 						= false;
volatile bool screenRotated 						= false;
volatile bool disableRotate						= false;

volatile uint32_t nb_ms_elapsed = 0;

extern __IO uint32_t 					SELStatus;
extern bool 									KEYPress;
extern uint8_t 								KEYPressed;
extern bool 									lockKEYUpDown;
extern bool 									refreshMenu;
extern bool										reversed;

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}




void RFTRANS_95HF_IRQ_HANDLER ( void )
{
	if(EXTI_GetITStatus(EXTI_RFTRANS_95HF_LINE) != RESET)
	{
		/* Clear IT flag */
		EXTI_ClearITPendingBit(EXTI_RFTRANS_95HF_LINE);
		/* Disable EXTI Line9 IRQ */
		EXTI->IMR &= ~EXTI_RFTRANS_95HF_LINE;
		
		if(RF_DataExpected)			
			RF_DataReady = true;
		
		/* Answer to command ready*/
		uDataReady = true;		
	}
}

/***************************************************************************************************************
*函数名：	TIM1_CC_IRQHandler
*描述：		定时器1捕获中断服务函数——测距出入引脚ECHO捕获输入
*作者：		hongxiang.liu
*参数：		无
*返回值：	无
*编写日期：	2019.3.25 10:33
*版本记录：	
****************************************************************************************************************/
void TIM1_CC_IRQHandler(void)
{
	if(HcSr04StartF == TRUE)								//开启了测距发送
	{
		if((Tim1CaptureState&0x80) == 0)						//最高位为0，没有完成一次输入捕获
		{
			if(TIM_GetITStatus(TIM1, TIM_IT_CC4) != RESET)	//输入捕获中断
			{
				TIM_ClearITPendingBit(TIM1,TIM_IT_CC4);				
				//清理中断

				if((Tim1CaptureState&0x40) == 0)			//本次输入捕获中第一次捕获到上升沿
				{
					Tim1CaptureState = 0;				//标志改变，开始计时
					Tim1CaptureCount = 0;					//计数值清零
					TIM_SetCounter(TIM1,0); 				//计数器清零
					Tim1CaptureState |=0X40;
					TIM_OC4PolarityConfig(TIM1,TIM_ICPolarity_Falling);//设置为下降沿捕获
				}
				else 
				if(Tim1CaptureState&0x40)				//非第一次捕获到渐变沿，说明此次是下降沿，计算时间
				{
					Tim1CaptureState |= 0x80;				//最高位设置为1，标志已完成一次完整的输入捕获
					Tim1CaptureCount = TIM_GetCapture4(TIM1);//取得当前计数值
					HcSr04StartF = FALSE;					//开启标志清0
					//TIM_GetCapture1(TIM1);					//获取当前计数器中的数值,计算总时间需要在main函数中,再增加溢出次数时间
					TIM_OC1PolarityConfig(TIM1,TIM_ICPolarity_Rising);//设置为上升沿捕获，为下一次捕获做准备
				}
			}
		}
	}
	//清理中断
	TIM_ClearITPendingBit(TIM1,TIM_IT_CC4);
}

/***************************************************************************************************************
*函数名：	TIM1_IRQHandler
*描述：		定时器1溢出中断服务函数——测距出入引脚ECHO捕获输入
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*编写日期：	2019.3.25 10:33
*版本记录：	
****************************************************************************************************************/
void TIM1_UP_IRQHandler(void)
{
	if(HcSr04StartF == TRUE)							//开启了测距发送
	{
		if(TIM_GetITStatus(TIM1, TIM_IT_Update) != RESET)//溢出中断
		{
			TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
			//清理中断
			if(Tim1CaptureState&0x40)					//已经捕获到上升沿
			{
				if((Tim1CaptureState&0x3F) == 0x3F)		//已经溢出过0x3F次，强制结束
				{
					Tim1CaptureState |= 0x80;			//最高位置1						
					HcSr04StartF = FALSE;				//开启标志清0
					Tim1CaptureCount = 0xFFFF;			//计数值为最大
				}
				else
				{
					Tim1CaptureState++; 				//如果溢出次数没有超出标志位的低六位所能表示的范围，那么就让标志位直接加1，以此来记录溢出的次数，溢出次数一直加下去，啥时候超出了标志位的低六位所能表示的范围，那么就强制退出检测，直接认为此次检测高电平已经结束，然后直接开启下一次检测
				}
			}
		}
	}
	//清理中断
	TIM_ClearITPendingBit(TIM1,TIM_IT_Update);
}

/***************************************************************************************************************
*函数名：		TIM3_IRQHandler
*描述：		定时器3中断服务函数——翻转PWM风扇IO
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*编写日期：	2019.3.25
*版本记录：	
****************************************************************************************************************/
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)
	{		
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
		if(PwmPowerFlag == TRUE)
		{	
			FAN_PWM_VALUE = 1;
		}
	}

	if(TIM_GetITStatus(TIM3,TIM_IT_CC2) != RESET)
	{
		TIM_ClearFlag(TIM3,TIM_IT_CC2);

		FAN_PWM_VALUE = 0;
	}
}

/***************************************************************************************************************
*函数名：		TIM6_IRQHandler
*描述：		定时器6中断服务函数——系统时间计数值增加(1ms)
*作者：		hongxiang.liu
*参数：		无
*返回值：		无
*完成日期：	2019.3.25
*版本记录：	
****************************************************************************************************************/
//void TIM6_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
//	{
//		TIM_ClearFlag(TIM6,TIM_FLAG_Update);
//		SystemCnt++;			
//	}
//}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
