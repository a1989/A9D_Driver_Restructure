#include "MoveControl.h"
#include "StepperControl.h"

#define POSITIVE_DIRECTION	1		//正方向
#define NEGTIVE_DIRECTION		-1	//负方向

/*本模块类型定义*/
typedef struct
{
		StepperControl Stepper_t;
}MotorControl;

//每个运动单节点的参数
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
	
		//电机控制类型, 控制单轴/多轴
		MotorControlMode eControlType;
		//单轴控制时轴的索引
		AxisEnum eAxisIndex;
	
		MotorType arrMotorType[AXIS_NUM];
	
}MoveNodeParams;

//定义操作MoveControl模块的结构体类型
typedef struct 
{
		//标识符
		uint8_t iID;
		//单节点信息缓存区
		MoveNodeParams arrNodeBuffer[MOVE_NODE_NUM];
		//写缓存区索引
		uint8_t iWriteIndex;
		//读缓存区索引
		uint8_t iReadIndex;
		//缓存区长度
		uint8_t iBufferLen;
	
		MotorControl MotorControl_t;
}MoveNode_t;
/*本模块类型定义End*/


//开辟一片内存区域, 初始化变量, 返回指向这片内存区域的MoveNode_t类型指针
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
		//结构体指针指向初始化的内存区域
		MoveNode_t *Node_t = MallocMoveNode_t();
		StepperControlInit(&Node_t->MotorControl_t.Stepper_t);
	
		//赋值给上层MoveBlock的私有结构指针
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
