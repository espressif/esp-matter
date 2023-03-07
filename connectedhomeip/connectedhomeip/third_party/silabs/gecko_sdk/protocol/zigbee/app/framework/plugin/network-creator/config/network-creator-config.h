/***************************************************************************//**
 * @brief Zigbee Network Creator component configuration header.
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

// <h>Zigbee Network Creator configuration

// <o EMBER_AF_PLUGIN_NETWORK_CREATOR_SCAN_DURATION> Scan duration exponent <0-14>
// <i> Default: 4
// <i> The exponent of the number of scan periods, where a scan period is 960 symbols, and a symbol is 16 miscroseconds.  The scan will occur for ((2^duration) + 1) scan periods.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_SCAN_DURATION   4

// <o EMBER_AF_PLUGIN_NETWORK_CREATOR_CHANNEL_MASK> Channel Mask
// <i> Default: 0x02108800
// <i> The channels that the plugin will scan when forming and joining.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_CHANNEL_MASK  0x02108800

// <o EMBER_AF_PLUGIN_NETWORK_CREATOR_CHANNEL_BEACONS_THRESHOLD> Channel beacons threshold <0-255>
// <i> Default: 20
// <i> This is the highest number of beacons that the network creator will hear on a channel and still try to form a network. If the network creator hears more than this ammount of beacons on one channel, it will disregard this channel from the formation process.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_CHANNEL_BEACONS_THRESHOLD   20

// <o EMBER_AF_PLUGIN_NETWORK_CREATOR_RADIO_POWER> Radio power to use on the formed network. <-8..20>
// <i> Default: 3
// <i> This is the default radio power that will be used on the network. The units are in dBm.
#define EMBER_AF_PLUGIN_NETWORK_CREATOR_RADIO_POWER   3

// </h>

// <<< end of configuration section >>>
