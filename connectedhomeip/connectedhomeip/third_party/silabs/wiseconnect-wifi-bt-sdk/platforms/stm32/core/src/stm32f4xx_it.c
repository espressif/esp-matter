/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#ifdef RSI_WITH_OS
#include "FreeRTOS.h"
#include "task.h"
#endif
#ifdef RSI_UART_INTERFACE
#include "rsi_os.h"
#include "rsi_events.h"
#endif
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
#ifdef RSI_CALIB_MODE_EN
void rsi_calib_uart_recv_isr(uint8_t cmd_char);
#endif

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
 DMA_HandleTypeDef hdma_spi1_rx;
 DMA_HandleTypeDef hdma_spi1_tx;
extern TIM_HandleTypeDef htim2;
#ifdef RSI_WITH_OS
extern TIM_HandleTypeDef htim1;
#endif
extern UART_HandleTypeDef com_port;
#ifdef RSI_UART_INTERFACE
extern UART_HandleTypeDef huart1;
#endif
uint8_t receive_completed,transmit_completed,send_ping_for_keep_alive;
#ifdef RSI_WITH_OS 
extern void xPortSysTickHandler( void );
#endif

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}


/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
#ifdef RSI_WITH_OS
	if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED)
	{
		xPortSysTickHandler();
	}
#endif
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}


#ifdef RSI_USING_TIMER_INTERRUPT
/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM1 update interrupt and TIM10 global interrupt.
  */
void TIM1_UP_TIM10_IRQHandler(void)
{
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 0 */
send_ping_for_keep_alive=1;
  /* USER CODE END TIM1_UP_TIM10_IRQn 0 */
  HAL_TIM_IRQHandler(&htim1);
  /* USER CODE BEGIN TIM1_UP_TIM10_IRQn 1 */

  /* USER CODE END TIM1_UP_TIM10_IRQn 1 */
}


/**
  * @brief This function handles TIM2 global interrupt.
  */
uint32_t timer;
void TIM2_IRQHandler(void)
{
  /* USER CODE BEGIN TIM2_IRQn 0 */

	send_ping_for_keep_alive=1;
	timer++;
  /* USER CODE END TIM2_IRQn 0 */
  HAL_TIM_IRQHandler(&htim2);
  /* USER CODE BEGIN TIM2_IRQn 1 */


  /* USER CODE END TIM2_IRQn 1 */
}
#endif

/**
  * @brief This function handles DMA2 stream0 global interrupt.
  */
void DMA2_Stream0_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Stream0_IRQn 0 */
receive_completed =1;
  /* USER CODE END DMA2_Stream0_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_rx);
  /* USER CODE BEGIN DMA2_Stream0_IRQn 1 */

  /* USER CODE END DMA2_Stream0_IRQn 1 */
}
//void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi)
//{
//  /* Prevent unused argument(s) compilation warning */
//	while(1);

//  /* NOTE : This function should not be modified, when the callback is needed,
//            the HAL_SPI_TxRxCpltCallback should be implemented in the user file
//   */
//}
/**
  * @brief This function handles DMA2 stream2 global interrupt.
  */
void DMA2_Stream2_IRQHandler(void)
{
	transmit_completed=1;
  /* USER CODE BEGIN DMA2_Stream2_IRQn 0 */

  /* USER CODE END DMA2_Stream2_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_spi1_tx);
  /* USER CODE BEGIN DMA2_Stream2_IRQn 1 */

  /* USER CODE END DMA2_Stream2_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
#ifdef RSI_UART_INTERFACE
uint8_t uart_rev_buf[1600]={0xff};
uint32_t  uart_rev_buf_indx  =0;
uint8_t uart_rx_start;
uint32_t index_cnt,Uart_recv_done,desired_len;
uint8_t uart_dump[1600];
uint32_t dump_index = 0;
uint32_t issue_hit;
uint32_t  value = 0, ack_cnt, previous;
uint8_t abrd_bit=0;

void USART1_IRQHandler(void)
{
 uint32_t isrflags   = READ_REG(huart1.Instance->SR);
  uint32_t cr1its     = READ_REG(huart1.Instance->CR1);
  uint32_t cr3its     = READ_REG(huart1.Instance->CR3);
  uint32_t errorflags = 0x00U;
  //uint32_t dmarequest = 0x00U;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
//      UART_Receive_IT(huart);
//      return;
    /* USER CODE BEGIN USART1_IRQn 0 */
    uart_dump[dump_index] = uart_rev_buf[uart_rev_buf_indx++] = huart1.Instance->DR;

    if(dump_index == 1599)
    {
      dump_index = 0;
    }
    else
    {
      dump_index++;
    }

    if (!abrd_bit)
    {
    }
    else
    {

      if(uart_rev_buf_indx == 3)
      {

        previous = desired_len;
        desired_len=(((*(uint16_t*)&uart_rev_buf[0])) - 4);
      }
      if((uart_rev_buf_indx > 3) && ((uart_rev_buf_indx - 4) == desired_len))
      {
        uart_rev_buf_indx =0 ;
        value = 0;
#ifdef RSI_WITH_OS
        rsi_set_event_from_isr(RSI_RX_EVENT);
#else
        rsi_set_event(RSI_RX_EVENT);
#endif

      }
    }
  }
}
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}
#endif


/**
  * @brief This function handles USART2 global interrupt.
  */
#ifdef RSI_CALIB_MODE_EN
void USART2_IRQHandler(void)
{

  uint32_t isrflags = READ_REG(com_port.Instance->SR);
  uint32_t cr1its = READ_REG(com_port.Instance->CR1);
  uint32_t cr3its = READ_REG(com_port.Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint8_t temp;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE |
        USART_SR_NE));
  if (errorflags == RESET) {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) &&
        ((cr1its & USART_CR1_RXNEIE) != RESET)) {
      /* USER CODE BEGIN USART2_IRQn 0 */
      temp = com_port.Instance->DR;
      rsi_calib_uart_recv_isr(temp);
      /* USER CODE END USART2_IRQn 0 */
    }
  }

  /* USER CODE BEGIN USART2_IRQn 1 */

  HAL_UART_IRQHandler(&com_port);

  /* USER CODE END USART2_IRQn 1 */
}
#endif

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
