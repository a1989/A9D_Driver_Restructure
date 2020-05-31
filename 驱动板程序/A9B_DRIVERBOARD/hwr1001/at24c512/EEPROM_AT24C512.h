#ifndef __EEPROM_AT24C512_H__
#define __EEPROM_AT24C512_H__

#include "defines.h"



typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		bool (*m_pAT24C512_WriteByte)(PRIVATE_MEMBER_TYPE *pThis, StorageByteOptions *Params_t);
		bool (*m_pAT24C512_ReadByte)(PRIVATE_MEMBER_TYPE *pThis, StorageByteOptions *Params_t);
}AT24C512_Control;

void AT24C512_Init(PRIVATE_MEMBER_TYPE *pThis, DriverConfigMode eMode);

#endif
