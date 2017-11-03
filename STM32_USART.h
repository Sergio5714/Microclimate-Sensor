#if !defined (STM32F10X_MD)
#define STM32F10X_MD
#endif
#include "stm32f10x.h"
//Initialize USART
void UsartInit(USART_TypeDef * USARTx, int baudrate );

//Disable USART
void UsartDisable(USART_TypeDef * USARTx);

//Read one char from USART
ErrorStatus UsartGetC(USART_TypeDef * USARTx, char * c);

//Write one char to USART
ErrorStatus UsartPutC(USART_TypeDef * USARTx, unsigned char c);
	
//Write string to the USART
ErrorStatus UsartPutS(USART_TypeDef * USARTx, const char *str);

//Send Int
ErrorStatus UsartPutInt(USART_TypeDef * USARTx, int value);

//Print number in ASCII
ErrorStatus UsartPutNumberInASCII(USART_TypeDef * USARTx, int x);

