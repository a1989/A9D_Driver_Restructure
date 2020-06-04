#include "DRV8711_Operation.h"
#include <stdlib.h>

#define CTRL_REG_ADDR		0x00

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
  StructRegTORQUE structReg;
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
}PrivateBlock;

static bool WriteSPI(SPI_HandleTypeDef *hSPI, uint8_t iAddr, uint16_t iData)
{
		uint8_t iSendBuffer[3];
		uint8_t iRecvBuffer[3];
	
		iSendBuffer[0] = iAddr;
		iSendBuffer[1] = (iData >> 8) & 0xFF;
		iSendBuffer[2] = iData & 0xFF;
	
		if(HAL_OK != HAL_SPI_TransmitReceive (hSPI, iSendBuffer, iRecvBuffer, 3, 0xFFFFFF))
		{
				return false;
			
		}
		
		return true;
}

bool SetRegisterCTRL(PrivateBlock *pPrivate)
{
		pPrivate->CTRL_RegValue.structReg.ENBL = 0b0;
		pPrivate->CTRL_RegValue.structReg.RDIR = 0b1;
		pPrivate->CTRL_RegValue.structReg.RSTEP = 0b0;
		pPrivate->CTRL_RegValue.structReg.MODE = 0b0100;
		pPrivate->CTRL_RegValue.structReg.EXSTALL = 0b0;
		pPrivate->CTRL_RegValue.structReg.ISGAIN = 0b00;
		pPrivate->CTRL_RegValue.structReg.DTIME = 0b11;
		
		WriteSPI(&pPrivate->hSPI, CTRL_REG_ADDR, pPrivate->CTRL_RegValue.iRegValue);
}

bool SetRegisterTORQUE(PrivateBlock *pPrivate)
{
		
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
					SPI_t = SPI2;
					break;
				default:
					
					break;
		}
		
		MX_SPI_Init(pPrivate->hSPI, SPI_t);
		
		
		Block_t->m_pThisPrivate = pPrivate;
		
		return true;
}

bool DRV8711_SetSubdivision(PrivateBlock *pPrivate, uint8_t iCfg)
{

    uint16_t tmp0,tmp1;

    tmp0 = drv8711_ctrl_value;
    switch (iCfg)
    {
				case 1:
						
						break;
        case 2:
				tmp1 = 0x01;//2细分
            break;
        case 4:
				tmp1 = 0x02;//4细分
            break;
        case 8:
				tmp1 = 0x03;//8细分
            break;
        case 16:
				tmp1 = 0x04;//16细分
            break;
        case 32:
				tmp1 = 0x05;//32细分
            break;
        case 64:
				tmp1 = 0x06;//64细分
            break;
        case 128:
				tmp1 = 0x07;//128细分
            break;
        case 255:
				tmp1 = 0x08;//256细分
            break;
        default:
				tmp1= 0x05;
    }
    tmp0 = tmp0 & 0xff87;
    tmp0 = tmp0 | (tmp1 << 3);
    drv8711_ctrl_value = tmp0;
	//printf ("\r\n %x", drv8711_ctrl_value);
    SPI_DRV8711_Write (CTRL_Register_ADDR, drv8711_ctrl_value);
    return tmp0;		
}
	