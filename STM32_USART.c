#include "stm32f10x.h"
#include "STM32_USART.h"
#define NumberOfTicks 2000000
#define Mask	0xff

//Initialize USART
void UsartInit(USART_TypeDef * USARTx, int baudrate )
{
	float frequency;
	int divMantissa;
	int divFraction;
	//USART settings
	//Turn on GPIOA and enable Alternative functions
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN|RCC_APB2ENR_AFIOEN;
	if (USARTx == USART1)
		{
		//Pins PA9 (TX1) and PA10(RX1) settings
		
		//Clear all settings on A9 and A10	
		GPIOA->CRH &= ~GPIO_CRH_MODE10&~GPIO_CRH_CNF10&~GPIO_CRH_MODE9&~GPIO_CRH_CNF9;
		
		//Set on PA9 Output mode (10 mHz) and Alternative function Output Push pull 
		//on PA10 Input mode with floating input 
		GPIOA->CRH |= GPIO_CRH_MODE9_0|GPIO_CRH_CNF9_1|GPIO_CRH_CNF10_0;
			
		//Enable clocking USART1
		RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
		}
	if (USARTx == USART2)
		{
		//Pins PA2 (TX2) and PA3(RX2) settings
		
		//Clear all settings on A2 and A3?
		GPIOA->CRL &= ~GPIO_CRL_MODE2&~GPIO_CRL_CNF2&~GPIO_CRL_MODE3&~GPIO_CRL_CNF3;
		
		//Set on PA2 Output mode (10 mHz) and Alternative function Output Push pull 
		//on PA3 Input mode with floating input 
		GPIOA->CRL |= GPIO_CRL_MODE2_0|GPIO_CRL_CNF2_1|GPIO_CRL_CNF3_0;
		
		//Enable clock for USART2
		RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
		}
	
	//Enable receiver, tranceiver and receive interrupt
	USARTx->CR1 |= USART_CR1_RE |USART_CR1_TE|USART_CR1_RXNEIE;
		
	SystemCoreClockUpdate();
	frequency = SystemCoreClock;
	if (USARTx == USART2)
	{
		frequency = frequency/2;
	}
	divMantissa = (int)(frequency/(16*baudrate));
	divFraction = (int)(((frequency/(16*baudrate)) - divMantissa)*16 + 0.5);
	//USARTx->BRR=8+(937<<4);	//Set baudrate 4800 (Clock 72mHz)
	if (divFraction == 16)
	{
		divMantissa++;
		divFraction = 0;
	}
	USARTx->BRR = divFraction + (divMantissa << 4);
	//Enable USARTx
	USARTx->CR1 |= USART_CR1_UE;
}

//Disable USARTx
void UsartDisable(USART_TypeDef * USARTx)
{
	USARTx->CR1 &= ~USART_CR1_UE;
}

ErrorStatus UsartGetC(USART_TypeDef * USARTx, char* c)
{
	int ticks = NumberOfTicks;
	while(!(USARTx->SR & USART_SR_RXNE)) // if Read buffer not empty flag is set
			{
				ticks--;
				if (ticks == 0)
				{
					return ERROR;
				}
			}
	*c = USARTx->DR;
	//Clear flag
	USARTx->SR&=~USART_SR_RXNE;
	return SUCCESS;
}
ErrorStatus UsartPutC(USART_TypeDef * USARTx,unsigned char c)
{
	int ticks = NumberOfTicks;
	USARTx->DR=c;
	//Wait transmitting
		while (!(USARTx->SR&USART_SR_TC) )
		{
				ticks--;
				if (ticks == 0)
				{
					return ERROR;
				}
		}
	return SUCCESS;
}
ErrorStatus UsartPutS(USART_TypeDef * USARTx, const char *str)
{
	ErrorStatus state;
	while(*str) 
	{
		state = UsartPutC(USARTx, *str++);
		if (state == ERROR)
		{
			return ERROR;
		}
	}
	return SUCCESS;
}
ErrorStatus UsartPutInt(USART_TypeDef * USARTx, int value)
{
	ErrorStatus state;
	//first part
	char Byte_1=(value)&(Mask); 
	//second part
	char Byte_2=(value)>>8;	

	state = UsartPutC(USARTx, Byte_1);
	state = UsartPutC(USARTx, Byte_2);
	return state;
}
ErrorStatus UsartPutNumberInASCII(USART_TypeDef * USARTx, int x)
{
	ErrorStatus state;
	//A temp array to hold results of conversion
  char value[10];
	//loop index
  int i = 0;
  do
  {
		//convert integer to character
    value[i++] = (char)(x % 10) + '0';
    x /= 10;
  } while(x);
  //send data
  while(i)
  {
    state = UsartPutC(USARTx, value[--i]); 
  }
	return state;
}
