/***************************************************************************//**
 * @brief Zigbee Events Server component configuration header.
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

// <h>Zigbee Events Server configuration

// <o EMBER_AF_PLUGIN_EVENTS_SERVER_TAMPER_LOG_SIZE> Tamper log size <0-255>
// <i> Default: 10
// <i> Maximum number of events stored in the tamper log. A value of 0 disables the log.
#define EMBER_AF_PLUGIN_EVENTS_SERVER_TAMPER_LOG_SIZE   10

// <o EMBER_AF_PLUGIN_EVENTS_SERVER_FAULT_LOG_SIZE> Fault log size <0-255>
// <i> Default: 10
// <i> Maximum number of events stored in the fault log. A value of 0 disables the log.
#define EMBER_AF_PLUGIN_EVENTS_SERVER_FAULT_LOG_SIZE   10

// <o EMBER_AF_PLUGIN_EVENTS_SERVER_GENERAL_LOG_SIZE> General log size <0-255>
// <i> Default: 10
// <i> Maximum number of events stored in the general log. A value of 0 disables the log.
#define EMBER_AF_PLUGIN_EVENTS_SERVER_GENERAL_LOG_SIZE   10

// <o EMBER_AF_PLUGIN_EVENTS_SERVER_SECURITY_LOG_SIZE> Security log size <0-255>
// <i> Default: 10
// <i> Maximum number of events stored in the security log. A value of 0 disables the log.
#define EMBER_AF_PLUGIN_EVENTS_SERVER_SECURITY_LOG_SIZE   10

// <o EMBER_AF_PLUGIN_EVENTS_SERVER_NETWORK_LOG_SIZE> Network log size <0-255>
// <i> Default: 10
// <i> Maximum number of events stored in the network log. A value of 0 disables the log.
#define EMBER_AF_PLUGIN_EVENTS_SERVER_NETWORK_LOG_SIZE   10

// <o EMBER_AF_PLUGIN_EVENTS_SERVER_EVENT_DATA_LENGTH> Event data length <0-255>
// <i> Default: 100
// <i> Maximum length of event data. A value of 0 indicates that no additional information is stored when an event occurs.
#define EMBER_AF_PLUGIN_EVENTS_SERVER_EVENT_DATA_LENGTH   100

// </h>

// <<< end of configuration section >>>
