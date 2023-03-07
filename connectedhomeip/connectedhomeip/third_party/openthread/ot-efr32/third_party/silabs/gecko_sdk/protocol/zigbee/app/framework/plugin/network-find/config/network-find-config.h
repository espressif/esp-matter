/***************************************************************************//**
 * @brief Zigbee Network Find component configuration header.
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

// <h>Zigbee Network Find configuration

// <o EMBER_AF_PLUGIN_NETWORK_FIND_CHANNEL_MASK> Channel mask
// <i> Default: 0x0318C800
// <i> The channels that the plugin will preferentially scan when forming and joining.
#define EMBER_AF_PLUGIN_NETWORK_FIND_CHANNEL_MASK  0x0318C800UL

// <q EMBER_AF_PLUGIN_NETWORK_FIND_ENABLE_ALL_CHANNELS> Enable scanning all channels
// <i> Default: FALSE
// <i> Enable scanning all channels when forming and joining if none of the preferred channels are found suitable. Use emberAfPluginNetworkFindGetEnableScanningAllChannelsCallback() to override this option.
#define EMBER_AF_PLUGIN_NETWORK_FIND_ENABLE_ALL_CHANNELS   0

// <o EMBER_AF_PLUGIN_NETWORK_FIND_CUT_OFF_VALUE> Cut-off value (dBm) <-128..127>
// <i> Default: -48
// <i> The maximum noise allowed on a channel to consider for forming a network. If the noise on all preferred channels is above this limit and "Enable scanning all channels" is ticked, the scan continues on all channels. Use emberAfPluginNetworkFindGetEnergyThresholdForChannelCallback() to override this value.
#define EMBER_AF_PLUGIN_NETWORK_FIND_CUT_OFF_VALUE   -48

// <o EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_POWER> Radio output power <-8..20>
// <i> Default: 3
// <i> The radio output power (in dBm) at which the node will operate. Enabling getting the power from a callback overrides this option.
#define EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_POWER   3

// <q EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_CALLBACK> Get radio output power from callback
// <i> Default: FALSE
// <i> Use a tx power callback to determine radio tx power.
#define EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_CALLBACK   0

// <o EMBER_AF_PLUGIN_NETWORK_FIND_DURATION> Scan duration exponent <0-14>
// <i> Default: 5
// <i> The exponent of the number of scan periods, where a scan period is 960 symbols, and a symbol is 16 miscroseconds.  The scan will occur for ((2^duration) + 1) scan periods.
#define EMBER_AF_PLUGIN_NETWORK_FIND_DURATION   5

//  <a.8 EMBER_AF_PLUGIN_NETWORK_FIND_EXTENDED_PAN_ID> NETWORK_FIND_EXTENDED_PAN_ID <0..255> <f.h>
//  <d> {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define EMBER_AF_PLUGIN_NETWORK_FIND_EXTENDED_PAN_ID { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// <o EMBER_AF_PLUGIN_NETWORK_FIND_JOINABLE_SCAN_TIMEOUT_MINUTES> Joinable scan timeout (minutes) <0-5>
// <i> Default: 1
// <i> The amount of time (in minutes) that the plugin will wait during a joinable network scan for the application to determine if the network joined to by the plugin is correct.  If the application leaves the network during this time, the plugin will automatically continue searching for other joinable networks.  If set to zero, the application will be unable to resume a joinable network search after successfully joining to a network.
#define EMBER_AF_PLUGIN_NETWORK_FIND_JOINABLE_SCAN_TIMEOUT_MINUTES   1

// </h>

// <<< end of configuration section >>>
