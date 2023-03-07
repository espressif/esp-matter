/*******************************************************************************
* @file  rsi_hal_mcu_platform_init.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file       rsi_hal_mcu_platform_init.c
 * @version    0.1
 * @date       11 OCT 2018
 *
 *
 *
 * @brief HAL Board Init: Functions related to platform initialization
 *
 * @section Description
 * This file contains the list of functions for configuring the microcontroller clock.
 * Following are list of API's which need to be defined in this file.
 *
 */
/**
 * Includes
 */
#include "rsi_driver.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_hal_uart.h"
#include "stm32f4xx_hal_tim_ex.h"
#include "rsi_driver.h"
#include "rsi_hal.h"
#include "rsi_board_configuration.h"
#include "stdio.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_m4.h"
#endif

#ifdef RSI_WITH_OS
#include "FreeRTOS.h"
//#include "cmsis_os.h"
#include "task.h"
//osThreadId_t defaultTaskHandle;
void StartDefaultTask(void *argument);
volatile TickType_t xTickCount;
#endif

//#include "stm32f4xx_hal_uart.h"
#if defined(RSI_SPI_INTERFACE)
SPI_HandleTypeDef hspi1;
#elif defined(RSI_UART_INTERFACE)
UART_HandleTypeDef huart1;
#endif
TIM_HandleTypeDef htim2;

uint8_t platform_initialized;

uint8_t	com_port_data;

#ifdef RSI_DEBUG_PRINTS
static void com_port_init(void);
UART_HandleTypeDef com_port;

#ifdef __GNUC__
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#endif

#if defined(RSI_SPI_INTERFACE)
SPI_HandleTypeDef hspi1;
static void MX_SPI1_Init(void);
#elif defined(RSI_UART_INTERFACE)
UART_HandleTypeDef huart1;
static void MX_USART1_UART_Init(void);
#endif

static void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
#ifdef RSI_WITH_OS
static void MX_TIM2_Init(void);
#endif

/*==============================================*/
/**
 * @fn           void rsi_hal_board_init()
 * @brief        This function Initializes the platform
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @section description
 * This function initializes the platform
 *
 */

void rsi_hal_board_init()
{
	if(!platform_initialized)
	{
  //! Initializes the platform
	   HAL_Init();

		/* Configure the system clock */
		SystemClock_Config();

	/* Initialize all configured peripherals */
		MX_GPIO_Init();
#ifdef RSI_WITH_OS
		MX_TIM2_Init();
#endif
		MX_DMA_Init();
#ifdef RSI_CALIB_MODE_EN
    com_port_init();

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&com_port, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error,
     * overrun error) */
    __HAL_UART_ENABLE_IT(&com_port, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&com_port, UART_IT_RXNE);

#else
#ifdef RSI_DEBUG_PRINTS
		com_port_init();
#endif
		//! Enable receive interrupt with 1 byte of size
		HAL_UART_Receive_IT(&com_port, &com_port_data ,1);
#endif
#if defined(RSI_SPI_INTERFACE)

	//! Intializes SPI
   	MX_SPI1_Init();

#elif defined(RSI_UART_INTERFACE)

			MX_USART1_UART_Init();

			//! Intializes UART
	 /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

		//! abrd detection
		ABRD();

#endif
		platform_initialized = 1;
	}
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
/*!This is the Maximum possible SPI clock,If any device init fails on any particular MCU board.
	 In such case reduce the MCU clock as per below:
	 RCC_OscInitStruct.PLL.PLLN = 60; //30Mhz SPI Clock
	 RCC_OscInitStruct.PLL.PLLN = 50; //25Mhz SPI Clock
	 RCC_OscInitStruct.PLL.PLLN = 70; //35Mhz SPI clock
*/
  RCC_OscInitStruct.PLL.PLLN = 80;   //42Mhz SPI Clock
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
#ifdef RSI_SPI_INTERFACE
static void MX_SPI1_Init(void)
{
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLED;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLED;
  hspi1.Init.CRCPolynomial = 10;
  HAL_SPI_Init(&hspi1);
}
#endif

#ifdef RSI_WITH_OS
/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 48000;   //As the clock is running at 48MHz
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	//htim2.Init.Period = 1000;  //1 second
	htim2.Init.Period = 1000*30; // 30 sec

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
#endif

/**
 * Enable DMA controller clock
 */
void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

}
#if defined(RSI_UART_INTERFACE)
/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

#endif

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

	/*Configure GPIO pin : PA9 */
	GPIO_InitStruct.Pin = GPIO_PIN_9;
#ifndef RSI_ACTIVE_LOW
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
#else
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
#endif
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

#ifdef LOGGING_STATS
		 /*Configure GPIO pin : PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
#endif
	/*Configure GPIO pin : PB6 */
	GPIO_InitStruct.Pin = GPIO_PIN_6;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	//!for
	HAL_NVIC_SetPriority(USART2_IRQn,6,0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);
  #ifdef RSI_WITH_OS
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  #endif

#ifdef LOGGING_STATS
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 6, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif

	 /* USART1 interrupt Init */
  HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

/*==============================================*/
/**
 * @fn           void rsi_switch_to_high_clk_freq()
 * @brief        This function intializes SPI to high clock
 * @param[in]    none
 * @param[out]   none
 * @return       none
 * @section description
 * This function intializes SPI to high clock
 *
 *
 */

void rsi_switch_to_high_clk_freq()
{
  //! Initializes the high speed clock

}


#ifdef RSI_DEBUG_PRINTS
static void com_port_init(void)
{

  com_port.Instance = COM_PORT_PERIPHERAL;
  com_port.Init.BaudRate = 115200;
  com_port.Init.WordLength = UART_WORDLENGTH_8B;
  com_port.Init.StopBits = UART_STOPBITS_1;
  com_port.Init.Parity = UART_PARITY_NONE;
  com_port.Init.Mode = UART_MODE_TX_RX;
  com_port.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  com_port.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&com_port) != HAL_OK)
  {
    Error_Handler();
  }
}
#endif

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM1 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM1) {
		HAL_IncTick();
	}
#if defined(RSI_WITH_OS) && defined(FREERTOS)
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
#endif
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}
#ifdef RSI_DEBUG_PRINTS
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&com_port, (uint8_t *)&ch, 1, 0xFFFF);
	return ch;
}
#endif
