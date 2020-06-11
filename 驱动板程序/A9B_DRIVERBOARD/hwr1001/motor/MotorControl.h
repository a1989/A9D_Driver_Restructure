#ifndef __MOTORCONTROL_H__
#define __MOTORCONTROL_H__

#include "defines.h"

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
	
//		void (*m_pHomeAxis)(PRIVATE_MEMBER_TYPE *pThisPrivate, MoveParams *pParams_t);
		bool (*m_pMotorHomeImmediately)(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, float fSpeed);
		bool (*m_pAddMotor)(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *pParams_t);
		void (*m_pExecuteBlock)(PRIVATE_MEMBER_TYPE *pThisPrivate);
		bool (*m_pSetMoveParams)(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, float fDist, float fSpeed);
		uint16_t (*m_pGetMotorPulseParamByID)(uint8_t iMotorID);
		uint16_t (*m_pGetMotorMoveParamByTIM)(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *htim, uint16_t *iData);
		bool (*m_pAddLimits)(PRIVATE_MEMBER_TYPE *pThisPrivate, LimitParams *Params_t);
		bool (*m_pReadLimitByID)(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, LimitFunction eFunc, bool *bStatu);
		void (*m_pExeMotorControl)(PRIVATE_MEMBER_TYPE *pThisPrivate);
}MotorControl;


//初始化运动节点结构,本结构私有成员指针
bool MotorControlInit(MotorControl *Block_t);
//bool RegisterMotorVar(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorControl *MotorVar);
void MotorIntHandler(PRIVATE_MEMBER_TYPE *pPrivate, TIM_HandleTypeDef *htim);

#endif
