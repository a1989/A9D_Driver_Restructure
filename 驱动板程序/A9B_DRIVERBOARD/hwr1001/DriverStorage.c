#include "DriverStorage.h"

#include "EEPROM_AT24C512.h"

typedef struct structStorageList
{
		uint8_t iStorageID;
		void *pStorage;
		struct structStorageList *pNext_t;
}StorageList;

typedef struct
{
		StorageList *pStorageList;
}PrivateBlock;

bool StorageBlockInit(StorageControl *Block_t)
{
		Block_t->m_pThisPrivate = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		
}

void AddDevice(PRIVATE_MEMBER_TYPE *pThisPrivate, StorageParams *pParams_t)
{
		PrivateBlock *pPrivate_t = NULL;
		AT24C512_Control *AT24C512_Control_t = NULL;
		void *pDevice = NULL;
		StorageList *pList = NULL;
		StorageList *pNode = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		
		
		pPrivate_t = (PrivateBlock *)pThisPrivate;
				
		switch(pParams_t->eStorageDevice)
		{
				case eAT24C512:
					//³õÊ¼»¯at24c512
					AT24C512_Control_t = (AT24C512_Control *)malloc(sizeof(AT24C512_Control));
					AT24C512_Init(AT24C512_Control_t->m_pThisPrivate, pParams_t->eStorageMode);
					pDevice = AT24C512_Control_t;
					break;
				default:
					break;
		}
		
		pList = (StorageList *)malloc(sizeof(StorageList));
		if(NULL == pList)
		{
				return;
		}
		
		pList->pStorage = pDevice;
		pList->iStorageID = pParams_t->iID;
		pList->pNext_t = NULL;
		
		if(NULL == pPrivate_t->pStorageList)
		{
				pPrivate_t->pStorageList = pList;
		}
		else
		{
				pNode = pPrivate_t->pStorageList;	
				while(pNode->pNext_t != NULL)
				{
						pNode = pNode->pNext_t;
				}
				
				pNode->pNext_t = pList;
		}		
}

bool StorageReadByte(PRIVATE_MEMBER_TYPE *pThisPrivate, StorageParams *pParams_t, StorageByteOptions *pOps_t)
{
		PrivateBlock *pPrivate_t = NULL;
		StorageList *pNode = NULL;
		AT24C512_Control *AT24C512_Control_t = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)pThisPrivate;	
		if(pPrivate_t->pStorageList == NULL)
		{
				return false;
		}
		
		pNode = pPrivate_t->pStorageList;
		while(pNode->pNext_t != NULL)
		{
				if(pParams_t->iID == pNode->iStorageID)
				{
						switch(pParams_t->eStorageDevice)
						{
								case eAT24C512:
									AT24C512_Control_t = (AT24C512_Control *)pNode->pStorage;
									AT24C512_Control_t->m_pAT24C512_ReadByte(AT24C512_Control_t->m_pThisPrivate, pOps_t);
									break;
								default:
									break;
						}
						return true;
				}
		}
		
		return false;
}

bool StorageWriteByte(PRIVATE_MEMBER_TYPE *pThisPrivate, StorageParams *pParams_t, StorageByteOptions *pOps_t)
{
		PrivateBlock *pPrivate_t = NULL;
		StorageList *pNode = NULL;
		AT24C512_Control *AT24C512_Control_t = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)pThisPrivate;	
		if(pPrivate_t->pStorageList == NULL)
		{
				return false;
		}
		
		pNode = pPrivate_t->pStorageList;
		while(pNode->pNext_t != NULL)
		{
				if(pParams_t->iID == pNode->iStorageID)
				{
						switch(pParams_t->eStorageDevice)
						{
								case eAT24C512:
									AT24C512_Control_t = (AT24C512_Control *)pNode->pStorage;
									AT24C512_Control_t->m_pAT24C512_ReadByte(AT24C512_Control_t->m_pThisPrivate, pOps_t);
									break;
								default:
									break;
						}
						return true;
				}
		}
		
		return false;	
}