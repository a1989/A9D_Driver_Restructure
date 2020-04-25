#ifndef __MOVECONTROL_H__
#define __MOVECONTROL_H__

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "DriverBoardConfig.h"

typedef uint8_t MoveBlockMsg;

typedef struct
{
		bool bBusy;
		bool bDistanceArrived;
		uint32_t iMotorTotalSteps[AXIS_NUM];
		uint32_t iMotorStepsPerSecond[AXIS_NUM];
		uint32_t iEncoderTotalSteps[AXIS_NUM];
		uint32_t iEncoderStepsPerSecond[AXIS_NUM];
		double dAcceleration;
		double dDeceleration;
		double dStartSpeed;
		double dEndSpeed;
		
}MoveBlock;

void MoveBlockInit(MoveBlock *structBlock, MoveBlockMsg *iMsg);

#endif
