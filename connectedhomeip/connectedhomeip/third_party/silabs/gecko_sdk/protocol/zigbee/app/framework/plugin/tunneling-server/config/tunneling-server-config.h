/***************************************************************************//**
 * @brief Zigbee Tunneling Server Cluster component configuration header.
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

// <h>Zigbee Tunneling Server Cluster configuration

// <o EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT> Tunnel Limit <1-255>
// <i> Default: 8
// <i> Maximum number of active tunnels supported by the plugin.
#define EMBER_AF_PLUGIN_TUNNELING_SERVER_TUNNEL_LIMIT   8

// <o EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSE_TUNNEL_TIMEOUT> Close Tunnel Timeout <1-65535>
// <i> Default: 65535
// <i> Minimum number of seconds that the plugin waits before closing an inactive tunnel.
#define EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSE_TUNNEL_TIMEOUT   65535

// <o EMBER_AF_PLUGIN_TUNNELING_SERVER_MAXIMUM_INCOMING_TRANSFER_SIZE> Maximum Incoming Transfer Size <1-65535>
// <i> Default: 128
// <i> The maximum size of an incoming tunnel message that the server can support.
#define EMBER_AF_PLUGIN_TUNNELING_SERVER_MAXIMUM_INCOMING_TRANSFER_SIZE   128

// <q EMBER_AF_PLUGIN_TUNNELING_SERVER_TEST_PROTOCOL_SUPPORT> Enable Support for the Test Protocol (199)
// <i> Default: FALSE
// <i> This enables support in the plugin for the Test Protocol (protocol 199). This is an echo protocol from the server which helps test two-way communications easily.
#define EMBER_AF_PLUGIN_TUNNELING_SERVER_TEST_PROTOCOL_SUPPORT   0

// <q EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSURE_NOTIFICATION_SUPPORT> Enable Support for sending Closure Nofication Messages
// <i> Default: FALSE
// <i> This enables support in the plugin for Closure Notification Messages. Since the client still needs to re-open, you may not want to support this optional message.
#define EMBER_AF_PLUGIN_TUNNELING_SERVER_CLOSURE_NOTIFICATION_SUPPORT   0

// </h>

// <<< end of configuration section >>>
