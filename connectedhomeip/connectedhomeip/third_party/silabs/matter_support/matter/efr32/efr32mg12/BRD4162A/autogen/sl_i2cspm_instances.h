/***************************************************************************//**
 * @file
 * @brief I2C simple poll-based master mode driver instances
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
 
#ifndef SL_I2CSPM_INSTANCES_H
#define SL_I2CSPM_INSTANCES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "sl_i2cspm.h"


#define SL_I2CSPM_SENSOR_PRESENT


extern sl_i2cspm_t *sl_i2cspm_sensor;

void sl_i2cspm_init_instances(void);

#ifdef __cplusplus
}
#endif

#endif // SL_I2CSPM_INSTANCES_H
