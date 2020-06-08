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
		bool (*m_pGetMotorMoveParamByTIM)(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *htim, uint16_t *iData);
		void (*m_pSetMotorMovement)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, const MoveParams *Params_t);
		void (*m_pHomeAxis)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MoveParams *Params_t);
		void (*m_pHomeAxisImmediately)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, uint8_t iMotorID, uint32_t iSpeed);
		void (*m_pGetMotionData)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MoveParams *Params_t);
}MotionManageBlock;

bool MotionBlockInit(MotionManageBlock *Block_t);

#endif