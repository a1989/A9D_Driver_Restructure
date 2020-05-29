#include "DRV8711_Operation.h"

typedef struct
{
		SPI_HandleTypeDef hSPI;
}PrivateBlock;

void DRV8711_Init(DRV8711_Block *Block_t, DRV8711_Params *Params_t)
{
		PrivateBlock *pPrivate = (PrivateBlock *)malloc(sizeof(PrivateBlock));
		SPI_TypeDef *SPI_t;
		
		switch(Params_t->eMode)
		{
				case eSPI1:
					SPI_t = SPI1;
					break;
				case eSPI2:
					SPI_t = SPI2;
					break;
				default:
					break;
		}
		
		MX_SPI_Init(pPrivate->hSPI, SPI_t);
		
		Block_t->m_pThisPrivate = pPrivate;
}


	