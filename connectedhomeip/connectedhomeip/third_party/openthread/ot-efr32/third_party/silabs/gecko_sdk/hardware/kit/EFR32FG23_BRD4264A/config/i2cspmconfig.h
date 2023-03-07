/***************************************************************************//**
 * @file
 * @brief I2CSPM driver configuration file
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

#ifndef I2CSPM_CONFIG_H
#define I2CSPM_CONFIG_H

/***************************************************************************//**
 * @addtogroup Drivers
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @addtogroup I2CSPM
 * @{
 ******************************************************************************/

/* I2C SPM driver config. This default override only works if one I2C interface
   is in use. If multiple interfaces are in use, define the peripheral setup
   inside the application in a I2CSPM_Init_TypeDef and then pass the initialization
   struct to I2CSPM_Init(). */
#define I2CSPM_INIT_DEFAULT                                                   \
  { I2C0,                      /* Use I2C instance 0 */                       \
    gpioPortC,                 /* SCL port */                                 \
    4,                         /* SCL pin */                                  \
    gpioPortC,                 /* SDA port */                                 \
    5,                         /* SDA pin */                                  \
    0,                         /* Use currently configured reference clock */ \
    I2C_FREQ_STANDARD_MAX,     /* Set to standard rate  */                    \
    i2cClockHLRStandard,       /* Set to use 4:4 low/high duty cycle */       \
  }

#define I2CSPM_TRANSFER_TIMEOUT 300000

/** @} (end addtogroup I2CSPM) */
/** @} (end addtogroup Drivers) */

#endif /* I2CSPM_CONFIG_H */
