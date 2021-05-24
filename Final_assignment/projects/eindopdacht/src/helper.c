/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#include "helper.h"

void USART_Setup(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  
  // --------------------------------------------------------------------------
  // Initialize USART1
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  // Setup Tx- and Rx pin
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);
  
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  USART_StructInit(&USART_InitStructure);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_Init(USART1, &USART_InitStructure);
  
  USART_Cmd(USART1, ENABLE);
}

void USART_Putc(char c)
{
  // Wait for Transmit data register empty
  while((USART1->ISR & USART_ISR_TXE) == 0) ;

  // Transmit data by writing to TDR, clears TXE flag  
  USART1->TDR = c;
}

/**
  * @brief  This function sends a string of characters through USART1.
  *         If a LINEFEED character ('\n') is detected, the functions also
  *         sends a CONTROL ('\r') character.
  * @param  str: NULL ('\0') terminated string
  * @retval None
  */
void USART_Putstr(char *str)
{
  while(*str)
  {
    if(*str == '\n')
    {
      while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){;}
      USART_SendData(USART1, '\r');
    }
    
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){;}
    USART_SendData(USART1, *str++);
  }
}

char USART_Getc(void)
{
	char c;
  // Was there an Overrun error?
  if((USART1->ISR & USART_ISR_ORE) != 0)
  {
    // Yes, clear it 
    USART1->ICR |= USART_ICR_ORECF;
  }

  // Wait for data in the Receive Data Register
  while((USART1->ISR & USART_ISR_RXNE) == 0) ;

  // Read data from RDR, clears the RXNE flag
  c = USART1->RDR;

  return(c);
}

void USART_Getstr(char *str)
{
	int i = 0;
  // Was there an Overrun error?
  if((USART1->ISR & USART_ISR_ORE) != 0)
  {
    // Yes, clear it 
    USART1->ICR |= USART_ICR_ORECF;
  }
	
  while((USART1->ISR & USART_ISR_RXNE) == 0) ;
	
	while((char)USART1->RDR != '0') {
		//display written character in console
		USART_Putc((char)USART1->RDR);
		//write character to char array
		str[i] = (char)USART1->RDR;
		//move to next position in array
		i++;
		//wait for next character to come in
		while((USART1->ISR & USART_ISR_RXNE) == 0);	
	}
	USART_Putstr("\n");
}

/**
  * @brief  This function implements the following VT100 terminal commands
  *         * Clear screan
  *         * Cursor home
  * @param  None
  * @retval None
  */
void USART_Clearscreen(void)
{
  char cmd1[5] = {0x1B, '[', '2', 'J', '\0'}; // Clear screen
  char cmd2[4] = {0x1B, '[', 'f', '\0'}; // Cursor home
  
  USART_Putstr(cmd1);
  USART_Putstr(cmd2);
}


long map(long x) {
	long val = (x>>5);
	if(val == 7)
	{
	val = 6;
	}
  return (val);
}

/**
  * @brief  This function converts an integer to an ASCII string of characters
  *         The string is terminated with a NULL ('\0') character.
  * @param  i: the number to convert. Negative numbers are also allowed.
  *         p: pointer to the destination buffer
  * @retval pointer to the start of the destination buffer
  */
char *USART_itoa(int16_t i, char *p)
{
  int16_t t1, t2;
  char h[10];

  t1 = t2 = 0;

  if (i < 0)
  {
    p[t2] = '-';
    t2++;
    i = -i;
  }
  
  do
  {
    h[t1] = i % 10 + 48;
    t1++;
    i = i / 10;

  }while (i > 0);

  while (t1 > 0)
  {
    p[t2++] = h[--t1];
  }

  p[t2] = '\0';

  return(p);
}

void moveDot(vdirection *vdir,hdirection *hdir,uint8_t *ver,uint8_t *hor)
{
	if(*ver> 8)
	{
		*vdir = down;
	}
	if(*ver< 1)
	{
		*vdir = up;
	}
	if(*hor>=7)
	{
		*hdir = right;
	}
	if(*hor< 1)
	{
		*hdir = left;
	}
	if(*hdir)
	{
		*hor = *hor-1;
	}else
	{
		*hor = *hor+1;
	}
	if(*vdir)
	{
		*ver = *ver-1;
	}else
	{
		*ver = *ver+1;
	}
}

void write_matrix (uint8_t byte)
{
	int i;

	for (i=1;i<=MAX_DIGITS;i++)
	{
		GPIO_ResetBits(GPIOA, CLK_PIN); 						// pull the clock pin low
		
		GPIO_WriteBit (GPIOA, DIN_PIN, byte&0x80);  // write the MSB bit to the data pin
		byte = byte<<1;  														// shift left
		
		GPIO_SetBits(GPIOA, CLK_PIN); 							// pull the clock pin HIGH
	}
}

void write_frame (uint8_t address, uint8_t data)
{
	GPIO_ResetBits(GPIOA, CS_PIN); // pull the CS pin LOW
	
	write_matrix (address);
	write_matrix (data); 
	
	GPIO_SetBits(GPIOA, CS_PIN); // pull the CS pin HIGH
}

void max_init(void)
{
 write_frame(0x09, 0x00);      
 write_frame(0x0a, 0x0f);       
 write_frame(0x0b, 0x07);         
 write_frame(0x0c, 0x01);       
 write_frame(0x0f, 0x00);       
}


void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;

  while(TimingDelay != 0);
}


void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}


