/**/
#include "Communication.h"
#include "DriverUSART.h"
#include "DriverCAN.h"

//��Ҫ�����ݽ�����ͨ��CAN, ��������Debug

#define RECV_TYPE_NAME	"RecvTypeDef"
#define SEND_TYPE_NAME	"SendTypeDef"
#define PARAMS_TYPE_NAME		"ParamsTypeDef"

/*��ģ��˽�����ݺ����Ͷ���*/
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
		pSend_t->iReadIndex = 0;
		pSend_t->iWriteIndex = 0;
		pSend_t->strType = SEND_TYPE_NAME;
	
		return pSend_t;
}

bool CommunicationBlockInit(CommunicationBlock *Block_t, CommunicationParams Params_t)
{
		PrivateBlock *pParamsBlock = NULL;
		//����һ��ָ�����ͨ�ŷ�ʽģ���ָ��,�����������ͨ�ŷ�ʽ���Լ�������
		CAN_Block *CAN_t = NULL;
		
		pParamsBlock = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		pParamsBlock->strType = PARAMS_TYPE_NAME;
		pParamsBlock->RecvBlock_t = MallocRecv_t();
		pParamsBlock->SendBlock_t = MallocSend_t();
	
		switch(Params_t.eType)
		{
			case eCAN1:
				//ͨ�Žӿ�ָ��CAN
				CAN_t = (CAN_Block *)malloc(sizeof(CAN_Block));
				pParamsBlock->pInterface = CAN_t;	
				//�������ò���
				memcpy(&pParamsBlock->Params_t, &Params_t, sizeof(Params_t));
				//˽�б�����ֵ
				Block_t->m_pThisPrivate = pParamsBlock;	
				//��ʼ��CAN
				DriverCAN_Init(CAN_t->m_pThisPrivate, CAN1, *((uint32_t *)Params_t.pParam));
				Block_t->hHandler = CAN_t->m_pCAN_GetHandler(CAN_t->m_pThisPrivate);				
				break;
			default:
				break;
		}
}

//����Ϣ���ջ������л�ȡһ����Ϣ
void PopMessage(PRIVATE_MEMBER_TYPE *Block_t, uint8_t *pData, uint8_t *iDataLen, bool *bAvailable)
{
		static PrivateBlock *Private_t = NULL;
		*bAvailable = false;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//�β����ͼ��
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return;
		}		
		
		if(Private_t->RecvBlock_t->iBufferLen)
		{
				if(NULL == pData)
				{
						printf("\r\nfunc:%s, error:pData Null", __FUNCTION__);
						return;
				}
				
				memcpy(pData, Private_t->RecvBlock_t->iRecvBuffer_t[Private_t->RecvBlock_t->iReadIndex].iBuffer, 
											Private_t->RecvBlock_t->iRecvBuffer_t[Private_t->RecvBlock_t->iReadIndex].iDataLen);
				Private_t->RecvBlock_t->iBufferLen--;
				Private_t->RecvBlock_t->iReadIndex = (Private_t->RecvBlock_t->iReadIndex + 1) % RECV_BUFFER_SIZE;
				*bAvailable = true;
		}
}

static void SendMessage(PRIVATE_MEMBER_TYPE *Block_t)
{
		static PrivateBlock *Private_t = NULL;
		CAN_Block *CAN_t = NULL;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//�β����ͼ��
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return;
		}		
		
		if(Private_t->SendBlock_t->iBufferLen)
		{				
				switch(Private_t->Params_t.eType)
				{
						case eCAN1:
							CAN_t = (CAN_Block *)Private_t->pInterface;
							CAN_t->m_pSendData(CAN_t->m_pThisPrivate, Private_t->SendBlock_t->iSendBuffer_t[Private_t->SendBlock_t->iReadIndex].iBuffer, 
																												Private_t->SendBlock_t->iSendBuffer_t[Private_t->SendBlock_t->iReadIndex].iDataLen);
							break;
						default:
							break;
				}
				
				Private_t->SendBlock_t->iBufferLen--;
				Private_t->SendBlock_t->iReadIndex = (Private_t->SendBlock_t->iReadIndex + 1) % SEND_BUFFER_SIZE;
		}		
}

//ִ�б�ģ��
void ExeBlock(PRIVATE_MEMBER_TYPE *Block_t)
{
		SendMessage(Block_t);
}

//��ȡ��ͨ��ģ���ͨ�Ų���
CommunicationParams *GetConfigParams(PRIVATE_MEMBER_TYPE *Block_t)
{
		static PrivateBlock *Private_t = NULL;
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return false;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//�β����ͼ��
		if(!IS_DATA_TYPE_CORRECT(Private_t->strType, PARAMS_TYPE_NAME))
		{
				printf("\r\nfunc:%s, error:Param Type", __FUNCTION__);
				return false;
		}		
		
		return &Private_t->Params_t;
}

//��������ȡ��Ϣ������Ϣ���뵽��Ϣ������
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

		//�β����ͼ��
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

//������λ�����������͵�����, �����ݴ��뷢�ͻ�����
void SlaveDataPrepare(PRIVATE_MEMBER_TYPE *Block_t, uint8_t *pData, uint8_t iDataLen)
{
		static PrivateBlock *Private_t = NULL;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);
				return;
		}
		
		Private_t = (PrivateBlock *)Block_t;

		//�β����ͼ��
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
