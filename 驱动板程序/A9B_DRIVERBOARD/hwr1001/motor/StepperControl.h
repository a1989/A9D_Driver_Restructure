#ifndef __STEPPERCONTROL_H__
#define __STEPPERCONTROL_H__

#include "defines.h"
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

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		void (*m_pSingleStepperPrepare)(AxisEnum eAxisIndex, float fDistance, float fSpeed);
		void (*m_pPulse)();
}StepperControl;

void StepperControlInit(StepperControl *Stepper_t, StepperParams *Params_t);

#endif
