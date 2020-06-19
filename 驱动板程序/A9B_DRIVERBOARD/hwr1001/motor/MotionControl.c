#include "MotionControl.h"
#include "MotorControl.h"
#include <stdlib.h>
#include "gpio.h"

#define MOTION_TYPE_NAME	"MotionTypedef"

//本模块的私有部分,用于控制电机,扩展后可用于控制气缸,电缸,电磁铁等运动机构
typedef struct
{
		MotorControl *pMotorControl_t;
		TIM_HandleTypeDef OutputTIM;
		char *strType;
}PrivateBlock;

void StopMotorImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		
}

static void HomeAxisImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, uint32_t iSpeed)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;
		DEBUG_LOG("\r\nStart home axis")
	
		pPrivate_t->pMotorControl_t->m_pMotorHomeImmediately(pPrivate_t->pMotorControl_t->m_pThisPrivate, iMotorID, iSpeed);		
}

//添加一个电机
static void AddMotor(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;
		
		DEBUG_LOG("\r\nDBG Add a motor")
		
		pPrivate_t->pMotorControl_t->m_pAddMotor(pPrivate_t->pMotorControl_t->m_pThisPrivate, Params_t);
}

bool AddMotorLimitSwitch(PRIVATE_MEMBER_TYPE *pThisPrivate, LimitParams *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;		
		
		DEBUG_LOG("\r\nDBG Limit Init GPIO")
		GPIO_InitInputNoPull(Params_t->GPIO_Port, Params_t->GPIO_Pin);
		pPrivate_t->pMotorControl_t->m_pAddLimits(pPrivate_t->pMotorControl_t->m_pThisPrivate, Params_t);
		
		return true;
}


bool GetMotorMoveParamByTIM(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *htim, uint16_t *iData)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate || NULL == htim)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;
		
		DEBUG_LOG("\r\nDBG Add a motor")
		
		if(pPrivate_t->pMotorControl_t->m_pGetMotorMoveParamByTIM(pPrivate_t->pMotorControl_t->m_pThisPrivate, htim, iData))
		{
				return false;
		}
		
		return true;
}



bool ReadMotorLimit(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, LimitFunction eFunc, bool *bStatu)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;		

		pPrivate_t->pMotorControl_t->m_pReadLimitByID(pPrivate_t->pMotorControl_t->m_pThisPrivate, iMotorID, eFunc, bStatu);		
		
		return true;
}

//static void HomeAxis(struct MotionBlock *pThis, MoveParams *Params_t)
//{
//		MoveBlock *MoveControl_t = NULL;
//	
//		MoveControl_t = pThis->m_pMoveControl;
//		MoveControl_t->m_pHomeAxis(MoveControl_t->m_pThis, Params_t);
//}

void GetMoveData()
{
		uint8_t iMoveType;
		//0:绝对位置运动
		//1:相对位置运动
		//2:复位
		//3:走到最大位置
		//4:停止
		switch(iMoveType)
		{
				case 0:
					break;
				case 1:
					break;
				case 2:
					break;
		}
}

void CalculateMoveBlockParams()
{
	
}

static float GetCurrentSpeed(void)
{
		return 0;
}

bool GetMotorLinearLocation(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, uint32_t *iLocation)
{
		PrivateBlock *pPrivate_t = NULL;
		float fLocation;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;		

		
		pPrivate_t->pMotorControl_t->m_pGetLinearLocation(pPrivate_t->pMotorControl_t->m_pThisPrivate, iMotorID, &fLocation);		
		
		*iLocation = (uint32_t)(fLocation * 10);
		
		return true;		
}

bool GetMotorLinearSpeed(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, uint32_t *iSpeed)
{
		PrivateBlock *pPrivate_t = NULL;
		float fSpeed;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;		

		
		pPrivate_t->pMotorControl_t->m_pGetLinearSpeed(pPrivate_t->pMotorControl_t->m_pThisPrivate, iMotorID, &fSpeed);		
		
		*iSpeed = (uint32_t)(fSpeed * 10);
		
		return true;				
}

bool GetMotionData(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, float *fPos, float *fSpeed)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;		

		pPrivate_t->pMotorControl_t->m_pGetMotorMoveData(pPrivate_t->pMotorControl_t->m_pThisPrivate, iMotorID, fPos, fSpeed);		
		
		return true;
}

bool SetMotorMoveData(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, uint32_t *iTarget, uint32_t *iSpeed)
{
		static PrivateBlock *pPrivate_t = NULL;
		float fTarget = (float)(*iTarget) / 10;
		float fSpeed = (float)(*iSpeed) / 10;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;	
		
		pPrivate_t->pMotorControl_t->m_pSetMoveParams(pPrivate_t->pMotorControl_t->m_pThisPrivate, iMotorID, &fTarget, &fSpeed);
		
		return true;
}

bool SetMotorDirPinHighAsForward(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, bool *bValue)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;		

		pPrivate_t->pMotorControl_t->m_pSetDirPinHighAsForward(pPrivate_t->pMotorControl_t->m_pThisPrivate, *iMotorID, *bValue);
		
		return true;		
}

void ExeMotionBlcok(PRIVATE_MEMBER_TYPE *m_pThisPrivate, CmdDataObj *eCmdType)
{
		static PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == m_pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		

		pPrivate_t = (PrivateBlock *)m_pThisPrivate;	
		
		pPrivate_t->pMotorControl_t->m_pExeMotorControl(pPrivate_t->pMotorControl_t->m_pThisPrivate, eCmdType);
}

//运动控制块初始化
bool MotionBlockInit(MotionManageBlock *Block_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:malloc private block failed", __FUNCTION__);
				return false;		
		}
		
		pPrivate_t->strType = MOTION_TYPE_NAME;
				
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				pPrivate_t->pMotorControl_t = (MotorControl *)malloc(sizeof(MotorControl));
				if(NULL == pPrivate_t->pMotorControl_t)
				{
						printf("\r\nfunc:%s:malloc motor block failed", __FUNCTION__);
						return false;		
				}
				
				if(!MotorControlInit(pPrivate_t->pMotorControl_t))
				{
						printf("\r\nfunc:%s:init motor block failed", __FUNCTION__);
						return false;								
				}
		#elif 
				
		#endif	
		
		Block_t->m_pThisPrivate = pPrivate_t;
		Block_t->m_pHomeAxisImmediately = HomeAxisImmediately;
		Block_t->m_pSetMotorMoveData = SetMotorMoveData;
		Block_t->m_pAddMotorLimit = AddMotorLimitSwitch;
		Block_t->m_pAddMotor = AddMotor;
		Block_t->m_pReadMotorLimit = ReadMotorLimit;
		Block_t->m_pGetMotionData = GetMotionData;
		Block_t->m_ExeMotionBlcok = ExeMotionBlcok;
		Block_t->m_pGetMotorLinearLocation = GetMotorLinearLocation;
		Block_t->m_pGetMotorLinearSpeed = GetMotorLinearSpeed;
		Block_t->m_pSetMotorDirPinHighAsForward = SetMotorDirPinHighAsForward;
				
		return true;
}
