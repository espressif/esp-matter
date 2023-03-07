/***************************************************************************//**
 * @file
 * @brief SPIDRV Config
 *******************************************************************************
 * # License
 * <b>Copyright 2019 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef LEGACY_NCP_SPI_CONFIG_H
#define LEGACY_NCP_SPI_CONFIG_H

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,CLK,(CS)> LEGACY_NCP_SPI
// $[USART_LEGACY_NCP_SPI]
#define LEGACY_NCP_SPI_PERIPHERAL                USART0
#define LEGACY_NCP_SPI_PERIPHERAL_NO             0

// USART0 TX on PC01
#define LEGACY_NCP_SPI_TX_PORT                   gpioPortC
#define LEGACY_NCP_SPI_TX_PIN                    1

// USART0 RX on PC02
#define LEGACY_NCP_SPI_RX_PORT                   gpioPortC
#define LEGACY_NCP_SPI_RX_PIN                    2

// USART0 CLK on PC03
#define LEGACY_NCP_SPI_CLK_PORT                  gpioPortC
#define LEGACY_NCP_SPI_CLK_PIN                   3

// USART0 CS on PC00
#define LEGACY_NCP_SPI_CS_PORT                   gpioPortC
#define LEGACY_NCP_SPI_CS_PIN                    0

// [USART_LEGACY_NCP_SPI]$

// <gpio> LEGACY_NCP_SPI_HOST_INT
// $[GPIO_LEGACY_NCP_SPI_HOST_INT]
#define LEGACY_NCP_SPI_HOST_INT_PORT             gpioPortA
#define LEGACY_NCP_SPI_HOST_INT_PIN              5

// [GPIO_LEGACY_NCP_SPI_HOST_INT]$

// <gpio> LEGACY_NCP_SPI_WAKE_INT
// $[GPIO_LEGACY_NCP_SPI_WAKE_INT]
#define LEGACY_NCP_SPI_WAKE_INT_PORT             gpioPortD
#define LEGACY_NCP_SPI_WAKE_INT_PIN              2

// [GPIO_LEGACY_NCP_SPI_WAKE_INT]$
// <<< sl:end pin_tool >>>

#endif // SL_SPIDRV_EXP_CONFIG_H
