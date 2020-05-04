#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"

/*系统功能分为三大部分:通信, 运动控制, 存储*/
//运动控制块,包含所有运动相关的操作, 使用前必须使用MotionBlockInit()初始化
MotionManageBlock StructMotionBlock;

//初始化所用到的数据结构
void DataStructureInit(void)
{
		
		MotionBlockMsg iMsg;
		if(!MotionBlockInit(&StructMotionBlock, &iMsg))
		{
			//0x01:运动控制块变量未分配内存
			//0x02:轴编号函数指针为空
			//0x04:轴号错误
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
			//请求版本号
			case VERSION:				
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
			
		}
		
		SetTransData();
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


void RecvDataAnalyze()
{
		if()
}

void ProcessData(void)
{
		RecvDataAnalyze();
		switch()
		{
			case HEARTBEAT:
				break;
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
		//如果收到数据就处理数据
		if(HostDataGet())
		{
				ProcessData();
		}
		
		//根据当前的状态确定LED的闪烁情况
		LED_ErrorInstruction();
		//喂狗
		FeedWatchDog();
}