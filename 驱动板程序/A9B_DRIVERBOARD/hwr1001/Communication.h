#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "defines.h"

typedef enum
{
		HEARTBEAT = 0,
		QUERY,
		CMD,
		UPDATE,
		Error
}RecvDataType;


typedef enum
{
		REALTIME_LOCATION = 1,
		REALTIME_SPEED,
		LIMIT_STATUS,
		REALTIME_CURRENT,
		MAX_LEN,
		VERSION,
		MAX_CURRENT,
		SUBDIVISION
}QueryDataObj;


typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		bool (*m_pHostDataGet)(PRIVATE_MEMBER_TYPE *m_pPrivate, CAN_TypeDef *CAN_t, uint8_t *pData, uint8_t iDataLen);
		uint16_t (*m_pCAN_GetStdID)(void);
}structRecvBlock;


typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
}structSendBlock;


typedef struct 
{
		structRecvBlock RecvBlock_t;
		structSendBlock SendBlock_t;		
}CommunicationBlock;

bool CommunicationBlockInit(CommunicationBlock *Block_t);

#endif
