/***************************************************************************//**
 * @brief Zigbee ZLL Identify Server component configuration header.
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

// <h>Zigbee ZLL Identify Server configuration

// <o EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_EVENT_DELAY> Event delay <1-65535>
// <i> Default: 1024
// <i> Used to specify the delay between trigger effect events. Specified in milliseconds.
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_EVENT_DELAY   1024

// <o EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_BLINK_EVENTS> Blink events <1-255>
// <i> Default: 2
// <i> Used to specify the number events associated with the Blink trigger effect. Should be even, but not required.
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_BLINK_EVENTS   2

// <o EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_BREATHE_EVENTS> Breathe events <1-255>
// <i> Default: 4
// <i> Used to specify the number events associated with the Breathe trigger effect. Should be even, but not required.
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_BREATHE_EVENTS   4

// <o EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_OKAY_EVENTS> Okay events <1-255>
// <i> Default: 6
// <i> Used to specify the number events associated with the Okay trigger effect. Should be even, but not required.
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_OKAY_EVENTS   6

// <o EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_CHANNEL_CHANGE_EVENTS> Channel change events <1-255>
// <i> Default: 8
// <i> Used to specify the number events associated with the Channel Change trigger effect. Should be even, but not required.
#define EMBER_AF_PLUGIN_ZLL_IDENTIFY_SERVER_CHANNEL_CHANGE_EVENTS   8

// </h>

// <<< end of configuration section >>>
