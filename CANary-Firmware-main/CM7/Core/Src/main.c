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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>
#include <string.h>
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
#define RPMLOGGING 0
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

FDCAN_HandleTypeDef hfdcan1;
FDCAN_HandleTypeDef hfdcan2;

SD_HandleTypeDef hsd1;

SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi4;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim14;
TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */

FRESULT res;
FILINFO fno;
char fileName[9] = "0000.TXT\0";

uint32_t byteswritten;
uint8_t rtext[_MAX_SS];

FDCAN_TxHeaderTypeDef TxHeader;
FDCAN_RxHeaderTypeDef RxHeader1;
FDCAN_RxHeaderTypeDef RxHeader2;
uint8_t CANTxData[8];
uint8_t RxData1[8];
uint8_t RxData99[26];
uint8_t RxData2[8];
uint32_t TxMailbox;
uint32_t sdDataByteCount;
uint32_t sdDataWriteCount;
uint8_t sdData[512*30];
uint32_t curtime;
uint32_t nextStartTime;
uint8_t radFanStatus;
uint8_t mapStatus;
uint8_t hiya;
uint8_t hiya2;
uint8_t debounce;
uint8_t engineStartNumber;
//play around with the value inside this buffer
uint8_t bufferData[128];
uint32_t bufferDataByteCount; //in main, initialize this as equal to 0
uint8_t frame[] = {
    0x7E, //Start Delimiter
    0x00, 0x13, //Length of Frame
    0x10, //Frame Type
    0x01, //Frame ID
    0x00, 0x13, 0xA2, 0x00, 0x42, 0x09, 0xC9, 0x44, //Destination Address
    0xFF, 0xFE, //16-bit Address
    0x00, 0x00, //broadcast radius, options
};

// Inter-core data
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
	uint16_t shiftingLowPressure;
  uint16_t wheelspeedFL;
	uint16_t wheelspeedFR;
	uint16_t wheelspeedRL;
	uint16_t wheelspeedRR;
	uint8_t shutdown;
	uint8_t sdInitialized;
	uint8_t sdPresent;
	uint8_t loggingStatus;
	uint8_t logNumber;
	uint8_t flag;
	uint8_t ECUMapState;
	uint8_t screenState;
};

// Pointer to shared_data struct
volatile struct shared_data * const xfr_ptr = (struct shared_data *)0x38001000;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_SPI4_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM2_Init(void);
static void MX_FDCAN2_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM14_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

void sdWrite();
void sdTestRead();
void adcInit();
void adcRead();
void sdDetect();
void setCurrentFile();
void incrementCurrentFile();
void incrementFileNumber();
void toggleRadfan();
void toggleMap();
void dataFlag();
static void xbeeTransmit(void);
static void frameCreator(uint8_t *newFrame);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// CAN1 Bus callback. Turns off CAN and ADC interrupts for the duration
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {

  if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {

    /* Retreive Rx messages from RX FIFO0 */
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader1, RxData1) != HAL_OK) {

    	/* Reception Error */
    	Error_Handler();

    }

    //Deactivated Interrupts
    HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
    HAL_FDCAN_DeactivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
    HAL_TIM_Base_Stop_IT(&htim16);

    //Check if SD write buffer will be filled out --> if so write to SD before continuing
    if (sdDataByteCount + 13 >= 15360) {

    	sdWrite();

    }

	//sorts out compound messages of LTC
	if (!(RxHeader1.Identifier == 0x460 && RxData1[0] != 0)) {

		curtime = __HAL_TIM_GET_COUNTER(&htim2) / 10;
		sdData[sdDataByteCount] = (RxHeader1.Identifier >> 8) & 0xFF;
		sdData[sdDataByteCount+1] = RxHeader1.Identifier & 0xFF;
		sdData[sdDataByteCount+2] = RxData1[0];
		sdData[sdDataByteCount+3] = RxData1[1];
		sdData[sdDataByteCount+4] = RxData1[2];
		sdData[sdDataByteCount+5] = RxData1[3];
		sdData[sdDataByteCount+6] = RxData1[4];
		sdData[sdDataByteCount+7] = RxData1[5];
		sdData[sdDataByteCount+8] = RxData1[6];
		sdData[sdDataByteCount+9] = RxData1[7];
		sdData[sdDataByteCount+10] = (curtime >> 16) & 0xFF;
		sdData[sdDataByteCount+11] = (curtime >> 8) & 0xFF;
		sdData[sdDataByteCount+12] = curtime & 0xFF;
		sdDataByteCount = sdDataByteCount + 13;

	}

    //Turn ADC timer back on
	HAL_TIM_Base_Start_IT(&htim16);

	//Save updated information for screen purposes
    if (RxHeader1.Identifier == 0x649) {

    	xfr_ptr->wtemp = RxData1[0];
    	xfr_ptr->voltage = RxData1[5];
    	xfr_ptr->fuelUsed = (RxData1[6] << 8) + RxData1[7];

    } else if (RxHeader1.Identifier == 0x64C) {

    	xfr_ptr->uptime = (RxData1[2] << 8) + RxData1[3];

    } else if (RxHeader1.Identifier == 0x640) {

    	xfr_ptr->rpm = (RxData1[0] << 8) + RxData1[1];

    } else if (RxHeader1.Identifier == 0x641) {

    	xfr_ptr->fuelPressure = (RxData1[4] << 8) + RxData1[5];

    } else if (RxHeader1.Identifier == 0x460) {

    	xfr_ptr->mix = RxData1[0] == 0 ? (RxData1[1] << 8) + RxData1[2] : xfr_ptr->mix;

    } else if (RxHeader1.Identifier == 0x64D) {

    	xfr_ptr->gear = RxData1[6] & 0b1111;

    } else if (RxHeader1.Identifier == 0x404) {

    	xfr_ptr->shutdown = RxData1[3];

    } else if (RxHeader1.Identifier == 0x410) {

    	xfr_ptr->ECUMapState = RxData1[0];

    } else if (RxHeader1.Identifier == 0x648) {
      xfr_ptr->wheelspeedFL = (RxData1[0] << 8)  + RxData1[1];
    	xfr_ptr->wheelspeedFR = (RxData1[2] << 8) + RxData1[3];
    	xfr_ptr->wheelspeedRL = (RxData1[4] << 8) + RxData1[5];
    	xfr_ptr->wheelspeedRR = (RxData1[6] << 8) + RxData1[7];
    }

    //Turn CAN notifications back on
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  }
}

// CAN2 Bus callback. Turns off CAN and ADC interrupts for the duration
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs) {

	if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET) {

	/* Retreive Rx messages from RX FIFO0 */
    if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader2, RxData2) != HAL_OK) {

    	/* Reception Error */
    	Error_Handler();
    }

    //Deactivated Interrupts
    HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
    HAL_FDCAN_DeactivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
    HAL_TIM_Base_Stop_IT(&htim16);

    //Check if SD write buffer will be filled out --> if so write to SD before continuing
    if (sdDataByteCount + 13 >= 15360) {

    	sdWrite();

    }


    // Temp check for tire temps TODO
    if (RxHeader2.Identifier == 0x4C2) {
    	sdDataByteCount = sdDataByteCount;
    }
    else if (RxHeader2.Identifier == 0xA220) {
    	sdDataByteCount = sdDataByteCount;
    } else if (RxHeader2.Identifier == 0xA320) {
    	sdDataByteCount = sdDataByteCount;
    } else if (RxHeader2.Identifier == 0xA420) {
    	sdDataByteCount = sdDataByteCount;
    }
    else if (RxHeader2.Identifier == 0xA120) {
        	sdDataByteCount = sdDataByteCount;
    }

	curtime = __HAL_TIM_GET_COUNTER(&htim2) / 10;
	sdData[sdDataByteCount] = (RxHeader2.Identifier >> 8) & 0xFF;
	sdData[sdDataByteCount+1] = RxHeader2.Identifier & 0xFF;
	sdData[sdDataByteCount+2] = RxData2[0];
	sdData[sdDataByteCount+3] = RxData2[1];
	sdData[sdDataByteCount+4] = RxData2[2];
	sdData[sdDataByteCount+5] = RxData2[3];
	sdData[sdDataByteCount+6] = RxData2[4];
	sdData[sdDataByteCount+7] = RxData2[5];
	sdData[sdDataByteCount+8] = RxData2[6];
	sdData[sdDataByteCount+9] = RxData2[7];
	sdData[sdDataByteCount+10] = (curtime >> 16) & 0xFF;
	sdData[sdDataByteCount+11] = (curtime >> 8) & 0xFF;
	sdData[sdDataByteCount+12] = curtime & 0xFF;
	sdDataByteCount = sdDataByteCount + 13;

    //Turn ADC timer back on, turn CAN notifications back on
    HAL_TIM_Base_Start_IT(&htim16);
    HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
    HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	sdDataByteCount = 0;
	xfr_ptr->sdInitialized = 0;
	xfr_ptr->sdPresent = 0;
	xfr_ptr->loggingStatus = 0;
	xfr_ptr->wtemp = 0;
	xfr_ptr->mix = 0;
	xfr_ptr->gear = 0;
	xfr_ptr->voltage = 0;
	xfr_ptr->uptime = 0;
	xfr_ptr->rpm = 0;
	xfr_ptr->fuelUsed = 0;
	xfr_ptr->fuelPressure = 0;
	xfr_ptr->shiftingPressure = 0;
	xfr_ptr->shiftingLowPressure = 0;
  xfr_ptr->wheelspeedFL = 0;
	xfr_ptr->wheelspeedFR = 0;
	xfr_ptr->wheelspeedRL = 0;
	xfr_ptr->wheelspeedRR = 0;
	xfr_ptr->shutdown = 0;
	xfr_ptr->logNumber = 0;
	xfr_ptr->flag = 0;
	hiya = 0;
	hiya2 = 0;
	debounce = 0;
	engineStartNumber = 0;
	nextStartTime = 0;
	radFanStatus = 0;
  /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */

/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SDMMC1_SD_Init();
  MX_SPI4_Init();
  MX_FATFS_Init();
  MX_TIM6_Init();
  MX_TIM2_Init();
  MX_FDCAN2_Init();
  MX_FDCAN1_Init();
  MX_TIM16_Init();
  MX_TIM14_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  // Timer for log timestamp (in 0.1ms)
  HAL_TIM_Base_Start(&htim2);

  // Timer for blocking microsecond delays
  HAL_TIM_Base_Start(&htim6);

  // ADC SPI CS
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

  // Initialize the first file to be written to
  setCurrentFile();

  // Sets the log number to be displayed on screen
  char fileNum[5];
  fileNum[0] = fileName[0];
  fileNum[1] = fileName[1];
  fileNum[2] = fileName[2];
  fileNum[3] = fileName[3];
  fileNum[4] = '\0';
  xfr_ptr->logNumber = atoi(fileNum);

  adcInit();

  // Timer for ADC read interrupts
  HAL_TIM_Base_Start_IT(&htim16);

  // FDCAN responsible for CAN1 Bus
  HAL_FDCAN_Start(&hfdcan2);
  if (HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
  {
	  Error_Handler();
  }

  // FDCAN responsible for CAN2 Bus
  HAL_FDCAN_Start(&hfdcan1);
  if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK)
  {
	  Error_Handler();
  }

  //configure Tx message for CAN
  CANTxData[0] = 0;
  CANTxData[1] = 0;
  CANTxData[2] = 0;
  CANTxData[3] = 0;
  CANTxData[4] = 0;
  CANTxData[5] = 0;
  CANTxData[6] = 0;
  CANTxData[7] = 0;

  radFanStatus = 0;
  mapStatus = 0;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  // Checks to see if rad fan should be toggled and will do that
//	  if (xfr_ptr->wtemp > 110 && radFanStatus == 0) {
//		  toggleRadfan();
//	  } else if (xfr_ptr->wtemp < 108 && radFanStatus == 1) {
//		  toggleRadfan();
//	  }

	  if (!xfr_ptr->sdInitialized) {
		  MX_SDMMC1_SD_Init();
	  } else {
		  xfr_ptr->sdPresent = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) ? false : true;
	  }

	  // Handles check to see if SD card needs to be reinitialized. Opens file in append mode
//	  if (!xfr_ptr->sdInitialized && xfr_ptr->sdPresent == true) {
//		  hiya = FATFS_UnLinkDriver(SDPath);
//		  MX_FATFS_Init();
//		  MX_SDMMC1_SD_Init();
//		  if(f_mount(&SDFatFS, "", 0) == FR_OK) {
//			f_open(&SDFile, fileName, FA_OPEN_APPEND | FA_WRITE);
//		  }
//	  } else {
//		  xfr_ptr->sdPresent = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) ? false : true;
//	  }
//
//	  if (xfr_ptr->sdPresent == false) {
//		  xfr_ptr->sdInitialized = false;
//	  }

	  // Temp way to communicate button interrupts between cores
	  if (xfr_ptr->flag == 1) {
		  toggleMap();
		  xfr_ptr->flag = 0;
	  } else if (xfr_ptr->flag == 2) {
		  dataFlag();
		  xfr_ptr->flag = 0;
	  }

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 120;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = DISABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 1;
  hfdcan1.Init.NominalSyncJumpWidth = 2;
  hfdcan1.Init.NominalTimeSeg1 = 117;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 8;
  hfdcan1.Init.DataSyncJumpWidth = 7;
  hfdcan1.Init.DataTimeSeg1 = 7;
  hfdcan1.Init.DataTimeSeg2 = 7;
  hfdcan1.Init.MessageRAMOffset = 0;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 1;
  hfdcan1.Init.RxFifo0ElmtsNbr = 0;
  hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxFifo1ElmtsNbr = 1;
  hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.RxBuffersNbr = 0;
  hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan1.Init.TxEventsNbr = 0;
  hfdcan1.Init.TxBuffersNbr = 0;
  hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */
  	FDCAN_FilterTypeDef sFilterConfig;

	sFilterConfig.IdType = FDCAN_EXTENDED_ID;
	// sFilterConfig.IdType = FDCAN_STANDARD_ID;
	sFilterConfig.FilterIndex = 0;
	//change to allow all, use to test both CAN busses
	sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
	sFilterConfig.FilterID1 = 0x400;
	// sFilterConfig.FilterID2 = 0x640;
	sFilterConfig.FilterID2 = 0xA421;
	sFilterConfig.RxBufferIndex = 0;
	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
	Error_Handler();
	}

	HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief FDCAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN2_Init(void)
{

  /* USER CODE BEGIN FDCAN2_Init 0 */

  /* USER CODE END FDCAN2_Init 0 */

  /* USER CODE BEGIN FDCAN2_Init 1 */

  /* USER CODE END FDCAN2_Init 1 */
  hfdcan2.Instance = FDCAN2;
  hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
  hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan2.Init.AutoRetransmission = DISABLE;
  hfdcan2.Init.TransmitPause = DISABLE;
  hfdcan2.Init.ProtocolException = DISABLE;
  hfdcan2.Init.NominalPrescaler = 1;
  hfdcan2.Init.NominalSyncJumpWidth = 2;
  hfdcan2.Init.NominalTimeSeg1 = 117;
  hfdcan2.Init.NominalTimeSeg2 = 2;
  hfdcan2.Init.DataPrescaler = 8;
  hfdcan2.Init.DataSyncJumpWidth = 7;
  hfdcan2.Init.DataTimeSeg1 = 7;
  hfdcan2.Init.DataTimeSeg2 = 7;
  hfdcan2.Init.MessageRAMOffset = 40;
  hfdcan2.Init.StdFiltersNbr = 1;
  hfdcan2.Init.ExtFiltersNbr = 0;
  hfdcan2.Init.RxFifo0ElmtsNbr = 1;
  hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.RxFifo1ElmtsNbr = 0;
  hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.RxBuffersNbr = 0;
  hfdcan2.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
  hfdcan2.Init.TxEventsNbr = 0;
  hfdcan2.Init.TxBuffersNbr = 0;
  hfdcan2.Init.TxFifoQueueElmtsNbr = 1;
  hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  hfdcan2.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
  if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN2_Init 2 */
  FDCAN_FilterTypeDef sFilterConfig;

  sFilterConfig.IdType = FDCAN_STANDARD_ID;
  sFilterConfig.FilterIndex = 0;
  //change to allow all, use to test both CAN busses
  sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
  sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
  sFilterConfig.FilterID1 = 0x400;
  sFilterConfig.FilterID2 = 0x64D;
  sFilterConfig.RxBufferIndex = 0;
  if (HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig) != HAL_OK) {
	  Error_Handler();
  }

  HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);

  /* USER CODE END FDCAN2_Init 2 */

}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */
  if (xfr_ptr->sdInitialized)
  {
	  xfr_ptr->sdPresent = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1) ? false : true;
	  return;
  }

  if (HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_1))
  {
	  xfr_ptr->sdInitialized = false;
	  xfr_ptr->sdPresent = false;
	  return;
  }

  xfr_ptr->sdInitialized = true;
  xfr_ptr->sdPresent = true;
  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_4B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 0;
  if (HAL_SD_Init(&hsd1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief SPI4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI4_Init(void)
{

  /* USER CODE BEGIN SPI4_Init 0 */

  /* USER CODE END SPI4_Init 0 */

  /* USER CODE BEGIN SPI4_Init 1 */

  /* USER CODE END SPI4_Init 1 */
  /* SPI4 parameter configuration*/
  hspi4.Instance = SPI4;
  hspi4.Init.Mode = SPI_MODE_MASTER;
  hspi4.Init.Direction = SPI_DIRECTION_2LINES;
  hspi4.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi4.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi4.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi4.Init.NSS = SPI_NSS_SOFT;
  hspi4.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi4.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi4.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi4.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi4.Init.CRCPolynomial = 0x0;
  hspi4.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi4.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi4.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi4.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi4.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi4.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi4.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi4.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi4.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI4_Init 2 */

  /* USER CODE END SPI4_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 24000-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 240-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 24000-1;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 50000-1;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(void)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = 240-1;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = 1000-1;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PE15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PD1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 15, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == GPIO_PIN_7 && debounce == 0) {
		cycleBrightness();
    	//debounce = 1;
    	//HAL_TIM_Base_Start_IT(&htim5);
    } else if (GPIO_Pin == GPIO_PIN_8 && debounce == 0) {
    	//debounce = 1;
    	//HAL_TIM_Base_Start_IT(&htim5);
    } else if (GPIO_Pin == GPIO_PIN_12) {
    	hiya = 12;
    } else if (GPIO_Pin == GPIO_PIN_13) {
    	cycleBrightness();
    	hiya = 13;
    } else if (GPIO_Pin == GPIO_PIN_14) {
    	hiya = 14;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {

	// ADC read timer
	if (htim == &htim16) {
		adcRead();
		return;
	}

	// Timer for 5 sec after engine off
	if (htim == &htim14) {

		// Engine has restarted so we ignore the 5sec timeout
		if (xfr_ptr->rpm >= 1000) {
			// Stop the timer interrupts
			HAL_TIM_Base_Stop_IT(&htim14);
			// Set logging to on
			xfr_ptr->loggingStatus = 1;
			return;
		}
		// Set logging to off
		xfr_ptr->loggingStatus = 0;

		// Increment engine start number
		engineStartNumber += 1;

		// Set time of next engine start
		nextStartTime = __HAL_TIM_GET_COUNTER(&htim2) + 150000;

		// Stop the timer interrupts
		HAL_TIM_Base_Stop_IT(&htim14);
		return;
	}
}

// Delays the inputed number of Microseconds
void delayMicro(int microSeconds)
{
	__HAL_TIM_SET_COUNTER(&htim6, 0);
	while (__HAL_TIM_GET_COUNTER(&htim6) < microSeconds);
}

// Delays the inputed number of Milliseconds
void delayMilli(int milliSeconds) {
	HAL_Delay(milliSeconds);
}

// Flushes data buffer to SD
// TODO remove rpm based logging
// TODO use DMA
void sdWrite() {

	// Ensures SD Card is present. Resets write buffer if not
	if (!(xfr_ptr->sdPresent && xfr_ptr->sdInitialized)) {
		sdDataByteCount = 0;
		return;
	}

	if (RPMLOGGING == 0) {

		// No RPM based logging so we always log data
		xfr_ptr->loggingStatus = 1;

	} else if (xfr_ptr->rpm >= 1000) {

		// Stops timer because log will still run
		if (xfr_ptr->loggingStatus == 2) {
			HAL_TIM_Base_Stop_IT(&htim14);
		}

		// RPM above cutoff so we log current buffer
		xfr_ptr->loggingStatus = 1;


	} else if (xfr_ptr->loggingStatus == 1) {

		// RPM below cutoff but was previously above cutoff. We change status to waiting and set timer for 5 seconds
		xfr_ptr->loggingStatus = 2;
		__HAL_TIM_SET_COUNTER(&htim14, 0);
		__HAL_TIM_CLEAR_FLAG(&htim14, TIM_FLAG_UPDATE);
		HAL_TIM_Base_Start_IT(&htim14);

	} else if (xfr_ptr->loggingStatus == 0) {

		// Not logging so reset buffer
		sdDataByteCount = 0;
		if (engineStartNumber != 0) {

			// Between engine starts, so time resets to last engine stop plus 15 sec
			__HAL_TIM_SET_COUNTER(&htim2, nextStartTime);

		} else {

			// Before first engine start, so time resets to 0
			__HAL_TIM_SET_COUNTER(&htim2, 0);

		}
		return;
	}

	// Ensures the buffer written is on a 32byte boundary
	uint8_t overflowAmount = sdDataByteCount % 32;
	sdDataWriteCount = sdDataByteCount - overflowAmount;

	// Write to SD
	res = f_write(&SDFile, sdData, sdDataWriteCount, (void *)&byteswritten);
	if (res != FR_OK) {
		sdDataByteCount = 0;
		return;
	}

	//TODO remove when 5V drop detection active
	//TODO check if sd card is full and display warning

	// Sync to ensure data is saved
	// TODO should be on a timer not every time
	res = f_sync(&SDFile);
	if (res != FR_OK) {
		sdDataByteCount = 0;
		return;
	}

	// Sets beginning of next buffer to hold contents of the overflowed data
	uint8_t i;
	for (i = 0; i < overflowAmount; i++) {
		sdData[i] = sdData[sdDataWriteCount+i];
	}
	sdDataByteCount = overflowAmount;
}

// Sets the current file on startup
void setCurrentFile() {

	// Ensures that SD card is present and initialized
	if (!(xfr_ptr->sdPresent && xfr_ptr->sdInitialized)) {
		return;
	}

	// Mount file system for startup
	if(f_mount(&SDFatFS, "", 0) != FR_OK) {
		return;
	} else {
		// Find first available file number
		// TODO use a config file to save this instead?
		while (f_stat(fileName, &fno) != FR_NO_FILE) {
			incrementFileNumber();
		}
	}

	// Open file
	if(f_open(&SDFile, fileName, FA_OPEN_ALWAYS | FA_WRITE) != FR_OK) {
		return;
	}
}

// Increments fileName to next number
void incrementFileNumber() {
	if (fileName[3] == 57) {
		fileName[3] = 48;
		if (fileName[2] == 57) {
			fileName[2] = 48;
			if (fileName[1] == 57) {
				fileName[1] = 48;
				fileName[0]++;
			} else {
				fileName[1]++;
			}
		} else {
			  fileName[2]++;
		}
	} else {
		fileName[3]++;
	}
}

// Initializes ADC to manual mode
// TODO change to averaging mode
void adcInit() {

	//calibrate 0x1, 0b00000010
	//config auto channel picking 0x12, 0b11111111
	//seq mode 0x10, 0b00000001
	//stats en
	//sample rate
	//seq start 0x10, 0b00010001

	char tx_buf[20];
	char rx_buf[20];

	tx_buf[0] = 0x8; // Write CMD
	tx_buf[1] = 0x1; // Address
	tx_buf[2] = 0b00000010; // Data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	// Append 4-bit channel ID
	tx_buf[0] = 0x8; // Write CMD
	tx_buf[1] = 0x2; // Address
	tx_buf[2] = 0b00010000; // Data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	tx_buf[0] = 0x8; // Write CMD
	tx_buf[1] = 0x11; // Address
	tx_buf[2] = 0b00000000; // Data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

}

// Reads ADC inputs 0-7, Turns off CAN and ADC interrupts for the duration
void adcRead() {
	HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
	HAL_FDCAN_DeactivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
	HAL_TIM_Base_Stop_IT(&htim16);

	char tx_buf[3];
	char rx_buf[3];
	uint16_t curdata;

	// Check if buffer needs to be cleared
	if (sdDataByteCount + 26 >= 15360)
	{
		sdWrite();
	}

	// ID of first message 0x01
	sdData[sdDataByteCount] = 0x00;
	sdData[sdDataByteCount+1] = 0x01;

	// Setting output of ADC to be channel 2 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x02; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);

	// Data for analog input 0
	sdData[sdDataByteCount+2] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+3] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Setting output of ADC to be channel 3 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x03; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);

	// Data for analog input 1
	sdData[sdDataByteCount+4] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+5] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Setting output of ADC to be channel 4 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x04; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);

	// Data for analog input 2
	sdData[sdDataByteCount+6] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+7] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Setting output of ADC to be channel 5 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x05; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);

	xfr_ptr->shiftingPressure = (rx_buf[0] << 4) + (rx_buf[1]>>4);

	// Data for analog input 3
	sdData[sdDataByteCount+8] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+9] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Time stamp
	curtime = __HAL_TIM_GET_COUNTER(&htim2) / 10;
	sdData[sdDataByteCount+10] = (curtime >> 16) & 0xFF;
	sdData[sdDataByteCount+11] = (curtime >> 8) & 0xFF;
	sdData[sdDataByteCount+12] = curtime & 0xFF;

	// Increment buffer byte count
	sdDataByteCount = sdDataByteCount + 13;

	// ID of second message 0x02
	sdData[sdDataByteCount] = 0x00;
	sdData[sdDataByteCount+1] = 0x02;

	// Setting output of ADC to be channel 6 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x06; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);


	// Data for analog input 4
	sdData[sdDataByteCount+2] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+3] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Setting output of ADC to be channel 7 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x07; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);


	// Data for analog input 5
	sdData[sdDataByteCount+4] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+5] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Setting output of ADC to be channel 0 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x00; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);

	xfr_ptr->shiftingLowPressure = (rx_buf[0] << 4) + (rx_buf[1]>>4);

//	xfr_ptr->shiftingLowPressure = 2.919;


	// Data for analog input 6
	sdData[sdDataByteCount+6] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+7] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Setting output of ADC to be channel 1 in 2 read cycles
	tx_buf[0] = 0x08; //write cmd
	tx_buf[1] = 0x11; //address MANUAL_CHID
	tx_buf[2] = 0x01; //data

	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_RESET);
	HAL_SPI_TransmitReceive(&hspi4, (uint8_t *)tx_buf, (uint8_t *)rx_buf, 3, 100);
	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_15, GPIO_PIN_SET);

	curdata = rx_buf[0];
	curdata = curdata << 4;
	curdata = curdata + (rx_buf[1] >> 4);


	// Data for analog input 7
	sdData[sdDataByteCount+8] = (curdata>>8) & 0xFF;
	sdData[sdDataByteCount+9] = curdata & 0xFF;

	// Delay for ADC timing purposes
	delayMicro(1);

	// Time stamp
	curtime = __HAL_TIM_GET_COUNTER(&htim2) / 10;
	sdData[sdDataByteCount+10] = (curtime >> 16) & 0xFF;
	sdData[sdDataByteCount+11] = (curtime >> 8) & 0xFF;
	sdData[sdDataByteCount+12] = curtime & 0xFF;

	// Increment buffer byte count
	sdDataByteCount = sdDataByteCount + 13;

	// Re-enable Interrupts
	HAL_TIM_Base_Start_IT(&htim16);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
	HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

}

// Requests to change ECU Map
// TODO not fully functional
void toggleMap() {
	// Configure TX Header
	TxHeader.Identifier = 0x409;
	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	//configure Tx message for CAN

	mapStatus = mapStatus ^ 1;

	CANTxData[0] = mapStatus;
	CANTxData[1] = 0;
	CANTxData[2] = 0;
	CANTxData[3] = 0;
	CANTxData[4] = 0;
	CANTxData[5] = 0;
	CANTxData[6] = 0;
	CANTxData[7] = 0;

	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, CANTxData)!= HAL_OK) {
	  Error_Handler();
	}
}

// Toggles Radiator fan
void toggleRadfan() {
	// Configure TX Header
	TxHeader.Identifier = 0x407;
	TxHeader.IdType = FDCAN_STANDARD_ID;
	TxHeader.TxFrameType = FDCAN_DATA_FRAME;
	TxHeader.DataLength = FDCAN_DLC_BYTES_8;
	TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
	TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
	TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	TxHeader.MessageMarker = 0;

	//configure Tx message for CAN

	radFanStatus = radFanStatus ^ 1;

	CANTxData[0] = radFanStatus;
	CANTxData[1] = 0;
	CANTxData[2] = 0;
	CANTxData[3] = 0;
	CANTxData[4] = 0;
	CANTxData[5] = 0;
	CANTxData[6] = 0;
	CANTxData[7] = 0;

	if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader, CANTxData)!= HAL_OK) {
	  Error_Handler();
	}
}

// Writes a flag to SD
void dataFlag() {
	//Deactivated Interrupts
	HAL_FDCAN_DeactivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE);
	HAL_FDCAN_DeactivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE);
	HAL_TIM_Base_Stop_IT(&htim16);

	//Check if SD write buffer will be filled out --> if so write to SD before continuing
	if (sdDataByteCount + 13 >= 15360) {

		sdWrite();

	}

	curtime = __HAL_TIM_GET_COUNTER(&htim2) / 10;
	sdData[sdDataByteCount] = 0x00;
	sdData[sdDataByteCount+1] = 0x10;
	sdData[sdDataByteCount+2] = 1;
	sdData[sdDataByteCount+3] = 0;
	sdData[sdDataByteCount+4] = 0;
	sdData[sdDataByteCount+5] = 0;
	sdData[sdDataByteCount+6] = 0;
	sdData[sdDataByteCount+7] = 0;
	sdData[sdDataByteCount+8] = 0;
	sdData[sdDataByteCount+9] = 0;
	sdData[sdDataByteCount+10] = (curtime >> 16) & 0xFF;
	sdData[sdDataByteCount+11] = (curtime >> 8) & 0xFF;
	sdData[sdDataByteCount+12] = curtime & 0xFF;
	sdDataByteCount = sdDataByteCount + 13;

	HAL_TIM_Base_Start_IT(&htim16);
	HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0);
	HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);

}
static void xbeeTransmit() {
    if (bufferDataByteCount + 13 >= 128) {
      uint8_t newFrame[bufferDataByteCount + 18];
      frameCreator(newFrame);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
      HAL_SPI_Transmit(&hspi1, newFrame, sizeof(newFrame), 100);
      HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
      bufferDataByteCount = 0;
    }
    curtime = __HAL_TIM_GET_COUNTER(&htim2) / 10;
    bufferData[bufferDataByteCount] = (RxHeader1.Identifier >> 8) & 0xFF;
    bufferData[bufferDataByteCount+1] = RxHeader1.Identifier & 0xFF;
    bufferData[bufferDataByteCount+2] = RxData1[0];
    bufferData[bufferDataByteCount+3] = RxData1[1];
    bufferData[bufferDataByteCount+4] = RxData1[2];
    bufferData[bufferDataByteCount+5] = RxData1[3];
    bufferData[bufferDataByteCount+6] = RxData1[4];
    bufferData[bufferDataByteCount+7] = RxData1[5];
    bufferData[bufferDataByteCount+8] = RxData1[6];
    bufferData[bufferDataByteCount+9] = RxData1[7];
    bufferData[bufferDataByteCount+10] = (curtime >> 16) & 0xFF;
    bufferData[bufferDataByteCount+11] = (curtime >> 8) & 0xFF;
    bufferData[bufferDataByteCount+12] = curtime & 0xFF;
    bufferDataByteCount = bufferDataByteCount + 13;
  }

  static void frameCreator(uint8_t *newFrame) {
    uint8_t sum = 0;
    newFrame[0] = 0x7E;
    newFrame[1] = 0x00;
    newFrame[2] = bufferDataByteCount;
    for (int i = 3; i < 17; i++) {
      newFrame[i] = frame[i];
      sum += frame[i];
    }
    for (int i = 0; i < (int) bufferDataByteCount; i++) {
      newFrame[i + 17] = bufferData[i];
      sum += bufferData[i];
    }
    newFrame[bufferDataByteCount + 17] = 255 - (sum & 0xFF);
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
  while (1){
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
