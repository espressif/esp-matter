/***************************************************************************//**
 * @file
 * @brief I2CSPM Config
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

#ifndef SL_I2CSPM_SENSOR_CONFIG_H
#define SL_I2CSPM_SENSOR_CONFIG_H

// <<< Use Configuration Wizard in Context Menu

// <h>I2CSPM settings

// <o SL_I2CSPM_SENSOR_REFERENCE_CLOCK> Reference clock frequency
// <i> Frequency in Hz of the reference clock.
// <i> Select 0 to use the frequency of the currently selected clock.
// <i> Default: 0
#define SL_I2CSPM_SENSOR_REFERENCE_CLOCK 0

// <o SL_I2CSPM_SENSOR_SPEED_MODE> Speed mode
// <0=> Standard mode (100kbit/s)
// <1=> Fast mode (400kbit/s)
// <2=> Fast mode plus (1Mbit/s)
// <i> Default: 0
#define SL_I2CSPM_SENSOR_SPEED_MODE      0
// </h> end I2CSPM config

// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <i2c signal=SCL,SDA> SL_I2CSPM_SENSOR
// $[I2C_SL_I2CSPM_SENSOR]
#define SL_I2CSPM_SENSOR_PERIPHERAL              I2C0
#define SL_I2CSPM_SENSOR_PERIPHERAL_NO           0

// I2C0 SCL on PC11
#define SL_I2CSPM_SENSOR_SCL_PORT                gpioPortC
#define SL_I2CSPM_SENSOR_SCL_PIN                 11
#define SL_I2CSPM_SENSOR_SCL_LOC                 15

// I2C0 SDA on PC10
#define SL_I2CSPM_SENSOR_SDA_PORT                gpioPortC
#define SL_I2CSPM_SENSOR_SDA_PIN                 10
#define SL_I2CSPM_SENSOR_SDA_LOC                 15

// [I2C_SL_I2CSPM_SENSOR]$
// <<< sl:end pin_tool >>>

#endif // SL_I2CSPM_SENSOR_CONFIG_H
