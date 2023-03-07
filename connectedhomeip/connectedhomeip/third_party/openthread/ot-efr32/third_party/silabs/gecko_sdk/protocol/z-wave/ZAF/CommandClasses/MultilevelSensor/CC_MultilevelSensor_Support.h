/***************************************************************************//**
 * @file CC_MultilevelSensor_Support.h
 * @brief CC_MultilevelSensor_Support.h
 * @copyright 2020 Silicon Laboratories Inc.
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
#ifndef CC_MULTILEVELSENSOR_SUPPORT_H
#define CC_MULTILEVELSENSOR_SUPPORT_H
// -----------------------------------------------------------------------------
//                   Includes
// -----------------------------------------------------------------------------
#include "CC_MultilevelSensor_Configuration.h"
// -----------------------------------------------------------------------------
//                Macros and Typedefs
// -----------------------------------------------------------------------------
#define MULTILEVEL_SENSOR_DEFAULT_AUTOREPORT_PEDIOD_SEC (MULTILEVEL_SENSOR_DEFAULT_AUTOREPORT_PEDIOD_MIN*60)
#define MULTILEVEL_SENSOR_DEFAULT_AUTOREPORT_PEDIOD_MS  (MULTILEVEL_SENSOR_DEFAULT_AUTOREPORT_PEDIOD_SEC*1000)
// -----------------------------------------------------------------------------
//                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//              Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * Initializes all registered sensor interfaces and starts a
 * periodic timer which will report the registered sensors' measured values.
 */
void cc_multilevel_sensor_init(void);

/**
 * This function will report the registered sensor's measured datas
 * to the Lifeline group.
 */
void cc_multilevel_sensor_send_sensor_data();

#endif  // CC_MULTILEVELSENSOR_SUPPORT_H
