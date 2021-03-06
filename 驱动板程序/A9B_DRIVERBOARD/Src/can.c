/**
  ******************************************************************************
  * File Name          : CAN.c
  * Description        : This file provides code for the configuration
  *                      of the CAN instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "can.h"

/* USER CODE BEGIN 0 */
#include "includes.h"
#include "defines.h"
/* USER CODE END 0 */

//CAN_HandleTypeDef hcan;
//CAN_HandleTypeDef g_hCAN1;

/* CAN init function */
void MX_CAN_Init(CAN_HandleTypeDef *hCAN, CAN_TypeDef *CAN_t)
{

  hCAN->Instance = CAN_t;
  hCAN->Init.Prescaler = 8;
  hCAN->Init.Mode = CAN_MODE_NORMAL;
  hCAN->Init.SJW = CAN_SJW_1TQ;
  hCAN->Init.BS1 = CAN_BS1_5TQ;
  hCAN->Init.BS2 = CAN_BS2_3TQ;
  hCAN->Init.TTCM = DISABLE;
  hCAN->Init.ABOM = DISABLE;
  hCAN->Init.AWUM = DISABLE;
  hCAN->Init.NART = ENABLE;
  hCAN->Init.RFLM = DISABLE;
  hCAN->Init.TXFP = DISABLE;
  if (HAL_CAN_Init(hCAN) != HAL_OK)
  {
		DEBUG_LOG("\r\nMX CAN init failed")
    Error_Handler();
  }
	
	DEBUG_LOG("\r\nMX CAN init success")
}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  if(canHandle->Instance == CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN GPIO Configuration    
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX 
    */
    GPIO_InitStruct.Pin = CAN_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(CAN_RX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = CAN_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(CAN_TX_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN1_MspInit 1 */
  
  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance == CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();
  
    /**CAN GPIO Configuration    
    PA11     ------> CAN_RX
    PA12     ------> CAN_TX 
    */
    HAL_GPIO_DeInit(GPIOA, CAN_RX_Pin|CAN_TX_Pin);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
