#ifndef __DRIVERCAN_H__
#define __DRIVERCAN_H__

#include "stm32f1xx_hal.h"
#include "DriverBoardConfig.h"
#include "defines.h"

typedef struct
{
		//ָ��ģ��˽�����ݽṹ��ָ��
		PRIVATE_MEMBER_TYPE *pThisPrivate;
}CAN_SendBlock;

void DriverCAN_Init(CAN_SendBlock *Block_t, CAN_TypeDef *CAN_t, uint32_t iStdID);
	
#endif
