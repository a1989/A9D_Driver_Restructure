#ifndef __MOTIONCONTROL_H__
#define __MOTIONCONTROL_H__

#include <stdbool.h>
#include "includes.h"
#include "DriverBoardConfig.h"
#include "MoveControl.h"

#define QUEUELENGTH	8

typedef uint8_t AxisIndex;
typedef uint8_t MotionBlockMsg;



typedef struct MotionBlock
{
		MoveBlock MoveBlock_t;	
		//
		uint8_t iBlockError;
		uint8_t iQueueIndex;
		AxisEnum iAxisIndex;
		
		void (*m_pSetAxisIndex)(const uint8_t iBoardID, AxisEnum *iAxisInex);
		void (*m_pSetMotionData)(struct MotionBlock *Block_t);
}MotionManageBlock;

bool MotionBlockInit(MotionManageBlock *structBlock);
void MotionControlInit(void);
void SetMotionData(MotionManageBlock *Block_t);

#endif