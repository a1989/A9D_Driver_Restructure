#include "IncEncoderControl.h"
#include "stdlib.h"
#include "tim.h"
#include "timer.h"
#include "encoder.h"
#include <math.h>

typedef struct
{
		int32_t iCurrentCount;
		int32_t iTargetCount;
		bool bCountUp;
		uint32_t iCountPerRound;
		TIM_HandleTypeDef hTIM;
		int32_t iPeriodCount;
		float fPitch;
		int32_t iCountDelta;
		bool bReverseCountDir;
		int32_t iRelativeDistStart;
		int32_t iRelativeDist;
		bool bArrived;
}PrivateBlock;

IncEncoderTableInt *IncEncoderTableInt_t = NULL;

bool IsTargetArrived(PRIVATE_MEMBER_TYPE *m_pThisPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				

		return pPrivate_t->bArrived;
}

bool IsNextPointSet(PRIVATE_MEMBER_TYPE *m_pThisPrivate)
{
		static int32_t iLastTargetCount;
	
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		if(iLastTargetCount == pPrivate_t->iTargetCount)
		{
				return false;
		}
		else
		{
				iLastTargetCount = pPrivate_t->iTargetCount;
				return true;
		}
}

bool SetEncoderTarget(PRIVATE_MEMBER_TYPE *m_pThisPrivate, float fTarget)
{
		float fRound;
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				
		DEBUG_LOG("\r\nDBG encoder target %f", fTarget)
		fRound = fTarget / pPrivate_t->fPitch;
		DEBUG_LOG("\r\nDBG pitch %f", pPrivate_t->fPitch)
		DEBUG_LOG("\r\nDBG round %f", fRound)
		pPrivate_t->iTargetCount = pPrivate_t->iCountPerRound * fRound;
		DEBUG_LOG("\r\nDBG set encoder cur %d", pPrivate_t->iCurrentCount)
		DEBUG_LOG("\r\nDBG set encoder tar %d", pPrivate_t->iTargetCount)
		pPrivate_t->bCountUp = (pPrivate_t->iTargetCount > pPrivate_t->iCurrentCount) ? true : false;
		pPrivate_t->iRelativeDistStart = pPrivate_t->iCurrentCount;
		pPrivate_t->bArrived = false;
		
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
		
//		pPrivate_t->iRelativeDistStart = pPrivate_t->iCurrentCount;
		DEBUG_LOG("\r\nDBG set encoder pos %f", fValue)
		DEBUG_LOG("\r\nDBG set cur cnt %d", pPrivate_t->iCurrentCount)
		
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

		if(!pPrivate_t->bReverseCountDir)
		{
				pPrivate_t->iCurrentCount = pPrivate_t->iPeriodCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&htim3);
		}
		else
		{
				pPrivate_t->iCurrentCount = -pPrivate_t->iPeriodCount * 0xFFFF - __HAL_TIM_GET_COUNTER (&htim3);
		}
		
//		DEBUG_LOG("\r\nDBG icnt %d,ircnt %d,it3 %d", pPrivate_t->iCurrentCount, pPrivate_t->iPeriodCount, __HAL_TIM_GET_COUNTER (&htim3))
		pPrivate_t->iRelativeDist = pPrivate_t->iCurrentCount - pPrivate_t->iRelativeDistStart;
}

bool GetEncoderRelativeValueAbs(PRIVATE_MEMBER_TYPE *m_pThisPrivate, uint32_t *iValue)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}	

		*iValue = abs(pPrivate_t->iRelativeDist);
		return true;
}

bool GetEncoderLinearValue(PRIVATE_MEMBER_TYPE *m_pThisPrivate, float *fValue)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				
						
//		pPrivate_t->iCurrentCount = pPrivate_t->iRoundCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&pPrivate_t->hTIM);
		
//		pPrivate_t->iCurrentCount = pPrivate_t->iPeriodCount * 0xFFFF + __HAL_TIM_GET_COUNTER (&htim3);
		FreshEncoderCount(m_pThisPrivate);
		DEBUG_LOG("\r\nDBG get encoder pos %d", pPrivate_t->iCurrentCount)
		*fValue = (float)pPrivate_t->iCurrentCount / pPrivate_t->iCountPerRound * pPrivate_t->fPitch;		
		
		return true;
}

void EncoderIntSpeedHandler(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		static int32_t iCountLast = 0;
	
		PrivateBlock *pPrivate_t = (PrivateBlock *)pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}
		
		FreshEncoderCount(pThisPrivate);
		
		pPrivate_t->iCountDelta = pPrivate_t->iCurrentCount - iCountLast;
		iCountLast = pPrivate_t->iCurrentCount;
}

bool GetEncoderLinearSpeed(PRIVATE_MEMBER_TYPE *m_pThisPrivate, float *fValue)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				
				
		*fValue = fabs((float)pPrivate_t->iCountDelta / pPrivate_t->iCountPerRound * pPrivate_t->fPitch * 100);
		
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
				//DIR位为0时向上计数
				if (htim3.Instance->CR1 & 0x0010) 
				{
						//DEBUG_LOG("\r\nDBG cr1-")
						pPrivate_t->iPeriodCount -= 1;
				}
				else
				{
						//DEBUG_LOG("\r\nDBG cr1+")
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

bool ReverseCountDir(PRIVATE_MEMBER_TYPE *m_pThisPrivate, bool bValue)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)m_pThisPrivate;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}				
		
		pPrivate_t->bReverseCountDir = bValue;
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
						DEBUG_LOG("\r\nDBG inc Arrived+,%d", pPrivate_t->iCurrentCount)
						pPrivate_t->bArrived = true;
						return true;						
				}
		}
		else
		{
				if(pPrivate_t->iCurrentCount <= pPrivate_t->iTargetCount)
				{
						DEBUG_LOG("\r\nDBG inc Arrived-")
						pPrivate_t->bArrived = true;
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
		
		//配置编码器线数
		pPrivate_t->iCountPerRound = Params_t->iEncoderLines * Params_t->iMultiplication;	
//		DEBUG_LOG("\r\nDBG encoder count per round %d", pPrivate_t->iCountPerRound)
		//配置编码器当前值
		pPrivate_t->iCurrentCount = 0;
		pPrivate_t->fPitch = Params_t->fPitch;
		
		pPrivate_t->iRelativeDistStart = 0;
		pPrivate_t->iRelativeDist = 0;
		pPrivate_t->iPeriodCount = 0;
		pPrivate_t->iCountDelta = 0;
		pPrivate_t->iTargetCount = 0;
		pPrivate_t->bArrived = false;
		Block_t->m_pGetEncoderLinearValue = GetEncoderLinearValue;
		Block_t->m_pIncEncoderTargetArrived = IncEncoderTargetArrived;
		Block_t->m_pThisPrivate = pPrivate_t;
		Block_t->m_pSetEncoderTarget = SetEncoderTarget;
		Block_t->m_pSetEncoderValuef = SetEncoderValuef;
		Block_t->m_pGetEncoderLinearSpeed = GetEncoderLinearSpeed;
		Block_t->m_pReverseCountDir = ReverseCountDir;
		Block_t->m_pGetEncoderRelativeValueAbs = GetEncoderRelativeValueAbs;
		Block_t->m_pIsTargetArrived = IsTargetArrived;
		Block_t->m_pIsNextPointSet = IsNextPointSet;
		
		RegisterEncoderVar(pPrivate_t);
//		__HAL_TIM_SET_COUNTER (&htim3, 0);
		__HAL_TIM_CLEAR_IT (&htim3, TIM_IT_UPDATE);
		 __HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE); 
		TIM3_Encoder_Switch (1);//开编码器
		
		HAL_TIM_Encoder_Start_IT(&htim3,TIM_CHANNEL_ALL);
		
		DEBUG_LOG("\r\nDBG inc encoder init success")
}

