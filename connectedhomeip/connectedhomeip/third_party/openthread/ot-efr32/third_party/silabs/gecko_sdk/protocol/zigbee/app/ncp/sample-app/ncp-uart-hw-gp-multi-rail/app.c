/***************************************************************************//**
 * @file app.c
 * @brief Callbacks implementation and application specific code.
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

#include "sl_component_catalog.h"
#include "hal.h"
#include "stack/include/ember.h"
#include "stack/include/gp-types.h"
#include "app/util/ezsp/ezsp-enum.h"
#include "app/xncp/xncp-sample-custom-ezsp-protocol.h"
#include "zigbee_app_framework_common.h"

#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)
#include "multirail-demo.h"

static sl_zigbee_event_t gp_transmit_complete_event;

// The time between receiving a GP packet and sending a scheduled response
#ifndef GP_RX_OFFSET_USEC
#define GP_RX_OFFSET_USEC 20500
#endif

//--------------------------------
// Extern and forward declarations

extern uint8_t sl_mac_lower_mac_get_radio_channel(uint8_t mac_index);
extern EmberMessageBuffer emGpdfMakeHeader(bool useCca,
                                           EmberGpAddress *src,
                                           EmberGpAddress *dst);
extern void emberDGpSentHandler(EmberStatus status,
                                uint8_t gpepHandle);
extern EmberStatus emberAfPluginXncpSendCustomEzspMessage(uint8_t length, uint8_t *payload);

static void appGpScheduleOutgoingGpdf(EmberZigbeePacketType packetType,
                                      int8u* packetData,
                                      int8u size_p);

//---------------
// Event handlers

static void gp_transmit_complete_event_handler(sl_zigbee_event_t *event)
{
  emberDGpSentHandler(EMBER_SUCCESS, 0);
}

//----------------------
// Implemented Callbacks

/** @brief Init
 * Application init function
 */
void emberAfMainInitCallback(void)
{
  sl_zigbee_event_init(&gp_transmit_complete_event, gp_transmit_complete_event_handler);
}

/** @brief Incoming Custom EZSP Message Callback
 *
 * This function is called when the NCP receives a custom EZSP message from the
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
  // In the sample custom EZSP protocol, the command ID is always the first byte
  // of a custom EZSP command.
  uint8_t index = 0;
  uint8_t commandId = messagePayload[index++];

  switch (commandId) {
    case EMBER_CUSTOM_EZSP_COMMAND_INIT_APP_GP_TX_QUEUE: {
      emberGpClearTxQueue();
      RAIL_Handle_t h =  emberAfPluginMultirailDemoInit(NULL,
                                                        NULL,
                                                        true,
                                                        RAIL_GetTxPowerDbm(emberGetRailHandle()),
                                                        NULL,
                                                        0,
                                                        0xFFFF,
                                                        NULL);
      return ((h == NULL) ? EMBER_ERR_FATAL : EMBER_SUCCESS);
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE: {
      EmberGpAddress addr = { 0 };
      addr.applicationId = messagePayload[index++];
      if (addr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
        addr.id.sourceId = (uint32_t)messagePayload[index++];
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 8);
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 16);
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 24);
      } else {
        // Initial support only for GPD Src ID
        return EMBER_INVALID_CALL;
      }
      uint8_t gpdCommandId = messagePayload[index++];

      if (messageLength >= index) {
        if (emberDGpSend(true,//bool action,
                         false,//bool useCca,
                         &addr,
                         gpdCommandId,
                         (messageLength - index),
                         &messagePayload[index],
                         0,
                         0) == EMBER_SUCCESS) {
          return EMBER_SUCCESS;
        }
      }
      return EMBER_INVALID_CALL;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_REMOVE_APP_GP_TX_QUEUE: {
      EmberGpAddress addr = { 0 };
      addr.applicationId = messagePayload[index++];
      if (addr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
        addr.id.sourceId = (uint32_t)messagePayload[index++];
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 8);
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 16);
        addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 24);
      } else {
        // Initial support only for GPD Src ID
        return EMBER_INVALID_CALL;
      }

      if (messageLength >= index) {
        if (emberDGpSend(false,//bool action,
                         false,//bool useCca,
                         &addr,
                         0,
                         0,
                         NULL,
                         0,
                         0) == EMBER_SUCCESS) {
          return EMBER_SUCCESS;
        }
      }
      return EMBER_INVALID_CALL;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE: {
      EmberGpTxQueueEntry txQueue;
      txQueue.addr.applicationId = messagePayload[index++];
      if (txQueue.addr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
        txQueue.addr.id.sourceId = (uint32_t)messagePayload[index++];
        txQueue.addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 8);
        txQueue.addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 16);
        txQueue.addr.id.sourceId += (((uint32_t)messagePayload[index++]) << 24);
      } else {
        // Initial support only for GPD Src ID
        return EMBER_INVALID_CALL;
      }
      uint8_t buffer[128];
      uint16_t outPktLength = 0;

      if (emberGpGetTxQueueEntryFromQueue(&txQueue,
                                          buffer,
                                          &outPktLength,
                                          128) != EMBER_NULL_MESSAGE_BUFFER) {
        replyPayload[0] = txQueue.addr.applicationId;
        replyPayload[1] = txQueue.addr.id.sourceId >> 0;
        replyPayload[2] = txQueue.addr.id.sourceId >> 8;
        replyPayload[3] = txQueue.addr.id.sourceId >> 16;
        replyPayload[4] = txQueue.addr.id.sourceId >> 24;
        MEMCOPY(&replyPayload[5], buffer, outPktLength);
        *replyPayloadLength = (outPktLength + 5);
        return EMBER_SUCCESS;
      }
      return EMBER_INVALID_CALL;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_SET_APP_GP_TX_QUEUE_MAX_SIZE: {
      uint16_t maxTxGpQueueSize = (uint32_t)messagePayload[index++];
      maxTxGpQueueSize += (((uint32_t)messagePayload[index]) << 8);
      emberGpSetMaxTxQueueEntry(maxTxGpQueueSize);
      return EMBER_SUCCESS;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_MAX_SIZE: {
      uint16_t maxTxGpQueueSize = emberGetGpMaxTxQListCount();
      replyPayload[0] = maxTxGpQueueSize >> 0;
      replyPayload[1] = maxTxGpQueueSize >> 8;
      *replyPayloadLength = sizeof(maxTxGpQueueSize);
      return EMBER_SUCCESS;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_GET_APP_GP_TX_QUEUE_COUNT: {
      uint16_t txGpQueueCount = emberGetGpTxQListCount();
      replyPayload[0] = txGpQueueCount >> 0;
      replyPayload[1] = txGpQueueCount >> 8;
      *replyPayloadLength = sizeof(txGpQueueCount);
      return EMBER_SUCCESS;
    }
    break;
    case EMBER_CUSTOM_EZSP_COMMAND_SEND_APP_GP_RAW: {
      uint8_t channel = messagePayload[index++];
      uint8_t delayInMsec = messagePayload[index++];
      uint8_t* buff = &messagePayload[index];
      uint8_t size = messageLength - index;
      RAIL_ScheduleTxConfig_t scheduledTxConfig = {
        .mode = RAIL_TIME_DELAY,
        .when = (delayInMsec * 1000)
      };
      RAIL_SchedulerInfo_t schedulerInfo = {
        .priority = 50,
        .slipTime = 2000,
        .transactionTime = 5000
      };
      RAIL_Status_t s = emberAfPluginMultirailDemoSend(buff,
                                                       size,
                                                       ((channel == 0) ? sl_mac_lower_mac_get_radio_channel(0) : channel),
                                                       ((delayInMsec == 0) ? NULL : &scheduledTxConfig),
                                                       &schedulerInfo);
      return ((s == RAIL_STATUS_NO_ERROR) ? EMBER_SUCCESS : EMBER_INVALID_CALL);
    }
    break;
    default:
      return EMBER_INVALID_CALL;
      break;
  }
  return EMBER_INVALID_CALL;
}

EmberPacketAction emAfPacketHandoffIncomingCallback(EmberZigbeePacketType packetType,
                                                    EmberMessageBuffer packetBuffer,
                                                    uint8_t index,
                                                    void *data)
{
  uint8_t size_p = emberMessageBufferLength(packetBuffer) - index;
  uint8_t packetData[128];
  // Flat packet : [<-----MAC Frame----->|<--8 bytes Appended Info-->]
  emberCopyFromLinkedBuffers(packetBuffer,
                             index,
                             packetData,
                             size_p);
  appGpScheduleOutgoingGpdf(packetType,
                            packetData,
                            size_p);
  return EMBER_ACCEPT_PACKET;
}

/** @brief A callback called whenever a secondary instance RAIL event occurs.
 *
 * @param[in] handle A handle for a RAIL instance.
 * @param[in] events A bit mask of RAIL events (full list in rail_types.h)
 */
void emberAfPluginMultirailDemoRailEventCallback(RAIL_Handle_t handle,
                                                 RAIL_Events_t events)
{
  if (events & RAIL_EVENT_TX_PACKET_SENT) {
    sl_zigbee_event_set_delay_ms(&gp_transmit_complete_event, 0);
  }
  (void)handle; // unreferenced parameter
}

/** @brief
 *
 * Application framework equivalent of ::emberRadioNeedsCalibratingHandler
 */
void emberAfRadioNeedsCalibratingCallback(void)
{
  sl_mac_calibrate_current_channel();
}

//-----------------
// Static functions

// Gets an entry from the Gp Stub queue if available and RAIL 2 is ready
static EmberGpTxQueueEntry* get_gp_stub_tx_queue(EmberGpAddress* addr)
{
  // Steal the GP Queue to send it through the additional RAIL handle
  // Check if RAIL 2 handle available and there is anything in GP Stub queue
  EmberGpTxQueueEntry emGpTxQueue;
  MEMCOPY(&emGpTxQueue.addr, addr, sizeof(EmberGpAddress));
  uint8_t data[128];
  uint16_t dataLength;
  if (emberAfPluginMultirailDemoGetHandle()
      && emberGpGetTxQueueEntryFromQueue(&emGpTxQueue,
                                         data,
                                         &dataLength,
                                         128) != EMBER_NULL_MESSAGE_BUFFER) {
    // Allocate a buffer and prepare a outgoing MAC header using gpd address in the emGpTxQueue
    EmberMessageBuffer header = emGpdfMakeHeader(true, NULL, &(emGpTxQueue.addr));
    // Add the command Id from the queue to the buffer
    uint8_t len = emberMessageBufferLength(header) + 1;
    emberAppendToLinkedBuffers(header, &(emGpTxQueue.gpdCommandId), 1);
    // Copy Command Payload from the queue to the buffer and update the length
    emberSetLinkedBuffersLength(header,
                                emberMessageBufferLength(header)
                                + dataLength);
    // Add the payload
    emberCopyToLinkedBuffers(data,
                             header,
                             len,
                             dataLength);
    // Clear the Stub queue because everything is serialised in header
    emberGpRemoveFromTxQueue(&emGpTxQueue);

    // Prepare a RAIL frame to be transported using the additional handle
    uint8_t outPktLength = emberMessageBufferLength(header);
    uint8_t outPkt[128]; //128 = MAX size
    // RAIL Frame : [Total Length (excludes itself) | <-----MAC FRAME ---->| 2 byte CRC]
    outPkt[0] = outPktLength + 2;
    // Copy the data from the buffer
    emberCopyFromLinkedBuffers(header,
                               0,
                               &outPkt[1],
                               outPktLength);
    // Free the header as the rail frame will be submitted with a new buffer asdu
    emberReleaseMessageBuffer(header);

    // This entry is exempt from marking (see emberAfMarkBuffersCallback below),
    // since it is allocated and released within the context of the same function
    // call (appGpScheduleOutgoingGpdf).
    static EmberGpTxQueueEntry copyOfGpStubTxQueue;
    copyOfGpStubTxQueue.inUse = true;
    copyOfGpStubTxQueue.asdu = emberFillLinkedBuffers(outPkt, (outPkt[0] + 1));
    MEMCOPY(&(copyOfGpStubTxQueue.addr), addr, sizeof(EmberGpAddress));
    return &copyOfGpStubTxQueue;
  }
  return NULL;
}

#define macToAppDelay(macTimeStamp) ((RAIL_GetTime() & 0x00FFFFFF) - (macTimeStamp))

static void appGpScheduleOutgoingGpdf(EmberZigbeePacketType packetType,
                                      int8u* packetData,
                                      int8u size_p)
{
  //MAC Frame  : [<---------------MAC Header------------->||<------------------------------------NWK Frame----------------------------------->]
  //              FC(2) | Seq(1) | DstPan(2) | DstAddr(2) || FC(1) | ExtFC(0/1) | SrcId(0/4) | SecFc(0/4) | MIC(0/4) | <------GPDF(1/n)------>
  //              FC    : ExtFC Present(b7)=1| AC(b6)=0| Protocol Ver(b5-b2)=3 GP frames| Frame Type(b1-b0) = 0
  //              ExtFC :  rxAfteTX (b6) = 1 |  AppId(b2-b0) = 0
  if (packetType == EMBER_ZIGBEE_PACKET_TYPE_RAW_MAC
      && size_p > 9 // minimum
      && ((packetData[7] & 0x3C) == 0x0C)            // FC : GP Frames has Protocol Version = 3 ?
      && (((packetData[7] & 0xC3) == 0x01)           // FC : ExtFC(b7) = 0 | AC(b6) = 0 | Frame type(b1-b0) = 1 = Maint, Frame
          || (((packetData[7] & 0xC3) == 0x80)     // FC : ExtFC(b7) = 1 | AC(b6) = 0 | Frame type(b1-b0) = 0 = Data frame
              && ((packetData[8] & 0xC0) == 0x40)))) { // ExtFC :  Direction(b7) = 0 | rxAfteTX (b6) = 1
    // The last 3 bytes of packetData contain the MAC time stamp
    const uint32_t macTimeStamp = ((uint32_t)packetData[size_p - 3] << 16)
                                  + ((uint32_t)packetData[size_p - 2] << 8)
                                  + ((uint32_t)packetData[size_p - 1]);

    // Do we have enough time to schedule the response?
    if (macToAppDelay(macTimeStamp) < GP_RX_OFFSET_USEC) {
      EmberGpAddress gpdAddr;
      gpdAddr.applicationId = EMBER_GP_APPLICATION_SOURCE_ID;
      gpdAddr.id.sourceId = 0;
      if (((packetData[7] & 0xC3) == 0x80)   // FC : ExtFC(b7) = 1 | AC(b6) = 0 | Frame type(b1-b0) = 0 = Data frame
          && ((packetData[8] & 0x07) == EMBER_GP_APPLICATION_SOURCE_ID)) {// ExtFC :  App Id (b2-b0) = 0
        (void) memcpy(&gpdAddr.id.sourceId, &packetData[9], sizeof(EmberGpSourceId));
      }
      // Is there a queued response for this source ID?
      EmberGpTxQueueEntry* entry = get_gp_stub_tx_queue(&gpdAddr);
      if (entry) {
        uint8_t outPktLength = emberMessageBufferLength(entry->asdu);
        uint8_t outPkt[128];
        emberCopyFromLinkedBuffers(entry->asdu,
                                   0,
                                   outPkt,
                                   outPktLength);

        // Schedule sending the response.
        RAIL_SchedulerInfo_t schedulerInfo = {
          .priority = 50,
          .slipTime = 2000,
          .transactionTime = 5000
        };
        RAIL_ScheduleTxConfig_t scheduledTxConfig = {
          .mode = RAIL_TIME_DELAY,
          // We could reuse macToAppDelay here, but recalculating the delay
          // will give us the most up-to-date timings:
          .when = GP_RX_OFFSET_USEC - macToAppDelay(macTimeStamp)
        };

        RAIL_Status_t UNUSED status = emberAfPluginMultirailDemoSend(outPkt,
                                                                     outPktLength,
                                                                     sl_mac_lower_mac_get_radio_channel(0),
                                                                     &scheduledTxConfig,
                                                                     &schedulerInfo);
        emberGpRemoveFromTxQueue(entry);
      }
    }
  }
}
#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT
