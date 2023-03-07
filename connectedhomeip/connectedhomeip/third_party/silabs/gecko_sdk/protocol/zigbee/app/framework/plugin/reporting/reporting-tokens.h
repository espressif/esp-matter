/***************************************************************************//**
 * @file
 * @brief Tokens for the Reporting plugin.
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
#ifdef UC_BUILD
#include "reporting-config.h"
#endif

#ifdef UC_BUILD
#if (EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE == 1)
#define EXPANDED_TABLE
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE
#define EXPANDED_TABLE
#endif
#endif // UC_BUILD

#ifdef EXPANDED_TABLE
#else
#define CREATOR_REPORT_TABLE  (0x8725)
// This key is used for an indexed token and the subsequent 0x7F keys are also reserved
#define NVM3KEY_REPORT_TABLE (NVM3KEY_DOMAIN_ZIGBEE | 0x4000)

#ifdef DEFINETYPES
// Include or define any typedef for tokens here
#endif //DEFINETYPES
#ifdef DEFINETOKENS
// Define the actual token storage information here

// Following is for backward compatibility.
// The default reporting will generate a table that is mandatory
// but user may still allocate some table for adding more reporting over
// the air or by cli as part of reporting plugin.
#if defined EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE
#define REPORT_TABLE_SIZE (EMBER_AF_GENERATED_REPORTING_CONFIG_DEFAULTS_TABLE_SIZE + EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#else
#define REPORT_TABLE_SIZE (EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE)
#endif

DEFINE_INDEXED_TOKEN(REPORT_TABLE,
                     EmberAfPluginReportingEntry,
                     REPORT_TABLE_SIZE,
                     { EMBER_ZCL_REPORTING_DIRECTION_REPORTED,
                       EMBER_AF_PLUGIN_REPORTING_UNUSED_ENDPOINT_ID })

#endif //DEFINETOKENS
#endif //EXPANDED_TABLE
