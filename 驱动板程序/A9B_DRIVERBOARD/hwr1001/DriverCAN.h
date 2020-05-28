#ifndef __DRIVERCAN_H__
#define __DRIVERCAN_H__

#include "stm32f1xx_hal.h"
#include "DriverBoardConfig.h"
#include "defines.h"

typedef struct
{
		//指向本模块私有数据结构的指针
		PRIVATE_MEMBER_TYPE *m_pThisPrivate;
		CAN_HandleTypeDef *(*m_pCAN_GetHandler)(PRIVATE_MEMBER_TYPE *pThis);
		void (*m_pGetData)(PRIVATE_MEMBER_TYPE *pThis, uint8_t *pData, uint8_t *iLen);
		void (*m_pSendData)(PRIVATE_MEMBER_TYPE *pThis, uint8_t *pData, uint8_t iLen);
}CAN_Block;

void DriverCAN_Init(CAN_Block *Block_t, CAN_TypeDef *CAN_t, uint32_t iStdID);
	
#endif
