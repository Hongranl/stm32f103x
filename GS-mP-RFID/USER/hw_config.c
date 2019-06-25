/**
  ******************************************************************************
  * @file    hw_config.c
  * @author  MMY Application Team
  * @version V2.0.0
  * @date    06/12/2012
  * @brief   Hardware Configuration & Setup of the MCU
	* @brief   GPIO used to receive 95HF interruption is declared in this file
	******************************************************************************
  * @copyright
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "hw_config.h"

/** @addtogroup User_Appli
 * 	@{
 *  @brief      <b>This folder contains the application files</b>
 */

/** @addtogroup HW_Config
 * 	@{
 * @brief      This file defines a set of command to initialize the MCU
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef USE_MSD_DRIVE
static void IntToUnicode (uint32_t value, uint8_t *pbuf, uint8_t len);
#endif /*USE_MSD_DRIVE*/

/** @addtogroup HW_Config_Private_Functions
 * 	@{
 */


/**
 *	@brief  Configures the interrupts
 *  @param  None
 *  @retval None
 */
void Interrupts_Config (void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

#ifdef SPI_INTERRUPT_MODE_ACTIVATED
    /* Enable and set RF transceiver IRQ to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel 						= EXTI_RFTRANS_95HF_IRQ_CHANNEL;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= EXTI_RFTRANS_95HF_PREEMPTION_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= EXTI_RFTRANS_95HF_SUB_PRIORITY;
    NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif /* SPI_INTERRUPT_MODE_ACTIVATED */

    /* Enable the EXTI9_5 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel 						= EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel 						= EXTI4_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority 	= 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority 			= 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd 					= ENABLE;
    NVIC_Init(&NVIC_InitStructure);

}

#ifdef USE_MSD_DRIVE

/**
 *	@brief  This function creates the serial number string descriptor.
 */
void Get_SerialNum(void)
{

    uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

    Device_Serial0 = *(__IO uint32_t*)(0x1FFFF7E8);
    Device_Serial1 = *(__IO uint32_t*)(0x1FFFF7EC);
    Device_Serial2 = *(__IO uint32_t*)(0x1FFFF7F0);

    Device_Serial0 += Device_Serial2;

    if (Device_Serial0 != 0)
    {
        IntToUnicode (Device_Serial0, &COMPOSITE_StringSerial[2], 6);
        IntToUnicode (Device_Serial1, &COMPOSITE_StringSerial[18], 6);
    }
}

/**
 *	@brief  Convert Hex 32Bits value into char.
 */
static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
    uint8_t idx = 0;

    for( idx = 0 ; idx < len ; idx ++)
    {
        if( ((value >> 28)) < 0xA )
        {
            pbuf[ 2* idx] = (value >> 28) + '0';
        }
        else
        {
            pbuf[2* idx] = (value >> 28) + 'A' - 10;
        }

        value = value << 4;

        pbuf[ 2* idx + 1] = 0;
    }
}

/**
 *	@brief  MAL_layer configuration
 *  @param  None
 *  @return None
 */
void MAL_Config(void)
{
    MAL_Init(0);
}
#endif /*USE_MSD_DRIVE*/

/**
 *	@brief 	Time delay in millisecond. The default priority are changed for this function.
 * 					This function is called by reset function to guarantee the execution time
 *  @param  delay : delay in ms.
 *  @retval none
 */
void delayHighPriority_ms(uint16_t delay)
{

	delay_ms((u32)delay);
}

/**
  * @}
  */
/**
  * @}
  */
/**
  * @}
  */



/******************* (C) COPYRIGHT 2014 STMicroelectronics *****END OF FILE****/
