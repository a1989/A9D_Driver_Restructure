#include "IncEncoderControl.h"
#include "stdlib.h"
#include "tim.h"
#include "timer.h"
#include "encoder.h"

typedef struct
{
		int32_t iCurrentCount;
		int32_t iTargetCount;
		bool bCountUp;
		uint32_t iCountPerRound;
		TIM_HandleTypeDef hTIM;
		int32_t iPeriodCount;
		float fPitch;
}PrivateBlock;

IncEncoderTableInt *IncEncoderTableInt_t = NULL;

bool SetEncoderTarget(PRIVATE_MEMBER_TYPE *m_pThisPrivate, float fTarget)
{
		float fRound;
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				

		fRound = fTarget / pPrivate_t->fPitch;
		pPrivate_t->iTargetCount = pPrivate_t->iCountPerRound * fRound;
		DEBUG_LOG("\r\nDBG set encoder cur %d", pPrivate_t->iCurrentCount)
		DEBUG_LOG("\r\nDBG set encoder tar %d", pPrivate_t->iTargetCount)
		pPrivate_t->bCountUp = (pPrivate_t->iTargetCount > pPrivate_t->iCurrentCount) ? true : false;
				
		return true;
}

bool SetEncoderValuef(PRIVATE_MEMBER_TYPE *m_pThisPrivate, float fValue)
{
		float fRound;
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				

		pPrivate_t->iCurrentCount = fValue / pPrivate_t->fPitch * pPrivate_t->iCountPerRound;
		pPrivate_t->iPeriodCount = pPrivate_t->iCurrentCount / 0xFFFF;
		
		__HAL_TIM_SET_COUNTER(&htim3, pPrivate_t->iCurrentCount % 0xFFFF);
		
		return true;
}

void FreshEncoderCount(PRIVATE_MEMBER_TYPE *m_pThisPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		

		pPrivate_t->iCurrentCount = pPrivate_t->iPeriodCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&htim3);
}

bool GetEncoderValue(PRIVATE_MEMBER_TYPE *m_pThisPrivate, int32_t *iValue)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				
						
//		pPrivate_t->iCurrentCount = pPrivate_t->iRoundCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&pPrivate_t->hTIM);
		
		pPrivate_t->iCurrentCount = pPrivate_t->iPeriodCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&htim3);
		*iValue = pPrivate_t->iCurrentCount;
		
		return true;
}

bool RegisterEncoderVar(PRIVATE_MEMBER_TYPE *pThisPrivate)
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
				
		if(NULL == IncEncoderTableInt_t)
		{
				IncEncoderTableInt_t = (IncEncoderTableInt *)malloc(sizeof(IncEncoderTableInt));
				if(NULL == IncEncoderTableInt_t)
				{
						printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
						return false;
				}
				
				memcpy(IncEncoderTableInt_t, pNode, sizeof(IncEncoderTableInt));
				free(pNode);
		}
		else
		{
				pList = IncEncoderTableInt_t;
			
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
		
		if(htim->Instance == htim3.Instance)
		{
				if (htim3.Instance->CR1 & 0x0010) 
				{
						pPrivate_t->iPeriodCount -= 1;
				}
				else
				{
						pPrivate_t->iPeriodCount += 1;
				}				
		}
					
//		if(htim->Instance == pPrivate_t->hTIM.Instance)
//		{
//				if (pPrivate_t->hTIM.Instance->CR1 & 0x0010) 
//				{
//						pPrivate_t->iRoundCount -= 1;
//				}
//				else
//				{
//						pPrivate_t->iRoundCount += 1;
//				}				
//		}
}

bool IncEncoderTargetArrived(PRIVATE_MEMBER_TYPE *m_pThisPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}
		
		FreshEncoderCount(pPrivate_t);
		//DEBUG_LOG("\r\nt%d,c%d", pPrivate_t->iTargetCount, pPrivate_t->iCurrentCount)
		if(pPrivate_t->bCountUp)
		{
				
				if(pPrivate_t->iCurrentCount >= pPrivate_t->iTargetCount)
				{						
						return true;
				}
		}
		else
		{
				if(pPrivate_t->iCurrentCount <= pPrivate_t->iTargetCount)
				{
						DEBUG_LOG("\r\nDBG Arrived2")
						return true;
				}				
		}

		return false;
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
					//MX_TIM_Encoder_Init(&pPrivate_t->hTIM, TIM3);
					MX_TIM3_Init();
					break;
				default:
					break;
		}
		
		//ÅäÖÃ±àÂëÆ÷ÏßÊý
		pPrivate_t->iCountPerRound = Params_t->iEncoderLines * Params_t->iMultiplication;	
		//ÅäÖÃ±àÂëÆ÷µ±Ç°Öµ
		pPrivate_t->iCurrentCount = 0;
		pPrivate_t->fPitch = 10;
		
		Block_t->m_pGetEncoderValue = GetEncoderValue;
		Block_t->m_pIncEncoderTargetArrived = IncEncoderTargetArrived;
		Block_t->m_pThisPrivate = pPrivate_t;
		Block_t->m_pSetEncoderTarget = SetEncoderTarget;
		Block_t->m_pSetEncoderValuef = SetEncoderValuef;
		
		RegisterEncoderVar(pPrivate_t);
		
		__HAL_TIM_CLEAR_IT (&htim3, TIM_IT_UPDATE);
		if (HAL_TIM_Base_Start_IT (&htim3) != HAL_OK)
		{
				Error_Handler();
		}
		
		TIM3_Encoder_Switch (1);//¿ª±àÂëÆ÷
		DEBUG_LOG("\r\nDBG inc encoder init success")
}

