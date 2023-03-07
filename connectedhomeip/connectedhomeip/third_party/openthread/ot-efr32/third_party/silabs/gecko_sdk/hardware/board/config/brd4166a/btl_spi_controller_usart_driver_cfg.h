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
// <i> Default: 6400000
#define SL_USART_EXTFLASH_FREQUENCY           6400000

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,CLK,(CS)> SL_USART_EXTFLASH
// $[USART_SL_USART_EXTFLASH]
#define SL_USART_EXTFLASH_PERIPHERAL             USART2
#define SL_USART_EXTFLASH_PERIPHERAL_NO          2

// USART2 TX on PK0
#define SL_USART_EXTFLASH_TX_PORT                gpioPortK
#define SL_USART_EXTFLASH_TX_PIN                 0
#define SL_USART_EXTFLASH_TX_LOC                 29

// USART2 RX on PK2
#define SL_USART_EXTFLASH_RX_PORT                gpioPortK
#define SL_USART_EXTFLASH_RX_PIN                 2
#define SL_USART_EXTFLASH_RX_LOC                 30

// USART2 CLK on PF7
#define SL_USART_EXTFLASH_CLK_PORT               gpioPortF
#define SL_USART_EXTFLASH_CLK_PIN                7
#define SL_USART_EXTFLASH_CLK_LOC                18

// USART2 CS on PK1
#define SL_USART_EXTFLASH_CS_PORT                gpioPortK
#define SL_USART_EXTFLASH_CS_PIN                 1
#define SL_USART_EXTFLASH_CS_LOC                 27

// [USART_SL_USART_EXTFLASH]$

// <gpio> SL_EXTFLASH_WP
// $[GPIO_SL_EXTFLASH_WP]

// [GPIO_SL_EXTFLASH_WP]$

// <gpio> SL_EXTFLASH_HOLD
// $[GPIO_SL_EXTFLASH_HOLD]

// [GPIO_SL_EXTFLASH_HOLD]$

// <<< sl:end pin_tool >>>

#endif // BTL_SPI_CONTROLLER_USART_DRIVER_CONFIG_H
