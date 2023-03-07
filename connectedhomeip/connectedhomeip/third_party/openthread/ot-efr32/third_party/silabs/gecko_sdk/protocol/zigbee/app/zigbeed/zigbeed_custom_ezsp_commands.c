/***************************************************************************//**
 * @file
 * @brief This callback file is created for your convenience. You may edit application code in this file.
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

#include PLATFORM_HEADER
#include EMBER_AF_API_EMBER_TYPES
#include EMBER_AF_API_HAL

#include "app/ncp/plugin/xncp/xncp.h"
#include "event_queue/event-queue.h"

/* This sample application demostrates Zigbeed using a custom protocol to
 * communicate with the host. As an example protocol, the Zigbeed has defined
 * commands so that the host can control an event/functionality on Zigbeed.
 *
 * The host sends custom EZSP commands to Zigbeed, and Zigbeed acts on them
 * based on the functionality in the code found below.
 * This sample application is meant to be paired with the XncpSensorHost
 * sample application in the ZCL Application Framework.
 */

// -----------------------------------------------------------------------------
// Declarations

#define ZIGBEED_VERSION_NUMBER  0x1234
#define ZIGBEED_MANUFACTURER_ID 0xABCD

// The CUSTOM protocol custom EZSP frames are no longer than 5 bytes.
// These consist of 1 command byte and possibly 4 parameter bytes.
#define CUSTOM_PROTOCOL_MAX_FRAME_LENGTH (5)

// Each custom EZSP frame starts with a command.
#define CUSTOM_PROTOCOL_COMMAND_INDEX (0)

// If an CUSTOM protocol command has a response, it will be at the beginning of
// the response buffer.
#define CUSTOM_PROTOCOL_RESPONSE_INDEX (0)

// Each command is a byte.
enum {
  CUSTOM_PROTOCOL_COMMAND_SET_CUSTOM    = 0x00,
  CUSTOM_PROTOCOL_COMMAND_CLEAR_CUSTOM  = 0x01,
  CUSTOM_PROTOCOL_COMMAND_STROBE_CUSTOM = 0x02,

  // The response to this command contains a byte corresponding to one of the
  // three enum values above.
  CUSTOM_PROTOCOL_COMMAND_GET_CUSTOM    = 0x03,
};
typedef uint8_t LedProtocolCommand;

static uint8_t customState;
static uint32_t customEventHandlerDelayMS = MILLISECOND_TICKS_PER_SECOND;

// -----------------------------------------------------------------------------
// Custom event
extern EmberEventQueue emAppEventQueue;
void customEventHandler(EmberEvent *event);
static EmberEvent customEvent = {
  {
    &emAppEventQueue,
    customEventHandler,
    NULL,
    "Custom Event"
  },
  NULL
};

// Custom event handler.
void customEventHandler(EmberEvent *event)
{
  switch (customState) {
    default:
      ;
  }

  emberEventSetDelayMs(&customEvent, customEventHandlerDelayMS);
}

// -----------------------------------------------------------------------------
// Callbacks

/** @brief Main Init
 *
 * This function is calcustom when the application starts and can be used to
 * perform any additional initialization required at system startup.
 */
void emberAfMainInitCallback(void)
{
  emberEventSetActive(&customEvent);
}

/** @brief Incoming Custom EZSP Message Callback
 *
 * This function is calcustom when Zigbeed receives a custom EZSP message from the
 * HOST.  The message length and payload is passed to the callback in the first
 * two arguments.  The implementation can then fill in the replyPayload and set
 * the replayPayloadLength to the number of bytes in the replyPayload.
 * See documentation for the function ezspCustomFrame on sending these messages
 * from the HOST.
 *
 * @param messageLength The length of the messagePayload.
 * @param messagePayload The custom message that was sent from the HOST.
 * Ver.: always
 * @param replyPayloadLength The length of the replyPayload.  This needs to be
 * set by the implementation in order for a properly formed respose to be sent
 * back to the HOST. Ver.: always
 * @param replyPayload The custom message to send back to the HOST in respose
 * to the custom message. Ver.: always
 *
 * @return An ::EmberStatus indicating the result of the custom message
 * handling.  This returned status is always the first byte of the EZSP
 * response.
 */
EmberStatus emberAfPluginXncpIncomingCustomFrameCallback(uint8_t messageLength,
                                                         uint8_t *messagePayload,
                                                         uint8_t *replyPayloadLength,
                                                         uint8_t *replyPayload)
{
  // First byte is the command ID.
  uint8_t commandId = messagePayload[CUSTOM_PROTOCOL_COMMAND_INDEX];
  EmberStatus status = EMBER_SUCCESS;
  *replyPayloadLength = 0;

  switch (commandId) {
    case CUSTOM_PROTOCOL_COMMAND_SET_CUSTOM:
      customState = CUSTOM_PROTOCOL_COMMAND_SET_CUSTOM;
      break;
    case CUSTOM_PROTOCOL_COMMAND_CLEAR_CUSTOM:
      customState = CUSTOM_PROTOCOL_COMMAND_CLEAR_CUSTOM;
      break;
    case CUSTOM_PROTOCOL_COMMAND_STROBE_CUSTOM:
      customState = CUSTOM_PROTOCOL_COMMAND_STROBE_CUSTOM;
      break;
    case CUSTOM_PROTOCOL_COMMAND_GET_CUSTOM:
      replyPayload[0] = customState;
      *replyPayloadLength += sizeof(customState);
      break;
    default:
      status = EMBER_INVALID_CALL;
  }

  return status;
}

/** @brief Get Zigbeed Information
 *
 * This callback enables users to communicate the version number and
 * manufacturer ID of their Zigbeed application to the framework. This information
 * is needed for the EZSP command frame calcustom getXncpInfo. This callback will
 * be called when that frame is received so that the application can report
 * its version number and manufacturer ID to be sent back to the HOST.
 *
 * @param versionNumber The version number of the Zigbeed application.
 * @param manufacturerId The manufacturer ID of the Zigbeed application.
 */
void emberAfPluginXncpGetXncpInformation(uint16_t *manufacturerId,
                                         uint16_t *versionNumber)
{
  *versionNumber = ZIGBEED_VERSION_NUMBER;
  *manufacturerId = ZIGBEED_MANUFACTURER_ID;
}
