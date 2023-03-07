/***************************************************************************//**
 * @brief APIs for mailbox client.
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

#include "mailbox-client-config.h"

#include "stack/include/ember.h"
#include "hal/hal.h"

#include "mailbox-client.h"

#define CLIENT_STATE_INITIAL                        0x00
#define CLIENT_STATE_SUBMIT_PENDING                 0x01
#define CLIENT_STATE_CHECK_MAILBOX_PENDING          0x02

//------------------------------------------------------------------------------
// Static and global variables and forward declarations.

EmberEventControl emAfPluginMailboxClientEventControl;

static uint8_t internalState = CLIENT_STATE_INITIAL;

static struct {
  EmberNodeId server;
  EmberNodeId destination;
  uint8_t tag;
} pendingMessageData;

static EmberStatus sendAddMessageCommand(EmberNodeId server,
                                         EmberNodeId destination,
                                         uint8_t *payload,
                                         uint8_t payloadLength,
                                         uint8_t tag,
                                         bool useSecurity);
static void addMessageCommandSentHandler(EmberStatus status,
                                         uint8_t tag,
                                         EmberNodeId server,
                                         EmberNodeId destination);
static EmberStatus sendGetMessageCommand(EmberNodeId server,
                                         bool useSecurity);
static void getMessageCommandSentHandler(EmberStatus status,
                                         EmberNodeId server);
static void incomingAddMessageResponseCommandHandler(uint8_t tag,
                                                     EmberNodeId server,
                                                     EmberNodeId destination,
                                                     EmberAfMailboxStatus status);
static void incomingMessageStatusCommandHandler(uint8_t tag,
                                                EmberNodeId server,
                                                EmberNodeId destination,
                                                EmberAfMailboxStatus status);
static void incomingDataMessageCommandHandler(uint8_t tag,
                                              EmberNodeId server,
                                              EmberNodeId messageSource,
                                              uint8_t *payload,
                                              uint8_t payloadLength,
                                              bool moreMessages);
static void incomingNullMessageCommandHandler(EmberNodeId server);

//------------------------------------------------------------------------------
// Public APIs

EmberAfMailboxStatus emberAfPluginMailboxClientMessageSubmit(EmberNodeId mailboxServer,
                                                             EmberNodeId messageDestination,
                                                             uint8_t *message,
                                                             EmberMessageLength messageLength,
                                                             uint8_t tag,
                                                             bool useSecurity)
{
  EmberStatus status;

  if (message == NULL || messageLength == 0) {
    return EMBER_MAILBOX_STATUS_INVALID_CALL;
  }

  if (mailboxServer == EMBER_NULL_NODE_ID
      || messageDestination == EMBER_NULL_NODE_ID) {
    return EMBER_MAILBOX_STATUS_INVALID_ADDRESS;
  }

  if (messageLength
      > ((useSecurity)
         ? EMBER_MAX_SECURED_APPLICATION_PAYLOAD_LENGTH
         : EMBER_MAX_UNSECURED_APPLICATION_PAYLOAD_LENGTH)
      - EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_HEADER_LENGTH) {
    return EMBER_MAILBOX_STATUS_MESSAGE_TOO_LONG;
  }

  if (internalState != CLIENT_STATE_INITIAL) {
    return EMBER_MAILBOX_STATUS_BUSY;
  }

  internalState = CLIENT_STATE_SUBMIT_PENDING;

  pendingMessageData.destination = messageDestination;
  pendingMessageData.server = mailboxServer;
  pendingMessageData.tag = tag;

  status = sendAddMessageCommand(mailboxServer,
                                 messageDestination,
                                 message,
                                 messageLength,
                                 tag,
                                 useSecurity);

  if (status != EMBER_SUCCESS) {
    internalState = CLIENT_STATE_INITIAL;
    return EMBER_MAILBOX_STATUS_STACK_ERROR;
  }

  return EMBER_MAILBOX_STATUS_SUCCESS;
}

EmberAfMailboxStatus emberAfPluginMailboxClientCheckInbox(EmberNodeId mailboxServer,
                                                          bool useSecurity)

{
  EmberStatus status;

  if (mailboxServer == EMBER_NULL_NODE_ID) {
    return EMBER_MAILBOX_STATUS_INVALID_ADDRESS;
  }

  if (internalState != CLIENT_STATE_INITIAL) {
    return EMBER_MAILBOX_STATUS_BUSY;
  }

  internalState = CLIENT_STATE_CHECK_MAILBOX_PENDING;

  pendingMessageData.server = mailboxServer;

  status = sendGetMessageCommand(mailboxServer, useSecurity);

  if (status != EMBER_SUCCESS) {
    internalState = CLIENT_STATE_INITIAL;
    return EMBER_MAILBOX_STATUS_STACK_ERROR;
  }

  return EMBER_MAILBOX_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
// Implemented plugin callbacks

void emAfPluginMailboxClientInitCallback(void)
{
  emberEventControlSetInactive(emAfPluginMailboxClientEventControl);
}

void emAfPluginMailboxClientEventHandler(void)
{
  emberEventControlSetInactive(emAfPluginMailboxClientEventControl);

  if (internalState == CLIENT_STATE_SUBMIT_PENDING) {
    internalState = CLIENT_STATE_INITIAL;
    emberAfPluginMailboxClientMessageSubmitCallback(EMBER_MAILBOX_STATUS_MESSAGE_NO_RESPONSE,
                                                    pendingMessageData.server,
                                                    pendingMessageData.destination,
                                                    pendingMessageData.tag);
  } else if (internalState == CLIENT_STATE_CHECK_MAILBOX_PENDING) {
    internalState = CLIENT_STATE_INITIAL;
    emberAfPluginMailboxClientCheckInboxCallback(EMBER_MAILBOX_STATUS_MESSAGE_NO_RESPONSE,
                                                 pendingMessageData.server,
                                                 EMBER_NULL_NODE_ID,
                                                 NULL,   // message
                                                 0,      // messageLength
                                                 0,      // tag
                                                 false); // moreMessages
  } else {
    assert(0);
  }
}

void emAfPluginMailboxClientIncomingMessageCallback(EmberIncomingMessage *message)
{
  if (message->endpoint == EMBER_AF_PLUGIN_MAILBOX_CLIENT_MAILBOX_ENDPOINT) {
    uint8_t commandId = mailboxProtocolCommandId(message->payload);

    switch (commandId) {
      case EMBER_MAILBOX_PROTOCOL_COMMAND_ID_ADD_MESSAGE_RESPONSE:
        if (message->length < EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_HEADER_LENGTH) {
          return;
        }

        incomingAddMessageResponseCommandHandler(message->payload[EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_TAG_OFFSET],
                                                 message->source,
                                                 emberFetchLowHighInt16u(message->payload + EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_DESTINATION_OFFSET),
                                                 message->payload[EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_RESPONSE_STATUS_OFFSET]);
        break;
      case EMBER_MAILBOX_PROTOCOL_COMMAND_ID_MESSAGE_STATUS:
        incomingMessageStatusCommandHandler(message->payload[EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_TAG_OFFSET],
                                            message->source,
                                            emberFetchLowHighInt16u(message->payload + EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_DESTINATION_OFFSET),
                                            message->payload[EMBER_MAILBOX_PROTOCOL_MESSAGE_STATUS_RESULT_OFFSET]);
        break;
      case EMBER_MAILBOX_PROTOCOL_COMMAND_ID_DATA_MESSAGE:
        // We enforce messages of at least 1 byte.
        if (message->length <= EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_HEADER_LENGTH) {
          return;
        }

        incomingDataMessageCommandHandler(message->payload[EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_TAG_OFFSET],
                                          message->source,
                                          emberFetchLowHighInt16u(message->payload + EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_SOURCE_OFFSET),
                                          message->payload + EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_PAYLOAD_OFFSET,
                                          message->length - EMBER_MAILBOX_PROTOCOL_DATA_MESSAGE_HEADER_LENGTH,
                                          mailboxProtocolMessagePendingFlag(message->payload));
        break;
      case EMBER_MAILBOX_PROTOCOL_COMMAND_ID_NULL_MESSAGE:
        if (message->length < EMBER_MAILBOX_PROTOCOL_NULL_MESSAGE_HEADER_LENGTH) {
          return;
        }

        incomingNullMessageCommandHandler(message->source);
        break;
    }
  }
}

void emAfPluginMailboxClientMessageSentCallback(EmberStatus status,
                                                EmberOutgoingMessage *message)
{
  if (message->endpoint == EMBER_AF_PLUGIN_MAILBOX_CLIENT_MAILBOX_ENDPOINT) {
    uint8_t commandId = mailboxProtocolCommandId(message->payload);

    switch (commandId) {
      case EMBER_MAILBOX_PROTOCOL_COMMAND_ID_ADD_MESSAGE:
        addMessageCommandSentHandler(status,
                                     message->payload[EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_TAG_OFFSET],
                                     message->destination,
                                     emberFetchLowHighInt16u(message->payload + EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_DESTINATION_OFFSET));
        break;
      case EMBER_MAILBOX_PROTOCOL_COMMAND_ID_GET_MESSAGE:
        getMessageCommandSentHandler(status, message->destination);
        break;
    }
  }
}

//------------------------------------------------------------------------------
// Static functions

static EmberStatus sendAddMessageCommand(EmberNodeId server,
                                         EmberNodeId destination,
                                         uint8_t *payload,
                                         uint8_t payloadLength,
                                         uint8_t tag,
                                         bool useSecurity)
{
  uint8_t message[EMBER_MAX_UNSECURED_APPLICATION_PAYLOAD_LENGTH];

  message[EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_OFFSET] =
    EMBER_MAILBOX_PROTOCOL_COMMAND_ID_ADD_MESSAGE;

  message[EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_TAG_OFFSET] = tag;

  emberStoreLowHighInt16u(message + EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_DESTINATION_OFFSET,
                          destination);

  MEMCOPY(message + EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_PAYLOAD_OFFSET,
          payload,
          payloadLength);

  return emberMessageSend(server,
                          EMBER_AF_PLUGIN_MAILBOX_CLIENT_MAILBOX_ENDPOINT,
                          0, // local tag
                          EMBER_MAILBOX_PROTOCOL_ADD_MESSAGE_HEADER_LENGTH + payloadLength,
                          message,
                          (EMBER_OPTIONS_ACK_REQUESTED
                           | (useSecurity
                              ? EMBER_OPTIONS_SECURITY_ENABLED
                              : 0)));
}

static void addMessageCommandSentHandler(EmberStatus status,
                                         uint8_t tag,
                                         EmberNodeId server,
                                         EmberNodeId destination)
{
  assert(internalState == CLIENT_STATE_SUBMIT_PENDING);

  if (tag == pendingMessageData.tag
      && destination == pendingMessageData.destination
      && server == pendingMessageData.server) {
    if (status == EMBER_SUCCESS) {
      emberEventControlSetDelayMS(emAfPluginMailboxClientEventControl,
                                  EMBER_MAILBOX_CLIENT_MESSAGE_TIMEOUT_MS);
    } else {
      internalState = CLIENT_STATE_INITIAL;
      emberAfPluginMailboxClientMessageSubmitCallback(EMBER_MAILBOX_STATUS_STACK_ERROR,
                                                      server,
                                                      destination,
                                                      tag);
    }
  }
}

static void incomingAddMessageResponseCommandHandler(uint8_t tag,
                                                     EmberNodeId server,
                                                     EmberNodeId destination,
                                                     EmberAfMailboxStatus status)
{
  if (internalState == CLIENT_STATE_SUBMIT_PENDING
      && tag == pendingMessageData.tag
      && destination == pendingMessageData.destination
      && server == pendingMessageData.server) {
    internalState = CLIENT_STATE_INITIAL;
    emberEventControlSetInactive(emAfPluginMailboxClientEventControl);
    emberAfPluginMailboxClientMessageSubmitCallback(status,
                                                    server,
                                                    destination,
                                                    tag);
  }
}

static void incomingMessageStatusCommandHandler(uint8_t tag,
                                                EmberNodeId server,
                                                EmberNodeId destination,
                                                EmberAfMailboxStatus status)
{
  emberAfPluginMailboxClientMessageDeliveredCallback(status,
                                                     server,
                                                     destination,
                                                     tag);
}

static EmberStatus sendGetMessageCommand(EmberNodeId server,
                                         bool useSecurity)
{
  uint8_t message[EMBER_MAILBOX_PROTOCOL_GET_MESSAGE_HEADER_LENGTH];

  message[EMBER_MAILBOX_PROTOCOL_FRAME_CONTROL_OFFSET] =
    EMBER_MAILBOX_PROTOCOL_COMMAND_ID_GET_MESSAGE;

  return emberMessageSend(server,
                          EMBER_AF_PLUGIN_MAILBOX_CLIENT_MAILBOX_ENDPOINT,
                          0, // local tag
                          EMBER_MAILBOX_PROTOCOL_GET_MESSAGE_HEADER_LENGTH,
                          message,
                          (EMBER_OPTIONS_ACK_REQUESTED
                           | (useSecurity
                              ? EMBER_OPTIONS_SECURITY_ENABLED
                              : 0)));
}

static void getMessageCommandSentHandler(EmberStatus status,
                                         EmberNodeId server)
{
  assert(internalState == CLIENT_STATE_CHECK_MAILBOX_PENDING);

  if (server == pendingMessageData.server) {
    if (status == EMBER_SUCCESS) {
      emberEventControlSetDelayMS(emAfPluginMailboxClientEventControl,
                                  EMBER_MAILBOX_CLIENT_MESSAGE_TIMEOUT_MS);
    } else {
      internalState = CLIENT_STATE_INITIAL;
      emberAfPluginMailboxClientCheckInboxCallback(EMBER_MAILBOX_STATUS_STACK_ERROR,
                                                   server,
                                                   EMBER_NULL_NODE_ID,
                                                   NULL,   // message
                                                   0,      // messageLength
                                                   0,      // tag
                                                   false); // moreMessages
    }
  }
}

static void incomingDataMessageCommandHandler(uint8_t tag,
                                              EmberNodeId server,
                                              EmberNodeId messageSource,
                                              uint8_t *payload,
                                              uint8_t payloadLength,
                                              bool moreMessages)
{
  if (internalState == CLIENT_STATE_CHECK_MAILBOX_PENDING
      && server == pendingMessageData.server) {
    internalState = CLIENT_STATE_INITIAL;
    emberEventControlSetInactive(emAfPluginMailboxClientEventControl);
    emberAfPluginMailboxClientCheckInboxCallback(EMBER_MAILBOX_STATUS_SUCCESS,
                                                 server,
                                                 messageSource,
                                                 payload,
                                                 payloadLength,
                                                 tag,
                                                 moreMessages);
  }
}

static void incomingNullMessageCommandHandler(EmberNodeId server)
{
  if (internalState == CLIENT_STATE_CHECK_MAILBOX_PENDING
      && server == pendingMessageData.server) {
    internalState = CLIENT_STATE_INITIAL;
    emberEventControlSetInactive(emAfPluginMailboxClientEventControl);
    emberAfPluginMailboxClientCheckInboxCallback(EMBER_MAILBOX_STATUS_MESSAGE_NO_DATA,
                                                 server,
                                                 EMBER_NULL_NODE_ID,
                                                 NULL,   // message
                                                 0,      // messageLength
                                                 0,      // tag
                                                 false); // moreMessages
  }
}
