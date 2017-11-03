#include "STM32_I2C.h"
#define I2C_ADDRESS(addr, mode) ((addr<<1)| mode)
#define NumberOfTicks 2000000
ErrorStatus STATE = SUCCESS;
void I2CInit()
{
	int Ticks = NumberOfTicks;
	//Turn on clocking I2C and GPIOB
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN; 
	
	//PB6 (SCL), PB7 (SDA) AF settings
	GPIOB->CRL |= GPIO_CRL_MODE6|GPIO_CRL_MODE7;
	GPIOB->CRL |= GPIO_CRL_CNF6|GPIO_CRL_CNF7;
	
	//Reset I2C
	I2C1->CR1|=I2C_CR1_SWRST;
	I2C1->CR1^=I2C_CR1_SWRST;
	
	//writing freq to CR2
	I2C1->CR2 &= ~I2C_CR2_FREQ;
	I2C1->CR2 |= 36; // 36 MHz of APB
	
	//Event and buffer interrupt enable
	//I2C1->CR2 |= I2C_CR2_ITEVTEN|I2C_CR2_ITBUFEN; 
	//Error interrupt enable
	I2C1->CR2 |= I2C_CR2_ITERREN; 
	// FastMode, 400 kHz
	I2C1->CCR |= I2C_CCR_FS;	
	
	//Clock control register
	I2C1->CCR &= ~I2C_CCR_CCR;
	I2C1->CCR |= 90;
	
	//Speed of signal's rising
	I2C1->TRISE = 15;
	
	//Enable I2C block
	I2C1->CR1 |= I2C_CR1_PE;
	
	//NVIC_EnableIRQ(I2C1_ER_IRQn);
	while (Ticks)
	{
		Ticks--;
	}
	STATE=SUCCESS;
	return;
}
void I2CReset(void)
{
	int Ticks = NumberOfTicks;
	I2C1->CR1 |= I2C_CR1_SWRST;
	while (Ticks)
	{
		Ticks--;
	}
	I2CInit();
	return;
}
ErrorStatus I2CStart()
{
	int Ticks = NumberOfTicks;
	//Start
	I2C1->CR1 |= I2C_CR1_START;
	//checking START and sending address
	while (!((I2C1->SR1)&I2C_SR1_SB))
	{
		if (STATE==ERROR)
		{
			return STATE;
		}
		Ticks--;
		if (Ticks == 0)
		{
			return ERROR;
		}
	}
	//Clear SB flag (+ followed writing in DR)
	(void) I2C1->SR1;
	return STATE;
}
void I2CStop()
{
	//STOP
	I2C1->CR1|=I2C_CR1_STOP;
	return;
}
ErrorStatus I2CSendAddr(int Addr, I2C_MODE Mode)
{
	int Ticks = NumberOfTicks;
	//Send adress + mode
	I2C1->DR = I2C_ADDRESS(Addr,Mode);
	//Waiting for sending adress
	while (!(I2C1->SR1 & I2C_SR1_ADDR))
	{
		if (STATE==ERROR)
		{
			return STATE;
		}
		Ticks--;
		if (Ticks == 0)
		{
			return ERROR;
		}
	}
	return STATE;
}
void I2CClearADDR()
{
	//clear address flag
	(void) I2C1->SR2;
}
void I2CSetPOS()
{
	I2C1->CR1 |= I2C_CR1_POS;
}
void I2CUnSetPOS()
{
	I2C1->CR1 &= ~I2C_CR1_POS;
}
void I2CSetAck()
{
		I2C1->CR1 |= I2C_CR1_ACK;
}
void I2CUnSetAck()
{
		I2C1->CR1 &= ~I2C_CR1_ACK;
}
ErrorStatus I2CWaitForByte()
{
	int Ticks = NumberOfTicks;
	//Waiting for RXNE flag to be set
	while (!(I2C1->SR1 & I2C_SR1_RXNE))
	{
		if (STATE==ERROR)
		{
			return STATE;
		}
		Ticks--;
		if (Ticks == 0)
		{
			return ERROR;
		}
	}
	return STATE;
}
ErrorStatus I2CWaitForBTF()
{
	int Ticks = NumberOfTicks;
	//Waiting for BTF flag to be set
	while (!(I2C1->SR1 & I2C_SR1_BTF))
	{
		if (STATE==ERROR)
		{
			return STATE;
		}
		Ticks--;
		if (Ticks == 0)
		{
			return ERROR;
		}
	}
	return STATE;
}
ErrorStatus I2CWaitBusyLine()
{
	int Ticks = NumberOfTicks;
	//Waiting for Bus to be free
	while (I2C1->SR2 & I2C_SR2_BUSY)
	{
		if (STATE==ERROR)
		{
			return STATE;
		}
		Ticks--;
		if (Ticks == 0)
		{
			return ERROR;
		}
	}
	return STATE;
}
void I2CReadDR(int *byte)
{
	*byte=I2C1->DR;
}

ErrorStatus I2CSendBytes(int* Bytes, int NumberOfSendBytes, int SlaveAddr)
{
	if(I2CWaitBusyLine() == ERROR)
		return ERROR;
	if(I2CStart() == ERROR)
		return ERROR;
	if(I2CSendAddr(SlaveAddr,I2C_MODE_WRITE)== ERROR)
		return ERROR;
	I2CClearADDR();
	while (NumberOfSendBytes)
	{
		//sending Byte
		I2C1->DR = *Bytes;
		//Waiting for sending Byte
		if (I2CWaitForBTF() == ERROR)
				return ERROR;
		//Clear BTF flag
		(void) I2C1->SR1;
		Bytes++;
		NumberOfSendBytes--;
	}
	I2CStop();
	if(I2CWaitForStopToBeCleared() == ERROR)
		return ERROR;
	return SUCCESS;
}
void I2CSearch()
{
	int Address = 0x7F;
	while (Address!=0)
	{
		int Ticks = NumberOfTicks/1000;
		I2CSetAck();
		I2CWaitBusyLine();
		I2CStart();
		//Send adress + mode Read
		I2C1->DR = I2C_ADDRESS(Address,I2C_MODE_WRITE);
		//Waiting for sending adress
		while ((!(I2C1->SR1 & I2C_SR1_ADDR))&&(Ticks!=0))
		{
			Ticks--;
		}
		I2CStop();
		I2CWaitForStopToBeCleared();
		if (Ticks!=0)
		{
			int device = Address;
		}
		Address--;
		I2C1->SR1&=~I2C_SR1_AF;
		//clear address flag
		(void) I2C1->SR2;
	}
	
}
ErrorStatus I2CWaitForStopToBeCleared()
{
	int Ticks = NumberOfTicks;
	while (I2C1->CR1 & I2C_CR1_STOP)
	{
		if (STATE == ERROR)
		{
			return STATE;
		}
		Ticks--;
		if (Ticks == 0)
		{
			return ERROR;
		}
	}
	return STATE;
}
ErrorStatus I2CReadBytes(int*PResults, int NumBytesToRead, int SlaveAddr)
{
	if(I2CWaitBusyLine()== ERROR)
			return ERROR;
	if (NumBytesToRead == 1)
	{
		if(I2CStart() == ERROR)
			return ERROR;
		if(I2CSendAddr(SlaveAddr,I2C_MODE_READ)== ERROR)
			return ERROR;
		I2CUnSetAck();
		__disable_irq();
		I2CClearADDR();
		I2CStop();
		__enable_irq();
		if(I2CWaitForByte()== ERROR)
			return ERROR;
		I2CReadDR(PResults);
		if(I2CWaitForStopToBeCleared()== ERROR)
			return ERROR;
		I2CSetAck();
		return SUCCESS;
	}
	else if (NumBytesToRead == 2)
  {
		I2CSetAck();
		I2CSetPOS();
		if(I2CStart() == ERROR)
			return ERROR;
		if(I2CSendAddr(SlaveAddr,I2C_MODE_READ)== ERROR)
			return ERROR;
		__disable_irq();
		I2CClearADDR();
		I2CUnSetAck();
		__enable_irq();
		if(I2CWaitForBTF()== ERROR)
			return ERROR;
		__disable_irq();
		I2CStop();
		I2CReadDR(PResults);
		__enable_irq();
		PResults++;
		I2CReadDR(PResults);
		if(I2CWaitForStopToBeCleared()== ERROR)
			return ERROR;
		I2CUnSetPOS();
		I2CSetAck();
	}
	else
  {
		if(I2CStart() == ERROR)
			return ERROR;
		if(I2CSendAddr(SlaveAddr,I2C_MODE_READ)== ERROR)
			return ERROR;
		I2CClearADDR();
		while (NumBytesToRead)
		{
			if (NumBytesToRead != 3)
			{
				if(I2CWaitForBTF()== ERROR)
					return ERROR;
				I2CReadDR(PResults);
				PResults++;
				NumBytesToRead--;
			}
			if (NumBytesToRead == 3)
			{
				if(I2CWaitForBTF()== ERROR)
					return ERROR;
				I2CUnSetAck();
				__disable_irq();
				I2CReadDR(PResults);
				PResults++;
				I2CStop();
				I2CReadDR(PResults);
				__enable_irq();
				PResults++;
				if(I2CWaitForByte()== ERROR)
					return ERROR;
				I2CReadDR(PResults);
				NumBytesToRead = 0;
			}
		}
		if(I2CWaitForStopToBeCleared()== ERROR)
			return ERROR;
		I2CSetAck();
	}
	return SUCCESS;
}
