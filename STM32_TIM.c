#include "stm32f10x.h"
#include "STM32_TIM.h"

void TIMInit(TIM_TypeDef * TIMx, int Prescaler, int autoReloadRegisterValue)
{
	/*float frequency;
	int AHBDivider;
	int APB1Divider;
	SystemCoreClockUpdate();
	frequency = SystemCoreClock;
	
	//AHBDivider = ((RCC->CFGR)& ~RCC_CFGR_HPRE_DIV512) >> 4 -;
	//APB1Divider = ((RCC->CFGR)& ~RCC_CFGR_PPRE1_DIV16) >> 8 - 8;
	//frequency = frequency/AHBDivider/APB1Divider;
	*/	
	
	//Enable clocking TIM
	if (TIMx == TIM2)
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	if (TIMx == TIM3)
	RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
	if (TIMx == TIM4)
	RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;
	
	// Clear the update event flag
	TIMx->SR = 0;
	
	//Update request source is only counter 
	TIMx->CR1 = TIM_CR1_URS;
	
	//if APB1 prescaler !=1 then for TIM2,3,4 clock freq = APB1 clock freq*2
	//Now clock for TIM1 is 72MHz
	
	//Set Auto reload register //2000 = 1 sec
	TIMx->ARR = autoReloadRegisterValue;
	//TIMx->EGR|=TIM_EGR_UG;
	
	//Enable interrupt
	TIMx->DIER|=TIM_DIER_UIE;
	//Set prescaler for input clock
	TIMx->PSC = Prescaler;
}
void TIMEnable(TIM_TypeDef * TIMx)
{
	TIMx->CNT=0;
	TIMx->CR1|=TIM_CR1_CEN;
	// clear UIF flag
	TIMx->SR &= ~TIM_SR_UIF;
}
void TIMDisable(TIM_TypeDef * TIMx)
{
	TIMx->CR1&=~TIM_CR1_CEN;
	TIMx->CNT=0;
}
void TIMDelay(TIM_TypeDef * TIMx)
{
	TIMEnable(TIMx);
	while (!(TIMx->SR & TIM_SR_UIF)); // if UIF flag is set
	TIMx->SR &= ~TIM_SR_UIF; // clear UIF flag
	TIMDisable(TIMx);
}

