#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"

/*ϵͳ���ܷ�Ϊ���󲿷�:ͨ��, �˶�����, �洢*/
//�˶����ƿ�,���������˶���صĲ���, ʹ��ǰ����ʹ��MotionBlockInit()��ʼ��
MotionManageBlock StructMotionBlock;

//��ʼ�����õ������ݽṹ
void DataStructureInit(void)
{
		
		MotionBlockMsg iMsg;
		if(!MotionBlockInit(&StructMotionBlock, &iMsg))
		{
			//0x01:�˶����ƿ����δ�����ڴ�
			//0x02:���ź���ָ��Ϊ��
			//0x04:��Ŵ���
		}
}

void DiverSystemInit(void)
{
		HardwareConifg();
		HardwareInit();
		DataStructureInit();
}

void QueryFromHostHandler(void)
{
		
		switch()
		{
			//����汾��
			case VERSION:				
				break;
			//����ϸ������
			case SUBDIVISION:
				break;
			//����������
			case MAX_CURRENT:
				break;
			//��������г�
			case MAX_LEN:
				break;
			//����ʵʱλ��
			case REALTIME_LOCATION:
				break;
			//����ʵʱ�ٶ�
			case REALTIME_SPEED:
				break;
			//������λ״̬
			case LIMIT_STATUS:
				break;
			
		}
		
		SetTransData();
}

void CommandFromHostHandler(void)
{
		switch()
		{
			case MOVE:
				//���˶�����д���˶����ƿ�
				StructMotionBlock.m_SetMoveData();
				break;
			case HOME:
				StructMotionBlock.m_HomeAxis();
				break;
			case BOARD_RESET:
				break;
			case EN_TORQUE:
				StructMotionBlock.m_SetTorque();
				break;
			case MOTOR_STOP:
				StructMotionBlock.m_StopMotor();
				break;
			case SET_ID:
				break;
			case SET_CURRENT:
				StructMotionBlock.m_SetCurrent();
				break;
			case SET_SUBDIVISION:
				StructMotionBlock.m_SetSubdivision();
				break;				
		}
}


void RecvDataAnalyze()
{
		if()
}

void ProcessData(void)
{
		RecvDataAnalyze();
		switch()
		{
			case HEARTBEAT:
				break;
			case QUERY:
				QueryFromHostHandler();
				break;
			case CMD:
				CommandFromHostHandler();
				break;
			case UPDATE:
				break;
		}
}

void DriverSystemRun(void)
{
		//����յ����ݾʹ�������
		if(HostDataGet())
		{
				ProcessData();
		}
		
		//���ݵ�ǰ��״̬ȷ��LED����˸���
		LED_ErrorInstruction();
		//ι��
		FeedWatchDog();
}