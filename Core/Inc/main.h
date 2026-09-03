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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "string.h"
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
#define Key1_Pin GPIO_PIN_4
#define Key1_GPIO_Port GPIOA
#define Key2_Pin GPIO_PIN_5
#define Key2_GPIO_Port GPIOA
#define Key3_Pin GPIO_PIN_6
#define Key3_GPIO_Port GPIOA
#define Key4_Pin GPIO_PIN_7
#define Key4_GPIO_Port GPIOA
#define Lazer_Pin GPIO_PIN_12
#define Lazer_GPIO_Port GPIOD
#define Radar_Ctrl_Pin GPIO_PIN_13
#define Radar_Ctrl_GPIO_Port GPIOD
#define OLED_DC_Pin GPIO_PIN_11
#define OLED_DC_GPIO_Port GPIOC
#define OLED_CS_Pin GPIO_PIN_12
#define OLED_CS_GPIO_Port GPIOC
#define LED_G_Pin GPIO_PIN_1
#define LED_G_GPIO_Port GPIOD
#define LED_R_Pin GPIO_PIN_2
#define LED_R_GPIO_Port GPIOD
#define LED_B_Pin GPIO_PIN_3
#define LED_B_GPIO_Port GPIOD
#define Buzzer_Pin GPIO_PIN_9
#define Buzzer_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
