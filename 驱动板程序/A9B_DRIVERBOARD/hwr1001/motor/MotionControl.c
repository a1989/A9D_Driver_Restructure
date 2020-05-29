#include "MotionControl.h"
#include "MotorControl.h"

#define MOTION_TYPE_NAME	"MotionTypedef"

//��ģ���˽�в���,���ڿ��Ƶ��,��չ������ڿ�������,���,��������˶�����
typedef struct
{
		MotorControl *pMotorControl_t;
		char *strType;
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

//�˶����ƿ��ʼ��
bool MotionBlockInit(MotionManageBlock *Block_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		
		pPrivate_t->strType = MOTION_TYPE_NAME;
		pPrivate_t->pMotorControl_t = (MotorControl *)malloc(sizeof(MotorControl));
		
		Block_t->m_pThisPrivate = pPrivate_t;
		
		return true;
}

//���һ�����
void AddMotor(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
	
		if(NULL == pThisPrivate || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		

		pPrivate_t = (PrivateBlock *)pThisPrivate;
		
		pPrivate_t->pMotorControl_t->m_pAddMotor(pPrivate_t->pMotorControl_t->m_pThisPrivate, Params_t);
}

static void HomeSingleAxisImmediately()
{
		
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
