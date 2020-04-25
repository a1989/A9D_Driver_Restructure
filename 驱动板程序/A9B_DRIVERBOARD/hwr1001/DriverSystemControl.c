#include "DriverSystemControl.h"
#include "MotionControl.h"
#include "Hardware.h"

/*系统功能分为三大部分:通信, 运动控制, 存储*/
//运动控制块,包含所有运动相关的操作, 使用前必须使用MotionBlockInit()初始化
MotionManageBlock StructMotionBlock;


//初始化所用到的数据结构
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
		//如果收到数据就处理数据
		if(GetControlData())
		{
				ProcessData();
		}
			
		FeedWatchDog();
}