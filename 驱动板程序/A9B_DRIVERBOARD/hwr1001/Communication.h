#ifndef __COMMUNICATION_H__
#define __COMMUNICATION_H__

#include "DriverBoardConfig.h"
#include "DriverCAN.h"

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
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		uint8_t iRecvBuffer[RECV_BUFFER_SIZE][RECV_HOST_DATA_SIZE];
		uint8_t iCache[RECV_HOST_DATA_SIZE];
}structRecvBlock;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iSendIndex;
		CAN_SendBlock SendBuffer_t[SEND_BUFFER_SIZE];
}structSendBlock;

typedef struct 
{
		structRecvBlock RecvBlock_t;
		structSendBlock SendBlock_t;
}CommunicationBlock;


bool HostDataGet(CommunicationBlock *Block_t, uint8_t *pData);
void WriteSendBuffer(CommunicationBlock *Block_t, uint8_t *pData, uint8_t iDataLen);
void SlaveDataSend(CommunicationBlock *Block_t);
bool CommunicationBlockInit(CommunicationBlock *Block_t);

#endif
