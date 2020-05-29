#ifndef __INCENCODERCONTROL_H__
#define __INCENCODERCONTROL_H__

#include "defines.h"



typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
}IncEncoderControl;

void IncEncoderControlInit(IncEncoderControl *Block_t, EncoderParmas *Params_t);

#endif