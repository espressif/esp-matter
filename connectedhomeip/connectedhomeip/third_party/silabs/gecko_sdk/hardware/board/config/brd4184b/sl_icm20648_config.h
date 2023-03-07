/***************************************************************************//**
 * @file
 * @brief ICM20648 Config
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

#ifndef SL_ICM20648_CONFIG_H
#define SL_ICM20648_CONFIG_H

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,CLK> SL_ICM20648_SPI
// $[USART_SL_ICM20648_SPI]
#define SL_ICM20648_SPI_PERIPHERAL               USART0
#define SL_ICM20648_SPI_PERIPHERAL_NO            0

// USART0 TX on PC00
#define SL_ICM20648_SPI_TX_PORT                  gpioPortC
#define SL_ICM20648_SPI_TX_PIN                   0

// USART0 RX on PC01
#define SL_ICM20648_SPI_RX_PORT                  gpioPortC
#define SL_ICM20648_SPI_RX_PIN                   1

// USART0 CLK on PC02
#define SL_ICM20648_SPI_CLK_PORT                 gpioPortC
#define SL_ICM20648_SPI_CLK_PIN                  2

// [USART_SL_ICM20648_SPI]$

// <gpio> SL_ICM20648_SPI_CS
// $[GPIO_SL_ICM20648_SPI_CS]
#define SL_ICM20648_SPI_CS_PORT                  gpioPortB
#define SL_ICM20648_SPI_CS_PIN                   2

// [GPIO_SL_ICM20648_SPI_CS]$

// <gpio optional=true> SL_ICM20648_INT
// $[GPIO_SL_ICM20648_INT]
#define SL_ICM20648_INT_PORT                     gpioPortA
#define SL_ICM20648_INT_PIN                      0

// [GPIO_SL_ICM20648_INT]$

// <<< sl:end pin_tool >>>

#endif // SL_ICM20648_CONFIG_H
