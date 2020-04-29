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

void DriverSystemRun(void)
{
		//����յ����ݾʹ�������
		if(GetControlData())
		{
				ProcessData();
		}
			
		FeedWatchDog();
}