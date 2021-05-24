/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#ifndef _HELPER_H_
#define _HELPER_H_

#include "stm32f0xx.h"

typedef enum 
{
	left,
	right
}hdirection;
typedef enum 
{
	up,
	down
}vdirection;



// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------
void Delay(__IO uint32_t nTime);
void TimingDelay_Decrement(void);
void  USART_Setup(void);
void  USART_Putstr(char *str);
void  USART_Putc(char c);
char  USART_Getc(void);
void  USART_Getstr(char *str);
void  USART_Clearscreen(void);
char *USART_itoa(int16_t i, char *p);
long map(long x);
void moveDot(vdirection *vdir,hdirection *hdir,uint8_t *ver,uint8_t *hor);
void write_frame (uint8_t address, uint8_t data);
static __IO uint32_t TimingDelay;

#define MULTIPLIER 1.905
#define DIN_PIN GPIO_Pin_7
#define CS_PIN GPIO_Pin_4
#define CLK_PIN GPIO_Pin_5

#define AMOUNT_OF_FRAMES (1)										//contains amount of frames of the whole animation
#define MAX_DIGITS (8)	



#endif /* _HELPER_H_ */
