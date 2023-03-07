/***************************************************************************//**
 * @file
 * @brief Bosch Sensortec BMP280 barometric pressure sensor configuration file
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

#ifndef BAP_CONFIG_H
#define BAP_CONFIG_H

#define BAP_I2C_BUS_TIMEOUT         (1000)
#define BAP_I2C_DEVICE              (I2C1)
#define BAP_I2C_BUS_ADDRESS         (0x77)

#define BAP_SDA_LOCATION     (I2C_ROUTELOC0_SDALOC_LOC17)
#define BAP_SCL_LOCATION     (I2C_ROUTELOC0_SCLLOC_LOC17)
#define BAP_SDA_LOC          17
#define BAP_SCL_LOC          17
#define BAP_SDA_PORT         gpioPortC
#define BAP_SDA_PIN          4
#define BAP_SCL_PORT         gpioPortC
#define BAP_SCL_PIN          5

#define I2CSPM_INIT_BAP                                                      \
  { BAP_DEVICE,               /* I2C instance                             */ \
    BAP_SCL_PORT,             /* SCL port                                 */ \
    BAP_SCL_PIN,              /* SCL pin                                  */ \
    BAP_SDA_PORT,             /* SDA port                                 */ \
    BAP_SDA_PIN,              /* SDA pin                                  */ \
    BAP_SCL_LOC,              /* Port location of SCL signal              */ \
    BAP_SDA_LOC,              /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // BAP_CONFIG_H
