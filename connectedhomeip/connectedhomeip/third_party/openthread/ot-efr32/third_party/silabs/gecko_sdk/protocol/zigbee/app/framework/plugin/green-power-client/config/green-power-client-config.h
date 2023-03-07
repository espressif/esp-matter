/***************************************************************************//**
 * @brief Zigbee Green Power Client component configuration header.
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

// <h>Zigbee Green Power Client configuration

// <o EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_GPP_COMMISSIONING_WINDOW> Proxy Commissioning Window (secs) <1-65535>
// <i> Default: 160
// <i> Maximum amount of time (secs) the proxy is allowed to be in commissioning mode.
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_GPP_COMMISSIONING_WINDOW        160

// <o EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_GPP_DUPLICATE_TIMEOUT_SEC> Proxy Duplicate Timeout (secs) <1-65535>
// <i> Default: 160
// <i> Maximum amount of time (secs) the proxy keeps information on the received GPDF in order to filter out duplicates.
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_GPP_DUPLICATE_TIMEOUT_SEC       160

// <o EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_ADDR_ENTRIES> Maximum number of addresses stored for duplicate filtering <1-255>
// <i> Default: 3
// <i> Maximum number of GP addresses the plugin will store for duplicate filtering
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_ADDR_ENTRIES                3

// <o EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_SEQ_NUM_ENTRIES_PER_ADDR> Maximum number of saved random sequence numbers per address <1-255>
// <i> Default: 4
// <i> Maximum number of saved random sequence numbers per address used for duplicate filtering
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_MAX_SEQ_NUM_ENTRIES_PER_ADDR    4

// <q EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_ENABLE_BIDIRECTIONAL_OPERATION> Enable Advanced GPP feature of bidirectional operation
// <i> Default: FALSE
// <i> This enables advanced proxy feature of bidirectional operational message. This option will allow the proxy to make its gp stub tx queue availability in the Gp Notification. This is not implemented in a Proxy Basic.
#define EMBER_AF_PLUGIN_GREEN_POWER_CLIENT_ENABLE_BIDIRECTIONAL_OPERATION  0

// </h>

// <<< end of configuration section >>>
