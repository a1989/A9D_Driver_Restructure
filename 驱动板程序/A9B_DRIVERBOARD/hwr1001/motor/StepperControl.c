#include "StepperControl.h"
#include "DRV8711_Operation.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "tim.h"
#include "interrupt.h"

#define ACC_TIME_DIVISION	20
#define DEC_TIME_DIVISION	20
#define ACC_TIME	0.4
#define DEC_TIME	0.4

#define CURVE_BUFFER_SIZE	2

uint32_t iDebug0 = 0;
uint32_t iDebug1 = 0;

//标准速度参考表, 速度为10000个脉冲每秒, 0.4秒加速, 两个抛物线组成的S形
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
		float fAccTime;
		float fDecTime;
		uint32_t iNominalStartSpeed;
		uint32_t iNominalEndSpeed;		
		uint16_t arrAccDivisionTable[ACC_TIME_DIVISION][2];
		uint16_t arrDecDivisionTable[DEC_TIME_DIVISION][2];
		uint32_t iAccDist;
		uint32_t iPlatDist;
		uint32_t iDecDist;
		bool bAccAddIndex;
		uint32_t iAccTableIndex;
		uint32_t iAccAccumulation;
		bool bDecAddIndex;
		uint32_t iDecTableIndex;
		uint32_t iDecAccumulation;
		bool bPlateauAll;
		int16_t iPlatValueOC;
		bool bDirForward;
		bool bAccPlan;
		bool bDecPlan;
		float fMaxSpeed;
		uint16_t iPlanOC;			
}CurveParmas;

typedef struct
{
		VOID_HandleTypeDef *hHandle;
		TIM_HandleTypeDef hTIM;
		char *strType;
//		float fAccTime;
//		float fDecTime;
//		uint32_t iNominalStartSpeed;
//		uint32_t iNominalEndSpeed;
		StepperParams StepperParams_t;
//		uint16_t arrAccDivisionTable[ACC_TIME_DIVISION][2];
//		uint16_t arrDecDivisionTable[DEC_TIME_DIVISION][2];
		void *pDriver;
		CurveParmas arrCurveParmas[CURVE_BUFFER_SIZE];
		bool bStop;
		bool bCheckDir;
//		uint32_t iAccDist;
//		uint32_t iPlatDist;
//		uint32_t iDecDist;
		
//		bool bAccAddIndex;
//		uint32_t iAccTableIndex;
//		uint32_t iAccAccumulation;
//	
//		bool bDecAddIndex;
//		uint32_t iDecTableIndex;
//		uint32_t iDecAccumulation;
//		
//		bool bPlateauAll;
		uint8_t iParamsReadIndex;
		uint8_t iParmasWriteIndex;
		volatile uint8_t iParmasBufferLen;
//		int16_t iPlatValueOC;
}PrivateBlock;

static bool IsStepperStop(PRIVATE_MEMBER_TYPE *pPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}			
		
		return (pPrivate_t->bStop);
}


static bool IsNextDirectionReverse(PRIVATE_MEMBER_TYPE *pPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
		bool bDirForwardCurrent = true;
		bool bDirForwardNext = true;
		bool bReverse;
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}		
		
//		DEBUG_LOG("\r\nDBG l %d,c %d", bDirForwardLast, pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward)

		bDirForwardCurrent = pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward;
		bDirForwardNext = pPrivate_t->arrCurveParmas[(pPrivate_t->iParamsReadIndex + 1) % CURVE_BUFFER_SIZE].bDirForward;

		pPrivate_t->bCheckDir = true;
		if(bDirForwardCurrent != bDirForwardNext)
		{
				
				return true;
		}
		else
		{
				return false;
		}
}

void StepperStop(PRIVATE_MEMBER_TYPE *pPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
		static bool bDirForwardLast = true;
		static bool bWaitNextPointSet = false;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}	

		if(pPrivate_t->bStop)
		{
				return;
		}
//		DEBUG_LOG("\r\nDBG dir %d", pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward)
		if(pPrivate_t->iParmasBufferLen > 0)
		{				
				DEBUG_LOG("\r\nDBG Plan Buffer len- %d", pPrivate_t->iParmasBufferLen)
				bDirForwardLast = pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward;
				pPrivate_t->iParamsReadIndex = (pPrivate_t->iParamsReadIndex + 1) % CURVE_BUFFER_SIZE;				
				pPrivate_t->iParmasBufferLen--;
		}
		
		if(0 == pPrivate_t->iParmasBufferLen)
		{
				DEBUG_LOG("\r\nDBG stop oc buflen=0")			
				
				HAL_TIM_OC_Stop_IT (&htim2, TIM_CHANNEL_1);	
				pPrivate_t->bStop = true;
				bWaitNextPointSet = false;
				pPrivate_t->bCheckDir = false;
				return;
		}
//		DEBUG_LOG("\r\nDBG lst dir %d", bDirForwardLast)
//		DEBUG_LOG("\r\nDBG dir %d", pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward)
		if(bDirForwardLast != pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward)
		{
				//如果即将开始的运动和本次运动方向不同则停止
				DEBUG_LOG("\r\nDBG stop oc dir rev")
			
				HAL_TIM_OC_Stop_IT (&htim2, TIM_CHANNEL_1);
				pPrivate_t->bStop = true;
		}
		else
		{
//				HAL_TIM_OC_Stop_IT (&htim2, TIM_CHANNEL_1);
				bWaitNextPointSet = true;
		}

//		bDirForwardLast = pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward;

//		HAL_TIM_OC_Stop_IT (&pPrivate_t->hTIM, TIM_CHANNEL_1);		
}

//void StepperLocationArrived(PRIVATE_MEMBER_TYPE *pPrivate)
//{
//		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
//	
//		if(NULL == pPrivate_t)
//		{
//				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
//				return;
//		}	

////		if(pPrivate_t->iParmasBufferLen > 0)
////		{
////				pPrivate_t->iParamsReadIndex = (pPrivate_t->iParamsReadIndex + 1) % CURVE_BUFFER_SIZE;
////				pPrivate_t->iParmasBufferLen--;
////		}
////		else
////		{
////				StepperStop(pPrivate_t);
////		}
//		StepperStop(pPrivate_t);
//}

void StopStepperModerate(PrivateBlock *pPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}	

//		HAL_TIM_OC_Stop_IT (&pPrivate_t->hTIM, TIM_CHANNEL_1);
}


void SetTIM_OC(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *hTIM, uint32_t iPos)
{
		uint16_t iCount; 
		static uint16_t iToggleParam = 50;
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}					
		
		//如果是全程匀速的情况
		if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bPlateauAll)
		{
				iToggleParam = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlatValueOC;
		}
		else
		{
				iDebug1 = iPos;
				iDebug0 = 4;

				//加减速情况
				//当前处于加速段
				if(iPos < pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccDist)
				{
						iDebug0 = 1;
						if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bAccAddIndex)
						{
								//累加一段距离
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccAccumulation += 
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].arrAccDivisionTable[pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccTableIndex][1];
							
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bAccAddIndex = false;
						}
						if(iPos > pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccAccumulation)
						{
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccTableIndex++;
								if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccTableIndex > 19)
								{
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccTableIndex = 19;
								}
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bAccAddIndex = true;
						}
												
						iToggleParam = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].arrAccDivisionTable[pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccTableIndex][0];
						if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bAccPlan)
						{
								if(iToggleParam > pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC)
								{
										iToggleParam = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC;
								}
						}
				}
				//匀速段
				else if(iPos >= pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iAccDist && iPos < pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecAccumulation)
				{
						iDebug0 = 2;
				}
				//减速段
				else
				{
						iDebug0 = 3;
						if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecAddIndex)
						{
								//累加一段距离
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecAccumulation += 
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].arrDecDivisionTable[pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecTableIndex][1];
								
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecAddIndex = false;
						}
						if(iPos > pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecAccumulation)
						{
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecTableIndex++;
								if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecTableIndex > 19)
								{
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecTableIndex = 19;
								}
								pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecAddIndex = true;
						}
						
						iToggleParam = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].arrDecDivisionTable[pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iDecTableIndex][0];		
						if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecPlan)
						{
								if(iToggleParam > pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC)
								{
										iToggleParam = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC;
								}
						}
				}
		}
		
		if(iToggleParam < 35)
		{
				iToggleParam = 35;
		}
		
		iCount = __HAL_TIM_GET_COUNTER (hTIM);
//		__HAL_TIM_SET_COMPARE (&pPrivate->hTIM, TIM_CHANNEL_1, (uint16_t)(iCount + iToggleParam));
		__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_1, (uint16_t)(iCount + iToggleParam));
//		HAL_TIM_OC_Start_IT (&htim2, TIM_CHANNEL_1);
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
		float fZoomFactorAcc = 1.0;
		float fZoomFactorDec = 1.0;
		float fMaxSpeed;
		float fCurveMultipleAcc = 1.0;
		float fCurveMultipleDec = 1.0;
		bool bPlateauAll = false;		
		float fTimeTickAcc;
		float fTimeTickDec;
		int i = 0;
	
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
		
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return;
		}	
		
		DEBUG_LOG("\r\nDBG idist %d", iDistance)
		DEBUG_LOG("\r\nDBG ispd %d", iSpeed)

		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].bAccAddIndex = true;
		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iAccTableIndex = 0;
		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iAccAccumulation = 0;
		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].bDecAddIndex = true;
		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iDecTableIndex = 0;
		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iDecAccumulation = 0;
		pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bPlateauAll = false;
		iStartSpeed = pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iNominalStartSpeed;
		iEndSpeed = pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iNominalEndSpeed;
		
		//抛物线查表法
		fTimeTickAcc = pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fAccTime / ACC_TIME_DIVISION;
		fTimeTickDec = pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fDecTime / DEC_TIME_DIVISION;
		
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
				fAccAvg = (float)((int32_t)iSpeed - (int32_t)iStartSpeed) / pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fAccTime;
				fDecAvg = (float)((int32_t)iEndSpeed - (int32_t)iSpeed) / pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fDecTime;
				//计算加速段距离	(Vt ^ 2 - V0 ^ 2) / 2a = S, V0 = 0;
				fAccDist = (float)( pow(iSpeed, 2) - pow(iStartSpeed, 2) ) / (2 * fAccAvg);
				
				//计算减速段距离
//				DEBUG_LOG("\r\nDBG iStartSpeed %d", iStartSpeed)
//				DEBUG_LOG("\r\nDBG iEndSpeed %d", iEndSpeed)
//				DEBUG_LOG("\r\nDBG atime %f", pPrivate_t->fAccTime)
//				DEBUG_LOG("\r\nDBG dtime %f", pPrivate_t->fDecTime)
//				DEBUG_LOG("\r\nDBG faavg %f", fAccAvg)
//				DEBUG_LOG("\r\nDBG fdavg %f", fDecAvg)
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
						//printf("\r\ntrian");
						fZoomFactorAcc = (float)iDistance / ( fAccDist + fDecDist );
						fZoomFactorDec = (float)iDistance / ( fAccDist + fDecDist );
					
						fAccDist = fAccDist * fZoomFactorAcc;
						fDecDist = fDecDist * fZoomFactorDec;
						//重新计算所能达到的速度 V0 * t + a' * t ^ 2 / 2 = AccDist
						//a' = 2 * (AccDist - V0 * t) / t ^ 2
						//Vt = V0 + a' * t = V0 + 2 * (AccDist - V0 * t) / t
						fMaxSpeed = iStartSpeed + 
						2 * (fAccDist - iStartSpeed * pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fAccTime) / pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fAccTime;
						//fMaxSpeed
						if(fMaxSpeed < min(iStartSpeed, iEndSpeed))
						{
								//重计算速度过小时以低速匀速运动
								fMaxSpeed = min(iStartSpeed, iEndSpeed);
								bPlateauAll = true;
						}
						else
						{
								//重计算速度适中以钟形曲线运动
								fPlateauDist = fDecDist / 5;
								bPlateauAll = false;
						}
				}
				
				if(!bPlateauAll)
				{							
						//如果不是全程匀速运动的情况
						//计算与标准曲线的比值
						fCurveMultipleAcc = (float)(fMaxSpeed - iStartSpeed) / 10000 / pPrivate->StepperParams_t.fFeedBackRatio;
					//printf("\r\nnpa");
						for(i = 0; i < ACC_TIME_DIVISION; i++)
						{
								pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[i][0] = (uint16_t)((float)2000000 / ((fZoomFactorAcc * (float)arrSpeedTable[i][0] * fCurveMultipleAcc + iStartSpeed) * 1));
								pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[i][1] = (uint16_t)((float)arrSpeedTable[i][0] * fTimeTickAcc * fCurveMultipleAcc * pPrivate->StepperParams_t.fFeedBackRatio );
								pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[i][0] = (uint16_t) ((float)pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[i][0] / 2 - (7 + 8.75 / pPrivate->StepperParams_t.fFeedBackRatio)); // / pPrivate->StepperParams_t.fFeedBackRatio);
								DEBUG_LOG("\r\n%d,%d", pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[i][0], pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[i][1])
						}
						
						fCurveMultipleDec = (float)(fMaxSpeed - iEndSpeed) / 10000 / pPrivate->StepperParams_t.fFeedBackRatio;
						DEBUG_LOG("\r\n---")
						
						for(i = 0; i < DEC_TIME_DIVISION; i++)
						{
								pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrDecDivisionTable[DEC_TIME_DIVISION - i - 1][0] = (uint16_t)((float)2000000 / ((fZoomFactorDec * (float)arrSpeedTable[i][0] * fCurveMultipleDec + iEndSpeed) * 1));
								pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrDecDivisionTable[DEC_TIME_DIVISION - i - 1][1] = (uint16_t)((float)arrSpeedTable[i][0] * fTimeTickDec * fCurveMultipleDec * pPrivate->StepperParams_t.fFeedBackRatio);
								pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrDecDivisionTable[DEC_TIME_DIVISION - i - 1][0] = (uint16_t) ((float)pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrDecDivisionTable[DEC_TIME_DIVISION - i - 1][0] / 2 - (7 + 8.75 / pPrivate->StepperParams_t.fFeedBackRatio)); // / pPrivate->StepperParams_t.fFeedBackRatio);
								DEBUG_LOG("\r\n%d,%d", pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrDecDivisionTable[DEC_TIME_DIVISION - i - 1][0], pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].arrDecDivisionTable[DEC_TIME_DIVISION - i - 1][1])
						}
						
						pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iAccDist = fAccDist;
						pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iDecDist = fDecDist;
						pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlatDist = fPlateauDist + fDecDist / 5;
						pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iDecAccumulation = pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iAccDist + pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlatDist;
						
						DEBUG_LOG("\r\nDBG iaccdist %d", pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iAccDist)
						DEBUG_LOG("\r\nDBG idecdist %d", pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iDecDist)
						DEBUG_LOG("\r\nDBG iplatdist %d", pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlatDist)
				}
				//至此S曲线段参数已全部确定					
		}
		else
		{
				//printf("\r\npa1");
				fMaxSpeed = min(iStartSpeed, iEndSpeed);
				bPlateauAll = true;
		}
		
		pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].fMaxSpeed = fMaxSpeed;
		
		if(bPlateauAll)
		{
				//printf("\r\npa");
				pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].bPlateauAll = bPlateauAll;
//				for(i = 0; i < ACC_TIME_DIVISION; i++)
//				{
//						pPrivate_t->arrAccDivisionTable[i][0] = (uint16_t)((float)2000000 / ((fZoomFactorAcc * (float)arrSpeedTable[i][0] * fCurveMultipleAcc + iStartSpeed) * 2));
//						pPrivate_t->arrAccDivisionTable[i][1] = (uint16_t)((float)arrSpeedTable[i][0] * fTimeTickAcc);
//						pPrivate_t->arrAccDivisionTable[i][0] = (uint16_t) ((float)pPrivate_t->arrAccDivisionTable[i][0]);
//						printf("\r\n%d,%d", pPrivate_t->arrAccDivisionTable[i][0], pPrivate_t->arrAccDivisionTable[i][1]);
//				}			
				pPrivate_t->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlatValueOC = (uint16_t)((float)2000000 / (fMaxSpeed) / 2);
		}
}

bool StepperForward(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		DRV8711_Control *pDRV8711 = NULL;
	
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}			
		
		DEBUG_LOG("\r\nStepper dir forward")		
		
		switch(pPrivate->StepperParams_t.eDriver)
		{
				case eDRV8711:
					pDRV8711 = pPrivate->pDriver;
					DEBUG_LOG("\r\nconfig 8711 dir pin")
					pDRV8711->m_pDRV8711_Forward(pDRV8711->m_pThisPrivate);
					break;
				default:
					break;
		}
		
		return false;
}

bool StepperBackward(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		DRV8711_Control *pDRV8711 = NULL;
	
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}			
		
		DEBUG_LOG("\r\nStepper dir Backward")		
		
		switch(pPrivate->StepperParams_t.eDriver)
		{
				case eDRV8711:
					pDRV8711 = pPrivate->pDriver;
					DEBUG_LOG("\r\nconfig 8711 dir pin")
					pDRV8711->m_pDRV8711_Backward(pDRV8711->m_pThisPrivate);
					break;
				default:
					break;
		}		
		
		return false;
}

bool StepperMove(PRIVATE_MEMBER_TYPE *pPrivate)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}	
				
		DEBUG_LOG("\r\nDBG Plan Buffer len move %d", pPrivate_t->iParmasBufferLen)
		if(pPrivate_t->iParmasBufferLen > 0)
		{
				if(pPrivate_t->arrCurveParmas[pPrivate_t->iParamsReadIndex].bDirForward)
				{
						StepperForward(pPrivate_t);
				}
				else
				{
						StepperBackward(pPrivate_t);
				}
				DEBUG_LOG("\r\nDBG stepper start oc")
				pPrivate_t->bStop = false;
				HAL_TIM_OC_Start_IT (&htim2, TIM_CHANNEL_1);					
		}
		else
		{
				DEBUG_LOG("\r\nDBG move stop")
				StepperStop(pPrivate_t);
		}
		
		return true;
}

bool SetStepperDirHighAsForward(PRIVATE_MEMBER_TYPE *pThisPrivate, bool bValue)
{
		DRV8711_Control *pDRV8711 = NULL;
	
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}	

		pDRV8711 = pPrivate->pDriver;
		pDRV8711->m_pDRV8711_DirPinHighAsForward(pDRV8711->m_pThisPrivate, bValue);
}

bool StepperPrepare(PRIVATE_MEMBER_TYPE *pThisPrivate, float fDistance, float fSpeed)
{
		uint32_t iPulseDist;
	  uint32_t iPulseSpeed;
		static bool fLastDirForward = false;
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}			
		
//		DEBUG_LOG("\r\nDBG pre buffer len %d", pPrivate->iParmasBufferLen)
		if(pPrivate->iParmasBufferLen < CURVE_BUFFER_SIZE)
		{
				if(pPrivate->iParmasBufferLen > 0)
				{
						if(!pPrivate->bCheckDir)
						{
								return false;
						}
				}
				//如果是新点则不需要进行速度规划
				//如果新的位置点与当前位置点方向不同则不需要速度规划
				DEBUG_LOG("\r\nStepper Prepare")
				DEBUG_LOG("\r\nDBG fdist %f", fDistance)
				DEBUG_LOG("\r\nDBG fspd %f", fSpeed)
				
		//		DEBUG_LOG("\r\nDBG fpithch %f", pPrivate->StepperParams_t.fPitch)
		//		DEBUG_LOG("\r\nDBG isubdivision %d", pPrivate->StepperParams_t.iSubdivisionCfg)
		//		DEBUG_LOG("\r\nDBG iratio %f", pPrivate->StepperParams_t.fFeedBackRatio)
				
				//计算脉冲距离
				iPulseDist = fabs((fDistance / pPrivate->StepperParams_t.fPitch) * pPrivate->StepperParams_t.iSubdivisionCfg * 200
										* pPrivate->StepperParams_t.fFeedBackRatio);
				
				//计算脉冲速度
				iPulseSpeed = (fSpeed / pPrivate->StepperParams_t.fPitch) * pPrivate->StepperParams_t.iSubdivisionCfg * 200
										* pPrivate->StepperParams_t.fFeedBackRatio;
				
				pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bAccPlan = false;
				pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bDecPlan = false;
				//计算起始和收尾脉冲速度
				pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].iNominalStartSpeed = (float)MOTOR_START_SPEED * pPrivate->StepperParams_t.fFeedBackRatio;
				pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].iNominalEndSpeed = (float)MOTOR_END_SPEED * pPrivate->StepperParams_t.fFeedBackRatio;

				pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bDirForward = true;
				if(fDistance < 0)
				{
						//DEBUG_LOG("\r\ndir backward")
						pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bDirForward = false;
				}
				
				fLastDirForward = pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bDirForward;
				CalcCurveForBlockLinear(pThisPrivate, iPulseDist, iPulseSpeed);
				
				//如果当前的速度规划buffer里已经有数据了
				if(pPrivate->iParmasBufferLen > 0)
				{
						if(fLastDirForward == pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bDirForward)
						{
								//如果当前的最大速度等于下一线段的最大速度
								if(fabs(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].fMaxSpeed - pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].fMaxSpeed) < 0.1)
								{
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecPlan = true;
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].arrAccDivisionTable[ACC_TIME_DIVISION - 1][0];
										pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bAccPlan = true;
										pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlanOC = pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[ACC_TIME_DIVISION - 1][0];
								}
								//如果当前的最大速度大于下一线段的最大速度
								else if(pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].fMaxSpeed - pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].fMaxSpeed > 0.1)
								{
										pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bAccPlan = true;
										pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlanOC = pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].arrAccDivisionTable[ACC_TIME_DIVISION - 1][0];
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecPlan = true;
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC = pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlanOC;										
								}
								//如果当前的最大速度小于下一线段的最大速度
								else
								{
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].bDecPlan = true;
										pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].arrAccDivisionTable[ACC_TIME_DIVISION - 1][0];	
										pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].bAccPlan = true;
										pPrivate->arrCurveParmas[pPrivate->iParmasWriteIndex].iPlanOC = pPrivate->arrCurveParmas[pPrivate->iParamsReadIndex].iPlanOC;									
								}
						}
						else
						{
								
						}
				}
				else
				{
						
				}
				pPrivate->iParmasWriteIndex = (pPrivate->iParmasWriteIndex + 1) % CURVE_BUFFER_SIZE;
				pPrivate->iParmasBufferLen++;
				
				DEBUG_LOG("\r\nDBG Plan Buffer len %d", pPrivate->iParmasBufferLen)
				//HAL_TIM_OC_Start_IT (&pPrivate->hTIM, TIM_CHANNEL_1);
//				HAL_TIM_OC_Start_IT (&htim2, TIM_CHANNEL_1);
//				DEBUG_LOG("\r\nStart OC")
				
				return true;
		}
		else
		{
				return false;
		}
}


TIM_HandleTypeDef *GetStepperTimHandle(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s,null pointer", __FUNCTION__);				
				return false;
		}		

		return &htim2; //&pPrivate->hTIM;
}

void StepperControlInit(StepperControl *pStepper_t, StepperParams *pParams_t)
{
		PrivateBlock *pPrivate = NULL;
		DRV8711_Control *pDRV8711 = NULL;
		DRV8711_Params pDRV8711_Params;
		uint8_t i = 0;
	
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
					pPrivate->pDriver = pDRV8711;
					break;
				case eTMC2590:
					break;
				default:
					break;
		}
		
		memcpy(&pPrivate->StepperParams_t, pParams_t, sizeof(StepperParams));
		pPrivate->StepperParams_t.eDriver = pParams_t->eDriver;
		
		for(i = 0; i < CURVE_BUFFER_SIZE; i++)
		{
				pPrivate->arrCurveParmas[i].fAccTime = ACC_TIME;
				pPrivate->arrCurveParmas[i].fDecTime = DEC_TIME;
		}
//		pPrivate->fAccTime = ACC_TIME;
//		pPrivate->fDecTime = DEC_TIME;
		
		pPrivate->iParamsReadIndex = 0;
		pPrivate->iParmasBufferLen = 0;
		pPrivate->iParmasWriteIndex = 0;
		
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
		pStepper_t->m_pGetStepperTimHandle = GetStepperTimHandle;
		pStepper_t->m_pStepperStop = StepperStop;
		pStepper_t->m_pSetTIM_OC = SetTIM_OC;
		pStepper_t->m_pStepperBackward = StepperBackward;
		pStepper_t->m_pStepperForward = StepperForward;
		pStepper_t->m_pSetStepperDirHighAsForward = SetStepperDirHighAsForward;
		pStepper_t->m_pStepperMove = StepperMove;
		pStepper_t->m_pThisPrivate = pPrivate;
		pStepper_t->m_pIsStepperStop = IsStepperStop;
		pStepper_t->m_pIsNextDirectionReverse = IsNextDirectionReverse;
		
		DEBUG_LOG("\r\nstepper init success")
}
