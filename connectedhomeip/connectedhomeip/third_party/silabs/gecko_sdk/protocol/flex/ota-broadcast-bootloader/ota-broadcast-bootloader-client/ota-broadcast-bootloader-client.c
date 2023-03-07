/***************************************************************************//**
 * @brief Set of APIs for ota-broadcast-bootloader-client.
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

#include "ota-broadcast-bootloader-client-config.h"

#include "stack/include/ember.h"
#include "hal/hal.h"

#include "ota-broadcast-bootloader-client.h"
#include "ota-broadcast-bootloader-client-internal.h"

#if defined(SL_CATALOG_KERNEL_PRESENT)
#include "cmsis-rtos-support.h"
#endif

//------------------------------------------------------------------------------
// Internal variables and static functions prototypes.

EmberEventControl emAfPluginOtaBootloaderClientEventControl;

static uint8_t internalState = STATE_OTA_CLIENT_IDLE;
static EmberBuffer segmentBitmaskBuffer = EMBER_NULL_BUFFER;

static uint8_t historyTableHeadIndex, historyTableTailIndex;
static EmHistoryEntry historyTable[HISTORY_TABLE_SIZE];

static struct {
  uint32_t size;
  EmberNodeId serverId;
  EmberNodeId alternateServerId;
  uint16_t blockIndex;
  uint8_t tag;
} imageInfo;

// Image download process related static functions
static void imageDownloadFinished(EmberAfOtaBootloaderStatus status);
static void handleIncomingImageSegment(EmberIncomingMessage *message);
static void handleIncomingMissingSegmentsRequest(EmberIncomingMessage *message);
static void historyTableAddEntry(uint8_t otaProtocolStatus,
                                 uint8_t applicationStatus);
static void historyTableRemoveEntry(uint8_t entryIndex);
static uint8_t historyTableLookup(EmberNodeId serverId, uint8_t imageTag);

// Target status request process related static function
static void handleIncomingTargetStatusRequest(EmberIncomingMessage *message);

// Bootload request process related static function
static void handleIncomingBootloadRequest(EmberIncomingMessage *message);

//------------------------------------------------------------------------------
// Public APIs

EmberAfOtaBootloaderStatus emberAfPluginOtaBootloaderClientAbortImageDownload(uint8_t imageTag,
                                                                              uint8_t applicationErrorStatus)
{
  if (clientInImageDownload()
      && imageTag == imageInfo.tag) {
    historyTableAddEntry(EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ABORTED,
                         applicationErrorStatus);
    imageDownloadFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS;
  }

  return EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_INVALID_CALL;
}

//------------------------------------------------------------------------------
// Implemented plugin callbacks

void emAfPluginOtaBootloaderClientInitCallback(void)
{
  uint8_t i;

  // Initialize the history table.
  historyTableHeadIndex = historyTableTailIndex = 0;
  for (i = 0; i < HISTORY_TABLE_SIZE; i++) {
    historyTable[i].serverId = EMBER_NULL_NODE_ID;
  }
}

void emAfPluginOtaBootloaderClientIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint != EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_ENDPOINT) {
    return;
  }

#if (EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_SECURITY_ENABLED > 0)
  // If security is enabled, discard all unsecured messages.
  if (!(message->options & EMBER_OPTIONS_SECURITY_ENABLED)) {
    return;
  }
#endif // EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_SECURITY_ENABLED > 0

  switch (emOtaBootloaderProtocolCommandId(message->payload)) {
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT:
      if (message->length
          > EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH) {
        handleIncomingImageSegment(message);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_MISSING_SEGMENTS_REQUEST:
      if (message->length
          == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_HEADER_LENGTH) {
        handleIncomingMissingSegmentsRequest(message);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_TARGET_STATUS_REQUEST:
      if (message->length
          == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_REQ_HEADER_LENGTH) {
        handleIncomingTargetStatusRequest(message);
      }
      break;
    case EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST:
      if (message->length
          == EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH) {
        handleIncomingBootloadRequest(message);
      }
      break;
  }
}

void emAfPluginOtaBootloaderClientMarkBuffersCallback(void)
{
  emberMarkBuffer(&segmentBitmaskBuffer);
}

void emAfPluginOtaBootloaderClientEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginOtaBootloaderClientEventControl);

  // We timed-out waiting for a message from the server(s): fail the ongoing
  // image download process.
  if (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING) {
    historyTableAddEntry(EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED,
                         EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS);
    imageDownloadFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_TIMEOUT);
  }
}

//------------------------------------------------------------------------------
// Image download process related static functions

static void imageDownloadFinished(EmberAfOtaBootloaderStatus status)
{
  internalState = STATE_OTA_CLIENT_IDLE;
  segmentBitmaskBuffer = EMBER_NULL_BUFFER;
  emberEventControlSetInactive(emAfPluginOtaBootloaderClientEventControl);
  emberAfPluginOtaBootloaderClientImageDownloadCompleteCallback(status,
                                                                imageInfo.tag,
                                                                ((status == EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS)
                                                                 ? imageInfo.size
                                                                 : 0));
}

static bool serverOk(EmberNodeId serverId)
{
  return (serverId == imageInfo.serverId
          || serverId == imageInfo.alternateServerId
          || imageInfo.alternateServerId == EMBER_BROADCAST_ADDRESS);
}

static void initSegmentsBitmask(void)
{
  uint8_t *segmentBitmaskBufferPtr;

  assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  segmentBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer);
  MEMSET(segmentBitmaskBufferPtr, 0x00, MISSING_SEGMENTS_BITMASK_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif
}

static bool getSegmentBit(uint16_t segmentIndex)
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

static void setSegmentBit(uint16_t segmentIndex)
{
  uint16_t byteIndex = ((segmentIndex % MAX_SEGMENTS_IN_A_BLOCK) / 8);
  uint8_t bitIndex = ((segmentIndex % MAX_SEGMENTS_IN_A_BLOCK) % 8);
  uint8_t *segmentBitmaskBufferPtr;

  assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

  segmentBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer);
  segmentBitmaskBufferPtr[byteIndex] |= BIT(bitIndex);

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif
}

// TODO: optimize this!
static bool currentBlockCompleted(uint16_t firstSegmentIndex,
                                  uint16_t lastSegmentIndex)
{
  uint16_t index;

  for (index = firstSegmentIndex; index <= lastSegmentIndex; index++) {
    if (!getSegmentBit(index)) {
      return false;
    }
  }

  return true;
}

static uint16_t getTotalSegmentsCount(void)
{
  uint16_t totalSegments =
    (uint16_t)(imageInfo.size / MAX_SEGMENT_PAYLOAD_LENGTH);

  assert(imageInfo.size > 0);

  if ((imageInfo.size % MAX_SEGMENT_PAYLOAD_LENGTH) > 0) {
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

static void handleIncomingImageSegment(EmberIncomingMessage *message)
{
  EmberNodeId serverId = message->source;
  uint8_t imageTag =
    message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_TAG_OFFSET];
  uint16_t segmentIndex =
    emberFetchLowHighInt16u(message->payload
                            + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_INDEX_OFFSET);
  uint16_t blockIndex = segmentIndex / MAX_SEGMENTS_IN_A_BLOCK;
  int8_t segmentLength = message->length
                         - EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH;

  // Incoming segment belongs to the first block and the client is idle: see if
  // the application is interested in receiving this image.
  if (blockIndex == 0 && clientIsIdle()) {
    imageInfo.serverId = serverId;
    imageInfo.alternateServerId = serverId;
    imageInfo.tag = imageTag;
    imageInfo.blockIndex = 0;
    imageInfo.size = 0;

#if defined(SL_CATALOG_KERNEL_PRESENT)
    emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

    // Allocate the bitmask before we query the application: if we have no RAM
    // to allocate the bitmask, there is no point in querying the application.
    segmentBitmaskBuffer = emberAllocateBuffer(MISSING_SEGMENTS_BITMASK_LENGTH);

#if defined(SL_CATALOG_KERNEL_PRESENT)
    emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

    if (segmentBitmaskBuffer == EMBER_NULL_BUFFER
        || !emberAfPluginOtaBootloaderClientNewIncomingImageCallback(imageInfo.serverId,
                                                                     &imageInfo.alternateServerId,
                                                                     imageInfo.tag)) {
      // Application refused to download the image: free the allocated bitmask
      // (in case we allocated it), add an entry in the history table and
      // silently discard the segment command.
      segmentBitmaskBuffer = EMBER_NULL_BUFFER;
      historyTableAddEntry(EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED,
                           EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS);
      return;
    }

    initSegmentsBitmask();
    internalState = STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING;

    // If there is a matching entry in the history table, remove it.
    {
      uint8_t historyTableIndex = historyTableLookup(serverId, imageTag);
      if (historyTableIndex < 0xFF) {
        historyTableRemoveEntry(historyTableIndex);
      }
    }
  }

  if (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING
      && serverOk(serverId)
      && imageTag == imageInfo.tag) {
    if (blockIndex == imageInfo.blockIndex) {
      // Incoming segment belongs to the current block: hand the segment to the
      // application in case this is the first time we receive this segment.
      if (!getSegmentBit(segmentIndex)) {
        emberAfPluginOtaBootloaderClientIncomingImageSegmentCallback(serverId,
                                                                     segmentIndex * MAX_SEGMENT_PAYLOAD_LENGTH,
                                                                     segmentIndex * MAX_SEGMENT_PAYLOAD_LENGTH + segmentLength - 1,
                                                                     imageTag,
                                                                     message->payload + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_PAYLOAD_OFFSET);

        if (internalState == STATE_OTA_CLIENT_IDLE) {
          // Download was aborted in the incoming segment callback, give up.
          return;
        }
        setSegmentBit(segmentIndex);
      }

      // Refresh the download timeout.
      emberEventControlSetDelayQS(emAfPluginOtaBootloaderClientEventControl,
                                  EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_IMAGE_DOWNLOAD_TIMEOUT_S * 4);
    } else if (blockIndex < imageInfo.blockIndex) {
      // Segment from an older block: the server is re-sending some missing
      // segment while this client already moved to the next block: just
      // refresh the download timeout.
      emberEventControlSetDelayQS(emAfPluginOtaBootloaderClientEventControl,
                                  EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_IMAGE_DOWNLOAD_TIMEOUT_S * 4);
    } else {
      // The block index is > than the current block index: this means that
      // somehow the server moved on to the next block while the client still
      // had missing segments on the current block: all we can do is to fail the
      // image download process.
      historyTableAddEntry(EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED,
                           EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS);
      imageDownloadFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_FAILED);
    }
  }
}

static void sendMissingSegmentsResponse(EmberNodeId serverId,
                                        uint8_t otaProtocolStatus,
                                        uint8_t applicationStatus,
                                        uint8_t imageTag,
                                        bool allZerosBitmask)
{
  uint8_t message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    ((EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_MISSING_SEGMENTS_RESPONSE
      << EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
     | (otaProtocolStatus
        << EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));
  // Application status
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_APP_STATUS_OFFSET] =
    applicationStatus;
  // Image tag
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_TAG_OFFSET] =
    imageTag;

  if (allZerosBitmask) {
    MEMSET(message + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_BITMASK_OFFSET,
           0x00,
           MISSING_SEGMENTS_BITMASK_LENGTH);
  } else {
    uint8_t i;
    uint8_t *segmensBitmaskBufferPtr;

    assert(segmentBitmaskBuffer != EMBER_NULL_BUFFER);

#if defined(SL_CATALOG_KERNEL_PRESENT)
    emberAfPluginCmsisRtosAcquireBufferSystemMutex();
#endif

    segmensBitmaskBufferPtr = emberGetBufferPointer(segmentBitmaskBuffer);
    for (i = 0; i < MISSING_SEGMENTS_BITMASK_LENGTH; i++) {
      message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_BITMASK_OFFSET + i] =
        ~segmensBitmaskBufferPtr[i];
    }
  }

#if defined(SL_CATALOG_KERNEL_PRESENT)
  emberAfPluginCmsisRtosReleaseBufferSystemMutex();
#endif

  // We don't bother checking the return status here. If we fail, we simply wait
  // for another request from the server.
  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0, // messageTag
                   EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

static void handleIncomingMissingSegmentsRequest(EmberIncomingMessage *message)
{
  uint8_t imageTag =
    message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_TAG_OFFSET];
  uint32_t imageSize = emberFetchLowHighInt32u(message->payload
                                               + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_IMAGE_SIZE_OFFSET);
  uint16_t firstSegmentIndex =
    emberFetchLowHighInt16u(message->payload
                            + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_FIRST_INDEX_OFFSET);
  uint16_t lastSegmentIndex =
    emberFetchLowHighInt16u(message->payload
                            + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_MISSING_SEGS_REQ_LAST_INDEX_OFFSET);
  bool downloadFailed = false;

  // The node is currently downloading this image and the request is from a
  // legal server.
  if (clientInImageDownload()
      && imageInfo.tag == imageTag
      && serverOk(message->source)) {
    uint16_t blockIndex = firstSegmentIndex / MAX_SEGMENTS_IN_A_BLOCK;

    // This request is related to the current block or an older block.
    if (blockIndex <= imageInfo.blockIndex) {
      bool blockCompleted = (blockIndex == imageInfo.blockIndex
                             && currentBlockCompleted(firstSegmentIndex,
                                                      lastSegmentIndex));
      bool isLastBlock;

      // Store the actual image size.
      imageInfo.size = imageSize;

      // This check has to happen after we set the image size.
      isLastBlock = (blockIndex == (getTotalBlocksCount() - 1));

      sendMissingSegmentsResponse(message->source,
                                  ((isLastBlock && blockCompleted)
                                   ? EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED
                                   : EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING),
                                  EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS,
                                  imageTag,
                                  (blockIndex < imageInfo.blockIndex));
      if (blockCompleted) {
        // Move to the next block.
        imageInfo.blockIndex++;

        // We fully received the last block: success!
        if (isLastBlock) {
          historyTableAddEntry(EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED,
                               EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS);
          imageDownloadFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_SUCCESS);
        } else {
          // Still more blocks to process, reset the segments bitmask.
          initSegmentsBitmask();
        }
      }

      return;
    } else {
      // The block index is > than the current block index: this means that
      // somehow the server moved on to the next block while the client still
      // had missing segments on the current block: all we can do is to fail the
      // image download process.
      downloadFailed = true;

      historyTableAddEntry(EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED,
                           EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS);
    }
  }

  // Send response based on the history table if a matching entry is found,
  // otherwise send a default response.
  {
    uint8_t otaProtocolStatus, applicationErrorStatus;
    uint8_t historyTableEntryIndex = historyTableLookup(message->source,
                                                        imageTag);

    if (historyTableEntryIndex == 0xFF) {
      // No entry found: set protocol status and application status to default
      // values.
      otaProtocolStatus = EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED;
      applicationErrorStatus = EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS;
    } else {
      otaProtocolStatus = historyTable[historyTableEntryIndex].otaProtocolStatus;
      applicationErrorStatus = historyTable[historyTableEntryIndex].applicationStatus;
    }

    sendMissingSegmentsResponse(message->source,
                                otaProtocolStatus,
                                applicationErrorStatus,
                                imageTag,
                                true);
  }

  if (downloadFailed) {
    imageDownloadFinished(EMBER_OTA_BROADCAST_BOOTLOADER_STATUS_FAILED);
  }
}

//-------------------------------
// History table static functions

static void historyTableAddEntry(uint8_t otaProtocolStatus,
                                 uint8_t applicationStatus)
{
  uint8_t entryIndex = historyTableLookup(imageInfo.serverId, imageInfo.tag);
  bool updatingExistingEntry = true;

  // No matching entry in the table: look for a free entry or for an entry to
  // overwrite in case the table is full.
  if (entryIndex == 0xFF) {
    updatingExistingEntry = false;
    // Table is full: overwrite the oldest entry and adjust the head and tail
    // indexes accordingly (both head and tail indexes move forward).
    if (historyTableHeadIndex == historyTableTailIndex
        && historyTable[historyTableHeadIndex].serverId != EMBER_NULL_NODE_ID) {
      entryIndex = historyTableHeadIndex;
      historyTableHeadIndex = (historyTableHeadIndex + 1) % HISTORY_TABLE_SIZE;
      historyTableTailIndex = historyTableHeadIndex;
    } else {
      // Head index points to the next free entry: write that entry and move
      // forward the head index.
      entryIndex = historyTableHeadIndex;
      historyTableHeadIndex = (historyTableHeadIndex + 1) % HISTORY_TABLE_SIZE;
    }
  }

  assert(entryIndex < HISTORY_TABLE_SIZE);

  scriptCheckHistoryTableAdd(entryIndex);

  historyTable[entryIndex].serverId = imageInfo.serverId;
  historyTable[entryIndex].alternateServerId = imageInfo.alternateServerId;
  historyTable[entryIndex].imageTag = imageInfo.tag;
  historyTable[entryIndex].otaProtocolStatus = otaProtocolStatus;
  // Don't overwrite the application status if we are updating an existing entry
  // and the stored value is a valid value.
  if (!updatingExistingEntry
      || historyTable[entryIndex].applicationStatus
      == EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS) {
    historyTable[entryIndex].applicationStatus = applicationStatus;
  }
}

static uint8_t historyTableLookup(EmberNodeId serverId, uint8_t imageTag)
{
  uint8_t i;

  for (i = 0; i < HISTORY_TABLE_SIZE; i++) {
    if (historyTable[i].serverId != EMBER_NULL_NODE_ID
        && historyTable[i].imageTag == imageTag
        && (historyTable[i].serverId == serverId
            || historyTable[i].alternateServerId == serverId
            || historyTable[i].alternateServerId == EMBER_BROADCAST_ADDRESS)) {
      return i;
    }
  }

  return 0xFF;
}

static void historyTableRemoveEntry(uint8_t entryIndex)
{
  uint8_t i = (entryIndex + 1) % HISTORY_TABLE_SIZE;

  // Set the entry to delete as free.
  historyTable[entryIndex].serverId = EMBER_NULL_NODE_ID;

  // Move back all the entries up to the head index (not included).
  while (i != historyTableHeadIndex) {
    uint8_t destinationEntryIndex = (i > 0) ? i - 1 : HISTORY_TABLE_SIZE - 1;

    MEMCOPY(&historyTable[destinationEntryIndex],
            &historyTable[i],
            sizeof(EmHistoryEntry));

    i = (i + 1) % HISTORY_TABLE_SIZE;
  }

  // Move back the head index.
  historyTableHeadIndex = (historyTableHeadIndex > 0)
                          ? historyTableHeadIndex - 1
                          : HISTORY_TABLE_SIZE - 1;

#if defined(EMBER_SCRIPTED_TEST)
  {
    bool empty = (historyTableHeadIndex == historyTableTailIndex
                  && historyTable[historyTableHeadIndex].serverId
                  == EMBER_NULL_NODE_ID);
    scriptCheckHistoryTableRemove(entryIndex, (empty) ? 0xFF : historyTableTailIndex);
  }
#endif // EMBER_SCRIPTED_TEST
}

//------------------------------------------------------------------------------
// Target status request process related static function

static void sendTargetStatusResponse(EmberNodeId serverId,
                                     uint8_t applicationStatus)
{
  uint8_t message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    ((EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_TARGET_STATUS_RESPONSE
      << EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
     | (EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED
        << EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));
  // Application status
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_RESP_APP_STATUS_OFFSET] =
    applicationStatus;

  // We don't bother checking the return status here. If we fail, we simply wait
  // for another request from the server.
  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0, // messageTag
                   EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

static void handleIncomingTargetStatusRequest(EmberIncomingMessage *message)
{
  uint8_t serverApplicationStatus =
    message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_TARGET_STATUS_REQ_APP_STATUS_OFFSET];
  uint8_t responseApplicationStatus =
    EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_INVALID_APPLICATION_TARGET_STATUS;

  emberAfPluginOtaBootloaderClientIncomingRequestStatusCallback(message->source,
                                                                serverApplicationStatus,
                                                                &responseApplicationStatus);

  sendTargetStatusResponse(message->source, responseApplicationStatus);
}

//------------------------------------------------------------------------------
// Bootload request process related static function

static void sendBootloadResponse(EmberNodeId serverId,
                                 uint8_t imageTag,
                                 uint8_t applicationStatus,
                                 bool accepted)
{
  uint8_t message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    ((EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_RESPONSE
      << EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
     | (((accepted)
         ? EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED
         : EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED)
        << EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));
  // Image tag
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_TAG_OFFSET] = imageTag;
  // Application status
  message[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_APP_STATUS_OFFSET] =
    applicationStatus;

  // We don't bother checking the return status here. If we fail, we simply wait
  // for another request from the server.
  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_BROADCAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0,  // messageTag
                   EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

static void handleIncomingBootloadRequest(EmberIncomingMessage *message)
{
  uint8_t applicationStatus;
  uint8_t imageTag =
    message->payload[EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_TAG_OFFSET];
  uint32_t delayMs = emberFetchLowHighInt32u(message->payload
                                             + EMBER_OTA_BROADCAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_DELAY_OFFSET);
  bool accepted =
    emberAfPluginOtaBootloaderClientIncomingRequestBootloadCallback(message->source,
                                                                    imageTag,
                                                                    delayMs,
                                                                    &applicationStatus);

  sendBootloadResponse(message->source, imageTag, applicationStatus, accepted);
}
