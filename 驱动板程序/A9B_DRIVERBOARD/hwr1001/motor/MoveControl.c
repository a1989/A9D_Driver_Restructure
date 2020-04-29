#include "MoveControl.h"

#define POSITIVE_DIRECTION	1		//正方向
#define NEGTIVE_DIRECTION		-1	//负方向

void MoveBlockInit(MoveBlock *structBlock, MoveBlockMsg *iMsg)
{
		structBlock->bBusy = false;
		structBlock->bDistanceArrived = false;
		structBlock->dAcceleration = 0.0;
}

static float CalcCurveForBlock(void)
{
	
}