/***************************************************************************//**
 * @brief Zigbee Packet Handoff component configuration header.
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

// <h>Zigbee Packet Handoff configuration

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ALL_PACKETS> Handoff All Packets
// <i> Default: TRUE
// <i> Allow all packets
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ALL_PACKETS   1

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_RAW_MAC> Handoff Raw Mac
// <i> Default: FALSE
// <i> Allow raw mac
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_RAW_MAC   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_MAC_COMMAND> Handoff Mac Commands
// <i> Default: FALSE
// <i> Allow mac command
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_MAC_COMMAND   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA> Handoff Network Data
// <i> Default: FALSE
// <i> Allow network data
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_DATA   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND> Handoff Network Commands
// <i> Default: FALSE
// <i> Allow network command
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_NETWORK_COMMAND   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_DATA> Handoff APS Data
// <i> Default: FALSE
// <i> Allow aps data
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_DATA   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_COMMAND> Handoff APS Commands
// <i> Default: FALSE
// <i> Allow aps command
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_APS_COMMAND   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZDO> Handoff ZDO Commands
// <i> Default: FALSE
// <i> Allow zdo
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZDO   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZCL> Handoff ZCL Commands
// <i> Default: FALSE
// <i> Allow zcl
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_ZCL   0

// <q EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_BEACON> Handoff Beacons
// <i> Default: FALSE
// <i> Allow beacon
#define EMBER_AF_PLUGIN_PACKET_HANDOFF_ALLOW_BEACON   0

// <o PACKET_HANDOFF_BUFFER_SIZE> Packet Handoff Buffer Size <128-512>
// <i> Default: 256
// <i> Handoff buffer size
#define PACKET_HANDOFF_BUFFER_SIZE   256

// </h>

// <<< end of configuration section >>>
