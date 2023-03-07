/***************************************************************************//**
 * @file
 * @brief Si7210 Hall Effect Sensor configuration file
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
