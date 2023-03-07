/***************************************************************************//**
 * @file
 * @brief SPI abstraction used by memory lcd display
 *******************************************************************************
 * # License
 * <b>Copyright 2022 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SL_MEMLCD_CONFIG_H
#define SL_MEMLCD_CONFIG_H

// <<< sl:start pin_tool >>>
// <usart signal=TX,CLK> SL_MEMLCD_SPI
// $[USART_SL_MEMLCD_SPI]
#warning "Memory LCD SPI peripheral not configured"
// #define SL_MEMLCD_SPI_PERIPHERAL     USART1
// #define SL_MEMLCD_SPI_PERIPHERAL_NO  1

// #define SL_MEMLCD_SPI_CLK_LOC        11
// #define SL_MEMLCD_SPI_CLK_PORT       gpioPortC
// #define SL_MEMLCD_SPI_CLK_PIN        8
// #define SL_MEMLCD_SPI_TX_LOC         11
// #define SL_MEMLCD_SPI_TX_PORT        gpioPortC
// #define SL_MEMLCD_SPI_TX_PIN         6
// [USART_SL_MEMLCD_SPI]$

// <gpio> SL_MEMLCD_SPI_CS
// $[GPIO_SL_MEMLCD_SPI_CS]
#warning "Memory LCD SPI CS pin not configured"
// #define SL_MEMLCD_SPI_CS_PORT        gpioPortD
// #define SL_MEMLCD_SPI_CS_PIN         14
// [GPIO_SL_MEMLCD_SPI_CS]$

// <gpio optional=true> SL_MEMLCD_EXTCOMIN
// $[GPIO_SL_MEMLCD_EXTCOMIN]
// #define SL_MEMLCD_EXTCOMIN_PORT      gpioPortD
// #define SL_MEMLCD_EXTCOMIN_PIN       13
// [GPIO_SL_MEMLCD_EXTCOMIN]$

// <<< sl:end pin_tool >>>

#endif
