#include "DriverCAN.h"
#include "can.h"
#include <stdlib.h>

#define CAN_TYPE_NAME		"CAN_TypeDef"

typedef struct
{
		CAN_HandleTypeDef hCAN;
		CanTxMsgTypeDef TxMessage;
		CanRxMsgTypeDef RxMessage;
		uint32_t iStdID;
		uint8_t arrSendBuffer[8];
		uint8_t iDataLen;
		void (*m_pSendConfig)();
		char *strType;
		bool bDataReceived;
}CAN_Handler;

CAN_Handler *CAN_MallocBlock(CAN_TypeDef *CAN_t, uint32_t iStdID)
{
		CAN_Handler *pCAN_Handler = (CAN_Handler*)malloc(sizeof(CAN_Handler));
	
		if(NULL == pCAN_Handler)
		{
				printf("\r\nFunc:%s,malloc Failed", __FUNCTION__);
		}
		else
		{
				DEBUG_LOG("\r\nstart init CAN bus")
				MX_CAN_Init(&pCAN_Handler->hCAN, CAN_t);
				pCAN_Handler->iStdID = iStdID;
				pCAN_Handler->hCAN.pRxMsg = &pCAN_Handler->RxMessage;
				pCAN_Handler->hCAN.pTxMsg = &pCAN_Handler->TxMessage;
				pCAN_Handler->strType = CAN_TYPE_NAME;
				
				if (HAL_CAN_Init (&pCAN_Handler->hCAN) != HAL_OK)
				{
						printf("\r\nFunc:%s,Error:CAN bus Init Failed", __FUNCTION__);
						Error_Handler();
				}		
		}	
		
		return pCAN_Handler;
}

//配置CAN过滤器
void CAN_FilterConfig(CAN_Handler *CAN_t)
{
		DEBUG_LOG("\r\nDBG start CAN filter config")
		CAN_FilterConfTypeDef  sFilterConfig;
		uint32_t StdId1 = CAN_t->iStdID;
		uint32_t StdId2 = 0x124;
		uint32_t StdId3 = 0x125;
		uint32_t StdId4 = 0x126;

		sFilterConfig.FilterNumber = 1;
		sFilterConfig.FilterMode = CAN_FILTERMODE_IDLIST;
		sFilterConfig.FilterScale = CAN_FILTERSCALE_16BIT;
		sFilterConfig.FilterIdHigh = StdId1 << 5;
		sFilterConfig.FilterIdLow = StdId2 << 5;
		sFilterConfig.FilterMaskIdHigh = StdId3 << 5;
		sFilterConfig.FilterMaskIdLow = StdId4 << 5;
		sFilterConfig.FilterFIFOAssignment = 0;
		sFilterConfig.FilterActivation = ENABLE;
		sFilterConfig.BankNumber = 14;
		if (HAL_CAN_ConfigFilter (&CAN_t->hCAN, &sFilterConfig) != HAL_OK)
		{
			Error_Handler();
		}
}

CAN_HandleTypeDef *CAN_GetHandler(PRIVATE_MEMBER_TYPE *pThis)
{
		CAN_Handler *pHandler = NULL;
		
		pHandler = (CAN_Handler *)pThis;
		if(!IS_DATA_TYPE_CORRECT(pHandler->strType, CAN_TYPE_NAME))
		{
				printf("\r\nFunc:%s,Error:Param Error", __FUNCTION__);
				return NULL;
		}
		
		return &pHandler->hCAN;
}

uint32_t *CAN_GetStdID(PRIVATE_MEMBER_TYPE *pThis)
{
		CAN_Handler *pHandler = NULL;
		
		pHandler = (CAN_Handler *)pThis;
		if(!IS_DATA_TYPE_CORRECT(pHandler->strType, CAN_TYPE_NAME))
		{
				printf("\r\nFunc:%s,Error:Param Error", __FUNCTION__);
				return NULL;
		}
		
		return &pHandler->iStdID;		
}

void SendData(PRIVATE_MEMBER_TYPE *pThis, uint8_t *pData, uint8_t iLen)
{
		CAN_Handler *pHandler = NULL;
		
		pHandler = (CAN_Handler *)pThis;
		if(!IS_DATA_TYPE_CORRECT(pHandler->strType, CAN_TYPE_NAME))
		{
				printf("\r\nFunc:%s,Error:Param Error", __FUNCTION__);
				return;
		}
		DEBUG_LOG("\r\n%d,%d", pHandler->RxMessage.StdId, pHandler->iStdID)

		pHandler->TxMessage.StdId = pHandler->iStdID;
		pHandler->TxMessage.IDE = CAN_ID_STD;
		pHandler->TxMessage.RTR = CAN_RTR_DATA;
		pHandler->TxMessage.DLC = iLen;
	
		DEBUG_LOG("\r\nDBG CAN Prepare Send")
		memcpy(pHandler->TxMessage.Data, pData, iLen);
		__HAL_UNLOCK (&pHandler->hCAN);
		HAL_CAN_Transmit_IT (&pHandler->hCAN);

}

void DriverCAN_Init(CAN_Block *Block_t, CAN_TypeDef *CAN_t, uint32_t iStdID)
{		
		CAN_Handler *pHandler_t = NULL;
	DEBUG_LOG("\r\n%d", iStdID)
		if(NULL == Block_t)
		{
				printf("\r\nFunc:%s,block NULL pointer", __FUNCTION__);
				return;				
		}
		
		Block_t->m_pThisPrivate = CAN_MallocBlock(CAN_t, iStdID);
		pHandler_t = Block_t->m_pThisPrivate;
		
		if(NULL == Block_t->m_pThisPrivate)
		{
				printf("\r\nFunc:%s,CAN Block int failed", __FUNCTION__);
				return;
		}
		
		CAN_FilterConfig((CAN_Handler *)Block_t->m_pThisPrivate);
		
		Block_t->m_pCAN_GetHandler = CAN_GetHandler;
		Block_t->m_pCAN_GetStdID = CAN_GetStdID;
		Block_t->m_pSendData = SendData;
		
		HAL_CAN_Receive_IT (&pHandler_t->hCAN, CAN_FIFO0); //开CAN接收中断
		
		DEBUG_LOG("\r\nDBG CAN Prepare")
}

//配置CAN发送内容
//static void CAN_SendConfig(PRIVATE_MEMBER_TYPE *pThis)
//{
//		CAN_Handler *Block_t = (CAN_Handler *)pThis;
//		if(!IS_DATA_TYPE_CORRECT(Block_t->strType, CAN_TYPE_NAME))
//		{
//				printf("\r\nFunc:%s,Error:Param Error", __FUNCTION__);
//				return;
//		}
//		
//		Block_t->TxMessage.StdId = Block_t->iStdID;
//		Block_t->TxMessage.IDE = CAN_ID_STD;
//		Block_t->TxMessage.RTR = CAN_RTR_DATA;
//		Block_t->TxMessage.DLC = Block_t->iDataLen;
//		memcpy(Block_t->TxMessage.Data, Block_t->arrSendBuffer, Block_t->iDataLen);
//		
//		DEBUG_LOG("\r\nDBG CAN Prepare Send")
//		__HAL_UNLOCK (&Block_t->hCAN);
//		HAL_CAN_Transmit_IT (&Block_t->hCAN);
//}

void GetData(PRIVATE_MEMBER_TYPE *pThis, uint8_t *pData, uint8_t *iLen)
{
		CAN_Handler *pHandler = NULL;
		
		pHandler = (CAN_Handler *)pThis;
		if(!IS_DATA_TYPE_CORRECT(pHandler->strType, CAN_TYPE_NAME))
		{
				printf("\r\nFunc:%s,Error:Param Error", __FUNCTION__);
				return;
		}
		
		if(pHandler->RxMessage.StdId == pHandler->iStdID)
		{
				memcpy(pData, pHandler->RxMessage.Data, pHandler->RxMessage.DLC);
				*iLen = pHandler->RxMessage.DLC;
		}
}

