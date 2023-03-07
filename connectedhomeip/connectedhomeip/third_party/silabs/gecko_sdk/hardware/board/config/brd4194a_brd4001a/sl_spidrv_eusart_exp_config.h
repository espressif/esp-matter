/***************************************************************************//**
 * @file
 * @brief SPIDRV_EUSART Config
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

#ifndef SL_SPIDRV_EUSART_EXP_CONFIG_H
#define SL_SPIDRV_EUSART_EXP_CONFIG_H

#include "spidrv.h"

// <<< Use Configuration Wizard in Context Menu >>>
// <h> SPIDRV settings

// <o SL_SPIDRV_EUSART_EXP_BITRATE> SPI bitrate
// <i> Default: 1000000
#define SL_SPIDRV_EUSART_EXP_BITRATE           1000000

// <o SL_SPIDRV_EUSART_EXP_FRAME_LENGTH> SPI frame length <7-16>
// <i> Default: 8
#define SL_SPIDRV_EUSART_EXP_FRAME_LENGTH      8

// <o SL_SPIDRV_EUSART_EXP_TYPE> SPI mode
// <spidrvMaster=> Master
// <spidrvSlave=> Slave
#define SL_SPIDRV_EUSART_EXP_TYPE              spidrvMaster

// <o SL_SPIDRV_EUSART_EXP_BIT_ORDER> Bit order on the SPI bus
// <spidrvBitOrderLsbFirst=> LSB transmitted first
// <spidrvBitOrderMsbFirst=> MSB transmitted first
#define SL_SPIDRV_EUSART_EXP_BIT_ORDER         spidrvBitOrderMsbFirst

// <o SL_SPIDRV_EUSART_EXP_CLOCK_MODE> SPI clock mode
// <spidrvClockMode0=> SPI mode 0: CLKPOL=0, CLKPHA=0
// <spidrvClockMode1=> SPI mode 1: CLKPOL=0, CLKPHA=1
// <spidrvClockMode2=> SPI mode 2: CLKPOL=1, CLKPHA=0
// <spidrvClockMode3=> SPI mode 3: CLKPOL=1, CLKPHA=1
#define SL_SPIDRV_EUSART_EXP_CLOCK_MODE        spidrvClockMode0

// <o SL_SPIDRV_EUSART_EXP_CS_CONTROL> SPI master chip select (CS) control scheme.
// <spidrvCsControlAuto=> CS controlled by the SPI driver
// <spidrvCsControlApplication=> CS controlled by the application
#define SL_SPIDRV_EUSART_EXP_CS_CONTROL        spidrvCsControlAuto

// <o SL_SPIDRV_EUSART_EXP_SLAVE_START_MODE> SPI slave transfer start scheme
// <spidrvSlaveStartImmediate=> Transfer starts immediately
// <spidrvSlaveStartDelayed=> Transfer starts when the bus is idle (CS deasserted)
// <i> Only applies if instance type is spidrvSlave
#define SL_SPIDRV_EUSART_EXP_SLAVE_START_MODE  spidrvSlaveStartImmediate
// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <eusart signal=TX,RX,SCLK,(CS)> SL_SPIDRV_EUSART_EXP
// $[EUSART_SL_SPIDRV_EUSART_EXP]
#define SL_SPIDRV_EUSART_EXP_PERIPHERAL          EUSART0
#define SL_SPIDRV_EUSART_EXP_PERIPHERAL_NO       0

// EUSART0 TX on PC00
#define SL_SPIDRV_EUSART_EXP_TX_PORT             gpioPortC
#define SL_SPIDRV_EUSART_EXP_TX_PIN              0

// EUSART0 RX on PC01
#define SL_SPIDRV_EUSART_EXP_RX_PORT             gpioPortC
#define SL_SPIDRV_EUSART_EXP_RX_PIN              1

// EUSART0 SCLK on PC02
#define SL_SPIDRV_EUSART_EXP_SCLK_PORT           gpioPortC
#define SL_SPIDRV_EUSART_EXP_SCLK_PIN            2

// EUSART0 CS on PC03
#define SL_SPIDRV_EUSART_EXP_CS_PORT             gpioPortC
#define SL_SPIDRV_EUSART_EXP_CS_PIN              3

// [EUSART_SL_SPIDRV_EUSART_EXP]$
// <<< sl:end pin_tool >>>

#endif // SL_SPIDRV_EUSART_EXP_CONFIG_HEUSART_
