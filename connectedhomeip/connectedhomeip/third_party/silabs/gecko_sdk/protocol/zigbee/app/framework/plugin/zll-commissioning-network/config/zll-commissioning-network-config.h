/***************************************************************************//**
 * @brief Zigbee ZLL Commissioning Network component configuration header.
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

// <h>Zigbee ZLL Commissioning Network configuration

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_NETWORK_JOINABLE_SCAN_TIMEOUT_MINUTES> Joinable scan timeout (minutes) <0-5>
// <i> Default: 1
// <i> The amount of time (in minutes) that the plugin will wait during a joinable network scan for the application to determine if the network joined to by the plugin is correct.  If the application leaves the network during this time, the plugin will automatically continue searching for other joinable networks.  If set to zero, the application will be unable to resume a joinable network search after successfully joining to a network.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_NETWORK_JOINABLE_SCAN_TIMEOUT_MINUTES   1

//  <a.8 EMBER_AF_PLUGIN_ZLL_COMMISSIONING_NETWORK_EXTENDED_PAN_ID> NETWORK_EXTENDED_PAN_ID <0..255> <f.h>
//  <d> {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_NETWORK_EXTENDED_PAN_ID { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

// </h>

// <<< end of configuration section >>>
