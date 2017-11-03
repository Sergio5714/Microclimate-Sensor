#if !defined (STM32F10X_MD)
#define STM32F10X_MD
#endif
#include "stm32f10x.h"
void TIMInit(TIM_TypeDef * TIMx, int Prescaler, int autoReloadRegisterValue);
void TIMEnable(TIM_TypeDef * TIMx);
void TIMDisable(TIM_TypeDef * TIMx);
void TIMDelay(TIM_TypeDef * TIMx);
