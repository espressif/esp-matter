/***************************************************************************//**
 * @file
 * @brief Si1133 Ambient Light and UV sensor configuration file
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

#ifndef SI1133_CONFIG_H
#define SI1133_CONFIG_H

#define SI1133_I2C_BUS_TIMEOUT          (1000)
#define SI1133_I2C_DEVICE_BUS_ADDRESS   (0xAA)

#define SI1133_I2C_DEVICE      (I2C1)
#define SI1133_SDA_PORT        gpioPortD
#define SI1133_SDA_PIN         2
#define SI1133_SCL_PORT        gpioPortD
#define SI1133_SCL_PIN         3

#endif // SI1133_CONFIG_H
