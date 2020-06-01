/**
  ******************************************************************************
  * File Name          : USART.c
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

//UART_HandleTypeDef huart1;
//DMA_HandleTypeDef hdma_usart1_tx;

USART_Handle	USART_Handle_t;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  USART_Handle_t.hUART.Instance = USART1;
  USART_Handle_t.hUART.Init.BaudRate = 115200;
  USART_Handle_t.hUART.Init.WordLength = UART_WORDLENGTH_8B;
  USART_Handle_t.hUART.Init.StopBits = UART_STOPBITS_1;
  USART_Handle_t.hUART.Init.Parity = UART_PARITY_NONE;
  USART_Handle_t.hUART.Init.Mode = UART_MODE_TX_RX;
  USART_Handle_t.hUART.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  USART_Handle_t.hUART.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&USART_Handle_t.hUART) != HAL_OK)
  {
    Error_Handler();
  }
	
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();
  
    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    GPIO_InitStruct.Pin = USART1_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(USART1_TX_GPIO_Port, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = USART1_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(USART1_RX_GPIO_Port, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_TX Init */
    USART_Handle_t.hUSART_DMA.Instance = DMA1_Channel4;
    USART_Handle_t.hUSART_DMA.Init.Direction = DMA_MEMORY_TO_PERIPH;
    USART_Handle_t.hUSART_DMA.Init.PeriphInc = DMA_PINC_DISABLE;
    USART_Handle_t.hUSART_DMA.Init.MemInc = DMA_MINC_ENABLE;
    USART_Handle_t.hUSART_DMA.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    USART_Handle_t.hUSART_DMA.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    USART_Handle_t.hUSART_DMA.Init.Mode = DMA_NORMAL;
    USART_Handle_t.hUSART_DMA.Init.Priority = DMA_PRIORITY_MEDIUM;
    if (HAL_DMA_Init(&USART_Handle_t.hUSART_DMA) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmatx,USART_Handle_t.hUSART_DMA);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, USART1_TX_Pin|USART1_RX_Pin);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmatx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
} 

int fputc (int ch, FILE *f)
{
    /* 发送一个字节数据到串口RS232_USART */
    HAL_UART_Transmit (&USART_Handle_t.hUART, (uint8_t *)&ch, 1, 1000);
    return (ch);
}

int fgetc (FILE *f)
{
    int ch;
    HAL_UART_Receive (&USART_Handle_t.hUART, (uint8_t *)&ch, 1, 1000);
    return (ch);
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
