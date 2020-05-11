/**/
#include "Communication.h"
#include "DriverUSART.h"
#include "DriverCAN.h"

//主要的数据接收是通过CAN, 串口用于Debug

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
						return false;
				}
				memcpy(pData, Block_t->iRecvBuffer[Block_t->iReadIndex], RECV_HOST_DATA_SIZE);
				Block_t->iBufferLen -= 1;
				Block_t->iReadIndex = (Block_t->iReadIndex + 1) % RECV_BUFFER_SIZE;
		}
}

bool HostDataGet(CommunicationBlock *Block_t, uint8_t *pData)
{
		if(NULL == Block_t)
		{
				return false;
		}
		
		if(!PopRecvBuffer(Block_t->RecvBlock_t, pData))
		{
				return false;
		}
		
		return true;
}

//写缓冲区操作
void CAN_WriteSendBuffer(structSendBlock *Block_t, uint8_t *pData, uint8_t iDataLen)
{
		memcpy(Block_t->SendBuffer_t[Block_t->iWriteIndex].arrSendBuffer, pData, iDataLen);
		Block_t->SendBuffer_t[Block_t->iWriteIndex].iDataLen = iDataLen;
		Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % SEND_BUFFER_SIZE;
		Block_t->iBufferLen++;
}

//下位机数据写入缓冲区中
void WriteSendBuffer(CommunicationBlock *Block_t, uint8_t *pData, uint8_t iDataLen)
{
		if(NULL == Block_t)
		{
				return;
		}
		
		CAN_WriteSendBuffer(&Block_t->SendBlock_t, pData, iDataLen);	
}

//下位机数据发送
void SlaveDataSend(CommunicationBlock *Block_t)
{
		if(NULL == Block_t)
		{
				return;
		}
		
		CAN_SendConfig(&Block_t->SendBlock_t.SendBuffer_t[Block_t->SendBlock_t.iSendIndex]);		
}
