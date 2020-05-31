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

/*系统功能分为三大部分:通信, 运动控制, 存储*/
//运动控制块,包含所有运动相关的操作, 使用前必须使用MotionBlockInit()初始化
MotionManageBlock 	g_SingleAxis_t;
CommunicationBlock 	g_BlockCAN1_t;
StorageControl 	g_StorageDataBlock_t;

//驱动系统的信息
struct DevInfo
{
		uint8_t iMajorVersion;
		uint8_t iMinorVersion;
		uint8_t iDriverID;
}DriverBoardInfo;

//初始化所用到的数据结构
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
				//配置电机参数
				MotorParams_t.iMotorID = 0;
				MotorParams_t.eMotorType = eSTEPPER_ENCODER;
				StepperSysParams_t.StepperParams_t.eConfigMode = eSPI1;
				StepperSysParams_t.StepperParams_t.eDriver = eDRV8711;
				StepperSysParams_t.StepperParams_t.eMotorTIM = eTIM2;
				StepperSysParams_t.EncoderParmas_t.eEncoderTIM = eTIM3;
				StepperSysParams_t.EncoderParmas_t.iEncoderLines = ;
				StepperSysParams_t.EncoderParmas_t.iMultiplication = ;

				
				MotorParams_t.MotorSysParams = &StepperSysParams_t;
				//新增一个电机
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

//处理数据发送
void DataSendHandler(uint8_t *pData, uint8_t iDataLen)
{
		WriteSendBuffer(&CommunicationBlock_t, pData, iDataLen);
}

//上位机请求数据
void QueryFromHostHandler(const uint8_t *pRawData)
{
		uint8_t arrDataBuffer[16];
		uint8_t iDataLen = 0;
	
		switch((QueryDataObj)pRawData[0])
		{
			//请求版本号
			case VERSION:		
				arrDataBuffer[0] = DriverBoardInfo.iMajorVersion;
				arrDataBuffer[1] = DriverBoardInfo.iMinorVersion;
				iDataLen = 2;
				break;
			//请求细分设置
			case SUBDIVISION:
				iDataLen = 2;
				break;
			//请求最大电流
			case MAX_CURRENT:
				break;
			//请求最大行程
			case MAX_LEN:
				break;
			//请求实时位置
			case REALTIME_LOCATION:
				break;
			//请求实时速度
			case REALTIME_SPEED:
				break;
			//请求限位状态
			case LIMIT_STATUS:
				break;
			case REALTIME_CURRENT:
				break;
			
		}
		
		DataSendHandler(arrDataBuffer, iDataLen);
}

//上位机命令
void CommandFromHostHandler(void)
{
		switch()
		{
			case MOVE:
				//将运动数据写入运动控制块
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

//心跳
void HeartBeatHandler(uint8_t *pData)
{
		uint8_t iData = HEART_BEAT_DATA;
		g_BlockCAN1_t.m_pSlaveDataPrepare(g_BlockCAN1_t.m_pThisPrivate, &iData, 1);
}

//解析消息类型
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

//根据数据做对应的处理
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

//主程序
void DriverSystemRun(void)
{
		uint8_t *arrData;
		uint8_t iDataLen = 0;
		bool bDataAvailabel;
	
		g_BlockCAN1_t.m_pPopMessage(g_BlockCAN1_t.m_pThisPrivate, arrData, &iDataLen, &bDataAvailabel)
		//如果收到数据就处理数据
		if(bDataAvailabel)
		{
				ProcessData(arrData, iDataLen);
		}
		//
		SlaveDataSend(&CommunicationBlock_t);
		//根据当前的状态确定LED的闪烁情况
		LED_ErrorInstruction();
		//喂狗
		FeedWatchDog();
}