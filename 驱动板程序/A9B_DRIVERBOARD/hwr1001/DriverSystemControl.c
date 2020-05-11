#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"
#include "Communication.h"
#include "Version.h"
#include "DriverStorage.h"

/*ϵͳ���ܷ�Ϊ���󲿷�:ͨ��, �˶�����, �洢*/
//�˶����ƿ�,���������˶���صĲ���, ʹ��ǰ����ʹ��MotionBlockInit()��ʼ��
MotionManageBlock MotionBlock_t;
CommunicationBlock CommunicationBlock_t;
StorageDataBlock StorageDataBlock_t;

//����ϵͳ����Ϣ
struct DevInfo
{
		uint8_t iMajorVersion;
		uint8_t iMinorVersion;
		uint8_t iDriverID;
}DriverBoardInfo;

//��ʼ�����õ������ݽṹ
void DataStructureInit(void)
{
		DriverBoardInfo.iMajorVersion = VERSION_MAJOR;
		DriverBoardInfo.iMinorVersion = VERSION_MINOR;
	
		while(!StorageBlockInit(&StorageDataBlock_t))
		{
				Delay_ms(2000);
		}
	
		while(!MotionBlockInit(&MotionBlock_t))
		{
				Delay_ms(2000);
		}
		
		while(!CommunicationBlockInit(&CommunicationBlock_t))
		{
				Delay_ms(2000);
		}
}

void DiverSystemInit(void)
{
		HardwareConifg();
		HardwareInit();
		DataStructureInit();
}

//�������ݷ���
void DataSendHandler(uint8_t *pData, uint8_t iDataLen)
{
		WriteSendBuffer(&CommunicationBlock_t, pData, iDataLen);
}

//��λ����������
void QueryFromHostHandler(const uint8_t *pRawData)
{
		uint8_t arrDataBuffer[16];
		uint8_t iDataLen = 0;
	
		switch((QueryDataObj)pRawData[0])
		{
			//����汾��
			case VERSION:		
				arrDataBuffer[0] = DriverBoardInfo.iMajorVersion;
				arrDataBuffer[1] = DriverBoardInfo.iMinorVersion;
				iDataLen = 2;
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
			case REALTIME_CURRENT:
				break;
			
		}
		
		DataSendHandler(arrDataBuffer, iDataLen);
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



void HeartBeatHandler(uint8_t *pData)
{
		WriteSendBuffer(&CommunicationBlock_t, pData, 1);
}

//������Ϣ����
RecvDataType RecvDataAnalyze(const uint8_t *pRawData, uint8_t *pData)
{
		RecvDataType iType;
		if(NULL == pRawData || NULL == pData)
		{
				iType = Error;
		}
		else
		{
				switch(pRawData[0])
				{
						case 0x81:
							memcpy(pData, pRawData, 1);
							iType = HEARTBEAT;
							break;
						case 0x01:
							memcpy(pData, pRawData + 1, 1);
							iType = CMD;
							break;
						case 0x02:
							memcpy(pData, pRawData + 1, 1);
							iType = QUERY;
							break;
						case 0x03:
							iType = UPDATE;
							break;
				}
		}
		
		return iType;
}

//������������Ӧ�Ĵ���
void ProcessData(uint8_t *pRawData)
{
		RecvDataType iDataType;
		uint8_t pData[16];
		if(NULL == pRawData)
		{
				printf("\r\nfunc:%s, error:null pointer", __FUNCTION__);
				return;
		}
		iDataType = RecvDataAnalyze(pRawData, pData);
		switch(iDataType)
		{
			case HEARTBEAT:
				HeartBeatHandler(pData);
				break;
			case QUERY:
				QueryFromHostHandler(pData);
				break;
			case CMD:
				CommandFromHostHandler();
				break;
			case UPDATE:
				break;
			case Error:
				//sprintf();
				printf("\r\nfunc:%s,Error Data", __FUNCTION__);
				break;
		}
}

void DriverSystemRun(void)
{
		uint8_t arrData[16];
		//����յ����ݾʹ�������
		if(HostDataGet(&CommunicationBlock_t, arrData))
		{
				ProcessData(arrData);
		}
		//
		SlaveDataSend(&CommunicationBlock_t);
		//���ݵ�ǰ��״̬ȷ��LED����˸���
		LED_ErrorInstruction();
		//ι��
		FeedWatchDog();
}