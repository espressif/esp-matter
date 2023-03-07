/***************************************************************************//**
 * @file
 * @brief APIs and defines for the Prepayment Snapshot Storage plugin.
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

#ifndef SILABS_PREPAYMENT_SNAPSHOT_STORAGE_H
#define SILABS_PREPAYMENT_SNAPSHOT_STORAGE_H

/**
 * @defgroup prepayment-snapshot-storage Prepayment Snapshot Storage
 * @ingroup component
 * @brief API and Callbacks for the Prepayment Snapshot Storage Component
 *
 * This component implements storage for the prepayment snapshots.
 *
 */

/**
 * @addtogroup prepayment-snapshot-storage
 * @{
 */

// 0 is technically valid, but we'll designate it to be our "invalid" value.
#define INVALID_SNAPSHOT_ID           0
#define INVALID_SNAPSHOT_SCHEDULE_ID  0

/** @brief Defined snapshot causes.
 */
#ifdef DOXYGEN_SHOULD_SKIP_THIS
enum SnapshotCause
#else
enum
#endif
{
  SNAPSHOT_CAUSE_GENERAL                        = ( ((uint32_t)1) << 0),
  SNAPSHOT_CAUSE_END_OF_BILLING_PERIOD          = ( ((uint32_t)1) << 1),
  SNAPSHOT_CAUSE_CHANGE_OF_TARIFF_INFO          = ( ((uint32_t)1) << 3),
  SNAPSHOT_CAUSE_CHANGE_OF_PRICE_MATRIX         = ( ((uint32_t)1) << 4),
  SNAPSHOT_CAUSE_MANUALLY_TRIGGERED_FROM_CLIENT = ( ((uint32_t)1) << 10),
  SNAPSHOT_CAUSE_CHANGE_OF_TENANCY              = ( ((uint32_t)1) << 12),
  SNAPSHOT_CAUSE_CHANGE_OF_SUPPLIER             = ( ((uint32_t)1) << 13),
  SNAPSHOT_CAUSE_CHANGE_OF_METER_MODE           = ( ((uint32_t)1) << 14),
  SNAPSHOT_CAUSE_TOP_UP_ADDITION                = ( ((uint32_t)1) << 18),
  SNAPSHOT_CAUSE_DEBT_OR_CREDIT_ADDITION        = ( ((uint32_t)1) << 19),
};
#define SNAPSHOT_CAUSE_ALL_SNAPSHOTS  0xFFFFFFFF

typedef struct {
  uint32_t snapshotId;
  uint32_t snapshotCauseBitmap;
  uint32_t snapshotTime;    // Time snapshot was taken.
  uint8_t  snapshotType;
//  EmberNodeId requestingId;

  // For now, only 1 snapshot type exists (Debt/Credit), so there is only 1 payload.
  int32_t accumulatedDebt;
  uint32_t type1DebtRemaining;
  uint32_t type2DebtRemaining;
  uint32_t type3DebtRemaining;
  int32_t emergencyCreditRemaining;
  int32_t creditRemaining;
} EmberAfPrepaymentSnapshotPayload;

typedef struct {
  // Stores fields needed to schedule a new snapshot.
  uint32_t snapshotScheduleId;
  uint32_t snapshotStartTime;
  uint32_t snapshotCauseBitmap;
  EmberNodeId requestingId;
  uint8_t srcEndpoint;
  uint8_t dstEndpoint;
  uint8_t snapshotPayloadType;
} EmberAfPrepaymentSnapshotSchedulePayload;

/**
 * @name API
 * @{
 */
/** @brief publish prepayment snapshot
 *
 * @param nodeId Ver.: always
 * @param srcEndpoint Ver.: always
 * @param dstEndpoint Ver.: always
 * @param snashotTableIndex Ver.: always
 *
 * @return EmberStatus status code
 *
 */
EmberStatus emberAfPluginPrepaymentSnapshotStoragePublishSnapshot(EmberNodeId nodeId,
                                                                  uint8_t srcEndpoint,
                                                                  uint8_t dstEndpoint,
                                                                  uint32_t snapshotTableIndex);

/** @brief Take prepayment snapshot
 *
 * @param endpoint Ver.: always
 * @param snapshotCause Ver.: always
 *
 * @return uin32_t snapshot ID
 *
 */
uint32_t emberAfPluginPrepaymentSnapshotStorageTakeSnapshot(uint8_t endpoint, uint32_t snapshotCause);

/** @} */ // end of name API

/**
 * @name Callbacks
 * @{
 */

/**
 * @defgroup prepayment_snapshot_storage_cb Prepayment Snapshot Storage
 * @ingroup af_callback
 * @brief Callbacks for Prepayment Snapshot Storage Component
 *
 */

/**
 * @addtogroup prepayment_snapshot_storage_cb
 * @{
 */

/** @brief Callback for get prepayment snapshot
 *
 * @param nodeId Ver.: always
 * @param srcEndpoint Ver.: always
 * @param sdtEndpoint Ver.: always
 * @param startTime Ver.: always
 * @param endTime Ver.: always
 * @param snapshotOffset Ver.: always
 * @param snapShotCause Ver.: always
 *
 * @return uint8_t payload size
 *
 */
uint8_t emberAfPluginPrepaymentServerGetSnapshotCallback(EmberNodeId nodeId, uint8_t srcEndpoint, uint8_t dstEndpoint,
                                                         uint32_t startTime,
                                                         uint32_t endTime,
                                                         uint8_t  snapshotOffset,
                                                         uint32_t snapshotCause);

/** @} */ // end of prepayment_snapshot_storage_cb
/** @} */ // end of name Callbacks
/** @} */ // end of prepayment-snapshot-storage

#endif  // #ifndef _PREPAYMENT_SNAPSHOT_STORAGE_H_
