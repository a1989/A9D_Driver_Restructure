#include "MotorControl.h"
#include "StepperControl.h"
#include "IncEncoderControl.h"
#include "gpio.h"
#include <stdlib.h>
#include <math.h>
#include "tim.h"

#define POSITIVE_DIRECTION	1		//正方向
#define NEGTIVE_DIRECTION		-1	//负方向

void *MotorVarToInt = NULL;

/*本模块类型定义*/
typedef struct
{
		float *fCurrentPos;
		float *fTargetPos;
}MoveInfo;

typedef struct structLimits
{
		LimitParams LimitParams_t;
		struct structLimits *pNext_t;
}LimitsNode;

typedef struct structMotorList
{
		uint8_t iMotorID;
		MotorType eMotorType;	
		void *pMotor_t;
		void *pEncoder_t;
		struct structMotorList *pNext_t;
		LimitsNode *pLimitsList;
		MoveInfo MoveInfo_t;
		int32_t iPulseLocation;
		float fCoordLocation;
		float fPlanTarget;
		float fCurrentMoveDist;
		bool bDirForward;
		bool bFindZeroLimit;
		//用于快速电机操作
		TIM_HandleTypeDef *hMotorTim;
//		uint16_t (*m_pSetMoveParams)(void *pMotor_t);
//		void (*m_pStopMove)(void *pMotor_t);
		bool (*m_pLimitTouch)(LimitsNode *LimitNode, LimitFunction eLimit);
		bool (*m_pTargetArrived)(void *pEncoder_t);		
		bool bHomed;
		bool bGoingHome;
		bool bTargetChange;
		float fLastTarget;
}MotorList;

//每个运动单节点的参数
typedef struct
{
		uint8_t *p_iID;
		//本运动节点忙标志
		bool bBusy;
	
		//到达位置标志
		bool bDistanceArrived;
	
		//停止标志
		bool bStop;
	
		//是否是Home
		bool *p_bHome;
		
		//Home方向
		int8_t *p_iHomeDir;
	
		//根据需要指向一个或一组数据
		float *p_fSpeed;
	
		float *p_fTargetPos;
}MoveNodeParams;

static MoveNodeParams g_arrNodeBuffer[MOVE_NODE_NUM] = {0};
//定义操作MoveControl模块的结构体类型
typedef struct 
{
		//单节点信息缓存区
//		MoveNodeParams arrNodeBuffer[MOVE_NODE_NUM];
		MoveNodeParams *arrNodeBuffer;
		//写缓存区索引
		volatile uint8_t iWriteIndex;
		//读缓存区索引
		volatile uint8_t iReadIndex;
		//缓存区长度
		volatile uint8_t iBufferLen;
}MoveNodeList;

typedef struct
{
		MotorList *pMotorList;
		MoveNodeList *pMoveNodeList;
		char *strType;
}PrivateBlock;

typedef enum
{
		ePOP_DATA = 0,
		eWAIT_DONE,
		eEXE_DONE
}eBlockExeSteps;

typedef enum 
{
		eSET_HOME_PARAMS0 = 0,
		eCHECK_LIMIT_FIRST,
		eMOVE_REVERSE_DIR,
		eWAIT_REVERSE_ARRIVED,
		eSET_HOME_PARAMS1,
		eCHECK_LIMIT_SECOND,
		eDONE
}eMotorHomeStep;

typedef enum
{
		eSET_PARAMS = 0,
		eWAIT_ARRIVE,
		eARRIVE,
		eWAIT_STOP,
}eNormalMoveStep;

typedef enum
{
		eMotorHome = 0,
		eMotorMove,
		eNoAction
}eExeType;

/*本模块类型定义End*/


MotorList *g_pMotorTabel;

//开辟一片内存区域, 初始化变量, 返回指向这片内存区域的MoveNode_t类型指针
MoveNodeList *MallocMoveNode_t(void)
{
		uint8_t i;
	
		MoveNodeList *pNode_t = (MoveNodeList*)malloc(sizeof(MoveNodeList));
		if(NULL == pNode_t)
		{
				printf("\r\nfunc:%s:NodeList malloc failed", __FUNCTION__);
				return NULL;				
		}
		
//		for(i = 0; i < MOVE_NODE_NUM; i++)
//		{
//				#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
//						pNode_t->arrNodeBuffer[i].p_iID = (uint8_t *)malloc(sizeof(uint8_t));
//						pNode_t->arrNodeBuffer[i].p_bHome = (bool *)malloc(sizeof(bool));
//						pNode_t->arrNodeBuffer[i].p_iHomeDir = (int8_t *)malloc(sizeof(int8_t));
//						pNode_t->arrNodeBuffer[i].p_fSpeed = (float *)malloc(sizeof(float));
//						pNode_t->arrNodeBuffer[i].p_fTargetPos = (float *)malloc(sizeof(float));
//						
//						if(NULL == pNode_t->arrNodeBuffer[i].p_iID || 
//								NULL == pNode_t->arrNodeBuffer[i].p_bHome || 
//								NULL == pNode_t->arrNodeBuffer[i].p_iHomeDir || 
//								NULL == pNode_t->arrNodeBuffer[i].p_fSpeed || 
//								NULL == pNode_t->arrNodeBuffer[i].p_fTargetPos)
//						{
//								printf("\r\nfunc:%s:var malloc failed", __FUNCTION__);
//								return NULL;									
//						}
//				#else
//						printf("\r\nfunc:%s:wrong hardware version", __FUNCTION__);
//						return NULL;
//				#endif								
//		}

		for(i = 0; i < MOVE_NODE_NUM; i++)
		{
				#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
						g_arrNodeBuffer[i].p_iID = (uint8_t *)malloc(sizeof(uint8_t));
						g_arrNodeBuffer[i].p_bHome = (bool *)malloc(sizeof(bool));
						g_arrNodeBuffer[i].p_iHomeDir = (int8_t *)malloc(sizeof(int8_t));
						g_arrNodeBuffer[i].p_fSpeed = (float *)malloc(sizeof(float));
						g_arrNodeBuffer[i].p_fTargetPos = (float *)malloc(sizeof(float));
						
						if(NULL == g_arrNodeBuffer[i].p_iID || 
								NULL == g_arrNodeBuffer[i].p_bHome || 
								NULL == g_arrNodeBuffer[i].p_iHomeDir || 
								NULL == g_arrNodeBuffer[i].p_fSpeed || 
								NULL == g_arrNodeBuffer[i].p_fTargetPos)
						{
								printf("\r\nfunc:%s:var malloc failed", __FUNCTION__);
								return NULL;									
						}
				#else
						printf("\r\nfunc:%s:wrong hardware version", __FUNCTION__);
						return NULL;
				#endif								
		}
		
		pNode_t->arrNodeBuffer = g_arrNodeBuffer;
		pNode_t->iBufferLen = 0;
		pNode_t->iReadIndex = 0;
		pNode_t->iWriteIndex = 0;
	
		return pNode_t;
}

bool RegisterMotorVar(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorControl *MotorVar)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pThisPrivate;
//		PrivateBlock *pMotorVar = (PrivateBlock *)PrivateBlock;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}			
		
		DEBUG_LOG("\r\nDBG start register motor to Int")
		
//		if(pPrivate_t->pMotorList == NULL)
//		{
//			DEBUG_LOG("\r\nDBG no motor")
//		}
		
//		MotorVar->m_pThisPrivate = pPrivate_t; //(PrivateBlock *)malloc(sizeof(PrivateBlock));
//		memcpy(MotorVar->m_pThisPrivate, pPrivate_t, sizeof(PrivateBlock));
		MotorVar->m_pThisPrivate = pPrivate_t;
		
		DEBUG_LOG("\r\nDBG end register motor to Int")
		
		return true;
}

bool LimitTouch(LimitsNode *LimitNode, LimitFunction eLimit)
{
		LimitsNode *pNode = LimitNode;
	
		if(NULL == pNode)
		{
				return false;
		}
		
		while(pNode != NULL)
		{
				if(pNode->LimitParams_t.eFunc == eLimit)
				{
						if((bool)READ_GPIO_PIN(pNode->LimitParams_t.GPIO_Port, pNode->LimitParams_t.GPIO_Pin))
						{
								return true;
						}						
				}

				pNode = pNode->pNext_t;
		}
		
		return false;
}

static bool AddMotor(PRIVATE_MEMBER_TYPE *pThisPrivate, MotorParams *pParams_t)
{
		StepperControl *pStepper_t = NULL;
		IncEncoderControl *pIncEncoder_t = NULL;
		StepperSysParams *pStepperSysParams_t = NULL;
		MotorList *pList = NULL;
		MotorList *pNode = NULL;
		TIM_HandleTypeDef *hTIM = NULL;
	
		PrivateBlock *pPrivate_t = NULL;
		
		if(NULL == pThisPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;
		}		
		
		pPrivate_t = (PrivateBlock *)pThisPrivate;
		
		pList = (MotorList *)malloc(sizeof(MotorList));
		if(NULL == pList)
		{
				printf("\r\nfunc:%s:malloc list node failed", __FUNCTION__);
				return false;
		}
		
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
					if(NULL == pIncEncoder_t)
					{
							printf("\r\nfunc:%s:malloc encoder node failed", __FUNCTION__);
							return false;
					}
					IncEncoderControlInit(pIncEncoder_t, &pStepperSysParams_t->EncoderParmas_t);
					
					pList->fCoordLocation = 0.0;
					pList->pMotor_t = pStepper_t;
					pList->pEncoder_t = pIncEncoder_t;
					pList->hMotorTim = pStepper_t->m_pGetStepperTimHandle(pStepper_t->m_pThisPrivate);
					pList->m_pTargetArrived = pIncEncoder_t->m_pIncEncoderTargetArrived;
					pList->m_pLimitTouch = LimitTouch;
					pList->bGoingHome = false;
					pList->fLastTarget = 0;
					break;
				default:
					break;
		}
		

		

		pList->eMotorType = pParams_t->eMotorType;

		pList->pLimitsList = NULL;
		pList->pNext_t = NULL;
		
		if(NULL == pPrivate_t->pMotorList)
		{
				DEBUG_LOG("\r\nAdd a motor")
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
				DEBUG_LOG("\r\nAdd a motor")
		}		
		
		return true;
}

void MotorIntHandler(PRIVATE_MEMBER_TYPE *pPrivate, TIM_HandleTypeDef *hTIM)
{
		PrivateBlock *pPrivate_t = (PrivateBlock *)pPrivate;
		MotorList *Motor_t = pPrivate_t->pMotorList;
		StepperControl *pStepper_t = NULL;
		IncEncoderControl *pIncEncoder_t = NULL;
		static uint32_t iRelativeValue = 0;
	
		if(NULL == pPrivate_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}			
		
		while(Motor_t != NULL)
		{
				if(Motor_t->hMotorTim == hTIM)
				{
						break;
				}
				
				Motor_t = Motor_t->pNext_t;
		}

		if(NULL == Motor_t)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return;
		}

//		iCount =__HAL_TIM_GET_COUNTER (hTIM);
//		__HAL_TIM_SET_COMPARE (&htim2, TIM_CHANNEL_1, (uint16_t)(iCount + 100));
		
		switch(Motor_t->eMotorType)
		{
				case eSTEPPER_ENCODER:
					pStepper_t = Motor_t->pMotor_t;
					pIncEncoder_t = Motor_t->pEncoder_t;
					if(!Motor_t->bDirForward)
					{
							if(Motor_t->m_pLimitTouch(Motor_t->pLimitsList, eZero))
							{
									DEBUG_LOG("\r\nDBG 0 Limit touch")
									
									pStepper_t->m_pStepperStop(pStepper_t->m_pThisPrivate);
									return;
							}
					}
					else
					{
							if(Motor_t->m_pLimitTouch(Motor_t->pLimitsList, ePositive))
							{
									pStepper_t->m_pStepperStop(pStepper_t->m_pThisPrivate);
									return;
							}							
					}
										
					if(!Motor_t->bFindZeroLimit && Motor_t->m_pTargetArrived(pIncEncoder_t->m_pThisPrivate))
					{
							if(Motor_t->bTargetChange)
							{
									DEBUG_LOG("\r\nDBG Int target arrived")
									//如果运动参数的buffer size为0则停止运动
									pStepper_t->m_pStepperStop(pStepper_t->m_pThisPrivate);
									Motor_t->bTargetChange = false;
							}
					}
					else
					{
							pIncEncoder_t->m_pGetEncoderRelativeValueAbs(pIncEncoder_t->m_pThisPrivate, &iRelativeValue);
							pStepper_t->m_pSetTIM_OC(pStepper_t->m_pThisPrivate, hTIM, iRelativeValue);
					}					
					break;
				default:
					break;
		}		
}

void PushMoveData(MoveNodeList *Node_t, MoveNodeParams *Params_t)
{
		if(Node_t->iBufferLen < MOVE_NODE_NUM)
		{
				DEBUG_LOG("\r\nDBG Push Move Data")

				if(NULL == Params_t)
				{
					DEBUG_LOG("\r\nDBG null")
				}
				Node_t->arrNodeBuffer[Node_t->iWriteIndex].bBusy = Params_t->bBusy;

				Node_t->arrNodeBuffer[Node_t->iWriteIndex].bDistanceArrived = Params_t->bDistanceArrived;		

				Node_t->arrNodeBuffer[Node_t->iWriteIndex].bStop = Params_t->bStop;

				#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
						memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_bHome, Params_t->p_bHome, sizeof(bool));

						memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_iHomeDir, Params_t->p_iHomeDir, sizeof(uint8_t));

						memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_iID, Params_t->p_iID, sizeof(uint8_t));

						memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_fSpeed, Params_t->p_fSpeed, sizeof(float));		

						memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_fTargetPos, Params_t->p_fTargetPos, sizeof(float));
				#endif
			
				Node_t->iWriteIndex = (Node_t->iWriteIndex + 1) % MOVE_NODE_NUM;
				Node_t->iBufferLen++;
				
				DEBUG_LOG("\r\nDBG Push Move Data End")
		}
		else
		{
				DEBUG_LOG("\r\nDBG Move buffer full")
		}
}

bool PopMoveData(MoveNodeList *Node_t, MoveNodeParams *Params_t)
{
		if(NULL == Node_t)
		{
				return false;
		}
		
		if(Node_t->iBufferLen)
		{				
				// DEBUG_LOG("\r\nDBG %d", Node_t->iBufferLen)
//				DEBUG_LOG("\r\nDBG Pop Move Data")
				memcpy(Params_t, &Node_t->arrNodeBuffer[Node_t->iReadIndex], sizeof(MoveNodeParams));
			
//				Node_t->iReadIndex = (Node_t->iReadIndex + 1) % MOVE_NODE_NUM;
//				Node_t->iBufferLen--;
				
				return true;
		}
		
		return false;
}

static bool FreshNodeBufferReadIndex(MoveNodeList *Node_t)
{
		if(NULL == Node_t)
		{
				return false;
		}
		
		if(Node_t->iBufferLen)
		{							
				Node_t->iReadIndex = (Node_t->iReadIndex + 1) % MOVE_NODE_NUM;
				Node_t->iBufferLen--;
				
				return true;
		}
		
		return false;		
}

void ClearMoveNodeAll(MoveNodeList *pList)
{
		if(NULL == pList)
		{
				printf("\r\nfunc:%s:list null pointer", __FUNCTION__);
				return;
		}		
		
		pList->iReadIndex = pList->iWriteIndex;
		pList->iBufferLen = 0;
		DEBUG_LOG("\r\nDBG Clear move node")
}

void StopAxisImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID)
{
		
}

//平缓的停止轴
void StopAxisModerate(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;					
		}				
}

//轴回到初始位置
void MotorHome(MoveNodeList *pNode, uint8_t *iMotorID, uint32_t *iSpeed)
{		
		MoveNodeParams Params_t;
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
					bool bHome = true;
					bool bStop = false;
					int8_t iHomeDir = NEGTIVE_DIRECTION;
					float fSpeed = (float)(*iSpeed) / 10;
					float fTarget = -100000;
		#endif
		Params_t.bBusy = false;
		Params_t.bDistanceArrived = false;
		Params_t.p_bHome = &bHome;
		Params_t.p_iHomeDir = &iHomeDir;
		Params_t.bStop = false;
		Params_t.p_iID = iMotorID;
		Params_t.p_fSpeed = &fSpeed;
		Params_t.p_fTargetPos = &fTarget;
	
		PushMoveData(pNode, &Params_t);
}

bool SetMoveParams(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, float *fMoveDist, float *fMoveSpeed)
{
		MoveNodeParams Params_t;
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
					bool bHome = false;
					bool bStop = false;
					int8_t iHomeDir = NEGTIVE_DIRECTION;
//					float fSpeed = (float)(*fMoveSpeed);
//					float fDist = (float)(*fMoveDist);
		#endif
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}		
		
		Params_t.bBusy = false;
		Params_t.bDistanceArrived = false;
		Params_t.p_bHome = &bHome;
		Params_t.p_fTargetPos = fMoveDist;
		Params_t.p_fSpeed = fMoveSpeed;
		Params_t.p_iID = iMotorID;
		Params_t.bStop = false;
		Params_t.p_iHomeDir = &iHomeDir;
		
		PushMoveData(pPrivate->pMoveNodeList, &Params_t);
		
		return true;
}

static bool MotorHomeImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, uint32_t iSpeed)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}
		
		StopAxisModerate(pPrivate, &iMotorID);
//		ClearMoveNodeAll(pPrivate->pMoveNodeList);

		MotorHome(pPrivate->pMoveNodeList, &iMotorID, &iSpeed);
		
		return true;
}

//uint16_t GetMotorPulseParamByID(uint8_t iMotorID)
//{
//		
//}

void PauseAxisImmediately()
{
		
}

MotorList *FindMotor(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		MotorList *pNode = NULL; 
		
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return NULL;					
		}	
		
		pNode = pPrivate->pMotorList;
		if(NULL == pNode)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return NULL;
		}
		
		do
		{
				if(pNode->iMotorID == iMotorID)
				{
						return pNode;
				}
				
				pNode = pNode->pNext_t;
		}while(pNode != NULL);
		
		return NULL;
}

LimitsNode *FindLimit(MotorList *pMotorNode, LimitFunction eFunc)
{		
		LimitsNode *pLimitsNode = NULL;
	
		if(NULL == pMotorNode)
		{
				printf("\r\nfunc:%s:motor null pointer", __FUNCTION__);
				return NULL;					
		}	
		
		pLimitsNode = pMotorNode->pLimitsList;
		if(NULL == pLimitsNode)
		{
				printf("\r\nfunc:%s:no limit", __FUNCTION__);
				return NULL;
		}
		
		do
		{
				if(pLimitsNode->LimitParams_t.eFunc == eFunc)
				{
						return pLimitsNode;
				}
				
				pLimitsNode = pLimitsNode->pNext_t;
		}while(pLimitsNode != NULL);
		
		return NULL;
}

void MovePosition(MotorList *pMotorNode, float fPos)
{
		
}

void SetPositionEnforce(MotorList *pMotorNode, float fPos)
{
		
}

bool CheckLimits(MotorList *pMotorNode, eLimitType eLimit)
{
		return true;
}

bool AddLimits(PRIVATE_MEMBER_TYPE *pThisPrivate, LimitParams *Params_t)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		MotorList *pNode = NULL; 
		LimitsNode *pLimitNode = NULL;
		LimitsNode *pLimitNodeTry = NULL;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}			
		
		DEBUG_LOG("\r\nDBG Start add limit")
		
		pNode = pPrivate->pMotorList;
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return false;			
		}
		
		//查找到对应的电机节点
		pNode = FindMotor(pPrivate, Params_t->iMotorBelong);
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor match", __FUNCTION__);
				return false;					
		}
		
		pLimitNode = (LimitsNode *)malloc(sizeof(LimitsNode));
		if(NULL == pLimitNode)
		{
				printf("\r\nfunc:%s:malloc list node failed", __FUNCTION__);
				return false;
		}
		
		pLimitNode->LimitParams_t.eFunc = Params_t->eFunc;
		pLimitNode->LimitParams_t.GPIO_Pin = Params_t->GPIO_Pin;
		pLimitNode->LimitParams_t.GPIO_Port = Params_t->GPIO_Port;
		pLimitNode->LimitParams_t.iMotorBelong = Params_t->iMotorBelong;
		pLimitNode->pNext_t = NULL;
		
		if(NULL == pPrivate->pMotorList->pLimitsList)
		{
				pPrivate->pMotorList->pLimitsList = pLimitNode;
		}
		else
		{
				pLimitNodeTry = pPrivate->pMotorList->pLimitsList;	
				while(pLimitNodeTry->pNext_t != NULL)
				{
						pLimitNodeTry = pPrivate->pMotorList->pLimitsList->pNext_t;
				}
				
				pLimitNodeTry->pNext_t = pLimitNode;
		}
		
		return true;
}

bool ReadLimitByID(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, LimitFunction eFunc, bool *bStatu)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		MotorList *pNode = NULL; 
		LimitsNode *pLimitNode = NULL;
		LimitsNode *pLimitNodeTry = NULL;

	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}					
		
		pNode = pPrivate->pMotorList;
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return false;			
		}
		
		//查找到对应的电机节点
		pNode = FindMotor(pPrivate, iMotorID);
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor match", __FUNCTION__);
				return false;					
		}		
		
		pLimitNode = FindLimit(pNode, eFunc);
		if(pLimitNode == NULL)
		{
				return false;
		}
		
//		DEBUG_LOG("\r\nDBG Start read limit")
		
		*bStatu = (bool)READ_GPIO_PIN(pLimitNode->LimitParams_t.GPIO_Port, pLimitNode->LimitParams_t.GPIO_Pin);
//		DEBUG_LOG("\r\nDBG read limit:%d", READ_GPIO_PIN(pLimitNode->LimitParams_t.GPIO_Port, pLimitNode->LimitParams_t.GPIO_Pin))
		return true;
}

bool GetLinearLocation(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, float *fPos)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		MotorList *pNode = NULL; 
		IncEncoderControl *pEncoder;
		float fEncoderPos;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}					
		
		pNode = pPrivate->pMotorList;
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return false;			
		}
		
		//查找到对应的电机节点
		pNode = FindMotor(pPrivate, *(uint8_t *)iMotorID);
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor match", __FUNCTION__);
				return false;					
		}		
	
		DEBUG_LOG("\r\nDBG start get linear move data")
		
		pEncoder = pNode->pEncoder_t;
		
		pEncoder->m_pGetEncoderLinearValue(pEncoder->m_pThisPrivate, fPos);
		DEBUG_LOG("\r\nDBG encoder pos mm:%f", *fPos)
		
		return true;		
}

bool GetLinearSpeed(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, float *fSpeed)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		MotorList *pNode = NULL; 
		IncEncoderControl *pEncoder;
		float fEncoderPos;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}					
		
		pNode = pPrivate->pMotorList;
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return false;			
		}
		
		//查找到对应的电机节点
		pNode = FindMotor(pPrivate, *(uint8_t *)iMotorID);
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor match", __FUNCTION__);
				return false;					
		}		
	
		DEBUG_LOG("\r\nDBG start get linear speed data")
		
		pEncoder = pNode->pEncoder_t;
		
		pEncoder->m_pGetEncoderLinearSpeed(pEncoder->m_pThisPrivate, fSpeed);
		DEBUG_LOG("\r\nDBG encoder speed:%f", *fSpeed)
		
		return true;			
}

bool SetDirPinHighAsForward(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, bool bValue)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
		MotorList *pNode = NULL; 
		StepperControl *pStepper_t = NULL;
		IncEncoderControl *pIncEncoder_t = NULL;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}					
		
		pNode = pPrivate->pMotorList;
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor", __FUNCTION__);
				return false;			
		}
		
		//查找到对应的电机节点
		pNode = FindMotor(pPrivate, iMotorID);
		if(pNode == NULL)
		{
				printf("\r\nfunc:%s:no motor match", __FUNCTION__);
				return false;					
		}		
		
		switch(pNode->eMotorType)
		{
				case eSTEPPER_ENCODER:
					pStepper_t = pNode->pMotor_t;
					pIncEncoder_t = pNode->pEncoder_t;
					pStepper_t->m_pSetStepperDirHighAsForward(pStepper_t->m_pThisPrivate, bValue);
					pIncEncoder_t->m_pReverseCountDir(pIncEncoder_t->m_pThisPrivate, !bValue);
				default:
					break;
		}		
		
		return true;			
}

//bool GetMotorMoveData(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, float *fPos, float *fSpeed)
//{
//		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
//		MotorList *pNode = NULL; 
//		IncEncoderControl *pEncoder;
//		int32_t iPos;
//	
//		if(NULL == pPrivate)
//		{
//				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
//				return false;					
//		}					
//		
//		pNode = pPrivate->pMotorList;
//		if(pNode == NULL)
//		{
//				printf("\r\nfunc:%s:no motor", __FUNCTION__);
//				return false;			
//		}
//		
//		//查找到对应的电机节点
//		pNode = FindMotor(pPrivate, iMotorID);
//		if(pNode == NULL)
//		{
//				printf("\r\nfunc:%s:no motor match", __FUNCTION__);
//				return false;					
//		}		
//	
//		DEBUG_LOG("\r\nDBG start get linear move data")
//		
//		pEncoder = pNode->pEncoder_t;
//		
//		pEncoder->m_pGetEncoderLinearValue(pEncoder->m_pThisPrivate, &iPos);
//		DEBUG_LOG("\r\nDBG encoder pos:%d", iPos)
//		
//		return true;
//}

static void ExeMotorHome(PRIVATE_MEMBER_TYPE *pPrivate, MotorList *pMotorNode, MoveNodeParams Params_t, CmdDataObj *eCmdType)
{
		static eMotorHomeStep eHomeStep;		
		float fTarget;
		static IncEncoderControl *IncEncoder_t = NULL;
		static StepperControl *Stepper_t = NULL;
		bool bLimitStatu = false;
	
		switch(eHomeStep)
		{
				//如果是电机Home命令
				case eSET_HOME_PARAMS0:
					DEBUG_LOG("\r\nStart home")												
					switch(pMotorNode->eMotorType)
					{
							case eSTEPPER_ENCODER:
								//朝Home方向走长距离
								pMotorNode->bDirForward = false;
								pMotorNode->bFindZeroLimit = true;
								Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
								IncEncoder_t = (IncEncoderControl *)pMotorNode->pEncoder_t;
							
								fTarget = (float)(MAX_LENGTH * NEGTIVE_DIRECTION) - pMotorNode->fCoordLocation;
//								DEBUG_LOG("\r\nStart home %d", (MAX_LENGTH * NEGTIVE_DIRECTION))	
//								DEBUG_LOG("\r\nStart home %d", MAX_LENGTH)
//								DEBUG_LOG("\r\nStart home %f", pMotorNode->fCoordLocation)
								//设置编码器距离
								IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, fTarget);
								//设置步进电机为负方向
								Stepper_t->m_pStepperBackward(Stepper_t->m_pThisPrivate);
								//步进电机准备运动
								Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, *Params_t.p_fSpeed);
								pMotorNode->bTargetChange = true;
								Stepper_t->m_pStepperMove(Stepper_t->m_pThisPrivate);
//								printf("\r\nstep1");
								break;
							default:
								break;
					}
					
					eHomeStep = eCHECK_LIMIT_FIRST;												
				case eCHECK_LIMIT_FIRST:
					//检查到接触了Home限位开关,在中断里检测
					if(!ReadLimitByID(pPrivate, *(uint8_t *)Params_t.p_iID, eZero, &bLimitStatu))
					{														
							return;
					}
					
					if(bLimitStatu)
					{
							DEBUG_LOG("\r\nDBG zero limit touch")
							
							IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, 0);
							while(!Stepper_t->m_pIsStepperStop(Stepper_t->m_pThisPrivate))
							{
									Delay_ms(10);
							}
							eHomeStep = eMOVE_REVERSE_DIR;		
//							printf("\r\nstep2");
							Delay_ms(10);
					}
					else
					{
//														DEBUG_LOG("\r\nchange status")
							break;
					}
				case eMOVE_REVERSE_DIR:	
					//反方向走10mm
//					if(!Stepper_t->m_pIsStepperStop(Stepper_t->m_pThisPrivate))
//					{
//							break;
//					}
					DEBUG_LOG("\r\nDBG leave zero limit")
					pMotorNode->bDirForward = true;
					pMotorNode->bFindZeroLimit = false;
					Stepper_t->m_pStepperForward(Stepper_t->m_pThisPrivate);
					IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, 10);
					Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, 10, 5);
					pMotorNode->bTargetChange = true;
					Stepper_t->m_pStepperMove(Stepper_t->m_pThisPrivate);
					eHomeStep = eWAIT_REVERSE_ARRIVED;
//					printf("\r\nstep3");
					//break;
				case eWAIT_REVERSE_ARRIVED:
					//等待10mm走完
//					if(IncEncoder_t->m_pIncEncoderTargetArrived(IncEncoder_t->m_pThisPrivate))
					if(IncEncoder_t->m_pIsTargetArrived(IncEncoder_t->m_pThisPrivate))
					{
							eHomeStep = eSET_HOME_PARAMS1;
							
					}
					else
					{
							break;
					}
				case eSET_HOME_PARAMS1:
					//以程序内规定的速度二次回零
					pMotorNode->bDirForward = false;
					pMotorNode->bFindZeroLimit = true;
					fTarget = (float)MAX_LENGTH * NEGTIVE_DIRECTION - pMotorNode->fCoordLocation;
					IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, fTarget);
					//pMotorNode->iPulseLocation = pMotorNode->iPulseLocation - MAX_LENGTH * NEGTIVE_DIRECTION * 
					Stepper_t->m_pStepperBackward(Stepper_t->m_pThisPrivate);
					Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, 3);
					pMotorNode->bTargetChange = true;
					Stepper_t->m_pStepperMove(Stepper_t->m_pThisPrivate);
					//Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, 2);
					eHomeStep = eCHECK_LIMIT_SECOND;
//					printf("\r\nstep4");
				case eCHECK_LIMIT_SECOND:
					//检查到二次接触限位开关,在中断里检测												
					if(!ReadLimitByID(pPrivate, *(uint8_t *)Params_t.p_iID, eZero, &bLimitStatu))
					{														
							return;
					}
					
					if(bLimitStatu)
					{
							DEBUG_LOG("\r\nDBG zero limit 2nd touch")
							//IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, 0);
							eHomeStep = eDONE;		
							Delay_ms(10);
					}
					else
					{
							break;
					}
				case eDONE:
					//停止,修改位置,Home完成
					IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, 0);
					pMotorNode->fCoordLocation = 0;	
					
					pMotorNode->bFindZeroLimit = false;
					//SetPositionEnforce(Stepper_t->m_pThisPrivate, 0);
					*eCmdType = HOME;												
					eHomeStep = eSET_HOME_PARAMS0;												
					DEBUG_LOG("\r\nDBG Home Complete")
					pMotorNode->bHomed = true;
					pMotorNode->fLastTarget = 0;
					pMotorNode->fPlanTarget = 0;
					break;
				default:
					break;										
		}
}

static void ExeMotorFindMaxBoundary(MotorList *pMotorNode)
{
		
}

static void ExeMotorMove(MotorList *pMotorNode, MoveNodeParams Params_t, CmdDataObj *eCmdType, bool *bStop)
{
		static eNormalMoveStep eMoveStep = eSET_PARAMS;
		static StepperControl *Stepper_t = NULL;
		static IncEncoderControl *IncEncoder_t = NULL;
		static float arrTarget[8];
		static uint8_t iReadIndex = 0;
		static uint8_t iWriteIndex = 0;
		static uint8_t iBufferLen = 0;
//		static float fLastTarget = 0;
		static bool bDirForward = false;

		Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
		IncEncoder_t = (IncEncoderControl *)pMotorNode->pEncoder_t;
	
		if(!pMotorNode->bHomed)
		{
				printf("\r\n home first");
				return;
		}	
		
		
		if(fabs(*Params_t.p_fTargetPos - pMotorNode->fLastTarget) > 0.01)
		{
				DEBUG_LOG("\r\nDBG dist %f", (*Params_t.p_fTargetPos - pMotorNode->fLastTarget))
				bDirForward = ((*Params_t.p_fTargetPos - pMotorNode->fLastTarget) > 0) ? true : false;
				DEBUG_LOG("\r\nDBG bdir %d", bDirForward)
				arrTarget[iWriteIndex] = *Params_t.p_fTargetPos;
				iWriteIndex = (iWriteIndex + 1) % 8;
				iBufferLen++;
				pMotorNode->fLastTarget = *Params_t.p_fTargetPos;
		}
		
		switch(eMoveStep)
		{
				case eSET_PARAMS:
					if(0 == iBufferLen)
					{
							DEBUG_LOG("\r\nDBG buffer 0")
							*bStop = true;
							break;
					}
					DEBUG_LOG("\r\nDBG start move")
					//DEBUG_LOG("\r\nDBG buffer len %d", iBufferLen)
					IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, arrTarget[iReadIndex]);
					pMotorNode->bFindZeroLimit = false;
					pMotorNode->bDirForward = bDirForward;
					DEBUG_LOG("\r\nDBG dir value %d", pMotorNode->bDirForward)
					pMotorNode->bTargetChange = true;
					Stepper_t->m_pStepperMove(Stepper_t->m_pThisPrivate);
					eMoveStep = eWAIT_ARRIVE;
				case eWAIT_ARRIVE:					
//					if(IncEncoder_t->m_pIncEncoderTargetArrived(IncEncoder_t->m_pThisPrivate))

					if(IncEncoder_t->m_pIsTargetArrived(IncEncoder_t->m_pThisPrivate))
					{
							//DEBUG_LOG("\r\nDBG target arrived")
							//IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, *Params_t.p_fTargetPos);
							eMoveStep = eARRIVE;
					}
					else
					{
							if(pMotorNode->m_pLimitTouch(pMotorNode->pLimitsList, eZero) && !pMotorNode->bDirForward)
							{				
									eMoveStep = eARRIVE;
							}
							else
							{
									break;
							}
					}
				case eARRIVE:
						*eCmdType = MOVE;		
						if(iBufferLen > 0)
						{
								iBufferLen--;
						}
						
						if(0 == iBufferLen)
						{
								//如果buffer中没有点位数据就需要设置为停止状态
								*bStop = true;
								//等待停止完成
								eMoveStep = eWAIT_STOP;
						}
						else
						{
								if(Stepper_t->m_pIsNextDirectionReverse(Stepper_t->m_pThisPrivate))
								{
										DEBUG_LOG("\r\nDBG dir rev")
										eMoveStep = eWAIT_STOP;
								}
								else
								{
										eMoveStep = eSET_PARAMS;
								}
						}
						iReadIndex = (iReadIndex + 1) % 8;
						break;
				case eWAIT_STOP:
					if(Stepper_t->m_pIsStepperStop(Stepper_t->m_pThisPrivate))
					{
							DEBUG_LOG("\r\nDBG wait stop")
							eMoveStep = eSET_PARAMS;
					}
					else
					{
							break;
					}
				default:
					break;
		}
		
//		switch(eMoveStep)
//		{
//				case eSET_PARAMS:
//						DEBUG_LOG("\r\nDBG start move")
//						if(fabs(pMotorNode->fCoordLocation - *Params_t.p_fTargetPos) > 0.1)
//						{
//								switch(pMotorNode->eMotorType)
//								{
//										case eSTEPPER_ENCODER:																																		
//											pMotorNode->bDirForward = (pMotorNode->fCoordLocation < *Params_t.p_fTargetPos) ? true : false;
//											pMotorNode->bFindZeroLimit = false;
//											if(pMotorNode->bDirForward)
//											{
//													DEBUG_LOG("\r\nDBG dir+")
//													pMotorNode->bDirForward = true;
//													Stepper_t->m_pStepperForward(Stepper_t->m_pThisPrivate);
//											}
//											else
//											{
//													DEBUG_LOG("\r\nDBG dir-")
//													pMotorNode->bDirForward = false;
//													Stepper_t->m_pStepperBackward(Stepper_t->m_pThisPrivate);
//											}
//											
//											DEBUG_LOG("\r\nDBG move target %f", *Params_t.p_fTargetPos)
//											IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, *Params_t.p_fTargetPos);
//											Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, 
//																									(pMotorNode->fCoordLocation - *Params_t.p_fTargetPos), 
//																									*Params_t.p_fSpeed);
//											pMotorNode->fCoordLocation = *Params_t.p_fTargetPos;
//											break;
//										default:
//											break;
//								}						
//								eMoveStep = eWAIT_ARRIVE;
//						}
//						else
//						{
//								eMoveStep = eARRIVE;
//						}
//				case eWAIT_ARRIVE:
//						if(IncEncoder_t->m_pIncEncoderTargetArrived(IncEncoder_t->m_pThisPrivate))
//						{
//								DEBUG_LOG("\r\nDBG target arrived")
//								//IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, *Params_t.p_fTargetPos);
//								eMoveStep = eARRIVE;
//						}
//						else
//						{
//								break;
//						}
//				case eARRIVE:
//						*eCmdType = MOVE;
//						eMoveStep = eSET_PARAMS;
//						break;
//			}									
}

static bool MotorMovePlan(MotorList *pMotorNode, MoveNodeParams Params_t)
{
		static StepperControl *Stepper_t = NULL;
		float fMoveDist = 0.0;
	
		if(NULL == pMotorNode)
		{
				return false;
		}
		
		Stepper_t = (StepperControl *)pMotorNode->pMotor_t;		

		fMoveDist = *Params_t.p_fTargetPos - pMotorNode->fPlanTarget;
		
//		if(fMoveDist > 0)
//		{
//				pMotorNode->bDirForward = true;
//		}
//		else
//		{
//				pMotorNode->bDirForward = false;
//		}
		if(fabs(*Params_t.p_fTargetPos - pMotorNode->fPlanTarget) < 0.01)
		{
				pMotorNode->fPlanTarget = *Params_t.p_fTargetPos;
				return true;				
		}
		pMotorNode->fCurrentMoveDist = fMoveDist;
		if(Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, fMoveDist, *Params_t.p_fSpeed))
		{
				pMotorNode->fPlanTarget = *Params_t.p_fTargetPos;
				return true;
		}		
		
		return false;
}

static void ExeMotorControl(PRIVATE_MEMBER_TYPE *pPrivate, CmdDataObj *eCmdType)
{		
		MoveNodeParams Params_t;
		static MoveNodeParams ParamsCopy_t;
		bool bHome;
		bool bStop = false;
		static MotorList *pMotorNode = NULL;		
		static eBlockExeSteps eExeSteps;
		static PrivateBlock *pPrivate_t = NULL;		
		static eExeType eCurrentExeType = eNoAction;
		
		pPrivate_t = (PrivateBlock *)pPrivate;
		if(NULL == pPrivate_t)
		{
				return;
		}
		//以复制方式预取运动参数,方便速度规划
		if(PopMoveData(pPrivate_t->pMoveNodeList, &Params_t))
		{
				pMotorNode = FindMotor(pPrivate, *(uint8_t *)Params_t.p_iID);
				if(NULL == pMotorNode)
				{
						printf("\r\nfunc:%s:can't find motor", __FUNCTION__);
						return;
				}
							
				if(*Params_t.p_bHome)
				{
						//DEBUG_LOG("\r\nhome cmd")		
						//如果当前没有运动且是Home指令, 将运动状态设置为Home
						if(eNoAction == eCurrentExeType)
						{
								eCurrentExeType = eMotorHome;
								//刷新运动buffer的读索引
								FreshNodeBufferReadIndex(pPrivate_t->pMoveNodeList);
								ParamsCopy_t = Params_t;
						}
				}
				else
				{
						if(pMotorNode->bHomed)
						{
								//不是Home指令, 进行速度规划
								if(MotorMovePlan(pMotorNode, Params_t))
								{
										//DEBUG_LOG("\r\nmove cmd")	
										//如果新的运动点加入规划成功则刷新读索引
										FreshNodeBufferReadIndex(pPrivate_t->pMoveNodeList);
										ParamsCopy_t = Params_t;
								}
								
								//如果当前没有运动, 将运动状态设置为运动
								if(eNoAction == eCurrentExeType)
								{
										eCurrentExeType = eMotorMove;										
								}
						}
				}
		}
		
		switch(eCurrentExeType)
		{
				case eNoAction:
					break;
				case eMotorHome:
					ExeMotorHome(pPrivate, pMotorNode, ParamsCopy_t, eCmdType);
					if(HOME == *eCmdType)
					{
							eCurrentExeType = eNoAction;
					}
					break;
				case eMotorMove:
					ExeMotorMove(pMotorNode, ParamsCopy_t, eCmdType, &bStop);
					if(bStop)
					{
							//如果运动执行返回是停止, 则运动状态设置为无动作
							eCurrentExeType = eNoAction;
					}
					break;
		}
		
//		switch(eExeSteps)
//		{
//			case ePOP_DATA:
////				pPrivate_t = (PrivateBlock *)pPrivate;
////				if(NULL == pPrivate_t)
////				{
////						printf("\r\nfunc:%s:null pointer", __FUNCTION__);
////						return;					
////				}
////				if(PopMoveData(pPrivate_t->pMoveNodeList, &Params_t))
////				{
////						//检查从buffer中取出数据的类型
////						if(*Params_t.p_bHome)
////						{
////								
////						}
////						else
////						{
////								
////						}
////						pMotorNode = FindMotor(pPrivate, *(uint8_t *)Params_t.p_iID);
////						if(NULL == pMotorNode)
////						{
////								printf("\r\nfunc:%s:can't find motor", __FUNCTION__);
////								return;
////						}
////						Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
////						IncEncoder_t = (IncEncoderControl *)pMotorNode->pEncoder_t;						
////						eExeSteps = eWAIT_DONE;
////				}
////				else
////				{
////						break;
////				}
//			case eWAIT_DONE:
//					#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1	
//							if(true == *Params_t.p_bHome)
//							{		
//									
////									switch(eHomeStep)
////									{
////											//如果是电机Home命令
////											case eSET_HOME_PARAMS0:
////												DEBUG_LOG("\r\nStart home")												
////												switch(pMotorNode->eMotorType)
////												{
////														case eSTEPPER_ENCODER:
////															//朝Home方向走长距离
////															pMotorNode->bDirForward = false;
////															pMotorNode->bFindZeroLimit = true;
////															fTarget = (float)MAX_LENGTH * NEGTIVE_DIRECTION - pMotorNode->fCoordLocation;
////															
////															//设置编码器距离
////															IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, fTarget);
////															//设置步进电机为负方向
////															Stepper_t->m_pStepperBackward(Stepper_t->m_pThisPrivate);
////															//步进电机准备运动
////															Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, *Params_t.p_fSpeed);
////															break;
////														default:
////															break;
////												}
////												
////												eHomeStep = eCHECK_LIMIT_FIRST;												
////											case eCHECK_LIMIT_FIRST:
////												//检查到接触了Home限位开关,在中断里检测
////												if(!ReadLimitByID(pPrivate, *(uint8_t *)Params_t.p_iID, eZero, &bLimitStatu))
////												{														
////														return;
////												}
////												
////												if(bLimitStatu)
////												{
////														DEBUG_LOG("\r\nDBG zero limit touch")
////														IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, 0);
////														eHomeStep = eMOVE_REVERSE_DIR;		
////														Delay_ms(10);
////												}
////												else
////												{
//////														DEBUG_LOG("\r\nchange status")
////														break;
////												}
////											case eMOVE_REVERSE_DIR:	
////												//反方向走10mm
////												DEBUG_LOG("\r\nDBG leave zero limit")
////												pMotorNode->bDirForward = true;
////												pMotorNode->bFindZeroLimit = false;
////												Stepper_t->m_pStepperForward(Stepper_t->m_pThisPrivate);
////												IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, 10);
////												Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, 10, 5);
////												eHomeStep = eWAIT_REVERSE_ARRIVED;
////												//break;
////											case eWAIT_REVERSE_ARRIVED:
////												//等待10mm走完
////												if(IncEncoder_t->m_pIncEncoderTargetArrived(IncEncoder_t->m_pThisPrivate))
////												{
////														eHomeStep = eSET_HOME_PARAMS1;
////												}
////												else
////												{
////														break;
////												}
////											case eSET_HOME_PARAMS1:
////												//以程序内规定的速度二次回零
////												pMotorNode->bDirForward = false;
////												pMotorNode->bFindZeroLimit = true;
////												fTarget = (float)MAX_LENGTH * NEGTIVE_DIRECTION - pMotorNode->fCoordLocation;
////												IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, fTarget);
////												//pMotorNode->iPulseLocation = pMotorNode->iPulseLocation - MAX_LENGTH * NEGTIVE_DIRECTION * 
////												Stepper_t->m_pStepperBackward(Stepper_t->m_pThisPrivate);
////												Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, 3);
////												//Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, 2);
////												eHomeStep = eCHECK_LIMIT_SECOND;
////											case eCHECK_LIMIT_SECOND:
////												//检查到二次接触限位开关,在中断里检测												
////												if(!ReadLimitByID(pPrivate, *(uint8_t *)Params_t.p_iID, eZero, &bLimitStatu))
////												{														
////														return;
////												}
////												
////												if(bLimitStatu)
////												{
////														DEBUG_LOG("\r\nDBG zero limit 2nd touch")
////														//IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, 0);
////														eHomeStep = eDONE;		
////														Delay_ms(10);
////												}
////												else
////												{
////														break;
////												}
////											case eDONE:
////												//停止,修改位置,Home完成
////												IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, 0);
////												pMotorNode->fCoordLocation = 0;	
////												pMotorNode->bHomed = true;
////												//SetPositionEnforce(Stepper_t->m_pThisPrivate, 0);
////												*eCmdType = HOME;												
////												eHomeStep = eSET_HOME_PARAMS0;												
////												eExeSteps = eEXE_DONE;
////												DEBUG_LOG("\r\nDBG Home Complete")
////												break;
////											default:
////												break;										
////									}
//							}
//							else
//							{							
//									if(!pMotorNode->bHomed)
//									{
//											printf("\r\n home first");
//											eExeSteps = eEXE_DONE;
//											break;
//									}
////									switch(eMoveStep)
////									{
////											case eSET_PARAMS:
////													DEBUG_LOG("\r\nDBG start move")
////													if(fabs(pMotorNode->fCoordLocation - *Params_t.p_fTargetPos) > 0.1)
////													{
////															switch(pMotorNode->eMotorType)
////															{
////																	case eSTEPPER_ENCODER:																
////																		Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
////																		//pMotorNode->bDirForward = true;
////																		pMotorNode->bDirForward = (pMotorNode->fCoordLocation < *Params_t.p_fTargetPos) ? true : false;
////																		pMotorNode->bFindZeroLimit = false;
////																		if(pMotorNode->bDirForward)
////																		{
////																				DEBUG_LOG("\r\nDBG dir+")
////																				pMotorNode->bDirForward = true;
////																				Stepper_t->m_pStepperForward(Stepper_t->m_pThisPrivate);
////																		}
////																		else
////																		{
////																				DEBUG_LOG("\r\nDBG dir-")
////																				pMotorNode->bDirForward = false;
////																				Stepper_t->m_pStepperBackward(Stepper_t->m_pThisPrivate);
////																		}
////																		
////																		DEBUG_LOG("\r\nDBG move target %f", *Params_t.p_fTargetPos)
////																		IncEncoder_t->m_pSetEncoderTarget(IncEncoder_t->m_pThisPrivate, *Params_t.p_fTargetPos);
////																		Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, 
////																																(pMotorNode->fCoordLocation - *Params_t.p_fTargetPos), 
////																																*Params_t.p_fSpeed);
////																		pMotorNode->fCoordLocation = *Params_t.p_fTargetPos;
////																		break;
////																	default:
////																		break;
////															}
////															//break;	
////															eMoveStep = eWAIT_ARRIVE;
////													}
////													else
////													{
////															eMoveStep = eARRIVE;
////													}
////											case eWAIT_ARRIVE:
////													if(IncEncoder_t->m_pIncEncoderTargetArrived(IncEncoder_t->m_pThisPrivate))
////													{
////															//printf("\r\nar");
////															DEBUG_LOG("\r\nDBG target arrived")
////															//IncEncoder_t->m_pSetEncoderValuef(IncEncoder_t->m_pThisPrivate, *Params_t.p_fTargetPos);
////															eMoveStep = eARRIVE;
////													}
////													else
////													{
////															//DEBUG_LOG("\r\nDBG target not arrived")
////															break;
////													}
////											case eARRIVE:
////													*eCmdType = MOVE;
////													eMoveStep = eSET_PARAMS;
////													eExeSteps = eEXE_DONE;
////													break;
////										}									
//							}
//							
//						#endif
//				break;
//			case eEXE_DONE:
//				eExeSteps = ePOP_DATA;	
//				break;
//		}
		
	
		//如果是最大位置命令
		//朝最大位置走长距离
		//检查到接触了最大位置限位开关,在中断里检测
		//停止,记录位置,完成
}

bool MotorControlInit(MotorControl *Block_t)
{
		//结构体指针指向初始化的内存区域
//		uint8_t i = 0;
		MotorVarToInt = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		PrivateBlock *pPrivate_t = (PrivateBlock *)MotorVarToInt;
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
		
		pPrivate_t->pMoveNodeList = MallocMoveNode_t();
		
		
		Block_t->m_pThisPrivate = pPrivate_t;
		Block_t->m_pAddMotor = AddMotor;
		Block_t->m_pSetMoveParams = SetMoveParams;
		Block_t->m_pAddLimits = AddLimits;
		Block_t->m_pReadLimitByID = ReadLimitByID;
		Block_t->m_pMotorHomeImmediately = MotorHomeImmediately;
		Block_t->m_pExeMotorControl = ExeMotorControl;
//		Block_t->m_pGetMotorMoveData = GetMotorMoveData;
		Block_t->m_pGetLinearLocation = GetLinearLocation;
		Block_t->m_pGetLinearSpeed = GetLinearSpeed;
		Block_t->m_pSetDirPinHighAsForward = SetDirPinHighAsForward;
		g_pMotorTabel = pPrivate_t->pMotorList;
		
		
//		RegisterMotorVar(Block_t->m_pThisPrivate, &MotorVarToInt);
		
		DEBUG_LOG("\r\nDBG Motor control init success")
		return true;
}
