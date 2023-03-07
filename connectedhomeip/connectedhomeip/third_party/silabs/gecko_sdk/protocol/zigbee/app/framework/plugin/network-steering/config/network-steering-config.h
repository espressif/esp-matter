/***************************************************************************//**
 * @brief Zigbee Network Steering component configuration header.
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

// <h>Zigbee Network Steering configuration

// <q EMBER_AF_PLUGIN_NETWORK_STEERING_CHANNEL_MASK> Channel Mask
// <i> Default: 0x0318C800
// <i> The channels that the plugin will scan when forming and joining.
#define EMBER_AF_PLUGIN_NETWORK_STEERING_CHANNEL_MASK       0x0318C800

// <o EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_POWER> Radio output power <-8..20>
// <i> Default: 3
// <i> The radio output power (in dBm) at which the node will operate.
#define EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_POWER   3

// <q EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_CALLBACK> Get radio output power from callback
// <i> Default: FALSE
// <i> Use a tx power callback to determine radio tx power.
#define EMBER_AF_PLUGIN_NETWORK_STEERING_RADIO_TX_CALLBACK   0

// <o EMBER_AF_PLUGIN_NETWORK_STEERING_SCAN_DURATION> Scan duration exponent <0-14>
// <i> Default: 4
// <i> The exponent of the number of scan periods, where a scan period is 960 symbols, and a symbol is 16 miscroseconds.  The scan will occur for ((2^duration) + 1) scan periods.
#define EMBER_AF_PLUGIN_NETWORK_STEERING_SCAN_DURATION   4

// <o EMBER_AF_PLUGIN_NETWORK_STEERING_COMMISSIONING_TIME_S> Network steering commissioning time (seconds) <180-255>
// <i> Default: 180
// <i> The amount of time (in seconds) that a node will open its network when performing network steering for node on a network, or at the end of the network steering process if a node is a router
#define EMBER_AF_PLUGIN_NETWORK_STEERING_COMMISSIONING_TIME_S   180

// <q EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS> Optimize scans
// <i> Default: TRUE
// <i> When this option is enabled, this plugin begins by scanning all channels, both primary and secondary channels, and caches all the beacons received. Networks are joined using the cached beacons for each network steering state (use install code based key, use well-known key, use distributed key). Enabling this option may reduce the number of scans significantly, since the legacy steering code would scan each channel every time it ran a steering state.
#define EMBER_AF_PLUGIN_NETWORK_STEERING_OPTIMIZE_SCANS   1

// <q EMBER_AF_PLUGIN_NETWORK_STEERING_TRY_ALL_KEYS> Try all keys at once
// <i> Default: FALSE
// <i> When enabled, network steering will attempt to join each network using the install code derived key, centralized key, and distributed key at the same time, rather than trying each key individually. Users that wish to prioritize joining using install code keys should not check this option.
#define EMBER_AF_PLUGIN_NETWORK_STEERING_TRY_ALL_KEYS   0

// </h>

// <<< end of configuration section >>>
