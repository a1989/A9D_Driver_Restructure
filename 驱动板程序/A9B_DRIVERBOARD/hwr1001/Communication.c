/**/
#include "Communication.h"
#include "DriverUSART.h"
#include "DriverCAN.h"

//主要的数据接收是通过CAN, 串口用于Debug

#define RECV_TYPE_NAME	"RecvTypeDef"
#define SEND_TYPE_NAME	"SendTypeDef"

/*本模块私有数据类型定义*/
typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		uint8_t iRecvBuffer[RECV_BUFFER_SIZE][RECV_HOST_DATA_SIZE];
		uint8_t iCache[RECV_HOST_DATA_SIZE];
		char *strType;
}StructRecvHandler;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iSendIndex;
		uint8_t SendBuffer_t[SEND_BUFFER_SIZE];
		char *strType;
}StructSendHandler;
/**/

StructRecvHandler *MallocRecv_t(void)
{
		StructRecvHandler *pRecv_t = (StructRecvHandler*)malloc(sizeof(StructRecvHandler));
		pRecv_t->iBufferLen = 0;
		pRecv_t->iReadIndex = 0;
		pRecv_t->iWriteIndex = 0;
		pRecv_t->strType = RECV_TYPE_NAME;
	
		return pRecv_t;
}

StructSendHandler *MallocSend_t(void)
{
		StructSendHandler *pSend_t = (StructSendHandler*)malloc(sizeof(StructSendHandler));
		pSend_t->iBufferLen = 0;
		pSend_t->iWriteIndex = 0;
		pSend_t->strType = SEND_TYPE_NAME;
	
		return pSend_t;
}

bool CommunicationBlockInit(CommunicationBlock *Block_t)
{
		Block_t->RecvBlock_t.m_pThisPrivate = MallocRecv_t();
		Block_t->SendBlock_t.m_pThisPrivate = MallocSend_t();
}

void PushRecvBuffer(StructRecvHandler *Block_t, uint8_t *pData)
{
		memcpy(Block_t->iRecvBuffer[Block_t->iWriteIndex], pData, RECV_HOST_DATA_SIZE);
		Block_t->iBufferLen += 1;
		Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % RECV_BUFFER_SIZE;
}

bool PopRecvBuffer(StructRecvHandler *Block_t, uint8_t *pData)
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

bool HostDataGet(PRIVATE_MEMBER_TYPE *Block_t, CAN_TypeDef *CAN_t)
{
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return false;
		}
		
		StructRecvHandler *RecvBlock_t = (StructRecvHandler*)Block_t;
		strcmp(RecvBlock_t->strType, RECV_TYPE_NAME)
		{
				printf("\r\nfunc:%s, error:Parameter Type", __FUNCTION__);
				return false;
		}
		
		return true;
}

//写缓冲区操作
void CAN_WriteSendBuffer(StructSendHandler *Block_t, uint8_t *pData, uint8_t iDataLen)
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
