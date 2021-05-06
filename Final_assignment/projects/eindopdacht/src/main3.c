/******************************************************************************
 * File           : Main program - DMA Flash to RAM example
 *****************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "helper.h"

#define BUFFER_SIZE (100)

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
// Global flag to indicate end of DMA transfer. This flag is set in the interrupt
// handler and tested in the main loop.
// 0 = reset
// 1 = set
extern volatile uint32_t DMA_EndOfTransfer; 

// Initialize the flash buffer. Declaring it ‘const’ makes the compiler place the
// code in Flash memory. Start the debugger to see the exact address.
const uint32_t FlashBuffer[BUFFER_SIZE] = {0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9,
                                           0,1,2,3,4,5,6,7,8,9};

// Zero initialize the entire RAM buffer.
uint32_t RamBuffer[BUFFER_SIZE] = {0};

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
  uint32_t i;
  char str[10];
  
  DMA_InitTypeDef  DMA_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;


  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  // Initialize User Button on STM32F0-Discovery
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  USART_Setup();
  USART_Clearscreen();
  USART_Putstr("Workshop 6b: DMA - Flash to RAM\n");
  USART_Putstr("Transfer 100 words (32-bit)\n");
  
  // ----------------------------------------------------------------------
  //  Transfer 100 bytes with DMA
  // ----------------------------------------------------------------------
  // Enable DMA1 peripheral
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
  
  // De-initialize DMA1 Channel 1  
  DMA_DeInit(DMA1_Channel1);
  
  // DMA channel Configuration
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)FlashBuffer;
  DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)RamBuffer;
  DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
  DMA_InitStructure.DMA_BufferSize         = BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
  DMA_InitStructure.DMA_M2M                = DMA_M2M_Enable; 
  DMA_Init(DMA1_Channel1, &DMA_InitStructure);
  
  // Enable DMA1 Channel1 Transfer Complete interrupt
  DMA_ITConfig(DMA1_Channel1, DMA_IT_TC, ENABLE);
  
  // Enable DMA1 channel1 IRQ Channel
  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);    

  // Enable DMA1 Channel 1
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  
  // Wait for the end of transmission
  while(DMA_EndOfTransfer == 0){;}
  
  // Compare the two buffers
  for(i=0; i<BUFFER_SIZE; i++)
  {
    if(RamBuffer[i] != FlashBuffer[i])
    {
      USART_itoa(i, str);
      USART_Putstr("Error: Different data at index i=");
      USART_Putstr(str);
      USART_Putstr("\n");
    }
  }

  // Display ready
  USART_Putstr("Done\n");
  
  while(1); // Do nothing
}
