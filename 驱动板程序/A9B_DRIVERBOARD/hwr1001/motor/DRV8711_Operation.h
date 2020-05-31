#ifndef __DRV8711_H__
#define __DRV8711_H__

#include "defines.h"
#include "spi.h"

typedef struct
{	
		DriverMode eMode;
		uint8_t iCurrent;
		uint16_t iSubdivision;
}DRV8711_Params;

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
}DRV8711_Control;

void DRV8711_Init(DRV8711_Control *Block_t, DRV8711_Params *Params_t);

#endif
