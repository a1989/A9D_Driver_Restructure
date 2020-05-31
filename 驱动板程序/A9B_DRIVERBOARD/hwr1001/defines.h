#ifndef __DEFINES_H__
#define __DEFINES_H__

#include "DriverBoardConfig.h"
#include <string.h>
#include "stm32f1xx_hal.h"
#include "i2c.h"

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
	eMOTOR_DEFAULT = 0,
	eSTEPPER,
	eSTEPPER_ENCODER,
	eBRUSHLESS,
	eBRUSHLESS_ENCODER
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
		eIO = 0,
		eSPI1,
		eSPI2,
		eI2C1,
		eI2C2
}DriverConfigMode;

typedef enum
{
		eNO_DRIVER = 0,
		eDRV8711,
		eTMC2590
}MotorDriver;

typedef enum
{
		eTIM1 = 0,
		eTIM2,
		eTIM3,
		eTIM4
}PulseTIM;

typedef struct
{
		PulseTIM eEncoderTIM;
		uint32_t iEncoderLines;
		uint8_t iMultiplication;
}EncoderParmas;

typedef struct
{
		MotorDriver eDriver;
		DriverConfigMode eConfigMode;
		uint16_t iSubdivision;
		PulseTIM eMotorTIM;
		float fCurrent;		
}StepperParams;

typedef struct
{
		StepperParams StepperParams_t;
		EncoderParmas EncoderParmas_t;
}StepperSysParams;

typedef struct
{
		uint8_t iMotorID;
		MotorType eMotorType;				
		void *MotorSysParams;
}MotorParams;

typedef struct
{
		uint8_t	iAxisNum;
		uint8_t *iMotorID;
		float *fTarget;
		float *fSpeed;
		
}MotorMovementBlock;

typedef enum
{
		eAT24C512 = 0,
		eAT93C66b
}StorageDevice;

typedef struct
{
		uint8_t iID;
		DriverConfigMode eStorageMode;
		StorageDevice eStorageDevice;
}StorageParams;

typedef struct
{
		uint16_t iAddress;
		uint8_t iData;
		HAL_StatusTypeDef eStatus;
}StorageByteOptions;

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