#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "DriverBoardConfig.h"

typedef struct
{
		bool arrHomeFlag[AXIS_NUM];
		float arrTarget[AXIS_NUM];
		float arrSpeed[AXIS_NUM];
		MotorType arrMotorType[AXIS_NUM];
}MoveParams;

#endif