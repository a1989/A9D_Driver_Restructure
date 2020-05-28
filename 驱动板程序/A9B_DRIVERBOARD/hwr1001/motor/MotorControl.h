#ifndef __MOTORCONTROL_H__
#define __MOTORCONTROL_H__

#include "defines.h"

typedef struct
{
		
}MotorParams;

typedef struct
{
		void *m_pThisPrivate;
		void (*m_pHomeAxis)(void *pThisPrivate, MoveParams *pParams_t);
		void (*m_pHomeAxisImmediately)(void *pThisPrivate, MoveParams *pParams_t);
		void (*m_pExecuteBlock)(void *pThisPrivate);
}MotorBlock;

//��ʼ���˶��ڵ�ṹ,���ṹ˽�г�Աָ��
void MotorBlockInit(MotorBlock *Block_t, MotorParams *Params_t);

#endif
