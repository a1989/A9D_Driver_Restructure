#include "DRV8711_Operation.h"
#include <stdlib.h>
#include <gpio.h>

#define CTRL_REG_ADDR		0x00
#define TORQUE_REG_ADDR		0x01
#define OFF_REG_ADDR		0x02
#define BLANK_REG_ADDR		0x03
#define DECAY_REG_ADDR		0x04
#define STALL_REG_ADDR		0x05
#define DRIVE_REG_ADDR		0x06
#define STATUS_REG_ADDR		0x07

//位段模式,由低位到高位
typedef struct 
{
  uint16_t ENBL : 1;
  uint16_t RDIR : 1;
  uint16_t RSTEP : 1;
  uint16_t MODE : 4;
  uint16_t EXSTALL : 1;
  uint16_t ISGAIN : 2;
  uint16_t DTIME : 2;
	uint16_t  : 4;
}StructRegCTRL;

typedef union
{
  StructRegCTRL structReg;
  uint16_t iRegValue;
}unRegCTRL;
//

//
typedef struct 
{
  uint16_t TORQUE : 8;
  uint16_t SIMPLTH : 3;
  uint16_t  : 1;
}StructRegTORQUE;

typedef union
{
  StructRegTORQUE structReg;
  uint16_t iRegValue;
}unRegTORQUE;
//

//
typedef struct 
{
  uint16_t TOFF : 8;
  uint16_t PWMMODE : 1;
  uint16_t  : 3;
}StructRegOFF;

typedef union
{
  StructRegOFF structReg;
  uint16_t iRegValue;
}unRegOFF;
//

//
typedef struct 
{
  uint16_t TBLANK : 8;
  uint16_t ABT : 1;
  uint16_t  : 3;
}StructRegBLANK;

typedef union
{
  StructRegBLANK structReg;
  uint16_t iRegValue;
}unRegBLANK;
//

//
typedef struct 
{
  uint16_t TDECAY : 8;
  uint16_t DECMOD : 3;
  uint16_t  : 1;
}StructRegDECAY;

typedef union
{
  StructRegDECAY structReg;
  uint16_t iRegValue;
}unRegDECAY;
//

//
typedef struct 
{
  uint16_t SDTHR : 8;
  uint16_t SDCNT : 2;
  uint16_t VDIV : 2;
}StructRegSTALL;

typedef union
{
  StructRegSTALL structReg;
  uint16_t iRegValue;
}unRegSTALL;
//

//
typedef struct 
{
  uint16_t OCPTH : 2;
	uint16_t OCPDEG : 2;
	uint16_t TDRIVEN : 2;
	uint16_t TDRIVEP : 2;
	uint16_t IDRIVEN : 2;
	uint16_t IDRIVEP : 2;
}StructRegDRIVE;

typedef union
{
  StructRegDRIVE structReg;
  uint16_t iRegValue;
}unRegDRIVE;
//

//
typedef struct 
{
  uint16_t OTS : 1;
	uint16_t AOCP : 1;
	uint16_t BOCP : 1;
	uint16_t UVLO : 1;
	uint16_t APDF : 1;
	uint16_t BPDF : 1;
	uint16_t STD : 1;
	uint16_t STDLAT : 1;
	uint16_t  : 4;
}StructRegSTATUS;

typedef union
{
  StructRegSTATUS structReg;
  uint16_t iRegValue;
}unRegSTATUS;
//


typedef struct
{
		SPI_HandleTypeDef hSPI;
		unRegCTRL CTRL_RegValue;		
		unRegTORQUE TORQUE_RegValue;
		unRegOFF OFF_RegValue;
		unRegBLANK BLANK_RegValue;
		unRegDECAY DECAY_RegValue;
		unRegSTALL STALL_RegValue;
		unRegDRIVE DRIVE_RegValue;
		unRegSTATUS STATUS_RegValue;
		DRV8711_PinConfig PinConfig_t;
}PrivateBlock;

static bool WriteSPI(SPI_HandleTypeDef *hSPI, DRV8711_PinConfig *pPinConfig, uint8_t iAddr, uint16_t iData)
{
		uint8_t iSendBuffer[2];
		uint8_t iRecvBuffer[2];
		HAL_StatusTypeDef eStatus;
	
		iSendBuffer[0] = (iData >> 8) & 0xFF;
		iSendBuffer[0] = iSendBuffer[0] | (iAddr << 4);
		iSendBuffer[1] = iData & 0xFF;
		
		DEBUG_LOG("\r\nwrite spi:0x%02x%02x", iSendBuffer[0], iSendBuffer[1])
		
		SET_PIN(pPinConfig->CSPin.GPIO_Port, pPinConfig->CSPin.GPIO_Pin);
	
		eStatus = HAL_SPI_TransmitReceive (hSPI, iSendBuffer, iRecvBuffer, 2, 0xFFFFFF);
		if(HAL_OK != eStatus)
		{
				printf("\r\nwrite 8711 spi failed:%02x", eStatus);
				return false;			
		}
		
		RESET_PIN(pPinConfig->CSPin.GPIO_Port, pPinConfig->CSPin.GPIO_Pin);
		
		return true;
}

static bool ReadSPI(SPI_HandleTypeDef *hSPI, DRV8711_PinConfig *pPinConfig, uint8_t iAddr, uint16_t *iData)
{
		uint8_t iSendBuffer[2];
		uint8_t iRecvBuffer[2];
		uint16_t iRecvData;
		HAL_StatusTypeDef eStatus;
		
		iSendBuffer[0] = iAddr;
		iSendBuffer[1] = 0xFF;
		
		DEBUG_LOG("\r\nwrite spi:0x%02x%02x", iSendBuffer[0], iSendBuffer[1])
		
		SET_PIN(pPinConfig->CSPin.GPIO_Port, pPinConfig->CSPin.GPIO_Pin);
	
		eStatus = HAL_SPI_TransmitReceive (hSPI, iSendBuffer, iRecvBuffer, 2, 0xFFFFFF);
		if(HAL_OK != eStatus)
		{
				printf("\r\nwrite 8711 spi failed:%02x", eStatus);
				return false;			
		}
		
		RESET_PIN(pPinConfig->CSPin.GPIO_Port, pPinConfig->CSPin.GPIO_Pin);
		
		iRecvData = (((uint16_t)iSendBuffer[0]) << 8) | (iSendBuffer[1]);
		*iData = iRecvData;
		
		return true;		
}

bool EnableMotor(PrivateBlock *pPrivate)
{
		pPrivate->CTRL_RegValue.structReg.ENBL = BIN_1;
	
		DEBUG_LOG("\r\nEnable motor:0x%x", pPrivate->CTRL_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);
		return true;
}

bool SetRegisterDefaultCTRL(PrivateBlock *pPrivate)
{
		pPrivate->CTRL_RegValue.structReg.ENBL = BIN_0;
		pPrivate->CTRL_RegValue.structReg.RDIR = BIN_1;
		pPrivate->CTRL_RegValue.structReg.RSTEP = BIN_0;
		pPrivate->CTRL_RegValue.structReg.MODE = BIN_100;
		pPrivate->CTRL_RegValue.structReg.EXSTALL = BIN_0;
		pPrivate->CTRL_RegValue.structReg.ISGAIN = BIN_0;
		pPrivate->CTRL_RegValue.structReg.DTIME = BIN_11;
	
		DEBUG_LOG("\r\nConfig CTRL Reg:0x%x", pPrivate->CTRL_RegValue.iRegValue)
		
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);		
	
		return true;
}

bool SetRegisterDefaultTORQUE(PrivateBlock *pPrivate)
{
		pPrivate->TORQUE_RegValue.structReg.TORQUE = 0xFF;
		pPrivate->TORQUE_RegValue.structReg.SIMPLTH = BIN_1;
	
		DEBUG_LOG("\r\nConfig TORQUE Reg:0x%x", pPrivate->TORQUE_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);		
		
		return true;
}

bool SetRegisterDefaultOFF(PrivateBlock *pPrivate)
{
		pPrivate->OFF_RegValue.structReg.TOFF = 0x40;
		pPrivate->OFF_RegValue.structReg.PWMMODE = BIN_1;
	
		DEBUG_LOG("\r\nConfig OFF Reg:0x%x", pPrivate->OFF_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);	

		return true;
}

bool SetRegisterDefaultBLANK(PrivateBlock *pPrivate)
{
		pPrivate->BLANK_RegValue.structReg.TBLANK = 0x40;
		pPrivate->BLANK_RegValue.structReg.ABT = BIN_1;
	
		DEBUG_LOG("\r\nConfig BLANK Reg:0x%x", pPrivate->BLANK_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);		
	
		return true;
}

bool SetRegisterDefaultDECAY(PrivateBlock *pPrivate)
{
		pPrivate->DECAY_RegValue.structReg.TDECAY = BIN_10;
		pPrivate->DECAY_RegValue.structReg.DECMOD = BIN_1;
	
		DEBUG_LOG("\r\nConfig DECAY Reg:0x%x", pPrivate->DECAY_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);		
	
		return true;
}

bool SetRegisterDefaultSTALL(PrivateBlock *pPrivate)
{
		pPrivate->STALL_RegValue.structReg.SDTHR = BIN_10;
		pPrivate->STALL_RegValue.structReg.SDCNT = BIN_0;
		pPrivate->STALL_RegValue.structReg.VDIV = BIN_11;
	
		DEBUG_LOG("\r\nConfig STALL Reg:0x%x", pPrivate->STALL_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);		
	
		return true;
}

bool SetRegisterDefaultDRIVE(PrivateBlock *pPrivate)
{
		pPrivate->DRIVE_RegValue.structReg.OCPTH = BIN_1;
		pPrivate->DRIVE_RegValue.structReg.OCPDEG = BIN_10;
		pPrivate->DRIVE_RegValue.structReg.TDRIVEN = BIN_1;
		pPrivate->DRIVE_RegValue.structReg.TDRIVEP = BIN_1;
		pPrivate->DRIVE_RegValue.structReg.IDRIVEN = BIN_10;
		pPrivate->DRIVE_RegValue.structReg.IDRIVEP = BIN_10;
	
		DEBUG_LOG("\r\nConfig DRIVE Reg:0x%x", pPrivate->DRIVE_RegValue.iRegValue)
	
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);		
	
		return true;
}

//复位DRV8711
void DRV8711_Reset(DRV8711_PinConfig *pConfig_t)
{
		SET_PIN(pConfig_t->ResetPin.GPIO_Port, pConfig_t->ResetPin.GPIO_Pin);
    Delay_ms (5);
    RESET_PIN(pConfig_t->ResetPin.GPIO_Port, pConfig_t->ResetPin.GPIO_Pin);
    Delay_ms (5);
}
//使能睡眠模式
void DRV8711_SleepEnable(DRV8711_PinConfig *pConfig_t)
{
    Delay_ms (5);
		RESET_PIN(pConfig_t->SleepPin.GPIO_Port, pConfig_t->SleepPin.GPIO_Pin);
    Delay_ms (5);
}

//解除睡眠模式
void DRV8711_SleepDisable (DRV8711_PinConfig *pConfig_t)
{
    Delay_ms (5);
    SET_PIN(pConfig_t->SleepPin.GPIO_Port, pConfig_t->SleepPin.GPIO_Pin);
    Delay_ms (5);
}

bool DRV8711_SetSubdivision(PrivateBlock *pPrivate, uint8_t iCfg)
{
    uint8_t iRegCfg = 0;

    switch (iCfg)
    {
				case 1:
						
						break;
        case 2:
						iRegCfg = 0x01;//2细分
            break;
        case 4:
						iRegCfg = 0x02;//4细分
            break;
        case 8:
						iRegCfg = 0x03;//8细分
            break;
        case 16:
						iRegCfg = 0x04;//16细分
            break;
        case 32:
						iRegCfg = 0x05;//32细分
            break;
        case 64:
						iRegCfg = 0x06;//64细分
            break;
        case 128:
						iRegCfg = 0x07;//128细分
            break;
        case 255:
						iRegCfg = 0x08;//256细分
            break;
        default:
						iRegCfg = 0x05;
						break;
    }
		
		pPrivate->CTRL_RegValue.structReg.MODE = iRegCfg;
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);
		
    return true;		
}

bool DRV8711_SetTorque(PrivateBlock *pPrivate, uint8_t iCfg)
{	
		pPrivate->TORQUE_RegValue.structReg.TORQUE = iCfg;
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);
		
    return true;		
}

bool DRV8711_SetSenseGain(PrivateBlock *pPrivate, uint8_t iCfg)
{	
	  uint8_t iRegCfg = 0;
	
    switch (iCfg)
    {
        case 5:
						iRegCfg = 0x0; // 5倍增益
            break;
        case 10:
						iRegCfg = 0x1; // 10倍增益
            break;
        case 20:
						iRegCfg = 0x2; // 20倍增益
            break;
        case 40:
						iRegCfg = 0x3; // 40倍增益
            break;
        default:
						iRegCfg = 0x0;
    }
		pPrivate->CTRL_RegValue.structReg.ISGAIN = iRegCfg;
		WriteSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);
		
    return true;		
}

bool DRV8711_Forward(PRIVATE_MEMBER_TYPE *pThisPrivate)
{	
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:malloc 8711 block failed", __FUNCTION__);
				return false;				
		}
		
		SET_PIN(pPrivate->PinConfig_t.DirPin.GPIO_Port, pPrivate->PinConfig_t.DirPin.GPIO_Pin);
}

bool DRV8711_Backward(PRIVATE_MEMBER_TYPE *pThisPrivate)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:malloc 8711 block failed", __FUNCTION__);
				return false;				
		}
		
		SET_PIN(pPrivate->PinConfig_t.DirPin.GPIO_Port, pPrivate->PinConfig_t.DirPin.GPIO_Pin);
}

bool DRV8711_Init(DRV8711_Control *Block_t, DRV8711_Params *Params_t)
{		
		PrivateBlock *pPrivate = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:malloc 8711 block failed", __FUNCTION__);
				return false;				
		}
		
		SPI_TypeDef *SPI_t;
		
		switch(Params_t->eMode)
		{
				case eSPI1:
					DEBUG_LOG("\r\nDBG 8711 choose SPI1")
					SPI_t = SPI1;
					break;
				case eSPI2:
					DEBUG_LOG("\r\nDBG 8711 choose SPI2")
					SPI_t = SPI2;
					break;
				default:
					
					break;
		}
		MX_SPI_Init(&pPrivate->hSPI, SPI_t);
		
		memcpy(&pPrivate->PinConfig_t, (DRV8711_PinConfig *)Params_t->pDriverPinConfig, sizeof(DRV8711_PinConfig));
		
		GPIO_InitPullDown(pPrivate->PinConfig_t.ResetPin.GPIO_Port, pPrivate->PinConfig_t.ResetPin.GPIO_Pin);
		GPIO_InitPullDown(pPrivate->PinConfig_t.SleepPin.GPIO_Port, pPrivate->PinConfig_t.SleepPin.GPIO_Pin);
		GPIO_InitNoPull(pPrivate->PinConfig_t.DirPin.GPIO_Port, pPrivate->PinConfig_t.DirPin.GPIO_Pin);
		GPIO_InitNoPull(pPrivate->PinConfig_t.CSPin.GPIO_Port, pPrivate->PinConfig_t.CSPin.GPIO_Pin);
			
		DRV8711_Reset(&pPrivate->PinConfig_t);
		DRV8711_SleepDisable(&pPrivate->PinConfig_t);
			
		SetRegisterDefaultCTRL(pPrivate);
		Delay_ms(1);
		SetRegisterDefaultTORQUE(pPrivate);
		Delay_ms(1);
		SetRegisterDefaultOFF(pPrivate);
		Delay_ms(1);
		SetRegisterDefaultBLANK(pPrivate);
		Delay_ms(1);
		SetRegisterDefaultDECAY(pPrivate);
		Delay_ms(1);
		SetRegisterDefaultSTALL(pPrivate);
		Delay_ms(1);
		SetRegisterDefaultDRIVE(pPrivate);
		
		DRV8711_SetSubdivision(pPrivate, Params_t->iSubdivisionCfg);
		DEBUG_LOG("\r\nDBG 8711 SubdivisionCfg val:%x", Params_t->iSubdivisionCfg);
		DRV8711_SetSenseGain(pPrivate, 5);
		DRV8711_SetTorque(pPrivate, Params_t->iCurrentCfg);
		DEBUG_LOG("\r\nDBG 8711 CurrentCfg val:%x", Params_t->iCurrentCfg);
		
		EnableMotor(pPrivate);
		
		Block_t->m_pThisPrivate = pPrivate;
		
//		uint16_t iData;
//		ReadSPI(&pPrivate->hSPI, &pPrivate->PinConfig_t, DRIVE_REG_ADDR, &iData);
//		DEBUG_LOG("\r\nDBG Read DRIVE:%x", iData)
//		
		return true;
}
	