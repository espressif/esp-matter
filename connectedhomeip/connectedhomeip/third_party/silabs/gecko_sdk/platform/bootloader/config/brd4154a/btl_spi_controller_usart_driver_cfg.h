/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader Spi Controller Usart Driver
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
#ifndef BTL_SPI_CONTROLLER_USART_DRIVER_CONFIG_H
#define BTL_SPI_CONTROLLER_USART_DRIVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>SPI Controller USART Driver

// <o SL_USART_EXTFLASH_FREQUENCY> Frequency
// <i> Default: 2000000
#define SL_USART_EXTFLASH_FREQUENCY           2000000

// </h>

// <<< end of configuration section >>>

// SL_USART_EXTFLASH_PERIPHERAL
#define SL_USART_EXTFLASH_PERIPHERAL                   USART1
#define SL_USART_EXTFLASH_PERIPHERAL_NO                1

// USART1 TX on PF6
#define SL_USART_EXTFLASH_TX_PORT                      gpioPortF
#define SL_USART_EXTFLASH_TX_PIN                       6
#define SL_USART_EXTFLASH_TX_LOC                       30

// USART1 RX on PC7
#define SL_USART_EXTFLASH_RX_PORT                      gpioPortC
#define SL_USART_EXTFLASH_RX_PIN                       7
#define SL_USART_EXTFLASH_RX_LOC                       11

// USART1 CLK on PC9
#define SL_USART_EXTFLASH_CLK_PORT                     gpioPortC
#define SL_USART_EXTFLASH_CLK_PIN                      9
#define SL_USART_EXTFLASH_CLK_LOC                      12

// USART1 CS on PC6
#define SL_USART_EXTFLASH_CS_PORT                      gpioPortC
#define SL_USART_EXTFLASH_CS_PIN                       6
#define SL_USART_EXTFLASH_CS_LOC                       0

// SL_EXTFLASH_WP
#define SL_EXTFLASH_WP_PORT                     gpioPortF
#define SL_EXTFLASH_WP_PIN                      7

// SL_EXTFLASH_HOLD
#define SL_EXTFLASH_HOLD_PORT                   gpioPortC
#define SL_EXTFLASH_HOLD_PIN                    8

#endif // BTL_SPI_CONTROLLER_USART_DRIVER_CONFIG_H
