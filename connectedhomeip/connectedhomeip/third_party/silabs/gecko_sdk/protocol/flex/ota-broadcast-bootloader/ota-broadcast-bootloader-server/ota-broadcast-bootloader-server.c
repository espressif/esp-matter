/***************************************************************************//**
 * @brief Set of APIs for ota-broadcast-bootloader-server.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include "ota-broadcast-bootloader-server-config.h"

#include "stack/include/ember.h"
#include "hal/hal.h"

#include "ota-broadcast-bootloader-server.h"
#include "ota-broadcast-bootloader-server-internal.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis-rtos-support.h"
#endif

// Refer to Jira FLEX-868 for more info on this.
#if (defined(EMBER_TEST) && defined(UNIX_HOST))
#undef EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS
#define EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS 2500
#endif

//------------------------------------------------------------------------------
// Internal variables and static functions prototypes.

EmberEventControl emAfPluginOtaBootloaderServerEventControl;

static uint8_t internalState = STATE_OTA_SERVER_IDLE;
static EmberBuffer segmentBitmaskBuffer = EMBER_NULL_BUFFER;
static EmberBuffer targetListBuffer = EMBER_NULL_BUFFER;
static EmberBuffer completeCallbackTargetListBuffer = EMBER_NULL_BUFFER;

// This keeps track of stack errors such as send() API failures or other non-ACK
// related failures.
static uint8_t stackErrorsCount;
// This keeps track of target errors such as not receiving an ACK or not
// receiving an expected response.
static uint8_t currentTargetErrorsCount;
// The index of the block currently being broadcasted or for which targets are
// being queried for missing segments.
static uint16_t currentBlockIndex;
// This keeps track of how many broadcast rounds we have performed for the
// current block.
static uint8_t currentBlockBroadcastRoundsCount;
// The next segment index (if the server is currently broadcasting segments) or
// the next target index (if the server currently querying targets for missing
// segments).
static uint16_t nextSegmentOrTargetIndex;
// Stores the current image size in bytes or the bootload time (ms).
static uint32_t currentImageSizeOrBootloadTimeMs;
// Stores the current image tag (image distribution process) or the or the
// current server status (target status request process).
static uint8_t currentImageTagOrServerStatus;
// This is set to the corresponding "complete" state of the completing process.
// It facilitates new processes to be initiated within a complete callback while
// target information can be retrieved for the process that just completed.
static uint8_t processCompleteState = STATE_OTA_SERVER_IDLE;

// Common static functions
static void initTargetList(EmberNodeId *targetList, uint16_t targetListLength);

// Image distribution process static functions
static void broadcastNextSegment(void);
static void queryNextTargetForMissingSegments(void);
static void scheduleImageDistributionProcessNextTask(bool newSegmentOrNewTarget);
static void imageDistributionProcessFinished(EmberAfOtaBootloaderStatus status);
static uint16_t getTotalSegmentsCount(void);
static uint16_t getTotalBlocksCount(void);
static uint16_t getCurrentBlockSegmentsCount(void);
static void initSegmentsBitmask(void);
static void processMissingSegmentsBitmask(uint8_t *bitmask);

// Target status request process static functions
static void queryNextTargetForTargetStatus(void);
static void scheduleTargetStatusRequestProcessNextTask(bool newTarget);
static void targetsStatusRequestProcessFinished(EmberAfOtaBootloaderStatus status);

// Bootload request process static functions.
static void requestNextTargetForBootload(void);
static void scheduleBootloadRequestProcessNextTask(bool newTarget);
static void bootloadRequestProcessFinished(EmberAfOtaBootloaderStatus status);

//------------------------------------------------------------------------------
// Public APIs

EmberAfOtaBootloaderStatus emberAfPluginOtaBootloaderServerInitiateImageDistribution(uint32_t imageSize,
                                                                                     uint8_t imageTag,
                                                                                     EmberNodeId *targetList,
                                                                                     uint16_t targetListLength)
{
  uint16_t i;

  if (!serverIsIdle()) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_BUSY;
  }

  if (imageSize == 0
      || imageSize > MAXIMUM_IMAGE_SIZE
      || targetList == NULL
      || targetListLength == 0) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
  }

  // Sanity check on the passed target list.
  for (i = 0; i < targetListLength; i++) {
    if (targetList[i] == EMBER_NULL_NODE_ID) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBuffer =
    emberAllocateBuffer(targetListLength * TARGET_LIST_ENTRY_LENGTH);
  segmentBitmaskBuffer = emberAllocateBuffer(MISSING_SEGMENTS_BITMASK_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  if (targetListBuffer == EMBER_NULL_BUFFER
      || segmentBitmaskBuffer == EMBER_NULL_BUFFER) {
    targetListBuffer = EMBER_NULL_BUFFER;
    segmentBitmaskBuffer = EMBER_NULL_BUFFER;

    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_NO_BUFFERS;
  }

  // Initialize the target list
  initTargetList(targetList, targetListLength);

  // Initialize the bitmask: all segments should be broadcasted.
  initSegmentsBitmask();

  currentImageSizeOrBootloadTimeMs = imageSize;
  currentImageTagOrServerStatus = imageTag;

  currentBlockIndex = 0;
  nextSegmentOrTargetIndex = 0;
  stackErrorsCount = 0;
  currentBlockBroadcastRoundsCount = 0;

  internalState = STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL;

  emberEventControlSetActive(emAfPluginOtaBootloaderServerEventControl);

  return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
}

EmberAfOtaBootloaderStatus emberAfPluginBootloaderServerInitiateRequestTargetsStatus(EmberNodeId *targetList,
                                                                                     uint16_t targetListLength,
                                                                                     uint8_t applicationServerStatus)
{
  uint16_t i;

  if (!serverIsIdle()) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_BUSY;
  }

  if (targetList == NULL || targetListLength == 0) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
  }

  // Sanity check on the passed target list.
  for (i = 0; i < targetListLength; i++) {
    if (targetList[i] == EMBER_NULL_NODE_ID) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBuffer =
    emberAllocateBuffer(targetListLength * TARGET_LIST_ENTRY_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  if (targetListBuffer == EMBER_NULL_BUFFER) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_NO_BUFFERS;
  }

  // Initialize the target list
  initTargetList(targetList, targetListLength);

  nextSegmentOrTargetIndex = 0;
  stackErrorsCount = 0;
  currentTargetErrorsCount = 0;
  currentImageTagOrServerStatus = applicationServerStatus;

  internalState = STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_INTERVAL;

  emberEventControlSetActive(emAfPluginOtaBootloaderServerEventControl);

  return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
}

EmberAfOtaBootloaderStatus emberAfPluginBootloaderServerInitiateRequestTargetsBootload(uint32_t bootloadDelayMs,
                                                                                       uint8_t imageTag,
                                                                                       EmberNodeId *targetList,
                                                                                       uint16_t targetListLength)
{
  uint16_t i;

  if (!serverIsIdle()) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_BUSY;
  }

  if (targetList == NULL || targetListLength == 0) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
  }

  // Sanity check on the passed target list.
  for (i = 0; i < targetListLength; i++) {
    if (targetList[i] == EMBER_NULL_NODE_ID) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBuffer =
    emberAllocateBuffer(targetListLength * TARGET_LIST_ENTRY_LENGTH);

  if (targetListBuffer == EMBER_NULL_BUFFER) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_NO_BUFFERS;
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  // Initialize the target list
  initTargetList(targetList, targetListLength);

  nextSegmentOrTargetIndex = 0;
  stackErrorsCount = 0;
  currentTargetErrorsCount = 0;
  currentImageTagOrServerStatus = imageTag;
  currentImageSizeOrBootloadTimeMs =
    halCommonGetInt32uMillisecondTick() + bootloadDelayMs;

  internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL;

  emberEventControlSetActive(emAfPluginOtaBootloaderServerEventControl);

  return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
}

EmberAfOtaBootloaderTargetStatus emberAfPluginBootloaderServerGetTargetStatus(EmberNodeId targetId,
                                                                              uint8_t *applicationTargetStatus)
{
  uint16_t targetIndex;
  uint8_t otaStatus;

  if (serverIsIdle() && !serverIsCompletingProcess()) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_INVALID;
  }

  targetIndex = emTargetLookup(targetId);

  if (targetIndex == INVALID_TARGET_LIST_INDEX) {
    return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_INVALID;
  }

  otaStatus = emGetTargetOtaStatus(targetIndex);
  // Set the application status.
  *applicationTargetStatus = emGetTargetApplicationStatus(targetIndex);

  if (serverInImageDistributionProcess()
      || serverCompletingDistributionProcess()) {
    if (emTargetIsUnreachable(targetIndex)) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_UNREACHABLE;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_COMPLETED;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_ONGOING;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_FAILED;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_REFUSED;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ABORTED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_DISTRIBUTION_ABORTED;
    }
  }

  if (serverInTargetStatusRequestProcess()
      || serverCompletingTargetStatusRequestProcess()) {
    if (emTargetIsUnreachable(targetIndex)) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_UNREACHABLE;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_STATUS_REQUEST_COMPLETED;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_STATUS_REQUEST_ONGOING;
    }
  }

  if (serverInBootloadRequestProcess()
      || serverCompletingBootloadRequestProcess()) {
    if (emTargetIsUnreachable(targetIndex)) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_UNREACHABLE;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_ACCEPTED;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_ONGOING;
    } else if (otaStatus
               == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED) {
      return EMBER_OTA_BROADCAST_BOOTLOADER_TARGET_STATUS_BOOTLOAD_REQUEST_REFUSED;
    }
  }

  return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
}

EmberAfOtaBootloaderStatus emberAfPluginOtaBootloaderServerAbortCurrentProcess(void)
{
  if (serverInImageDistributionProcess()) {
    imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
  }

  if (serverInTargetStatusRequestProcess()) {
    targetsStatusRequestProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
  }

  if (serverInBootloadRequestProcess()) {
    bootloadRequestProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
  }

  return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
}

//------------------------------------------------------------------------------
// Implemented plugin callbacks

void emAfPluginOtaBootloaderServerIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint != EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT) {
    return;
  }

#if (EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_SECURITY_ENABLED > 0)
  // If security is enabled, discard all unsecured messages.
  if (!(message->options & EMBER_OPTIONS_SECURITY_ENABLED)) {
    return;
  }
#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_SECURITY_ENABLED > 0

  switch (emOtaBootloaderProtocolCommandId(message->payload)) {
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_MISSING_SEGMENTS_RESPONSE:
      if (internalState == STATE_OTA_SERVER_MISSING_SEGMENTS_WAITING_RESPONSE
          && message->length == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_HEADER_LENGTH) {
        uint16_t targetIndex = emTargetLookup(message->source);

        if (targetIndex == INVALID_TARGET_LIST_INDEX
            || targetIndex != nextSegmentOrTargetIndex) {
          return;
        }

        // Save the OTA response status.
        emSetTargetOtaStatus(targetIndex,
                             emOtaBootloaderProtocolResponseStatus(message->payload));

        // Save the application status.
        emSetTargetApplicationStatus(targetIndex,
                                     message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_APP_STATUS_OFFSET]);

        // If the client is still "ongoing", update the missing segments bitmask.
        if (emOtaBootloaderProtocolResponseStatus(message->payload)
            == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING) {
          processMissingSegmentsBitmask(message->payload
                                        + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_BITMASK_OFFSET);
        }

        scheduleImageDistributionProcessNextTask(true);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_TARGET_STATUS_RESPONSE:
      if (internalState == STATE_OTA_SERVER_TARGET_STATUS_REQUEST_WAITING_RESPONSE
          && message->length == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_RESP_HEADER_LENGTH) {
        uint16_t targetIndex = emTargetLookup(message->source);

        if (targetIndex == INVALID_TARGET_LIST_INDEX
            || targetIndex != nextSegmentOrTargetIndex) {
          return;
        }

        // Save the OTA response status.
        emSetTargetOtaStatus(targetIndex,
                             emOtaBootloaderProtocolResponseStatus(message->payload));

        // Save the application status.
        emSetTargetApplicationStatus(targetIndex,
                                     message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_RESP_APP_STATUS_OFFSET]);

        scheduleTargetStatusRequestProcessNextTask(true);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_RESPONSE:
      if (internalState == STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE
          && message->length == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH) {
        uint16_t targetIndex = emTargetLookup(message->source);

        if (targetIndex == INVALID_TARGET_LIST_INDEX
            || targetIndex != nextSegmentOrTargetIndex) {
          return;
        }

        // Save the OTA response status.
        emSetTargetOtaStatus(targetIndex,
                             emOtaBootloaderProtocolResponseStatus(message->payload));

        // Save the application status.
        emSetTargetApplicationStatus(targetIndex,
                                     message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_APP_STATUS_OFFSET]);

        scheduleBootloadRequestProcessNextTask(true);
      }
      break;
  }
}

void emAfPluginOtaBootloaderServerMessageSentCallback(EmberStatus status,
                                                      EmberOutgoingMessage *message)
{
  if (message->endpoint != EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT) {
    return;
  }

  switch (emOtaBootloaderProtocolCommandId(message->payload)) {
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT:
      if (internalState != STATE_OTA_SERVER_SEGMENT_BROADCAST_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, bump segment counter and reset the
        // tx  error counter.
        stackErrorsCount = 0;
        scheduleImageDistributionProcessNextTask(true);
      } else {
        // Message was not sent out, increase the tx error count and leave the
        // segment counter untouched. We will try again after a full broadcast
        // interval.
        stackErrorsCount++;
        scheduleImageDistributionProcessNextTask(false);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_MISSING_SEGMENTS_REQUEST:
      if (internalState != STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, wait for the corresponding missing
        // segment response and reset the stack errors count.
        internalState = STATE_OTA_SERVER_MISSING_SEGMENTS_WAITING_RESPONSE;
        emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS);
        stackErrorsCount = 0;
      } else {
        // If the message was sent out but no ACK was received we increase the
        // target error count and reset the stack errors count.
        if (status == EMBER_MAC_NO_ACK_RECEIVED) {
          currentTargetErrorsCount++;
          stackErrorsCount = 0;
        } else {
          // If the message was not sent out because of a stack issue (CCA or
          // others), we bump the stack errors count.
          stackErrorsCount++;
        }

        scheduleImageDistributionProcessNextTask(false);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_TARGET_STATUS_REQUEST:
      if (internalState != STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, wait for the corresponding target
        // status response  and reset the stack errors count.
        internalState = STATE_OTA_SERVER_TARGET_STATUS_REQUEST_WAITING_RESPONSE;
        emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS);
        stackErrorsCount = 0;
      } else {
        // If the message was sent out but no ACK was received we increase the
        // target error count  and reset the stack errors count.
        if (status == EMBER_MAC_NO_ACK_RECEIVED) {
          currentTargetErrorsCount++;
          stackErrorsCount = 0;
        } else {
          // If the message was not sent out because of a stack issue (CCA or
          // others), we bump the stack errors count.
          stackErrorsCount++;
        }

        scheduleTargetStatusRequestProcessNextTask(false);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST:
      if (internalState != STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, wait for the corresponding bootload
        // response  and reset the stack errors count.
        internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE;
        emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS);
        stackErrorsCount = 0;
      } else {
        // If the message was sent out but no ACK was received we increase the
        // target error count  and reset the stack errors count.
        if (status == EMBER_MAC_NO_ACK_RECEIVED) {
          currentTargetErrorsCount++;
          stackErrorsCount = 0;
        } else {
          // If the message was not sent out because of a stack issue (CCA or
          // others), we bump the stack errors count.
          stackErrorsCount++;
        }

        scheduleBootloadRequestProcessNextTask(false);
      }
      break;
  }
}

void emAfPluginOtaBootloaderServerEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderServerEventControl);

  switch (internalState) {
    case STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL:
      broadcastNextSegment();
      break;
    case STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_INTERVAL:
      queryNextTargetForMissingSegments();
      break;
    case STATE_OTA_SERVER_MISSING_SEGMENTS_WAITING_RESPONSE:
      // No response from the target: bump the target's error count and schedule
      // the next task.
      currentTargetErrorsCount++;
      scheduleImageDistributionProcessNextTask(false);
      break;
    case STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_INTERVAL:
      queryNextTargetForTargetStatus();
      break;
    case STATE_OTA_SERVER_TARGET_STATUS_REQUEST_WAITING_RESPONSE:
      // No response from the target: bump the target's error count and schedule
      // the next task.
      currentTargetErrorsCount++;
      scheduleTargetStatusRequestProcessNextTask(false);
      break;
    case STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL:
      requestNextTargetForBootload();
      break;
    case STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE:
      // No response from the target: bump the target's error count and schedule
      // the next task.
      currentTargetErrorsCount++;
      scheduleBootloadRequestProcessNextTask(false);
      break;
  }
}

uint8_t emGetOrSetTargetMiscInfo(uint16_t targetIndex,
                                 bool isSet,
                                 uint8_t info)
{
  EmberBuffer currentTargetListBuffer = ((serverIsCompletingProcess())
                                         ? completeCallbackTargetListBuffer
                                         : targetListBuffer);
  uint8_t *targetListBufferPtr;
  uint8_t retVal = 0;

  assert(currentTargetListBuffer != EMBER_NULL_BUFFER
         && emberGetBufferLength(currentTargetListBuffer)
         >= (targetIndex + 1) * TARGET_LIST_ENTRY_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(currentTargetListBuffer);

  if (isSet) {
    targetListBufferPtr[targetIndex * TARGET_LIST_ENTRY_LENGTH
                        + TARGET_LIST_ENTRY_MISC_INFO_OFFSET] = info;
  } else {
    retVal = targetListBufferPtr[targetIndex * TARGET_LIST_ENTRY_LENGTH
                                 + TARGET_LIST_ENTRY_MISC_INFO_OFFSET];
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  return retVal;
}

uint8_t emGetOrSetTargetApplicationStatus(uint16_t targetIndex,
                                          bool isSet,
                                          uint8_t appStatus)
{
  EmberBuffer currentTargetListBuffer = ((serverIsCompletingProcess())
                                         ? completeCallbackTargetListBuffer
                                         : targetListBuffer);
  uint8_t *targetListBufferPtr;
  uint8_t retVal = 0;

  assert(currentTargetListBuffer != EMBER_NULL_BUFFER
         && emberGetBufferLength(currentTargetListBuffer)
         >= (targetIndex + 1) * TARGET_LIST_ENTRY_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(currentTargetListBuffer);

  if (isSet) {
    targetListBufferPtr[targetIndex * TARGET_LIST_ENTRY_LENGTH
                        + TARGET_LIST_ENTRY_APP_STATUS_OFFSET] = appStatus;
  } else {
    retVal = targetListBufferPtr[targetIndex * TARGET_LIST_ENTRY_LENGTH
                                 + TARGET_LIST_ENTRY_APP_STATUS_OFFSET];
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  return retVal;
}

uint16_t emTargetLookup(EmberNodeId targetShortId)
{
  EmberBuffer currentTargetListBuffer = ((serverIsCompletingProcess())
                                         ? completeCallbackTargetListBuffer
                                         : targetListBuffer);
  uint8_t *targetListBufferPtr;
  uint16_t i;

  assert(currentTargetListBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(currentTargetListBuffer);

  for (i = 0;
       i < emberGetBufferLength(currentTargetListBuffer) / TARGET_LIST_ENTRY_LENGTH;
       i++) {
    EmberNodeId nodeId =
      emberFetchLowHighInt16u(targetListBufferPtr + i * TARGET_LIST_ENTRY_LENGTH
                              + TARGET_LIST_ENTRY_SHORT_ID_OFFSET);
    if (nodeId == targetShortId) {
#if defined(SL_CATALOG_KERNEL_PRESENT)
      emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

      return i;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  return INVALID_TARGET_LIST_INDEX;
}

void emAfPluginOtaBootloaderServerMarkBuffersCallback(void)
{
  emberMarkBuffer(&targetListBuffer);
  emberMarkBuffer(&completeCallbackTargetListBuffer);
  emberMarkBuffer(&segmentBitmaskBuffer);
}

//------------------------------------------------------------------------------
// Common static functions

static void initTargetList(EmberNodeId *targetList, uint16_t targetListLength)
{
  uint16_t i;
  uint8_t *targetListBufferPtr;

  assert(targetListBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(targetListBuffer);

  // Copy the target list and initialize the application status and the
  // misc info fields.
  for (i = 0; i < targetListLength; i++) {
    emberStoreLowHighInt16u(targetListBufferPtr + i * TARGET_LIST_ENTRY_LENGTH
                            + TARGET_LIST_ENTRY_SHORT_ID_OFFSET,
                            targetList[i]);
    targetListBufferPtr[i * TARGET_LIST_ENTRY_LENGTH
                        + TARGET_LIST_ENTRY_APP_STATUS_OFFSET] =
      EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS;
    targetListBufferPtr[i * TARGET_LIST_ENTRY_LENGTH
                        + TARGET_LIST_ENTRY_MISC_INFO_OFFSET] =
      TARGET_LIST_ENTRY_MISC_INFO_INIT_VALUE;
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif
}

static bool updateNextTargetIndex(bool newTarget)
{
  if (newTarget) {
    nextSegmentOrTargetIndex++;
  }

  for (;
       nextSegmentOrTargetIndex
       < (emberGetBufferLength(targetListBuffer) / TARGET_LIST_ENTRY_LENGTH);
       nextSegmentOrTargetIndex++) {
    uint8_t targetOtaStatus = emGetTargetOtaStatus(nextSegmentOrTargetIndex);

    // We skip targets that have been declared unreachable or that reported
    // failure, refused the image or aborted the download process.
    if (!emTargetIsUnreachable(nextSegmentOrTargetIndex)
        && targetOtaStatus
        <= EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING) {
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------------
// Image distribution process static functions

bool currentBlockCompleted(void)
{
  uint8_t *segmentBitmaskBufferPtr;
  uint16_t i;

  assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  segmentBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer);

  for (i = 0; i < MISSING_SEGMENTS_BITMASK_LENGTH; i++) {
    if (segmentBitmaskBufferPtr[i] > 0) {
#if defined(SL_CATALOG_KERNEL_PRESENT)
      emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

      return false;
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  return true;
}

static bool segmentIndexToBit(uint16_t segmentIndex)
{
  uint16_t byteIndex = ((segmentIndex % MAX_SEGMENTS_IN_A_BLOCK) / 8);
  uint8_t bitIndex = ((segmentIndex % MAX_SEGMENTS_IN_A_BLOCK) % 8);
  uint8_t *segmentBitmaskBufferPtr;
  bool ret;

  assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  segmentBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer);

  ret = ((segmentBitmaskBufferPtr[byteIndex] & BIT(bitIndex)) > 0);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  return ret;
}

// Returns true if a new segment was found, false if there are no more segments
// to send.
static bool updateNextSegmentIndex(bool newSegment)
{
  if (newSegment) {
    nextSegmentOrTargetIndex++;
  }

  for (;
       nextSegmentOrTargetIndex < getCurrentBlockSegmentsCount();
       nextSegmentOrTargetIndex++) {
    if (segmentIndexToBit(nextSegmentOrTargetIndex)) {
      return true;
    }
  }

  // No more segments to be sent.
  return false;
}

static void broadcastNextSegment(void)
{
  uint8_t message[MAX_APPLICATION_PAYLOAD_LENGTH];
  uint32_t startIndex = nextSegmentOrTargetIndex * MAX_SEGMENT_PAYLOAD_LENGTH;
  uint32_t endIndex = startIndex + MAX_SEGMENT_PAYLOAD_LENGTH - 1;

  // Account for the last segment which may very well be a partial segment.
  if (endIndex >= currentImageSizeOrBootloadTimeMs) {
    endIndex = currentImageSizeOrBootloadTimeMs - 1;
  }

  if (!emberAfPluginOtaBootloaderServerGetImageSegmentCallback(startIndex,
                                                               endIndex,
                                                               currentImageTagOrServerStatus,
                                                               message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH)) {
    imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_DATA_UNDERFLOW);
  } else {
    EmberStatus status;

    // Frame control
    message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
      EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT;
    // Image tag
    message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_TAG_OFFSET] =
      currentImageTagOrServerStatus;
    // Segment index
    emberStoreLowHighInt16u(message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_INDEX_OFFSET,
                            nextSegmentOrTargetIndex);

    status = emberMessageSend(EMBER_BROADCAST_ADDRESS,
                              EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT,
                              0, // messageTag
                              (EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH
                               + endIndex - startIndex + 1),
                              message,
                              BROADCAST_TX_OPTIONS);

    if (status == EMBER_SUCCESS) {
      // Wait for the messageSent() corresponding call.
      internalState = STATE_OTA_SERVER_SEGMENT_BROADCAST_PENDING;
    } else {
      // If we failed submitting a message to the stack, we increase the tx
      // count and try again after a full broadcast interval.
      stackErrorsCount++;
      scheduleImageDistributionProcessNextTask(false);
    }
  }
}

static void queryNextTargetForMissingSegments(void)
{
  uint8_t message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_HEADER_LENGTH];
  EmberStatus status;
  EmberNodeId destination;
  uint8_t *targetListBufferPtr;
  uint16_t firstSegmentIndex = currentBlockIndex * MAX_SEGMENTS_IN_A_BLOCK;
  uint16_t lastSegmentIndex =
    (currentBlockIndex + 1) * MAX_SEGMENTS_IN_A_BLOCK - 1;

  if (lastSegmentIndex >= getTotalSegmentsCount()) {
    lastSegmentIndex = getTotalSegmentsCount() - 1;
  }

  assert(targetListBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(targetListBuffer);

  destination =
    emberFetchLowHighInt16u(targetListBufferPtr
                            + nextSegmentOrTargetIndex * TARGET_LIST_ENTRY_LENGTH
                            + TARGET_LIST_ENTRY_SHORT_ID_OFFSET);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  // Frame control
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_MISSING_SEGMENTS_REQUEST;
  // Image tag
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_TAG_OFFSET] =
    currentImageTagOrServerStatus;
  // Image size
  emberStoreLowHighInt32u(message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_IMAGE_SIZE_OFFSET,
                          currentImageSizeOrBootloadTimeMs);
  // First segment index
  emberStoreLowHighInt16u(message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_FIRST_INDEX_OFFSET,
                          firstSegmentIndex);
  // Last segment index
  emberStoreLowHighInt16u(message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_LAST_INDEX_OFFSET,
                          lastSegmentIndex);

  status = emberMessageSend(destination,
                            EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT,
                            0, // messageTag
                            EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_HEADER_LENGTH,
                            message,
                            UNICAST_TX_OPTIONS);

  if (status == EMBER_SUCCESS) {
    // Wait for the messageSent() corresponding call.
    internalState = STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_PENDING;
  } else {
    // If we failed submitting a message to the stack, we increase the tx
    // count and try again after a full broadcast interval.
    stackErrorsCount++;
    scheduleImageDistributionProcessNextTask(false);
  }
}

static void scheduleImageDistributionProcessNextTask(bool newSegmentOrNewTarget)
{
  switch (internalState) {
    case STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL:
    case STATE_OTA_SERVER_SEGMENT_BROADCAST_PENDING:
      if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
        imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_STACK_ERROR);
        return;
      }

      // We schedule a new segment broadcast if:
      // - we have to resend the current segment
      // - or we still have a new segment to broadcast within the current block.
      if (updateNextSegmentIndex(newSegmentOrNewTarget)) {
        internalState = STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL;
      } else {
        // We sent all the segments already: set the target index to 0, update
        // the next target index, reset the missing segments bitmask and start the
        // querying process.
        nextSegmentOrTargetIndex = 0;
        stackErrorsCount = 0;
        assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
        emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

        MEMSET(emberGetBufferPointer(segmentBitmaskBuffer),
               0x00,
               MISSING_SEGMENTS_BITMASK_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
        emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

        // Here we assert: if we started a new broadcast round we had at least
        // one active target.
        assert(updateNextTargetIndex(false));
        internalState = STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_INTERVAL;
      }

      emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                  EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
      break;
    case STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_INTERVAL:
    case STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_PENDING:
    case STATE_OTA_SERVER_MISSING_SEGMENTS_WAITING_RESPONSE:
      if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
        imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_STACK_ERROR);
        return;
      }

      // The current target reached the maximum consecutive unicast errors, we
      // set it as unreachable and reset the current target errors count.
      if (currentTargetErrorsCount
          >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
        emSetTargetUnreachable(nextSegmentOrTargetIndex);
        currentTargetErrorsCount = 0;
      }

      if (updateNextTargetIndex(newSegmentOrNewTarget)) {
        internalState = STATE_OTA_SERVER_MISSING_SEGMENTS_UNICAST_INTERVAL;

        emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
        // If this is a new target, reset the target errors count.
        if (newSegmentOrNewTarget) {
          currentTargetErrorsCount = 0;
        }
      } else {
        // We completed the missing segments querying process.

        // If all the targets are in "unreachable" state, fail the process.
        // (we check it using the updateNextTargetIndex() function).
        nextSegmentOrTargetIndex = 0;
        if (!updateNextTargetIndex(false)) {
          imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_FAILED);
          return;
        }

        // if the current block is all done, reset the segments bitmask and move
        // to the next block (if any).
        if (currentBlockCompleted()) {
          currentBlockIndex++;
          currentBlockBroadcastRoundsCount = 0;
          nextSegmentOrTargetIndex = currentBlockIndex * MAX_SEGMENTS_IN_A_BLOCK;
          initSegmentsBitmask();
        } else {
          // The current block has missing segments, increase the broadcast round
          // count and reset the next segment.
          currentBlockBroadcastRoundsCount++;
          nextSegmentOrTargetIndex = currentBlockIndex * MAX_SEGMENTS_IN_A_BLOCK;
          // We assert here: if we got here, there should be at least a segment
          // to be sent.
          assert(updateNextSegmentIndex(false));

          // If we reached the maximum broadcast rounds, fail distribution.
          if (currentBlockBroadcastRoundsCount
              >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_BROADCAST_ROUNDS) {
            imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_TIMEOUT);
            return;
          }
        }

        // If there still are blocks to be distributed, kickoff a new broadcast
        // round.
        if (currentBlockIndex < getTotalBlocksCount()) {
          internalState = STATE_OTA_SERVER_SEGMENT_BROADCAST_INTERVAL;

          emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                      EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
        } else {
          // All blocks have been distributed, success!
          imageDistributionProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS);
        }
      }

      break;
    default:
      assert(0);
  }
}

static void imageDistributionProcessFinished(EmberAfOtaBootloaderStatus status)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderServerEventControl);

  internalState = STATE_OTA_SERVER_IDLE;
  completeCallbackTargetListBuffer = targetListBuffer;
  targetListBuffer = EMBER_NULL_BUFFER;
  segmentBitmaskBuffer = EMBER_NULL_BUFFER;

  processCompleteState = STATE_OTA_SERVER_IMAGE_DISTRIBUTION_COMPLETED;
  emberAfPluginOtaBootloaderServerImageDistributionCompleteCallback(status);
  processCompleteState = STATE_OTA_SERVER_IDLE;

  completeCallbackTargetListBuffer = EMBER_NULL_BUFFER;
}

static uint16_t getTotalSegmentsCount(void)
{
  uint16_t totalSegments =
    (uint16_t)(currentImageSizeOrBootloadTimeMs / MAX_SEGMENT_PAYLOAD_LENGTH);

  if ((currentImageSizeOrBootloadTimeMs % MAX_SEGMENT_PAYLOAD_LENGTH) > 0) {
    totalSegments++;
  }

  return totalSegments;
}

static uint16_t getTotalBlocksCount(void)
{
  uint16_t totalBlocks = getTotalSegmentsCount() / MAX_SEGMENTS_IN_A_BLOCK;

  if ((getTotalSegmentsCount() % MAX_SEGMENTS_IN_A_BLOCK) > 0) {
    totalBlocks++;
  }

  return totalBlocks;
}

static uint16_t getCurrentBlockSegmentsCount(void)
{
  uint16_t segmentsCount =
    (currentBlockIndex + 1) * MAX_SEGMENTS_IN_A_BLOCK;

  if (segmentsCount > getTotalSegmentsCount()) {
    segmentsCount = getTotalSegmentsCount();
  }

  return segmentsCount;
}

static void initSegmentsBitmask(void)
{
  uint8_t *segmentBitmaskBufferPtr;

  assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  segmentBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer);
  MEMSET(segmentBitmaskBufferPtr, 0xFF, MISSING_SEGMENTS_BITMASK_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif
}

static void processMissingSegmentsBitmask(uint8_t *bitmask)
{
  uint8_t i;
  uint8_t *segmentBitmaskBufferPtr;

  assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  segmentBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer); \
  for (i = 0; i < MISSING_SEGMENTS_BITMASK_LENGTH; i++) {
    segmentBitmaskBufferPtr[i] |= bitmask[i];
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif
}

// -----------------------------------------------------------------------------
// Target status request process static functions

static void queryNextTargetForTargetStatus(void)
{
  uint8_t message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_REQ_HEADER_LENGTH];
  EmberStatus status;
  EmberNodeId destination;
  uint8_t *targetListBufferPtr;

  assert(targetListBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(targetListBuffer);

  destination =
    emberFetchLowHighInt16u(targetListBufferPtr
                            + nextSegmentOrTargetIndex * TARGET_LIST_ENTRY_LENGTH
                            + TARGET_LIST_ENTRY_SHORT_ID_OFFSET);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  // Frame control
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_TARGET_STATUS_REQUEST;
  // Application status
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_REQ_APP_STATUS_OFFSET] =
    currentImageTagOrServerStatus;

  status = emberMessageSend(destination,
                            EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT,
                            0, // messageTag
                            EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_REQ_HEADER_LENGTH,
                            message,
                            UNICAST_TX_OPTIONS);

  if (status == EMBER_SUCCESS) {
    // Wait for the messageSent() corresponding call.
    internalState = STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_PENDING;
  } else {
    // If we failed submitting a message to the stack, we increase the tx
    // count and try again after a full broadcast interval.
    stackErrorsCount++;
    scheduleTargetStatusRequestProcessNextTask(false);
  }
}

static void scheduleTargetStatusRequestProcessNextTask(bool newTarget)
{
  assert(serverInTargetStatusRequestProcess());

  if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
    targetsStatusRequestProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_STACK_ERROR);
    return;
  }

  // The current target reached the maximum consecutive unicast errors, we
  // set it as unreachable and reset the current target errors count.
  if (currentTargetErrorsCount
      >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
    emSetTargetUnreachable(nextSegmentOrTargetIndex);
    currentTargetErrorsCount = 0;
  }

  if (updateNextTargetIndex(newTarget)) {
    internalState = STATE_OTA_SERVER_TARGET_STATUS_REQUEST_UNICAST_INTERVAL;

    emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);

    // If this is a new target, reset the target errors count.
    if (newTarget) {
      currentTargetErrorsCount = 0;
    }
  } else {
    // We completed the targets status request process.
    targetsStatusRequestProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS);
  }
}

static void targetsStatusRequestProcessFinished(EmberAfOtaBootloaderStatus status)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderServerEventControl);

  internalState = STATE_OTA_SERVER_IDLE;
  completeCallbackTargetListBuffer = targetListBuffer;
  targetListBuffer = EMBER_NULL_BUFFER;

  processCompleteState = STATE_OTA_SERVER_TARGET_STATUS_REQUEST_COMPLETED;
  emberAfPluginBootloaderServerRequestTargetsStatusCompleteCallback(status);
  processCompleteState = STATE_OTA_SERVER_IDLE;

  completeCallbackTargetListBuffer = EMBER_NULL_BUFFER;
}

// -----------------------------------------------------------------------------
// Bootload request process static functions

static void requestNextTargetForBootload(void)
{
  uint8_t message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH];
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  uint8_t *targetListBufferPtr;
  EmberNodeId destination;
  EmberStatus status;
  uint32_t delayMs;

  assert(targetListBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  targetListBufferPtr = emberGetBufferPointer(targetListBuffer);

  destination =
    emberFetchLowHighInt16u(targetListBufferPtr
                            + nextSegmentOrTargetIndex * TARGET_LIST_ENTRY_LENGTH
                            + TARGET_LIST_ENTRY_SHORT_ID_OFFSET);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  if (timeGTorEqualInt32u(nowMs, currentImageSizeOrBootloadTimeMs)) {
    delayMs = 0;
  } else {
    delayMs = elapsedTimeInt32u(nowMs, currentImageSizeOrBootloadTimeMs);
  }

  // Frame control
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST;
  // Image tag
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_TAG_OFFSET] =
    currentImageTagOrServerStatus;
  // Delay
  emberStoreLowHighInt32u(message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_DELAY_OFFSET,
                          delayMs);

  status = emberMessageSend(destination,
                            EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_ENDPOINT,
                            0, // messageTag
                            EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH,
                            message,
                            UNICAST_TX_OPTIONS);

  if (status == EMBER_SUCCESS) {
    // Wait for the messageSent() corresponding call.
    internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_PENDING;
  } else {
    // If we failed submitting a message to the stack, we increase the tx
    // count and try again after a full broadcast interval.
    stackErrorsCount++;
    scheduleBootloadRequestProcessNextTask(false);
  }
}

static void scheduleBootloadRequestProcessNextTask(bool newTarget)
{
  assert(serverInBootloadRequestProcess());

  if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
    bootloadRequestProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_STACK_ERROR);
    return;
  }

  // The current target reached the maximum consecutive unicast errors, we
  // set it as unreachable and reset the current target errors count.
  if (currentTargetErrorsCount
      >= EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
    emSetTargetUnreachable(nextSegmentOrTargetIndex);
    currentTargetErrorsCount = 0;
  }

  if (updateNextTargetIndex(newTarget)) {
    internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL;

    emberEventControlSetDelayMS(emAfPluginOtaBootloaderServerEventControl,
                                EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);

    // If this is a new target, reset the target errors count.
    if (newTarget) {
      currentTargetErrorsCount = 0;
    }
  } else {
    // We completed the bootload request process.
    bootloadRequestProcessFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS);
  }
}

static void bootloadRequestProcessFinished(EmberAfOtaBootloaderStatus status)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderServerEventControl);

  internalState = STATE_OTA_SERVER_IDLE;
  completeCallbackTargetListBuffer = targetListBuffer;
  targetListBuffer = EMBER_NULL_BUFFER;

  processCompleteState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_COMPLETED;
  emberAfPluginBootloaderServerRequestTargetsBootloadCompleteCallback(status);
  processCompleteState = STATE_OTA_SERVER_IDLE;

  completeCallbackTargetListBuffer = EMBER_NULL_BUFFER;
}
