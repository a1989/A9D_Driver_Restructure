#include "MotionControl.h"
#include "stm32f1xx_hal.h"

static void SetAxisIndex(const uint8_t iBoardID, AxisEnum *iAxisInex)
{
		if(0xA1 == iBoardID || 0xB1 == iBoardID)
		{
				*iAxisInex = X_AXIS;
		}
		else if(0xA2 == iBoardID || 0xB2 == iBoardID)
		{
				*iAxisInex = Y_AXIS;
		}
		else if(0xA3 == iBoardID || 0xB3 == iBoardID)
		{
				*iAxisInex = Z_AXIS;
		}
		else
		{
				*iAxisInex = UNKNOWN_AXIS;
		}
}

//�����˶����ƿ�
static void ResetMotionBlock(MotionManageBlock *structBlock)
{
		int i = 0;
		//��ʼ������˶����ƿ�
		for(i = 0; i < QUEUELENGTH; i++)
		{
				MoveBlockInit(structBlock->arrMoveBlockQueue[i]);
		}
		
		structBlock->m_pSetAxisIndex = SetAxisIndex;
}

void MotionBlockInit(MotionManageBlock *structBlock, MotionBlockMsg *iMsg)
{
		*iMsg = 0;
	
		if(null == structBlock)
		{
				iMsg |= 0x1;
				return;
		}		
		
		ResetMotionBlock(structBlock);
		
		if(null == structBlock->m_pSetAxisIndex)
		{
				iMsg |= 0x2;
				return;
		}
		
		structBlock->m_pSetAxisIndex(, structBlock->iAxisIndex);
		
		//�����δ֪��,�򷵻ش���
		if(UNKNOWN_AXIS == structBlock->iAxisIndex)
		{
				iMsg |= 0x4;
				return;
		}
		
		
}

void ProcessBlockData()
{
	
}

static void GetCoordinate(void)
{
	
}

static void PrepareMove(void)
{
	
}



static void HomeAxis(AxisIndex iAxis)
{
		
}

void GetMoveData()
{
		uint8_t iMoveType;
		//0:����λ���˶�
		//1:���λ���˶�
		//2:��λ
		//3:�ߵ����λ��
		//4:ֹͣ
		switch(iMoveType)
		{
				case 0:
					GetCoordinate();
					PrepareMove();
					break;
				case 1:
					break;
				case 2:
					break;
		}
}

void CalculateMoveBlockParams()
{
	
}

static float GetCurrentSpeed(void)
{

}

static float GetCurrentLocation(void)
{

}


