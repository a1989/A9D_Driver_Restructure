#ifndef __MOVECONTROL_H__
#define __MOVECONTROL_H__

#include "stm32f1xx_hal.h"
#include "stdbool.h"
#include "DriverBoardConfig.h"

typedef uint8_t MoveBlockMsg;

typedef struct
{
		//���æ��־
		bool bBusy;
		//����λ�ñ�־
		bool bDistanceArrived;
		//ÿ������˶�����(mm)
		uint32_t fMoveDistance[AXIS_NUM];
		//ÿ����ĵ���˶�����(steps)
		uint32_t iMotorSteps[AXIS_NUM];
		//ÿ����ĵ���˶��ٶ�(steps/s)
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
