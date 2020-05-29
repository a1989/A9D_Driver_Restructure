#ifndef __MOTORCONTROL_H__
#define __MOTORCONTROL_H__

#include "defines.h"

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		void (*m_pHomeAxis)(PRIVATE_MEMBER_TYPE *pThisPrivate, MoveParams *pParams_t);
		void (*m_pHomeAxisImmediately)(PRIVATE_MEMBER_TYPE *pThisPrivate, MoveParams *pParams_t);
		void (*m_pAddMotor)(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *pParams_t);
		void (*m_pExecuteBlock)(PRIVATE_MEMBER_TYPE *pThisPrivate);
}MotorControl;

//初始化运动节点结构,本结构私有成员指针
void MotorControlInit(MotorControl *Block_t);

#endif
