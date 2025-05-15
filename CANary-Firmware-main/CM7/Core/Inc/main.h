/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
extern uint16_t rpm;
extern uint16_t fuelUsed;
extern uint16_t wtemp;
extern uint16_t mix;
extern uint16_t gear;
extern uint16_t voltage;
extern uint16_t uptime;
extern uint16_t fuelPressure;
extern uint16_t wheelspeedFL;
extern uint16_t wheelspeedFR;
extern uint16_t wheelspeedRL;
extern uint16_t wheelspeedRR;
extern uint16_t shiftingPressure;
extern uint16_t shiftingLowPressure;
extern uint8_t shutdown;
extern uint8_t sdInitialized;
extern uint8_t sdPresent;
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void delayMicro(int microSeconds);
void delayMilli(int milliSeconds);
//uint32_t getTime();
void SPI_Transmit(uint8_t * pData, uint16_t Size);
void SPI_Receive(uint8_t * pData, uint16_t Size);
void SPI_Transmit_Receive(char * pTxData, char * pRxData, uint16_t Size);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
