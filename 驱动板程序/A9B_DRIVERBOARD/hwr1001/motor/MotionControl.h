#ifndef __MOTIONCONTROL_H__
#define __MOTIONCONTROL_H__

#include <stdbool.h>
#include "includes.h"
#include "DriverBoardConfig.h"
#include "defines.h"

#define QUEUELENGTH	8

typedef struct MotionBlock
{					
		void *m_pMoveControl;
		MoveParams Params_t;
	
		void (*m_pMotorDriverInit)(struct MotionBlock *pThis);
		void (*m_pSetMotionData)(struct MotionBlock *pThis, const MoveParams *Params_t);
		void (*m_pHomeAxis)(struct MotionBlock *pThis, MoveParams *Params_t);
		void (*m_pGetMotionData)(struct MotionBlock *pThis, MoveParams *Params_t);
}MotionManageBlock;

bool MotionBlockInit(MotionManageBlock *Block_t, MoveParams *Params_t);
void MotionControlInit(void);

#endif