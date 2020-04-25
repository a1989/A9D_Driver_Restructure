#include "MotorControl.h"
#include "drv8711.h"

bool MotorDriverInit(MotorDriverMsg *iMsg)
{
		#if (DISTRIBUTE == USE_DRV8711)
			Drv8711_Init();
		#elif DISTRIBUTE == USE_TMC2590
			TMC2590_Init();
		#else
			
		#endif
}