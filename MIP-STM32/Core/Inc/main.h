/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "stm32u0xx_hal.h"

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
#define NRST_Pin GPIO_PIN_2
#define NRST_GPIO_Port GPIOF
#define IMU_INT2_Pin GPIO_PIN_0
#define IMU_INT2_GPIO_Port GPIOA
#define IMU_INT2_EXTI_IRQn EXTI0_1_IRQn
#define IMU_INT1_Float_Pin GPIO_PIN_1
#define IMU_INT1_Float_GPIO_Port GPIOA
#define RTC_INT_Pin GPIO_PIN_2
#define RTC_INT_GPIO_Port GPIOA
#define RTC_INT_EXTI_IRQn EXTI2_3_IRQn
#define Vcom_Sel_Pin GPIO_PIN_1
#define Vcom_Sel_GPIO_Port GPIOB
#define XRST_Pin GPIO_PIN_10
#define XRST_GPIO_Port GPIOB
#define ENBS_Pin GPIO_PIN_11
#define ENBS_GPIO_Port GPIOB
#define SI7_Pin GPIO_PIN_12
#define SI7_GPIO_Port GPIOB
#define SI6_Pin GPIO_PIN_13
#define SI6_GPIO_Port GPIOB
#define SI5_Pin GPIO_PIN_14
#define SI5_GPIO_Port GPIOB
#define SI4_Pin GPIO_PIN_15
#define SI4_GPIO_Port GPIOB
#define DISP_SCLK_Pin GPIO_PIN_8
#define DISP_SCLK_GPIO_Port GPIOA
#define SI3_Pin GPIO_PIN_9
#define SI3_GPIO_Port GPIOA
#define SI2_Pin GPIO_PIN_10
#define SI2_GPIO_Port GPIOA
#define SI1_Pin GPIO_PIN_11
#define SI1_GPIO_Port GPIOA
#define SI0_Pin GPIO_PIN_12
#define SI0_GPIO_Port GPIOA
#define DBG_SWDIO_Pin GPIO_PIN_13
#define DBG_SWDIO_GPIO_Port GPIOA
#define DBG_SWCLK_Pin GPIO_PIN_14
#define DBG_SWCLK_GPIO_Port GPIOA
#define DEN_Pin GPIO_PIN_15
#define DEN_GPIO_Port GPIOA
#define DBG_SWO_Pin GPIO_PIN_3
#define DBG_SWO_GPIO_Port GPIOB
#define ENBG_Pin GPIO_PIN_4
#define ENBG_GPIO_Port GPIOB
#define BOOT0_Pin GPIO_PIN_3
#define BOOT0_GPIO_Port GPIOF

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
