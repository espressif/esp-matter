/***************************************************************************//**
 * @brief Zigbee Gas Proxy Function (GPF) component configuration header.
 *\n*******************************************************************************
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

// <<< Use Configuration Wizard in Context Menu >>>

// <h>Zigbee Gas Proxy Function (GPF) configuration

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_REMOTE_COMMSHUB_ENDPOINT> Remote CommsHub Endpoint <1-255>
// <i> Default: 3
// <i> This is a local endpoint representing the Remote CommsHub.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_REMOTE_COMMSHUB_ENDPOINT   3

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MIRROR_ENDPOINT> Meter Mirror Endpoint <1-255>
// <i> Default: 2
// <i> This is a local mirror endpoint representing the meter.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MIRROR_ENDPOINT   2

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_ESI_ENDPOINT> Meter ESI Endpoint <1-255>
// <i> Default: 1
// <i> This is a local endpoint representing the ESI.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_ESI_ENDPOINT   1

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_HIDDEN_CLIENT_SIDE_ENDPOINT> Hidden Client Side Endpoint <1-255>
// <i> Default: 4
// <i> This endpoint should implement all necessary cluster for the success operation / processing of TOM commands.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_HIDDEN_CLIENT_SIDE_ENDPOINT   4

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_PROFILE_DATA_LOG_ENTRIES> Max Profile Data Log Entries <1-65535>
// <i> Default: 19056
// <i> Maximum number of entries in the GPF Profile Data Log.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_PROFILE_DATA_LOG_ENTRIES   19056

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_DAILY_CONSUMPTION_LOG_ENTRIES> Max Daily Consumption Log Entries <1-65535>
// <i> Default: 731
// <i> Maximum number of entries in the GPF Daily Consumption Log.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_DAILY_CONSUMPTION_LOG_ENTRIES   731

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_DAILY_READ_LOG_ENTRIES> Max Daily Read Log Entries <1-255>
// <i> Default: 31
// <i> Maximum number of entries in the GPF Daily Read Log.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_DAILY_READ_LOG_ENTRIES   31

// <o EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_PREPAYMENT_DAILY_READ_LOG_ENTRIES> Max Prepayment Daily Read Log Entries <1-255>
// <i> Default: 31
// <i> Maximum number of entries in the GPF Prepayment Daily Read Log.
#define EMBER_AF_PLUGIN_GAS_PROXY_FUNCTION_MAX_PREPAYMENT_DAILY_READ_LOG_ENTRIES   31

// </h>

// <<< end of configuration section >>>
