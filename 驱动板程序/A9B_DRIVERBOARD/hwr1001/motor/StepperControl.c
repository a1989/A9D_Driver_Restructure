#include "StepperControl.h"
#include "DRV8711_Operation.h"

typedef struct
{
		VOID_HandleTypeDef *hHandle;
		char *strType;
}PrivateBlock;

//×ªËÙ,¾àÀë
const uint16_t arrSpeedTable[ACC_TIME_DIVISION][2] = {
{49,0},
{199,3},
{449,8},
{799,15},
{1250,25},
{1800,36},
{2450,49},
{3199,63},
{4049,80},
{4999,99},
{5000,100},
{5950,119},
{6800,136},
{7550,151},
{8200,164},
{8750,175},
{9200,184},
{9550,191},
{9800,196},
{9950,199}
};

void SingleStepperPrepare(AxisEnum eAxisIndex, float fDistance, float fSpeed)
{
		
}

void StepperControlInit(StepperControl *pStepper_t, StepperParams *pParams_t)
{
		PrivateBlock *pPrivate = NULL;
		DRV8711_Block *pDRV8711 = NULL;
		DRV8711_Params *pDRV8711_Params = NULL;
	
		if(NULL == pStepper_t)
		{
				printf("\r\nfunc:%s, error:Null Pointer", __FUNCTION__);				
				return;
		}
		
		pPrivate = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		
		switch(pParams_t->eDriver)
		{
				case eDRV8711:					
					pDRV8711 = (DRV8711_Block *)malloc(sizeof(DRV8711_Block));
					pDRV8711_Params->eMode = pParams_t->eConfigMode;
					DRV8711_Init(pDRV8711->m_pThisPrivate, pDRV8711_Params);
					break;
				case eTMC2590:
					break;
				default:
					break;
		}
		
		Stepper_t->m_pThisPrivate = pPrivate;
		Stepper_t->m_pSingleEncoderStepperPrepare = SingleEncoderStepperPrepare;
}


