/******************************************************************************
 * File           : Main program - DMA RAM to USART example
 *****************************************************************************/
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include "helper.h"

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------
uint8_t TxBuffer[1024];

// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void init_DMA(void);

// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------
int main(void)
{
  uint32_t i;

  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  // Initialize User Button on STM32F0-Discovery
  STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  USART_Setup();

  while(1)
  {
    // Delay ~1 sec.
    Delay(SystemCoreClock/8);
    
    // Set printable character in TxBuffer
    for(i=0; i<1024; i++){ TxBuffer[i] = '.'; }

    USART_Clearscreen();
    USART_Putstr("Final Assignment - Morse Code\n");

    // ----------------------------------------------------------------------
    //  Transfer 1 kB with DMA
    // ----------------------------------------------------------------------
    init_DMA();
		
    // Wait for the USART DMA Tx transfer to complete
    while(DMA_GetFlagStatus(DMA1_FLAG_TC2) == SET){;}
    
    // Delay ~0.04 sec.
    //Delay(SystemCoreClock/8/25);

    // At this moment in time:
    // - In 0.04 seconds, 0.04 * 115200 = 4608 bits can be transmitted at most
    // - Each transmitted byte has 1 start bit, 8 databits and 1 stopbit
    // - So 4608 / 10 = ~460 bytes are already transmitted
    
    // Change Transmit buffer while DMA transfer is busy
    TxBuffer[200]  = 'X'; // This change will not be transmitted!
    TxBuffer[400]  = 'X'; // This change will not be transmitted!
    TxBuffer[600]  = 'X';
    TxBuffer[800]  = 'X';
    TxBuffer[1000] = 'X';
  }
}

void init_DMA(void)
{	
	DMA_InitTypeDef DMA_InitStructure;
	
	// Enable DMA1 peripheral
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    
   // De-initialize DMA1 Channel 2
   DMA_DeInit(DMA1_Channel2);
    
   // DMA channel Tx of USART Configuration
   DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART1->TDR);
   DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t) TxBuffer;
   DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
   DMA_InitStructure.DMA_BufferSize         = 1024;
   DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
   DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
   DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
   DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
   DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
   DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
   DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; 
   DMA_Init(DMA1_Channel2, &DMA_InitStructure);
    
   // Enable USART1_Tx DMA interface
   USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
    
   // Clear the Transmission Complete flag
   USART_ClearFlag(USART1, USART_FLAG_TC);
    
   // Enable DMA1 Channel 2 (USART1_Tx)
   DMA_Cmd(DMA1_Channel2, ENABLE);
}