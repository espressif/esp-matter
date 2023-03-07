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
#warning "ICM20648 SPI peripheral not configured"
// #define SL_ICM20648_SPI_PERIPHERAL                  USART3
// #define SL_ICM20648_SPI_PERIPHERAL_NO               3

// // USART3 TX on PC0
// #define SL_ICM20648_SPI_TX_PORT                     gpioPortC
// #define SL_ICM20648_SPI_TX_PIN                      0
// #define SL_ICM20648_SPI_TX_LOC                      18

// // USART3 RX on PC1
// #define SL_ICM20648_SPI_RX_PORT                     gpioPortC
// #define SL_ICM20648_SPI_RX_PIN                      1
// #define SL_ICM20648_SPI_RX_LOC                      18

// // USART3 CLK on PC2
// #define SL_ICM20648_SPI_CLK_PORT                    gpioPortC
// #define SL_ICM20648_SPI_CLK_PIN                     2
// #define SL_ICM20648_SPI_CLK_LOC                     18

// [USART_SL_ICM20648_SPI]$

// <gpio> SL_ICM20648_SPI_CS
// $[GPIO_SL_ICM20648_SPI_CS]
#warning "ICM20648 SPI CS pin not configured"
// #define SL_ICM20648_SPI_CS_PORT                     gpioPortC
// #define SL_ICM20648_SPI_CS_PIN                      3
// [GPIO_SL_ICM20648_SPI_CS]$

// <gpio optional=true> SL_ICM20648_INT
// $[GPIO_SL_ICM20648_INT]
// #define SL_ICM20648_INT_PORT                        gpioPortF
// #define SL_ICM20648_INT_PIN                         12
// [GPIO_SL_ICM20648_INT]$

// <<< sl:end pin_tool >>>

#endif // SL_ICM20648_CONFIG_H
