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

/***************************************************************************//**
 * @addtogroup Debug
 * @{
 * @brief Debug Component
 * @details
 *   This component provides the bootloader with support for debugging functions.
 *   The component implements two types of debugging functionality:
 *   * Defining SL_DEBUG_ASSERT enables assertions on compile-time
 *     configurable parameters in the bootloader
 *   * Defining SL_DEBUG_PRINT enables debug prints at strategic
 *     points in the code.
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
// <dbg signal=SWV> SL_DEBUG
// $[DBG_SL_DEBUG]
/// SWO Debug Peripheral.
#define SL_DEBUG_PERIPHERAL                     DBG

/// SWO Debug Port.
#define SL_DEBUG_SWV_PORT                       gpioPortF
/// SWO Debug Pin.
#define SL_DEBUG_SWV_PIN                        2
/// SWO Debug Location
#define SL_DEBUG_SWV_LOC                        0
// [DBG_SL_DEBUG]$
// <<< sl:end pin_tool >>>

/**
 * @} (end addtogroup Debug)
 */

#endif // BTL_DEBUG_CONFIG_H