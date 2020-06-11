#include "IncEncoderControl.h"
#include "stdlib.h"
#include "tim.h"

typedef struct
{
		int32_t iCurrentCount;
		int32_t iTargetCount;
		int8_t iDir;
		uint32_t iCountPerRound;
		TIM_HandleTypeDef hTIM;
		int32_t iRoundCount;
}PrivateBlock;

IncEncoderTableInt IncEncoderTableInt_t;

bool GetEncoderValue(PRIVATE_MEMBER_TYPE *m_pThisPrivate, int32_t *iValue)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				
						
		pPrivate_t->iCurrentCount = pPrivate_t->iRoundCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&pPrivate_t->hTIM);
		*iValue = pPrivate_t->iCurrentCount;
		
		return true;
}

bool RegisterEncoderVar(PRIVATE_MEMBER_TYPE *pThisPrivate, IncEncoderTableInt *IncEncoderTable_t)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pThisPrivate;
		IncEncoderTableInt *pList = NULL;
		IncEncoderTableInt *pNode = NULL;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}			
		
		pNode = (IncEncoderTableInt *)malloc(sizeof(IncEncoderTableInt));
		if(NULL == pNode)
		{
				printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
				return false;
		}
		
		DEBUG_LOG("\r\nDBG register inc encoder to Int")
		
		pNode->pNext = NULL;
		pNode->m_pThisPrivate = pThisPrivate;
				
		if(NULL == IncEncoderTable_t)
		{
				//DEBUG_LOG("\r\nfirst register")
				IncEncoderTable_t = pNode;
		}
		else
		{
				pList = IncEncoderTable_t;
			
				while(pList->pNext != NULL)
				{
						pList = pList->pNext;
				}
				
				pList->pNext = pNode;
		}		
//		DEBUG_LOG("\r\nDBG end register inc encoder to Int")
}

void IncEncoderIntHandler(PRIVATE_MEMBER_TYPE *m_pThisPrivate, TIM_HandleTypeDef *htim)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}
		
		if(htim->Instance == pPrivate_t->hTIM.Instance)
		{
				if (pPrivate_t->hTIM.Instance->CR1 & 0x0010) 
				{
						pPrivate_t->iRoundCount -= 1;
				}
				else
				{
						pPrivate_t->iRoundCount += 1;
				}				
		}
}

bool IncEncoderTargetArrived(PRIVATE_MEMBER_TYPE *m_pThisPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}
		
		if(1 == pPrivate_t->iDir)
		{
				if(pPrivate_t->iCurrentCount >= pPrivate_t->iTargetCount)
				{
						return true;
				}
		}
		else if(-1 == pPrivate_t->iDir)
		{
				if(pPrivate_t->iCurrentCount <= pPrivate_t->iTargetCount)
				{
						return true;
				}				
		}			
}

void IncEncoderControlInit(IncEncoderControl *Block_t, EncoderParmas *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
		if(NULL == Block_t || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		
		
		pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
				return;					
		}
		
		DEBUG_LOG("\r\nDBG Start init inc encoder")
		switch(Params_t->eEncoderTIM)
		{
				case eTIM3:
					MX_TIM_Encoder_Init(&pPrivate_t->hTIM, TIM3);
					break;
				default:
					break;
		}
		
		pPrivate_t->iCountPerRound = Params_t->iEncoderLines * Params_t->iMultiplication;		
		
		Block_t->m_pGetEncoderValue = GetEncoderValue;
		Block_t->m_pIncEncoderTargetArrived = IncEncoderTargetArrived;
		Block_t->m_pThisPrivate = pPrivate_t;
		
		RegisterEncoderVar(pPrivate_t, &IncEncoderTableInt_t);
		
		DEBUG_LOG("\r\nDBG inc encoder init success")
}

