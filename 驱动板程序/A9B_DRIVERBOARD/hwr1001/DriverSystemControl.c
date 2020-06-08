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
#include "Communication.h"
#include "Version.h"
#include "DriverStorage.h"
#include "dma.h"
#include "usart.h"

/*ϵͳ���ܷ�Ϊ�Ĵ󲿷�:ͨ��, �˶�����, �洢, Ӳ������*/
//�˶����ƿ�,���������˶���صĲ���, ʹ��ǰ����ʹ��MotionBlockInit()��ʼ��
MotionManageBlock 	g_MotionBlock_t;
CommunicationBlock 	g_Communication_t;
StorageControl 	g_StorageDataBlock_t;
DevInfo 	DriverBoardInfo;
//����ϵͳ����Ϣ


//��ʼ�����õ������ݽṹ
static void DataStructureInit(void)
{
		MotorParams MotorParams_t;
		StepperSysParams StepperSysParams_t;
		CommunicationParams ParamsCAN1;
		StorageParams StorageParams_t;
		StorageByteOptions ByteOptions_t;
		uint32_t iStdId = 0x0;
		DriverBoardInfo.iMajorVersion = VERSION_MAJOR;
		DriverBoardInfo.iMinorVersion = VERSION_MINOR;
	
		//��ʼ���洢����
		while(!StorageBlockInit(&g_StorageDataBlock_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				//���ô洢�豸�Ĳ���
				StorageParams_t.iID = 0;
				StorageParams_t.iHardwareAddress = 0xA0;
				StorageParams_t.eStorageDevice = eAT24C512;
				StorageParams_t.eStorageMode = eI2C2;
				g_StorageDataBlock_t.m_pAddDevice(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif	
		
		Delay_ms(10);
//		goto CANCFG;
			
		//�����˶��豸����
		while(!MotionBlockInit(&g_MotionBlock_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				MotorParams_t.pMotorSysParams = NULL;
		
				//���õ������,����Ӳ��,���Ϊ���������Ĳ������
				MotorParams_t.iMotorID = 0;
				MotorParams_t.eMotorType = eSTEPPER_ENCODER;
				
				//��ȡ������EEPROM�е�����
				//��ȡ��������
				ByteOptions_t.iAddress = EEPROM_CURRENT_CFG_ADDR;
				g_StorageDataBlock_t.m_pStorageReadByte(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t, &ByteOptions_t);
				StepperSysParams_t.StepperParams_t.iCurrentCfg = ByteOptions_t.iData;
		
				//��ȡϸ������
				ByteOptions_t.iAddress = EEPROM_SUBDIVISION_CFG_ADDR;
				g_StorageDataBlock_t.m_pStorageReadByte(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t, &ByteOptions_t);
				StepperSysParams_t.StepperParams_t.iSubdivisionCfg = ByteOptions_t.iData;
		
				//����Ӳ�����ò�������ͱ���������
				StepperSysParams_t.StepperParams_t.eConfigMode = eSPI1;		//SPI1��ʽ
				StepperSysParams_t.StepperParams_t.eDriver = eDRV8711;		//����ΪDRV8711
				StepperSysParams_t.StepperParams_t.eMotorTIM = eTIM2;			//����ʹ�õ�������TIM2����
				StepperSysParams_t.EncoderParmas_t.eEncoderTIM = eTIM3;		//������ʹ��TIM3����
				StepperSysParams_t.EncoderParmas_t.iEncoderLines = ENCODER_LINES;		//����Ϊ1000��
				StepperSysParams_t.EncoderParmas_t.iMultiplication = ENCODER_MULTIPLY;		//������4��Ƶ
				
				MotorParams_t.pMotorSysParams = &StepperSysParams_t;
				//����һ�����
				g_MotionBlock_t.m_pAddMotor(g_MotionBlock_t.m_pThisPrivate, &MotorParams_t);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif

//CANCFG:

		while(!CommunicationBlockInit(&g_Communication_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				ParamsCAN1.eType = eCAN1;				
				//��ȡ��ID				
				ByteOptions_t.iAddress = EEPROM_ID_CFG_ADDR;
				g_StorageDataBlock_t.m_pStorageReadByte(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t, &ByteOptions_t);
				iStdId = ByteOptions_t.iData;
				ParamsCAN1.pParam = &iStdId;	
				DriverBoardInfo.iDriverID = ByteOptions_t.iData;
				g_Communication_t.m_pAddCommunicationInterface(g_Communication_t.m_pThisPrivate, ParamsCAN1);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
		#endif
}

void DiverSystemInit(void)
{
		//�������ڵ���,��Ϊһ������ģ�鵥����ʼ��
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				MX_DMA_Init();
				MX_USART1_UART_Init();
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif	
	
		Delay_ms(200);		
		printf("\r\nUSART init success");
	
		DataStructureInit();
}

//�������ݷ���
void DataSendHandler(uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
{
		g_Communication_t.m_pSlaveDataPrepare(g_Communication_t.m_pThisPrivate, pData, iDataLen, eType);		
}

//��λ����������
void QueryFromHostHandler(const uint8_t *pRawData, const uint8_t iRawDataLen)
{
		uint8_t arrDataBuffer[16];
		uint8_t iDataLen = 0;
	
		switch((QueryDataObj)pRawData[0])
		{
			//����汾��
			case VERSION:		
				DEBUG_LOG("\r\nQuerry Version")
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
		
		DataSendHandler(arrDataBuffer, iDataLen, eCAN1);
}

//��λ������
void CommandFromHostHandler(const uint8_t *pRawData, const uint8_t iDataLen)
{
		switch((CmdDataObj)pRawData[0])
		{
			case MOVE:
				//���˶�����д���˶����ƿ�
//				MotionBlock_t.m_SetMoveData();
				break;
			case HOME:
				#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
						g_MotionBlock_t.m_pHomeAxisImmediately(g_MotionBlock_t.m_pThisPrivate, 0, (pRawData[2] << 8 | pRawData[3]));
				#endif
				break;
			case BOARD_RESET:
				break;
			case EN_TORQUE:
//				MotionBlock_t.m_SetTorque();
				break;
			case MOTOR_STOP:
//				MotionBlock_t.m_StopMotor();
				break;
			case SET_ID:
				break;
			case SET_CURRENT:
//				StructMotionBlock.m_SetCurrent();
				break;
			case SET_SUBDIVISION:
//				StructMotionBlock.m_SetSubdivision();
				break;				
		}
}

//����
void HeartBeatHandler(uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
{
		uint8_t iData = HEART_BEAT_DATA;
		g_Communication_t.m_pSlaveDataPrepare(g_Communication_t.m_pThisPrivate, &iData, iDataLen, eType);
}

//������Ϣ����
static RecvDataType RecvDataAnalyze(const uint8_t *pRawData, const uint8_t iRawDataLen, uint8_t *pData, uint8_t *iDataLen)
{
		RecvDataType eType;
	
		if(NULL == pRawData || NULL == pData)
		{
				eType = Error;
		}		
		else
		{
				switch(pRawData[0])
				{
						case 0x81:
							memcpy(pData, pRawData, 1);
							*iDataLen = 1;
							eType = HEARTBEAT;
							break;
						case 0x01:
							memcpy(pData, pRawData + 2, 1);
							*iDataLen = iRawDataLen - 2;
							eType = CMD;
							break;
						case 0x02:
							memcpy(pData, pRawData + 2, 1);
							eType = QUERY;
							break;
						case 0x03:
							eType = UPDATE;
							break;
				}
		}
		
		return eType;
}

//������������Ӧ�Ĵ���
bool ProcessMessage(const uint8_t *pRawData, const uint8_t iRawDataLen)
{
		RecvDataType eDataType;
		static uint8_t pData[16] = {0};
		static uint8_t iDataLen = 0;
		bool bClearBuffer = true;
		
		if(NULL == pRawData)
		{
				printf("\r\nfunc:%s, error:null pointer", __FUNCTION__);
				return false;
		}
		
		eDataType = RecvDataAnalyze(pRawData, iRawDataLen, pData, &iDataLen);
		
		switch(eDataType)
		{
			case HEARTBEAT:
				DEBUG_LOG("\r\nHeart Beat Process")
				HeartBeatHandler(pData, iDataLen, eCAN1);
				break;
			case ASSEMBLE:
				//�������
				bClearBuffer = false;
				break;
			case QUERY:
				QueryFromHostHandler(pData, iDataLen);
				break;
			case CMD:
				CommandFromHostHandler(pData, iDataLen);
				break;
			case UPDATE:
				break;
			case Error:
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
		uint8_t arrData[16];
		uint8_t iDataLen = 0;
		bool bDataAvailabel;
	
		g_Communication_t.m_pPopMessage(g_Communication_t.m_pThisPrivate, arrData, &iDataLen, &bDataAvailabel);
		//����յ����ݾʹ�������
		if(bDataAvailabel)
		{
				ProcessMessage(arrData, iDataLen);
		}
		
		g_Communication_t.m_pExeBlock(g_Communication_t.m_pThisPrivate);
		//
		
		//���ݵ�ǰ��״̬ȷ��LED����˸���
		//LED_ErrorInstruction();
		//ι��
		//FeedWatchDog();
}