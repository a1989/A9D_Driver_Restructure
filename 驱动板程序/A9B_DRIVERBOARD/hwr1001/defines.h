#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "DriverBoardConfig.h"

typedef enum
{
		DISTRIBUTE = 0,
		ALL_IN_ONE
}BoardMode;

typedef enum
{
	//���Ƶ�һ��
	CONTROL_SINGLE_AXIS = 0,
	//���ƶ����
	CONTROL_MULTI_AXIS,
}MotorControlMode;		

typedef enum
{
		//������
		NO_LINKAGE = 0,
		//��һ���͵������
		LINKAGE_MOTOR_SINGLE,
		//������͵������
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

#if HARDWARE_VERSION == CHENGDU_DESIGN
#define X_AXIS_TIM	htim2
#define Y_AXIS_TIM	htim2
#define Z_AXIS_TIM	htim2

#endif

#endif