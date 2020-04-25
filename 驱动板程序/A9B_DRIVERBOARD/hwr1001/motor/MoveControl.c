#include "MoveControl.h"



void MoveBlockInit(MoveBlock *structBlock, MoveBlockMsg *iMsg)
{
		structBlock->bBusy = false;
		structBlock->bDistanceArrived = false;
		structBlock->dAcceleration = 0.0;
}

static float CalcCurveForBlock(void)
{
	
}