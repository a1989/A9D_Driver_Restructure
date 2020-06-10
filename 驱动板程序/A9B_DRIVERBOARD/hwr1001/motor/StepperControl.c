#include "StepperControl.h"
#include "DRV8711_Operation.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "tim.h"
#include "interrupt.h"

#define ACC_TIME_DIVISION	20

//标准速度参考表, 速度为10000个脉冲每秒, 0.4秒加速, 抛物线形
//转速,距离
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

//typedef struct
//{
//		
//}CurveParams;

typedef struct
{
		VOID_HandleTypeDef *hHandle;
		TIM_HandleTypeDef hTIM;
		char *strType;
		float fAccTime;
		float fDecTime;
		uint32_t iNominalStartSpeed;
		uint32_t iNominalEndSpeed;
		StepperParams StepperParams_t;
		uint16_t arrAccDivisionTable[ACC_TIME_DIVISION][2];
}PrivateBlock;

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

static void CalcCurveForBlockLinear(PrivateBlock *pPrivate, uint32_t iDistance, uint32_t iSpeed)
{
		uint32_t iStartSpeed;
		uint32_t iEndSpeed;
		float fAccAvg;
		float fDecAvg;
		float fAccDist;
		float fDecDist;
		float fPlateauDist;
		float fZoomFactorAcc;
		float fMaxSpeed;
		float fCurveMultipleAcc = 1.0;
		bool bPlateauAll = false;
		int i = 0;
		float fTimeTickAcc;
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
		
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}	
		
		iStartSpeed = pPrivate_t->iNominalStartSpeed;
		iEndSpeed = pPrivate_t->iNominalEndSpeed;
		
		//抛物线查表法
		fTimeTickAcc = pPrivate_t->fAccTime / ACC_TIME_DIVISION;

		if(iSpeed < min(iStartSpeed, iEndSpeed))
		{
				//如果比起始速度和收尾速度都小,则以两个速度中较小的值匀速运动
				fMaxSpeed = min(iStartSpeed, iEndSpeed);
				bPlateauAll = true;
		}
		else if(iSpeed > min(iStartSpeed, iEndSpeed) && iSpeed < max(iStartSpeed, iEndSpeed))
		{
				fMaxSpeed = max(iStartSpeed, iEndSpeed);
				bPlateauAll = true;
		}
		
		//给定速度大于起始速度且大于收尾速度时计算S曲线参数
		else if(iSpeed > iStartSpeed && iSpeed > iEndSpeed)
		{								
				//先以梯形为参考,一半加速时间时,Vm为给定速度一半且Vm - V0 = a*Tm, a = (Vm - V0) / Tm
				fAccAvg = (float)(iSpeed - iStartSpeed) / pPrivate_t->fAccTime;
				fDecAvg = (float)(iEndSpeed - iSpeed) / pPrivate_t->fDecTime;
				//计算加速段距离	(Vt ^ 2 - V0 ^ 2) / 2a = S, V0 = 0;
				fAccDist = (float)( pow(iSpeed, 2) - pow(iStartSpeed, 2) ) / (2 * fAccAvg);
				
				//计算减速段距离
				fDecDist = (float)( pow(iEndSpeed, 2) - pow(iSpeed, 2)) / (2 * fDecAvg);
				
				//最大速度就是给定速度
				fMaxSpeed = iSpeed;
				
				//计算匀速段距离
				fPlateauDist = (float)iDistance - fAccDist - fDecDist;
				//structParams->iEncoderPlatDist = (uint32_t)structParams->fPlateauDistance;
			
				//计算匀速段小于0									
				if(fPlateauDist < 0)
				{
						//梯形退化为三角形
						fZoomFactorAcc = (float)iDistance / ( fAccDist + fDecDist );
						
						fAccDist = fAccDist * fZoomFactorAcc;
						fDecDist = fDecDist * fZoomFactorAcc;
						//重新计算所能达到的速度 V0 * t + a' * t ^ 2 / 2 = AccDist
						//a' = 2 * (AccDist - V0 * t) / t ^ 2
						//Vt = V0 + a' * t = V0 + 2 * (AccDist - V0 * t) / t
						fMaxSpeed = iStartSpeed + 2 * (fAccDist - iStartSpeed * pPrivate_t->fAccTime) / pPrivate_t->fAccTime;
						if(fMaxSpeed < min(iStartSpeed, iEndSpeed))
						{
								//重计算速度过小时以低速匀速运动
								fMaxSpeed = min(iStartSpeed, iEndSpeed);
								bPlateauAll = true;
						}
						else
						{
								//重计算速度适中以钟形曲线运动
								fPlateauDist = 0;
								bPlateauAll = false;
						}
				}
				
				if(!bPlateauAll)
				{							
						//如果不是全程匀速运动的情况
						//计算与标准曲线的比值
						fCurveMultipleAcc = (float)(fMaxSpeed - iStartSpeed) / 10000;
					
						for(i = 0; i < ACC_TIME_DIVISION; i++)
						{
								pPrivate_t->arrAccDivisionTable[i][0] = (uint16_t)((float)2000000 / ((fZoomFactorAcc * (float)arrSpeedTable[i][0] * fCurveMultipleAcc + iStartSpeed) * 2));
								pPrivate_t->arrAccDivisionTable[i][1] = (uint16_t)((float)arrSpeedTable[i][0] * fTimeTickAcc);
								pPrivate_t->arrAccDivisionTable[i][0] = (uint16_t) ((float)pPrivate_t->arrAccDivisionTable[i][0]);
								printf("\r\n%d,%d", pPrivate_t->arrAccDivisionTable[i][0], pPrivate_t->arrAccDivisionTable[i][1]);
						}

				}
				//至此S曲线段参数已全部确定					
		}
		else
		{
				fMaxSpeed = min(iStartSpeed, iEndSpeed);
				bPlateauAll = true;
		}
}

bool StepperPrepare(PRIVATE_MEMBER_TYPE *pThisPrivate, float fDistance, float fSpeed)
{
		uint32_t iPulseDist;
	  uint32_t iPulseSpeed;
	
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}			
		
		DEBUG_LOG("\r\nStepper Prepare")
		
//		(iDir == 1) ? ;
		
		iPulseDist = fabs((fDistance / pPrivate->StepperParams_t.fPitch) * pPrivate->StepperParams_t.iSubdivisionCfg * 200
								* pPrivate->StepperParams_t.fFeedBackRatio);
		
		iPulseSpeed = (fSpeed / pPrivate->StepperParams_t.fPitch) * pPrivate->StepperParams_t.iSubdivisionCfg * 200
								* pPrivate->StepperParams_t.fFeedBackRatio;
		
		pPrivate->iNominalStartSpeed = (float)MOTOR_START_SPEED * pPrivate->StepperParams_t.fFeedBackRatio;
		pPrivate->iNominalStartSpeed = (float)MOTOR_END_SPEED * pPrivate->StepperParams_t.fFeedBackRatio;
		
		CalcCurveForBlockLinear(pThisPrivate, iPulseDist, iPulseSpeed);
		
		DEBUG_LOG("\r\nStart calc curve")
		
		//HAL_TIM_OC_Start_IT (&pPrivate->hTIM, TIM_CHANNEL_1);
		HAL_TIM_OC_Start_IT (&htim2, TIM_CHANNEL_1);
		DEBUG_LOG("\r\nStart OC")
}

void SetTIM_OC(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *hTIM, uint16_t iPos)
{
		uint16_t iCount; 
		uint16_t iToggleParam = 500;
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}			
		
		iCount =__HAL_TIM_GET_COUNTER (&pPrivate->hTIM);
//		__HAL_TIM_SET_COMPARE (&pPrivate->hTIM, TIM_CHANNEL_1, (uint16_t)(iCount + iToggleParam));
		__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_1, (uint16_t)(iCount + iToggleParam));
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
					pDRV8711_Params.pDriverPinConfig = pParams_t->pDriverPinConfig;
					
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
					//MX_TIM_OC_Init(&pPrivate->hTIM, TIM2);
					MX_TIM2_Init();
					break;
				default:
					break;
		}
		
		pStepper_t->m_pStepperPrepare = StepperPrepare;
		
		pStepper_t->m_pThisPrivate = pPrivate;
}


