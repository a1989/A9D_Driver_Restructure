#ifndef __DRIVERBOARDCONFIG_H__
#define __DRIVERBOARDCONFIG_H__

#include <stdbool.h>

#define DISTRIBUTE	0
#define LINKAGE		1
#define DRIVER_BOARD_MODE	DISTRIBUTE

#define USE_DRV8711	0
#define USE_TMC2590	1
#define MOTOR_DRIVER	USE_DRV8711

#define AXIS_NUM	3

typedef enum 
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
	UNKNOWN_AXIS
}AxisEnum;

#define X_MAX_POS	500
#define Y_MAX_POS	400
#define Z_MAX_POS	300
#define X_MIN_POS	0
#define Y_MIN_POS	0
#define Z_MIN_POS	0

#define X_MAX_LENGTH	(X_MAX_POS - X_MIN_POS)
#define Y_MAX_LENGTH	(Y_MAX_POS - Y_MIN_POS)
#define Z_MAX_LENGTH	(Z_MAX_POS - Z_MIN_POS)

#endif
