/******************************************************************************
 * File           : Main program - morse code converter
 *****************************************************************************/

// ----------------------------------------------------------------------------
// Includes
// ----------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f0_discovery.h"
#include <stdint.h>
#include "helper.h"

#define BUFFER_SIZE 10
#define ALPHABET 26
#define ASCIICONVERT 96

#define DIN_PIN GPIO_Pin_7
#define CS_PIN GPIO_Pin_4
#define CLK_PIN GPIO_Pin_5

#define AMOUNT_OF_FRAMES (1)										//contains amount of frames of the whole animation
#define MAX_DIGITS (8)													//amount of rows per frame



// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void init_DMA(void);
void buffer_to_morse(void);
void read_morse(void);
void init_Timer2(void);
void init_Timer3(void);
void init_GPIO(void);
void init_main(void);
void init_ADC(void);
void init_interrupt_timer2(void);
void init_interrupt_timer3(void);
void init_Timer14(void);
void init_interrupt_timer14(void);
void GPIO_SPI_init(void);
void SPI_init(void);
void init_gpio_button(void);
void init_interrupt_button(void);
void init_nvic_button(void);
void init_button(void);

void write_matrix (uint8_t byte);
void write_frame (uint8_t address, uint8_t data);
void max_init(void);
void moveDot(vdirection *vdir,hdirection *hdir,uint8_t *ver,uint8_t *hor);

// ----------------------------------------------------------------------------
// Global variables
// ----------------------------------------------------------------------------

uint16_t adc;

uint8_t RxBuffer[BUFFER_SIZE+1] = {0};


// ----------------------------------------------------------------------------
// Initialize functions
// ----------------------------------------------------------------------------
void init_main(void)
{
	init_GPIO();
	GPIO_SPI_init();
	SPI_init();
	init_Timer2();
	init_interrupt_timer2();
	init_Timer3();
	init_interrupt_timer3();

	init_ADC();
	init_button();
  // Configure LED3 and LED4 on STM32F0-Discovery
  STM_EVAL_LEDInit(LED3);
  STM_EVAL_LEDInit(LED4);
  
  // Initialize User Button on STM32F0-Discovery
 // STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);
  
  USART_Setup();
  USART_Clearscreen();
}

void SPI_init(void)
{
	SPI_InitTypeDef SPI_structure = {0};
	
	SPI_structure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_structure.SPI_Mode = SPI_Mode_Master;
	SPI_structure.SPI_DataSize = SPI_DataSize_16b;
  SPI_structure.SPI_CPOL = SPI_CPOL_High;
	SPI_structure.SPI_CPHA = SPI_CPHA_1Edge;
  SPI_structure.SPI_NSS = SPI_NSS_Hard;
  SPI_structure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
  SPI_structure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(SPI1, &SPI_structure);
	SPI_SSOutputCmd(SPI1, ENABLE);
	SPI_NSSPulseModeCmd(SPI1, ENABLE);
}

void init_ADC(void)
{
	ADC_InitTypeDef				ADC_InitStructure;
	
	//Enable the ADC interface clock
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	
	//Configure these ADC pins in analog mode using GPIO_Init();  
	//is done in function init_GPIO()
	
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_ScanDirection = ADC_ScanDirection_Upward;
	
	ADC_Init(ADC1, &ADC_InitStructure);	
	
	//Active the Calibration operation for the selected ADC.
	ADC_GetCalibrationFactor(ADC1);
	
	//To activate the continuous mode, use the ADC_ContinuousModeCmd()
	ADC_ContinuousModeCmd(ADC1, ENABLE);
	
	//Activate the ADC peripheral using ADC_Cmd() function.
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_StartOfConversion(ADC1);

	//wait untill ADC is ready
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_ADEN) == RESET);
}

// ----------------------------------------------------------------------------
// Configure Pin C7 to alternate function 1 (TIM3)
// ----------------------------------------------------------------------------
void init_GPIO(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;  

  //(#) Configure the TIM pins by configuring the corresponding GPIO pins
  //    This is LED3 on STM32F0-Discovery
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOC, ENABLE);
	
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF; //Alternate Function
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;	//pin C7	
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3; //High Speed
	
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource7, GPIO_AF_1);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN; //Analog
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;		//pin C1
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}

void GPIO_SPI_init(void)
{
  GPIO_InitTypeDef        GPIO_InitStructure;  
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void init_Timer3(void)
{
 	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_TimeBaseStructure.TIM_ClockDivision		= TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode		= TIM_CounterMode_Up;
	TIM_TimeBaseStructure.TIM_Period				= 5999;
	TIM_TimeBaseStructure.TIM_Prescaler			= 0;
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	// Timer 3 Output Compare
	TIM_OCInitStructure.TIM_OCMode			= TIM_OCMode_Toggle;
	TIM_OCInitStructure.TIM_OutputState		= TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse			= 0;
	TIM_OCInitStructure.TIM_OCPolarity		= TIM_OCPolarity_High;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);
	
	TIM_Cmd(TIM3,ENABLE);
}

void init_interrupt_timer3(void)
{
	NVIC_InitTypeDef initNvic;

	initNvic.NVIC_IRQChannel			= TIM3_IRQn;
	initNvic.NVIC_IRQChannelPriority	= 1;
	initNvic.NVIC_IRQChannelCmd			= ENABLE;
	
	NVIC_Init(&initNvic); 
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM3, ENABLE);
}

void init_DMA(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		
		// Enable DMA1 peripheral
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    // De-initialize DMA1 Channel 3    
    DMA_DeInit(DMA1_Channel3);

    // DMA channel Rx of USART Configuration
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(USART1->RDR);
    DMA_InitStructure.DMA_MemoryBaseAddr     = (uint32_t)RxBuffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize         = BUFFER_SIZE;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable; 
    DMA_Init(DMA1_Channel3, &DMA_InitStructure);
    
    // Enable USART1_Rx DMA interface
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
    
    // Enable DMA1 Channel 3 (USART1_Rx)
    DMA_Cmd(DMA1_Channel3, ENABLE);
}

void init_Timer2(void)
{
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  
  //[..] To use the Timer in Output Compare mode, the following steps are mandatory:

  //(#) Enable TIM clock using 
  //    RCC_APBxPeriphClockCmd(RCC_APBxPeriph_TIMx, ENABLE) function.
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
  
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
  // ----------------------------------------------------------------------------
	//	prescaler choice : 
	//
	//
	// ----------------------------------------------------------------------------
	TIM_TimeBaseStructure.TIM_Period        = 7500 - 1;
  TIM_TimeBaseStructure.TIM_Prescaler     = 1372 - 1;
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
}

void init_interrupt_timer2(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	//(#) Enable the NVIC if you need to generate the update interrupt.
	// Enable the TIM gloabal Interrupt
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	//(#) Enable the corresponding interrupt using the function
	// TIM_ITConfig(TIMx, TIM_IT_Update).
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

	TIM_Cmd(TIM2, ENABLE);	
}





void init_button(void)
{
	init_gpio_button();
	init_interrupt_button();
	init_nvic_button();
}

void init_gpio_button(void)
{
	GPIO_InitTypeDef GPIO_initstructure;
	
	// Connect USER button to EXTI line with interrupt generation capability
  // Enable the BUTTON clock
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  // Configure button pin as input
  GPIO_initstructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_initstructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_initstructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_initstructure);
}

void init_interrupt_button(void)
{
	EXTI_InitTypeDef EXTI_initstructure;
	
	// connect Button EXTI line to Button GPIO Pin
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
  
  // configure Button EXTI line
  EXTI_initstructure.EXTI_Line = EXTI_Line0;
  EXTI_initstructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_initstructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_initstructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_initstructure);
}

void init_nvic_button(void)
{
		NVIC_InitTypeDef NVIC_initstructure;
	
	 // configure button interrupt
  NVIC_initstructure.NVIC_IRQChannel = EXTI0_1_IRQn;
  NVIC_initstructure.NVIC_IRQChannelPriority = 3;
  NVIC_initstructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_initstructure); 
}


// ----------------------------------------------------------------------------
// Main
// ----------------------------------------------------------------------------

uint8_t playfield[1][MAX_DIGITS] =
{
	{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
};
uint8_t positions[7] =
{
	3,6,12,24,48,96,192
};

int main(void)
{
	int i;
	uint8_t verticalpos =5;
	uint8_t horizontalpos=1;
	hdirection hdir = left;
	vdirection vdir = up;
	char str[10];
	init_main();
  USART_Putstr("Final assignment - Morse code\n");
	
	//To configure the ADC channels features, use ADC_Init() and  ADC_ChannelConfig() functions.
	//PC1 is configured to channel 11
	ADC_ChannelConfig(ADC1,ADC_Channel_11,ADC_SampleTime_55_5Cycles);
	
	ADC_StartOfConversion(ADC1);
		
	if (SysTick_Config(SystemCoreClock / 1000))
  { 
		USART_Putstr("An error occured\n");
    while (1);
  }
	
	
max_init();
  while(1)
  {
    USART_Putstr("Transfer 10 bytes: ");

    // ----------------------------------------------------------------------
    //  Transfer 10 bytes with DMA
    // ----------------------------------------------------------------------
		
		
    // Wait for the USART DMA Rx transfer to complete
   
			
    // Display received characters
		
    USART_itoa(verticalpos,str);
		USART_Putstr(str);
		USART_Putstr(" ");
		    USART_itoa(adc,str);
		USART_Putstr(str);
    USART_Putstr("\n");

		
		
		
		
		for(i=1;i<=MAX_DIGITS;i++)
			{
					write_frame(i, playfield[0][i-1]);
			}
		
		Delay(30);
	
  }
}






void delay(const int d)
{
  volatile int i;
  for(i=d; i>0; i--);
  return;
}
