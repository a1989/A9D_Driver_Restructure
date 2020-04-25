#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"

/*ϵͳ���ܷ�Ϊ���󲿷�:ͨ��, �˶�����, �洢*/
//�˶����ƿ�,���������˶���صĲ���, ʹ��ǰ����ʹ��MotionBlockInit()��ʼ��
MotionManageBlock StructMotionBlock;


//��ʼ�����õ������ݽṹ
void DataStructureInit(void)
{
		MotionBlockInit(&StructMotionBlock);
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