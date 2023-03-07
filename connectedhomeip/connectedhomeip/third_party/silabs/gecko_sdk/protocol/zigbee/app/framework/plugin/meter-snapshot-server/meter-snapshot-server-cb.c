/***************************************************************************//**
 * @file
 * @brief
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#include "af.h"

/** @brief Schedule Snapshot
 *
 * This function is called by the Meter Snapshot server plugin whenever a
 * ScheduleSnapshot command is received. A plugin implementing backhaul
 * functionality for the snapshot mechanism should proceed to schedule the
 * snapshot as required.
 *
 * @param srcEndpoint   Ver.: always
 * @param dstEndpoint   Ver.: always
 * @param dest   Ver.: always
 * @param snapshotPayload   Ver.: always
 * @param responsePayload   Ver.: always
 */
WEAK(void emberAfPluginMeterSnapshotServerScheduleSnapshotCallback(uint8_t srcEndpoint,
                                                                   uint8_t dstEndpoint,
                                                                   EmberNodeId dest,
                                                                   uint8_t*snapshotPayload,
                                                                   uint8_t*responsePayload))
{
}

/** @brief Take Snapshot
 *
 * This function is called by the Meter Snapshot server plugin whenever a
 * TakeSnapshot command is received. A plugin implementing backhaul
 * functionality for the snapshot mechanism should proceed to manually take a
 * snapshot as required. If for some reason the snapshot could not be taken,
 * this should be reflected in the snapshotConfirmation argument.
 *
 * @param endpoint   Ver.: always
 * @param snapshotCause   Ver.: always
 * @param snapshotConfirmation   Ver.: always
 */
WEAK(uint32_t emberAfPluginMeterSnapshotServerTakeSnapshotCallback(uint8_t endpoint,
                                                                   uint32_t snapshotCause,
                                                                   uint8_t*snapshotConfirmation))
{
  return 0;
}

/** @brief Get Snapshot
 *
 * This function is called by the Meter Snapshot server plugin whenever a
 * GetSnapshot command is received. A plugin implementing backhaul
 * functionality for the snapshot mechanism should proceed to retrieve the
 * snapshot and send a PublishSnapshot command to the requesting client.
 *
 * @param srcEndpoint   Ver.: always
 * @param dstEndpoint   Ver.: always
 * @param dest   Ver.: always
 * @param snapshotCriteria   Ver.: always
 */
WEAK(void emberAfPluginMeterSnapshotServerGetSnapshotCallback(uint8_t srcEndpoint,
                                                              uint8_t dstEndpoint,
                                                              EmberNodeId dest,
                                                              uint8_t*snapshotCriteria))
{
}
