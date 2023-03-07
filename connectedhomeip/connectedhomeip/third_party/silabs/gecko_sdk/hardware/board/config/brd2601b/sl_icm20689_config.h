/*****************************CS**********************************************//**
 * @file
 * @brief ICM20689 Config
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

#ifndef SL_ICM20689_CONFIG_H
#define SL_ICM20689_CONFIG_H

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,SCLK,CS> SL_ICM20689_SPI_EUSART
// $[EUSART_SL_ICM20689_SPI_EUSART]
#define SL_ICM20689_SPI_EUSART_PERIPHERAL        EUSART1
#define SL_ICM20689_SPI_EUSART_PERIPHERAL_NO     1

// EUSART1 TX on PC03
#define SL_ICM20689_SPI_EUSART_TX_PORT           gpioPortC
#define SL_ICM20689_SPI_EUSART_TX_PIN            3

// EUSART1 RX on PC02
#define SL_ICM20689_SPI_EUSART_RX_PORT           gpioPortC
#define SL_ICM20689_SPI_EUSART_RX_PIN            2

// EUSART1 SCLK on PC01
#define SL_ICM20689_SPI_EUSART_SCLK_PORT         gpioPortC
#define SL_ICM20689_SPI_EUSART_SCLK_PIN          1

// EUSART1 CS on PA07
#define SL_ICM20689_SPI_EUSART_CS_PORT           gpioPortA
#define SL_ICM20689_SPI_EUSART_CS_PIN            7

// [EUSART_SL_ICM20689_SPI_EUSART]$

// <gpio optional=true> SL_ICM20689_INT
// $[GPIO_SL_ICM20689_INT]
#define SL_ICM20689_INT_PORT                     gpioPortB
#define SL_ICM20689_INT_PIN                      1

// [GPIO_SL_ICM20689_INT]$
// <<< sl:end pin_tool >>>

#endif // SL_ICM20689_CONFIG_H
