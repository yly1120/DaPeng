/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Light_Pin GPIO_PIN_1
#define Light_GPIO_Port GPIOA
#define Soil_Humidity_Pin GPIO_PIN_2
#define Soil_Humidity_GPIO_Port GPIOA
#define Soil_PH_Pin GPIO_PIN_4
#define Soil_PH_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_5
#define KEY1_GPIO_Port GPIOA
#define KEY2_Pin GPIO_PIN_6
#define KEY2_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_7
#define KEY3_GPIO_Port GPIOA
#define KEY4_Pin GPIO_PIN_0
#define KEY4_GPIO_Port GPIOB
#define CO2_TX_Pin GPIO_PIN_10
#define CO2_TX_GPIO_Port GPIOB
#define CO2_RX_Pin GPIO_PIN_11
#define CO2_RX_GPIO_Port GPIOB
#define FAN_Pin GPIO_PIN_12
#define FAN_GPIO_Port GPIOB
#define PUMP_Pin GPIO_PIN_13
#define PUMP_GPIO_Port GPIOB
#define SPRAY_Pin GPIO_PIN_14
#define SPRAY_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_15
#define BEEP_GPIO_Port GPIOB
#define LED_Pin GPIO_PIN_8
#define LED_GPIO_Port GPIOA
#define ESP8266_TX_Pin GPIO_PIN_9
#define ESP8266_TX_GPIO_Port GPIOA
#define ESP8266_RX_Pin GPIO_PIN_10
#define ESP8266_RX_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_7
#define DHT11_GPIO_Port GPIOB
#define OLED_SCL_Pin GPIO_PIN_8
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
typedef enum
{
    CONTROL_MODE_AUTO = 0,
    CONTROL_MODE_MANUAL
} ControlMode_t;


extern volatile ControlMode_t control_mode;


/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
