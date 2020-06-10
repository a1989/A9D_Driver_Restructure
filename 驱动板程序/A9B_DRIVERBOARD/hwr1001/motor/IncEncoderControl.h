#ifndef __INCENCODERCONTROL_H__
#define __INCENCODERCONTROL_H__

#include "defines.h"
#include "interrupt.h"

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		bool (*m_pGetEncoderValue)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, int32_t *iValue);
		void (*m_pInterruptHandler)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, TIM_HandleTypeDef *hTIM);
}IncEncoderControl;

void IncEncoderControlInit(IncEncoderControl *Block_t, EncoderParmas *Params_t);
void RegisterEncoderVar(IncEncoderVar *Var_t, PRIVATE_MEMBER_TYPE *pThisPrivate, void (*pIntHandler)(PRIVATE_MEMBER_TYPE *pThisPrivate, TIM_HandleTypeDef *htim));
void IncEncoderInterruptHandler(PRIVATE_MEMBER_TYPE *m_pThisPrivate, TIM_HandleTypeDef *htim);

#endif