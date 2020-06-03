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
//         					佛祖保佑       永无BUG                      	//
////////////////////////////////////////////////////////////////////


#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"
#include "Communication.h"
#include "Version.h"
#include "DriverStorage.h"
#include "dma.h"
#include "usart.h"

/*系统功能分为四大部分:通信, 运动控制, 存储, 硬件操作*/
//运动控制块,包含所有运动相关的操作, 使用前必须使用MotionBlockInit()初始化
MotionManageBlock 	g_SingleAxis_t;
CommunicationBlock 	g_Communication_t;
StorageControl 	g_StorageDataBlock_t;
DevInfo 	DriverBoardInfo;
//驱动系统的信息


//初始化所用到的数据结构
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
	
		//初始化存储器件
		while(!StorageBlockInit(&g_StorageDataBlock_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				//配置存储设备的参数
				StorageParams_t.iID = 0;
				StorageParams_t.iHardwareAddress = 0xA0;
				StorageParams_t.eStorageDevice = eAT24C512;
				StorageParams_t.eStorageMode = eI2C2;
				g_StorageDataBlock_t.m_pAddDevice(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif	
		
		Delay_ms(10);
		goto CANCFG;
			
		//配置运动设备驱动
		while(!MotionBlockInit(&g_SingleAxis_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				//配置电机参数
				MotorParams_t.iMotorID = 0;
				MotorParams_t.eMotorType = eSTEPPER_ENCODER;
				//读取保存在EEPROM中的配置
				//读取电流配置
				ByteOptions_t.iAddress = EEPROM_CURRENT_CFG_ADDR;
				g_StorageDataBlock_t.m_pStorageReadByte(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t, &ByteOptions_t);
				StepperSysParams_t.StepperParams_t.iCurrentCfg = ByteOptions_t.iData;
				//读取细分配置
				ByteOptions_t.iAddress = EEPROM_SUBDIVISION_CFG_ADDR;
				g_StorageDataBlock_t.m_pStorageReadByte(g_StorageDataBlock_t.m_pThisPrivate, &StorageParams_t, &ByteOptions_t);
				StepperSysParams_t.StepperParams_t.iSubdivisionCfg = ByteOptions_t.iData;
		
				StepperSysParams_t.StepperParams_t.eConfigMode = eSPI1;
				StepperSysParams_t.StepperParams_t.eDriver = eDRV8711;
				StepperSysParams_t.StepperParams_t.eMotorTIM = eTIM2;
				StepperSysParams_t.EncoderParmas_t.eEncoderTIM = eTIM3;
				StepperSysParams_t.EncoderParmas_t.iEncoderLines = 1000;
				StepperSysParams_t.EncoderParmas_t.iMultiplication = 4;
				
				MotorParams_t.MotorSysParams = &StepperSysParams_t;
				//新增一个电机
				g_SingleAxis_t.m_pAddMotor(g_SingleAxis_t.m_pThisPrivate, &MotorParams_t);
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif

CANCFG:

		while(!CommunicationBlockInit(&g_Communication_t))
		{
				Delay_ms(2000);
		}
		
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				ParamsCAN1.eType = eCAN1;				
				//读取板ID				
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
		//串口用于调试,作为一个独立模块单独初始化
		#if HARDWARE_VERSION == CHENGDU_DESIGN
				MX_DMA_Init();
				MX_USART1_UART_Init();
		#elif HARDWARE_VERSION == SHENZHEN_DESIGN_V1
				
		#endif	
	
		Delay_ms(200);		
		printf("\r\nUSART init success");
	
		DataStructureInit();
}

//处理数据发送
void DataSendHandler(uint8_t *pData, uint8_t iDataLen)
{
		
}

//上位机请求数据
//void QueryFromHostHandler(const uint8_t *pRawData)
//{
//		uint8_t arrDataBuffer[16];
//		uint8_t iDataLen = 0;
//	
//		switch((QueryDataObj)pRawData[0])
//		{
//			//请求版本号
//			case VERSION:		
//				arrDataBuffer[0] = DriverBoardInfo.iMajorVersion;
//				arrDataBuffer[1] = DriverBoardInfo.iMinorVersion;
//				iDataLen = 2;
//				break;
//			//请求细分设置
//			case SUBDIVISION:
//				iDataLen = 2;
//				break;
//			//请求最大电流
//			case MAX_CURRENT:
//				break;
//			//请求最大行程
//			case MAX_LEN:
//				break;
//			//请求实时位置
//			case REALTIME_LOCATION:
//				break;
//			//请求实时速度
//			case REALTIME_SPEED:
//				break;
//			//请求限位状态
//			case LIMIT_STATUS:
//				break;
//			case REALTIME_CURRENT:
//				break;
//			
//		}
//		
//		DataSendHandler(arrDataBuffer, iDataLen);
//}

//上位机命令
//void CommandFromHostHandler(void)
//{
//		switch()
//		{
//			case MOVE:
//				//将运动数据写入运动控制块
//				MotionBlock_t.m_SetMoveData();
//				break;
//			case HOME:
//				SingleAxis_t.m_HomeAxis();
//				break;
//			case BOARD_RESET:
//				break;
//			case EN_TORQUE:
//				MotionBlock_t.m_SetTorque();
//				break;
//			case MOTOR_STOP:
//				MotionBlock_t.m_StopMotor();
//				break;
//			case SET_ID:
//				break;
//			case SET_CURRENT:
//				StructMotionBlock.m_SetCurrent();
//				break;
//			case SET_SUBDIVISION:
//				StructMotionBlock.m_SetSubdivision();
//				break;				
//		}
//}

//心跳
void HeartBeatHandler(uint8_t *pData, uint8_t iDataLen, CommunicationType eType)
{
		uint8_t iData = HEART_BEAT_DATA;
		g_Communication_t.m_pSlaveDataPrepare(g_Communication_t.m_pThisPrivate, &iData, iDataLen, eType);
}

//解析消息类型
static RecvDataType RecvDataAnalyze(const uint8_t *pRawData, const uint8_t iRawDataLen, uint8_t *pData, uint8_t *iDataLen)
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
							*iDataLen = 1;
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

//根据数据做对应的处理
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
				//组合命令
				bClearBuffer = false;
				break;
			case QUERY:
//				QueryFromHostHandler(pData);
				break;
			case CMD:
//				CommandFromHostHandler();
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

//主程序
void DriverSystemRun(void)
{
		uint8_t arrData[16];
		uint8_t iDataLen = 0;
		bool bDataAvailabel;
	
		g_Communication_t.m_pPopMessage(g_Communication_t.m_pThisPrivate, arrData, &iDataLen, &bDataAvailabel);
		//如果收到数据就处理数据
		if(bDataAvailabel)
		{
				ProcessMessage(arrData, iDataLen);
		}
		
		g_Communication_t.m_pExeBlock(g_Communication_t.m_pThisPrivate);
		//
		
		//根据当前的状态确定LED的闪烁情况
		//LED_ErrorInstruction();
		//喂狗
		//FeedWatchDog();
}