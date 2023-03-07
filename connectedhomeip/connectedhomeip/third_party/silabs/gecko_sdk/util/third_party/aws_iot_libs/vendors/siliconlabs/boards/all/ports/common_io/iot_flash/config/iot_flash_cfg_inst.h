/***************************************************************************//**
 * @file    iot_flash_cfg_inst.h
 * @brief   Common I/O flash instance configurations.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#ifndef _IOT_FLASH_CFG_INSTANCE_H_
#define _IOT_FLASH_CFG_INSTANCE_H_

/*******************************************************************************
 *                         Flash Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> Flash General Options

// <o IOT_FLASH_CFG_INSTANCE_INST_NUM> Instance number
// <i> Instance number used when iot_flash_open() is called.
// <i> Default: 0
#define IOT_FLASH_CFG_INSTANCE_INST_NUM              0

// <o IOT_FLASH_CFG_INSTANCE_INST_TYPE> Instance type
// <0=> Internal Flash (MSC)
// <1=> External Flash (SPI)
// <i> Specify whether this instance is for internal flash (MSC)
//     or an external SPI flash. If external, then you need to setup
//     SPI configs below.
// <i> Default: 0
#define IOT_FLASH_CFG_INSTANCE_INST_TYPE             0

// </h>

// <h> SPI Configuration

// <o  IOT_FLASH_CFG_INSTANCE_SPI_BITRATE> Default SPI bitrate
// <i> Default: 1000000
#define IOT_FLASH_CFG_INSTANCE_SPI_BITRATE           1000000

// <o  IOT_FLASH_CFG_INSTANCE_SPI_FRAME_LENGTH> Default SPI frame length <4-16>
// <i> Default: 8
#define IOT_FLASH_CFG_INSTANCE_SPI_FRAME_LENGTH      8

// <o  IOT_FLASH_CFG_INSTANCE_SPI_TYPE> Default SPI master/slave mode
// <spidrvMaster=> Master
// <spidrvSlave=> Slave
#define IOT_FLASH_CFG_INSTANCE_SPI_TYPE              spidrvMaster

// <o  IOT_FLASH_CFG_INSTANCE_SPI_BIT_ORDER> Default SPI bit order
// <spidrvBitOrderLsbFirst=> LSB transmitted first
// <spidrvBitOrderMsbFirst=> MSB transmitted first
#define IOT_FLASH_CFG_INSTANCE_SPI_BIT_ORDER         spidrvBitOrderMsbFirst

// <o  IOT_FLASH_CFG_INSTANCE_SPI_CLOCK_MODE> Default SPI clock mode
// <spidrvClockMode0=> SPI mode 0: CLKPOL=0, CLKPHA=0
// <spidrvClockMode1=> SPI mode 1: CLKPOL=0, CLKPHA=1
// <spidrvClockMode2=> SPI mode 2: CLKPOL=1, CLKPHA=0
// <spidrvClockMode3=> SPI mode 3: CLKPOL=1, CLKPHA=1
#define IOT_FLASH_CFG_INSTANCE_SPI_CLOCK_MODE        spidrvClockMode0

// <o  IOT_FLASH_CFG_INSTANCE_SPI_CS_CONTROL> Default SPI CS control scheme
// <spidrvCsControlAuto=> CS controlled by the SPI driver
// <spidrvCsControlApplication=> CS controlled by the application
#define IOT_FLASH_CFG_INSTANCE_SPI_CS_CONTROL        spidrvCsControlApplication

// <o  IOT_FLASH_CFG_INSTANCE_SPI_SLAVE_START_MODE> Default SPI transfer scheme
// <spidrvSlaveStartImmediate=> Transfer starts immediately
// <spidrvSlaveStartDelayed=> Transfer starts when the bus is idle
#define IOT_FLASH_CFG_INSTANCE_SPI_SLAVE_START_MODE  spidrvSlaveStartImmediate

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                   EXTERNAL FLASH: H/W PERIPHERAL CONFIG
 ******************************************************************************/

// <<< sl:start pin_tool >>>
// <usart signal=TX,RX,CLK,CS> IOT_FLASH_CFG_INSTANCE_SPI
// $[USART_IOT_FLASH_CFG_INSTANCE_SPI]
// #define IOT_FLASH_CFG_INSTANCE_SPI_PERIPHERAL      USART0
// #define IOT_FLASH_CFG_INSTANCE_SPI_PERIPHERAL_NO   0

// #define IOT_FLASH_CFG_INSTANCE_SPI_TX_PORT         gpioPortA
// #define IOT_FLASH_CFG_INSTANCE_SPI_TX_PIN          10
// #define IOT_FLASH_CFG_INSTANCE_SPI_TX_LOC          2

// #define IOT_FLASH_CFG_INSTANCE_SPI_RX_PORT         gpioPortA
// #define IOT_FLASH_CFG_INSTANCE_SPI_RX_PIN          11
// #define IOT_FLASH_CFG_INSTANCE_SPI_RX_LOC          2

// #define IOT_FLASH_CFG_INSTANCE_SPI_CLK_PORT        gpioPortA
// #define IOT_FLASH_CFG_INSTANCE_SPI_CLK_PIN         12
// #define IOT_FLASH_CFG_INSTANCE_SPI_CLK_LOC         2

// #define IOT_FLASH_CFG_INSTANCE_SPI_CS_PORT         gpioPortA
// #define IOT_FLASH_CFG_INSTANCE_SPI_CS_PIN          13
// #define IOT_FLASH_CFG_INSTANCE_SPI_CS_LOC          2
// [USART_IOT_FLASH_CFG_INSTANCE_SPI]$
// <<< sl:end pin_tool >>>

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_FLASH_CFG_INSTANCE_H_ */
