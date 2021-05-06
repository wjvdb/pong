/******************************************************************************
 * File           : Main program - DMA Flash to DAC 
 *									skeleton code for exercise
 *****************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "helper.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
const uint16_t sine12bit_lut[32] = {
    2047, 2447, 2831, 3185, 3498, 3750, 3939, 4056,
    4095, 4056, 3939, 3750, 3495, 3185, 2831, 2447,
    2047, 1647, 1263,  909,  599,  344,  155,   38,
       0,   38,  155,  344,  599,  909, 1263, 1647};

//     4095 +      .                      Vdd
//          |   .     .
//          | .         .
//          |.           .
//     2047 +------+------+------+------+ Vdd/2
//          |              .           . 
//          |               .         .  
//          |                 .     .    
//        0 +                    .        GND

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void DMA_Setup(void);
void DAC_Setup(void);
void TIM_Setup(void);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  // Initialize User Button on STM32F0-Discovery
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  DMA_Setup();
  DAC_Setup();
  TIM_Setup();
  
  // Use an oscilloscope on PA4 to make the generated wave visible
  
  while(1)
  {
    Delay(SystemCoreClock/8/2);
    STM_EVAL_LEDToggle(LED3);
  }
}

/**
  * @brief  Configures DMA1 channel3
  * @param  None
  * @retval None
  */
void DMA_Setup(void)
{
	//to be implemented
}

/**
  * @brief  Configures DAC channel 1
  * @param  None
  * @retval None
  */
void DAC_Setup(void)
{
	//to be implemented
}

/**
  * @brief  The desired sine frequency is 1 kHz. 
  * @param  None
  * @retval None
  */
void TIM_Setup(void)
{
	//to be implemented
}
