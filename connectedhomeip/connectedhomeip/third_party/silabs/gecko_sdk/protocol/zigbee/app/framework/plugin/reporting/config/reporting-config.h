/***************************************************************************//**
 * @brief Zigbee Reporting component configuration header.
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

// <h>Zigbee Reporting configuration

// <q EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE> Enable expanded reporting table size
// <i> Default: FALSE
// <i> Select for configurations greater than 127 entries.  For SoC applications this requires EFR32 architecture and the NVM3 plugin
#define EMBER_AF_PLUGIN_REPORTING_ENABLE_EXPANDED_TABLE   0

// <o EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE> Reporting table size <1-127>
// <i> Default: 5
// <i> Maximum number of entries in the reporting table.
#define EMBER_AF_PLUGIN_REPORTING_TABLE_SIZE   5

// <o EMBER_AF_PLUGIN_REPORTING_EXPANDED_TABLE_SIZE> Expanded reporting table size <1-1024>
// <i> Default: 20
// <i> Maximum number of entries in the expanded reporting table.
#define EMBER_AF_PLUGIN_REPORTING_EXPANDED_TABLE_SIZE   20

// <q EMBER_AF_PLUGIN_REPORTING_ENABLE_GROUP_BOUND_REPORTS> Allow reports to send via group bindings (zigbee 3.0 mandatory behavior)
// <i> Default: TRUE
// <i> This feature is enabled by default to satisfy zigbee 3.0 compliance. Network commissioners should handle reports over group bindings with caution as the frequency and number of reports over multicasts can stagnate the network. Multicasts are treated as broadcasts, which consume network bandwidth.
#define EMBER_AF_PLUGIN_REPORTING_ENABLE_GROUP_BOUND_REPORTS   1

// </h>

// <<< end of configuration section >>>
