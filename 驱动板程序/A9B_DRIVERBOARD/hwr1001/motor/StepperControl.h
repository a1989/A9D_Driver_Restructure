#ifndef __STEPPERCONTROL_H__
#define __STEPPERCONTROL_H__

#include "DriverBoardConfig.h"
#include "stm32f1xx_hal.h"

typedef struct 
{
		//轴的运动距离(mm)
		float fMoveDistance;
		//轴的电机运动步数(steps)
		uint32_t iMotorSteps;
		//轴的电机运动速度(steps/s)
		uint32_t iMotorStepsPerSecond;
		//编码器运动步数
		uint32_t iEncoderSteps;
		//编码器运动速度
		uint32_t iEncoderStepsPerSecond;
		float fAcceleration;
		float fDeceleration;
		float fStartSpeed;
		float fEndSpeed;
}StepperParams;

#endif
