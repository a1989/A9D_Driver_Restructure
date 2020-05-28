#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "DriverBoardConfig.h"
#include <string.h>
#include "stm32f1xx_hal.h"

#define PRIVATE_MEMBER_TYPE		void
#define VOID_HandleTypeDef		void
#define IS_DATA_TYPE_CORRECT(strParams1, strParams2)		(!strcmp(strParams1, strParams2))

typedef enum
{
		DISTRIBUTE = 0,
		ALL_IN_ONE
}BoardMode;

typedef enum
{
	//控制单一轴
	CONTROL_SINGLE_AXIS = 0,
	//控制多个轴
	CONTROL_MULTI_AXIS,
}MotorControlMode;		

typedef enum
{
		//无联动
		NO_LINKAGE = 0,
		//单一类型电机联动
		LINKAGE_MOTOR_SINGLE,
		//混合类型电机联动
		LINKAGE_MOTOR_MIX
}MultiAxisMode;

typedef enum
{
	MOTOR_DEFAULT = 0,
	STEPPER,
	STEPPER_ENCODER,
	BRUSHLESS,
	BRUSHLESS_ENCODER
}MotorType;

typedef enum 
{
	X_AXIS = 0,
	Y_AXIS,
	Z_AXIS,
	RX_AXIS,
	RY_AXIS,
	RZ_AXIS,
	UNKNOWN_AXIS
}AxisEnum;

typedef struct
{
		bool arrHomeFlag[AXIS_NUM];
		float arrTarget[AXIS_NUM];
		float arrSpeed[AXIS_NUM];
		MotorType arrMotorType[AXIS_NUM];
}MoveParams;

typedef enum
{
		eSPI1 = 0,
		eSPI2
}DriverMode;

typedef enum
{
		eDRV8711 = 0,
		eTMC2590
}StepperDriver;

typedef union
{
		int iIntData;
		float fFloatData;
}unData;

#define HEART_BEAT_DATA		0x81

#define X_AXIS_INDEX		0
#define Y_AXIS_INDEX		1
#define Z_AXIS_INDEX		2
#define RX_AXIS_INDEX		3
#define RY_AXIS_INDEX		4
#define RZ_AXIS_INDEX		5

#if HARDWARE_VERSION == CHENGDU_DESIGN

	#define X1_AXIS_TIM		htim2
	#define Y1_AXIS_TIM		htim2
	#define Z1_AXIS_TIM		htim2
	#define RX1_AXIS_TIM 	htim2
	#define RX2_AXIS_TIM	htim2
	#define RX3_AXIS_TIM	htim2

	#define X2_AXIS_TIM		htim2
	#define Y2_AXIS_TIM		htim2
	#define Z2_AXIS_TIM		htim2
	#define RX2_AXIS_TIM 	htim2
	#define RX2_AXIS_TIM	htim2
	#define RX2_AXIS_TIM	htim2

#endif

#endif