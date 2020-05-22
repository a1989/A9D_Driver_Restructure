#include "MoveControl.h"
#include "StepperControl.h"

#define POSITIVE_DIRECTION	1		//������
#define NEGTIVE_DIRECTION		-1	//������

/*��ģ�����Ͷ���*/
typedef struct
{
		StepperControl Stepper_t;
}MotorControl;

//ÿ���˶����ڵ�Ĳ���
typedef struct
{
		//���˶��ڵ�æ��־
		bool bBusy;
		//����λ�ñ�־
		bool bDistanceArrived;
		//ֹͣ��־
		bool bStop;
		//�Ƿ��Ǹ�λ
		bool bHome[AXIS_NUM];
	
		//�����������, ���Ƶ���/����
		MotorControlMode eControlType;
		//�������ʱ�������
		AxisEnum eAxisIndex;
	
		MotorType arrMotorType[AXIS_NUM];
	
}MoveNodeParams;

//�������MoveControlģ��Ľṹ������
typedef struct 
{
		//��ʶ��
		uint8_t iID;
		//���ڵ���Ϣ������
		MoveNodeParams arrNodeBuffer[MOVE_NODE_NUM];
		//д����������
		uint8_t iWriteIndex;
		//������������
		uint8_t iReadIndex;
		//����������
		uint8_t iBufferLen;
	
		MotorControl MotorControl_t;
}MoveNode_t;
/*��ģ�����Ͷ���End*/


//����һƬ�ڴ�����, ��ʼ������, ����ָ����Ƭ�ڴ������MoveNode_t����ָ��
MoveNode_t *MallocMoveNode_t(void)
{
		MoveNode_t *pNode_t = (MoveNode_t*)malloc(sizeof(MoveNode_t));
		pNode_t->iBufferLen = 0;
		pNode_t->iReadIndex = 0;
		pNode_t->iWriteIndex = 0;
	
		return pNode_t;
}

void MoveBlockInit(MoveBlock *Block_t)
{
		//�ṹ��ָ��ָ���ʼ�����ڴ�����
		MoveNode_t *Node_t = MallocMoveNode_t();
		StepperControlInit(&Node_t->MotorControl_t.Stepper_t);
	
		//��ֵ���ϲ�MoveBlock��˽�нṹָ��
		Block_t->m_pThisPrivate = Node_t;
}

void PushData(MoveNode_t *Node_t, MoveNodeParams *Params_t)
{
		memcpy(&Node_t->arrNodeBuffer[Node_t->iWriteIndex], Params_t, sizeof(MoveNodeParams));
		Node_t->iWriteIndex = (Node_t->iWriteIndex + 1) % MOVE_NODE_NUM;
		Node_t->iBufferLen++;
}

void ClearNodeAll(MoveNode_t *Node_t)
{
	
}

void HomeAxis(MoveNode_t *pNode, MoveParams *pParams_t)
{
		MoveNodeParams Params_t;
		memcpy(Params_t.bHome, pParams_t->arrHomeFlag, sizeof(Params_t.bHome));
		memcpy(Params_t.arrMotorType, pParams_t->arrMotorType, sizeof(Params_t.arrMotorType));
		PushData(pNode, &Params_t);
}

void HomeAxisImmediately(MoveNode_t *pNode, MoveParams *pParams_t)
{
		ClearNodeAll(pNode);
		HomeAxis(pNode, pParams_t);
}

void PushMoveData(MoveNode_t *pNode, MoveParams *pParams_t)
{
		MoveNodeParams Params_t;
		memcpy(Params_t.bHome, pParams_t->arrHomeFlag, sizeof(Params_t.bHome));
		memcpy(Params_t.arrMotorType, pParams_t->arrMotorType, sizeof(Params_t.arrMotorType));
		PushData(pNode, &Params_t);
}

void PauseAxisImmediately()
{
		
}

void StopAxisImmediately()
{
		
}

static void SigleMotorControl(MoveNode_t *pNode)
{		
		float fDistance;
		float fSpeed;
		MoveNodeParams Params_t = pNode->arrNodeBuffer[pNode->iReadIndex];
		switch(Params_t.arrMotorType[Params_t.eAxisIndex])
		{
				case STEPPER:
					break;
				case STEPPER_ENCODER:
					pNode->MotorControl_t.Stepper_t.m_pSingleEncoderStepperPrepare();
					break;
				case BRUSHLESS:
					break;
				default:
					break;
		}
}

void ExecuteBlock(MoveNode_t *pNode)
{	
		if(pNode->iBufferLen)
		{
				switch(pNode->arrNodeBuffer[pNode->iReadIndex].eControlType)
				{
						case CONTROL_SINGLE_AXIS:							
							SigleMotorControl(&pNode->arrNodeBuffer[pNode->iReadIndex]);
							break;
						default:
							break;
				}
				pNode->iReadIndex = (pNode->iReadIndex + 1) % MOVE_NODE_NUM;
				pNode->iBufferLen--;
		}
}

static float CalcCurveForBlock(void)
{
	
}
