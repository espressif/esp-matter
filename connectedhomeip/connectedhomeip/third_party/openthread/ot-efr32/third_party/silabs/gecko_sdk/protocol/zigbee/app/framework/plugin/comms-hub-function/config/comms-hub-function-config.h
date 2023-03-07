/***************************************************************************//**
 * @brief Zigbee Comms Hub Function (CHF)component configuration header.
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

// <h>Zigbee Comms Hub Function (CHF) configuration

// <o EMBER_AF_PLUGIN_COMMS_HUB_FUNCTION_COMMSHUB_TUNNEL_CLIENT_ENDPOINT> Comms Hub Tunneling Client Endpoint <1-255>
// <i> Default: 1
// <i> Tunneling client endpoint on the comms hub
#define EMBER_AF_PLUGIN_COMMS_HUB_FUNCTION_COMMSHUB_TUNNEL_CLIENT_ENDPOINT   1

// <o EMBER_AF_PLUGIN_COMMS_HUB_FUNCTION_GBZ_OFFSET> GBZ Offset <0-255>
// <i> Default: 8
// <i> Offset from the begin of a tunneled remote party message where the GBZ packet begins
#define EMBER_AF_PLUGIN_COMMS_HUB_FUNCTION_GBZ_OFFSET   8

// <o EMBER_AF_PLUGIN_COMMS_HUB_FUNCTION_TUNNEL_CHECK_PERIOD_SECONDS> Tunnel Check Period (seconds) <1-255>
// <i> Default: 10
// <i> This is the period of how often the Comms Hub checks to make sure that all ESME devices have tunnels initiated to them.
#define EMBER_AF_PLUGIN_COMMS_HUB_FUNCTION_TUNNEL_CHECK_PERIOD_SECONDS   10

// </h>

// <<< end of configuration section >>>
