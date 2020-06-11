#ifndef __INCENCODERCONTROL_H__
#define __INCENCODERCONTROL_H__

#include "defines.h"
#include "interrupt.h"

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		bool (*m_pGetEncoderValue)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, int32_t *iValue);
		void (*m_pInterruptHandler)(PRIVATE_MEMBER_TYPE *m_pThisPrivate, TIM_HandleTypeDef *hTIM);
		bool (*m_pIncEncoderTargetArrived)(PRIVATE_MEMBER_TYPE *m_pThisPrivate);
}IncEncoderControl;

typedef struct IncEncoderTable
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		struct IncEncoderTable *pNext;
}IncEncoderTableInt;

void IncEncoderControlInit(IncEncoderControl *Block_t, EncoderParmas *Params_t);
bool RegisterEncoderVar(PRIVATE_MEMBER_TYPE *pThisPrivate, IncEncoderTableInt *IncEncoderTable_t);
void IncEncoderIntHandler(PRIVATE_MEMBER_TYPE *m_pThisPrivate, TIM_HandleTypeDef *htim);

#endif