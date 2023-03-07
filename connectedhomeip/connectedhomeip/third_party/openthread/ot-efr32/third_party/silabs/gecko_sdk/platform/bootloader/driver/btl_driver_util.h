/***************************************************************************//**
 * @file
 * @brief Gecko bootloader driver utility functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#ifndef BTL_DRIVER_UTIL_H
#define BTL_DRIVER_UTIL_H

#include <stdint.h>
#if defined(BTL_UART_ENABLE) || defined(BTL_SPI_USART_ENABLE)
#include "em_usart.h"
#endif
#include "em_cmu.h"

/**
 * Get System Clock Frequency.
 *
 * @return system clock frequency.
 */
uint32_t util_getClockFreq(void);

#if defined(BTL_UART_ENABLE) || defined(BTL_SPI_USART_ENABLE)
/**
 * Disable USART TX, RX, and USART Clock.
 *
 * @param[in] btlUsart      The USART to deinitialize
 * @param[in] usartNum      The index of the USART to deinitialize
 * @param[in] btlUsartClock The clock signal for the USART to deinitialize
 */
void util_deinitUsart(USART_TypeDef *btlUsart, uint8_t usartNum, CMU_Clock_TypeDef btlUsartClock);
#endif
#endif
