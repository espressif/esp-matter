/***************************************************************************//**
 * @brief Zigbee Green Power Server component configuration header.
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

// <h>Zigbee Green Power Server configuration

// <o EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_PROXY_ZCL_MESSAGE_SRC_ENDPOINT> Hidden ZCL Message Proxy Endpoint <1-240>
// <i> Default: 1
// <i> This is a local endpoint is used to send/receive
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_HIDDEN_PROXY_ZCL_MESSAGE_SRC_ENDPOINT   1

// <o EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT> ZCL Message Default Destination Endpoint <1-240>
// <i> Default: 1
// <i> This is the default destination endpoint for all translated ZCL messages from GPDF commands.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_ZCL_MESSAGE_DST_ENDPOINT   1

// <o EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES> Number of GPD instances during bidirectional commissioning. <1-20>
// <i> Default: 1
// <i> Number of of GPD instances and there commissioning states to be maintained during bi-directional commissioning.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES   1

// <o EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_TIMEOUT_IN_SEC> Multi Sensor Commissioning Timeout in seconds <1-255>
// <i> Default: 20
// <i> Timeout to close the multi-sensor commissioning.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_TIMEOUT_IN_SEC   20

// <o EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_BUFFER_SIZE> Multi Sensor Commissioning Buffer Size <1-255>
// <i> Default: 1
// <i> Multi sensor commissioning buffer size to hold a complete one application description buffer.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_BUFFER_SIZE   1

// <o EMBER_AF_PLUGIN_GREEN_POWER_SERVER_GENERIC_SWITCH_COMMISSIONING_TIMEOUT_IN_SEC> Generic Switch Commissioning Timeout in seconds <1-255>
// <i> Default: 60
// <i> Timeout to close the generic switch commissioning.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_GENERIC_SWITCH_COMMISSIONING_TIMEOUT_IN_SEC   60

// <q EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS> On SOC platform, store the table in persistent memory
// <i> Default: 1
// <i> On an SOC platform, this option enables the persistent storage of the sink table into the FLASH memory using the tokens.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS   1

// <q EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE> If user has a default translation table
// <i> Default: 0
// <i> In green power server, user can specfy if a default translation table to be used for command forwarding.
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USER_HAS_DEFAULT_TRANSLATION_TABLE 0
// </h>

// <<< end of configuration section >>>
