#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "DriverBoardConfig.h"

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		uint8_t iRecvBuffer[RECV_BUFFER_SIZE][RECV_HOST_DATA_SIZE];
		uint8_t iCache[RECV_HOST_DATA_SIZE];
//		void (*m_PushRecvBuffer)(structSendBlock *Block_t);
//		void (*m_PopRecvBuffer)(structSendBlock *Block_t);
}structRecvBlock;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iIndex;
		uint8_t iSendBuffer[SEND_BUFFER_SIZE][RECV_HOST_DATA_SIZE];
}structSendBlock;

typedef struct 
{
		structRecvBlock RecvBlock_t;
		structSendBlock SendBlock_t;
		bool (*m_HostDataGet)(structRecvBlock *Block_t);
}CommunicationBlock;

#endif