#ifndef __EEPROM_AT24C512_H__
#define __EEPROM_AT24C512_H__

#include "i2c.h"

void AT24C512_Init(void);
HAL_StatusTypeDef AT24C512_WriteByte(uint16_t iAddress, uint8_t iData);
HAL_StatusTypeDef AT24C512_ReadByte(uint16_t iAddress, uint8_t *iData);

#endif
