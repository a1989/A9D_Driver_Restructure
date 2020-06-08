#ifndef __DEFINES_H__
#define __DEFINES_H__

//#include <stdlib.h>
#include "DriverBoardConfig.h"
#include <string.h>
#include "delay.h"
#include "i2c.h"

#define PRIVATE_MEMBER_TYPE		void
#define VOID_HandleTypeDef		void
#define IS_DATA_TYPE_CORRECT(strParams1, strParams2)		(!strcmp(strParams1, strParams2))

#define DEBUG_LOG_ENABLE	1
#define DEBUG_LOG(strParams, ...)		if(DEBUG_LOG_ENABLE)\
																		{\
																				printf(strParams, ##__VA_ARGS__);\
																		}

#define MAX_LENGTH	3000		//mm																		

typedef struct
{
		uint8_t iMajorVersion;
		uint8_t iMinorVersion;
		uint8_t iDriverID;
}DevInfo;																		
																		
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
		uint16_t iSubdivisionCfg;
		uint8_t iCurrentCfg;
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
		//指向一个电机配置参数
		void *pMotorSysParams;		
}MotorParams;

typedef enum
{
		eNegative = 0,
		eZero,
		ePositive
}LimitFunction;

typedef struct
{
		GPIO_TypeDef *GPIO_Port;
		uint16_t GPIO_Pin;
		uint8_t iMotorBelong;
		LimitFunction eFunc;
}LimitParams;

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
		uint8_t iHardwareAddress;
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

typedef enum
{
		eNEG_LIMIT = 0,
		eZERO_LIMIT,
		ePOS_LIMIT
}eLimitType;

#define HEART_BEAT_DATA		0x81

#define X_AXIS_INDEX		0
#define Y_AXIS_INDEX		1
#define Z_AXIS_INDEX		2
#define RX_AXIS_INDEX		3
#define RY_AXIS_INDEX		4
#define RZ_AXIS_INDEX		5

#if HARDWARE_VERSION == CHENGDU_DESIGN
		#define AT24C512_HARDWARE_ADDR		0xA0
		#define EEPROM_ID_CFG_ADDR		0xF0
		#define EEPROM_CURRENT_CFG_ADDR		0xF1
		#define EEPROM_SUBDIVISION_CFG_ADDR		0xF2
		
		#define ENCODER_LINES		1000
		#define ENCODER_MULTIPLY		4
#endif

#define BIN_0			0x0
#define BIN_1			0x1
#define BIN_10		0x2
#define BIN_11		0x3
#define BIN_100		0x4
#define BIN_101		0x5
#define BIN_110		0x6
#define BIN_111		0x7
#define BIN_1000	0x8
#define BIN_1001	0x9
#define BIN_1010	0xA
#define BIN_1011	0xB
#define BIN_1100	0xC
#define BIN_1101	0xD
#define BIN_1110	0xE
#define BIN_1111	0xF

#endif

