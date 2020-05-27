#include "DriverCAN.h"
#include "can.h"

#define CAN_TYPE_NAME		"CAN_TypeDef"

typedef struct
{
		CAN_HandleTypeDef hCAN;
		CanTxMsgTypeDef TxMessage;
		CanRxMsgTypeDef RxMessage;
		uint8_t iStdID;
		uint8_t arrSendBuffer[8];
		uint8_t iDataLen;
		void (*m_pSendConfig)();
		char *strType;
		bool bDataReceived;
}CAN_Handler;


CAN_Handler *CAN_MallocBlock(CAN_TypeDef *CAN_t, uint32_t iStdID)
{
		CAN_Handler *pCAN_Handler = (CAN_Handler*)malloc(sizeof(CAN_Handler));
	
		MX_CAN_Init(pCAN_Handler->hCAN, CAN_t);
		pCAN_Handler->iStdID = iStdID;
		pCAN_Handler->hCAN.pRxMsg = &pCAN_Handler->RxMessage;
		pCAN_Handler->hCAN.pTxMsg = &pCAN_Handler->TxMessage;
		if (HAL_CAN_Init (&pCAN_Handler->hCAN) != HAL_OK)
		{
				printf("\r\nFunc:%s,Error:CAN Init Failed", __FUNCTION__);
				Error_Handler();
		}		
}

//ÅäÖÃCAN¹ýÂËÆ÷
void CAN_FilterConfig(CAN_Handler *CAN_t)
{
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

void DriverCAN_Init(CAN_Block *Block_t, CAN_TypeDef *CAN_t, uint32_t iStdID)
{		
		Block_t->pThisPrivate = CAN_MallocBlock(CAN_t, iStdID);
		CAN_FilterConfig((CAN_Handler *)Block_t->pThisPrivate);
}

//ÅäÖÃCAN·¢ËÍÄÚÈÝ
static void CAN_SendConfig(PRIVATE_MEMBER_TYPE *pThis)
{
		CAN_Handler *Block_t = (CAN_Handler *)pThis;
		if(!IS_DATA_TYPE_CORRECT(Block_t->strType, CAN_TYPE_NAME))
		{
				printf("\r\nFunc:%s,Error:Param Error", __FUNCTION__);
				return;
		}
		
		Block_t->TxMessage.StdId = Block_t->iStdID;
		Block_t->TxMessage.IDE = CAN_ID_STD;
		Block_t->TxMessage.RTR = CAN_RTR_DATA;
		Block_t->TxMessage.DLC = Block_t->iDataLen;
		memcpy(Block_t->TxMessage.Data, Block_t->arrSendBuffer, Block_t->iDataLen);
		__HAL_UNLOCK (&Block_t->hCAN);
		HAL_CAN_Transmit_IT (&Block_t->hCAN);
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