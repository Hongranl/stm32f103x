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
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
