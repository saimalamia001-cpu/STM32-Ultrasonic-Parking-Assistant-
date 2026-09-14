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

#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

void Error_Handler(void);

#define trig_Pin GPIO_PIN_0
#define trig_GPIO_Port GPIOA
#define echo_Pin GPIO_PIN_1
#define echo_GPIO_Port GPIOA
#define buzzer_Pin GPIO_PIN_4
#define buzzer_GPIO_Port GPIOA
#define green_Pin GPIO_PIN_0
#define green_GPIO_Port GPIOB
#define RED_Pin GPIO_PIN_10
#define RED_GPIO_Port GPIOA
#define yellow_Pin GPIO_PIN_6
#define yellow_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
