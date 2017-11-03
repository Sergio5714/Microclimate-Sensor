#ifndef __STM32_I2C_h__
 #define __STM32_I2C_h__
#include "stm32f10x.h"
typedef enum
{
	I2C_MODE_WRITE,
	I2C_MODE_READ,
} I2C_MODE;
void I2CInit(void);
void I2CReset(void);
ErrorStatus I2CStart(void);
void I2CStop(void);
ErrorStatus I2CSendAddr(int Addr, I2C_MODE Mode);
void I2CClearADDR(void);
void I2CSetPOS(void);
void I2CUnSetPOS(void);
ErrorStatus I2CSendBytes(int* Bytes, int NumberOfSendBytes, int SlaveAddr);
ErrorStatus I2CWaitForByte(void);
ErrorStatus I2CWaitForBTF(void);
ErrorStatus I2CWaitForStopToBeCleared(void);
ErrorStatus I2CWaitBusyLine(void);
ErrorStatus I2CReadBytes(int*PResults, int NumBytesToRead, int SlaveAddr);
void I2CSearch(void);
void I2CReadDR(int *byte);
void I2CSetAck(void);
void I2CUnSetAck(void);

#endif /* __STM32_I2C_h_ */
