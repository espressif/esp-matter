/***************************************************************************//**
 * @file
 * @brief WFX host configuration and pinout
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef SL_WFX_HOST_BUS_PINOUT_H
#define SL_WFX_HOST_BUS_PINOUT_H

// <<< sl:start pin_tool >>>
// <gpio> SL_WFX_HOST_PINOUT_SPI_WIRQ
// $[GPIO_SL_WFX_HOST_PINOUT_SPI_WIRQ]
#define SL_WFX_HOST_PINOUT_SPI_WIRQ_PORT          gpioPortB
#define SL_WFX_HOST_PINOUT_SPI_WIRQ_PIN           6
// [GPIO_SL_WFX_HOST_PINOUT_SPI_WIRQ]$

// <usart signal=TX,RX,CLK,CS> SL_WFX_HOST_PINOUT_SPI
// $[USART_SL_WFX_HOST_PINOUT_SPI]
#define SL_WFX_HOST_PINOUT_SPI_PERIPHERAL         USART2
#define SL_WFX_HOST_PINOUT_SPI_PERIPHERAL_NO      2

#define SL_WFX_HOST_PINOUT_SPI_TX_PORT            gpioPortA
#define SL_WFX_HOST_PINOUT_SPI_TX_PIN             6
#define SL_WFX_HOST_PINOUT_SPI_TX_LOC             1

#define SL_WFX_HOST_PINOUT_SPI_RX_PORT            gpioPortA
#define SL_WFX_HOST_PINOUT_SPI_RX_PIN             7
#define SL_WFX_HOST_PINOUT_SPI_RX_LOC             1

#define SL_WFX_HOST_PINOUT_SPI_CLK_PORT           gpioPortA
#define SL_WFX_HOST_PINOUT_SPI_CLK_PIN            8
#define SL_WFX_HOST_PINOUT_SPI_CLK_LOC            1

#define SL_WFX_HOST_PINOUT_SPI_CS_PORT            gpioPortA
#define SL_WFX_HOST_PINOUT_SPI_CS_PIN             9
#define SL_WFX_HOST_PINOUT_SPI_CS_LOC             1
// [USART_SL_WFX_HOST_PINOUT_SPI]$
// <<< sl:end pin_tool >>>

#endif
