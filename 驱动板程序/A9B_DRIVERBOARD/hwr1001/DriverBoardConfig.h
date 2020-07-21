#ifndef __DRIVERBOARDCONFIG_H__
#define __DRIVERBOARDCONFIG_H__

//#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define USE_DRV8711	0
#define USE_TMC2590	1
#define MOTOR_DRIVER		USE_DRV8711

#define CHENGDU_DESIGN				1
#define SHENZHEN_DESIGN_V1		2
#define HARDWARE_VERSION			CHENGDU_DESIGN

#define USE_AT24C512	0
#define USE_AT93C66		1
#define DRIVER_STORAGE		USE_AT24C512

#define STEPPER_WITHOUT_ENCODER		0
#define STEPPER_WITH_ENCODER		1
#define STERPER_TYPE	STEPPER_WITH_ENCODER

//联动控制时控制机械臂的组数
#define ARM_SYS_NUM		1

#define AXIS_NUM	3

#define X_MAX_POS	500
#define Y_MAX_POS	400
#define Z_MAX_POS	300
#define X_MIN_POS	0
#define Y_MIN_POS	0
#define Z_MIN_POS	0

#define X_MAX_LENGTH	(X_MAX_POS - X_MIN_POS)
#define Y_MAX_LENGTH	(Y_MAX_POS - Y_MIN_POS)
#define Z_MAX_LENGTH	(Z_MAX_POS - Z_MIN_POS)

#define RECV_HOST_DATA_SIZE		8
#define SEND_HOST_DATA_SIZE		8
#define RECV_BUFFER_SIZE	24
#define SEND_BUFFER_SIZE	24

#define MOVE_NODE_NUM		16

#define DRIVER_DATA_CHANNEL	CAN1

#endif
