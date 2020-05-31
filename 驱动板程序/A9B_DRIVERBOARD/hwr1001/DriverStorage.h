#ifndef __DRIVERSTORAGE_H__
#define __DRIVERSTORAGE_H__

#include "defines.h"

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		
		void (*m_pAddDevice)(PRIVATE_MEMBER_TYPE *pThis, StorageParams *pParams_t);
		bool (*m_pEEPROM_ReadByte)(PRIVATE_MEMBER_TYPE *pThis, EEPROM_Params *pParams_t);
		bool (*m_pEEPROM_WriteByte)(PRIVATE_MEMBER_TYPE *pThis, EEPROM_Params *pParams_t);
}StorageControl;

bool StorageBlockInit(StorageControl *Block_t);
	
#endif
