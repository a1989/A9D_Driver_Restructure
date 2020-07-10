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
		void (*m_pSetTIM_OC)(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *hTIM, uint32_t iPos);
		void (*m_pPulse)();
		void (*m_pStepperStop)(PRIVATE_MEMBER_TYPE *pPrivate);
		bool (*m_pStepperMove)(PRIVATE_MEMBER_TYPE *pPrivate);
		void (*m_pSetPositionEnforce)(PRIVATE_MEMBER_TYPE *pPrivate, float fPosition);
		bool (*m_pStepperPrepare)(PRIVATE_MEMBER_TYPE *pPrivate, float fTarget, float fSpeed);
		bool (*m_pStepperForward)(PRIVATE_MEMBER_TYPE *pPrivate);
		bool (*m_pStepperBackward)(PRIVATE_MEMBER_TYPE *pPrivate);
		bool (*m_pSetStepperDirHighAsForward)(PRIVATE_MEMBER_TYPE *pPrivate, bool bValue);
		TIM_HandleTypeDef *(*m_pGetStepperTimHandle)(PRIVATE_MEMBER_TYPE *pPrivate);
		bool (*m_pIsStepperStop)(PRIVATE_MEMBER_TYPE *pPrivate);
		bool (*m_pIsDirectionReverse)(PRIVATE_MEMBER_TYPE *pPrivate);
}StepperControl;

void StepperControlInit(StepperControl *Stepper_t, StepperParams *Params_t);

#endif
