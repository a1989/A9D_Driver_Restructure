#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"
#include "Communication.h"
#include "Version.h"
#include "DriverStorage.h"

/*系统功能分为三大部分:通信, 运动控制, 存储*/
//运动控制块,包含所有运动相关的操作, 使用前必须使用MotionBlockInit()初始化
MotionManageBlock MotionBlock_t;
CommunicationBlock CommunicationBlock_t;
StorageDataBlock StorageDataBlock_t;

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

void CommandFromHostHandler(void)
{
		switch()
		{
			case MOVE:
				//将运动数据写入运动控制块
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

//解析消息类型
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

//根据数据做对应的处理
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
		//如果收到数据就处理数据
		if(HostDataGet(&CommunicationBlock_t, arrData))
		{
				ProcessData(arrData);
		}
		//
		SlaveDataSend(&CommunicationBlock_t);
		//根据当前的状态确定LED的闪烁情况
		LED_ErrorInstruction();
		//喂狗
		FeedWatchDog();
}