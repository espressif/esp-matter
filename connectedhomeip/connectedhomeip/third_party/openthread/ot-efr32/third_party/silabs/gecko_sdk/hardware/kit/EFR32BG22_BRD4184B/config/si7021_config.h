/***************************************************************************//**
 * @file
 * @brief Si7021 I2C Humidity and Temperature Sensor configuration file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef SI7021_CONFIG_H
#define SI7021_CONFIG_H

#define SI7021_I2C_BUS_ADDRESS     (0x40)
#define SI7021_DEVICE_ID           (0x15)

#define SI7021_I2C_DEVICE      (I2C1)
#define SI7021_SDA_LOCATION    (I2C_ROUTELOC0_SDALOC_LOC17)
#define SI7021_SCL_LOCATION    (I2C_ROUTELOC0_SCLLOC_LOC17)
#define SI7021_SDA_LOC         17
#define SI7021_SCL_LOC         17
#define SI7021_SDA_PORT        gpioPortD
#define SI7021_SDA_PIN         2
#define SI7021_SCL_PORT        gpioPortD
#define SI7021_SCL_PIN         3

#define I2CSPM_INIT_SI7021                                                   \
  { SI7021_I2C_DEVICE,        /* I2C instance                             */ \
    SI7021_SCL_PORT,          /* SCL port                                 */ \
    SI7021_SCL_PIN,           /* SCL pin                                  */ \
    SI7021_SDA_PORT,          /* SDA port                                 */ \
    SI7021_SDA_PIN,           /* SDA pin                                  */ \
    SI7021_SCL_LOC,           /* Port location of SCL signal              */ \
    SI7021_SDA_LOC,           /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // SI7021_CONFIG_H
