/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Meter Snapshot Storage plugin, which handles
 *        snapshots of meter states in a network.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifdef UC_BUILD
#include "meter-snapshot-storage-config.h"
#endif // UC_BUILD

/**
 * @defgroup meter-snapshot-storage  Meter Snapshot Storage
 * @ingroup component
 * @brief API and Callbacks for the meter-snapshot-storage Component
 *
 */

/**
 * @addtogroup meter-snapshot-storage
 * @{
 */

#define INVALID_SNAPSHOT_SCHEDULE_ID 0

// 0 is technically valid, but we'll designate it to be our "invalid" id for init purposes
#define INVALID_SNAPSHOT_ID 0

#define SNAPSHOT_SCHEDULE_PAYLOAD_SIZE 15

#define SNAPSHOT_CAUSE_MANUAL 0x00000400

#define SUMMATION_TIERS EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_SUM_TIERS_SUPPORTED
#define BLOCK_TIERS  EMBER_AF_PLUGIN_METER_SNAPSHOT_STORAGE_BLOCK_TIERS_SUPPORTED
#define SNAPSHOT_PAYLOAD_SIZE SUMMATION_TIERS + BLOCK_TIERS + 94

typedef struct {
  uint8_t tierSummation[SUMMATION_TIERS * 6];
  uint8_t tierBlockSummation[BLOCK_TIERS * 6];
  uint8_t currentSummation[6];
  uint32_t billToDate;
  uint32_t billToDateTimeStamp;
  uint32_t projectedBill;
  uint32_t projectedBillTimeStamp;
  uint32_t snapshotId;
  uint32_t snapshotTime;
  uint32_t snapshotCause;
  EmberNodeId requestingId;
  uint8_t tiersInUse;
  uint8_t tiersAndBlocksInUse;
  uint8_t srcEndpoint;
  uint8_t dstEndpoint;
  uint8_t billTrailingDigit;
  uint8_t payloadType;
} EmberAfSnapshotPayload;

typedef struct {
  uint32_t snapshotStartDate;
  uint32_t snapshotSchedule;
  uint32_t snapshotCause;
  EmberNodeId requestingId;
  uint8_t srcEndpoint;
  uint8_t dstEndpoint;
  uint8_t snapshotPayloadType;
  uint8_t snapshotScheduleId;
} EmberAfSnapshotSchedulePayload;

/** @} */ // end of meter-snapshot-storage
