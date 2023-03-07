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

#include "app/framework/include/af.h"
#include "sl_component_catalog.h"
#include "network-creator.h"
#include "network-creator-security.h"
#include "network-steering.h"
#include "find-and-bind-target.h"
#include "zll-commissioning.h"

#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)
#include "multirail-demo.h"
#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT

#if defined(SL_CATALOG_LED0_PRESENT)
#include "sl_led.h"
#include "sl_simple_led_instances.h"
#define led_turn_on(led) sl_led_turn_on(led)
#define led_turn_off(led) sl_led_turn_off(led)
#define led_toggle(led) sl_led_toggle(led)
#define COMMISSIONING_STATUS_LED (&sl_led_led0)
#define ON_OFF_LIGHT_LED         (&sl_led_led0)
#else // !SL_CATALOG_LED0_PRESENT
#define led_turn_on(led)
#define led_turn_off(led)
#define led_toggle(led)
#endif // SL_CATALOG_LED0_PRESENT

#define LED_BLINK_PERIOD_MS      2000

static sl_zigbee_event_t commissioning_led_event;
static sl_zigbee_event_t finding_and_binding_event;
static sl_zigbee_event_t sink_commissioning_mode_event;
#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)
static sl_zigbee_event_t gp_transmit_complete_event;
#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT
static bool enterComm;

#define LIGHT_ENDPOINT 1

//---------------
// Event handlers

void commissioning_led_event_handler(sl_zigbee_event_t *event)
{
  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    uint16_t identifyTime;
    emberAfReadServerAttribute(LIGHT_ENDPOINT,
                               ZCL_IDENTIFY_CLUSTER_ID,
                               ZCL_IDENTIFY_TIME_ATTRIBUTE_ID,
                               (uint8_t *)&identifyTime,
                               sizeof(identifyTime));
    if (identifyTime > 0) {
      led_toggle(COMMISSIONING_STATUS_LED);
      sl_zigbee_event_set_delay_ms(&commissioning_led_event,
                                   LED_BLINK_PERIOD_MS << 1);
    } else {
      led_turn_on(COMMISSIONING_STATUS_LED);
    }
  } else {
    EmberStatus status = emberAfPluginNetworkSteeringStart();
    sl_zigbee_app_debug_print("%s network %s: 0x%02X\n", "Join", "start", status);
  }
}

void finding_and_binding_event_handler(sl_zigbee_event_t *event)
{
  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    sl_zigbee_app_debug_print("Find and bind target start: 0x%02X\n",
                              emberAfPluginFindAndBindTargetStart(LIGHT_ENDPOINT));
  }
}

extern bool emAfPluginGreenPowerServerGpSinkCommissioningModeCommandHandler(uint8_t options,
                                                                            uint16_t gpmAddrForSecurity,
                                                                            uint16_t gpmAddrForPairing,
                                                                            uint8_t sinkEndpoint);

// Enter or exit sink commissioning mode
void sink_commissioning_mode_event_handler(sl_zigbee_event_t *event)
{
  uint8_t options = EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_PROXIES \
                    | ((enterComm) ? EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_ACTION : 0);

  emAfPluginGreenPowerServerGpSinkCommissioningModeCommandHandler(options,    //options - (Involve Proxy | Enter)
                                                                  0xFFFF, //addr
                                                                  0xFFFF, //addr
                                                                  LIGHT_ENDPOINT); //light Endpoint
  if (enterComm) {
    led_turn_on(COMMISSIONING_STATUS_LED);
    enterComm = false;
  } else {
    led_turn_off(COMMISSIONING_STATUS_LED);
    enterComm = true;
  }
}

#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)

extern void emberDGpSentHandler(EmberStatus status, uint8_t gpepHandle);

void gp_transmit_complete_event_handler(sl_zigbee_event_t *event)
{
  emberDGpSentHandler(EMBER_SUCCESS, 0);
}

#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT

//----------------------
// Implemented Callbacks

/** @brief Init
 * Application init function
 */
void emberAfMainInitCallback(void)
{
  sl_zigbee_event_init(&commissioning_led_event, commissioning_led_event_handler);
  sl_zigbee_event_init(&finding_and_binding_event, finding_and_binding_event_handler);
  sl_zigbee_event_init(&sink_commissioning_mode_event, sink_commissioning_mode_event_handler);
#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)
  sl_zigbee_event_init(&gp_transmit_complete_event, gp_transmit_complete_event_handler);
#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT
  sl_zigbee_event_set_active(&commissioning_led_event);
}

/** @brief Stack Status
 *
 * This function is called by the application framework from the stack status
 * handler.  This callbacks provides applications an opportunity to be notified
 * of changes to the stack status and take appropriate action.  The return code
 * from this callback is ignored by the framework.  The framework will always
 * process the stack status after the callback returns.
 */
void emberAfStackStatusCallback(EmberStatus status)
{
  // Make sure to change the ZLL factory new state based on whether or not
  // we are on a network.
  if (status == EMBER_NETWORK_DOWN) {
    led_turn_off(COMMISSIONING_STATUS_LED);
  } else if (status == EMBER_NETWORK_UP) {
    led_turn_on(COMMISSIONING_STATUS_LED);
    sl_zigbee_event_set_active(&finding_and_binding_event);
  }
}

/** @brief Complete network steering.
 *
 * This callback is fired when the Network Steering plugin is complete.
 *
 * @param status On success this will be set to EMBER_SUCCESS to indicate a
 * network was joined successfully. On failure this will be the status code of
 * the last join or scan attempt. Ver.: always
 *
 * @param totalBeacons The total number of 802.15.4 beacons that were heard,
 * including beacons from different devices with the same PAN ID. Ver.: always
 * @param joinAttempts The number of join attempts that were made to get onto
 * an open Zigbee network. Ver.: always
 *
 * @param finalState The finishing state of the network steering process. From
 * this, one is able to tell on which channel mask and with which key the
 * process was complete. Ver.: always
 */
void emberAfPluginNetworkSteeringCompleteCallback(EmberStatus status,
                                                  uint8_t totalBeacons,
                                                  uint8_t joinAttempts,
                                                  uint8_t finalState)
{
  sl_zigbee_app_debug_print("%s network %s: 0x%02X\n", "Join", "complete", status);

  if (status != EMBER_SUCCESS) {
    status = emberAfPluginNetworkCreatorStart(false); // distributed
    sl_zigbee_app_debug_print("%s network %s: 0x%02X\n", "Form", "start", status);
  }
}

/** @brief Complete the network creation process.
 *
 * This callback notifies the user that the network creation process has
 * completed successfully.
 *
 * @param network The network that the network creator plugin successfully
 * formed. Ver.: always
 *
 * @param usedSecondaryChannels Whether or not the network creator wants to
 * form a network on the secondary channels. Ver.: always
 */
void emberAfPluginNetworkCreatorCompleteCallback(const EmberNetworkParameters *network,
                                                 bool usedSecondaryChannels)
{
  sl_zigbee_app_debug_print("%s network %s: 0x%02X\n",
                            "Form distributed",
                            "complete",
                            EMBER_SUCCESS);
}

/** @brief Post Attribute Change
 *
 * This function is called by the application framework after it changes an
 * attribute value. The value passed into this callback is the value to which
 * the attribute was set by the framework.
 */
void emberAfPostAttributeChangeCallback(uint8_t endpoint,
                                        EmberAfClusterId clusterId,
                                        EmberAfAttributeId attributeId,
                                        uint8_t mask,
                                        uint16_t manufacturerCode,
                                        uint8_t type,
                                        uint8_t size,
                                        uint8_t* value)
{
  if (clusterId == ZCL_ON_OFF_CLUSTER_ID
      && attributeId == ZCL_ON_OFF_ATTRIBUTE_ID
      && mask == CLUSTER_MASK_SERVER) {
    bool onOff;
    if (emberAfReadServerAttribute(endpoint,
                                   ZCL_ON_OFF_CLUSTER_ID,
                                   ZCL_ON_OFF_ATTRIBUTE_ID,
                                   (uint8_t *)&onOff,
                                   sizeof(onOff))
        == EMBER_ZCL_STATUS_SUCCESS) {
      if (onOff) {
        led_turn_on(ON_OFF_LIGHT_LED);
      } else {
        led_turn_off(ON_OFF_LIGHT_LED);
      }
    }
  }
}

/** @brief On/off Cluster Server Post Init.
 *
 * Following resolution of the On/Off state at startup for this endpoint, perform any
 * additional initialization needed; e.g., synchronize hardware state.
 *
 * @param endpoint Endpoint that is being initialized  Ver.: always
 */
void emberAfPluginOnOffClusterServerPostInitCallback(uint8_t endpoint)
{
  // At startup, trigger a read of the attribute and possibly a toggle of the
  // LED to make sure they are always in sync.
  emberAfPostAttributeChangeCallback(endpoint,
                                     ZCL_ON_OFF_CLUSTER_ID,
                                     ZCL_ON_OFF_ATTRIBUTE_ID,
                                     CLUSTER_MASK_SERVER,
                                     0,
                                     0,
                                     0,
                                     NULL);
}

/** @brief Green power server commissioning timeout callback
 *
 * This function is called by the Green Power Server upon expiry of any of the
 * commissioning timer , those are server commissioning window expiry, generic
 * switch commissioning or multi-sensor commissioning timer expiry.
 *
 * @param commissioningTimeoutType one of the types
 *                     COMMISSIONING_TIMEOUT_TYPE_COMMISSIONING_WINDOW_TIMEOUT,
 *                     COMMISSIONING_TIMEOUT_TYPE_GENERIC_SWITCH
 *                     or COMMISSIONING_TIMEOUT_TYPE_MULTI_SENSOR Ver.: always
 * @param numberOfEndpoints Number of sink endpoints participated in the commissioning Ver.: always
 * @param endpoints list of sink endpoints Ver.: always
 */
void emberAfGreenPowerServerCommissioningTimeoutCallback(uint8_t commissioningTimeoutType,
                                                         uint8_t numberOfEndpoints,
                                                         uint8_t * endpoints)
{
  // Commissioning exit, clear the state and LED indication.
  enterComm = false;
  led_turn_off(COMMISSIONING_STATUS_LED);
}

/** @brief Green power server pairing complete callback
 *
 * This function is called by the Green Power Server upon the completion of the pairing
 * and to indicate the closure of the pairing session.
 *
 * @param numberOfEndpoints number of sink endpoints participated in the pairing Ver.: always
 * @param endpoints list of sink endpoints Ver.: always
 */
void emberAfGreenPowerServerPairingCompleteCallback(uint8_t numberOfEndpoints,
                                                    uint8_t * endpoints)
{
  sl_zigbee_app_debug_print("%s: No of Eps = %x EPs[",
                            __FUNCTION__,
                            numberOfEndpoints);
  sl_zigbee_app_debug_print_buffer(endpoints, numberOfEndpoints, true);
  sl_zigbee_app_debug_print("]\n");
  led_turn_off(COMMISSIONING_STATUS_LED);
}

/** @brief
 *
 * Application framework equivalent of ::emberRadioNeedsCalibratingHandler
 */
void emberAfRadioNeedsCalibratingCallback(void)
{
  sl_mac_calibrate_current_channel();
}

#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)
static void appGpScheduleOutgoingGpdf(EmberZigbeePacketType packetType,
                                      int8u* packetData,
                                      int8u size_p);

/** @brief Incoming Packet Filter
 *
 * NOTE: REQUIRES INCLUDING THE PACKET-HANDOFF PLUGIN. This is called by the
 * Packet Handoff plugin when the stack receives a packet from one of the
 * protocol layers specified in ::EmberZigbeePacketType. The packetType argument
 * is one of the values of the ::EmberZigbeePacketType enum. If the stack
 * receives an 802.15.4 MAC beacon, it will call this function with the
 * packetType argument set to ::EMBER_ZIGBEE_PACKET_TYPE_BEACON. The
 * implementation of this callback may alter the data contained in packetData,
 * modify options and flags in the auxillary data, or consume the packet itself,
 * either sending the message, or discarding it as it sees fit.
 *
 * @param packetType the type of packet and associated protocol layer  Ver.:
 * always
 * @param packetData flat buffer containing the packet data associated with the
 * packet type  Ver.: always
 * @param size_p a pointer containing the size value of the packet  Ver.: always
 * @param data auxillary data included with the packet  Ver.: always
 */
EmberPacketAction emberAfIncomingPacketFilterCallback(EmberZigbeePacketType packetType,
                                                      int8u* packetData,
                                                      int8u* size_p,
                                                      void* data)
{
  appGpScheduleOutgoingGpdf(packetType, packetData, *size_p);

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
    sl_zigbee_event_set_active(&gp_transmit_complete_event);
  }
}
#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT

#if defined(SL_CATALOG_SIMPLE_BUTTON_PRESENT) && (EMBER_AF_PLUGIN_IDLE_SLEEP_USE_BUTTONS == 0)
#include "sl_simple_button.h"
#include "sl_simple_button_instances.h"

#define BUTTON0 0
#define BUTTON1 1

/***************************************************************************//**
 * A callback called in interrupt context whenever a button changes its state.
 *
 * @remark Can be implemented by the application if required. This function
 * can contain the functionality to be executed in response to changes of state
 * in each of the buttons, or callbacks to appropriate functionality.
 *
 * @note The button state should not be updated in this function, it is updated
 * by specific button driver prior to arriving here
 *
   @param[out] handle             Pointer to button instance
 ******************************************************************************/
void sl_button_on_change(const sl_button_t *handle)
{
  if (handle && sl_button_get_state(handle) == SL_SIMPLE_BUTTON_PRESSED) {
    if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON0) == handle) {
      sl_zigbee_event_set_active(&sink_commissioning_mode_event);
    } else if (SL_SIMPLE_BUTTON_INSTANCE(BUTTON1) == handle) {
      sl_zigbee_event_set_active(&finding_and_binding_event);
    }
  }
}
#endif // SL_CATALOG_SIMPLE_BUTTON_PRESENT && EMBER_AF_PLUGIN_IDLE_SLEEP_USE_BUTTONS == 0

//-------------------------------------
// Custom CLI commands and related code

#if defined(SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT)

void gpAppGpTxQueueInit(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  emberGpClearTxQueue();
  RAIL_Handle_t h = emberAfPluginMultirailDemoInit(NULL,
                                                   NULL,
                                                   true,
                                                   RAIL_GetTxPowerDbm(emberGetRailHandle()),
                                                   NULL,
                                                   0,
                                                   0xFFFF,
                                                   NULL);
  sl_zigbee_app_debug_print("Additional RAIL handle %sinitialized\n", h ? "" : "not ");
}

void gpAppGpTxQueueSet(sl_cli_command_arg_t *arguments)
{
  EmberGpAddress addr = { 0 };

  addr.applicationId = sl_cli_get_argument_uint8(arguments, 0);
  if (addr.applicationId != EMBER_GP_APPLICATION_SOURCE_ID) {
    sl_zigbee_app_debug_print("Invalid application ID\n");
    return;
  }
  addr.id.sourceId = sl_cli_get_argument_uint32(arguments, 1);

  uint8_t gpdCommandId = sl_cli_get_argument_uint8(arguments, 2);

  uint8_t gpdAsduLength = 0;
  uint8_t gpdAsdu[255];
  gpdAsduLength = sl_zigbee_copy_hex_arg(arguments, 3, gpdAsdu, sizeof(gpdAsdu), false);

  if (emberDGpSend(true,//bool action,
                   false,//bool useCca,
                   &addr,
                   gpdCommandId,
                   gpdAsduLength,
                   gpdAsdu,
                   0,
                   0) != EMBER_SUCCESS) {
    sl_zigbee_app_debug_print("Failed to add entry\n");
  }
}

void gpAppGpTxQueueGet(sl_cli_command_arg_t *arguments)
{
  EmberGpTxQueueEntry txQueue;
  txQueue.addr.applicationId = sl_cli_get_argument_uint8(arguments, 0);
  if (txQueue.addr.applicationId != EMBER_GP_APPLICATION_SOURCE_ID) {
    sl_zigbee_app_debug_print("Invalid application ID\n");
    return;
  }
  txQueue.addr.id.sourceId = sl_cli_get_argument_uint32(arguments, 1);

  uint8_t buffer[128];
  uint16_t length = 0;

  if (emberGpGetTxQueueEntryFromQueue(&txQueue,
                                      buffer,
                                      &length,
                                      128) != EMBER_NULL_MESSAGE_BUFFER) {
    sl_zigbee_app_debug_print("\n");
    sl_zigbee_app_debug_print_buffer(buffer, length, true);
    sl_zigbee_app_debug_print("\n");
  } else {
    sl_zigbee_app_debug_print("No entry found\n");
  }
}

void gpAppGpTxQueueRemove(sl_cli_command_arg_t *arguments)
{
  EmberGpAddress addr = { 0 };

  addr.applicationId = sl_cli_get_argument_uint8(arguments, 0);
  if (addr.applicationId != EMBER_GP_APPLICATION_SOURCE_ID) {
    sl_zigbee_app_debug_print("Invalid application ID\n");
    return;
  }
  addr.id.sourceId = sl_cli_get_argument_uint32(arguments, 1);

  if (emberDGpSend(false,//bool action,
                   false,//bool useCca,
                   &addr,
                   0,
                   0,
                   EMBER_NULL_MESSAGE_BUFFER,
                   0,
                   0) != EMBER_SUCCESS) {
    sl_zigbee_app_debug_print("No entry found\n");
  }
}

void gpAppGpSetTxQueueSize(sl_cli_command_arg_t *arguments)
{
  uint8_t size = sl_cli_get_argument_uint8(arguments, 0);
  emberGpSetMaxTxQueueEntry((uint16_t)size);
}

void gpAppGpGetTxQueueSize(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_zigbee_app_debug_print("Max Tx Queue Size = %d\n", emberGetGpMaxTxQListCount());
}

void gpAppGpGetTxQueueCount(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  sl_zigbee_app_debug_print("Tx Queue Count = %d\n", emberGetGpTxQListCount());
}

void gpAppGpPrintTxQueue(sl_cli_command_arg_t *arguments)
{
  (void)arguments;

  uint16_t listCount = emberGetGpTxQListCount();
  sl_zigbee_app_debug_print("\nNumber of Gp Tx Queue entries : %d\n", listCount);
  int i = 0;
  if (listCount != 0) {
    Buffer finger = emBufferQueueHead(emberGpGetTxQueueHead());
    while (finger != NULL_BUFFER) {
      sl_zigbee_app_debug_print("Entry [%d] :\n", i++);
      EmberGpTxQueueEntry *entry = (EmberGpTxQueueEntry *)emberMessageBufferContents(finger);
      sl_zigbee_app_debug_print("  App Id =  %d\n", entry->addr.applicationId);
      sl_zigbee_app_debug_print("  Src Id = 0x%04X\n", entry->addr.id.sourceId);
      uint8_t *data = (uint8_t *) (emberMessageBufferContents(finger) + sizeof(EmberGpTxQueueEntry));
      uint16_t dataLength = emberMessageBufferLength(finger) - sizeof(EmberGpTxQueueEntry);
      sl_zigbee_app_debug_print("  Data Length = %d\n", dataLength);
      sl_zigbee_app_debug_print("  Data = [");
      sl_zigbee_app_debug_print_buffer(data, dataLength, true);
      sl_zigbee_app_debug_print("]\n");
      finger = emBufferQueueNext(emberGpGetTxQueueHead(), finger);
    }
  }
}

// The time between receiving a GP packet and sending a scheduled response
#ifndef GP_RX_OFFSET_USEC
  #define GP_RX_OFFSET_USEC 20500
#endif
#define macToAppDelay(macTimeStamp) ((RAIL_GetTime() & 0x00FFFFFF) - (macTimeStamp))

extern uint8_t sl_mac_lower_mac_get_radio_channel(uint8_t mac_index);

extern EmberMessageBuffer emGpdfMakeHeader(bool useCca,
                                           EmberGpAddress *src,
                                           EmberGpAddress *dst);

static void free_gp_tx_queue_entry(EmberGpTxQueueEntry* entry)
{
  emberGpRemoveFromTxQueue(entry);
}

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
        free_gp_tx_queue_entry(entry);
      }
    }
  }
}

#endif // SL_CATALOG_ZIGBEE_MULTIRAIL_DEMO_PRESENT
