#include "MotorControl.h"
#include "StepperControl.h"
#include "IncEncoderControl.h"
#include "stdlib.h"

#define POSITIVE_DIRECTION	1		//������
#define NEGTIVE_DIRECTION		-1	//������

/*��ģ�����Ͷ���*/
typedef struct structMotorList
{
		uint8_t iMotorID;
		MotorType eMotorType;	
		void *pMotor_t;
		void *pEncoder_t;
		struct structMotorList *pNext_t;
}MotorList;

//ÿ���˶����ڵ�Ĳ���
typedef struct
{
		uint8_t *iID;
		//���˶��ڵ�æ��־
		bool bBusy;
	
		//����λ�ñ�־
		bool bDistanceArrived;
	
		//ֹͣ��־
		bool bStop;
	
		//�Ƿ���Home
		bool *bHome;
		
		//Home����
		uint8_t *iHomeDir;
	
		//������Ҫָ��һ����һ������
		float *fSpeed;
	
}MoveNodeParams;

//�������MoveControlģ��Ľṹ������
typedef struct 
{
		//���ڵ���Ϣ������
		MoveNodeParams arrNodeBuffer[MOVE_NODE_NUM];
		//д����������
		uint8_t iWriteIndex;
		//������������
		uint8_t iReadIndex;
		//����������
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
		eSET_HOME_PARAMS0 = 0,
		eCHECK_LIMIT_FIRST,
		eMOVE_REVERSE_DIR,
		eSET_HOME_PARAMS1,
		eCHECK_LIMIT_SECOND,
		eDONE
}eMotorHomeStep;

/*��ģ�����Ͷ���End*/


//����һƬ�ڴ�����, ��ʼ������, ����ָ����Ƭ�ڴ������MoveNode_t����ָ��
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
						pNode_t->arrNodeBuffer[i].iID = (uint8_t *)malloc(sizeof(uint8_t));
						pNode_t->arrNodeBuffer[i].bHome = (bool *)malloc(sizeof(bool));
						pNode_t->arrNodeBuffer[i].iHomeDir = (uint8_t *)malloc(sizeof(uint8_t));
						pNode_t->arrNodeBuffer[i].fSpeed = (float *)malloc(sizeof(float));
						if(NULL == pNode_t->arrNodeBuffer[i].iID || 
								NULL == pNode_t->arrNodeBuffer[i].bHome || 
								NULL == pNode_t->arrNodeBuffer[i].iHomeDir || 
								NULL == pNode_t->arrNodeBuffer[i].fSpeed)
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
					//��ʼ���������
					DEBUG_LOG("\r\nDBG Start init a stepper")
					pStepper_t = (StepperControl *)malloc(sizeof(StepperControl));
					if(NULL == pStepper_t)
					{
							printf("\r\nfunc:%s:malloc stepper block failed", __FUNCTION__);
							return false;								
					}
					//ȡ�����ò���
					pStepperSysParams_t = (StepperSysParams *)pParams_t->pMotorSysParams;
					StepperControlInit(pStepper_t, &pStepperSysParams_t->StepperParams_t);
					//��ʼ��������
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
		//�ṹ��ָ��ָ���ʼ�����ڴ�����
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

void PushMoveData(MoveNodeList *Node_t, MoveNodeParams *Params_t)
{
		Node_t->arrNodeBuffer[Node_t->iWriteIndex].bBusy = Params_t->bBusy;
		Node_t->arrNodeBuffer[Node_t->iWriteIndex].bDistanceArrived = Params_t->bDistanceArrived;		
		Node_t->arrNodeBuffer[Node_t->iWriteIndex].bStop = Params_t->bStop;
	
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].bHome, Params_t->bHome, sizeof(bool));
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].iHomeDir, Params_t->iHomeDir, sizeof(uint8_t));
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].iID, Params_t->iID, sizeof(uint8_t));
				memcpy(Node_t->arrNodeBuffer[Node_t->iWriteIndex].fSpeed, Params_t->fSpeed, sizeof(uint32_t));
		#endif
	
		Node_t->iWriteIndex = (Node_t->iWriteIndex + 1) % MOVE_NODE_NUM;
		Node_t->iBufferLen++;
}

void PopMoveData(MoveNodeList *Node_t, MoveNodeParams *Params_t, bool bDone)
{
		if(Node_t->iBufferLen)
		{				

				if(!Node_t->arrNodeBuffer[Node_t->iReadIndex].bBusy)
				{
						Node_t->arrNodeBuffer[Node_t->iReadIndex].bBusy = true;
				}
				if(bDone)
				{
						Node_t->arrNodeBuffer[Node_t->iReadIndex].bBusy = false;
						Node_t->iReadIndex = (Node_t->iReadIndex + 1) % MOVE_NODE_NUM;
						Node_t->iBufferLen--;
				}
		}
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
}

void StopAxisImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t iMotorID)
{
		
}

//ƽ����ֹͣ��
void StopAxisModerate(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;					
		}				
}

//��ص���ʼλ��
void HomeAxis(MoveNodeList *pNode, uint8_t *iMotorID, uint32_t *iSpeed)
{		
		MoveNodeParams Params_t;
		
#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
			bool bHome = true;
			bool bStop = false;
			uint8_t iHomeDir = NEGTIVE_DIRECTION;
			float fSpeed = ((float)(*iSpeed)) / 10;
#endif
		Params_t.bBusy = false;
		Params_t.bDistanceArrived = false;
		Params_t.bHome = &bHome;
		Params_t.iHomeDir = &iHomeDir;
		Params_t.bStop = false;
		Params_t.iID = iMotorID;
		Params_t.fSpeed = &fSpeed;
	
		PushMoveData(pNode, &Params_t);
}

void HomeAxisImmediately(PRIVATE_MEMBER_TYPE *pThisPrivate, uint8_t *iMotorID, uint32_t *iSpeed)
{
		PrivateBlock *pPrivate = (PrivateBlock *)pThisPrivate;
	
		if(NULL == pPrivate)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;					
		}
		
		StopAxisModerate(pPrivate, iMotorID);
		ClearMoveNodeAll(pPrivate->pMoveNodeList);

		HomeAxis(pPrivate->pMoveNodeList, iMotorID, iSpeed);
}

uint16_t GetMotorPulseParamByID(uint8_t iMotorID)
{
		
}

void PauseAxisImmediately()
{
		
}



//static void SigleMotorControl(MoveNode_t *pNode)
//{		
//		float fDistance;
//		float fSpeed;
//		MoveNodeParams Params_t = pNode->arrNodeBuffer[pNode->iReadIndex];
//		switch(Params_t.arrMotorType[Params_t.eAxisIndex])
//		{
//				case eSTEPPER:
//					break;
//				case eSTEPPER_ENCODER:
//					//pNode->MotorControl_t.Stepper_t.m_pSingleEncoderStepperPrepare();
//					break;
//				case eBRUSHLESS:
//					break;
//				default:
//					break;
//		}
//}

//void ExecuteBlock(MoveNode_t *pNode)
//{	
//		if(pNode->iBufferLen)
//		{
//				switch(pNode->arrNodeBuffer[pNode->iReadIndex].eControlType)
//				{
//						case CONTROL_SINGLE_AXIS:							
////							SigleMotorControl(&pNode->arrNodeBuffer[pNode->iReadIndex]);
//							break;
//						default:
//							break;
//				}
//				pNode->iReadIndex = (pNode->iReadIndex + 1) % MOVE_NODE_NUM;
//				pNode->iBufferLen--;
//		}
//}

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

bool CheckLimits(MotorList *pMotorNode, eLimitType eLimit)
{
		
}

static void ExeMotorControl(PRIVATE_MEMBER_TYPE *pPrivate)
{
		MoveNodeParams Params_t;
		bool bHome;
		static MotorList *pMotorNode = NULL;
		static StepperControl *Stepper_t = NULL;
		static eMotorHomeStep eHomeStep;
//		PopMoveData();
		#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1				
				if(true == *Params_t.bHome)
				{
						pMotorNode = FindMotor(pPrivate, *(uint8_t *)Params_t.iID);
				}
				switch(eHomeStep)
				{
						//����ǵ��Home����
						case eSET_HOME_PARAMS0:
							switch(pMotorNode->eMotorType)
							{
									case eSTEPPER_ENCODER:
										Stepper_t = (StepperControl *)pMotorNode->pMotor_t;
										//��Home�����߳�����
										Stepper_t->m_pStepperPrepare(Stepper_t->m_pThisPrivate, MAX_LENGTH * NEGTIVE_DIRECTION, *Params_t.fSpeed);
										break;
									default:
										break;
							}
							//break;
							eHomeStep = eCHECK_LIMIT_FIRST;
						case eCHECK_LIMIT_FIRST:
							if(CheckLimits(pMotorNode, eZERO_LIMIT))
							{
									Stepper_t->m_pSetPositionEnforce(Stepper_t->m_pThisPrivate, 0);
									eHomeStep = eMOVE_REVERSE_DIR;
							}
							else
							{
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
							Stepper_t->m_pSetPositionEnforce(Stepper_t->m_pThisPrivate, 0);
							break;
						default:
							break;										
				}
		#endif

		
		//��鵽�Ӵ���Home��λ����,���ж�����
		//��������10mm
		//�Գ����ڹ涨���ٶȶ��λ���
		//��鵽���νӴ���λ����,���ж�����
		//ֹͣ,�޸�λ��,Home���
	
		//��������λ������
		//�����λ���߳�����
		//��鵽�Ӵ������λ����λ����,���ж�����
		//ֹͣ,��¼λ��,���
}