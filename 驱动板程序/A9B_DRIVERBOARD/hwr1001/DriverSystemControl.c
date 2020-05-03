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
			case VERSION:
				break;
			case SUBDIVISION:
				break;
			case CURRENT:
				break;
			case MAX_LEN:
				break;
			case REALTIME_LOCATION:
				break;
			case REALTIME_SPEED:
				break;
			case LIMIT_STATUS:
				break;
			
		}
}

void CommandFromHostHandler(void)
{
		
}

void ProcessData(void)
{
		switch()
		{
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
		if(GetControlData())
		{
				ProcessData();
		}
		
		//���ݵ�ǰ��״̬ȷ��LED����˸���
		LED_ErrorInstruction();
		//ι��
		FeedWatchDog();
}