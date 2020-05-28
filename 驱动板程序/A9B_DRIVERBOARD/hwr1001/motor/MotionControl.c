#include "MotionControl.h"
#include "MotorControl.h"

#define MOTION_TYPE_NAME	"MotionTypedef"

typedef struct
{
		MotorBlock *MotorBlock_t;
		MotorBlock *BlockNext;
}MotorList;

typedef struct
{
		MotionParams Params_t;
		void *pInterface;
		char *strType;
		MotorList MotorList_t;
}PrivateBlock;

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

void SetMotionData(float fDiatance, float fSpeed)
{
		
}

//�˶����ƿ��ʼ��
bool MotionBlockInit(MotionManageBlock *Block_t, MotionParams *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == Block_t || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		
		pPrivate_t->strType = MOTION_TYPE_NAME;
		
		Block_t->m_pSetMotionData = SetMotionData;
		Block_t->m_pHomeAxis = HomeAxis;
		Block_t->m_pThis = Block_t;
		
		return true;
}

//���һ�����
void AddMotor(PRIVATE_MEMBER_TYPE *m_pThisPrivate, MotorParams *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == m_pThisPrivate || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}				
}

static void HomeAxis(struct MotionBlock *pThis, MoveParams *Params_t)
{
		MoveBlock *MoveControl_t = NULL;
	
		MoveControl_t = pThis->m_pMoveControl;
		MoveControl_t->m_pHomeAxis(MoveControl_t->m_pThis, Params_t);
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

void PushMoveData(MoveBlock *Block_t, float fTargetPos, float fSpeed)	
{
		
}
