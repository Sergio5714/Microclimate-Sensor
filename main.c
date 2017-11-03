
#if !defined (STM32F10X_MD)
#define STM32F10X_MD
#endif

#include "stm32f10x.h"
#include "STM32_USART.h"
#include "MHZ19_sensor.h"
#include "STM32_TIM.h"
#include "STM32_I2C.h"
#include "Si7021_sensor.h"
int debugMarker = 0;

int messageReceived = 0;
int mode = 0;
int a = 0;
int message = 0;
const int sizeOfInputBuffer = 20;
const int sizeOfOutputBuffer = 25;
char inputBuffer [sizeOfInputBuffer];
char outputBuffer [sizeOfOutputBuffer];

int CO2Concentration = 0;
float Humidity = 0;
float Temperature = 0;

//Prepare Array in "fucking ASCII format" (FASCII)
void PrepareAndSendPackageInFASCII(USART_TypeDef * USARTx, char*array, int temperature, int humidity, int co2Concentration)
{
	int i = 0;
	//convert integer to character
	do
	{
		array[i++] = 0;
		array[i++] = (char)( co2Concentration% 10) + '0';
		co2Concentration /= 10;
	} while(co2Concentration);
	
	array[i++] = 0;
	array[i++] = ',';
	
	do
	{
		array[i++] = 0;
		array[i++] = (char)( humidity% 10) + '0';
		humidity /= 10;
	} while(humidity);
	
	array[i++] = 0;
	array[i++] = ',';	
	
	do
	{
		array[i++] = 0;
		array[i++] = (char)( temperature% 10) + '0';
		temperature /= 10;
	} while(temperature);
	
	//send data
	while(i)
	{
		UsartPutC(USARTx, array[--i]); 
	}
}
void USART1_IRQHandler (void)
{
	if(USART1->SR & USART_SR_RXNE) // if Read buffer not empty flag is set
	{
		//Clear flag
		USART1->SR &= ~USART_SR_RXNE;
	}
}
void massShift( char* massPointer, int size)
{
	int i =0;
	for (i = size - 1; i > 0 ; i--)
	{
		massPointer[i] = massPointer[i-1];
	}
}
void massClear( char* massPointer, int size)
{
	int i =0;
	for (i = 0; i < size ; i++)
	{
		massPointer[i] = 0;
	}
}
int recognizeString(const char* sequence, int sizeOfSequence, char* massPointer, int sizeOfPointer)
{
	int i;
	int j;
	for (i = sizeOfInputBuffer - 1; i >= sizeOfSequence - 1; i-- )
	{
		for (j = 0; j < sizeOfSequence; j++)
		{
			if ( sequence[j] != massPointer[i - j])
			{
				break;
			}
			if (j == sizeOfSequence - 1)
			{
				return 1;
			}
		}
	}
	return 0;
}
void USART2_IRQHandler (void)
{
	if(USART2->SR & USART_SR_RXNE) // if Read buffer not empty flag is set
	{
		massShift(&inputBuffer[0], sizeOfInputBuffer);
		inputBuffer[0] = USART2->DR;
		messageReceived = 1;
		//Clear flag
		USART2->SR&=~USART_SR_RXNE;
	}
}
void TIM2_IRQHandler (void)
{
	if(TIM2->SR & TIM_SR_UIF) // if UIF flag is set
	{
		TIM2->SR &= ~TIM_SR_UIF; // clear UIF flag
	}
	//PrepareAndSendPackageInFASCII(USART2, &outputBuffer[0], 27, 64, 400);
	PrepareAndSendPackageInFASCII(USART2, &outputBuffer[0], Temperature, Humidity, CO2Concentration);
}
void ConfigureBLEMaster()
{
	while (1)
	{
		UsartPutS(USART2, "AT");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
	while (1)
	{
		UsartPutS(USART2, "AT+IMME1");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
	while (1)
	{
		UsartPutS(USART2, "AT+ROLE1");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
	while (1)
	{
		UsartPutS(USART2, "AT+COND43639DAE426");
		TIMDelay(TIM3);
		if (recognizeString("OK+CONNA", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
}
void ConfigureBLESlave()
{
	while (1)
	{
		UsartPutS(USART2, "AT+RENEW");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
	while (1)
	{
		UsartPutS(USART2, "AT");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
	while (1)
	{
		UsartPutS(USART2, "AT+ROLE0");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
	while (1)
	{
		UsartPutS(USART2, "AT+RESET");
		TIMDelay(TIM3);
		if (recognizeString("OK", 2, &inputBuffer[0], sizeOfInputBuffer))
		{
			massClear(&inputBuffer[0], sizeOfInputBuffer);
			break;
		}
	}
}
int main()
{
	SystemCoreClockUpdate();
	
	//Timer for sending all the data to Bluetooth HM-10 module
	TIMInit(TIM2, 35999, 2000);	//1 second
	
	//delay timer
	TIMInit(TIM3, 35999, 1000);//1
	TIMInit(TIM4, 35999, 20000);// 10 seconds
	
	__enable_irq();
	NVIC_EnableIRQ(USART2_IRQn);
	//NVIC_EnableIRQ(USART1_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);
	//NVIC_EnableIRQ(TIM3_IRQn);
	
	//Possible solution to launch mhZ19 (it nedds some time to be initialized)
	TIMDelay(TIM4);

	//for BLE 4.0
	UsartInit(USART2,9600);
	//for CO2 sensor
	UsartInit(USART1,9600);
	
	ConfigureBLESlave();
	
	I2CInit();
	Si7021Reset();
	
	TIMDelay(TIM3);
	Z19CalibrateZero(USART1);
	TIMDelay(TIM3);
	Z19CalibrateSpan(USART1);
	TIMDelay(TIM3);
	
	TIMEnable(TIM2);
	while(1)
	{
		if(Z19MeasureCO2(USART1, &CO2Concentration) == ERROR)
		{
			UsartDisable(USART1);
			UsartInit(USART1, 9600);
			debugMarker++;
		}
		if(Si7021MeasureTemperature(&Temperature) == ERROR)
		{
			I2CReset();
			Si7021Reset();
		}
		if (Si7021MeasureHumidity(&Humidity) == ERROR)
		{
			I2CReset();
			Si7021Reset();
		}
		if (mode == 0)
		{
		// Some action
		}
		if (mode == 1)
		{
			if (CO2Concentration > 1000)
			{
				UsartPutC(USART2, 1);
			}
			if (CO2Concentration < 1000)
			{
				UsartPutC(USART2, 0);
			}
		}
		if (messageReceived == 1)
		{
			if (recognizeString("111", 2, &inputBuffer[0], sizeOfInputBuffer))
			{
				mode = 1;
				massClear(&inputBuffer[0], sizeOfInputBuffer);
				TIMDisable(TIM2);
				ConfigureBLEMaster();
			}
			if (recognizeString("000", 2, &inputBuffer[0], sizeOfInputBuffer))
			{
				mode = 0;
				massClear(&inputBuffer[0], sizeOfInputBuffer);
				ConfigureBLESlave();
				TIMEnable(TIM2);
			}
			messageReceived = 0;
		}
		TIMDelay(TIM4);
	}
}
