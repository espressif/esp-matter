/***************************************************************************//**
 * @brief Zigbee Device Query Service component configuration header.
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

// <h>Zigbee Device Query Service configuration

// <o EMBER_AF_PLUGIN_DEVICE_QUERY_SERVICE_MAX_FAILURES> Max Failures (per device) <1-5>
// <i> Default: 3
// <i> The max failures for a particular device before the plugin will give up and not try again.
#define EMBER_AF_PLUGIN_DEVICE_QUERY_SERVICE_MAX_FAILURES   3

// <q EMBER_AF_PLUGIN_DEVICE_QUERY_SERVICE_AUTO_START> Auto Start
// <i> Default: TRUE
// <i> Automatically start the service on boot.
#define EMBER_AF_PLUGIN_DEVICE_QUERY_SERVICE_AUTO_START   1

// </h>

// <<< end of configuration section >>>
