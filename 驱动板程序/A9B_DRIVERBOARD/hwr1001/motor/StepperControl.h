#ifndef __STEPPERCONTROL_H__
#define __STEPPERCONTROL_H__

#include "defines.h"
#include "stm32f1xx_hal.h"

typedef struct 
{
		//����˶�����(mm)
		float fMoveDistance;
		//��ĵ���˶�����(steps)
		uint32_t iMotorSteps;
		//��ĵ���˶��ٶ�(steps/s)
		uint32_t iMotorStepsPerSecond;
		//�������˶�����
		uint32_t iEncoderSteps;
		//�������˶��ٶ�
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
