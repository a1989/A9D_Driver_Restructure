/**/
#include "Communication.h"
#include "DriverUSART.h"
#include "DriverCAN.h"
#include <stdlib.h>

//主要的数据接收是通过CAN, 串口用于Debug

#define RECV_TYPE_NAME	"RecvTypeDef"
#define SEND_TYPE_NAME	"SendTypeDef"
#define PARAMS_TYPE_NAME		"ParamsTypeDef"

/*本模块私有数据和类型定义*/
typedef struct
{
		CommunicationType eType;
		uint8_t iBuffer[RECV_HOST_DATA_SIZE];
		uint8_t iDataLen;
}StructDataBuffer;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		StructDataBuffer iRecvBuffer_t[RECV_BUFFER_SIZE];
		char *strType;
}StructRecvHandler;

typedef struct
{
		uint8_t iBufferLen;
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		StructDataBuffer iSendBuffer_t[SEND_BUFFER_SIZE];
		char *strType;
}StructSendHandler;

typedef struct structList
{
		uint8_t iID;
		CommunicationParams Params_t;	
		void *pInterface;
		struct structList *pNext_t;
}InterfaceList;

typedef struct
{
		InterfaceList *pList;	
		char *strType;
		StructRecvHandler *RecvBlock_t;
		StructSendHandler *SendBlock_t;
}PrivateBlock;

/**/

static void PushRecvBuffer(StructRecvHandler *Block_t, uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
{
		DEBUG_LOG("\r\nrecv buffer start push data:length:%d", iDataLen)
		
		if(Block_t->iBufferLen < RECV_BUFFER_SIZE - 1)
		{				
				memcpy(Block_t->iRecvBuffer_t[Block_t->iWriteIndex].iBuffer, pData, iDataLen);
				Block_t->iRecvBuffer_t[Block_t->iWriteIndex].iDataLen = iDataLen;
				Block_t->iRecvBuffer_t[Block_t->iWriteIndex].eType = eType;
				Block_t->iBufferLen++;
				Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % RECV_BUFFER_SIZE;
			
				DEBUG_LOG("\r\nrecv buffer:length:%d,write index:%d\r\n", iDataLen, Block_t->iWriteIndex)
		}
		else
		{
				DEBUG_LOG("\r\nrecv buffer:full\r\n")
		}
}

static void PushSendBuffer(StructSendHandler *Block_t, uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
{
		DEBUG_LOG("\r\nsend buffer start push data:length:%d", iDataLen)
		
		if(Block_t->iBufferLen < SEND_BUFFER_SIZE - 1)
		{
				memcpy(Block_t->iSendBuffer_t[Block_t->iWriteIndex].iBuffer, pData, iDataLen);
				Block_t->iSendBuffer_t[Block_t->iWriteIndex].iDataLen = iDataLen;
				Block_t->iSendBuffer_t[Block_t->iWriteIndex].eType = eType;
				Block_t->iBufferLen++;
				Block_t->iWriteIndex = (Block_t->iWriteIndex + 1) % SEND_BUFFER_SIZE;
			
				DEBUG_LOG("\r\nrecv buffer:length:%d,write index:%d\r\n", iDataLen, Block_t->iWriteIndex)
		}
		else
		{
				DEBUG_LOG("\r\nrecv buffer:full\r\n")
		}
}

StructRecvHandler *MallocRecv_t(void)
{
		StructRecvHandler *pRecv_t = (StructRecvHandler*)malloc(sizeof(StructRecvHandler));
		if(NULL == pRecv_t)
		{
				printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
				return false;							
		}
		
		pRecv_t->iBufferLen = 0;
		pRecv_t->iReadIndex = 0;
		pRecv_t->iWriteIndex = 0;
		pRecv_t->strType = RECV_TYPE_NAME;
	
		return pRecv_t;
}

StructSendHandler *MallocSend_t(void)
{
		StructSendHandler *pSend_t = (StructSendHandler*)malloc(sizeof(StructSendHandler));
		if(NULL == pSend_t)
		{
				printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
				return false;							
		}
		
		pSend_t->iBufferLen = 0;
		pSend_t->iReadIndex = 0;
		pSend_t->iWriteIndex = 0;
		pSend_t->strType = SEND_TYPE_NAME;
	
		return pSend_t;
}

void AddCommunicationInterface(PRIVATE_MEMBER_TYPE *Block_t, CommunicationParams Params_t)
{
		InterfaceList *pList = NULL;
		InterfaceList *pNode = NULL;
		PrivateBlock *pPrivate_t = NULL;
		//定义一个指向具体通信方式模块的指针,如果有其他的通信方式可以继续定义
		CAN_Block *CAN_t = NULL;
		
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		
		
		pPrivate_t = (PrivateBlock *)Block_t;
		
		switch(Params_t.eType)
		{
			case eCAN1:
				//通信接口指向CAN
				printf("\r\nstart init CAN1");
				CAN_t = (CAN_Block *)malloc(sizeof(CAN_Block));
				if(NULL == CAN_t)
				{
						printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
						return;							
				}
				//保存配置参数
				//私有变量赋值
				//初始化CAN
				
				DriverCAN_Init(CAN_t, CAN1, *((uint32_t *)Params_t.pParam));			
				break;
			default:
				break;
		}		
		
		pList = (InterfaceList *)malloc(sizeof(InterfaceList));
		if(NULL == pList)
		{				
				return;
		}
		
		pList->pInterface = CAN_t;
		pList->iID = Params_t.iID;
		pList->pNext_t = NULL;
		
		if(NULL == pPrivate_t->pList)
		{
				pPrivate_t->pList = pList;
		}
		else
		{
				pNode = pPrivate_t->pList;	
				while(pNode->pNext_t != NULL)
				{
						pNode = pNode->pNext_t;
				}
				
				pNode->pNext_t = pList;
		}		
}

//从主机获取消息并将消息存入到消息缓冲区
bool GetHostData(PRIVATE_MEMBER_TYPE *Block_t, uint8_t *pData, uint8_t iLen, CommunicationType eType)
{
		static PrivateBlock *Private_t = NULL;
		CAN_Block *CAN_t = NULL;
	
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
		
		PushRecvBuffer(Private_t->RecvBlock_t, pData, iLen, eType);
		
		return true;
}

void *GetInterfaceHandle(PRIVATE_MEMBER_TYPE *m_pPrivate, CommunicationType eType)
{
		PrivateBlock *Private_t = NULL;
		InterfaceList *pNode = NULL;
		CAN_Block *CAN_t = NULL;
	
		if(NULL == m_pPrivate)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return NULL;
		}		
		
		Private_t = (PrivateBlock *)m_pPrivate;
		
		pNode = Private_t->pList;
		
		do
		{
				if(eType == pNode->Params_t.eType)
				{
						switch(eType)
						{
							case eCAN1:
								CAN_t = (CAN_Block *)pNode->pInterface;
								return CAN_t->m_pCAN_GetHandler(CAN_t->m_pThisPrivate);
								break;
							case eCAN2:
								break;
							default:
								break;
						}
				}
				pNode = pNode->pNext_t;
		}while(pNode != NULL);
		
		DEBUG_LOG("\r\nError:return NULL handler")
		return NULL;
}

//从消息接收缓冲区中获取一条消息
void PopMessage(PRIVATE_MEMBER_TYPE *pPrivate, uint8_t *pData, uint8_t *iDataLen, bool *bAvailable)
{
		PrivateBlock *Private_t = NULL;
		*bAvailable = false;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return;
		}
		
		Private_t = (PrivateBlock *)pPrivate;

		//形参类型检查
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return;
		}		
		
		//只有在buffer长度不为0的时候才能拷贝数据
		if(Private_t->RecvBlock_t->iBufferLen)
		{
				if(NULL == pData)
				{
						printf("\r\nfunc:%s, error:pData Null", __FUNCTION__);
						return;
				}
				
				DEBUG_LOG("\r\nstart pop message")
				memcpy(pData, Private_t->RecvBlock_t->iRecvBuffer_t[Private_t->RecvBlock_t->iReadIndex].iBuffer, 
											Private_t->RecvBlock_t->iRecvBuffer_t[Private_t->RecvBlock_t->iReadIndex].iDataLen);
				*iDataLen = Private_t->RecvBlock_t->iRecvBuffer_t[Private_t->RecvBlock_t->iReadIndex].iDataLen;
				
				Private_t->RecvBlock_t->iBufferLen--;
				Private_t->RecvBlock_t->iReadIndex = (Private_t->RecvBlock_t->iReadIndex + 1) % RECV_BUFFER_SIZE;
				*bAvailable = true;			
		}
}

void RefreshMessageBuffer(PRIVATE_MEMBER_TYPE *pPrivate)
{
		
}

//配置下位机向主机发送的数据, 将数据存入发送缓冲区
void SlaveDataPrepare(PRIVATE_MEMBER_TYPE *Block_t, uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
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
		
//		for(int i = 0; i < iDataLen; i++)
//		{
//				DEBUG_LOG("\r\n%x", *(pData + i))
//		}
		
		PushSendBuffer(Private_t->SendBlock_t, pData, iDataLen, eType);
}

InterfaceList *FindInterfaceNode(InterfaceList *pNode, CommunicationType eType)
{
		CAN_Block *CAN_t = NULL;
	
		if(NULL == pNode)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return NULL;			
		}
		
		do
		{
				if(eType == pNode->Params_t.eType)
				{
						return pNode;
				}
				pNode = pNode->pNext_t;
		}while(pNode != NULL);
		
		return NULL;
}

static void SendMessage(PRIVATE_MEMBER_TYPE *Block_t)
{
		PrivateBlock *Private_t = NULL;
		InterfaceList *pNode = NULL;
		CAN_Block *CAN_t = NULL;
	
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
		
		if(Private_t->SendBlock_t->iBufferLen)
		{				
				switch(Private_t->SendBlock_t->iSendBuffer_t[Private_t->SendBlock_t->iReadIndex].eType)
				{
						case eCAN1:
							DEBUG_LOG("\r\nfind CAN1 node")
							pNode = FindInterfaceNode(Private_t->pList, eCAN1);
							if(NULL == pNode)
							{
									printf("\r\nfunc:%s, error:can not find node", __FUNCTION__);
									return;									
							}
							CAN_t = (CAN_Block *)pNode->pInterface;
							CAN_t->m_pSendData(CAN_t->m_pThisPrivate, 
																	Private_t->SendBlock_t->iSendBuffer_t[Private_t->SendBlock_t->iReadIndex].iBuffer,
																	Private_t->SendBlock_t->iSendBuffer_t[Private_t->SendBlock_t->iReadIndex].iDataLen);
							break;
						case eCAN2:
							break;
						default:
							break;
				}
				Private_t->SendBlock_t->iBufferLen--;
				Private_t->SendBlock_t->iReadIndex = (Private_t->SendBlock_t->iReadIndex + 1) % SEND_BUFFER_SIZE;
		}		
}

//执行本模块
void ExeBlock(PRIVATE_MEMBER_TYPE *Block_t)
{
		SendMessage(Block_t);
}

bool CommunicationBlockInit(CommunicationBlock *Block_t)
{
		PrivateBlock *pBlock_t = NULL;
		//定义一个指向具体通信方式模块的指针,如果有其他的通信方式可以继续定义
		CAN_Block *CAN_t = NULL;
		
		pBlock_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		if(NULL == pBlock_t)
		{
				printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
				return false;							
		}
		pBlock_t->strType = PARAMS_TYPE_NAME;
		pBlock_t->RecvBlock_t = MallocRecv_t();
		pBlock_t->SendBlock_t = MallocSend_t();
		pBlock_t->pList = NULL;
		
		Block_t->m_pThisPrivate = pBlock_t;
		Block_t->m_pAddCommunicationInterface = AddCommunicationInterface;
		Block_t->m_pGetHostData = GetHostData;
		Block_t->m_pGetInterfaceHandle = GetInterfaceHandle;
		Block_t->m_pPopMessage = PopMessage;
		Block_t->m_pSlaveDataPrepare = SlaveDataPrepare;
		Block_t->m_pExeBlock = ExeBlock;
		
		DEBUG_LOG("\r\nDBG Communication Block Prepare")
}



void *GetInterfaceConfig(PRIVATE_MEMBER_TYPE *m_pPrivate, CommunicationType eType)
{
		PrivateBlock *Private_t = NULL;
		InterfaceList *pNode = NULL;
		
	
		if(NULL == m_pPrivate)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return NULL;
		}		
		
		Private_t = (PrivateBlock *)m_pPrivate;
		
		pNode = Private_t->pList;
		
//		do
//		{
//				if(eType == pNode->Params_t.eType)
//				{
//						switch(eType)
//						{
//							case eCAN1:
//								CAN_t = (CAN_Block *)pNode->pInterface;
//								return CAN_t->m_pCAN_GetStdID(CAN_t->m_pThisPrivate);
//								break;
//							case eCAN2:
//								break;
//							default:
//								break;
//						}
//				}
//				pNode = pNode->pNext_t;
//		}while(pNode != NULL);
//		
//		return NULL;
		return FindInterfaceNode(pNode, eType);
}





//获取此通信模块的通信参数
//CommunicationParams *GetConfigParams(PRIVATE_MEMBER_TYPE *Block_t)
//{
//		static PrivateBlock *Private_t = NULL;
//		if(NULL == Block_t)
//		{
//				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
//				return false;
//		}
//		
//		Private_t = (PrivateBlock *)Block_t;

//		//形参类型检查
//		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
//		{
//				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
//				return false;
//		}		
//		
//		return &Private_t->Params_t;
//}




