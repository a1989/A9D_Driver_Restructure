#ifndef __DRIVERCAN_H__
#define __DRIVERCAN_H__

#include "stm32f1xx_hal.h"
#include "DriverBoardConfig.h"

typedef struct
{
		CanTxMsgTypeDef TxMessage;
		uint8_t iStdID;
		uint8_t arrSendBuffer[8];
		uint8_t iDataLen;
}CAN_SendBlock;

void CAN_Init(void);
void CAN_SendConfig(CAN_SendBlock *Block_t);
	
#endif
