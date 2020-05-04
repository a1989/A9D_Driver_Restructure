/**/
#include "Communication.h"
#include "DriverUSART.h"
#include "DriverCAN.h"

bool HostDataGet(structRecvBlock *Block_t, uint8_t *pData)
{
		if(!PopRecvBuffer(Block_t, pData))
		{
				return false;
		}
		
		return true;
}

void PushRecvBuffer(structRecvBlock *Block_t, uint8_t *pData)
{
		memcpy(Block_t->iRecvBuffer[Block_t->iWriteIndex], pData, RECV_HOST_DATA_SIZE);
		Block_t->iBufferLen += 1;
		Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % RECV_BUFFER_SIZE;
}

bool PopRecvBuffer(structRecvBlock *Block_t, uint8_t *pData)
{
		if(Block_t->iBufferLen)
		{
				if(NULL == pData)
				{
						return;
				}
				memcpy(pData, Block_t->iRecvBuffer[Block_t->iReadIndex], RECV_HOST_DATA_SIZE);
				Block_t->iBufferLen -= 1;
				Block_t->iReadIndex = (Block_t->iReadIndex + 1) % RECV_BUFFER_SIZE;
		}
}

void WriteSendBuffer(structSendBlock *Block_t, uint8_t *pData)
{
		memcpy(Block_t->iSendBuffer[Block_t->iIndex], pData, SEND_HOST_DATA_SIZE);
		Block_t->iIndex = (Block_t->iIndex + 1) % SEND_BUFFER_SIZE;
}

void HostDataSend()
{
		
}
