#ifndef __MOTIONCONTROL_H__
#define __MOTIONCONTROL_H__

#include <stdbool.h>
#include "includes.h"
#include "DriverBoardConfig.h"
#include "MoveControl.h"

#define QUEUELENGTH	8

typedef uint8_t AxisIndex;
typedef uint8_t MotionBlockMsg;
	
typedef struct
{
		MoveBlock arrMoveBlockQueue[QUEUELENGTH];	
		//
		uint8_t iBlockError;
		uint8_t iQueueIndex;
		AxisEnum iAxisIndex;
		
		void (*m_pSetAxisIndex)(const uint8_t iBoardID, AxisEnum *iAxisInex);
		void ();
}MotionManageBlock;

void MotionBlockInit(MotionManageBlock *structBlock, MotionBlockMsg *iMsg);

#endif