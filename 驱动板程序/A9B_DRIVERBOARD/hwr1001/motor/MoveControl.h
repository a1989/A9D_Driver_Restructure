#ifndef __MOVECONTROL_H__
#define __MOVECONTROL_H__

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "DriverBoardConfig.h"
#include "defines.h"

typedef struct Move_t
{
		void *m_pThisPrivate;
		void (*m_pHomeAxis)(void *pThisPrivate, MoveParams *pParams_t);
		void (*m_pHomeAxisImmediately)(void *pThisPrivate, MoveParams *pParams_t);
		void (*m_pExecuteBlock)(void *pThisPrivate);
}MoveBlock;

//��ʼ���˶��ڵ�ṹ,���ṹthisָ��
void MoveBlockInit(MoveBlock *Block_t);

#endif
