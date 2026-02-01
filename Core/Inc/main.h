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
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
//---
#include "stm32u5g9j_discovery.h"
#include "stm32u5g9j_discovery_conf.h"
#include "stm32u5g9j_discovery_lcd.h"
#include "stm32u5g9j_discovery_ts.h"
//#include "stm32u5g9j_discovery_hspi.h"
#include "stm32_lcd.h"
#include "types_h.h"
#include "stm32u5g9j_discovery_hspi.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern __IO FlagStatus UserButtonPressed;
extern uint8_t DrawBackround;
__IO extern uint16_t possition_x;
__IO extern uint16_t possition_y;
__IO extern int8_t pressed;
extern const unsigned char back[7986UL + 1];

extern uint8_t idx[];
extern uint8_t idy[];

extern int my_itoa(uint8_t * buf, uint32_t data);
extern int my_utoa(uint8_t * buf, uint32_t data);
extern void my_htoa32(uint8_t * buf, uint32_t data);

extern void readPicFromFlash(uint32_t pic_nr);
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
extern int32_t Touchscreen_demo(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_ON_Pin GPIO_PIN_4
#define LCD_ON_GPIO_Port GPIOE
#define USB_DETECT_Pin GPIO_PIN_1
#define USB_DETECT_GPIO_Port GPIOA
#define USB_DETECT_EXTI_IRQn EXTI1_IRQn
#define USB_FAULT_Pin GPIO_PIN_4
#define USB_FAULT_GPIO_Port GPIOA
#define USB_FAULT_EXTI_IRQn EXTI4_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
