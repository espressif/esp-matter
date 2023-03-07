/***************************************************************************//**
 * @file
 * @brief Splits long messages into smaller blocks for transmission and reassembles
 * received blocks.
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

/**
 * @defgroup fragmentation Fragmentation
 * @ingroup component
 * @brief API and Callbacks for the Fragmentation Component
 *
 * This component supports sending long packets through fragmentation and
 * reassembly.
 *
 */

/**
 * @addtogroup fragmentation
 * @{
 */

#ifdef UC_BUILD
#include "fragmentation-config.h"
#endif

#ifndef ZIGBEE_APSC_MAX_TRANSMIT_RETRIES
#define ZIGBEE_APSC_MAX_TRANSMIT_RETRIES 3
#endif //ZIGBEE_APSC_MAX_TRANSMIT_RETRIES

#ifndef EMBER_AF_PLUGIN_FRAGMENTATION_RX_WINDOW_SIZE
#define EMBER_AF_PLUGIN_FRAGMENTATION_RX_WINDOW_SIZE 1
#endif //EMBER_AF_PLUGIN_FRAGMENTATION_RX_WINDOW_SIZE

#ifndef UC_BUILD
// These are defined in fragmentation-config.h
#ifndef EMBER_AF_PLUGIN_FRAGMENTATION_MAX_INCOMING_PACKETS
#define EMBER_AF_PLUGIN_FRAGMENTATION_MAX_INCOMING_PACKETS 2
#endif //EMBER_AF_PLUGIN_FRAGMENTATION_MAX_INCOMING_PACKETS

#ifndef EMBER_AF_PLUGIN_FRAGMENTATION_MAX_OUTGOING_PACKETS
#define EMBER_AF_PLUGIN_FRAGMENTATION_MAX_OUTGOING_PACKETS 2
#endif //EMBER_AF_PLUGIN_FRAGMENTATION_MAX_OUTGOING_PACKETS

#ifndef EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE
#define EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE 1500
#endif //EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE

// These are not needed in a UC build

// TODO: We should have the App Builder generating these events. For now, I
// manually added 10 events which means we will be able to set and accept up to
// 10 incoming distinct fragmented packets. In AppBuilder the max incoming
// packets number is capped to 10, therefore we will never run out of events.
#define EMBER_AF_FRAGMENTATION_EVENTS                                                 \
  { &(emAfFragmentationEvents[0]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[1]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[2]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[3]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[4]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[5]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[6]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[7]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[8]), (void (*)(void))emAfFragmentationAbortReception }, \
  { &(emAfFragmentationEvents[9]), (void (*)(void))emAfFragmentationAbortReception },

#define EMBER_AF_FRAGMENTATION_EVENT_STRINGS \
  "Frag 0",                                  \
  "Frag 1",                                  \
  "Frag 2",                                  \
  "Frag 3",                                  \
  "Frag 4",                                  \
  "Frag 5",                                  \
  "Frag 6",                                  \
  "Frag 7",                                  \
  "Frag 8",                                  \
  "Frag 9",

#ifndef DOXYGEN_SHOULD_SKIP_THIS
extern EmberEventControl emAfFragmentationEvents[10];
#endif //DOXYGEN_SHOULD_SKIP_THIS

#endif // UC_BUILD

//------------------------------------------------------------------------------
// Sending
#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef struct {
  EmberOutgoingMessageType  messageType;
  uint16_t                    indexOrDestination;
  uint8_t                     sequence;
  EmberApsFrame             apsFrame;
  uint8_t*                    bufferPtr;
  uint16_t                    bufLen;
  uint8_t                     fragmentLen;
  uint8_t                     fragmentCount;
  uint8_t                     fragmentBase;
  uint8_t                     fragmentsInTransit;
}txFragmentedPacket;

EmberStatus emAfFragmentationSendUnicast(EmberOutgoingMessageType type,
                                         uint16_t indexOrDestination,
                                         EmberApsFrame *apsFrame,
                                         uint8_t *buffer,
                                         uint16_t bufLen,
                                         uint16_t *messageTag);

bool emAfFragmentationMessageSent(EmberApsFrame *apsFrame,
                                  EmberStatus status);

void emAfFragmentationMessageSentHandler(EmberOutgoingMessageType type,
                                         uint16_t indexOrDestination,
                                         EmberApsFrame *apsFrame,
                                         uint8_t *buffer,
                                         uint16_t bufLen,
                                         EmberStatus status,
                                         uint16_t messageTag);

#endif //DOXYGEN_SHOULD_SKIP_THIS

//------------------------------------------------------------------------------
// Receiving.
#ifndef DOXYGEN_SHOULD_SKIP_THIS

typedef enum {
  EMBER_AF_PLUGIN_FRAGMENTATION_RX_PACKET_AVAILABLE         = 0,
  EMBER_AF_PLUGIN_FRAGMENTATION_RX_PACKET_ACKED             = 1,
  EMBER_AF_PLUGIN_FRAGMENTATION_RX_PACKET_IN_USE            = 2,
  EMBER_AF_PLUGIN_FRAGMENTATION_RX_PACKET_PAYLOAD_TOO_LARGE = 3
}rxPacketStatus;

typedef struct {
  rxPacketStatus status;
  uint8_t       ackedPacketAge;
  uint8_t       buffer[EMBER_AF_PLUGIN_FRAGMENTATION_BUFFER_SIZE];
  EmberNodeId fragmentSource;
  uint8_t       fragmentSequenceNumber;
  uint8_t       fragmentBase; // first fragment inside the rx window.
  uint16_t      windowFinger; //points to the first byte inside the rx window.
  uint8_t       fragmentsExpected; // total number of fragments expected.
  uint8_t       fragmentsReceived; // fragments received so far.
  uint8_t       fragmentMask; // bitmask of received fragments inside the rx window.
  uint8_t       lastfragmentLen; // Length of the last fragment.
  uint8_t       fragmentLen; // Length of the fragment inside the rx window.
                             // All the fragments inside the rx window should have
                             // the same length.
#ifdef UC_BUILD
  sl_zigbee_event_t fragmentEventControl;
#else // !UC_BUILD
  EmberEventControl *fragmentEventControl;
#endif // UC_BUILD
}rxFragmentedPacket;

bool emAfFragmentationIncomingMessage(EmberIncomingMessageType type,
                                      EmberApsFrame *apsFrame,
                                      EmberNodeId sender,
                                      uint8_t **buffer,
                                      uint16_t *bufLen);

#ifdef UC_BUILD
void emAfFragmentationAbortReception(sl_zigbee_event_t* control);
#else // !UC_BUILD
void emAfFragmentationAbortReception(EmberEventControl* control);
#endif // UC_BUILD

extern uint8_t  emberFragmentWindowSize;

extern uint16_t emberMacIndirectTimeout;
#if defined(EZSP_HOST)
extern uint16_t emberApsAckTimeoutMs;
#endif

void emAfPluginFragmentationPlatformInitCallback(void);

EmberStatus emAfPluginFragmentationSend(txFragmentedPacket* txPacket,
                                        uint8_t fragmentNumber,
                                        uint16_t fragmentLen,
                                        uint16_t offset);

void emAfPluginFragmentationHandleSourceRoute(txFragmentedPacket* txPacket,
                                              uint16_t indexOrDestination);

void emAfPluginFragmentationSendReply(EmberNodeId sender,
                                      EmberApsFrame* apsFrame,
                                      rxFragmentedPacket* rxPacket);

#if defined(EMBER_TEST)
extern uint8_t emAfPluginFragmentationArtificiallyDropBlockNumber;
#endif

#endif //DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of fragmentation
