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

// USART0 TX on PC6
#define LEGACY_NCP_SPI_TX_PORT                   gpioPortC
#define LEGACY_NCP_SPI_TX_PIN                    6
#define LEGACY_NCP_SPI_TX_LOC                    11

// USART0 RX on PC7
#define LEGACY_NCP_SPI_RX_PORT                   gpioPortC
#define LEGACY_NCP_SPI_RX_PIN                    7
#define LEGACY_NCP_SPI_RX_LOC                    11

// USART0 CLK on PB11
#define LEGACY_NCP_SPI_CLK_PORT                  gpioPortB
#define LEGACY_NCP_SPI_CLK_PIN                   11
#define LEGACY_NCP_SPI_CLK_LOC                   4

// USART0 CS on PF7
#define LEGACY_NCP_SPI_CS_PORT                   gpioPortF
#define LEGACY_NCP_SPI_CS_PIN                    7
#define LEGACY_NCP_SPI_CS_LOC                    28

// [USART_LEGACY_NCP_SPI]$

// <gpio> LEGACY_NCP_SPI_HOST_INT
// $[GPIO_LEGACY_NCP_SPI_HOST_INT]
#define LEGACY_NCP_SPI_HOST_INT_PORT             gpioPortD
#define LEGACY_NCP_SPI_HOST_INT_PIN              10

// [GPIO_LEGACY_NCP_SPI_HOST_INT]$

// <gpio> LEGACY_NCP_SPI_WAKE_INT
// $[GPIO_LEGACY_NCP_SPI_WAKE_INT]
#define LEGACY_NCP_SPI_WAKE_INT_PORT             gpioPortD
#define LEGACY_NCP_SPI_WAKE_INT_PIN              11

// [GPIO_LEGACY_NCP_SPI_WAKE_INT]$
// <<< sl:end pin_tool >>>

#endif // SL_SPIDRV_EXP_CONFIG_H
