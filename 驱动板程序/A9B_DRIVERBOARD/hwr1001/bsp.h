#ifndef __BSP_H
#define __BSP_H
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"

typedef struct
{
		uint8_t iDriverSubdivision;
		uint8_t iDiverCurrent;
		uint8_t iID;
}BasicParams_t;

//typedef struct
//{
//		
//}BSP_Init_t;

void BSP_Initializes(void);

#endif
