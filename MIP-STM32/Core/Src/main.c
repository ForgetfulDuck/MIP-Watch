/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lsm6dsr_reg.h"
#include "max31331_reg.h"

#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c3;
DMA_HandleTypeDef hdma_i2c3_rx;

RTC_HandleTypeDef hrtc;

/* USER CODE BEGIN PV */
/* ============== I2C Variables ============== */
typedef struct {
  I2C_HandleTypeDef *i2c;
  uint16_t addr_8bit;   // 7-bit I2C address (unshifted)
} i2c_dev_t;

static stmdev_ctx_t   s_lsm_ctx;
static max31331_ctx_t s_max_ctx;

static i2c_dev_t g_imu_bus = {
  .i2c = &hi2c3,
  .addr_8bit = (LSM6DSR_I2C_ADD_L)
};

static i2c_dev_t g_rtc_bus = {
  .i2c = &hi2c3,
  .addr_8bit = (MAX31331_I2C_ADD)
};

/* ============== JOB QUEUE VARIABLES ============== */
#define QUEUE_SIZE 16

typedef struct {
    int data[QUEUE_SIZE];
    int front;
    int rear;
    int count;
} JobQueue;

enum JobTypes {
  stepJob = 0,
  tapJob  = 1,
  timeJob = 2
};

volatile JobQueue jobQueue;
volatile uint8_t tapsInQueue = 0;
volatile uint8_t stepsInQueue = 0;
volatile uint8_t timeInQueue = 0;

volatile uint8_t DMArunning = 0;
volatile uint8_t TickDMAflag = 0;
volatile uint8_t TickRXcomp = 0;
volatile uint8_t TapDMAflag = 0;
volatile uint8_t TapRXcomp = 0;
volatile uint8_t StepDMAflag = 0;
volatile uint8_t StepRXcomp = 0;


/* ============== IMU VARIABLES ============== */
// Min # of steps before interrupts
#define STEP_DEBOUNCE_THRESHOLD 8
// DMA RX buffers
uint8_t s_step_rx_buf[2];
uint8_t s_tap_rx_buf[1];
/* Tracked values */
volatile uint16_t g_step_count = 0;
volatile uint16_t g_num_taps = 0;

/* ============== RTC VARIABLES ============== */
uint8_t s_rtc_rx_buf[8];
uint8_t g_RTC_time[8];

/* ============== Interrupt Setup ============== */
#define IMU_INT1_Pin            GPIO_PIN_1
#define IMU_INT1_GPIO_Port      GPIOA

#define IMU_INT2_Pin 			GPIO_PIN_0
#define IMU_INT2_GPIO_Port 		GPIOA

#define RTC_INT_Pin 			GPIO_PIN_2
#define RTC_INT_GPIO_Port 		GPIOA

#define I2C3_SCL_PIN 			GPIO_PIN_7
#define I2C3_SDA_PIN 			GPIO_PIN_6
#define I2C3_PORT 				GPIOA

uint8_t inStopMode  = 0;
uint8_t inSleepMode = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C3_Init(void);
static void MX_RTC_Init(void);
/* USER CODE BEGIN PFP */
/* ============== JOB QUEUE FUNCTIONS ============== */
void initJobQueue(volatile JobQueue *q);
int isJobsEmpty(volatile JobQueue *q);
int isJobsFull(volatile JobQueue *q);
int enqueueJob(volatile JobQueue *q, int value);
int dequeueJob(volatile JobQueue *q, int *value);
static void startJob(int job);
static void processJob(void);
int job = 5;

/* ============== I2C DEVICE FUNCTIONS ============== */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len);
static int32_t platform_read (void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static void clear_I2C_bus(void);
static void reconfigure_interupts(int interrupt_en);

/* ============== I2C DEVICE INIT FUNCTIONS ============== */
static void LSM6DSR_Init(void);
static void LSM6DSR_Init_Tap_INT2(void);
static void LSM6DSR_Init_Pedo_INT2(void);
static void MAX31331_Init(void);
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
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
//   reconfigure_interupts(0); // Configure IMU interrupt pins as GPIO outputs set to 0
//   __HAL_RCC_GPIOA_CLK_ENABLE();
  clear_I2C_bus();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C3_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */

  DMArunning 	= 0;
  tapsInQueue = 0;
  stepsInQueue = 0;
  timeInQueue = 0;
  
  TickDMAflag 	= 0;
  TickRXcomp 	= 0;

  TapDMAflag 	= 0;
  TapRXcomp 	= 0;
  
  StepDMAflag 	= 0;
  StepRXcomp 	= 0;

  inStopMode  = 0;
  inSleepMode = 0;

  LSM6DSR_Init();
//   reconfigure_interupts(1); // Configure IMU interrupt pins as EXTI
  
  MAX31331_Init();

  initJobQueue(&jobQueue);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    HAL_Delay(1);

    // Parse data from complete jobs
    processJob();
    
    // If DMA is free and job queue !empty, start job
    if(!DMArunning) {
      if (dequeueJob(&jobQueue, &job)) {
        startJob(job);
      } 
	//   else {// else job queue is empty, go back to STOP mode while we wait for next interrupt
	// 	inStopMode = 1;
	// 	HAL_SuspendTick();
	// 	HAL_PWREx_EnterSTOP1Mode(PWR_SLEEPENTRY_WFI);

	// 	HAL_ResumeTick();
	// 	inStopMode = 0;
	//   }
    } 
	// else {// Waiting for DMA to finish, go back to SLEEP mode while we wait for I2C callback
	// 	inSleepMode = 1;
	// 	HAL_SuspendTick();
	// 	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON,PWR_SLEEPENTRY_WFI);

	// 	HAL_ResumeTick();
	// 	inSleepMode = 0;
	// }
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI
                              |RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_HSE, RCC_MCO2DIV_32);
}

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */
  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.Timing = 0x00303D5B;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c3, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c3, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */
  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  hrtc.Init.BinMode = RTC_BINARY_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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

  /*Configure GPIO pin : IMU_INT1_Float_Pin */
  GPIO_InitStruct.Pin = IMU_INT1_Float_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(IMU_INT1_Float_GPIO_Port, &GPIO_InitStruct);
  
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, Vcom_Sel_Pin|XRST_Pin|ENBS_Pin|SI7_Pin
                          |SI6_Pin|SI5_Pin|SI4_Pin|DBG_SWO_Pin
                          |ENBG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, SI3_Pin|SI2_Pin|SI1_Pin|SI0_Pin
                          |DEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : NRST_Pin BOOT0_Pin */
  GPIO_InitStruct.Pin = NRST_Pin|BOOT0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : IMU_INT2_Pin */
  GPIO_InitStruct.Pin = IMU_INT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IMU_INT2_GPIO_Port, &GPIO_InitStruct);


  /*Configure GPIO pin : RTC_INT_Pin */
  GPIO_InitStruct.Pin = RTC_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(RTC_INT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Vcom_Sel_Pin XRST_Pin ENBS_Pin SI7_Pin
                           SI6_Pin SI5_Pin SI4_Pin DBG_SWO_Pin
                           ENBG_Pin */
  GPIO_InitStruct.Pin = Vcom_Sel_Pin|XRST_Pin|ENBS_Pin|SI7_Pin
                          |SI6_Pin|SI5_Pin|SI4_Pin|DBG_SWO_Pin
                          |ENBG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : DISP_SCLK_Pin */
  GPIO_InitStruct.Pin = DISP_SCLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_SWD;
  HAL_GPIO_Init(DISP_SCLK_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SI3_Pin SI2_Pin SI1_Pin SI0_Pin
                           DEN_Pin */
  GPIO_InitStruct.Pin = SI3_Pin|SI2_Pin|SI1_Pin|SI0_Pin
                          |DEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/* ============== JOB QUEUE ============== */
void initJobQueue(volatile JobQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->count = 0;
}

int isJobsEmpty(volatile JobQueue *q) {
    return q->count == 0;
}

int isJobsFull(volatile JobQueue *q) {
    return q->count == QUEUE_SIZE;
}

int enqueueJob(volatile JobQueue *q, int value){
	if (isJobsFull(q)) {
        // printf("Queue is full! Cannot enqueue %d\n", value);
        return 0;
    }
    
    q->rear = (q->rear + 1) % QUEUE_SIZE;
    q->data[q->rear] = value;
    q->count++;
    
    return 1;
}

int dequeueJob(volatile JobQueue *q, int *value){
	if (isJobsEmpty(q)) {
        // printf("Queue is empty! Cannot dequeue\n");
        return 0;
    }
    
    *value = q->data[q->front];
    q->front = (q->front + 1) % QUEUE_SIZE;
    q->count--;
    
    return 1;
}

// Start DMA read
static void startJob(int job)
{
	lsm6dsr_func_cfg_access_t func_cfg_access = {0};
	switch (job) {
		case stepJob:
			if (stepsInQueue) stepsInQueue--;
			func_cfg_access.reg_access = LSM6DSR_EMBEDDED_FUNC_BANK; // Enable standard register bank
			if (!HAL_I2C_Mem_Write(&hi2c3, LSM6DSR_I2C_ADD_L, LSM6DSR_FUNC_CFG_ACCESS, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&func_cfg_access, 1, 5)) {
				DMArunning = 1;
				StepDMAflag = 1;
				HAL_I2C_Mem_Read_DMA(&hi2c3, LSM6DSR_I2C_ADD_L, LSM6DSR_STEP_COUNTER_L, I2C_MEMADD_SIZE_8BIT, s_step_rx_buf, 2);
			}
			break;
		case tapJob:
			if (tapsInQueue) tapsInQueue--;
			func_cfg_access.reg_access = LSM6DSR_USER_BANK; // Enable standard register bank
			if(!HAL_I2C_Mem_Write(&hi2c3, LSM6DSR_I2C_ADD_L, LSM6DSR_FUNC_CFG_ACCESS, I2C_MEMADD_SIZE_8BIT, (uint8_t *)&func_cfg_access, 1, 5)) {
				DMArunning = 1;
				TapDMAflag = 1;
				HAL_I2C_Mem_Read_DMA(&hi2c3, LSM6DSR_I2C_ADD_L, LSM6DSR_TAP_SRC, I2C_MEMADD_SIZE_8BIT, s_tap_rx_buf, 1);
			}
			break;
		case timeJob:
			if (timeInQueue) timeInQueue--;
			uint8_t status;
			max31331_get_status(&s_max_ctx, &status); // Clears interrupt
			DMArunning = 1;
			TickDMAflag = 1;
			HAL_I2C_Mem_Read_DMA(&hi2c3, MAX31331_I2C_ADD, MAX31331_SECONDS_1_128, I2C_MEMADD_SIZE_8BIT, s_rtc_rx_buf, 8); // Read sec_128 to year in one go
			break;
		default:
			break;		
		// DMA started, we can go to sleep while we wait for I2C callback
	}
}

// Process DMA data
static void processJob(void) {
	if (TickRXcomp) { // Update Time
		uint8_t s128 = s_rtc_rx_buf[0]; // Unused for now
		uint8_t s  	 = bin2dec_bcd(s_rtc_rx_buf[1] & MAX31331_SECONDS_MSK);
		uint8_t m  	 = bin2dec_bcd(s_rtc_rx_buf[2] & MAX31331_MINUTES_MSK);
		uint8_t h  	 = s_rtc_rx_buf[3];

		uint8_t format  = (h & MAX31331_TS_F24_12_MSK) >> MAX31331_TS_F24_12_POS;
		
		int hour;

		if (format == HOUR24) {
			hour = ((h & MAX31331_HOURS_HOUR_MSK) 		>> MAX31331_HOURS_HOUR_POS) + 
				   (((h & MAX31331_HOURS_HR10_MSK) 		>> MAX31331_HOURS_HR10_POS) * 10) +
				   (((h & MAX31331_HOURS_HR20_AMPM_MSK) >> MAX31331_HOURS_HR20_AMPM_POS) * 20);
		} else {
			uint8_t h12 = ((h & MAX31331_HOURS_HR10_MSK) >> MAX31331_HOURS_HR10_POS) * 10
						+ ((h & MAX31331_HOURS_HOUR_MSK) >> MAX31331_HOURS_HOUR_POS);
			uint8_t am_pm = (h & MAX31331_HOURS_HR20_AMPM_MSK) >> MAX31331_HOURS_HR20_AMPM_POS;
			if (am_pm == AM) {          // AM
				hour = (h12 == 12) ? 0 : h12;
			} else {                    // PM
				hour = (h12 == 12) ? 12 : h12 + 12;
			}
		}

		uint8_t dy 	 = s_rtc_rx_buf[4] & MAX31331_DAY_MSK;
		uint8_t dt 	 = bin2dec_bcd(s_rtc_rx_buf[5] & MAX31331_DATE_MSK);
		uint8_t mn 	 = bin2dec_bcd(s_rtc_rx_buf[6] & MAX31331_MONTH_MSK);
		uint8_t yr 	 = bin2dec_bcd(s_rtc_rx_buf[7] & MAX31331_YEAR_MSK);

		g_RTC_time[0] = s128;
		g_RTC_time[1] = s;
		g_RTC_time[2] = m;
		g_RTC_time[3] = hour;
		g_RTC_time[4] = dy;
		g_RTC_time[5] = dt;
		g_RTC_time[6] = mn;
		g_RTC_time[7] = yr;
		uint8_t status;
		max31331_get_status(&s_max_ctx, &status); // Clears interrupt

		TickRXcomp = 0;
	} else if (TapRXcomp) { // Handle Tap/DoubleTap
		lsm6dsr_tap_src_t *tap_data = (lsm6dsr_tap_src_t *)s_tap_rx_buf;
		if (tap_data->double_tap) {
			g_num_taps = 2;
		} else if (tap_data->single_tap) {
			g_num_taps = 1;
		} else {
			g_num_taps = 0;
		}

		TapRXcomp = 0;
	} else if (StepRXcomp) { // Update Step Count
		g_step_count = (uint16_t)s_step_rx_buf[1] << 8 | (uint16_t)s_step_rx_buf[0];
		StepRXcomp = 0;
	}
}

/* ============== I2C Device Initialization ============== */
static void clear_I2C_bus(void) {
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* Configure SCL and SDA as Open-Drain GPIO outputs (we assume external pull-ups present) */
    GPIO_InitStruct.Pin = I2C3_SCL_PIN | I2C3_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C3_PORT, &GPIO_InitStruct);

	/* Make sure SDA & SCL released high (external pull-ups will pull them high) */
    HAL_GPIO_WritePin(I2C3_PORT, I2C3_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(I2C3_PORT, I2C3_SCL_PIN, GPIO_PIN_SET);
    HAL_Delay(1);

	for (int i = 0; i < 9; i++)
	{
		HAL_GPIO_WritePin(I2C3_PORT, I2C3_SCL_PIN, GPIO_PIN_RESET);
		HAL_Delay(10);
		HAL_GPIO_WritePin(I2C3_PORT, I2C3_SCL_PIN, GPIO_PIN_SET);
		HAL_Delay(10);
	}

	HAL_GPIO_DeInit(I2C3_PORT, I2C3_SCL_PIN | I2C3_SDA_PIN); // Return pins to default state
}

static void reconfigure_interupts(int interrupt_en) {
	HAL_GPIO_DeInit(GPIOA, IMU_INT2_Pin|IMU_INT1_Pin); // Reset pin configuration
	
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	if(interrupt_en) {
		/*Configure GPIO pins as interrupts : IMU_INT2_Pin IMU_INT1_Pin */
		GPIO_InitStruct.Pin = IMU_INT2_Pin|IMU_INT1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
		GPIO_InitStruct.Pull = GPIO_NOPULL;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

		/* EXTI interrupt init*/
		HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

		HAL_NVIC_SetPriority(EXTI2_3_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
	}
	else { // Configure them as GPIO outputs set to 0
		GPIO_InitTypeDef GPIO_InitStruct = {0};
		GPIO_InitStruct.Pin = IMU_INT1_Pin;
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLDOWN;
		HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	}
}

static void LSM6DSR_Init(void) {
	uint8_t rst = 1;
	uint8_t who = 0;

	// Platform initialization
	s_lsm_ctx.write_reg = platform_write;
	s_lsm_ctx.read_reg  = platform_read;
	s_lsm_ctx.mdelay    = HAL_Delay;
	s_lsm_ctx.handle    = &g_imu_bus;

	HAL_Delay(100); // Initial power-on delay

	// Perform software reset with proper timing
	do {
		if (lsm6dsr_reset_set(&s_lsm_ctx, 1) != 0) {
			Error_Handler();
		}
		HAL_Delay(50); // Extended delay for reset completion
	} while (lsm6dsr_reset_get(&s_lsm_ctx, &rst) != 0 || rst != 0);

	// Verify device ID with retries
	for (int attempt = 0; attempt < 10; attempt++) {
		if (lsm6dsr_device_id_get(&s_lsm_ctx, &who) == 0 && who == LSM6DSR_ID) {
			break;
		}
		HAL_Delay(20);
	}

	if (who != LSM6DSR_ID) {
		Error_Handler();
	}

	// Disable I3C interface
	if (lsm6dsr_i3c_disable_set(&s_lsm_ctx, LSM6DSR_I3C_DISABLE) != 0) {
		Error_Handler();
	}

	/* Disable I3C (Mode 1: IMU is I2C slave) */
	lsm6dsr_i3c_disable_set(&s_lsm_ctx, LSM6DSR_I3C_DISABLE);

	/* Put gyroscope into power-down */
	lsm6dsr_gy_data_rate_set(&s_lsm_ctx, LSM6DSR_GY_ODR_OFF);
	/* Gryo power mode should be irrelevant (set to normal for safe default) */
	lsm6dsr_gy_power_mode_set(&s_lsm_ctx, LSM6DSR_GY_NORMAL);
	/* BDU: Blocks of registers can't be updated mid-read */
	lsm6dsr_block_data_update_set(&s_lsm_ctx, 1);

	LSM6DSR_Init_Pedo_INT2();
	/* TAP detection overrides ODR to 416Hz, setting accel to high power mode */
	// LSM6DSR_Init_Tap_INT2();

	/* Configure pin mode/polarity */
	lsm6dsr_pin_mode_set(&s_lsm_ctx, LSM6DSR_PUSH_PULL);
	lsm6dsr_pin_polarity_set(&s_lsm_ctx, LSM6DSR_ACTIVE_HIGH);
	lsm6dsr_int_notification_set(&s_lsm_ctx, LSM6DSR_ALL_INT_PULSED);

}

// Pedometer Config based on Application Note 5358
static void LSM6DSR_Init_Pedo_INT2(void) {
	/* Pedometer works at accelerometer 2g Full Scale (STMicroelectronics/lsm6dsr-pid) */
	lsm6dsr_xl_full_scale_set(&s_lsm_ctx, LSM6DSR_4g);
	/* 6.1 Pedometer functions: ODR >= 26Hz for step counter*/
	lsm6dsr_xl_data_rate_set(&s_lsm_ctx, LSM6DSR_XL_ODR_26Hz);

	/* Enable pedometer with false-step rejection */
	lsm6dsr_pedo_sens_set(&s_lsm_ctx, PROPERTY_ENABLE);
	lsm6dsr_pedo_mode_set(&s_lsm_ctx, LSM6DSR_PEDO_ADV_FALSE_STEP_REJ);
	lsm6dsr_pedo_int_mode_set(&s_lsm_ctx, LSM6DSR_EVERY_STEP);
	lsm6dsr_steps_reset(&s_lsm_ctx);
	uint8_t debounce = STEP_DEBOUNCE_THRESHOLD;
	lsm6dsr_pedo_debounce_steps_set(&s_lsm_ctx, &debounce);

	/* Route Step Detector interrupt to INT1 */
	lsm6dsr_pin_int2_route_t int2_route;
	lsm6dsr_pin_int2_route_get(&s_lsm_ctx, &int2_route);
	int2_route.emb_func_int2.int2_step_detector = 1;
	lsm6dsr_pin_int2_route_set(&s_lsm_ctx, &int2_route);
}

// Tap Detection Config based on Application Note 5358
static void LSM6DSR_Init_Tap_INT2(void) {
	/* Accelerometer works at accelerometer 2g Full Scale*/
	lsm6dsr_xl_full_scale_set(&s_lsm_ctx, LSM6DSR_2g);
	/* 5.5 Single-tap and double-tap recognition: ODR >= 416Hz for tap detection*/
	lsm6dsr_xl_data_rate_set(&s_lsm_ctx, LSM6DSR_XL_ODR_416Hz);

	/* Enable tap detection on all axes */
	lsm6dsr_tap_detection_on_x_set(&s_lsm_ctx, 1);
	lsm6dsr_tap_detection_on_y_set(&s_lsm_ctx, 1);
	lsm6dsr_tap_detection_on_z_set(&s_lsm_ctx, 1);

	/* Use slope on internal path (recommended for taps) */
	lsm6dsr_xl_hp_path_internal_set(&s_lsm_ctx, LSM6DSR_USE_SLOPE);

	/* Thresholds: 0x0C -> ~0.75 g at FS=±2 g */
	lsm6dsr_tap_threshold_x_set(&s_lsm_ctx, 0x0C);
	lsm6dsr_tap_threshold_y_set(&s_lsm_ctx, 0x0C);
	lsm6dsr_tap_threshold_z_set(&s_lsm_ctx, 0x0C);

	/* Axis priority X->Y->Z */
	lsm6dsr_tap_axis_priority_set(&s_lsm_ctx, LSM6DSR_XYZ);

	/* Time windows at ODR_XL=416 Hz (approx):
	SHOCK=3 (~57.7 ms), QUIET=3 (~28.8 ms), DUR=7 (~538.5 ms) */
	lsm6dsr_tap_shock_set(&s_lsm_ctx, 0x03);
	lsm6dsr_tap_quiet_set(&s_lsm_ctx, 0x03);
	lsm6dsr_tap_dur_set(&s_lsm_ctx, 0x07);

	/* Enable both single & double tap recognition */
	lsm6dsr_tap_mode_set(&s_lsm_ctx, LSM6DSR_BOTH_SINGLE_DOUBLE);

	/* Route interrupts to INT2 */
	lsm6dsr_pin_int2_route_t int2_route;
	lsm6dsr_pin_int2_route_get(&s_lsm_ctx, &int2_route);
	int2_route.md2_cfg.int2_single_tap = 1;
	int2_route.md2_cfg.int2_double_tap = 1;
	lsm6dsr_pin_int2_route_set(&s_lsm_ctx, &int2_route);
}

static void MAX31331_Init(void) {
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours   = 14;  // Example: 2PM
	uint8_t day     = 3;   // WEDNESDAY
	uint8_t date    = 27;  // 27th
	uint8_t month   = 8;   // January
	uint8_t year    = 25;  // Year = 2025 (last two digits)
	uint8_t century = 0;   // 0 = 21st century, 1 = 22nd century

	s_max_ctx.write_reg = platform_write;
	s_max_ctx.read_reg  = platform_read;
	s_max_ctx.mdelay    = HAL_Delay;
	s_max_ctx.handle    = &g_rtc_bus;

	HAL_Delay(20);

	max31331_reset_rtc(&s_max_ctx);
	HAL_Delay(5);

	max31331_set_time(&s_max_ctx, seconds, minutes, hours);
	seconds 	= 0xFF;
	minutes 	= 0xFF;
	hours   	= 0xFF;
	uint8_t format 	= 0xFF;
	HAL_Delay(5);
	
	max31331_get_time(&s_max_ctx, &seconds, &minutes, &hours, &format);
	HAL_Delay(5);

	max31331_set_date(&s_max_ctx, day, date, month, century, year);
	day     = 0xFF;
	date    = 0xFF;
	month   = 0xFF;
	year    = 0xFF;
	century = 0xFF;
	HAL_Delay(5);
	
	max31331_get_date(&s_max_ctx, &day, &date, &month, &century, &year);
	HAL_Delay(5);

	max31331_set_clko(&s_max_ctx, 1, CLKO_1Hz);
	HAL_Delay(5);

	max31331_set_timer_config(&s_max_ctx, 0, 0, 1, TIMER_64Hz); // Disable timer and "resume" to change init
	HAL_Delay(5);
	max31331_set_timer_init(&s_max_ctx, 2);                    	// Update init to get interrupts every 2 ticks
	HAL_Delay(5);
	max31331_set_timer_config(&s_max_ctx, 1, 0, 1, TIMER_64Hz); // 64hz timer every 2 ticks => 32hz interrupts
	HAL_Delay(5);
	
	max31331_set_int_en(&s_max_ctx, 0, 0, 0, 0, 1, 0, 0);      // Enable timer interrupt only
	uint8_t int_en = 0xFF;
	HAL_Delay(5);
	max31331_get_int_en(&s_max_ctx, &int_en);
	HAL_Delay(5);
	uint8_t rtc_status = 0xFF;
	max31331_get_status(&s_max_ctx, &rtc_status);
	HAL_Delay(5);
}

/* ============== EXTI IRQ Handler ============== */
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == RTC_INT_Pin) {
		if (timeInQueue < 2) {
			if (enqueueJob(&jobQueue, timeJob)) {
				timeInQueue++;
			}
		}
	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin) {
	if (GPIO_Pin == IMU_INT2_Pin) {
		if (stepsInQueue < 2) {
			if (enqueueJob(&jobQueue, stepJob)) {
				stepsInQueue++;
			}
		}
	}
	if (GPIO_Pin == IMU_INT1_Pin) { // INT 1 is disabled for now
		if (tapsInQueue < 2) {
			if (enqueueJob(&jobQueue, tapJob)) {
				tapsInQueue++;
			}
		}
	}
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
	if(hi2c == &hi2c3) {
		// Step counter DMA finished
		if (TickDMAflag) {
			TickDMAflag = 0;
			TickRXcomp = 1;
		} else if (TapDMAflag) {
			TapDMAflag = 0;
			TapRXcomp = 1;
		} else if (StepDMAflag) {
			StepDMAflag = 0;
			StepRXcomp = 1;
		}
    }
	DMArunning = 0;
}

/* ============== Platform Specific Read/Write ============== */
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len)
{
	i2c_dev_t *dev = (i2c_dev_t *)handle;
	if (HAL_I2C_Mem_Write(dev->i2c, (dev->addr_8bit), reg,
							I2C_MEMADD_SIZE_8BIT, (uint8_t*)bufp, len, 100) != HAL_OK)
		return -1;
	return 0;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
	i2c_dev_t *dev = (i2c_dev_t *)handle;
	if (HAL_I2C_Mem_Read(dev->i2c, (dev->addr_8bit), reg,
							I2C_MEMADD_SIZE_8BIT, bufp, len, 100) != HAL_OK)
	return -1;
	return 0;
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
