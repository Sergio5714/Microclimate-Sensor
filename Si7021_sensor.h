#include "STM32_I2C.h"
ErrorStatus Si7021Reset(void);
ErrorStatus Si7021MeasureTemperature(float* Temp);
ErrorStatus Si7021MeasureHumidity(float* Humidity);
