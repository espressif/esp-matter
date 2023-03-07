/***************************************************************************//**
 * @brief Zigbee Meter Mirror component configuration header.
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

// <h>Zigbee Meter Mirror configuration

// <o EMBER_AF_PLUGIN_METER_MIRROR_METER_CLIENT_ENDPOINT> Meter Client Cluster Endpoint <1-254>
// <i> Default: 1
// <i> The endpoint where the Meter Client resides and where the mirror will receive reports.  This is also the location of the Physical Environment attribute that indicates mirror capacity.
#define EMBER_AF_PLUGIN_METER_MIRROR_METER_CLIENT_ENDPOINT   1

// <o EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START> Mirror Endpoint Start <1-254>
// <i> Default: 2
// <i> The start of a set of sequential endpoints where the mirror data will be located.  In other words, the Meter Server Endpoints.
#define EMBER_AF_PLUGIN_METER_MIRROR_ENDPOINT_START   2

// <o EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS> Max Number of Mirrors <1-5>
// <i> Default: 1
// <i> The maximum number of mirrors (and corresponding) endpoints, that are supported.
#define EMBER_AF_PLUGIN_METER_MIRROR_MAX_MIRRORS   1

// </h>

// <<< end of configuration section >>>
