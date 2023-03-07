/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader Spi Controller Eusart Driver
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
#ifndef BTL_SPI_CONTROLLER_EUSART_DRIVER_CONFIG_H
#define BTL_SPI_CONTROLLER_EUSART_DRIVER_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>

// <h>SPI Controller EUSART Driver

// <o SL_EUSART_EXTFLASH_FREQUENCY> Frequency
// <i> Default: 6400000
#define SL_EUSART_EXTFLASH_FREQUENCY          6400000

// </h>

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,SCLK,CS> SL_EUSART_EXTFLASH
// $[EUSART_SL_EUSART_EXTFLASH]
#define SL_EUSART_EXTFLASH_PERIPHERAL            EUSART1
#define SL_EUSART_EXTFLASH_PERIPHERAL_NO         1

// EUSART1 TX on PC00
#define SL_EUSART_EXTFLASH_TX_PORT               gpioPortC
#define SL_EUSART_EXTFLASH_TX_PIN                0

// EUSART1 RX on PC01
#define SL_EUSART_EXTFLASH_RX_PORT               gpioPortC
#define SL_EUSART_EXTFLASH_RX_PIN                1

// EUSART1 SCLK on PC02
#define SL_EUSART_EXTFLASH_SCLK_PORT             gpioPortC
#define SL_EUSART_EXTFLASH_SCLK_PIN              2

// EUSART1 CS on PA04
#define SL_EUSART_EXTFLASH_CS_PORT               gpioPortA
#define SL_EUSART_EXTFLASH_CS_PIN                4

// [EUSART_SL_EUSART_EXTFLASH]$

// <gpio> SL_EXTFLASH_WP
// $[GPIO_SL_EXTFLASH_WP]

// [GPIO_SL_EXTFLASH_WP]$

// <gpio> SL_EXTFLASH_HOLD
// $[GPIO_SL_EXTFLASH_HOLD]

// [GPIO_SL_EXTFLASH_HOLD]$

// <<< sl:end pin_tool >>>

#endif // BTL_SPI_CONTROLLER_EUSART_DRIVER_CONFIG_H
