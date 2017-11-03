#if !defined (STM32F10X_MD)
#define STM32F10X_MD
#endif
#include "stm32f10x.h"
ErrorStatus Z19MeasureCO2(USART_TypeDef * USARTx, int* Value);
void Z19CalibrateZero(USART_TypeDef * USARTx);
void Z19CalibrateSpan(USART_TypeDef * USARTx);
