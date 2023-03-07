/***************************************************************************//**
 * @brief Zigbee ZLL Commissioning Common component configuration header.
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

// <h>Zigbee ZLL Commissioning Common configuration

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_PRIMARY_CHANNEL_MASK> Primary channel mask
// <i> Default: 0x02108800
// <i> Defines the primary channel set to be used for touchlink commissioning and related operations.  The primary channel set will always be used during touch linking or when searching for joinable, non-ZLL networks.  The Base Device Behaviour specification defines the primary channel set as consisting of channels 11, 15, 20 and 25.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_PRIMARY_CHANNEL_MASK 0x02108800

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SCAN_SECONDARY_CHANNELS> Scan secondary channels
// <i> Default: TRUE
// <i> If this option is enabled, the plugin will scan the secondary channel set if no networks are found on a primary channel during a touchlink operation, or when searching for joinable, non-ZLL networks. This option will increase the scanning time but will potentially find more devices or networks.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SCAN_SECONDARY_CHANNELS   1

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SECONDARY_CHANNEL_MASK> Secondary channel mask
// <i> Default: 0x05EF7000
// <i> Defines the secondary channel set to be used for touchlink commissioning and related operations. The Base Device Behaviour specification defines the secondary channel set as consisting of 12, 13, 14, 16, 17, 18, 19, 21, 22, 23, 24, and 26.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_SECONDARY_CHANNEL_MASK 0x05EF7000

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_RADIO_TX_POWER> Radio transmit power
// <i> Default: 3
// <i> The radio transmit power (in dBm) at which the node will operate.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_RADIO_TX_POWER 3

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_ZIGBEE3_SUPPORT> ZigBee 3.0 Support
// <i> Default: TRUE
// <i> Enabling this option (the default for Zigbee 3.0) will set the Profile Interop bit in the Touchlink information field for ZLL commissioning cluster commands for both initiators and targets, and will cause Scan Response and Device Information Response messages to use the actual profile id specified in the endpoint, rather than the legacy ZLL profile id.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON_ZIGBEE3_SUPPORT   1

// </h>

// <<< end of configuration section >>>
