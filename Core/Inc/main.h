/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define LKnobButton_Pin GPIO_PIN_6
#define LKnobButton_GPIO_Port GPIOE
#define LJoystickX_Pin GPIO_PIN_0
#define LJoystickX_GPIO_Port GPIOC
#define LJoystickY_Pin GPIO_PIN_0
#define LJoystickY_GPIO_Port GPIOA
#define LcdReset_Pin GPIO_PIN_1
#define LcdReset_GPIO_Port GPIOA
#define RJoystickButton_Pin GPIO_PIN_2
#define RJoystickButton_GPIO_Port GPIOA
#define RJoystickY_Pin GPIO_PIN_4
#define RJoystickY_GPIO_Port GPIOC
#define RJoystickX_Pin GPIO_PIN_1
#define RJoystickX_GPIO_Port GPIOB
#define RKnobButton_Pin GPIO_PIN_15
#define RKnobButton_GPIO_Port GPIOA
#define LJoystickButton_Pin GPIO_PIN_3
#define LJoystickButton_GPIO_Port GPIOD
#define RKnobA_Pin GPIO_PIN_4
#define RKnobA_GPIO_Port GPIOB
#define RKnobA_EXTI_IRQn EXTI4_IRQn
#define LKnobA_Pin GPIO_PIN_9
#define LKnobA_GPIO_Port GPIOB
#define LKnobA_EXTI_IRQn EXTI9_5_IRQn
#define RKnobB_Pin GPIO_PIN_0
#define RKnobB_GPIO_Port GPIOE
#define LKnobB_Pin GPIO_PIN_1
#define LKnobB_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
