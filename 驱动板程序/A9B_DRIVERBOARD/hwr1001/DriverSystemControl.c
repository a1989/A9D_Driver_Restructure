////////////////////////////////////////////////////////////////////
//                          _ooOoo_                               //
//                         o8888888o                              //
//                         88" . "88                              //
//                         (| ^_^ |)                              //
//                         O\  =  /O                              //
//                      ____/`---'\____                           //
//                    .'  \\|     |//  `.                         //
//                   /  \\|||  :  |||//  \                        //
//                  /  _||||| -:- |||||-  \                       //
//                  |   | \\\  -  /// |   |                       //
//                  | \_|  ''\---/''  |   |                       //
//                  \  .-\__  `-`  ___/-. /                       //
//                ___`. .'  /--.--\  `. . ___                     //
//              ."" '<  `.___\_<|>_/___.'  >'"".                  //
//            | | :  `- \`.;`\ _ /`;.`/ - ` : | |                 //
//            \  \ `-.   \_ __\ /__ _/   .-` /  /                 //
//      ========`-.____`-.___\_____/___.-`____.-'========         //
//                           `=---='                              //
//      ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^        //
//         					���汣��       ����BUG                      	//
////////////////////////////////////////////////////////////////////


#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"
#include "Communication.h"
#include "Version.h"
#include "DriverStorage.h"

/*ϵͳ���ܷ�Ϊ���󲿷�:ͨ��, �˶�����, �洢*/
//�˶����ƿ�,���������˶���صĲ���, ʹ��ǰ����ʹ��MotionBlockInit()��ʼ��
MotionManageBlock 	g_SingleAxis_t;
CommunicationBlock 	g_BlockCAN1_t;
StorageControl 	g_StorageDataBlock_t;

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
		MotorParams MotorParams_t;
		StepperSysParams StepperSysParams_t;
		CommunicationParams ParamsCAN1;
		StorageParams StorageParams_t;
		uint32_t iStdId = 0x0;
		DriverBoardInfo.iMajorVersion = VERSION_MAJOR;
		DriverBoardInfo.iMinorVersion = VERSION_MINOR;
	
		while(!StorageBlockInit(&g_StorageDataBlock_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				StorageParams_t.eStorageDevice = eAT24C512;
				StorageParams_t.eStorageMode = eI2C2;
				g_StorageDataBlock_t.m_pAddDevice(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif	
		
		while(!MotionBlockInit(&g_SingleAxis_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				//���õ������
				MotorParams_t.iMotorID = 0;
				MotorParams_t.eMotorType = eSTEPPER_ENCODER;
				StepperSysParams_t.StepperParams_t.eConfigMode = eSPI1;
				StepperSysParams_t.StepperParams_t.eDriver = eDRV8711;
				StepperSysParams_t.StepperParams_t.eMotorTIM = eTIM2;
				StepperSysParams_t.EncoderParmas_t.eEncoderTIM = eTIM3;
				StepperSysParams_t.EncoderParmas_t.iEncoderLines = ;
				StepperSysParams_t.EncoderParmas_t.iMultiplication = ;

				
				MotorParams_t.MotorSysParams = &StepperSysParams_t;
				//����һ�����
				g_SingleAxis_t.m_pAddMotor(g_SingleAxis_t.m_pThisPrivate, &MotorParams_t);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif
		
		ParamsCAN1.eType = eCAN1;
		ParamsCAN1.pParam = &iStdId;
		while(!CommunicationBlockInit(&g_BlockCAN1_t, ParamsCAN1))
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
				iDataLen = 2;
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

//��λ������
void CommandFromHostHandler(void)
{
		switch()
		{
			case MOVE:
				//���˶�����д���˶����ƿ�
				MotionBlock_t.m_SetMoveData();
				break;
			case HOME:
				SingleAxis_t.m_HomeAxis();
				break;
			case BOARD_RESET:
				break;
			case EN_TORQUE:
				MotionBlock_t.m_SetTorque();
				break;
			case MOTOR_STOP:
				MotionBlock_t.m_StopMotor();
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

//����
void HeartBeatHandler(uint8_t *pData)
{
		uint8_t iData = HEART_BEAT_DATA;
		g_BlockCAN1_t.m_pSlaveDataPrepare(g_BlockCAN1_t.m_pThisPrivate, &iData, 1);
}

//������Ϣ����
static RecvDataType RecvDataAnalyze(const uint8_t *pRawData, uint8_t *pData, uint8_t iDataLen)
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
void ProcessData(uint8_t *pRawData, uint8_t iDataLen)
{
		RecvDataType iDataType;
		static uint8_t pData[16] = {0};
		bool bClearBuffer = true;
		if(NULL == pRawData)
		{
				printf("\r\nfunc:%s, error:null pointer", __FUNCTION__);
				return;
		}
		iDataType = RecvDataAnalyze(pRawData, pData, iDataLen);
		switch(iDataType)
		{
			case HEARTBEAT:
				HeartBeatHandler(pData);
				break;
			case ASSEMBLE:
				bClearBuffer = false;
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
		
		if(bClearBuffer)
		{
				memset(pData, 0, sizeof(pData));
		}
}

//������
void DriverSystemRun(void)
{
		uint8_t *arrData;
		uint8_t iDataLen = 0;
		bool bDataAvailabel;
	
		g_BlockCAN1_t.m_pPopMessage(g_BlockCAN1_t.m_pThisPrivate, arrData, &iDataLen, &bDataAvailabel)
		//����յ����ݾʹ�������
		if(bDataAvailabel)
		{
				ProcessData(arrData, iDataLen);
		}
		//
		SlaveDataSend(&CommunicationBlock_t);
		//���ݵ�ǰ��״̬ȷ��LED����˸���
		LED_ErrorInstruction();
		//ι��
		FeedWatchDog();
}