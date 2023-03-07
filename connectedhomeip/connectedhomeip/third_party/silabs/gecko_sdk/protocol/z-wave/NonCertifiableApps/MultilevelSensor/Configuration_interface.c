 /***************************************************************************//**
 * @file Configuration_interface.c
 * @brief Configuration_interface.c
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
// -----------------------------------------------------------------------------
//                   Includes
// -----------------------------------------------------------------------------
#include <string.h>
#include <stdbool.h>
#include <ZW_typedefs.h>
#include <ZAF_file_ids.h>
#include <CC_Configuration.h>
#include "Configuration_interface.h"
// -----------------------------------------------------------------------------
//                Macros and Typedefs
// -----------------------------------------------------------------------------
#define ZAF_FILE_ID_CC_CONFIGURATION_WITH_OFFSET(x) (ZAF_FILE_ID_CC_CONFIGURATION_BASE + x)
#define SIZE_OF_PARAMETER_POOL(x)                   (sizeof(x)/sizeof(x[0]))
// -----------------------------------------------------------------------------
//              Static Function Declarations
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Global Variables
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//                Static Variables
// -----------------------------------------------------------------------------
static const cc_config_parameter_metadata_t parameter_pool[] = {
  {
    .number            = configuration_index_minimum_celsius_temperature_limit,
    .next_number       = configuration_index_maximum_celsius_temperature_limit,
    .file_id           = ZAF_FILE_ID_CC_CONFIGURATION_WITH_OFFSET(0),
    .migration_handler = NULL,

    .attributes = {
      .name                        = "Minimum temperature limit",
      .info                        = "Minimum temperature limit celsius",
      .format                      = CC_CONFIG_PARAMETER_FORMAT_SIGNED_INTEGER,
      .size                        = CC_CONFIG_PARAMETER_SIZE_32_BIT,
      .min_value.as_int32          =  -100,
      .max_value.as_int32          =    50,

      .default_value.as_int32 = 10,

      .flags.read_only             = false,
      .flags.altering_capabilities = false,
      .flags.advanced              = false,
    },
  },
  {
    .number            = configuration_index_maximum_celsius_temperature_limit,
    .next_number       = 0x0000,
    .file_id           = ZAF_FILE_ID_CC_CONFIGURATION_WITH_OFFSET(1),
    .migration_handler = NULL,

    .attributes = {
      .name                        = "Maximum temperature limit",
      .info                        = "Maximum temperature limit celsius",
      .format                      = CC_CONFIG_PARAMETER_FORMAT_SIGNED_INTEGER,
      .size                        = CC_CONFIG_PARAMETER_SIZE_32_BIT,
      .min_value.as_int32          =  10,
      .max_value.as_int32          = 100,

      .default_value.as_int32 = 35,

      .flags.read_only             = false,
      .flags.altering_capabilities = false,
      .flags.advanced              = false,
    },
  },
};

static const cc_configuration_t default_configuration = {
  .numberOfParameters = 2,
  .parameters         = &parameter_pool[0]
};

// -----------------------------------------------------------------------------
//              Public Function Definitions
// -----------------------------------------------------------------------------

cc_configuration_t const*
configuration_get_configuration_pool(void)
{
  return &default_configuration;
}
// -----------------------------------------------------------------------------
//              Static Function Definitions
// -----------------------------------------------------------------------------


