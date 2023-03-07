/***************************************************************************//**
 * @brief Macros and includes for ota-unicast-bootloader servers.
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

#include "ota-unicast-bootloader-server-config.h"

#include "stack/include/ember.h"
#include "hal/hal.h"

#include "ota-unicast-bootloader-server.h"
#include "ota-unicast-bootloader-server-internal.h"

// Refer to Jira FLEX-868 for more info on this.
#if (defined(EMBER_TEST) && defined(UNIX_HOST))
#undef EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS
#define EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS 2500
#endif

//------------------------------------------------------------------------------
// Internal variables and static functions prototypes.

EmberEventControl emAfPluginOtaUnicastBootloaderServerEventControl;

static uint8_t internalState = STATE_OTA_SERVER_IDLE;
static EmberNodeId targetId = EMBER_NULL_NODE_ID;

// This is the last segment index that the client received.
static uint32_t sentSegment;
// This keeps track of stack errors such as send() API failures or other non-ACK
// related failures.
static uint8_t stackErrorsCount;
// This keeps track of target errors such as not receiving an ACK or not
// receiving an expected response.
static uint8_t currentTargetErrorsCount;
// The next segment index (if the server is currently sending segments)
static uint32_t nextSegment;
// Stores the current image size in bytes or the bootload time (ms).
static uint32_t currentImageSizeOrBootloadTimeMs;
// Stores the current image tag (image distribution process) or the or the
// current server status (target status request process).
static uint8_t currentImageTagOrServerStatus;

// Image distribution process static functions
static void scheduleImageDistributionProcessNextTask(bool newSegmentOrNewTarget);
static void imageDistributionProcessFinished(EmberAfOtaUnicastBootloaderStatus status);
static uint32_t getTotalSegmentsCount(void);
static void handleSegmentResponse(EmberIncomingMessage *message);
static void handleHandshakeResponse(EmberIncomingMessage *message,
                                    uint32_t startSegment);
static void unicastHandshake(void);
static void unicastNextSegment(void);

// Bootload request process static functions.
static void requestTargetForBootload(void);
static void scheduleBootloadRequestProcessNextTask(bool targetResponeded,
                                                   uint8_t targetStatusResponse);
static void bootloadRequestProcessFinished(EmberAfOtaUnicastBootloaderStatus status);

//------------------------------------------------------------------------------
// Public APIs

EmberAfOtaUnicastBootloaderStatus emberAfPluginOtaUnicastBootloaderServerInitiateImageDistribution(
  EmberNodeId target,
  uint32_t imageSize,
  uint8_t imageTag)
{
  if ( !serverIsIdle() ) {
    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_BUSY;
  }

  if ((EMBER_NULL_NODE_ID == target)
      || (imageSize == 0)
      || (imageSize > MAXIMUM_IMAGE_SIZE)) {
    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL;
  }

  currentImageSizeOrBootloadTimeMs = imageSize;
  currentImageTagOrServerStatus = imageTag;
  targetId = target;

  nextSegment = 0;
  stackErrorsCount = 0;
  currentTargetErrorsCount = 0;

  internalState = STATE_OTA_SERVER_HANDSHAKE_INTERVAL;

  emberEventControlSetActive(emAfPluginOtaUnicastBootloaderServerEventControl);

  return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS;
}

EmberAfOtaUnicastBootloaderStatus emberAfPluginUnicastBootloaderServerInitiateRequestTargetBootload(
  uint32_t bootloadDelayMs,
  uint8_t imageTag,
  EmberNodeId target
  )
{
  if (!serverIsIdle()) {
    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_BUSY;
  }

  if (target == EMBER_NULL_NODE_ID) {
    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL;
  }

  targetId = target;
  nextSegment = 0;
  stackErrorsCount = 0;
  currentTargetErrorsCount = 0;
  currentImageTagOrServerStatus = imageTag;
  currentImageSizeOrBootloadTimeMs = halCommonGetInt32uMillisecondTick()
                                     + bootloadDelayMs;
  internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL;
  emberEventControlSetActive(emAfPluginOtaUnicastBootloaderServerEventControl);

  return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS;
}

EmberAfOtaUnicastBootloaderStatus emberAfPluginOtaUnicastBootloaderServerAbortCurrentProcess(void)
{
  if (serverInImageDistributionProcess()) {
    imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS;
  }

  if (serverInBootloadRequestProcess()) {
    bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_ABORTED);

    return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS;
  }

  return EMBER_OTA_UNICAST_BOOTLOADER_STATUS_INVALID_CALL;
}

//------------------------------------------------------------------------------
// Implemented plugin callbacks

void emAfPluginOtaUnicastBootloaderServerIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint != EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT) {
    return;
  }

#if (EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_SECURITY_ENABLED > 0)
  // If security is enabled, discard all unsecured messages.
  if (!(message->options & EMBER_OPTIONS_SECURITY_ENABLED)) {
    return;
  }
#endif // EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_SECURITY_ENABLED > 0

  // Discard all responses from nodes other than the target node.
  if (message->source != targetId) {
    return;
  }

  switch (emOtaUnicastBootloaderProtocolCommandId(message->payload)) {
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE_RESPONSE:
      // Check the response (Expecting one? Same Tag we're distributing?)
      if ((internalState == STATE_OTA_SERVER_HANDSHAKE_WAITING_RESPONSE)
          && (currentImageTagOrServerStatus
              == message->payload[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_RESP_TAG_OFFSET]) ) {
        handleHandshakeResponse(message, 0);
      }
      break;
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_EXT_HANDSHAKE_RESPONSE:
      // Check the response (Expecting one? Same Tag we're distributing?)
      if ((internalState == STATE_OTA_SERVER_HANDSHAKE_WAITING_RESPONSE)
          && (currentImageTagOrServerStatus
              == message->payload[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_TAG_OFFSET]) ) {
        uint32_t startSegment =
          emberFetchLowHighInt32u(message->payload + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_EXT_HSHAKE_RESP_SEGMENT_INDEX_OFFSET);
        handleHandshakeResponse(message, startSegment);
      }
      break;
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT_RESPONSE:
      // Check the response (Expecting one? Same Tag we're distributing?)
      if ((internalState == STATE_OTA_SERVER_SEGMENT_UNICAST_PENDING
           || internalState == STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE)
          && (currentImageTagOrServerStatus == message->payload[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_TAG_OFFSET]) ) {
        // Set the internal state to WAITING_RESPONSE in case we received the
        // response before the sent() callback for the segment message.
        internalState = STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE;
        handleSegmentResponse(message);
      }
      break;
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_RESPONSE:
      if ((internalState == STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE)
          && (message->length == EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_RESP_HEADER_LENGTH)) {
        uint8_t targetResponseStatus =
          emOtaUnicastBootloaderProtocolResponseStatus(message->payload);
        scheduleBootloadRequestProcessNextTask(true, targetResponseStatus);
      }
      break;
  }
}

void emAfPluginOtaUnicastBootloaderServerMessageSentCallback(EmberStatus status,
                                                             EmberOutgoingMessage *message)
{
  if (message->endpoint != EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT) {
    return;
  }

  switch (emOtaUnicastBootloaderProtocolCommandId(message->payload)) {
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE:
      if (internalState != STATE_OTA_SERVER_HANDSHAKE_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, bump segment counter and reset the
        // tx error counter.
        stackErrorsCount = 0;
        internalState = STATE_OTA_SERVER_HANDSHAKE_WAITING_RESPONSE;
        emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS);
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
        internalState = STATE_OTA_SERVER_HANDSHAKE_INTERVAL;
        scheduleImageDistributionProcessNextTask(false);
      }
      break;

    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT:
      if (internalState != STATE_OTA_SERVER_SEGMENT_UNICAST_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, bump segment counter and reset the
        // tx  error counter.
        stackErrorsCount = 0;
        internalState = STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE;
        emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS);
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
        internalState = STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL;
        scheduleImageDistributionProcessNextTask(false);
      }
      break;
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST:
      if (internalState != STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_PENDING) {
        return;
      }

      if (status == EMBER_SUCCESS) {
        // Message was sent out successfully, wait for the corresponding bootload
        // response  and reset the stack errors count.
        internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE;
        emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_RESPONSE_TIMEOUT_MS);
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

        scheduleBootloadRequestProcessNextTask(false, 0xFF);
      }
      break;
  }
}

void emAfPluginOtaUnicastBootloaderServerEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginOtaUnicastBootloaderServerEventControl);

  switch (internalState) {
    case STATE_OTA_SERVER_HANDSHAKE_INTERVAL:
      unicastHandshake();
      break;
    case STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL:
      unicastNextSegment();
      break;
    case STATE_OTA_SERVER_HANDSHAKE_WAITING_RESPONSE:
    case STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE:
      // No response from the target: bump the target's error count and schedule
      // the next task.
      currentTargetErrorsCount++;
      scheduleImageDistributionProcessNextTask(false);
      break;
    case STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL:
      requestTargetForBootload();
      break;
    case STATE_OTA_SERVER_BOOTLOAD_REQUEST_WAITING_RESPONSE:
      // No response from the target: bump the target's error count and schedule
      // the next task.
      currentTargetErrorsCount++;
      scheduleBootloadRequestProcessNextTask(false, 0xFF);
      break;
  }
}

//------------------------------------------------------------------------------
// Image distribution process static functions

static void handleSegmentResponse(EmberIncomingMessage *message)
{
  uint32_t respSegment;
  // Check if client aborted or refused the image distribution
  if (emOtaUnicastBootloaderProtocolResponseStatus(message->payload)
      ==  EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_OUT_OF_SEQ) {
    // Client reports out-of-sequence segment
    imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED);
  } else if (emOtaUnicastBootloaderProtocolResponseStatus(message->payload)
             ==  EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED) {
    // Image refused by the client
    imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED);
  } else if (emOtaUnicastBootloaderProtocolResponseStatus(message->payload)
             ==  EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING) {
    // Response ok and client approved segment
    respSegment =
      emberFetchLowHighInt32u(message->payload + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_RESP_INDEX_OFFSET);
    if (respSegment == sentSegment) {
      currentTargetErrorsCount = 0;
      emberEventControlSetInactive(emAfPluginOtaUnicastBootloaderServerEventControl);
      // Segment index match, schedule next segment
      scheduleImageDistributionProcessNextTask(true);
    } else {
      // Segment index mismatch
      imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED);
    }
  } else if (emOtaUnicastBootloaderProtocolResponseStatus(message->payload)
             ==  EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED) {
    // Image distibution complete
    imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS);
  }
}

static void handleHandshakeResponse(EmberIncomingMessage *message,
                                    uint32_t startSegment)
{
  switch (emOtaUnicastBootloaderProtocolResponseStatus(message->payload)) {
    // Image refused by the client
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED:
      imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED);
      break;
    // Image accepted, Sschedule first image segment
    case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_ONGOING:
      nextSegment = startSegment;
      currentTargetErrorsCount = 0;
      internalState = STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL;
      scheduleImageDistributionProcessNextTask(true);
      break;
    default:
      // Treat any other response status as a generic failure
      imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED);
  }
}

static void unicastHandshake(void)
{
  uint8_t message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_HEADER_LENGTH];
  EmberStatus status;

  // Frame control
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_HANDSHAKE
     | (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAG_RESUME_SUPPORT
        << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_OFFSET));
  // Image tag
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_TAG_OFFSET] =
    currentImageTagOrServerStatus;
  // Image size
  emberStoreLowHighInt32u(message + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_LENGTH_OFFSET,
                          currentImageSizeOrBootloadTimeMs);

  status = emberMessageSend(targetId,
                            EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT,
                            0, // messageTag
                            (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_HANDSHAKE_HEADER_LENGTH),
                            message,
                            UNICAST_TX_OPTIONS);

  if (status == EMBER_SUCCESS) {
    // Wait for the messageSent() corresponding call.
    internalState = STATE_OTA_SERVER_HANDSHAKE_PENDING;
  } else {
    // If we failed submitting a message to the stack, we increase the tx
    // count and try again.
    stackErrorsCount++;
    scheduleImageDistributionProcessNextTask(false);
  }
}

static void unicastNextSegment(void)
{
  uint8_t message[MAX_APPLICATION_PAYLOAD_LENGTH];
  uint32_t startIndex = nextSegment * MAX_SEGMENT_PAYLOAD_LENGTH;
  uint32_t endIndex = startIndex + MAX_SEGMENT_PAYLOAD_LENGTH - 1;

  // Account for the last segment which may very well be a partial segment.
  if (endIndex >= currentImageSizeOrBootloadTimeMs) {
    endIndex = currentImageSizeOrBootloadTimeMs - 1;
  }

  if (!emberAfPluginOtaUnicastBootloaderServerGetImageSegmentCallback(startIndex,
                                                                      endIndex,
                                                                      currentImageTagOrServerStatus,
                                                                      message + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH)) {
    imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_DATA_UNDERFLOW);
  } else {
    EmberStatus status;

    // Frame control
    message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
      (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_IMAGE_SEGMENT
       | (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAG_RESUME_SUPPORT
          << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_OFFSET));
    // Image tag
    message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_TAG_OFFSET] =
      currentImageTagOrServerStatus;
    // Segment index
    emberStoreLowHighInt32u(message + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_INDEX_OFFSET,
                            nextSegment);

    status = emberMessageSend(targetId,
                              EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT,
                              0, // messageTag
                              (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_IMAGE_SEGMENT_HEADER_LENGTH
                               + endIndex - startIndex + 1),
                              message,
                              UNICAST_TX_OPTIONS);

    if (status == EMBER_SUCCESS) {
      // Wait for the messageSent() corresponding call.
      internalState = STATE_OTA_SERVER_SEGMENT_UNICAST_PENDING;
      // Let's store the last segment that was sent
      sentSegment = nextSegment;
    } else {
      // If we failed submitting a message to the stack, we increase the tx
      // count and try again if we still have tries left.
      stackErrorsCount++;
      scheduleImageDistributionProcessNextTask(false);
    }
  }
}

static void scheduleImageDistributionProcessNextTask(bool newSegment)
{
  switch (internalState) {
    case STATE_OTA_SERVER_HANDSHAKE_INTERVAL:
    case STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL:
      if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
        imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR);
        return;
      }
      if (currentTargetErrorsCount >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
        imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE);
        return;
      }

      emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                  EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
      break;
    case STATE_OTA_SERVER_SEGMENT_UNICAST_WAITING_RESPONSE:
      if (newSegment) {
        // Send next segment if any left
        if (nextSegment < getTotalSegmentsCount() ) {
          // Next segment
          nextSegment++;
        } else {
          // We sent all the segments
          nextSegment = 0;
          stackErrorsCount = 0;
          currentTargetErrorsCount = 0;
          return;
        }
      }
      internalState = STATE_OTA_SERVER_SEGMENT_UNICAST_INTERVAL;
      // Schedule next transmission if maximum attempts not yet reached
      // if the target does not respond to the handshake repeatedly, abort the process
      if (currentTargetErrorsCount >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
        imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE);
      } else {
        emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
      }
      break;
    case STATE_OTA_SERVER_HANDSHAKE_WAITING_RESPONSE:
      // if the target does not respond to the handshake repeatedly, abort the process
      if (currentTargetErrorsCount >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
        imageDistributionProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE);
      } else {
        // Try again
        internalState = STATE_OTA_SERVER_HANDSHAKE_INTERVAL;
        emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                    EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
      }
      break;
    default:
      assert(0);
  }
}

static void imageDistributionProcessFinished(EmberAfOtaUnicastBootloaderStatus status)
{
  emberEventControlSetInactive(emAfPluginOtaUnicastBootloaderServerEventControl);

  internalState = STATE_OTA_SERVER_IDLE;

  emberAfPluginOtaUnicastBootloaderServerImageDistributionCompleteCallback(status);
}

static uint32_t getTotalSegmentsCount(void)
{
  uint32_t totalSegments =
    (uint32_t)(currentImageSizeOrBootloadTimeMs / MAX_SEGMENT_PAYLOAD_LENGTH);

  if ((currentImageSizeOrBootloadTimeMs % MAX_SEGMENT_PAYLOAD_LENGTH) > 0) {
    totalSegments++;
  }

  return totalSegments;
}

// -----------------------------------------------------------------------------
// Bootload request process static functions

static void requestTargetForBootload(void)
{
  uint8_t message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH];
  uint32_t nowMs = halCommonGetInt32uMillisecondTick();
  EmberStatus status;
  uint32_t delayMs;

  if (timeGTorEqualInt32u(nowMs, currentImageSizeOrBootloadTimeMs)) {
    delayMs = 0;
  } else {
    delayMs = elapsedTimeInt32u(nowMs, currentImageSizeOrBootloadTimeMs);
  }

  // Frame control
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_OFFSET] =
    (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_COMMAND_ID_BOOTLOAD_REQUEST
     | (EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAG_RESUME_SUPPORT
        << EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_REQ_FLAGS_OFFSET));
  // Image tag
  message[EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_TAG_OFFSET] =
    currentImageTagOrServerStatus;
  // Delay
  emberStoreLowHighInt32u(message + EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_DELAY_OFFSET,
                          delayMs);

  status = emberMessageSend(targetId,
                            EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_ENDPOINT,
                            0, // messageTag
                            EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_BOOTLOAD_REQ_HEADER_LENGTH,
                            message,
                            UNICAST_TX_OPTIONS);

  if (status == EMBER_SUCCESS) {
    // Wait for the messageSent() corresponding call.
    internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_PENDING;
  } else {
    // If we failed submitting a message to the stack, we increase the tx
    // count and try again.
    stackErrorsCount++;
    scheduleBootloadRequestProcessNextTask(false, 0xFF);
  }
}

static void scheduleBootloadRequestProcessNextTask(bool targetResponeded,
                                                   uint8_t targetResponseStatus)
{
  assert(serverInBootloadRequestProcess());

  if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
    bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR);
    return;
  }

  if (targetResponeded) {
    // We completed the bootload request process, let's see what the outcome was.
    switch (targetResponseStatus) {
      case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_COMPLETED:
        bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_SUCCESS);
        break;
      case EMBER_OTA_UNICAST_BOOTLOADER_PROTOCOL_FRAME_CONTROL_RESP_STATUS_REFUSED:
        bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_REFUSED);
        break;
      default:
        // Unexpected response value
        bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_FAILED);
    }
  } else {
    if (stackErrorsCount >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_STACK_ERRORS) {
      bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_STACK_ERROR);
      return;
    }
    if (currentTargetErrorsCount
        >= EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_MAX_UNICAST_ERRORS) {
      bootloadRequestProcessFinished(EMBER_OTA_UNICAST_BOOTLOADER_STATUS_UNREACHABLE);
      return;
    }

    // Try/re-try
    internalState = STATE_OTA_SERVER_BOOTLOAD_REQUEST_UNICAST_INTERVAL;
    emberEventControlSetDelayMS(emAfPluginOtaUnicastBootloaderServerEventControl,
                                EMBER_AF_PLUGIN_OTA_UNICAST_BOOTLOADER_SERVER_TX_INTERVAL_MS);
  }
}

static void bootloadRequestProcessFinished(EmberAfOtaUnicastBootloaderStatus status)
{
  emberEventControlSetInactive(emAfPluginOtaUnicastBootloaderServerEventControl);

  internalState = STATE_OTA_SERVER_IDLE;
  emberAfPluginOtaUnicastBootloaderServerRequestTargetBootloadCompleteCallback(status);
}
