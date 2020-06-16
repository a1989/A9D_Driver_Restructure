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
#include "gpio.h"
#include "tim.h"
#include "timer.h"

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
		DRV8711_PinConfig DRV8711_PinConfig_t;
		StepperSysParams StepperSysParams_t;
		CommunicationParams ParamsCAN1;
		StorageParams StorageParams_t;
		StorageByteOptions ByteOptions_t;
		LimitParams LimitParams_t;
		uint32_t iStdId = 0x0;
		DriverBoardInfo.iMajorVersion = VERSION_MAJOR;
		DriverBoardInfo.iMinorVersion = VERSION_MINOR;
		bool bPinHighAsForward;
		uint8_t iMotorID = 0;
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
				MX_GPIO_Init();
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
				StepperSysParams_t.StepperParams_t.eConfigMode = eSPI2;		//SPI2��ʽ
				StepperSysParams_t.StepperParams_t.eDriver = eDRV8711;		//����ΪDRV8711
				StepperSysParams_t.StepperParams_t.eMotorTIM = eTIM2;			//����ʹ�õ�������TIM2����
				
				DRV8711_PinConfig_t.CSPin.GPIO_Port = GPIOB;
				DRV8711_PinConfig_t.CSPin.GPIO_Pin = GPIO_PIN_12;
				DRV8711_PinConfig_t.DirPin.GPIO_Port = GPIOA;
				DRV8711_PinConfig_t.DirPin.GPIO_Pin = GPIO_PIN_1;
				DRV8711_PinConfig_t.ResetPin.GPIO_Port = GPIOB;
				DRV8711_PinConfig_t.ResetPin.GPIO_Pin = GPIO_PIN_1;
				DRV8711_PinConfig_t.SleepPin.GPIO_Port = GPIOB;
				DRV8711_PinConfig_t.SleepPin.GPIO_Pin = GPIO_PIN_0;
				
				StepperSysParams_t.StepperParams_t.pDriverPinConfig = &DRV8711_PinConfig_t;				
				StepperSysParams_t.EncoderParmas_t.eEncoderTIM = eTIM3;		//������ʹ��TIM3����
				StepperSysParams_t.EncoderParmas_t.iEncoderLines = ENCODER_LINES;		//����Ϊ1000��
				StepperSysParams_t.EncoderParmas_t.iMultiplication = ENCODER_MULTIPLY;		//������4��Ƶ
				StepperSysParams_t.StepperParams_t.fFeedBackRatio = (float)(ENCODER_LINES * ENCODER_MULTIPLY) / (StepperSysParams_t.StepperParams_t.iSubdivisionCfg * 200);
				
				MotorParams_t.pMotorSysParams = &StepperSysParams_t;
				//����һ�����
				g_MotionBlock_t.m_pAddMotor(g_MotionBlock_t.m_pThisPrivate, &MotorParams_t);
				//Ϊ��ǰ�����������λ����
				LimitParams_t.eFunc = eZero;
				LimitParams_t.GPIO_Port = GPIOB;
				LimitParams_t.GPIO_Pin = GPIO_PIN_9;
				LimitParams_t.iMotorBelong = 0;
				g_MotionBlock_t.m_pAddMotorLimit(g_MotionBlock_t.m_pThisPrivate, &LimitParams_t);
				
				//Ϊ��ǰ������������λ����
				LimitParams_t.eFunc = ePositive;
				LimitParams_t.GPIO_Port = GPIOB;
				LimitParams_t.GPIO_Pin = GPIO_PIN_8;
				LimitParams_t.iMotorBelong = 0;
				g_MotionBlock_t.m_pAddMotorLimit(g_MotionBlock_t.m_pThisPrivate, &LimitParams_t);
				
				MX_TIM4_Init();
				TIM4_IT_Interrupt_Switch (1);
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
				DEBUG_LOG("\r\nRead Board ID:0x%x", DriverBoardInfo.iDriverID);
				g_Communication_t.m_pAddCommunicationInterface(g_Communication_t.m_pThisPrivate, ParamsCAN1);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
		#endif
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				if(0xB1 == iStdId || 0xA2 == iStdId)
//				if(0)
				{
						bPinHighAsForward = false;
						g_MotionBlock_t.m_pSetMotorDirPinHighAsForward(g_MotionBlock_t.m_pThisPrivate, &iMotorID, &bPinHighAsForward);
				}
				else
				{
						bPinHighAsForward = true;
						g_MotionBlock_t.m_pSetMotorDirPinHighAsForward(g_MotionBlock_t.m_pThisPrivate, &iMotorID, &bPinHighAsForward);
				}
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
	
		//������״̬����λֵ
		bool bSwitchValue0 = false;
		bool bSwitchValue1 = false;
		uint8_t iLimitValue = 0;
	
		//ʵʱλ�ú��ٶ�
		uint32_t iLinearSpeed;
		uint32_t iLinearPos;
		uint8_t iMotorID;
	
		//���������
		arrDataBuffer[0] = 0x0;
		arrDataBuffer[1] = 0x0;
		arrDataBuffer[2] = pRawData[0];	
		switch((QueryDataObj)pRawData[0])
		{
			//����汾��
			case VERSION:		
				DEBUG_LOG("\r\nQuerry Version")
				arrDataBuffer[3] = 2;
				arrDataBuffer[4] = DriverBoardInfo.iMajorVersion;
				arrDataBuffer[5] = DriverBoardInfo.iMinorVersion;
				iDataLen = 6;
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
			//����ʵʱλ���ٶ�
			case REALTIME_LOCATION_SPEED:
				DEBUG_LOG("\r\nQuerry linear pos speed")
				iMotorID = 0;
				g_MotionBlock_t.m_pGetMotorLinearLocation(g_MotionBlock_t.m_pThisPrivate, &iMotorID, &iLinearPos);
				g_MotionBlock_t.m_pGetMotorLinearSpeed(g_MotionBlock_t.m_pThisPrivate, &iMotorID, &iLinearSpeed);
				DEBUG_LOG("\r\nlinear pos:%d,speed:%d", iLinearPos, iLinearSpeed)
				arrDataBuffer[3] = 0x04;
				arrDataBuffer[4] = (iLinearPos >> 8) & 0xFF;
				arrDataBuffer[5] = iLimitValue & 0xFF;
				arrDataBuffer[6] = (iLimitValue >> 8) & 0xFF;
				arrDataBuffer[7] = iLimitValue & 0xFF;
				iDataLen = 8;
				break;
			//������λ״̬
			case LIMIT_STATUS:
				DEBUG_LOG("\r\nQuerry Limits Status")
				g_MotionBlock_t.m_pReadMotorLimit(g_MotionBlock_t.m_pThisPrivate, 0, eZero, &bSwitchValue0);
				g_MotionBlock_t.m_pReadMotorLimit(g_MotionBlock_t.m_pThisPrivate, 0, ePositive, &bSwitchValue1);
				if(bSwitchValue0 && (!bSwitchValue1))
				{
						iLimitValue = 1;
				}
				else if(bSwitchValue1 && (!bSwitchValue0))
				{
						iLimitValue = 2;
				}
				else
				{
						iLimitValue = 0;
				}
				
				arrDataBuffer[3] = 1;
				arrDataBuffer[4] = iLimitValue;
				iDataLen = 5;
				break;
			case REALTIME_CURRENT:
				break;			
		}
		
		DataSendHandler(arrDataBuffer, iDataLen, eCAN1);
}

//��λ������
void CommandFromHostHandler(const uint8_t *pRawData, const uint8_t iDataLen)
{
		uint32_t iDistRaw;
		uint32_t iSpeedRaw;
		uint8_t iMotorID;
	
		switch((CmdDataObj)pRawData[0])
		{
			case MOVE:
				DEBUG_LOG("\r\nDBG CMD Move")
				//���˶�����д���˶����ƿ�
				iDistRaw = ((uint32_t)pRawData[2] << 8 | pRawData[3]);
				iSpeedRaw = ((uint32_t)pRawData[4] << 8 | pRawData[5]);
				iMotorID = 0;
				if(4 == pRawData[1])
				{
						
						g_MotionBlock_t.m_pSetMotorMoveData(g_MotionBlock_t.m_pThisPrivate, &iMotorID, &iDistRaw, &iSpeedRaw);
				}
				break;
			case HOME:
				DEBUG_LOG("\r\nDBG CMD Home")
				#if HARDWARE_VERSION == CHENGDU_DESIGN || HARDWARE_VERSION == SHENZHEN_DESIGN_V1
						if(2 == pRawData[1])
						{
								g_MotionBlock_t.m_pHomeAxisImmediately(g_MotionBlock_t.m_pThisPrivate, 0, ((uint32_t)pRawData[2] << 8 | pRawData[3]));
						}
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
			default:
				break;
		}
}

//����
void HeartBeatHandler(uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
{
		uint8_t *iData = pData;
		*iData = HEART_BEAT_DATA;
		g_Communication_t.m_pSlaveDataPrepare(g_Communication_t.m_pThisPrivate, iData, iDataLen, eType);
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
//				switch(pRawData[0])
//				{
//						case 0x81:
//							memcpy(pData, pRawData, 1);
//							*iDataLen = 1;
//							eType = HEARTBEAT;
//							break;
//						case 0x01:
//							memcpy(pData, pRawData + 2, 1);
//							*iDataLen = iRawDataLen - 2;
//							eType = CMD;
//							break;
//						case 0x02:
//							memcpy(pData, pRawData + 2, 1);
//							eType = QUERY;
//							break;
//						case 0x03:
//							eType = UPDATE;
//							break;
//				}
//				DEBUG_LOG("\r\nstart data analyze")
				
				if(0x81 == pRawData[0])
				{
						memcpy(pData, pRawData, 1);
						*iDataLen = 1;
						eType = HEARTBEAT;						
				}
				else
				{
						if(pRawData[2] <= 0x08)
						{
//								DEBUG_LOG("\r\npRawData:0x%x,len:%d", *(pRawData + 2), (iRawDataLen - 2))
								memcpy(pData, (pRawData + 2), (iRawDataLen - 2));								
								*iDataLen = iRawDataLen - 2;
								eType = QUERY;								
						}
						else if((pRawData[2] >= 0x08) && (pRawData[2] <= 0x22))
						{
								memcpy(pData, pRawData + 2, iRawDataLen - 2);
								*iDataLen = iRawDataLen - 2;
								eType = CMD;								
						}
						else if(pRawData[2] >= 0xA0)
						{
								
						}
						else
						{
							
						}
				}
		}
		
//		DEBUG_LOG("\r\nend data analyze")
		
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
				pData[1] = DriverBoardInfo.iDriverID;
				HeartBeatHandler(pData, iDataLen + 1, eCAN1);
				break;
			case ASSEMBLE:
				//�������
				bClearBuffer = false;
				break;
			case QUERY:
				DEBUG_LOG("\r\nQuery Process")
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
		uint8_t arrData[16] = {0x00, 0x00};
		uint8_t iDataLen = 0;
		bool bDataAvailabel;
		CmdDataObj eCmdType = DO_NOTHING;
		uint8_t iMotorID;
		uint32_t iLinearPos;
		
		g_Communication_t.m_pPopMessage(g_Communication_t.m_pThisPrivate, arrData, &iDataLen, &bDataAvailabel);
		//����յ����ݾʹ�������
		if(bDataAvailabel)
		{
				ProcessMessage(arrData, iDataLen);
		}
		
		
		g_Communication_t.m_pExeBlock(g_Communication_t.m_pThisPrivate);
		g_MotionBlock_t.m_ExeMotionBlcok(g_MotionBlock_t.m_pThisPrivate, &eCmdType);
		switch(eCmdType)
		{
				case HOME:
					arrData[2] = (uint8_t)HOME;
					DataSendHandler(arrData, 3, eCAN1);
					break;
				case MOVE:
					arrData[2] = (uint8_t)MOVE;
					arrData[3] = 0x02;
					iMotorID = 0;
					g_MotionBlock_t.m_pGetMotorLinearLocation(g_MotionBlock_t.m_pThisPrivate, &iMotorID, &iLinearPos);
					arrData[4] = (iLinearPos >> 8) & 0xFF;
					arrData[5] = iLinearPos & 0xFF;
					DataSendHandler(arrData, 6, eCAN1);
					break;
				default:
					break;
		}
		//
		
		//���ݵ�ǰ��״̬ȷ��LED����˸���
		//LED_ErrorInstruction();
		//ι��
		//FeedWatchDog();
}
