/***************************************************************************//**
 * @brief Zigbee Prepayment Snapshot Storage component configuration header.
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

// <h>Zigbee Prepayment Snapshot Storage configuration

// <o EMBER_AF_PLUGIN_PREPAYMENT_SNAPSHOT_STORAGE_SNAPSHOT_CAPACITY> Snapshot capacity <1-254>
// <i> Default: 5
// <i> Specifies the maximum number of snapshots to be stored by the meter server or mirror.
#define EMBER_AF_PLUGIN_PREPAYMENT_SNAPSHOT_STORAGE_SNAPSHOT_CAPACITY   5

// <o EMBER_AF_PLUGIN_PREPAYMENT_SNAPSHOT_STORAGE_SCHEDULE_CAPACITY> Schedule capacity <1-254>
// <i> Default: 5
// <i> Specifies the maximum number of snapshot schedules to be stored by the meter server or mirror.
#define EMBER_AF_PLUGIN_PREPAYMENT_SNAPSHOT_STORAGE_SCHEDULE_CAPACITY   5

// </h>

// <<< end of configuration section >>>
