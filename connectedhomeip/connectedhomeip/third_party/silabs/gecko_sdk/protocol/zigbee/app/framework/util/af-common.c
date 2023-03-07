/***************************************************************************//**
 * @file af-common.c
 * @brief Code common to both the Host and SOC (system on a chip) versions
 * of the Application Framework.
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

#include "af.h"
#include "zigbee_zcl_callback_dispatcher.h"
#include "af-main.h"
#include "common.h"
#include "service-discovery.h"
#include "app/framework/security/af-security.h"
#include "app/framework/security/crypto-state.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "library.h"
#ifdef EMBER_AF_HAS_SECURITY_PROFILE_SE
  #include "stack/include/cbke-crypto-engine.h"  // emberGetCertificate()
#endif

#include "sl_component_catalog.h"

#ifdef SL_CATALOG_ZIGBEE_SUB_GHZ_CLIENT_PRESENT
#include "sub-ghz-client.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_SUB_GHZ_SERVER_PRESENT
#include "sub-ghz-server.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
  #include "fragmentation.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_CRITICAL_MESSAGE_QUEUE_PRESENT
 #include "critical-message-queue.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_TEST_HARNESS_PRESENT
  #include "test-harness-config.h"
#endif

#ifdef EZSP_HOST
#define INVALID_MESSAGE_TAG 0xFFFF
#define setStackProfile(stackProfile) \
  emberAfSetEzspConfigValue(EZSP_CONFIG_STACK_PROFILE, stackProfile, "stack profile")
#define setSecurityLevel(securityLevel) \
  emberAfSetEzspConfigValue(EZSP_CONFIG_SECURITY_LEVEL, securityLevel, "security level")
#else
#define INVALID_MESSAGE_TAG 0x0000
#define setStackProfile(stackProfile) emberSetStackProfile(stackProfile)
#define setSecurityLevel(securityLevel) emberSetSecurityLevel(securityLevel)
#endif

#define INVALID_CALLBACK_INDEX 0xFF

// flags the user can turn on or off to make the printing behave differently
bool emberAfPrintReceivedMessages = true;

// TODO: we hardcode these values here instead of pulling in ota-storage.h
// which define INVALID_OTA_IMAGE_ID. That would force a dependency to the
// zigbee_ota_storage_common component for all apps.
const EmberAfOtaImageId emberAfInvalidImageId
  = { 0xFFFF, 0xFFFF, 0xFFFFFFFFUL, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

static CallbackTableEntry messageSentCallbacks[EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE];

// We declare this variable 'const' but NOT const.  Those functions that we may use
// this variable would also have to declare it const in order to function
// correctly, which is not the case (e.g. emberFindKeyTableEntry()).
const EmberEUI64 emberAfNullEui64 = { 0, 0, 0, 0, 0, 0, 0, 0 };

//------------------------------------------------------------------------------
// Forward declarations

static uint8_t getMessageSentCallbackIndex(void);
static void invalidateMessageSentCallbackEntry(uint16_t messageTag);
static EmberAfMessageSentFunction getMessageSentCallback(uint16_t tag);
static EmberStatus send(EmberOutgoingMessageType type,
                        uint16_t indexOrDestination,
                        EmberApsFrame *apsFrame,
                        uint16_t messageLength,
                        uint8_t *message,
                        bool broadcast,
                        EmberNodeId alias,
                        uint8_t sequence,
                        EmberAfMessageSentFunction callback);
static EmberStatus broadcastPermitJoin(uint8_t duration);
static void printMessage(EmberIncomingMessageType type,
                         EmberApsFrame* apsFrame,
                         uint16_t messageLength,
                         uint8_t* messageContents);

//------------------------------------------------------------------------------
// Internal callbacks

// This is called from emAfIncomingMessageCallback() in af-soc.c or af-host.c
void emAfIncomingMessageHandler(EmberIncomingMessageType type,
                                EmberApsFrame *apsFrame,
                                uint8_t lastHopLqi,
                                int8_t lastHopRssi,
                                uint16_t messageLength,
                                uint8_t *messageContents)
{
  EmberNodeId sender = emberGetSender();
  EmberAfIncomingMessage im;

#ifdef SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
  if (emAfFragmentationIncomingMessage(type,
                                       apsFrame,
                                       sender,
                                       &messageContents,
                                       &messageLength)) {
    emberAfDebugPrintln("%pfragment processed.", "Fragmentation:");
    return;
  }
#endif // SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT

#ifdef SL_CATALOG_ZIGBEE_SUB_GHZ_SERVER_PRESENT
  if (emAfSubGhzServerIncomingMessage(type,
                                      apsFrame,
                                      sender,
                                      messageLength,
                                      messageContents)) {
    emberAfDebugPrintln("Sub-GHz: incoming message rejected.");
    return;
  }
#endif // SL_CATALOG_ZIGBEE_SUB_GHZ_SERVER_PRESENT

  emberAfDebugPrintln("Processing message: len=%d profile=%2x cluster=%2x",
                      messageLength,
                      apsFrame->profileId,
                      apsFrame->clusterId);
  emberAfDebugFlush();

  // Populate the incoming message struct to pass to the incoming message
  // callback.
  im.type              = type;
  im.apsFrame          = apsFrame;
  im.message           = messageContents;
  im.msgLen            = messageLength;
  im.source            = sender;
  im.lastHopLqi        = lastHopLqi;
  im.lastHopRssi       = lastHopRssi;
  im.bindingTableIndex = emberAfGetBindingIndex();
  im.addressTableIndex = emberAfGetAddressIndex();
  im.networkIndex      = emberGetCurrentNetwork();
  if (emberAfPreMessageReceivedCallback(&im)) {
    return;
  }

  // Handle service discovery responses.
  if (emAfServiceDiscoveryIncoming(sender,
                                   apsFrame,
                                   messageContents,
                                   messageLength)) {
    return;
  }

  // Handle ZDO messages.
  if (emAfProcessZdo(sender, apsFrame, messageContents, messageLength)) {
    return;
  }

  // Handle ZCL messages.
  if (emberAfProcessMessage(apsFrame,
                            type,
                            messageContents,
                            messageLength,
                            sender,
                            NULL)) { // no inter-pan header
    return;
  }
}

// This is called from emAfMessageSentCallback() in af-soc.c or af-host.c
void emAfMessageSentHandler(EmberOutgoingMessageType type,
                            uint16_t indexOrDestination,
                            EmberApsFrame *apsFrame,
                            EmberStatus status,
                            uint16_t messageLength,
                            uint8_t *messageContents,
                            uint16_t messageTag)
{
  EmberAfMessageSentFunction callback;
  if (status != EMBER_SUCCESS) {
    emberAfAppPrint("%ptx %x, ", "ERROR: ", status);
    printMessage(type, apsFrame, messageLength, messageContents);
  }

  callback = getMessageSentCallback(messageTag);
  invalidateMessageSentCallbackEntry(messageTag);

  if (status == EMBER_SUCCESS
      && apsFrame->profileId == EMBER_ZDO_PROFILE_ID
      && apsFrame->clusterId < CLUSTER_ID_RESPONSE_MINIMUM) {
    emberAfAddToCurrentAppTasks(EMBER_AF_WAITING_FOR_ZDO_RESPONSE);
  }

  emberAfRemoveFromCurrentAppTasks(EMBER_AF_WAITING_FOR_DATA_ACK);

  if (messageContents != NULL && messageContents[0] & ZCL_CLUSTER_SPECIFIC_COMMAND) {
    emberAfClusterMessageSentWithMfgCodeCallback(type,
                                                 indexOrDestination,
                                                 apsFrame,
                                                 messageLength,
                                                 messageContents,
                                                 status,
                                                 // If the manufacturer specific flag is set
                                                 // get read it as next part of message
                                                 // else use null code.
                                                 (((messageContents[0]
                                                    & ZCL_MANUFACTURER_SPECIFIC_MASK)
                                                   == ZCL_MANUFACTURER_SPECIFIC_MASK)
                                                  ? emberAfGetInt16u(messageContents, 1, messageLength)
                                                  : EMBER_AF_NULL_MANUFACTURER_CODE));
  }

  if (callback != NULL) {
    (*callback)(type, indexOrDestination, apsFrame, messageLength, messageContents, status);
  }

  emberAfMessageSentCallback(type,
                             indexOrDestination,
                             apsFrame,
                             messageLength,
                             messageContents,
                             status);
}

#ifdef SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
void emAfFragmentationMessageSentHandler(EmberOutgoingMessageType type,
                                         uint16_t indexOrDestination,
                                         EmberApsFrame *apsFrame,
                                         uint8_t *buffer,
                                         uint16_t bufLen,
                                         EmberStatus status,
                                         uint16_t messageTag)
{
  // the fragmented message is no longer in process
  emberAfDebugPrintln("%pend.", "Fragmentation:");
  emAfMessageSentHandler(type,
                         indexOrDestination,
                         apsFrame,
                         status,
                         bufLen,
                         buffer,
                         messageTag);

  // EMZIGBEE-4437: setting back the buffers to the original in case someone set
  // that to something else.
  emberAfSetExternalBuffer(appResponseData,
                           EMBER_AF_RESPONSE_BUFFER_LEN,
                           &appResponseLength,
                           &emberAfResponseApsFrame);
}
#endif // SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT

void emAfStackStatusCallback(EmberStatus status)
{
  emberAfAppFlush();

  // To be extra careful, we clear the network cache whenever a new status is
  // received.
  emAfClearNetworkCache(emberGetCurrentNetwork());

  switch (status) {
    case EMBER_NETWORK_UP:
    case EMBER_TRUST_CENTER_EUI_HAS_CHANGED:  // also means NETWORK_UP
    {
      // ZigBee 3.0 security configuration is handled in plugins.
#ifndef EMBER_AF_HAS_SECURITY_PROFILE_Z3
      // Set the runtime security settings as soon as the stack goes up.
      EmberExtendedSecurityBitmask oldExtended;
      EmberExtendedSecurityBitmask newExtended = 0;
      const EmberAfSecurityProfileData *data = emAfGetCurrentSecurityProfileData();
      bool trustCenter = (emberAfGetNodeId() == EMBER_TRUST_CENTER_NODE_ID);
      if (data != NULL) {
        newExtended = (trustCenter
                       ? data->tcExtendedBitmask
                       : data->nodeExtendedBitmask);
      }

      emberGetExtendedSecurityBitmask(&oldExtended);
      if ((oldExtended & EMBER_EXT_NO_FRAME_COUNTER_RESET) != 0U) {
        newExtended |= EMBER_EXT_NO_FRAME_COUNTER_RESET;
      }

      emberAfSecurityInitCallback(NULL, &newExtended, trustCenter);
      emberSetExtendedSecurityBitmask(newExtended);
#endif // EMBER_AF_HAS_SECURITY_PROFILE_Z3

      emberAfAppPrintln("%p%pUP 0x%2X", "EMBER_", "NETWORK_", emberAfGetNodeId());
      emberAfAppFlush();
#if defined(EMBER_TEST)
      simulatedTimePasses();
#endif

      if (status == EMBER_TRUST_CENTER_EUI_HAS_CHANGED) {
        emberAfAppPrintln("Trust Center EUI has changed.");
        // We abort registration because we want to clear out any previous
        // state and force it to start anew.  One of two results will occur after
        // we restart registration later.
        // (1) It succeeds and we are on a new network with a new TC, in which
        //     case we need to kick off key establishment to re-authenticate it
        //     and also re-discover other ESIs.
        // (2) It will fail, in which case we have to reboot to forget the untrusted
        //     network and its settings.
        emberAfRegistrationAbortCallback();
        emberAfRegistrationStartCallback();
      } else {
        emberStartWritingStackTokens();
      }

      // This kicks off registration for newly joined devices.  If registration
      // already occurred, nothing will happen here.
      emberAfRegistrationStartCallback();
      break;
    }

    case EMBER_RECEIVED_KEY_IN_THE_CLEAR:
    case EMBER_NO_NETWORK_KEY_RECEIVED:
    case EMBER_NO_LINK_KEY_RECEIVED:
    case EMBER_PRECONFIGURED_KEY_REQUIRED:
    case EMBER_MOVE_FAILED:
    case EMBER_JOIN_FAILED:
    case EMBER_NO_BEACONS:
    case EMBER_CANNOT_JOIN_AS_ROUTER:
    case EMBER_NETWORK_DOWN:
      if (status == EMBER_NETWORK_DOWN) {
        emberAfAppPrintln("%p%pDOWN", "EMBER_", "NETWORK_");
      } else {
        emberAfAppPrintln("%pJOIN%p", "EMBER_", "_FAILED");
      }
      emberAfAppFlush();
      emberAfStackDown();
      break;

    case EMBER_NETWORK_OPENED:
      emberAfAppPrintln("EMBER_NETWORK_OPENED: %d sec", emberAfGetOpenNetworkDurationSec());
      return;

    case EMBER_NETWORK_CLOSED:
      emberAfAppPrintln("EMBER_NETWORK_CLOSED");
      return;

    default:
      emberAfDebugPrintln("EVENT: stackStatus 0x%x", status);
  }

  emberAfAppFlush();
}

#ifdef EMBER_TEST
void halButtonIsr(uint8_t button, uint8_t state)
{
  emberAfHalButtonIsrCallback(button, state);
}
#endif

//------------------------------------------------------------------------------
// Public APIs

EmberStatus emberAfSendMulticastWithAliasWithCallback(EmberMulticastId multicastId,
                                                      EmberApsFrame *apsFrame,
                                                      uint16_t messageLength,
                                                      uint8_t *message,
                                                      EmberNodeId alias,
                                                      uint8_t sequence,
                                                      EmberAfMessageSentFunction callback)
{
  apsFrame->groupId = multicastId;
  return send(EMBER_OUTGOING_MULTICAST_WITH_ALIAS,
              multicastId,
              apsFrame,
              messageLength,
              message,
              true, //broadcast
              alias,
              sequence,
              callback);
}

EmberStatus emberAfSendMulticastWithCallback(EmberMulticastId multicastId,
                                             EmberApsFrame *apsFrame,
                                             uint16_t messageLength,
                                             uint8_t *message,
                                             EmberAfMessageSentFunction callback)
{
  apsFrame->groupId = multicastId;
  return send(EMBER_OUTGOING_MULTICAST,
              multicastId,
              apsFrame,
              messageLength,
              message,
              true, // broadcast?
              0, //alias
              0, //sequence
              callback);
}

EmberStatus emberAfSendMulticast(EmberMulticastId multicastId,
                                 EmberApsFrame *apsFrame,
                                 uint16_t messageLength,
                                 uint8_t *message)
{
  return emberAfSendMulticastWithCallback(multicastId,
                                          apsFrame,
                                          messageLength,
                                          message,
                                          NULL);
}

EmberStatus emberAfSendMulticastToBindings(EmberApsFrame *apsFrame,
                                           uint16_t messageLength,
                                           uint8_t* message)
{
  EmberStatus status = EMBER_INVALID_BINDING_INDEX;
#if (EMBER_BINDING_TABLE_SIZE > 0)
  uint8_t i;
  EmberBindingTableEntry binding;
  uint16_t groupDest;

  if ((NULL == apsFrame) || (0 == messageLength) || (NULL == message)) {
    return EMBER_BAD_ARGUMENT;
  }

  for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
    status = emberGetBinding(i, &binding);
    if (status != EMBER_SUCCESS) {
      return status;
    }

    if (binding.type == EMBER_MULTICAST_BINDING
        && binding.local == apsFrame->sourceEndpoint
        && binding.clusterId == apsFrame->clusterId) {
      groupDest = (binding.identifier[0]
                   + (((uint16_t)(binding.identifier[1])) << 8));
      apsFrame->groupId = groupDest;
      apsFrame->destinationEndpoint = binding.remote;

      status = emberAfSendMulticast(groupDest,        // multicast ID
                                    apsFrame,
                                    messageLength,
                                    message);

      if (status != EMBER_SUCCESS) {
        return status;
      }
    }
  }
#endif // (EMBER_BINDING_TABLE_SIZE > 0)

  return status;
}

EmberStatus emberAfSendBroadcastWithCallback(EmberNodeId destination,
                                             EmberApsFrame *apsFrame,
                                             uint16_t messageLength,
                                             uint8_t *message,
                                             EmberAfMessageSentFunction callback)
{
  return send(EMBER_OUTGOING_BROADCAST,
              destination,
              apsFrame,
              messageLength,
              message,
              true, // broadcast?
              0, //alias
              0, //sequence
              callback);
}
EmberStatus emberAfSendBroadcastWithAliasWithCallback(EmberNodeId destination,
                                                      EmberApsFrame *apsFrame,
                                                      uint16_t messageLength,
                                                      uint8_t *message,
                                                      EmberNodeId alias,
                                                      uint8_t sequence,
                                                      EmberAfMessageSentFunction callback)
{
  return send(EMBER_OUTGOING_BROADCAST_WITH_ALIAS,
              destination,
              apsFrame,
              messageLength,
              message,
              true, // broadcast?
              alias, //alias
              sequence, //sequence
              callback);
}

EmberStatus emberAfSendBroadcast(EmberNodeId destination,
                                 EmberApsFrame *apsFrame,
                                 uint16_t messageLength,
                                 uint8_t *message)
{
  return emberAfSendBroadcastWithCallback(destination,
                                          apsFrame,
                                          messageLength,
                                          message,
                                          NULL);
}

EmberStatus emberAfSendUnicastWithCallback(EmberOutgoingMessageType type,
                                           uint16_t indexOrDestination,
                                           EmberApsFrame *apsFrame,
                                           uint16_t messageLength,
                                           uint8_t *message,
                                           EmberAfMessageSentFunction callback)
{
  // The source endpoint in the APS frame MAY NOT be valid at this point if the
  // outgoing type is "via binding."
  if (type == EMBER_OUTGOING_VIA_BINDING) {
    // If using binding, set the endpoints based on those in the binding.  The
    // cluster in the binding is not used because bindings can be used to send
    // messages with any cluster id, not just the one set in the binding.
    EmberBindingTableEntry binding;
    EmberStatus status = emberGetBinding(indexOrDestination, &binding);
    if (status != EMBER_SUCCESS) {
      return status;
    }
    apsFrame->sourceEndpoint = binding.local;
    apsFrame->destinationEndpoint = binding.remote;
  }
  return send(type,
              indexOrDestination,
              apsFrame,
              messageLength,
              message,
              false, // broadcast?
              0, //alias
              0, //sequence
              callback);
}

EmberStatus emberAfSendUnicast(EmberOutgoingMessageType type,
                               uint16_t indexOrDestination,
                               EmberApsFrame *apsFrame,
                               uint16_t messageLength,
                               uint8_t *message)
{
  return emberAfSendUnicastWithCallback(type,
                                        indexOrDestination,
                                        apsFrame,
                                        messageLength,
                                        message,
                                        NULL);
}

EmberStatus emberAfSendUnicastToBindingsWithCallback(EmberApsFrame *apsFrame,
                                                     uint16_t messageLength,
                                                     uint8_t* message,
                                                     EmberAfMessageSentFunction callback)
{
  EmberStatus status = EMBER_INVALID_BINDING_INDEX;
#if (EMBER_BINDING_TABLE_SIZE > 0)
  uint8_t i;

  for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
    EmberBindingTableEntry binding;
    status = emberGetBinding(i, &binding);
    if (status != EMBER_SUCCESS) {
      return status;
    }
    if (binding.type == EMBER_UNICAST_BINDING
#ifndef EMBER_MULTI_NETWORK_STRIPPED
        && binding.networkIndex == emberGetCurrentNetwork()
#endif // EMBER_MULTI_NETWORK_STRIPPED
        && binding.local == apsFrame->sourceEndpoint
        && binding.clusterId == apsFrame->clusterId) {
      apsFrame->destinationEndpoint = binding.remote;

      status = send(EMBER_OUTGOING_VIA_BINDING,
                    i,
                    apsFrame,
                    messageLength,
                    message,
                    false, // broadcast?
                    0, //alias
                    0, //sequence
                    callback);
      if (status != EMBER_SUCCESS) {
        return status;
      }
    }
  }
#endif // (EMBER_BINDING_TABLE_SIZE > 0)

  return status;
}

EmberStatus emberAfSendUnicastToBindings(EmberApsFrame *apsFrame,
                                         uint16_t messageLength,
                                         uint8_t* message)
{
  return emberAfSendUnicastToBindingsWithCallback(apsFrame,
                                                  messageLength,
                                                  message,
                                                  NULL);
}

EmberStatus emberAfSendInterPan(EmberPanId panId,
                                const EmberEUI64 destinationLongId,
                                EmberNodeId destinationShortId,
                                EmberMulticastId multicastId,
                                EmberAfClusterId clusterId,
                                EmberAfProfileId profileId,
                                uint16_t messageLength,
                                uint8_t* messageBytes)
{
  EmberAfInterpanHeader header;
  MEMSET(&header, 0, sizeof(EmberAfInterpanHeader));
  header.panId = panId;
  header.shortAddress = destinationShortId;
  if (destinationLongId != NULL) {
    MEMMOVE(header.longAddress, destinationLongId, EUI64_SIZE);
    header.options |= EMBER_AF_INTERPAN_OPTION_MAC_HAS_LONG_ADDRESS;
    header.messageType = EMBER_AF_INTER_PAN_UNICAST;
  } else if (multicastId != 0) {
    header.groupId = multicastId;
    header.messageType = EMBER_AF_INTER_PAN_MULTICAST;
  } else {
    header.messageType = (destinationShortId < EMBER_BROADCAST_ADDRESS
                          ? EMBER_AF_INTER_PAN_UNICAST
                          : EMBER_AF_INTER_PAN_BROADCAST);
  }
  header.profileId = profileId;
  header.clusterId = clusterId;
  return emberAfInterpanSendMessageCallback(&header,
                                            messageLength,
                                            messageBytes);
}

void emberAfPrintMessageData(uint8_t* data, uint16_t length)
{
#ifdef EMBER_AF_PRINT_APP
  emberAfAppPrint(" payload (len %2x) [", length);
  emberAfAppPrintBuffer(data, length, true);
  emberAfAppPrintln("]");
#endif // EMBER_AF_PRINT_APP
}

void emAfPrintStatus(const char * task,
                     EmberStatus status)
{
  if (status == EMBER_SUCCESS) {
    emberAfPrint(emberAfPrintActiveArea,
                 "%p: %p",
                 "Success",
                 task);
  } else {
    emberAfPrint(emberAfPrintActiveArea,
                 "%p: %p: 0x%x",
                 "Error",
                 task,
                 status);
  }
}

EmberStatus emberAfPermitJoin(uint8_t duration,
                              bool broadcastMgmtPermitJoin)
{
  // Permit joining forever is bad behavior, so we want to limit
  // this.  If 254 is not enough a re-broadcast should be done later.
  if (duration == EMBER_AF_PERMIT_JOIN_FOREVER) {
    emberAfAppPrintln("Limiting duration of permit join from forever (255) to 254");
    duration = EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT;
  }
  return emAfPermitJoin(duration,
                        broadcastMgmtPermitJoin);
}

EmberAfCbkeKeyEstablishmentSuite emberAfIsFullSmartEnergySecurityPresent(void)
{
  EmberAfCbkeKeyEstablishmentSuite cbkeKeyEstablishmentSuite = EMBER_AF_INVALID_KEY_ESTABLISHMENT_SUITE;

#if defined EMBER_AF_HAS_SECURITY_PROFILE_SE
  EmberCertificateData cert;
  EmberCertificate283k1Data cert283k1;

  if ((emberGetLibraryStatus(EMBER_ECC_LIBRARY_ID)
       & EMBER_LIBRARY_PRESENT_MASK)
      && (EMBER_SUCCESS == emberGetCertificate(&cert)) ) {
    cbkeKeyEstablishmentSuite |= EMBER_AF_CBKE_KEY_ESTABLISHMENT_SUITE_163K1;
  }

  if ((emberGetLibraryStatus(EMBER_ECC_LIBRARY_283K1_ID)
       & EMBER_LIBRARY_PRESENT_MASK)
      &&  (EMBER_SUCCESS == emberGetCertificate283k1(&cert283k1))) {
    cbkeKeyEstablishmentSuite |= EMBER_AF_CBKE_KEY_ESTABLISHMENT_SUITE_283K1;
  }
#endif

  return cbkeKeyEstablishmentSuite;
}

EmberStatus emberAfFormNetwork(EmberNetworkParameters *parameters)
{
  EmberStatus status = EMBER_INVALID_CALL;
  EmberCurrentSecurityState securityState;
  if (emAfProIsCurrentNetwork()) {
    emberAfCorePrintln("%ping on ch %d, panId 0x%2X",
                       "Form",
                       parameters->radioChannel,
                       parameters->panId);
    emberAfCoreFlush();
    if (emAfCurrentZigbeeProNetwork->nodeType == EMBER_COORDINATOR) {
      zaTrustCenterSecurityInit(true); // centralized network
    }
    // ignore return value for now since it always returns EMBER_SUCCESS
    (void)emberGetCurrentSecurityState(&securityState);
    if (emAfCurrentZigbeeProNetwork->nodeType == EMBER_COORDINATOR  \
        || ((emAfCurrentZigbeeProNetwork->nodeType == EMBER_ROUTER) \
            && (securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE))) {
      status = emberFormNetwork(parameters);
    } else {
      emberAfCorePrintln("Error: Device does not support %s network formation",
                         (securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE) ? "distributed" : "centralized");
    }
  }
  return status;
}

EmberStatus emberAfJoinNetwork(EmberNetworkParameters *parameters)
{
  EmberStatus status = EMBER_INVALID_CALL;
  if (emAfProIsCurrentNetwork()) {
    EmberNodeType nodeType = emAfCurrentZigbeeProNetwork->nodeType;
    if (nodeType == EMBER_COORDINATOR) {
      nodeType = EMBER_ROUTER;
    }
    zaNodeSecurityInit(true); // centralized network
    emberAfCorePrintln("%ping on ch %d, panId 0x%2X",
                       "Join",
                       parameters->radioChannel,
                       parameters->panId);
    status = emberJoinNetwork(nodeType, parameters);
  }
  return status;
}

// mfgString is expected to be +1 of MFG_STRING_MAX_LENGTH
void emberAfFormatMfgString(uint8_t* mfgString)
{
  uint8_t i;
  emberAfGetMfgString(mfgString);

  for (i = 0; i < MFG_STRING_MAX_LENGTH; i++) {
    // The MFG string is not necessarily NULL terminated.
    // Uninitialized bytes are left at 0xFF so we make sure
    // it is NULL terminated.
    if (mfgString[i] == 0xFF) {
      mfgString[i] = '\0';
    }
  }
  mfgString[MFG_STRING_MAX_LENGTH] = '\0';
}

static const EmberReleaseTypeStruct releaseTypes[] = {
  EMBER_RELEASE_TYPE_TO_STRING_STRUCT_DATA
};

//------------------------------------------------------------------------------
// Internal APIs

// Called from emAfInitCallback() in af-soc.c or af-host.c
// Initialize stack profile and security level based on security profile.
void emAfNetworkSecurityInit(void)
{
  // Typically, we initialize the stack profile based on EMBER_STACK_PROFILE
  // and initialize the security level based on EMBER_SECURITY_LEVEL.

  // However, such an approach cannot properly initialize the stack profile and
  // security level for coordinators (e.g. Multi-PAN devices) which can form
  // multiple networks with different stack profiles and security levels, because
  // the EMBER_STACK_PROFILE and EMBER_SECURITY_LEVEL are common for each network.

  // As each network has its own security profile, instead of using EMBER_STACK_PROFILE
  // and EMBER_SECURITY_LEVEL, we have this function to set up the stack profile and
  // security level based on the security profile of each network.
  uint8_t stackProfile;
  uint8_t securityLevel;
  for (uint8_t i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    (void) emberAfPushNetworkIndex(i);
    if (emAfProIsCurrentNetwork()) {
      switch (emAfCurrentZigbeeProNetwork->securityProfile) {
        case EMBER_AF_SECURITY_PROFILE_NONE:
          stackProfile = EMBER_STACK_PROFILE_NONE;
          securityLevel = EMBER_SECURITY_LEVEL_NONE;
          break;
        case EMBER_AF_SECURITY_PROFILE_HA:
        case EMBER_AF_SECURITY_PROFILE_HA12:
        case EMBER_AF_SECURITY_PROFILE_SE_TEST:
        case EMBER_AF_SECURITY_PROFILE_SE_FULL:
        case EMBER_AF_SECURITY_PROFILE_Z3:
        case EMBER_AF_SECURITY_PROFILE_CUSTOM:
          stackProfile = EMBER_STACK_PROFILE_ZIGBEE_PRO;
          securityLevel = EMBER_SECURITY_LEVEL_Z3;
          break;
        default:
          emberAfCorePrintln("Invalid Security Profile: 0x%X",
                             emAfCurrentZigbeeProNetwork->securityProfile);
          EMBER_TEST_ASSERT(false);
          (void) emberAfPopNetworkIndex();
          return;
      }
      setStackProfile(stackProfile);
      setSecurityLevel(securityLevel);
    }
    (void) emberAfPopNetworkIndex();
  }
}

// This is templated into zigbee_af_callback:init
// If possible, initialize each network.  For ZigBee PRO networks, the node
// type of the device must match the one used previously, but note that
// coordinator-capable devices are allowed to initialize as routers.
#if (EMBER_AF_TC_SWAP_OUT_TEST == 0)
void emAfNetworkInit(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    bool initialize = true;
    (void) emberAfPushNetworkIndex(i);
    emAfClearNetworkCache(i);
    if (emAfProIsCurrentNetwork()) {
      EmberNodeType nodeType;
      if (emAfCurrentZigbeeProNetwork->nodeType == EMBER_COORDINATOR) {
        zaTrustCenterSecurityPolicyInit();
      }
      if (emberAfGetNodeType(&nodeType) == EMBER_SUCCESS
          && (nodeType != emAfCurrentZigbeeProNetwork->nodeType
              && (nodeType != EMBER_S2S_INITIATOR_DEVICE)
              && (nodeType != EMBER_S2S_TARGET_DEVICE)
              && (emAfCurrentZigbeeProNetwork->nodeType != EMBER_COORDINATOR
                  || nodeType != EMBER_ROUTER))) {
        initialize = false;
      }
    }
    if (initialize) {
      EmberNetworkInitStruct networkInitStruct = { EMBER_AF_CUSTOM_NETWORK_INIT_OPTIONS };
      emberNetworkInit(&networkInitStruct);
    }
    (void) emberAfPopNetworkIndex();
  }
}
//EMBER_AF_TC_SWAP_OUT_TEST Prevent calling emAfNetworkInit during stack initialization
#else // EMBER_AF_TC_SWAP_OUT_TEST
void emAfNetworkInit(SLXU_INIT_ARG)
{
}
#endif //EMBER_AF_TC_SWAP_OUT_TEST
// Called from emAfInitCallback() in af-soc.c or af-host.c
void emAfInitializeMessageSentCallbackArray(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE; i++) {
    messageSentCallbacks[i].tag = INVALID_MESSAGE_TAG;
    messageSentCallbacks[i].callback = NULL;
  }
}

void emMarkMessageSentTagBuffers(void)
{
#ifndef EZSP_HOST
  for (uint8_t i = 0; i < EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE; i++) {
    emMarkBuffer(&(messageSentCallbacks[i].tag));
  }
#endif
}

// Old API that doesn't restrict prevent permit joining forever (255)
EmberStatus emAfPermitJoin(uint8_t duration,
                           bool broadcastMgmtPermitJoin)
{
  EmberStatus status = emberPermitJoining(duration);
  emberAfAppPrintln("pJoin for %d sec: 0x%x", duration, status);
  if (broadcastMgmtPermitJoin) {
    status = broadcastPermitJoin(duration);
  }
  return status;
}

bool emAfProcessZdo(EmberNodeId sender,
                    EmberApsFrame* apsFrame,
                    uint8_t* message,
                    uint16_t length)
{
  if (apsFrame->profileId != EMBER_ZDO_PROFILE_ID) {
    return false;
  }

  // To make the printing simpler, we assume all 'request' messages
  // have a status of 0x00.  Request messages have no status value in them
  // but saying 'success' (0x00) seems appropriate.
  // Response messages will have their status value printed appropriately.
  emberAfZdoPrintln("RX: ZDO, command 0x%2x, status: 0x%X",
                    apsFrame->clusterId,
                    (apsFrame->clusterId >= CLUSTER_ID_RESPONSE_MINIMUM
                     ? message[1]
                     : 0));

  switch (apsFrame->clusterId) {
    case SIMPLE_DESCRIPTOR_RESPONSE:
      emberAfZdoPrintln("RX: %p Desc Resp", "Simple");
      break;
    case MATCH_DESCRIPTORS_RESPONSE:
      emberAfZdoPrint("RX: %p Desc Resp", "Match");
      emberAfZdoPrintln(", Matches: %d", message[4]);
      break;
    case END_DEVICE_BIND_RESPONSE:
      emberAfZdoPrintln("RX: End dev bind response, status=%x", message[1]);
      break;
    case END_DEVICE_ANNOUNCE:
      emberAfZdoPrintln("Device Announce: 0x%2x",
                        (uint16_t)(message[1]) + (uint16_t)(message[2] << 8));
      break;
    case IEEE_ADDRESS_RESPONSE:
      emberAfZdoPrintln("RX: IEEE Address Response");
      break;
    case ACTIVE_ENDPOINTS_RESPONSE:
      emberAfZdoPrintln("RX: Active EP Response, Count: %d", message[4]);
      break;
    case NODE_DESCRIPTOR_RESPONSE:
      emberAfZdoPrint("RX: %p Desc Resp", "Node");
      emberAfZdoPrintln(", Matches: 0x%2x",
                        (uint16_t)(message[1]) + (uint16_t)(message[2] << 8));
      break;
    default:
      break;
  }

  if (emAfPreZDOMessageReceived(sender, apsFrame, message, length)) {
    goto zdoProcessingDone;
  }

  emAfZDOMessageReceived(sender, apsFrame, message, length);

  switch (apsFrame->clusterId) {
    case BIND_RESPONSE:
      emberAfPartnerLinkKeyExchangeResponseCallback(sender, message[1]);
      break;
#ifdef SL_CATALOG_ZIGBEE_SUB_GHZ_SERVER_PRESENT
    case NWK_UNSOLICITED_ENHANCED_UPDATE_NOTIFY:
      emAfSubGhzServerZDOMessageReceivedCallback(sender, message, length);
      break;
#endif
    default:
      break;
  }

  zdoProcessingDone:
  // if it is a zdo response we can remove the zdo waiting task
  // and let a sleepy go back into hibernation
  if (apsFrame->clusterId > CLUSTER_ID_RESPONSE_MINIMUM) {
    emberAfRemoveFromCurrentAppTasks(EMBER_AF_WAITING_FOR_ZDO_RESPONSE);
  }

  return true;
}

void emAfParseAndPrintVersion(EmberVersion versionStruct)
{
  uint8_t i = 0;
  const char * typeText = NULL;
  while (releaseTypes[i].typeString != NULL) {
    if (releaseTypes[i].typeNum == versionStruct.type) {
      typeText = releaseTypes[i].typeString;
    }
    i++;
  }
  emberAfAppPrint("stack ver. [%d.%d.%d",
                  versionStruct.major,
                  versionStruct.minor,
                  versionStruct.patch);
  if (versionStruct.special != 0) {
    emberAfAppPrint(".%d",
                    versionStruct.special);
  }
  UNUSED_VAR(typeText);
  emberAfAppPrintln(" %p build %d]",
                    (typeText == NULL
                     ? "???"
                     : typeText),
                    versionStruct.build);
  emberAfAppFlush();
}

//------------------------------------------------------------------------------
// Static functions

static uint8_t getMessageSentCallbackIndex(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE; i++) {
    if (messageSentCallbacks[i].tag == INVALID_MESSAGE_TAG) {
      return i;
    }
  }

  return INVALID_CALLBACK_INDEX;
}

static void invalidateMessageSentCallbackEntry(uint16_t tag)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE; i++) {
    if (messageSentCallbacks[i].tag == tag) {
      messageSentCallbacks[i].tag = INVALID_MESSAGE_TAG;
      messageSentCallbacks[i].callback = NULL;
      return;
    }
  }
}

static EmberAfMessageSentFunction getMessageSentCallback(uint16_t tag)
{
  uint8_t i;
  for (i = 0; i < EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE; i++) {
    if (messageSentCallbacks[i].tag == tag) {
      return messageSentCallbacks[i].callback;
    }
  }

  return NULL;
}

static EmberStatus send(EmberOutgoingMessageType type,
                        uint16_t indexOrDestination,
                        EmberApsFrame *apsFrame,
                        uint16_t messageLength,
                        uint8_t *message,
                        bool broadcast,
                        EmberNodeId alias,
                        uint8_t sequence,
                        EmberAfMessageSentFunction callback)
{
  EmberStatus status;
  uint8_t commandId;
  uint8_t index;
  uint8_t messageSentIndex;
  uint16_t messageTag = INVALID_MESSAGE_TAG;

  // The send APIs only deal with ZCL messages, so they must at least contain
  // the ZCL header.
  if (messageLength < EMBER_AF_ZCL_OVERHEAD) {
    return EMBER_ERR_FATAL;
  } else if ((message[0] & ZCL_MANUFACTURER_SPECIFIC_MASK) != 0U) {
    if (messageLength < EMBER_AF_ZCL_MANUFACTURER_SPECIFIC_OVERHEAD) {
      return EMBER_ERR_FATAL;
    }
    commandId = message[4];
  } else {
    commandId = message[2];
  }

  messageSentIndex = getMessageSentCallbackIndex();
  if (callback != NULL && messageSentIndex == INVALID_CALLBACK_INDEX) {
    return EMBER_TABLE_FULL;
  }

  // The source endpoint in the APS frame MUST be valid at this point.  We use
  // it to set the appropriate outgoing network as well as the profile id in
  // the APS frame.
  EmberAfEndpointInfoStruct endpointInfo;
  uint8_t networkIndex = 0;
  if (emberAfGetEndpointInfoCallback(apsFrame->sourceEndpoint,
                                     &networkIndex,
                                     &endpointInfo)) {
    apsFrame->profileId = endpointInfo.profileId;
    status = emberAfPushNetworkIndex(networkIndex);
    if (status != EMBER_SUCCESS) {
      return status;
    }
  } else {
    index = emberAfIndexFromEndpoint(apsFrame->sourceEndpoint);
    if (index == 0xFF) {
      return EMBER_INVALID_ENDPOINT;
    }
    status = emberAfPushEndpointNetworkIndex(apsFrame->sourceEndpoint);
    if (status != EMBER_SUCCESS) {
      return status;
    }
    apsFrame->profileId = emberAfProfileIdFromIndex(index);
  }

#ifdef SL_CATALOG_ZIGBEE_SUB_GHZ_CLIENT_PRESENT
  // If the Sub-GHz client is present and currently in the "suspended" state,
  // block any outgoing message unless it comes from the Sub-GHz client itself.
  if (emberAfPluginSubGhzClientIsSendingZclMessagesSuspended()
      && apsFrame->clusterId != ZCL_SUB_GHZ_CLUSTER_ID) {
    return EMBER_TRANSMISSION_SUSPENDED;
  }
#endif // SL_CATALOG_ZIGBEE_SUB_GHZ_CLIENT_PRESENT

  // Encryption is turned on if it is required, but not turned off if it isn't.
  // This allows the application to send encrypted messages in special cases
  // that aren't covered by the specs by manually setting the encryption bit
  // prior to calling the send APIs.
  if (emberAfDetermineIfLinkSecurityIsRequired(commandId,
                                               false, // incoming?
                                               broadcast,
                                               apsFrame->profileId,
                                               apsFrame->clusterId,
                                               (type == EMBER_OUTGOING_DIRECT)
                                               ? indexOrDestination
                                               : EMBER_NULL_NODE_ID)) {
    apsFrame->options |= EMBER_APS_OPTION_ENCRYPTION;
  }

  {
    EmberAfMessageStruct messageStruct = {
      callback,
      apsFrame,
      message,
      indexOrDestination,
      messageLength,
      type,
      broadcast,
    };
    // Called prior to fragmentation in case the mesasge does not go out over the
    // Zigbee radio, and instead goes to some other transport that does not require
    // low level ZigBee fragmentation.
    if (emberAfPreMessageSendCallback(&messageStruct,
                                      &status)) {
      return status;
    }
  }

  // SE 1.4 requires an option to disable APS ACK and Default Response
  emAfApplyDisableDefaultResponse(&message[0]);
  emAfApplyRetryOverride(&apsFrame->options);

  if (messageLength
      <= emberAfMaximumApsPayloadLength(type, indexOrDestination, apsFrame)) {
    status = emAfSend(type,
                      indexOrDestination,
                      apsFrame,
                      (uint8_t)messageLength,
                      message,
                      &messageTag,
                      alias,
                      sequence);
#ifdef SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
  } else if (!broadcast) {
    status = emAfFragmentationSendUnicast(type,
                                          indexOrDestination,
                                          apsFrame,
                                          message,
                                          messageLength,
                                          &messageTag);
    emberAfDebugPrintln("%pstart:len=%d.", "Fragmentation:", messageLength);
#endif // SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
  } else {
    status = EMBER_MESSAGE_TOO_LONG;
  }

#ifdef SL_CATALOG_ZIGBEE_CRITICAL_MESSAGE_QUEUE_PRESENT
  // If this was a critical message queue entry, fire the callback
  if ((status != EMBER_SUCCESS)
      && (callback == emberAfPluginCriticalMessageQueueEnqueueCallback
          || callback == emAfPluginCriticalMessageQueueRetryCallback)) {
    callback(type,
             indexOrDestination,
             apsFrame,
             messageLength,
             message,
             status);
  }
#endif // SL_CATALOG_ZIGBEE_CRITICAL_MESSAGE_QUEUE_PRESENT

  if (callback != NULL
      && status == EMBER_SUCCESS
      && messageTag != INVALID_MESSAGE_TAG
      && messageSentIndex < EMBER_AF_MESSAGE_SENT_CALLBACK_TABLE_SIZE) {
    messageSentCallbacks[messageSentIndex].tag = messageTag;
    messageSentCallbacks[messageSentIndex].callback = callback;
  }

  if (status == EMBER_OPERATION_IN_PROGRESS
      && apsFrame->options & EMBER_APS_OPTION_DSA_SIGN) {
    // We consider "in progress" signed messages as being sent successfully.
    // The stack will send the message after signing.
    status = EMBER_SUCCESS;
    emAfSetCryptoOperationInProgress();
  }

  if (status == EMBER_SUCCESS) {
    emberAfAddToCurrentAppTasks(EMBER_AF_WAITING_FOR_DATA_ACK
                                | EMBER_AF_WAITING_FOR_ZCL_RESPONSE);
  }

  (void) emberAfPopNetworkIndex();
  return status;
}

static EmberStatus broadcastPermitJoin(uint8_t duration)
{
  EmberStatus status;
  uint8_t data[3] = { 0,   // sequence number (filled in later)
                      0,   // duration (filled in below)
                      1 }; // TC significance (always 1)

  data[1] = duration;
  status = emberSendZigDevRequest(EMBER_BROADCAST_ADDRESS,
                                  PERMIT_JOINING_REQUEST,
                                  0,   // APS options
                                  data,
                                  3);  // length
  return status;
}

static void printMessage(EmberIncomingMessageType type,
                         EmberApsFrame* apsFrame,
                         uint16_t messageLength,
                         uint8_t* messageContents)
{
  emberAfAppPrint("Profile: %p (0x%2X), Cluster: 0x%2X, %d bytes,",
                  (apsFrame->profileId == EMBER_ZDO_PROFILE_ID
                   ? "ZDO"
                   : (apsFrame->profileId == SE_PROFILE_ID
                      ? "SE"
                      : (apsFrame->profileId == 0x0104
                         ? "HA"
                         : "??"))),
                  apsFrame->profileId,
                  apsFrame->clusterId,
                  messageLength);
  if (apsFrame->profileId != EMBER_ZDO_PROFILE_ID
      && messageLength >= 3) {
    emberAfAppPrint(" ZCL %p Cmd ID: %d",
                    (messageContents[0] & ZCL_CLUSTER_SPECIFIC_COMMAND
                     ? "Cluster"
                     : "Global"),
                    messageContents[0] & ZCL_MANUFACTURER_SPECIFIC_MASK
                    ? messageContents[4] : messageContents[2]);
  }
  emberAfAppPrintln("");
}

EmberAfStatus emberAfStatus(bool wasHandled, bool clusterExists)
{
  if (wasHandled) {
    return EMBER_ZCL_STATUS_SUCCESS;
  } else if (clusterExists) {
    return EMBER_ZCL_STATUS_UNSUP_COMMAND;
  } else {
    return EMBER_ZCL_STATUS_UNSUPPORTED_CLUSTER;
  }
}

//------------------------------------------------------------------------------
// Weak callbacks

WEAK(bool emberAfMessageSentCallback(EmberOutgoingMessageType type,
                                     uint16_t indexOrDestination,
                                     EmberApsFrame* apsFrame,
                                     uint16_t msgLen,
                                     uint8_t* message,
                                     EmberStatus status))
{
  return false;
}
