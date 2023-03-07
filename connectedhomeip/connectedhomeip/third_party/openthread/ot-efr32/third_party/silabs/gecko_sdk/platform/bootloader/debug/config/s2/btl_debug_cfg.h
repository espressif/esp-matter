/***************************************************************************//**
 * @file
 * @brief Configuration header for bootloader debug
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc.  Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement.  This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 *
 ******************************************************************************/

#ifndef BTL_DEBUG_CONFIG_H
#define BTL_DEBUG_CONFIG_H

// <<< Use Configuration Wizard in Context Menu >>>
// <h> Bootloader Debug Configuration

// <q SL_DEBUG_PRINT> Debug prints
// <i> Default: 0
// <i> Print debug messages to SWO.
#ifndef SL_DEBUG_PRINT
/// Print debug message to SWO.
#define SL_DEBUG_PRINT                    0
#endif

// <q SL_DEBUG_ASSERT> Debug asserts
// <i> Default: 0
// <i> Enable assertions in the source code
/// Enable Assert in source code.
#define SL_DEBUG_ASSERT                    0

// </h>
// <<< end of configuration section >>>

// <<< sl:start pin_tool >>>
// <gpio signal=SWV> SL_DEBUG
// $[GPIO_SL_DEBUG]
/// SWO Debug Peripheral.
#define SL_DEBUG_PERIPHERAL                     GPIO

/// SWO Debug Port.
#define SL_DEBUG_SWV_PORT                       gpioPortA
/// SWO Debug Pin.
#define SL_DEBUG_SWV_PIN                        3
// [GPIO_SL_DEBUG]$
// <<< sl:end pin_tool >>>

#endif // BTL_DEBUG_CONFIG_H