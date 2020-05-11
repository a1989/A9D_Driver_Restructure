#include "DriverCAN.h"
#include "can.h"

void CAN1_Init(void)
{
		
}

//ÅäÖÃCAN¹ýÂËÆ÷
void CAN_FilterConfig(void)
{
		
}

void CAN_Init(void)
{
		MX_CAN_Init();
		CAN_FilterConfig();
}

//ÅäÖÃCAN·¢ËÍÄÚÈÝ
void CAN_SendConfig(CAN_SendBlock *Block_t)
{
		Block_t->TxMessage.StdId = Block_t->iStdID;
		Block_t->TxMessage.IDE = CAN_ID_STD;
		Block_t->TxMessage.RTR = CAN_RTR_DATA;
		Block_t->TxMessage.DLC = Block_t->iDataLen;
		memcpy(Block_t->TxMessage.Data, Block_t->arrSendBuffer, Block_t->iDataLen);
}