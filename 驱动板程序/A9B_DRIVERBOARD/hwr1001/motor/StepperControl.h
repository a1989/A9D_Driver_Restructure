#ifndef __STEPPERCONTROL_H__
#define __STEPPERCONTROL_H__

#include "defines.h"
#include "stm32f1xx_hal.h"

//typedef struct 
//{
//		//����˶�����(mm)
//		float fMoveDistance;
//		//��ĵ���˶�����(steps)
//		uint32_t iMotorSteps;
//		//��ĵ���˶��ٶ�(steps/s)
//		uint32_t iMotorStepsPerSecond;
//		//�������˶�����
//		uint32_t iEncoderSteps;
//		//�������˶��ٶ�
//		uint32_t iEncoderStepsPerSecond;
//		float fAcceleration;
//		float fDeceleration;
//		float fStartSpeed;
//		float fEndSpeed;
//}StepperParams;

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		void (*m_pSingleStepperPrepare)(AxisEnum eAxisIndex, float fDistance, float fSpeed);
		void (*m_pPulse)();
		void (*m_pSetPositionEnforce)(PRIVATE_MEMBER_TYPE *pPrivate, float fPosition);
		bool (*m_pStepperPrepare)(PRIVATE_MEMBER_TYPE *pPrivate, float fTarget, float fSpeed);
		bool (*m_pStepperForward)(PRIVATE_MEMBER_TYPE *pPrivate);
		bool (*m_pStepperBackward)(PRIVATE_MEMBER_TYPE *pPrivate);
}StepperControl;

void StepperControlInit(StepperControl *Stepper_t, StepperParams *Params_t);

#endif
