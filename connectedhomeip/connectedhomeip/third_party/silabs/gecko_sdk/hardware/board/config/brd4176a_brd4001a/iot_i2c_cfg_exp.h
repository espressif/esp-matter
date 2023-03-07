/***************************************************************************//**
 * @file    IOT_I2C_CFG_EXP_inst.h
 * @brief   Common I/O I2C instance configuration.
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

#ifndef _IOT_I2C_CFG_EXP_H_
#define _IOT_I2C_CFG_EXP_H_

/*******************************************************************************
 *                          I2C Default Configs
 ******************************************************************************/

// <<< Use Configuration Wizard in Context Menu >>>

// <h> I2C General Options

// <o IOT_I2C_CFG_EXP_INST_NUM> Instance number
// <i> Instance number used when iot_i2c_open() is called.
// <i> Default: 0
#define IOT_I2C_CFG_EXP_INST_NUM           0

// <o  IOT_I2C_CFG_EXP_DEFAULT_TIMEOUT> Default timeout (in msec)
// <i> Default: 500
#define IOT_I2C_CFG_EXP_DEFAULT_TIMEOUT    500

// <o  IOT_I2C_CFG_EXP_DEFAULT_FREQ> Default bus speed
// <100000=> Standard mode
// <400000=> Fast mode
// <1000000=> Fast plus mode
// <3400000=> High speed mode
// <i> Default: 400000
#define IOT_I2C_CFG_EXP_DEFAULT_FREQ       400000

// <q  IOT_I2C_CFG_EXP_ACCEPT_NACK> Accept NACK
// <i> If the driver receives NACK during a transfer, the transfer is halted
//     immediately but it is not considered as an error. Instead, the driver
//     returns success status (useful for test purposes).
// Default: 0
#define IOT_I2C_CFG_EXP_ACCEPT_NACK        0

// </h>

// <<< end of configuration section >>>

/*******************************************************************************
 *                        H/W PERIPHERAL CONFIG
 ******************************************************************************/

// <<< sl:start pin_tool >>>

// <gpio optional=true> IOT_I2C_CFG_EXP_ENABLE
// $[GPIO_IOT_I2C_CFG_EXP_ENABLE]
#define IOT_I2C_CFG_EXP_ENABLE_PORT              gpioPortD
#define IOT_I2C_CFG_EXP_ENABLE_PIN               2

// [GPIO_IOT_I2C_CFG_EXP_ENABLE]$

// <i2c signal=SCL,SDA> IOT_I2C_CFG_EXP
// $[I2C_IOT_I2C_CFG_EXP]
#define IOT_I2C_CFG_EXP_PERIPHERAL               I2C1
#define IOT_I2C_CFG_EXP_PERIPHERAL_NO            1

// I2C1 SCL on PC04
#define IOT_I2C_CFG_EXP_SCL_PORT                 gpioPortC
#define IOT_I2C_CFG_EXP_SCL_PIN                  4

// I2C1 SDA on PC05
#define IOT_I2C_CFG_EXP_SDA_PORT                 gpioPortC
#define IOT_I2C_CFG_EXP_SDA_PIN                  5

// [I2C_IOT_I2C_CFG_EXP]$

// <<< sl:end pin_tool >>>

/*******************************************************************************
 *                              SAFE GUARD
 ******************************************************************************/

#endif /* _IOT_I2C_CFG_EXP_H_ */
