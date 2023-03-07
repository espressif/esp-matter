/***************************************************************************//**
 * @brief Zigbee Smart Energy Registration component configuration header.
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

// <h>Zigbee Smart Energy Registration configuration

// <q EMBER_AF_PLUGIN_SMART_ENERGY_REGISTRATION_ESI_REDISCOVERY> Rediscover ESIs periodically
// <i> Default: TRUE
// <i> If set, the plugin will periodically rediscover ESIs in the network.
#define EMBER_AF_PLUGIN_SMART_ENERGY_REGISTRATION_ESI_REDISCOVERY   1

// <o EMBER_AF_PLUGIN_SMART_ENERGY_REGISTRATION_ESI_DISCOVERY_PERIOD> ESI discovery period (in hours) <3-24>
// <i> Default: 3
// <i> When ESI rediscovery is enabled, the plugin will rediscover ESIs in the network on this interval.
#define EMBER_AF_PLUGIN_SMART_ENERGY_REGISTRATION_ESI_DISCOVERY_PERIOD   3

// <q EMBER_AF_PLUGIN_SMART_ENERGY_REGISTRATION_ALLOW_NON_ESI_TIME_SERVERS> Allow non-ESI time servers
// <i> Default: TRUE
// <i> If set, any device with a time server can be considered as a time source candidate. Otherwise, only ESIs are considered.
#define EMBER_AF_PLUGIN_SMART_ENERGY_REGISTRATION_ALLOW_NON_ESI_TIME_SERVERS   1

// </h>

// <<< end of configuration section >>>
