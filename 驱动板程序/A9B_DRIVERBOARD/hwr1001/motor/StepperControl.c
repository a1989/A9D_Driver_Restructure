#include "StepperControl.h"
#include "DRV8711_Operation.h"
#include <stdlib.h>

typedef struct
{
		VOID_HandleTypeDef *hHandle;
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

void SingleStepperPrepare(AxisEnum eAxisIndex, float fDistance, float fSpeed)
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
		
		pStepper_t->m_pThisPrivate = pPrivate;
//		pStepper_t->m_pSingleEncoderStepperPrepare = SingleEncoderStepperPrepare;
}


