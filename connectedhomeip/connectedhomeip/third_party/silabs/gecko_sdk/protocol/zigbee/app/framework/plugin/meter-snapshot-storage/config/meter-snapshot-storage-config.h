/***************************************************************************//**
 * @brief Zigbee Meter Snapshot Storage component configuration header.
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

// <h>Zigbee Meter Snapshot Storage configuration

// <o EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SNAPSHOT_CAPACITY> Snapshot storage capacity <1-254>
// <i> Default: 2
// <i> Used to specify the maximum number of snapshots to be stored by the meter server or mirror.
#define EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SNAPSHOT_CAPACITY   2

// <o EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SCHEDULE_CAPACITY> Snapshot schedule capacity <1-254>
// <i> Default: 2
// <i> Used to specify the maximum number of snapshot schedules to be stored by the meter server or mirror.
#define EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SCHEDULE_CAPACITY   2

// <o EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_MANUAL_SNAPSHOT_TYPE> Manual snapshot type <0-3>
// <i> Default: 0
// <i> Used to determine the type of snapshot to be taken when a TakeSnapshot command is issued
#define EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_MANUAL_SNAPSHOT_TYPE   0

// <o EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SUM_TIERS_SUPPORTED> Summation tiers supported <0-288>
// <i> Default: 2
// <i> Number of summation tiers supported for snapshot storage
#define EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SUM_TIERS_SUPPORTED   2

// <o EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_BLOCK_TIERS_SUPPORTED> Block tiers supported <0-1536>
// <i> Default: 2
// <i> Number of block tiers supported for snapshot storage
#define EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_BLOCK_TIERS_SUPPORTED   2

// </h>

// <<< end of configuration section >>>
