#include "MoveControl.h"

#define POSITIVE_DIRECTION	1		//正方向
#define NEGTIVE_DIRECTION		-1	//负方向

typedef struct
{
		//本运动节点忙标志
		bool bBusy;
		//到达位置标志
		bool bDistanceArrived;
		//停止标志
		bool bStop;
		//是否是复位
		bool bHome[AXIS_NUM];
		//电机控制类型
		MotorControlMode eControlType;
		//单轴控制时轴的索引
		AxisEnum eAxisIndex;
	
		MotorType arrMotorType[AXIS_NUM];
	
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

static void SigleMotorControl(MoveNodeParams *pNode)
{		
		switch(pNode->arrMotorType[pNode->eAxisIndex])
		{
				case STEPPER:
					break;
				case STEPPER_ENCODER:
					
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
