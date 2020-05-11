#ifndef __DRIVERSTORAGE_H__
#define __DRIVERSTORAGE_H__

#include "DriverBoardConfig.h"

typedef struct
{
		uint8_t iSubdivisionCfg;
		uint8_t iCurrentCfg;
		uint8_t iDriverID;
}StorageDataBlock;

bool StorageBlockInit(StorageDataBlock *Block_t);
bool GetDriverInfo(StorageDataBlock *Block_t);
void WriteDriverInfo(StorageDataBlock *Block_t);
	
#endif
