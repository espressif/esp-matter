/***************************************************************************//**
 * @brief Zigbee Time Server Cluster component configuration header.
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

// <h>Zigbee Time Server Cluster configuration

// <q EMBER_AF_PLUGIN_TIME_SERVER_SUPERSEDING> Superseding bit
// <i> Default: FALSE
// <i> If this option is selected, the Superseding bit of the TimeStatus attribute will be set to indicate that the time server should be considered as a more authoritative time server. Warning: you should not set this unless your device can keep accurate time.
#define EMBER_AF_PLUGIN_TIME_SERVER_SUPERSEDING   0

// <q EMBER_AF_PLUGIN_TIME_SERVER_MASTER> Master bit
// <i> Default: FALSE
// <i> If this option is selected, the Master bit of the TimeStatus attribute will be set to indicate that the real time clock corresponding to the Time attribute is internally set to the time standard.
#define EMBER_AF_PLUGIN_TIME_SERVER_MASTER   0

// <q EMBER_AF_PLUGIN_TIME_SERVER_SYNCHRONIZED> Synchronized bit
// <i> Default: FALSE
// <i> If this option is selected and the Master bit option is not selected, the Synchronized bit of the TimeStatus attribute will be set to indicate that the time has been synchronized over the ZigBee network.  If the Master bit option is set, this option has no effect.
#define EMBER_AF_PLUGIN_TIME_SERVER_SYNCHRONIZED   0

// <q EMBER_AF_PLUGIN_TIME_SERVER_MASTER_ZONE_DST> Master Zone Dst bit
// <i> Default: FALSE
// <i> If this option is selected, the MasterZoneDst bit of the TimeStatus attribute will be set to indicate the TimeZone, DstStart, DstEnd, and DstShift attributes are set internally to correct values for the location of the clock.
#define EMBER_AF_PLUGIN_TIME_SERVER_MASTER_ZONE_DST   0

// </h>

// <<< end of configuration section >>>
