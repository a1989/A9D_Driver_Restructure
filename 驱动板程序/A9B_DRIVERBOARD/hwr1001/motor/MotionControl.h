#ifndef __MOTIONCONTROL_H__
#define __MOTIONCONTROL_H__

#include "defines.h"

#define QUEUELENGTH	8

typedef struct
{
		MultiAxisMode eAxisMode;
		MotorControlMode eMotorMode;
		MotorType *eMotorType;
		uint8_t iAxisNum;
}MotionParams;

typedef struct
{					
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		MoveParams Params_t;
		
		void (*m_pAddMotor)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MotionParams *Params_t);
		void (*m_pMotorDriverInit)(PRIVATE_MEMBER_TYPE *m_pThisPrivate);
		void (*m_pSetMotionData)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, const MoveParams *Params_t);
		void (*m_pHomeAxis)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MoveParams *Params_t);
		void (*m_pGetMotionData)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MoveParams *Params_t);
}MotionManageBlock;

bool MotionBlockInit(MotionManageBlock *Block_t, MotionParams *Params_t);

#endif