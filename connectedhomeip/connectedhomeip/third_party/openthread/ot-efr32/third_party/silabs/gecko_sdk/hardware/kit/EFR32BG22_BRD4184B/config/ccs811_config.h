/***************************************************************************//**
 * @file
 * @brief Cambridge CMOS Sensors CCS811 gas sensor configuration file
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

#ifndef CCS811_CONFIG_H
#define CCS811_CONFIG_H

#define CCS811_I2C_BUS_TIMEOUT (1000)
#define CCS811_BUS_ADDRESS     (0xB4)
#define CCS811_FIRMWARE_UPDATE (0)
#define CCS811_HW_ID           (0x20)

#define CCS811_I2C_DEVICE      (I2C1)
#define CCS811_SDA_LOCATION    (I2C_ROUTELOC0_SDALOC_LOC6)
#define CCS811_SCL_LOCATION    (I2C_ROUTELOC0_SCLLOC_LOC6)
#define CCS811_SDA_LOC         6
#define CCS811_SCL_LOC         6
#define CCS811_SDA_PORT        gpioPortB
#define CCS811_SDA_PIN         6
#define CCS811_SCL_PORT        gpioPortB
#define CCS811_SCL_PIN         7

#define I2CSPM_INIT_CCS811                                                   \
  { CCS811_I2C_DEVICE,        /* I2C instance                             */ \
    CCS811_SCL_PORT,          /* SCL port                                 */ \
    CCS811_SCL_PIN,           /* SCL pin                                  */ \
    CCS811_SDA_PORT,          /* SDA port                                 */ \
    CCS811_SDA_PIN,           /* SDA pin                                  */ \
    CCS811_SCL_LOC,           /* Port location of SCL signal              */ \
    CCS811_SDA_LOC,           /* Port location of SDA signal              */ \
    0,                        /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,    /* Set to standard rate                     */ \
    i2cClockHLRStandard,      /* Set to use 4:4 low/high duty cycle       */ \
  }

#endif // CCS811_CONFIG_H
