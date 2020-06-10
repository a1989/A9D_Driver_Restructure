/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
#define READ_GPIO_PIN(GPIO_PORT, PIN) 	HAL_GPIO_ReadPin (GPIO_PORT, PIN)
#define SET_PIN(GPIO_PORT, PIN)		HAL_GPIO_WritePin (GPIO_PORT, PIN, GPIO_PIN_SET)
#define RESET_PIN(GPIO_PORT, PIN)		HAL_GPIO_WritePin (GPIO_PORT, PIN, GPIO_PIN_RESET)

void MX_GPIO_Init(void);
void GPIO_InitInputNoPull(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin);
void GPIO_InitPullDown(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin);
void GPIO_InitNoPull(GPIO_TypeDef *GPIO_Port, uint16_t GPIO_Pin);
/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
