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
			case VERSION:
				break;
			case SUBDIVISION:
				break;
			case CURRENT:
				break;
			case MAX_LEN:
				break;
			case REALTIME_LOCATION:
				break;
			case REALTIME_SPEED:
				break;
			case LIMIT_STATUS:
				break;
			
		}
}

void CommandFromHostHandler(void)
{
		
}

void ProcessData(void)
{
		switch()
		{
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
		if(GetControlData())
		{
				ProcessData();
		}
		
		//根据当前的状态确定LED的闪烁情况
		LED_ErrorInstruction();
		//喂狗
		FeedWatchDog();
}