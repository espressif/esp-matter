/***************************************************************************//**
 * @brief Zigbee ZLL Commissioning Client component configuration header.
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

// <h>Zigbee ZLL Commissioning Client configuration

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_TOUCH_LINK_SECONDS_DELAY> Touch link delay time (in seconds) <1-255>
// <i> Default: 1
// <i> The duration, in seconds, for which the plugin will wait before sending a network form or start to a target device.  The user may abort the touch link procedure during this delay if, for example, the wrong target device has been selected.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_TOUCH_LINK_SECONDS_DELAY   1

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_SUB_DEVICE_TABLE_SIZE> Sub-device table size <1-255>
// <i> Default: 1
// <i> The maximum number of sub-devices (endpoints) discovered on remote nodes.  The plugin will attempt to retrieve device information records from each sub-device on the remote node during touch linking, up to this limit.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_SUB_DEVICE_TABLE_SIZE   1

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_IDENTIFY_DURATION> ZLL identify duration <1-65535>
// <i> Default: 65535
// <i> The amount of time (in seconds) the recipient is to remain in identify mode when an identify request command frame, used to request the recipient to identify itself in some application specific way to aid with touchlinking, is received. If set to 0 (0x0000), the recipient will exit identify mode. If set to 65535 (0xFFFF), the recipient will remain in identify mode for a default time known by the receiver. The field is set to 65535 (0xFFFF) by default.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_IDENTIFY_DURATION   65535

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_SCAN_POWER_LEVEL> Scan Power Level <-8..20>
// <i> Default: 0
// <i> The power level used to transmit the scan request messages by the initiator device during touch linking.  The Base Device Behaviour specification requires a nominal output power of 0dBm during touch linking.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_CLIENT_SCAN_POWER_LEVEL   3

// </h>

// <<< end of configuration section >>>
