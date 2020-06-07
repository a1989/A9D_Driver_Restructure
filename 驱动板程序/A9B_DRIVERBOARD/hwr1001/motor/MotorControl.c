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
		void *pMotor_t;
		void *pEncoder_t;
		struct structMotorList *pNext_t;
}MortorList;

typedef struct
{
		MortorList *pMotorList;
		char *strType;
}PrivateBlock;

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
		//����ǵ��Home����
		//��Home�����߳�����
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