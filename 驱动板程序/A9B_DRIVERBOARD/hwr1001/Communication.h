#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "defines.h"

typedef enum
{
		HEARTBEAT = 0,
		ASSEMBLE,
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

typedef enum
{
		eCAN1 = 0,
		eCAN2
}CommunicationType;

typedef struct
{
		CommunicationType eType;
		//void参数类型需要针对于不同的通信方式
		void *pParam;
}CommunicationParams;


typedef struct 
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;	
		VOID_HandleTypeDef *hHandler;
		bool (*m_pGetHostData)(PRIVATE_MEMBER_TYPE *m_pPrivate);
		void (*m_pSlaveDataPrepare)(PRIVATE_MEMBER_TYPE *m_pPrivate, uint8_t *pData, uint8_t iDataLen);
		CommunicationParams *(*m_pGetConfigParams)(PRIVATE_MEMBER_TYPE *Block_t);
		bool (*m_pPopMessage)(PRIVATE_MEMBER_TYPE *m_pPrivate, uint8_t *pData, uint8_t *iDataLen, bool *bAvailable);
		void (*m_pExeBlock)(PRIVATE_MEMBER_TYPE *m_pPrivate);
}CommunicationBlock;

//初始化通信控制块
bool CommunicationBlockInit(CommunicationBlock *Block_t, CommunicationParams Params_t);

#endif
