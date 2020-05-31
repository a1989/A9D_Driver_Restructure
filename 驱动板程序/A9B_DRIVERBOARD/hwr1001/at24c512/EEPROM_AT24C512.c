#include "EEPROM_AT24C512.h"

typedef struct
{
		I2C_HandleTypeDef hI2C;
		uint8_t iI2C_Address;
}PrivateBlock;

void AT24C512_Init(PRIVATE_MEMBER_TYPE *pThis, DriverConfigMode eMode)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		

		switch(eMode)
		{
			case eI2C1:
				break;
			case eI2C2:
				MX_I2C_Init(pPrivate_t->hI2C, I2C2);
				break;
			default:
				break;
		}
			
}

bool AT24C512_WriteByte(PRIVATE_MEMBER_TYPE *pThis, EEPROM_Params *Params_t)
{
		HAL_StatusTypeDef eStatus = HAL_OK;

		PrivateBlock *pPrivate_t = (PrivateBlock *)pThis;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
	
    eStatus = HAL_I2C_Mem_Write (&pPrivate_t->hI2C, 
																pPrivate_t->iI2C_Address, 
																Params_t->iAddress, 
																I2C_MEMADD_SIZE_16BIT, 
																&Params_t->iData, 1, 1000);
    /* Check the communication status */
    if(eStatus != HAL_OK)
    {
        /* Execute user timeout callback */
        //I2Cx_Error(Addr);
				printf ("\r\nfunc:%s,AT24C512 Status:%d", __FUNCTION__, eStatus);
				return false;
    }
		Params_t->eStatus = eStatus;
		
    while (HAL_I2C_GetState (&pPrivate_t->hI2C) != HAL_I2C_STATE_READY);
    /* Check if the EEPROM is ready for a new operation */
    while (HAL_I2C_IsDeviceReady (&pPrivate_t->hI2C, pPrivate_t->iI2C_Address, 300, 300) == HAL_TIMEOUT);
    /* Wait for the end of the transfer */
    while (HAL_I2C_GetState (&pPrivate_t->hI2C) != HAL_I2C_STATE_READY);
    
		return true;
}

bool AT24C512_ReadByte(PRIVATE_MEMBER_TYPE *pThis, EEPROM_Params *Params_t)
{
		HAL_StatusTypeDef eStatus = HAL_OK;
	
		PrivateBlock *pPrivate_t = (PrivateBlock *)pThis;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		eStatus = HAL_I2C_Mem_Read (&pPrivate_t->hI2C, 
																pPrivate_t->iI2C_Address, 
																Params_t->iAddress, 
																I2C_MEMADD_SIZE_16BIT, 
																&Params_t->iData, 1, 1000);
		if (eStatus != HAL_OK)
		{
				printf ("\r\nfunc:%s,AT24C512 Status:%d", __FUNCTION__, eStatus);
				return false;
		}

		return true;		
}