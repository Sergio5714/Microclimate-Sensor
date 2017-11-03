#include "STM32_USART.h"
#include "MHZ19_sensor.h"

ErrorStatus Z19MeasureCO2(USART_TypeDef * USARTx, int* value)
{
	int i = 0;
	char data[9];
	//Send command
	UsartPutC(USARTx,0xFF);
	UsartPutC(USARTx,0x01);
	UsartPutC(USARTx,0x86);
	for (i = 0; i < 5; i++)
	{
		if (UsartPutC(USARTx,0x00) == ERROR)
		return ERROR;
	}
	if (UsartPutC(USARTx,0x79) == ERROR)
	return ERROR;
	//Get answer
	for (i = 0; i < 9; i++)
	{
		if(UsartGetC(USARTx,&data[i]) == ERROR)
		return ERROR;
	}
	*value = (data[2]*256+data[3]);
	return SUCCESS;
}
void Z19CalibrateZero(USART_TypeDef * USARTx)
{
	int i = 0;
	UsartPutC(USARTx,0xFF);
	UsartPutC(USARTx,0x01);
	UsartPutC(USARTx,0x87);
	for (i=0;i<5;i++)
	{
		UsartPutC(USARTx,0x00);
	}
	UsartPutC(USARTx,0x78);
}
void Z19CalibrateSpan(USART_TypeDef * USARTx)
{
	int i = 0;
	UsartPutC(USARTx,0xFF);
	UsartPutC(USARTx,0x01);
	UsartPutC(USARTx,0x88);
	UsartPutC(USARTx,0x07);
	UsartPutC(USARTx,0xD0);
	for (i=0;i<3;i++)
	{
		UsartPutC(USARTx,0x00);
	}
	UsartPutC(USARTx,0xA0);
}

