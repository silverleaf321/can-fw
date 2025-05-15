/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ledUtil.h"
#include "display.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi5;

TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim13;
TIM_HandleTypeDef htim17;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_TIM7_Init(void);
static void MX_SPI5_Init(void);
static void MX_TIM17_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM13_Init(void);
/* USER CODE BEGIN PFP */

// inter-core buffers
struct shared_data
{
	uint16_t wtemp;
	uint16_t mix;
	uint16_t gear;
	uint16_t voltage;
	uint16_t uptime;
	uint16_t rpm;
	uint16_t fuelUsed;
	uint16_t fuelPressure;
	uint16_t shiftingPressure;
  uint16_t wheelspeedFL;
	uint16_t wheelspeedFR;
	uint16_t wheelspeedRL;
	uint16_t wheelspeedRR;
	uint16_t shiftingLowPressure;
	uint8_t shutdown;
	uint8_t sdInitialized;
	uint8_t sdPresent;
	uint8_t loggingStatus;
	uint8_t logNumber;
	uint8_t flag;
	uint8_t ECUMapState;
};

// pointer to shared_data struct (inter-core buffers and status)
volatile struct shared_data * const xfr_ptr = (struct shared_data *)0x38001000;

uint16_t wtemp;
uint16_t mix;
uint16_t gear;
uint16_t voltage;
uint16_t uptime;
uint16_t rpm;
uint16_t fuelUsed;
uint16_t fuelPressure;
uint16_t shiftingPressure;
	uint16_t wheelspeedFL;
	uint16_t wheelspeedFR;
	uint16_t wheelspeedRL;
	uint16_t wheelspeedRR;
uint16_t shiftingLowPressure;
uint8_t shutdown;
uint8_t sdInitialized;
uint8_t sdPresent;
uint8_t curScreen;
uint8_t debounce;
uint8_t loggingStatus;
uint8_t logNumber;
uint8_t ECUMapState;
uint8_t btnCheckNum;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	wtemp = 0;
	mix = 0;
	gear = 0;
	voltage = 0;
	uptime = 0;
	rpm = 0;
	fuelUsed = 0;
	fuelPressure = 0;
	shiftingPressure = 0;
  wheelspeedFL = 0;
	wheelspeedFR = 0;
	wheelspeedRL = 0;
	wheelspeedRR = 0;
	shiftingLowPressure = 0;
	shutdown = 0;
	sdInitialized = 0;
	sdPresent = 0;
	curScreen = 99;
	debounce = 0;
	loggingStatus = 0;
	logNumber = 0;
	ECUMapState = 0;
	btnCheckNum = 0;
  /* USER CODE END 1 */

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();
  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));
  /*
  Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
  perform system initialization (system clock config, external memory configuration.. )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);
  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM7_Init();
  MX_SPI5_Init();
  MX_TIM17_Init();
  MX_TIM5_Init();
  MX_TIM13_Init();
  /* USER CODE BEGIN 2 */

  // SPI CS for display
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);

  // Timer for microsecond delay
  HAL_TIM_Base_Start(&htim7);

  // Counts in Milliseconds
  HAL_TIM_Base_Start(&htim17);

  // Initialize the display
  initFT81x();

  // Display the welcome screen
  displayScreen();

  // Display cool LEDs
  startupLED();

  // Sets screen to AutoX
  curScreen = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	// Grab the current information and save it so other files can use
	wtemp = xfr_ptr->wtemp;
	mix = xfr_ptr->mix;
	gear = xfr_ptr->gear;
	voltage = xfr_ptr->voltage;
	uptime = xfr_ptr->uptime;
	rpm = xfr_ptr->rpm;
	fuelUsed = xfr_ptr->fuelUsed;
	fuelPressure = xfr_ptr->fuelPressure;
	shiftingPressure = xfr_ptr->shiftingPressure;
  wheelspeedFL = xfr_ptr->wheelspeedFL;
	wheelspeedFR = xfr_ptr->wheelspeedFR;
	wheelspeedRL = xfr_ptr->wheelspeedRL;
	wheelspeedRR = xfr_ptr->wheelspeedRR;
	shiftingLowPressure = xfr_ptr->shiftingLowPressure;
	shutdown = xfr_ptr->shutdown;
	sdInitialized = xfr_ptr->sdInitialized;
	sdPresent = xfr_ptr->sdPresent;
	loggingStatus = xfr_ptr->loggingStatus;
	logNumber = xfr_ptr->logNumber;
	ECUMapState = xfr_ptr->ECUMapState;

	// Refresh the screen with current info
	displayScreen();

	// Refresh LEDs wih current info
	writeRpm();
  }
  /* USER CODE END 3 */
}

/**
  * @brief SPI5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI5_Init(void)
{

  /* USER CODE BEGIN SPI5_Init 0 */

  /* USER CODE END SPI5_Init 0 */

  /* USER CODE BEGIN SPI5_Init 1 */

  /* USER CODE END SPI5_Init 1 */
  /* SPI5 parameter configuration*/
  hspi5.Instance = SPI5;
  hspi5.Init.Mode = SPI_MODE_MASTER;
  hspi5.Init.Direction = SPI_DIRECTION_2LINES;
  hspi5.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi5.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi5.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi5.Init.NSS = SPI_NSS_SOFT;
  hspi5.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi5.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi5.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi5.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi5.Init.CRCPolynomial = 0x0;
  hspi5.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi5.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi5.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi5.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi5.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi5.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi5.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi5.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi5.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi5.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI5_Init 2 */

  /* USER CODE END SPI5_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 24000-1;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 2500-1;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 240-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 65535;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief TIM13 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM13_Init(void)
{

  /* USER CODE BEGIN TIM13_Init 0 */

  /* USER CODE END TIM13_Init 0 */

  /* USER CODE BEGIN TIM13_Init 1 */

  /* USER CODE END TIM13_Init 1 */
  htim13.Instance = TIM13;
  htim13.Init.Prescaler = 24000-1;
  htim13.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim13.Init.Period = 50-1;
  htim13.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim13.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim13) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM13_Init 2 */

  /* USER CODE END TIM13_Init 2 */

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(void)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = 24000-1;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = 65535;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);

  /*Configure GPIO pin : PF3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : PE7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PD12 PD13 PD14 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PG4 PG5 PG7 PG8 */
  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_4 && debounce == 0) {
      cycleBrightness();
    	debounce = 4;
    	btnCheckNum = 0;
    	__HAL_TIM_SET_COUNTER(&htim13, 0);
    	__HAL_TIM_CLEAR_FLAG(&htim13, TIM_FLAG_UPDATE);
    	HAL_TIM_Base_Start_IT(&htim13);
    } else if (GPIO_Pin == GPIO_PIN_5 && debounce == 0) {
    	debounce = 5;
    	btnCheckNum = 0;
    	__HAL_TIM_SET_COUNTER(&htim13, 0);
    	__HAL_TIM_CLEAR_FLAG(&htim13, TIM_FLAG_UPDATE);
    	HAL_TIM_Base_Start_IT(&htim13);
    } else if (GPIO_Pin == GPIO_PIN_7 && debounce == 0) {
    	debounce = debounce;
      cycleBrightness();
    } else if (GPIO_Pin == GPIO_PIN_8 && debounce == 0) {

    	debounce = debounce;
//    	debounce = 8;
//		btnCheckNum = 0;
//		__HAL_TIM_SET_COUNTER(&htim13, 0);
//		__HAL_TIM_CLEAR_FLAG(&htim13, TIM_FLAG_UPDATE);
//		HAL_TIM_Base_Start_IT(&htim13);
    } else if (GPIO_Pin == GPIO_PIN_12) {
    	debounce = debounce;
    } else if (GPIO_Pin == GPIO_PIN_13) {
    	cycleBrightness();
    	debounce = debounce;
    } else if (GPIO_Pin == GPIO_PIN_14) {
    	debounce = debounce;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim == &htim5) {
		if (debounce == 4 && HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_4) == GPIO_PIN_SET) {
			debounce = 0;
			HAL_TIM_Base_Stop_IT(&htim5);
			return;
		}
		if (debounce == 5 && HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5) == GPIO_PIN_SET) {
			debounce = 0;
			HAL_TIM_Base_Stop_IT(&htim5);
			return;
		}
		if (debounce == 8 && HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_8) == GPIO_PIN_SET) {
			debounce = 0;
			HAL_TIM_Base_Stop_IT(&htim5);
			return;
		}
	}
	if (htim == &htim13) {
		if (btnCheckNum < 20) {
			if (debounce == 4) {
				if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_4) == GPIO_PIN_SET) {
					HAL_TIM_Base_Stop_IT(&htim13);
					debounce = 0;
				} else {
					btnCheckNum += 1;
				}
			} else if (debounce == 5) {
				if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_5) == GPIO_PIN_SET) {
					HAL_TIM_Base_Stop_IT(&htim13);
					debounce = 0;
				} else {
					btnCheckNum += 1;
				}
			} else if (debounce == 8) {
				if (HAL_GPIO_ReadPin(GPIOG, GPIO_PIN_8) == GPIO_PIN_SET) {
					HAL_TIM_Base_Stop_IT(&htim13);
					debounce = 0;
				} else {
					btnCheckNum += 1;
				}
			}
		} else {
			HAL_TIM_Base_Stop_IT(&htim13);
			if (debounce == 4) {
				xfr_ptr->flag = 1;
			} else if (debounce == 5) {
				curScreen = (curScreen + 1) % 5;
			} else if (debounce == 8) {
				xfr_ptr->flag = 2;
			}
			__HAL_TIM_SET_COUNTER(&htim5, 0);
			__HAL_TIM_CLEAR_FLAG(&htim5, TIM_FLAG_UPDATE);
			HAL_TIM_Base_Start_IT(&htim5);
		}

	}
}

// Delays in Microseconds. Blocking
void delayMicro(int microSeconds)
{
	__HAL_TIM_SET_COUNTER(&htim7, 0);
	while (__HAL_TIM_GET_COUNTER(&htim7) < microSeconds);
}

// Delays in Milliseconds. Blocking
void delayMilli(int milliSeconds) {
	HAL_Delay(milliSeconds);
}

// Gets time for screen and LED blinking. Returns current Millisecond
uint16_t getTime()
{
	return __HAL_TIM_GET_COUNTER(&htim17);
}

void SPI_Transmit(uint8_t * pData, uint16_t Size)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi5, pData, Size, 100);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);
}

void SPI_Receive(uint8_t * pData, uint16_t Size)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_Receive(&hspi5, pData, Size, 100);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);
}

void SPI_Transmit_Receive(char * pTxData, char * pRxData, uint16_t Size)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi5, (uint8_t *)pTxData, (uint8_t *)pRxData, Size, 100);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3, GPIO_PIN_SET);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
