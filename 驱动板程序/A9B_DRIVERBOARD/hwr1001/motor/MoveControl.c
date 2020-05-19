#include "MoveControl.h"
#include "stdlib.h"

#define POSITIVE_DIRECTION	1		//������
#define NEGTIVE_DIRECTION		-1	//������

typedef struct
{
		//���æ��־
		bool bBusy;
		//����λ�ñ�־
		bool bDistanceArrived;
	
		bool bStop;
		//�Ƿ��Ǹ�λ
		bool bHome[AXIS_NUM];
		//
		MotorType arrMotorType[AXIS_NUM];
		//ÿ������˶�����(mm)
		uint32_t fMoveDistance[AXIS_NUM];
		//ÿ����ĵ���˶�����(steps)
		uint32_t iMotorSteps[AXIS_NUM];
		//ÿ����ĵ���˶��ٶ�(steps/s)
		uint32_t iMotorStepsPerSecond[AXIS_NUM];
		uint32_t iEncoderSteps[AXIS_NUM];
		uint32_t iEncoderStepsPerSecond[AXIS_NUM];
		float fAcceleration;
		float fDeceleration;
		float fStartSpeed;
		float fEndSpeed;
}MoveNodeParams;

typedef struct 
{
		MoveNodeParams arrNodeBuffer[MOVE_NODE_NUM];
		uint8_t iWriteIndex;
		uint8_t iReadIndex;
		uint8_t iBufferLen;
}MoveNode_t;

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
		MoveNode_t *tNode = MallocMoveNode_t();
		Block_t->m_pThisPrivate = tNode;
}

static float CalcCurveForBlock(void)
{
	
}

void PushData(MoveNode_t *Node_t, MoveNodeParams *Params_t)
{
		
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

void StopAxisImmediately()
{
	
}

void ExecuteBlock(MoveNode_t *pNode)
{
		
}
