/***************************************************************************//**
 * @brief Zigbee ZLL Commissioning Server component configuration header.
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

// <h>Zigbee ZLL Commissioning Server configuration

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_RX_ON_AT_STARTUP_PERIOD> Rx On at Startup time (in seconds) <0-3600>
// <i> Default: 300
// <i> The duration, in seconds, for which the plugin will keep the radio receiver switched on at application startup, in order to listen for incoming scan requests. A duration of zero will disable this feature.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_RX_ON_AT_STARTUP_PERIOD   300

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_RX_ALWAYS_ON> Rx always on
// <i> Default: FALSE
// <i> Indicates whether the radio receiver should be permanently on, when the device is available for touchlinking. (Note that the radio will still be switched off for a sleepy end device while it is joined to a network)
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_RX_ALWAYS_ON   0

// <o EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_DEFAULT_RADIO_CHANNEL> Default radio channel <11-26>
// <i> Default: 11
// <i> The radio channel used by a factory new device to listen for incoming scan requests, and, if the device is a router, to form a new network on receipt of a Network Start request.
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_DEFAULT_RADIO_CHANNEL   11

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_STEALING_ALLOWED> Stealing Allowed
// <i> Default: TRUE
// <i> Indicates whether a non-factory-new device may be touchlinked. (Note, in conformance with the Zigbee 3.0 Base Device Behaviour specification, all joined devices are considered non-factory-new from a touchlink point of view, including devices which automatically form a network on startup, e.g. by using emberZllFormNetwork. For such devices, stealing may be allowed by default at startup, but then subsequently forbidden on a successful touchlink, by calling emberZllSetPolicy.)
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_STEALING_ALLOWED   1

// <q EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_REMOTE_RESET_ALLOWED> Reset Allowed
// <i> Default: TRUE
// <i> Indicates whether a non-factory-new device may be reset remotely. (Note, in conformance with the Zigbee 3.0 Base Device Behaviour specification, all joined devices are considered non-factory-new from a touchlink point of view, including devices which automatically form a network on startup, e.g. by using emberZllFormNetwork. For such devices, remote reset may be allowed by default at startup, but then subsequently forbidden on a successful touchlink, by calling emberZllSetPolicy.)
#define EMBER_AF_PLUGIN_ZLL_COMMISSIONING_SERVER_REMOTE_RESET_ALLOWED   1

// </h>

// <<< end of configuration section >>>
