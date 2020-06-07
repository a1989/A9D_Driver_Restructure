#include "MotorControl.h"
#include "StepperControl.h"
#include "IncEncoderControl.h"
#include "stdlib.h"

#define POSITIVE_DIRECTION	1		//正方向
#define NEGTIVE_DIRECTION		-1	//负方向

/*本模块类型定义*/
typedef struct structMotorList
{
		uint8_t iMotorID;
		void *pMotor_t;
		void *pEncoder_t;
		struct structMotorList *pNext_t;
}MortorList;

typedef struct
{
		MortorList *pMotorList;
		char *strType;
}PrivateBlock;

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

static bool AddMotor(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *pParams_t)
{
		StepperControl *pStepper_t = NULL;
		IncEncoderControl *pIncEncoder_t = NULL;
		StepperSysParams *pStepperSysParams_t = NULL;
		MortorList *pList = NULL;
		MortorList *pNode = NULL;
	
		PrivateBlock *pPrivate_t = NULL;
		
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)pThisPrivate;
				
		switch(pParams_t->eMotorType)
		{
				case eSTEPPER_ENCODER:
					//初始化步进电机
					DEBUG_LOG("\r\nDBG Start init a stepper")
					pStepper_t = (StepperControl *)malloc(sizeof(StepperControl));
					if(NULL == pStepper_t)
					{
							printf("\r\nfunc:%s:malloc stepper block failed", __FUNCTION__);
							return false;								
					}
					//取出配置参数
					pStepperSysParams_t = (StepperSysParams *)pParams_t->pMotorSysParams;
					StepperControlInit(pStepper_t, &pStepperSysParams_t->StepperParams_t);
					//初始化编码器
					pIncEncoder_t = (IncEncoderControl *)malloc(sizeof(IncEncoderControl));
					IncEncoderControlInit(pIncEncoder_t, &pStepperSysParams_t->EncoderParmas_t);
					break;
				default:
					break;
		}
		
		pList = (MortorList *)malloc(sizeof(MortorList));
		if(NULL == pList)
		{
				printf("\r\nfunc:%s:malloc list node failed", __FUNCTION__);
				return false;
		}
		
		pList->pMotor_t = pStepper_t;
		pList->pEncoder_t = pIncEncoder_t;
		pList->pNext_t = NULL;
		
		if(NULL == pPrivate_t->pMotorList)
		{
				pPrivate_t->pMotorList = pList;
		}
		else
		{
				pNode = pPrivate_t->pMotorList;	
				while(pNode->pNext_t != NULL)
				{
						pNode = pNode->pNext_t;
				}
				
				pNode->pNext_t = pList;
		}
}

bool MotorControlInit(MotorControl *Block_t)
{
		//结构体指针指向初始化的内存区域
		PrivateBlock *pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:malloc failed", __FUNCTION__);
				return false;					
		}
	
		if(NULL == Block_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}			
		
		Block_t->m_pThisPrivate = pPrivate_t;
		Block_t->m_pAddMotor = AddMotor;
		
		DEBUG_LOG("\r\nDBG Motor control init success")
		return true;
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
				case eSTEPPER:
					break;
				case eSTEPPER_ENCODER:
					//pNode->MotorControl_t.Stepper_t.m_pSingleEncoderStepperPrepare();
					break;
				case eBRUSHLESS:
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
//							SigleMotorControl(&pNode->arrNodeBuffer[pNode->iReadIndex]);
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

static void ExeMotorControl(MotorControl *Block_t)
{
		//如果是电机Home命令
		//朝Home方向走长距离
		//检查到接触了Home限位开关,在中断里检测
		//反方向走10mm
		//以程序内规定的速度二次回零
		//检查到二次接触限位开关,在中断里检测
		//停止,修改位置,Home完成
	
		//如果是最大位置命令
		//朝最大位置走长距离
		//检查到接触了最大位置限位开关,在中断里检测
		//停止,记录位置,完成
}