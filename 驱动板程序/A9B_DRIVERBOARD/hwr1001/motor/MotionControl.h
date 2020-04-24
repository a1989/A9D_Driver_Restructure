#ifndef __MOTIONCONTROL_H__
#define __MOTIONCONTROL_H__

#include <stdbool.h>
#include "includes.h"

#define QUEUELENGTH	8

typedef struct
{
		bool bBusy;
		bool bDistanceArrived;
		uint32_t iMotorTotalSteps;
		uint32_t iMotorStepsPerSecond;
		uint32_t iEncoderTotalSteps;
		uint32_t iEncoderStepsPerSecond;
		double dAcceleration;
		double dDeceleration;
		double dStartSpeed;
		double dEndSpeed;
}MoveBlock;

typedef struct
{
		MoveBlock arrMoveBlockQueue[QUEUELENGTH];		
		uint8_t iQueueIndex;
}MotionManageBlock;

void MotionBlockInit(MotionManageBlock *structBlock);

#endif