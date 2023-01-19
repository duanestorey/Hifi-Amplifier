/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define LED_MUTE_Pin GPIO_PIN_3
#define LED_MUTE_GPIO_Port GPIOC
#define LED_PCM_Pin GPIO_PIN_0
#define LED_PCM_GPIO_Port GPIOA
#define LED_DOLBY_Pin GPIO_PIN_1
#define LED_DOLBY_GPIO_Port GPIOA
#define LED_MUTEA3_Pin GPIO_PIN_3
#define LED_MUTEA3_GPIO_Port GPIOA
#define SETTING_BATTERY_Pin GPIO_PIN_4
#define SETTING_BATTERY_GPIO_Port GPIOC
#define LED_INPUT_1_Pin GPIO_PIN_10
#define LED_INPUT_1_GPIO_Port GPIOE
#define LED_INPUT_2_Pin GPIO_PIN_11
#define LED_INPUT_2_GPIO_Port GPIOE
#define LED_INPUT_3_Pin GPIO_PIN_12
#define LED_INPUT_3_GPIO_Port GPIOE
#define LED_INPUT_4_Pin GPIO_PIN_13
#define LED_INPUT_4_GPIO_Port GPIOE
#define BUTTON_POWER_Pin GPIO_PIN_14
#define BUTTON_POWER_GPIO_Port GPIOE
#define LED_POWER_Pin GPIO_PIN_15
#define LED_POWER_GPIO_Port GPIOE
#define LED_5CH_Pin GPIO_PIN_10
#define LED_5CH_GPIO_Port GPIOB
#define LED_RUN_Pin GPIO_PIN_11
#define LED_RUN_GPIO_Port GPIOB
#define BUTTON_VOLUME_Pin GPIO_PIN_10
#define BUTTON_VOLUME_GPIO_Port GPIOD
#define USB_PULLUP_Pin GPIO_PIN_15
#define USB_PULLUP_GPIO_Port GPIOA
#define DOLBY_IRQ_Pin GPIO_PIN_11
#define DOLBY_IRQ_GPIO_Port GPIOC
#define BUTTON_INPUT_Pin GPIO_PIN_3
#define BUTTON_INPUT_GPIO_Port GPIOD
#define PWM_BACKLIGHT_Pin GPIO_PIN_9
#define PWM_BACKLIGHT_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
