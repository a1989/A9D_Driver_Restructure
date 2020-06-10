#ifndef __MOTIONCONTROL_H__
#define __MOTIONCONTROL_H__

#include "defines.h"

#define QUEUELENGTH	8

//typedef struct
//{
//		MultiAxisMode eAxisMode;
//		MotorControlMode eMotorMode;
//		MotorType *eMotorType;
//		uint8_t iAxisNum;
//}MotionParams;

typedef struct
{					
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		
		void (*m_pAddMotor)(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *Params_t);
		bool (*m_pAddMotorLimit)(PRIVATE_MEMBER_TYPE *pThisPrivate, LimitParams *Params_t);
		bool (*m_pSetMotorMoveData)(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, uint32_t iTarget, uint32_t iSpeed);
		bool (*m_pReadMotorLimit)(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, LimitFunction eFunc, bool *bStatu);
//		bool (*m_pGetMotorMoveParamByTIM)(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *htim, uint16_t *iData);
//		void (*m_pSetMotorMovement)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, const MoveParams *Params_t);
//		void (*m_pHomeAxis)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MoveParams *Params_t);
		void (*m_pHomeAxisImmediately)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, uint8_t iMotorID, uint32_t iSpeed);
//		void (*m_pGetMotionData)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MoveParams *Params_t);
		bool (*m_GetMotorPosition)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, uint8_t iMotorID, float *fPosition);
		bool (*m_GetMotorSpeed)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, uint8_t iMotorID, float *fSpeed);
		void (*m_ExeMotionBlcok)(PRIVATE_MEMBER_TYPE *m_pThisPrivate);
}MotionManageBlock;

bool MotionBlockInit(MotionManageBlock *Block_t);

#endif
