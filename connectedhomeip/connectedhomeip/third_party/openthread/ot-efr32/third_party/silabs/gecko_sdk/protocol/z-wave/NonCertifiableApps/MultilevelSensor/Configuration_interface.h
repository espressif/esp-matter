/***************************************************************************//**
 * @file Configuration_interface.h
 * @brief Configuration_interface.h
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
#ifndef _CONFIGURATION_INTERFACE_H
#define _CONFIGURATION_INTERFACE_H
// -----------------------------------------------------------------------------
//                                   Includes
// -----------------------------------------------------------------------------
#include <stddef.h>
#include <stdbool.h>
#include "CC_Configuration.h"
// -----------------------------------------------------------------------------
//                              Macros and Typedefs
// -----------------------------------------------------------------------------
/**
 * Enumeration of the configuration parameters' numbers
 */
typedef enum {
  configuration_index_minimum_celsius_temperature_limit = 1,
  configuration_index_maximum_celsius_temperature_limit,
  configuration_index_count
} configuration_index;

// -----------------------------------------------------------------------------
//                                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                          Public Function Declarations
// -----------------------------------------------------------------------------

/**
 * Gets the io interface reference
 *
 * @return Returns pointer to the default factory configuration
 */
cc_configuration_t const*
configuration_get_configuration_pool(void);

#endif  // CONFIGURATION_INTERFACE_H
