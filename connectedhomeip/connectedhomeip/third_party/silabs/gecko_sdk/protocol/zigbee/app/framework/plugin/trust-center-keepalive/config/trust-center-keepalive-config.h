/***************************************************************************//**
 * @brief Zigbee Trust Center Keepalive component configuration header.
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

// <h>Zigbee Trust Center Keepalive configuration

// <o EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_DEFAULT_BASE_PERIOD_MINUTES> Default Keepalive Base (in minutes) <1-255>
// <i> Default: 2
// <i> If the device does not know the "TC Keep-Alive Base" attribute of the Trust Center (for example if a reboot has occurred), it will fall back on this value.
#define EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_DEFAULT_BASE_PERIOD_MINUTES   2

// <o EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_DEFAULT_JITTER_PERIOD_SECONDS> Default Jitter Period (in seconds) <0-512>
// <i> Default: 60
// <i> If the device does not know the "TC Keep-Alive Jitter" attribute of the Trust Center (for example if a reboot has occurred), it will fall back on this value.
#define EMBER_AF_PLUGIN_TRUST_CENTER_KEEPALIVE_DEFAULT_JITTER_PERIOD_SECONDS   60

// </h>

// <<< end of configuration section >>>
