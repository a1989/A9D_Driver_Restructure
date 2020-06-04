#include "MotionControl.h"
#include "MotorControl.h"
#include <stdlib.h>

#define MOTION_TYPE_NAME	"MotionTypedef"

//本模块的私有部分,用于控制电机,扩展后可用于控制气缸,电缸,电磁铁等运动机构
typedef struct
{
		MotorControl *pMotorControl_t;
		char *strType;
}PrivateBlock;

//分布式控制时, 根据板ID确定当前控制的是哪个轴
static void SetAxisIndex(const uint8_t iBoardID, AxisEnum *iAxisIndex)
{
		if(0xA1 == iBoardID || 0xB1 == iBoardID)
		{
				*iAxisIndex = X_AXIS;
		}
		else if(0xA2 == iBoardID || 0xB2 == iBoardID)
		{
				*iAxisIndex = Y_AXIS;
		}
		else if(0xA3 == iBoardID || 0xB3 == iBoardID)
		{
				*iAxisIndex = Z_AXIS;
		}
		else
		{
				*iAxisIndex = UNKNOWN_AXIS;
		}
}

static void HomeAxisImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		DEBUG_LOG("\r\nStart home axis")
	
		
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
		Block_t->m_pAddMotor = AddMotor;
		
		return true;
}



void AddMotorLimitSwitch(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *Params_t)
{
		
}



//static void HomeAxis(struct MotionBlock *pThis, MoveParams *Params_t)
//{
//		MoveBlock *MoveControl_t = NULL;
//	
//		MoveControl_t = pThis->m_pMoveControl;
//		MoveControl_t->m_pHomeAxis(MoveControl_t->m_pThis, Params_t);
//}

void ProcessBlockData()
{
	
}

static void GetCoordinate(void)
{
	
}

static void PrepareMove(void)
{
	
}





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
					GetCoordinate();
					PrepareMove();
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

}

static float GetCurrentLocation(void)
{

}

//void PushMoveData(MoveBlock *Block_t, float fTargetPos, float fSpeed)	
//{
//		
//}
