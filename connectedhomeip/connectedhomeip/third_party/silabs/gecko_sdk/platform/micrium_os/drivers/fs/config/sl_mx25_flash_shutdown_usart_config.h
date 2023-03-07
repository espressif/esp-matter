/***************************************************************************//**
 * @file
 * @brief SL_MX25_FLASH_SHUTDOWN_USART Config
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

#ifndef SL_MX25_FLASH_SHUTDOWN_CONFIG_H
#define SL_MX25_FLASH_SHUTDOWN_CONFIG_H

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,CLK> SL_MX25_FLASH_SHUTDOWN
// $[USART_SL_MX25_FLASH_SHUTDOWN]
#warning "MX25 SPI peripheral not configured"
// #define SL_MX25_FLASH_SHUTDOWN_PERIPHERAL     USART1
// #define SL_MX25_FLASH_SHUTDOWN_PERIPHERAL_NO  1

// #define SL_MX25_FLASH_SHUTDOWN_TX_PORT        gpioPortC
// #define SL_MX25_FLASH_SHUTDOWN_TX_PIN         6

// #define SL_MX25_FLASH_SHUTDOWN_RX_PORT        gpioPortC
// #define SL_MX25_FLASH_SHUTDOWN_RX_PIN         7

// #define SL_MX25_FLASH_SHUTDOWN_CLK_PORT       gpioPortC
// #define SL_MX25_FLASH_SHUTDOWN_CLK_PIN        8
// [USART_SL_MX25_FLASH_SHUTDOWN]$

// <gpio> SL_MX25_FLASH_SHUTDOWN_CS

// $[GPIO_SL_MX25_FLASH_SHUTDOWN_CS]
#warning "MX25 SPI pin not configured"
// #define SL_MX25_FLASH_SHUTDOWN_CS_PORT         gpioPortA
// #define SL_MX25_FLASH_SHUTDOWN_CS_PIN          4
// [GPIO_SL_MX25_FLASH_SHUTDOWN_CS]$

// <<< sl:end pin_tool >>>

#endif // SL_MX25_FLASH_SHUTDOWN_CONFIG_H
