#ifndef __PINSCONFIG_H__
#define __PINSCONFIG_H__

#include "stm32f1xx_hal.h"

#define Motor_PWM_CH1_Pin GPIO_PIN_0
#define Motor_PWM_CH1_GPIO_Port GPIOA
#define DRV8711_DIR_Pin GPIO_PIN_1
#define DRV8711_DIR_GPIO_Port GPIOA
#define Vin_Voltage_SO_Pin GPIO_PIN_4
#define Vin_Voltage_SO_GPIO_Port GPIOA
#define INA20X_Current_Sample_SO_Pin GPIO_PIN_5
#define INA20X_Current_Sample_SO_GPIO_Port GPIOA
#define Encoder_Phase_A_Pin GPIO_PIN_6
#define Encoder_Phase_A_GPIO_Port GPIOA
#define Encoder_Phase_B_Pin GPIO_PIN_7
#define Encoder_Phase_B_GPIO_Port GPIOA
#define Motor_nSLEEP_Pin GPIO_PIN_0
#define Motor_nSLEEP_GPIO_Port GPIOB
#define Motor_RESET_Pin GPIO_PIN_1
#define Motor_RESET_GPIO_Port GPIOB
#define I2C2_SCL_Pin GPIO_PIN_10
#define I2C2_SCL_GPIO_Port GPIOB
#define I2C2_SDA_Pin GPIO_PIN_11
#define I2C2_SDA_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define SPI2_SCK_Pin GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MISO_Pin GPIO_PIN_14
#define SPI2_MISO_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define INA20X_CMPOUT_Pin GPIO_PIN_8
#define INA20X_CMPOUT_GPIO_Port GPIOA
#define INA20X_CMPOUT_EXTI_IRQn EXTI9_5_IRQn
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define CAN_RX_Pin GPIO_PIN_11
#define CAN_RX_GPIO_Port GPIOA
#define CAN_TX_Pin GPIO_PIN_12
#define CAN_TX_GPIO_Port GPIOA
#define Motor_nFAULT_Pin GPIO_PIN_3
#define Motor_nFAULT_GPIO_Port GPIOB
#define Motor_nSTALL_Pin GPIO_PIN_4
#define Motor_nSTALL_GPIO_Port GPIOB
#define SIGNAL_LED_G_Pin GPIO_PIN_5
#define SIGNAL_LED_G_GPIO_Port GPIOB
#define SIGNAL_LED_R_Pin GPIO_PIN_6
#define SIGNAL_LED_R_GPIO_Port GPIOB
#define SIGNAL_LED_B_Pin GPIO_PIN_7
#define SIGNAL_LED_B_GPIO_Port GPIOB
#define Position_START_Pin GPIO_PIN_8
#define Position_START_GPIO_Port GPIOB
#define Position_END_Pin GPIO_PIN_9
#define Position_END_GPIO_Port GPIOB

#endif
