#include "DriverStorage.h"

#if HARDWARE_VERSION == CHENGDU_DESIGN
#include "EEPROM_AT24C512.h"
#elif
#endif

bool DriverStorageInit()
{
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				AT24C512_Init();
		#elif
		#endif		
}