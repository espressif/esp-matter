/***************************************************************************//**
 * @brief Zigbee Tunneling Client Cluster component configuration header.
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

// <h>Zigbee Tunneling Client Cluster configuration

// <o EMBER_AF_PLUGIN_TUNNELING_CLIENT_TUNNEL_LIMIT> Tunnel Limit <1-255>
// <i> Default: 8
// <i> Maximum number of active tunnels supported by the plugin.
#define EMBER_AF_PLUGIN_TUNNELING_CLIENT_TUNNEL_LIMIT   8

// <o EMBER_AF_PLUGIN_TUNNELING_CLIENT_MAXIMUM_INCOMING_TRANSFER_SIZE> Maximum Incoming Transfer Size <1-65535>
// <i> Default: 128
// <i> The maximum size of an incoming tunnel message that the server can support.
#define EMBER_AF_PLUGIN_TUNNELING_CLIENT_MAXIMUM_INCOMING_TRANSFER_SIZE   128

// <o EMBER_AF_PLUGIN_TUNNELING_CLIENT_TIMEOUT_SECONDS> Timeout (in seconds) <1-255>
// <i> Default: 5
// <i> The amount of time, in seconds, the plugin will wait for a server to respond to a request to open a new tunnel.
#define EMBER_AF_PLUGIN_TUNNELING_CLIENT_TIMEOUT_SECONDS   5

// </h>

// <<< end of configuration section >>>
