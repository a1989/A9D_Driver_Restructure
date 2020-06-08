#include "StepperControl.h"
#include "DRV8711_Operation.h"
#include <stdlib.h>
#include "tim.h"

typedef struct
{
		float fCurrentPos;
		float fSpeed;
}StepperMoveParams;

typedef struct
{
		VOID_HandleTypeDef *hHandle;
		TIM_HandleTypeDef hTIM;
		char *strType;
}PrivateBlock;

//×ªËÙ,¾àÀë
//const uint16_t arrSpeedTable[ACC_TIME_DIVISION][2] = {
//{49,0},
//{199,3},
//{449,8},
//{799,15},
//{1250,25},
//{1800,36},
//{2450,49},
//{3199,63},
//{4049,80},
//{4999,99},
//{5000,100},
//{5950,119},
//{6800,136},
//{7550,151},
//{8200,164},
//{8750,175},
//{9200,184},
//{9550,191},
//{9800,196},
//{9950,199}
//};

void StopStepperModerate(PrivateBlock *pPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}	

		HAL_TIM_OC_Stop_IT (&pPrivate_t->hTIM, TIM_CHANNEL_1);
}

void SingleStepperPrepare(PrivateBlock *pPrivate)
{
		
}

static float CalcCurveForBlock(void)
{
		
}

void StepperControlInit(StepperControl *pStepper_t, StepperParams *pParams_t)
{
		PrivateBlock *pPrivate = NULL;
		DRV8711_Control *pDRV8711 = NULL;
		DRV8711_Params pDRV8711_Params;
	
		if(NULL == pStepper_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return;
		}
		
		pPrivate = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		if(NULL == pStepper_t)
		{
				printf("\r\nfunc:%s,malloc failed", __FUNCTION__);				
				return;
		}		
		
		switch(pParams_t->eDriver)
		{
				case eDRV8711:			
					DEBUG_LOG("\r\nDBG Start init DRV8711")
					pDRV8711 = (DRV8711_Control *)malloc(sizeof(DRV8711_Control));
					if(NULL == pDRV8711)
					{
							printf("\r\nfunc:%s,malloc 8711 failed", __FUNCTION__);				
							return;
					}		
										
					pDRV8711_Params.eMode = pParams_t->eConfigMode;
					pDRV8711_Params.iCurrentCfg = pParams_t->iCurrentCfg;
					pDRV8711_Params.iSubdivisionCfg = pParams_t->iSubdivisionCfg;
					
					DRV8711_Init(pDRV8711, &pDRV8711_Params);
					
					break;
				case eTMC2590:
					break;
				default:
					break;
		}
		
		switch(pParams_t->eMotorTIM)
		{
				case eTIM1:
					break;
				case eTIM2:
					MX_TIM_OC_Init(&pPrivate->hTIM, TIM2);
					break;
				default:
					break;
		}
		
		pStepper_t->m_pThisPrivate = pPrivate;
//		pStepper_t->m_pSingleEncoderStepperPrepare = SingleEncoderStepperPrepare;
}


