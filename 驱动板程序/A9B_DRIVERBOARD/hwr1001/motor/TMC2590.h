#ifndef __TMC2590_H
#define __TMC2590_H

#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include <stdbool.h>

//TMC2590寄存器地址长度为20bit
#define   REG_DRVCTRL       0x00000000  
#define   REG_CHOPCONF      0x00080000
#define   REG_SMARTEN       0x000A0000    //CoolStep Control Register
#define   REG_SGCSCONF      0x000C0000    //StallGuard2 Control Register
#define   REG_DRVCONF       0x000E0000

#define   REG_CURRENT       REG_SGCSCONF        //设置电流的寄存器
#define   REG_TORQUE        REG_SGCSCONF        //设置力矩的寄存器

#define   MICROSTEP_256       0x00
#define   MICROSTEP_128       0x01
#define   MICROSTEP_64        0x02
#define   MICROSTEP_32        0x03
#define   MICROSTEP_16        0x04
#define   MICROSTEP_8         0x05
#define   MICROSTEP_4         0x06
#define   MICROSTEP_2         0x07
#define   MICROSTEP_1         0x08

#define   DRVCONF_BRIDGE_OFF      0x0000

#define   SGCSCONF_DEFAULT        0x0000

#define   DEBUG_USE_USART       1

#define   BIT_STST  7
#define   BIT_OLB  6
#define   BIT_OLA  5
#define   BIT_SHORTB  4
#define   BIT_SHORTA  3
#define   BIT_OTPW  2
#define   BIT_OT  1
#define   BIT_SG  0

#define		SG_POS			0
#define		SG_OT				1
#define		SG_OTPW			2
#define		SG_SHORTA		3
#define		SG_SHORTB		4
#define		SG_OLA			5
#define		SG_OLB			6
//#define   ReadDataBit(iStatu, iBit)   (  (iStatu) & (1 << (iBit)) )


extern uint8_t  motor_step_value;
extern uint8_t  TMC2590_error_status;

/*需配置的寄存器结构，以位域分割*/

//从低位开始占位
typedef struct 
{
  uint32_t iTOFF : 4;
  uint32_t iHSTRT : 3;
  uint32_t iHEND : 4;
  uint32_t iHDEC : 2;
  uint32_t iRNDTF : 1;
  uint32_t iCHM : 1;
  uint32_t iTBL : 2;
  uint32_t  : 3;
  uint32_t  : 4; 
  uint32_t  : 8;
}StructRegCHOPCONF;

typedef union
{
  StructRegCHOPCONF structReg;
  uint32_t iRegValue;
}unRegCHOPCONF;

typedef struct 
{
  uint32_t iSEMIN : 4;
  uint32_t  : 1;
  uint32_t iSEUP : 2;
  uint32_t  : 1;
  uint32_t iSEMAX : 4;
  uint32_t  : 1;
  uint32_t iSEDN : 2;
  uint32_t iSEIMIN : 1;
  uint32_t  : 8;
  uint32_t  : 8;  
}StructRegSMARTEN;

typedef union
{
  StructRegSMARTEN structReg;
  uint32_t iRegValue;
}unRegSMARTEN;

typedef struct
{
  uint32_t iEN_S2VS : 1;
  uint32_t iEN_PFD : 1;
  uint32_t iSHRTSENS : 1;
  uint32_t iOTSENS : 1;
  uint32_t iRDSEL : 2;
  uint32_t iVSENSE : 1;
  uint32_t iSDOFF : 1;
  uint32_t iTS2G : 2;
  uint32_t iDIS_S2G : 1;
  uint32_t iSLP2 : 1;
  uint32_t iSLPL : 2;
	uint32_t iSLPH : 2;
  uint32_t iTST : 1;
  uint32_t : 3; 
  uint32_t : 4;
  uint32_t : 8;
}StructRegDRVCONF;

typedef union
{
  StructRegDRVCONF structReg;
  uint32_t iRegValue;
}unRegDRVCONF;

typedef struct
{
  uint32_t iMRES : 4;
  uint32_t : 4;
  uint32_t iDEDGE : 1;
  uint32_t iINTPOL : 1;
  uint32_t : 6; 
  uint32_t : 8;
  uint32_t : 8;
}StructRegDRVCTRL;

typedef union
{
  StructRegDRVCTRL structReg;
  uint32_t iRegValue;
}unRegDRVCTRL;

typedef struct
{
  uint32_t iCS : 5;
  uint32_t : 3;
  uint32_t iSGT : 7;
  uint32_t : 1;
  uint32_t iSFILT: 1; 
  uint32_t : 7;
  uint32_t : 8;
}StructRegSGCSCONF;

typedef union
{
  StructRegSGCSCONF structReg;
  uint32_t iRegValue;
}unRegSGCSCONF;

/***************/

typedef enum
{
  MICRO_STEP_POSITION       = 0x0,
  STALL_GUARD_VALUE         = 0x1,
  STALL_GUARD_COOL_STEP     = 0x2,
  ALL_STATUS                = 0x3
} ReadSelect;


typedef struct
{
  uint32_t iSG : 1;
  uint32_t iOT : 1;
  uint32_t iOTPW : 1;
  uint32_t iSHORT : 2;
  uint32_t iOL : 2;
  uint32_t iSTST : 1;
  uint32_t : 2;
  uint32_t iSRDSEL : 10;
  uint32_t : 4;
  uint32_t : 8;
}StructRegRead;

typedef union
{
  StructRegRead structRead;
  uint32_t iRead;
}unReadStatus;

typedef struct
{
	uint16_t StallGuard;
	uint8_t  CoolStepScaling;
	uint32_t iValue;
	uint8_t  iStallOccurs;
}StructStatus;

#define SPI_TMC2590_CS_HIGH()   HAL_GPIO_WritePin (SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_SET)
#define SPI_TMC2590_CS_LOW()    HAL_GPIO_WritePin (SPI2_CS_GPIO_Port, SPI2_CS_Pin, GPIO_PIN_RESET)

#define TMC2590_SLEEP_PIN_HIGH()   HAL_GPIO_WritePin (Motor_nSLEEP_GPIO_Port, Motor_nSLEEP_Pin, GPIO_PIN_SET)
#define TMC2590_SLEEP_PIN_LOW()    HAL_GPIO_WritePin (Motor_nSLEEP_GPIO_Port, Motor_nSLEEP_Pin, GPIO_PIN_RESET)

#define TMC2590_RESET_PIN_HIGH()   HAL_GPIO_WritePin (Motor_RESET_GPIO_Port, Motor_RESET_Pin, GPIO_PIN_SET)
#define TMC2590_RESET_PIN_LOW()    HAL_GPIO_WritePin (Motor_RESET_GPIO_Port, Motor_RESET_Pin, GPIO_PIN_RESET)

HAL_StatusTypeDef TMC2590_ReadSingleStatus(ReadSelect iSelect, uint32_t *iValue);
HAL_StatusTypeDef TMC2590_SetReg (uint32_t iRegAddr, uint32_t iValue);
bool TMC2590_SetMicroStep (uint16_t iValue, HAL_StatusTypeDef *iErrorCode);
bool TMC2590_Init (void);
HAL_StatusTypeDef SPI_TMC2590_SendByte (uint32_t iWriteData, uint32_t *iRecvData);
void TMC2590_GetStatus(StructStatus *structStatus);

#endif
