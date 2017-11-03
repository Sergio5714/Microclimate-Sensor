#include "Si7021_sensor.h"
#define Si7021Addr 0x40
#define TemperatureCommand 0xE3
#define HumidityCommand 0xE5
#define Reset 0xE5
ErrorStatus Si7021Reset()
{
	int Command = Reset;
	int *Com = &Command;
	I2CSendBytes(Com,1,Si7021Addr);
	return SUCCESS;
}
ErrorStatus Si7021MeasureTemperature(float* Temp)
{
	int Command = TemperatureCommand;
	int *Com = &Command;
	int Value [2];
	if (I2CSendBytes(Com, 1, Si7021Addr) == ERROR)
		return ERROR;
	if (I2CReadBytes(&Value[0], 3, Si7021Addr) == ERROR)
		return ERROR;
	*Temp = 175.72*(float)(Value[0]*256 + Value[1])/65536 - 46.85;
	return SUCCESS;
}
ErrorStatus Si7021MeasureHumidity(float* Humidity)
{
	int Command = HumidityCommand;
	int *Com = &Command;
	int Value [2];
	if (I2CSendBytes(Com, 1, Si7021Addr) == ERROR)
		return ERROR;
	if (I2CReadBytes(&Value[0], 3, Si7021Addr) == ERROR)
		return ERROR;
	*Humidity = 125*(float)(Value[0]*256 + Value[1])/65536 - 6;
	return SUCCESS;
}
