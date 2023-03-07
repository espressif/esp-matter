/***************************************************************************//**
 * @file
 * @brief Programmable NCP code.
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

#ifndef SILABS_XNCP_H
#define SILABS_XNCP_H

#include PLATFORM_HEADER
#include "stack/include/ember.h"
#include "app/util/ezsp/ezsp-protocol.h"

#include "hal/hal.h"

// A legacy EZSP frame is defined as follows:
//   <sequence:1>
//   <frame control:1>
//   <frame ID:1>
//   <parameters:0-120>

// An extended EZSP frame (EZSP Version 8) is defined as follows:
//   <sequence:1>
//   <frame control:2>
//   <frame ID:2>
//   <parameters:0-120>

// Since we use the first byte of the parameters for the length,
// the largest possible custom EZSP frame payload is 119 bytes.
#define EMBER_MAX_CUSTOM_EZSP_MESSAGE_PAYLOAD 119

//------------------------------------------------------------------------------
// Callbacks

EmberStatus emberAfPluginXncpIncomingCustomFrameCallback(uint8_t messageLength,
                                                         uint8_t *messagePayload,
                                                         uint8_t *replyPayloadLength,
                                                         uint8_t *replyPayload);

#ifdef UC_BUILD
bool emberAfPluginXncpIncomingMessageCallback(EmberIncomingMessageType type,
                                              EmberApsFrame *apsFrame,
                                              EmberMessageBuffer message);
#else  // !UC_BUILD
bool emberAfIncomingMessageCallback(EmberIncomingMessageType type,
                                    EmberApsFrame *apsFrame,
                                    EmberMessageBuffer message);
#endif  //UC_BUILD

#ifdef UC_BUILD
bool emberAfPluginXncpPermitNcpToHostFrameCallback(uint16_t frameId,
                                                   uint8_t payloadLength,
                                                   uint8_t *payload);
#else   // !UC_BUILD
bool emberAfPluginCommandHandlerPermitNcpToHostFrameCallback(uint16_t frameId,
                                                             uint8_t payloadLength,
                                                             uint8_t *payload);
#endif  // UC_BUILD

#ifdef UC_BUILD
bool emberAfPluginXncpPermitHostToNcpFrameCallback(uint16_t frameId,
                                                   uint8_t payloadLength,
                                                   uint8_t *payload);
#else  // !UC_BUILD
bool emberAfPluginCommandHandlerPermitHostToNcpFrameCallback(uint16_t frameId,
                                                             uint8_t payloadLength,
                                                             uint8_t *payload);
#endif // UC_BUILD

//------------------------------------------------------------------------------
// Public APIs

EmberStatus emberAfPluginXncpSendCustomEzspMessage(uint8_t length, uint8_t *payload);

//------------------------------------------------------------------------------
// Private APIs

EmberStatus emXncpInit(void);
bool emXNcpAllowIncomingEzspCommand(void);
bool emXNcpAllowOutgoingEzspCallback(uint8_t *callback,
                                     uint8_t callbackLength);
bool emAfPluginXncpIncomingMessageCallback(EmberIncomingMessageType type,
                                           EmberApsFrame *apsFrame,
                                           EmberMessageBuffer message);
void emXNcpHandleIncomingCustomEzspMessage(void);
void emXNcpHandleGetInfoCommand(void);

#endif // SILABS_XNCP_H
