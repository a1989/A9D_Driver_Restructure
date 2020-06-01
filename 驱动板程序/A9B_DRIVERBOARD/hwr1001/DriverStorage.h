#ifndef __DRIVERSTORAGE_H__
#define __DRIVERSTORAGE_H__

#include "defines.h"

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		
		void (*m_pAddDevice)(PRIVATE_MEMBER_TYPE *pThis, StorageParams *pParams_t);
		bool (*m_pStorageReadByte)(PRIVATE_MEMBER_TYPE *pThisPrivate, StorageParams *pParams_t, StorageByteOptions *pOps_t);
		bool (*m_pStorageWriteByte)(PRIVATE_MEMBER_TYPE *pThisPrivate, StorageParams *pParams_t, StorageByteOptions *pOps_t);
}StorageControl;

bool StorageBlockInit(StorageControl *Block_t);
	
#endif
