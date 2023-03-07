/*******************************************************************************
* @file  rsi_uart.c
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
 * @file     rsi_uart.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 *  @brief : Contains UART HAL porting functionality
 *
 * Description  Contains UART HAL porting functionality
 *
 *
 */
#include "rsi_driver.h"
#ifndef EFM32_SDIO // This file is not needed for EFM32 board. In order to avoid compilation warnings, we excluded the below code for EFM32
#include "rsi_board_configuration.h"
#endif
#ifdef RSI_UART_INTERFACE
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
extern UART_HandleTypeDef huart1;

#include <stdlib.h>
#include <stdio.h>
#include "rsi_data_types.h"
#include "rsi_error.h"
#include "rsi_utils.h"
#include "rsi_pkt_mgmt.h"
#include "rsi_os.h"
#include "rsi_queue.h"
#include "rsi_uart.h"
#include "rsi_events.h"

rsi_linux_app_cb_t rsi_linux_app_cb;
/** @addtogroup DRIVER3
* @{
*/
/*==============================================*/
/**
*/

int16_t rsi_frame_read(uint8_t *pkt_buffer)
{

  memcpy(pkt_buffer, uart_rev_buf + 4, desired_len);

  return 0;
}

/*==============================================*/
/**
  */
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param)
{
  int16_t retval = 0;

  // API to write packet to UART interface
  retval = rsi_uart_send((uint8_t *)uFrameDscFrame, (size_param + RSI_FRAME_DESC_LEN));
  while (huart1.gState != HAL_UART_STATE_READY)
    ;
  while (huart1.TxXferCount != 0)
    ;
  return retval;
}

/*==============================================*/
/**
 * @brief       Initialize the UART interface module.
 * @param[in]   void
 * @return      0              - Success \n
 *              Negative Value - Failure
 */

int32_t rsi_uart_init(void)
{
  SL_PRINTF(SL_UART_INIT_ENTRY, DRIVER, LOG_INFO);
  huart1.Instance          = USART1;
  huart1.Init.BaudRate     = 921600;
  huart1.Init.WordLength   = UART_WORDLENGTH_8B;
  huart1.Init.StopBits     = UART_STOPBITS_1;
  huart1.Init.Parity       = UART_PARITY_NONE;
  huart1.Init.Mode         = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  SL_PRINTF(SL_UART_INIT_EXIT, DRIVER, LOG_INFO);
  return 0;
}

/*==============================================*/
/**
 * @brief       UART de-initialization
 * @param[in]   void
 * @return      0              - Success \n
 *              Negative Value - Failure
 */

int32_t rsi_uart_deinit(void)
{
  return 0;
}

#endif
/** @} */
