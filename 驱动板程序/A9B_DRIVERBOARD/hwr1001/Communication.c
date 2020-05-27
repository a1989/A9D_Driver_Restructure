/**/
#include "Communication.h"
#include "DriverUSART.h"
#include "DriverCAN.h"

//主要的数据接收是通过CAN, 串口用于Debug

#define RECV_TYPE_NAME	"RecvTypeDef"
#define SEND_TYPE_NAME	"SendTypeDef"
#define PARAMS_TYPE_NAME		"ParamsTypeDef"

/*本模块私有数据和类型定义*/
typedef struct
{
		uint8_t iBuffer[RECV_HOST_DATA_SIZE];
		uint8_t iDataLen;
}StructDataBuffer;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		StructDataBuffer iRecvBuffer_t[RECV_BUFFER_SIZE];
		StructDataBuffer iCache;
		char *strType;
}StructRecvHandler;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iSendIndex;
		StructDataBuffer iSendBuffer_t[SEND_BUFFER_SIZE];
		char *strType;
}StructSendHandler;

typedef struct
{
		CommunicationParams Params_t;
		void *pInterface;
		char *strType;
		StructRecvHandler *RecvBlock_t;
		StructSendHandler *SendBlock_t;
}PrivateBlock;

/**/

static void PushRecvBuffer(StructRecvHandler *Block_t, uint8_t *pData, uint8_t iDataLen)
{
		memcpy(Block_t->iRecvBuffer_t[Block_t->iWriteIndex].iBuffer, pData, iDataLen);
		Block_t->iRecvBuffer_t[Block_t->iWriteIndex].iDataLen = iDataLen;
		Block_t->iBufferLen++;
		Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % RECV_BUFFER_SIZE;
}

static void PushSendBuffer(StructSendHandler *Block_t, uint8_t *pData, uint8_t iDataLen)
{
		memcpy(Block_t->iSendBuffer_t[Block_t->iWriteIndex].iBuffer, pData, iDataLen);
		Block_t->iSendBuffer_t[Block_t->iWriteIndex].iDataLen = iDataLen;
		Block_t->iBufferLen++;
		Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % SEND_BUFFER_SIZE;
}

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

bool CommunicationBlockInit(CommunicationBlock *Block_t, CommunicationParams Params_t)
{
		PrivateBlock *pParamsBlock = NULL;
		//定义一个指向具体通信方式模块的指针,如果有其他的通信方式可以继续定义
		CAN_Block *CAN_t = NULL;
		
		pParamsBlock = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		pParamsBlock->strType = PARAMS_TYPE_NAME;
		pParamsBlock->RecvBlock_t = MallocRecv_t();
		pParamsBlock->SendBlock_t = MallocSend_t();
	
		switch(Params_t.eType)
		{
			case eCAN1:
				//通信接口指向CAN
				CAN_t = (CAN_Block *)malloc(sizeof(CAN_Block));
				pParamsBlock->pInterface = CAN_t;	
				//保存配置参数
				memcpy(&pParamsBlock->Params_t, &Params_t, sizeof(Params_t));
				//私有变量赋值
				Block_t->m_pThisPrivate = pParamsBlock;	
				//初始化CAN
				DriverCAN_Init(CAN_t->m_pThisPrivate, CAN1, *((uint32_t *)Params_t.pParam));
				Block_t->hHandler = CAN_t->m_pCAN_GetHandler(CAN_t->m_pThisPrivate);				
				break;
			default:
				break;
		}
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

//获取此通信模块的通信参数
CommunicationParams *GetConfigParams(PRIVATE_MEMBER_TYPE *Block_t)
{
		static PrivateBlock *Private_t = NULL;
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return false;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//形参类型检查
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return false;
		}		
		
		return &Private_t->Params_t;
}

//从主机获取消息并将消息存入消息缓冲区
bool GetHostData(PRIVATE_MEMBER_TYPE *Block_t)
{
		static PrivateBlock *Private_t = NULL;
		CAN_Block *CAN_t = NULL;
		uint8_t iData[8] = {0};
		uint8_t iDataLen = 0;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return false;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//形参类型检查
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return false;
		}
		
		switch(Private_t->Params_t.eType)
		{
			case eCAN1:
				CAN_t = (CAN_Block *)Private_t->pInterface;
				CAN_t->m_pGetData(CAN_t->m_pThisPrivate, iData, &iDataLen);
				PushRecvBuffer(Private_t->RecvBlock_t, iData, iDataLen);
				break;
			default:
				break;
		}
		
		return true;
}

//配置下位机向主机发送的数据, 将数据存入发送缓冲区
void SlaveDataPrepare(PRIVATE_MEMBER_TYPE *Block_t, uint8_t *pData, uint8_t iDataLen)
{
		static PrivateBlock *Private_t = NULL;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//形参类型检查
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return;
		}
		
		switch(Private_t->Params_t.eType)
		{
			case eCAN1:
				PushSendBuffer(Private_t->SendBlock_t, pData, iDataLen);
				break;
			default:
				break;
		}	
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


