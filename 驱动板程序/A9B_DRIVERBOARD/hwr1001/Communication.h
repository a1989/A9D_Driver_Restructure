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
		uint8_t iID;
		//void参数类型需要针对于不同的通信方式
		void *pParam;
}CommunicationParams;


typedef struct 
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;	

		void (*m_pAddCommunicationInterface)(PRIVATE_MEMBER_TYPE *m_pPrivate, CommunicationParams Params_t);
		bool (*m_pGetHostData)(PRIVATE_MEMBER_TYPE *Block_t, uint8_t *pData, uint8_t iLen, CommunicationType eType);
		void (*m_pSlaveDataPrepare)(PRIVATE_MEMBER_TYPE *m_pPrivate, uint8_t *pData, uint8_t iDataLen, CommunicationType eType);
		void *(*m_pGetInterfaceHandle)(PRIVATE_MEMBER_TYPE *m_pPrivate, CommunicationType eType);
		//
		void *(*m_pGetInterfaceConfig)(PRIVATE_MEMBER_TYPE *m_pPrivate, CommunicationType eType);
		void (*m_pPopMessage)(PRIVATE_MEMBER_TYPE *m_pPrivate, uint8_t *pData, uint8_t *iDataLen, bool *bAvailable);
		void (*m_pExeBlock)(PRIVATE_MEMBER_TYPE *m_pPrivate);
}CommunicationBlock;

//初始化通信控制块
bool CommunicationBlockInit(CommunicationBlock *Block_t);

#endif
