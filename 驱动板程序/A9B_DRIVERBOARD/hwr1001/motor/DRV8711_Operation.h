#ifndef __DRV8711_H__
#define __DRV8711_H__

#include "defines.h"
#include "spi.h"

typedef struct
{	
		DriverConfigMode eMode;
		uint8_t iCurrentCfg;
		uint16_t iSubdivisionCfg;
		void *pDriverPinConfig;
}DRV8711_Params;

typedef struct
{
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
}DRV8711_Control;

bool DRV8711_Init(DRV8711_Control *Block_t, DRV8711_Params *Params_t);

#endif
