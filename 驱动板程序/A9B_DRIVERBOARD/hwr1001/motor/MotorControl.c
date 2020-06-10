#include "MotorControl.h"
#include "StepperControl.h"
#include "IncEncoderControl.h"
#include "gpio.h"
#include "stdlib.h"


#define POSITIVE_DIRECTION	1		//正方向
#define NEGTIVE_DIRECTION		-1	//负方向

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
		uint8_t *p_iHomeDir;
	
		//根据需要指向一个或一组数据
		float *p_fSpeed;
	
		float *p_fTargetPos;
}MoveNodeParams;

//定义操作MoveControl模块的结构体类型
typedef struct 
{
		//单节点信息缓存区
		MoveNodeParams arrNodeBuffer[MOVE_NODE_NUM];
		//写缓存区索引
		uint8_t iWriteIndex;
		//读缓存区索引
		uint8_t iReadIndex;
		//缓存区长度
		uint8_t iBufferLen;
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
		eSET_HOME_PARAMS1,
		eCHECK_LIMIT_SECOND,
		eDONE
}eMotorHomeStep;

typedef enum
{
		eSET_PARAMS = 0,
		eWAIT_ARRIVE,
		eARRIVE
}eNormalMoveStep;

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
		
		for(i = 0; i < MOVE_NODE_NUM; i++)
		{
				#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
						pNode_t->arrNodeBuffer[i].p_iID = (uint8_t *)malloc(sizeof(uint8_t));
						pNode_t->arrNodeBuffer[i].p_bHome = (bool *)malloc(sizeof(bool));
						pNode_t->arrNodeBuffer[i].p_iHomeDir = (uint8_t *)malloc(sizeof(uint8_t));
						pNode_t->arrNodeBuffer[i].p_fSpeed = (float *)malloc(sizeof(float));
						if(NULL == pNode_t->arrNodeBuffer[i].p_iID || 
								NULL == pNode_t->arrNodeBuffer[i].p_bHome || 
								NULL == pNode_t->arrNodeBuffer[i].p_iHomeDir || 
								NULL == pNode_t->arrNodeBuffer[i].p_fSpeed)
						{
								printf("\r\nfunc:%s:var malloc failed", __FUNCTION__);
								return NULL;									
						}
				#else
						printf("\r\nfunc:%s:wrong hardware version", __FUNCTION__);
						return NULL;
				#endif								
		}
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
		MotorList *pList = NULL;
		MotorList *pNode = NULL;
		
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
		
		pList = (MotorList *)malloc(sizeof(MotorList));
		if(NULL == pList)
		{
				printf("\r\nfunc:%s:malloc list node failed", __FUNCTION__);
				return false;
		}
		
		pList->eMotorType = pParams_t->eMotorType;
		pList->pMotor_t = pStepper_t;
		pList->pEncoder_t = pIncEncoder_t;
		pList->pLimitsList = NULL;
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


void PushMoveData(MoveNodeList *Node_t, MoveNodeParams *Params_t)
{
		DEBUG_LOG("\r\nDBG Push Move Data")
	
		Node_t->arrNodeBuffer[Node_t->iWriteIndex].bBusy = Params_t->bBusy;
		Node_t->arrNodeBuffer[Node_t->iWriteIndex].bDistanceArrived = Params_t->bDistanceArrived;		
		Node_t->arrNodeBuffer[Node_t->iWriteIndex].bStop = Params_t->bStop;
	
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_bHome, Params_t->p_bHome, sizeof(bool));
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_iHomeDir, Params_t->p_iHomeDir, sizeof(uint8_t));
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_iID, Params_t->p_iID, sizeof(uint8_t));
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].p_fSpeed, Params_t->p_fSpeed, sizeof(uint32_t));
		#endif
	
		Node_t->iWriteIndex = (Node_t->iWriteIndex + 1) % MOVE_NODE_NUM;
		Node_t->iBufferLen++;
		
		DEBUG_LOG("\r\nDBG Push Move Data End")
}

bool PopMoveData(MoveNodeList *Node_t, MoveNodeParams *Params_t)
{
		if(Node_t->iBufferLen)
		{				
				DEBUG_LOG("\r\nDBG Pop Move Data")
				memcpy(Params_t, &Node_t->arrNodeBuffer[Node_t->iReadIndex], sizeof(MoveNodeParams));
			
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
					uint8_t iHomeDir = NEGTIVE_DIRECTION;
					float fSpeed = (float)(*iSpeed);
		#endif
		Params_t.bBusy = false;
		Params_t.bDistanceArrived = false;
		Params_t.p_bHome = &bHome;
		Params_t.p_iHomeDir = &iHomeDir;
		Params_t.bStop = false;
		Params_t.p_iID = iMotorID;
		Params_t.p_fSpeed = &fSpeed;
	
		PushMoveData(pNode, &Params_t);
}

bool SetMoveParams(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID, float *fDist, float *fMoveSpeed)
{
		MoveNodeParams Params_t;
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
					bool bHome = false;
					bool bStop = false;
					uint8_t iHomeDir = NEGTIVE_DIRECTION;
					float fSpeed = (float)(*fMoveSpeed);
		#endif
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return false;					
		}		
		
		Params_t.bBusy = false;
		Params_t.bDistanceArrived = false;
		
		PushMoveData(pPrivate->pMoveNodeList, &Params_t);
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
		ClearMoveNodeAll(pPrivate->pMoveNodeList);

		MotorHome(pPrivate->pMoveNodeList, &iMotorID, &iSpeed);
}

uint16_t GetMotorPulseParamByID(uint8_t iMotorID)
{
		
}

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


static void ExeMotorControl(PRIVATE_MEMBER_TYPE *pPrivate)
{		
		static MoveNodeParams Params_t;
		bool bHome;
		bool bLimitStatu = false;
		static MotorList *pMotorNode = NULL;
		static StepperControl *Stepper_t = NULL;
		static eMotorHomeStep eHomeStep;
		static eNormalMoveStep eMoveStep;
		static eBlockExeSteps eExeSteps;
		static PrivateBlock *pPrivate_t = NULL;
		float fDist;
		
		//DEBUG_LOG("\r\nDBG ExeMotorControl")
		switch(eExeSteps)
		{
			case ePOP_DATA:
				pPrivate_t = (PrivateBlock *)pPrivate;
				if(NULL == pPrivate_t)
				{
						printf("\r\nfunc:%s:null pointer", __FUNCTION__);
						return;					
				}
				if(PopMoveData(pPrivate_t->pMoveNodeList, &Params_t))
				{
						eExeSteps = eWAIT_DONE;
				}
				else
				{
						break;
				}
			case eWAIT_DONE:
					#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1	
							pMotorNode = FindMotor(pPrivate, *(uint8_t *)Params_t.p_iID);
							if(true == *Params_t.p_bHome)
							{		
									switch(eHomeStep)
									{
											//如果是电机Home命令
											case eSET_HOME_PARAMS0:
												DEBUG_LOG("\r\nStart home")
												switch(pMotorNode->eMotorType)
												{
														case eSTEPPER_ENCODER:
															Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
															//朝Home方向走长距离
															fDist = -(float)MAX_LENGTH * NEGTIVE_DIRECTION - *(pMotorNode->MoveInfo_t.fCurrentPos);
															Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, *Params_t.p_fSpeed);
															break;
														default:
															break;
												}
												
												eHomeStep = eCHECK_LIMIT_FIRST;												
											case eCHECK_LIMIT_FIRST:
												if(!ReadLimitByID(pPrivate, *(uint8_t *)Params_t.p_iID, eZero, &bLimitStatu))
												{
														return;
												}
												
												if(bLimitStatu)
												{
														SetPositionEnforce(Stepper_t->m_pThisPrivate, 0);
														eHomeStep = eMOVE_REVERSE_DIR;														
												}
												else
												{
//														DEBUG_LOG("\r\nchange status")
														break;
												}
											case eMOVE_REVERSE_DIR:												
												Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, 10, 3);
												eHomeStep = eSET_HOME_PARAMS1;
											case eSET_HOME_PARAMS1:
												Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, 2);
											case eCHECK_LIMIT_SECOND:
												if(CheckLimits(pMotorNode, eZERO_LIMIT))
												{
														eHomeStep = eDONE;
												}
												else
												{
														break;
												}
											case eDONE:
												SetPositionEnforce(Stepper_t->m_pThisPrivate, 0);
												eExeSteps = eEXE_DONE;
												break;
											default:
												break;										
									}
							}
							else
							{
									switch(eMoveStep)
									{
											case eSET_PARAMS:
													switch(pMotorNode->eMotorType)
													{
															case eSTEPPER_ENCODER:
																Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
																//朝Home方向走长距离
																fDist = *(Params_t.p_fTargetPos) - *(pMotorNode->MoveInfo_t.fCurrentPos);
																Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, *Params_t.p_fSpeed);
																break;
															default:
																break;
													}
													//break;
													eMoveStep = eWAIT_ARRIVE;
											case eWAIT_ARRIVE:
													break;
											case eARRIVE:
													eExeSteps = eEXE_DONE;
													break;
										}									
							}
							
						#endif
				break;
			case eEXE_DONE:
				eExeSteps = ePOP_DATA;	
				break;
		}
		
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

bool MotorControlInit(MotorControl *Block_t)
{
		//结构体指针指向初始化的内存区域
		uint8_t i = 0;
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
		
		pPrivate_t->pMoveNodeList = MallocMoveNode_t();
		
		
		Block_t->m_pThisPrivate = pPrivate_t;
		Block_t->m_pAddMotor = AddMotor;
		Block_t->m_pSetMoveParams = SetMoveParams;
		Block_t->m_pAddLimits = AddLimits;
		Block_t->m_pReadLimitByID = ReadLimitByID;
		Block_t->m_pMotorHomeImmediately = MotorHomeImmediately;
		Block_t->m_pExeMotorControl = ExeMotorControl;
		
		g_pMotorTabel = pPrivate_t->pMotorList;
		DEBUG_LOG("\r\nDBG Motor control init success")
		
		return true;
}
