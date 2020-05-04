#ifndef __MOVECONTROL_H__
#define __MOVECONTROL_H__

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "DriverBoardConfig.h"

typedef uint8_t MoveBlockMsg;

typedef struct
{
		//电机忙标志
		bool bBusy;
		//到达位置标志
		bool bDistanceArrived;
		//每个轴的运动距离(mm)
		uint32_t fMoveDistance[AXIS_NUM];
		//每个轴的电机运动步数(steps)
		uint32_t iMotorSteps[AXIS_NUM];
		//每个轴的电机运动速度(steps/s)
		uint32_t iMotorStepsPerSecond[AXIS_NUM];
		uint32_t iEncoderSteps[AXIS_NUM];
		uint32_t iEncoderStepsPerSecond[AXIS_NUM];
		double dAcceleration;
		double dDeceleration;
		double dStartSpeed;
		double dEndSpeed;
}MoveNode;

typedef struct
{
		MoveNode arrMoveBuffer[MOVE_NODE_NUM];
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
}MoveBlock;

void MoveBlockInit(MoveBlock *structBlock, MoveBlockMsg *iMsg);

#endif
