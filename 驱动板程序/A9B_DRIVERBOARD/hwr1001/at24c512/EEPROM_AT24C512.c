#include "EEPROM_AT24C512.h"

void AT24C512_Init(void)
{
		MX_I2C2_Init();
}

HAL_StatusTypeDef AT24C512_WriteByte(uint16_t iAddress, uint8_t iData)
{
}

HAL_StatusTypeDef AT24C512_ReadByte(uint16_t iAddress, uint8_t *iData)
{
}