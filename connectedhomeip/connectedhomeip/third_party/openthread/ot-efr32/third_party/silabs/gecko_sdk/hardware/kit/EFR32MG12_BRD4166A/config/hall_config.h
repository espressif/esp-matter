/***************************************************************************//**
 * @file
 * @brief Si7210 Hall Effect Sensor configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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
#ifndef HALL_CONFIG_H
#define HALL_CONFIG_H

#define HALL_GPIO_PORT_OUT   gpioPortB
#define HALL_GPIO_PIN_OUT    11

#define SI7210_I2C_BUS_TIMEOUT  1000
#define SI7210_I2C_BUS_ADDRESS  0x30
#define SI7210_DEVICE_ID        0x13
#define SI7210_CHIP_ID          0x01
#define SI7210_REV_ID_MIN       0x03
#define SI7210_REV_ID_MAX       0x04

#define SI7210_I2C_DEVICE       (I2C1)
#define SI7210_SDA_LOCATION     (I2C_ROUTELOC0_SDALOC_LOC8)
#define SI7210_SCL_LOCATION     (I2C_ROUTELOC0_SCLLOC_LOC8)
#define SI7210_SDA_LOC          8
#define SI7210_SCL_LOC          8
#define SI7210_SDA_PORT         gpioPortB
#define SI7210_SDA_PIN          8
#define SI7210_SCL_PORT         gpioPortB
#define SI7210_SCL_PIN          9

#define I2CSPM_INIT_SI7210                                                   \
  { SI7210_I2C_DEVICE,        /* I2C instance                             */ \
    SI7210_SCL_PORT,          /* SCL port                                 */ \
    SI7210_SCL_PIN,           /* SCL pin                                  */ \
    SI7210_SDA_PORT,          /* SDA port                                 */ \
    SI7210_SDA_PIN,           /* SDA pin                                  */ \
    SI7210_SCL_LOC,           /* Port location of SCL signal              */ \
    SI7210_SDA_LOC,           /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // HALL_CONFIG_H
