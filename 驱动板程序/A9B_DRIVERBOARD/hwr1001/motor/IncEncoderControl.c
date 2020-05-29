#include "IncEncoderControl.h"

typedef struct
{
		int32_t iCurrentCount;
		uint32_t iCountPerRound;
}PrivateBlock;

void IncEncoderControlInit(IncEncoderControl *Block_t, EncoderParmas *Params_t)
{
		PrivateBlock *pPrivate_t = NULL;
		if(NULL == Block_t || NULL == Params_t)
		{
				printf("\r\nfunc:%s:block null pointer", __FUNCTION__);
				return;
		}		
		
		pPrivate_t = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		pPrivate_t->iCountPerRound = Params_t->iEncoderLines * Params_t->iMultiplication;
		
}