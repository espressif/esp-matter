/***************************************************************************//**
 * @brief APIs/callbacks for ota-unicast-bootloader clients.
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

#include "ota-unicast-bootloader-client-config.h"

#include "stack/include/ember.h"
#include "hal/hal.h"

#include "ota-unicast-bootloader-client.h"
#include "ota-unicast-bootloader-client-internal.h"

//------------------------------------------------------------------------------
// Internal variables and static functions prototypes.

EmberEventControl emAfPluginOtaUnicastBootloaderClientEventControl;

static uint8_t internalState = STATE_OTA_CLIENT_IDLE;

// The index of the segment the client expects to receive
static uint32_t expectedSegmentIndex;

static struct {
  uint32_t size;
  EmberNodeId serverId;
  uint8_t tag;
  uint32_t startIndex;
} imageInfo;

// Image download process related static functions
static void imageDownloadFinished(EmberAfOtaUnicastBootloaderStatus status);
static void handleIncomingHandshake(EmberIncomingMessage *message);
static void handleIncomingImageSegment(EmberIncomingMessage *message);

// Bootload request process related static function
static void handleIncomingBootloadRequest(EmberIncomingMessage *message);
static void sendHandshakeResponse(EmberNodeId serverId,
                                  uint8_t imageTag,
                                  uint8_t responseStatus);
static void sendExtendedHandshakeResponse(EmberNodeId serverId,
                                          uint8_t imageTag,
                                          uint32_t segmentIndex,
                                          uint8_t responseStatus);
static void sendImageSegmentResponse(EmberNodeId serverId,
                                     uint8_t imageTag,
                                     uint32_t segmentIndex,
                                     uint8_t responseStatus);

//------------------------------------------------------------------------------
// Public APIs

EmberAfOtaUnicastBootloaderStatus emberAfPluginOtaUnicastBootloaderClientAbortImageDownload(uint8_t imageTag)
{
  if (clientInImageDownload()
      && imageTag == imageInfo.tag) {
    imageDownloadFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS;
  }

  return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL;
}

//------------------------------------------------------------------------------
// Implemented plugin callbacks

void emAfPluginOtaUnicastBootloaderClientInitCallback(void)
{
}

void emAfPluginOtaUnicastBootloaderClientIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint != EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT) {
    return;
  }

#if (EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_SECURITY_ENABLED > 0)
  // If security is enabled, discard all unsecured messages.
  if (!(message->options & EMBER_OPTIONS_SECURITY_ENABLED)) {
    return;
  }
#endif // EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_SECURITY_ENABLED > 0

  // Check which command ID has arrived
  switch (emOtaUnicastBootloaderProtocolCommandId(message->payload)) {
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE:
      if (message->length
          == EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_HEADER_LENGTH) {
        handleIncomingHandshake(message);
      }
      break;

    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT:
      if (message->length
          > EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH) {
        handleIncomingImageSegment(message);
      }
      break;
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST:
      if (message->length
          == EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH) {
        handleIncomingBootloadRequest(message);
      }
      break;
  }
}

void emAfPluginOtaUnicastBootloaderClientEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginOtaUnicastBootloaderClientEventControl);

  // We timed-out waiting for a message from the server(s): fail the ongoing
  // image download process.
  if (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING) {
    imageDownloadFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_TIMEOUT);
  }
}

//------------------------------------------------------------------------------
// Image download process related static functions

static void imageDownloadFinished(EmberAfOtaUnicastBootloaderStatus status)
{
  internalState = STATE_OTA_CLIENT_IDLE;
  emberEventControlSetInactive(emAfPluginOtaUnicastBootloaderClientEventControl);
  emberAfPluginOtaUnicastBootloaderClientImageDownloadCompleteCallback(status,
                                                                       imageInfo.tag,
                                                                       ((status == EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS)
                                                                        ? imageInfo.size
                                                                        : 0));
}

static bool serverOk(EmberNodeId serverId)
{
  return (serverId == imageInfo.serverId);
}

static uint32_t getTotalSegmentsCount(void)
{
  uint32_t totalSegments =
    (uint32_t)(imageInfo.size / MAX_SEGMENT_PAYLOAD_LENGTH);

  assert(imageInfo.size > 0);

  if ((imageInfo.size % MAX_SEGMENT_PAYLOAD_LENGTH) > 0) {
    totalSegments++;
  }

  return totalSegments;
}

static void handleIncomingHandshake(EmberIncomingMessage *message)
{
  EmberNodeId serverId = message->source;
  uint8_t imageTag =
    message->payload[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_TAG_OFFSET];

  // Client is idle: see if the application is interested in receiving this image.
  if (clientIsIdle()) {
    // Default to start the download at the beginning of the image. The
    // application can modify this within the callback.
    imageInfo.startIndex = 0;
    imageInfo.serverId = serverId;
    imageInfo.tag = imageTag;
    imageInfo.size =
      emberFetchLowHighInt32u(message->payload + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_LENGTH_OFFSET);

    if (!emberAfPluginOtaUnicastBootloaderClientNewIncomingImageCallback(imageInfo.serverId,
                                                                         imageInfo.tag,
                                                                         imageInfo.size,
                                                                         &imageInfo.startIndex)) {
      // Respond to the server with a REFUSED status
      sendHandshakeResponse(serverId,
                            imageTag,
                            EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED);
      imageDownloadFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED);

      return;
    }

    if (emOtaUnicastBootloaderProtocolResumeDownloadSupported(message->payload)) {
      expectedSegmentIndex = imageInfo.startIndex / MAX_SEGMENT_PAYLOAD_LENGTH;
    } else {
      // The server does not support download resume: start from the beginning.
      expectedSegmentIndex = 0;
      imageInfo.startIndex = 0;
    }

    // The application has requested to resume the download at an invalid
    // point of the image. Abort the download.
    if (imageInfo.startIndex >= imageInfo.size) {
      // Respond to the server with a FAILED status
      sendHandshakeResponse(serverId,
                            imageTag,
                            EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_FAILED);
      imageDownloadFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED);
    } else {
      internalState = STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING;
      // Respond to the server with a ONGOING status
      if (expectedSegmentIndex > 0) {
        sendExtendedHandshakeResponse(serverId,
                                      imageTag,
                                      expectedSegmentIndex,
                                      EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING);
      } else {
        sendHandshakeResponse(serverId,
                              imageTag,
                              EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING);
      }
    }
  } else {
    // unexpected handshake from a different server
    if (!serverOk(serverId)) {
      // Handshake from a different server, refuse it and go on.
      sendHandshakeResponse(serverId,
                            imageTag,
                            EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED);
    }
  }
}

static void sendHandshakeResponse(EmberNodeId serverId,
                                  uint8_t imageTag,
                                  uint8_t responseStatus)
{
  uint8_t message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    (  (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE_RESPONSE
        << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
       | (responseStatus
          << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));

  // Image tag
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_TAG_OFFSET] =
    imageTag;

  // Application status: deprecated field.
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_STATUS_OFFSET] = 0xFF;

  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0, // messageTag
                   EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

static void sendExtendedHandshakeResponse(EmberNodeId serverId,
                                          uint8_t imageTag,
                                          uint32_t segmentIndex,
                                          uint8_t responseStatus)
{
  uint8_t message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    ((EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_EXT_HANDSHAKE_RESPONSE
      << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
     | (responseStatus
        << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));

  // Image tag
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_TAG_OFFSET] =
    imageTag;

  // Segment Index
  emberStoreLowHighInt32u(message + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_SEGMENT_INDEX_OFFSET,
                          segmentIndex);

  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0, // messageTag
                   EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

static void handleIncomingImageSegment(EmberIncomingMessage *message)
{
  EmberNodeId serverId = message->source;
  uint8_t imageTag =
    message->payload[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_TAG_OFFSET];
  uint32_t segmentIndex =
    emberFetchLowHighInt32u(message->payload + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_INDEX_OFFSET);
  uint8_t segmentLength = message->length
                          - EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH;
  uint32_t startIndex = segmentIndex * MAX_SEGMENT_PAYLOAD_LENGTH;
  uint32_t endIndex = startIndex + segmentLength - 1;

  // Client is idle: image segments shall not be recevied without handshake
  if (clientIsIdle()) {
    return;
  }

  // Check if segment is expected and coming from the correct server and image
  if (internalState == STATE_OTA_CLIENT_IMAGE_DOWNLOAD_WAITING) {
    if ( (serverOk(serverId)) && (imageTag == imageInfo.tag)) {
      // Segment of expected image from expected server.

      // Check if segment is in sequence.
      if ( segmentIndex <= expectedSegmentIndex ) {
        // Segment OK or already received, don't abort

        if (segmentIndex == expectedSegmentIndex) {
          uint8_t startIndexOffset = 0;

          // Account for the application possibly wanting to resume in the
          // middle of a segment.
          if (imageInfo.startIndex > startIndex) {
            startIndexOffset = imageInfo.startIndex - startIndex;
          }

          // Account for the last segment which may very well be a partial
          // segment.
          if (endIndex >= imageInfo.size) {
            endIndex = imageInfo.size - 1;
          }

          // Segment is the one we expected, pass it to the application
          emberAfPluginOtaUnicastBootloaderClientIncomingImageSegmentCallback(serverId,
                                                                              startIndex + startIndexOffset,
                                                                              endIndex,
                                                                              imageTag,
                                                                              (message->payload
                                                                               + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_PAYLOAD_OFFSET
                                                                               + startIndexOffset));
          // expecting the next segment
          expectedSegmentIndex++;
        }
        // Last segment?
        if ( segmentIndex ==  (getTotalSegmentsCount() - 1) ) {
          // Yes
          sendImageSegmentResponse(serverId,
                                   imageTag,
                                   segmentIndex,
                                   EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED);
          // Finish with SUCCESS status
          imageDownloadFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS);
        } else {
          // Not the last segment
          // Send ONGOING response
          sendImageSegmentResponse(serverId,
                                   imageTag,
                                   segmentIndex,
                                   EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING);
        }

        // Refresh the download timeout.
        emberEventControlSetDelayQS(emAfPluginOtaUnicastBootloaderClientEventControl,
                                    EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_IMAGE_DOWNLOAD_TIMEOUT_S * 4);
      } else {
        // Segment is ahead of where we expect it to be from
        // -> might have missed some segments
        // Abort with OUT_OF_SEQ
        sendImageSegmentResponse(serverId,
                                 imageTag,
                                 segmentIndex,
                                 EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OUT_OF_SEQ);
        // Abort with FAILED status.
        imageDownloadFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED);
      }
    } else {
      // Unexptected image tag or server ID, no processing
      // Reply with REFUSED
      sendImageSegmentResponse(serverId,
                               imageTag,
                               segmentIndex,
                               EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED);
    }
  }
}

static void sendImageSegmentResponse(EmberNodeId serverId,
                                     uint8_t imageTag,
                                     uint32_t segmentIndex,
                                     uint8_t responseStatus)
{
  uint8_t message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    (  (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT_RESPONSE
        << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
       | (responseStatus
          << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));

  // Image tag
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_TAG_OFFSET] =
    imageTag;

  // Segment Index
  emberStoreLowHighInt32u(message + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_INDEX_OFFSET,
                          segmentIndex);

  // Application status: deprecated field.
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_STATUS_OFFSET] = 0xFF;

  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0, // messageTag
                   EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

//------------------------------------------------------------------------------
// Bootload request process related static function

static void sendBootloadResponse(EmberNodeId serverId,
                                 uint8_t imageTag,
                                 bool accepted)
{
  uint8_t message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH];

  // Frame control
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    ((EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_RESPONSE
      << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_COMMAND_ID_OFFSET)
     | (((accepted)
         ? EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED
         : EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED)
        << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OFFSET));
  // Image tag
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_TAG_OFFSET] = imageTag;

  // The deprecated "application status" field needs to be set to the
  // legacy EmberAfOtaUnicastBootloaderTargetStatus enum values because the
  // legacy version of the server look at this field instead of the response
  // field in the frame control.
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_APP_STATUS_OFFSET] =
    ((accepted)
     ? LEGACY_TARGET_STATUS_BOOTLOAD_REQUEST_ACCEPTED
     : LEGACY_TARGET_STATUS_BOOTLOAD_REQUEST_REFUSED);

  // We don't bother checking the return status here. If we fail, we simply wait
  // for another request from the server.
  emberMessageSend(serverId,
                   EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_CLIENT_ENDPOINT,
                   0,  // messageTag
                   EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH,
                   message,
                   UNICAST_TX_OPTIONS);
}

static void handleIncomingBootloadRequest(EmberIncomingMessage *message)
{
  uint8_t imageTag =
    message->payload[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_TAG_OFFSET];
  uint32_t delayMs =
    emberFetchLowHighInt32u(message->payload + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_DELAY_OFFSET);
  bool accepted =
    emberAfPluginOtaUnicastBootloaderClientIncomingRequestBootloadCallback(message->source,
                                                                           imageTag,
                                                                           delayMs);

  sendBootloadResponse(message->source, imageTag, accepted);
}
