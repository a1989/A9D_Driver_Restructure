#include "IncEncoderControl.h"
#include "stdlib.h"
#include "tim.h"

typedef struct
{
		int32_t iCurrentCount;
		uint32_t iCountPerRound;
		TIM_HandleTypeDef hTIM;
		int32_t iRoundCount;
}PrivateBlock;

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

void RegisterEncoderVar(IncEncoderVar *Var_t, PRIVATE_MEMBER_TYPE *pThisPrivate, void (*pIntHandler)(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *htim))
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}			
		
		Var_t->hTim = &pPrivate_t->hTIM;
		Var_t->pPrivate = pThisPrivate;
		Var_t->m_pHandler = pIntHandler;
}

void IncEncoderInterruptHandler(PRIVATE_MEMBER_TYPE *m_pThisPrivate, TIM_HandleTypeDef *htim)
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
		Block_t->m_pThisPrivate = pPrivate_t;
}
