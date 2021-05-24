/**
  ******************************************************************************
  * @file    stm32f0xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    23-March-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_it.h"
#include "STM32F0_discovery.h"
#include "helper.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
volatile uint32_t DMA_EndOfTransfer = 0;
extern uint16_t adc;
extern uint8_t playfield[1][MAX_DIGITS];
int i;
uint8_t play = 1;

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
  TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F0xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f0xx.s).                                               */
/******************************************************************************/

void DMA1_Channel1_IRQHandler(void)
{
  // Test on DMA1 Channel1 Transfer Complete interrupt
  if(DMA_GetITStatus(DMA1_IT_TC1))
  {
    // DMA1 finished the transfer of SrcBuffer
    DMA_EndOfTransfer = 1;

    // Clear DMA1 Channel1 Half Transfer, Transfer Complete and Global
    // interrupt pending bits
    DMA_ClearITPendingBit(DMA1_IT_GL1);
  }
}

char str[10];

	uint8_t verticalpos =5;
	uint8_t horizontalpos=1;
	hdirection hdir = left;
	vdirection vdir = up;

void TIM2_IRQHandler(void)
{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)			
		{
			playfield[0][verticalpos] = 0x00;
			playfield[0][0] = (3<<adc);
			moveDot(&vdir,&hdir,&verticalpos,&horizontalpos);
		
			playfield[0][verticalpos] = (1<<horizontalpos);
		
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
		}
		
		
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{		
		adc = map(ADC_GetConversionValue(ADC1));
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}

void EXTI0_1_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line0) != RESET)
  {

		
		STM_EVAL_LEDToggle(LED4);
		
		

    
    // clear pending bit
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
