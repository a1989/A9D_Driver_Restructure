#include "MotionControl.h"
#include "stm32f1xx_hal.h"

//�ֲ�ʽ����ʱ, ���ݰ�IDȷ����ǰ���Ƶ����ĸ���
static void SetAxisIndex(const uint8_t iBoardID, AxisEnum *iAxisIndex)
{
		if(0xA1 == iBoardID || 0xB1 == iBoardID)
		{
				*iAxisIndex = X_AXIS;
		}
		else if(0xA2 == iBoardID || 0xB2 == iBoardID)
		{
				*iAxisIndex = Y_AXIS;
		}
		else if(0xA3 == iBoardID || 0xB3 == iBoardID)
		{
				*iAxisIndex = Z_AXIS;
		}
		else
		{
				*iAxisIndex = UNKNOWN_AXIS;
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

//�˶����ƿ��ʼ��
bool MotionBlockInit(MotionManageBlock *structBlock, MotionBlockMsg *iMsg)
{
		*iMsg = 0;
	
		if(NULL == structBlock)
		{
				*iMsg |= 0x1;
				return false;
		}		
		
		ResetMotionBlock(structBlock);
		
		if(NULL == structBlock->m_pSetAxisIndex)
		{
				*iMsg |= 0x2;
				return false;
		}
		
		structBlock->m_pSetAxisIndex(, structBlock->iAxisIndex);
		
		//�����δ֪��,�򷵻ش���
		if(UNKNOWN_AXIS == structBlock->iAxisIndex)
		{
				*iMsg |= 0x4;
				return false;
		}
		
		return true;
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

void MotionControlInit(void)
{
		
}

void PushMoveData(MoveBlock *Block_t, float fTargetPos, float fSpeed)	
{
		
}
