#include "StepperControl.h"

//ת��,����
const uint16_t arrSpeedTable[ACC_TIME_DIVISION][2] = {
{49,0},
{199,3},
{449,8},
{799,15},
{1250,25},
{1800,36},
{2450,49},
{3199,63},
{4049,80},
{4999,99},
{5000,100},
{5950,119},
{6800,136},
{7550,151},
{8200,164},
{8750,175},
{9200,184},
{9550,191},
{9800,196},
{9950,199}
};

void SingleEncoderStepperPrepare(AxisEnum eAxisIndex, float fDistance, float fSpeed)
{
		
}

void StepperControlInit(StepperControl *Stepper_t)
{
		Stepper_t->m_pSingleEncoderStepperPrepare = SingleEncoderStepperPrepare;
}


