/******************************************************************************
 * File           : Helper funcions used throughout all targets
 *****************************************************************************/
#ifndef _HELPER_H_
#define _HELPER_H_

#include "stm32f0xx.h"

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
static __IO uint32_t TimingDelay;
#define MULTIPLIER 1.905

#endif /* _HELPER_H_ */
