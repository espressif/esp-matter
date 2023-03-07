/***************************************************************************//**
 * @file af-soc.c
 * @brief SoC-specific APIs and infrastructure code.
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

#include "af.h"
#include "sl_component_catalog.h"

#include "app/framework/util/af-main.h"
#include "app/framework/util/attribute-storage.h"
#include "app/util/security/security.h"
#include "app/util/zigbee-framework/zigbee-device-library.h"
#include "stack/include/source-route.h"
#include "stack/config/ember-configuration-defaults.h"

#ifdef SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
#include "fragmentation.h"
#endif

#ifdef SL_CATALOG_ZIGBEE_FORM_AND_JOIN_PRESENT
#include "app/util/common/form-and-join.h"
#endif

#if (defined(SL_CATALOG_ZIGBEE_DEBUG_PRINT_PRESENT) && !defined(EMBER_TEST))
#define EXTENDED_RESET_INFO
#include "cortexm3/diagnostic.h"
#endif // !defined(EMBER_TEST)

//------------------------------------------------------------------------------
// Forward declarations

static uint16_t calculateMessageTagHash(uint8_t *messageContents,
                                        uint8_t messageLength);

//------------------------------------------------------------------------------
// Internal callbacks

void emAfInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  emberAfCorePrintln("Reset info: 0x%x (%p)",
                     halGetResetInfo(),
                     halGetResetString());

#if defined(EXTENDED_RESET_INFO)
  emberAfCorePrintln("Extended Reset info: 0x%2X (%p)",
                     halGetExtendedResetInfo(),
                     halGetExtendedResetString());

  if (halResetWasCrash()) {
    // We pass port 0 here though this parameter is unused in the legacy HAL
    // version of the diagnostic code.
    halPrintCrashSummary(0);
    halPrintCrashDetails(0);
    halPrintCrashData(0);
  }
#endif // EXTENDED_RESET_INFO

  // This will initialize the stack of networks maintained by the framework,
  // including setting the default network.
  emAfInitializeNetworkIndexStack();

  // Initialize messageSentCallbacks table
  emAfInitializeMessageSentCallbackArray();

  emberAfEndpointConfigure();

  // The address cache needs to be initialized and used with the source routing
  // code for the trust center to operate properly.
  securityAddressCacheInit(EMBER_AF_PLUGIN_ADDRESS_TABLE_SIZE,                     // offset
                           EMBER_AF_PLUGIN_ADDRESS_TABLE_TRUST_CENTER_CACHE_SIZE); // size

  emAfNetworkSecurityInit();

  // Set the manufacturing code. This is defined by ZigBee document 053874r10
  // Ember's ID is 0x1002 and is the default, but this can be overridden in App Builder.
  emberSetManufacturerCode(EMBER_AF_MANUFACTURER_CODE);

  emberSetMaximumIncomingTransferSize(EMBER_AF_INCOMING_BUFFER_LENGTH);
  emberSetMaximumOutgoingTransferSize(EMBER_AF_MAXIMUM_SEND_PAYLOAD_LENGTH);
  emberSetTxPowerMode(EMBER_AF_TX_POWER_MODE);

  (void)emberSetRadioIeee802154CcaMode(EMBER_RADIO_802154_CCA_MODE);
}

EmberZdoStatus emAfRemoteSetBindingCallback(EmberBindingTableEntry *entry)
{
  EmberZdoStatus status = EMBER_ZDP_TABLE_FULL;
#if (EMBER_BINDING_TABLE_SIZE > 0)
  EmberStatus setStatus;
  EmberBindingTableEntry candidate;
  uint8_t i;

  // If we receive a bind request for the Key Establishment cluster and we are
  // not the trust center, then we are doing partner link key exchange.  We
  // don't actually have to create a binding.
  if (emberAfGetNodeId() != EMBER_TRUST_CENTER_NODE_ID
      && entry->clusterId == ZCL_KEY_ESTABLISHMENT_CLUSTER_ID) {
    status = emberAfPartnerLinkKeyExchangeRequestCallback(entry->identifier);
    goto kickout;
  }

  // ask the application if current binding request is allowed or not
  status = emberAfRemoteSetBindingPermissionCallback(entry) == EMBER_SUCCESS ? EMBER_ZDP_SUCCESS : EMBER_ZDP_NOT_AUTHORIZED;
  if (status == EMBER_ZDP_SUCCESS) {
    // For all other requests, we search the binding table for an unused entry
    // and store the new entry there if we find one.
    for (i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
      if (emberGetBinding(i, &candidate) == EMBER_SUCCESS
          && candidate.type == EMBER_UNUSED_BINDING) {
        setStatus = emberSetBinding(i, entry);
        status = setStatus == EMBER_SUCCESS ? EMBER_ZDP_SUCCESS // binding set
                 : (setStatus == EMBER_BINDING_IS_ACTIVE ? EMBER_ZDP_NOT_PERMITTED // selected index is active
                    : EMBER_ZDP_TABLE_FULL); // report full for any other failure
        goto kickout;
      }
    }
    // If we get here, we didn't find an empty table slot, so table is full.
    status = EMBER_ZDP_TABLE_FULL;
  }

  kickout:
#endif
  return status;
}

EmberZdoStatus emAfRemoteDeleteBindingCallback(uint8_t index)
{
  EmberStatus deleteStatus;
  EmberZdoStatus status;

  // ask the application if current binding request is allowed or not
  status = emberAfRemoteDeleteBindingPermissionCallback(index) == EMBER_SUCCESS
           ? EMBER_ZDP_SUCCESS
           : EMBER_ZDP_NOT_AUTHORIZED;
  if (status == EMBER_SUCCESS) {
    deleteStatus = emberDeleteBinding(index);
    status = deleteStatus == EMBER_SUCCESS ? EMBER_ZDP_SUCCESS // binding deleted
             : (deleteStatus == EMBER_BINDING_IS_ACTIVE ? EMBER_ZDP_NOT_PERMITTED //selected index is active
                : EMBER_ZDP_NO_ENTRY); // report no entry for any other failure
    emberAfZdoPrintln("delete binding: %x %x", index, status);
  }

  return status;
}

void emAfIncomingMessageCallback(EmberIncomingMessageType type,
                                 EmberApsFrame *apsFrame,
                                 EmberMessageBuffer message)
{
  uint8_t lastHopLqi;
  int8_t lastHopRssi;

  emberGetLastHopLqi(&lastHopLqi);
  emberGetLastHopRssi(&lastHopRssi);

  emAfIncomingMessageHandler(type,
                             apsFrame,
                             lastHopLqi,
                             lastHopRssi,
                             emGetBufferLength(message),
                             emGetBufferPointer(message));
}

// Called when a message we sent is acked by the destination or when an
// ack fails to arrive after several retransmissions.
void emAfMessageSentCallback(EmberOutgoingMessageType type,
                             uint16_t indexOrDestination,
                             EmberApsFrame *apsFrame,
                             EmberMessageBuffer message,
                             EmberStatus status)
{
  uint8_t* messageContents = emGetBufferPointer(message);
  uint8_t messageLength = emGetBufferLength(message);
  uint16_t messageTag;

#ifdef SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT
  if (emAfFragmentationMessageSent(apsFrame, status)) {
    return;
  }
#endif //SL_CATALOG_ZIGBEE_FRAGMENTATION_PRESENT

  emberConcentratorNoteDeliveryFailure(type, status);

  messageTag = calculateMessageTagHash(messageContents, messageLength);
  emAfMessageSentHandler(type,
                         indexOrDestination,
                         apsFrame,
                         status,
                         messageLength,
                         messageContents,
                         messageTag);
}

//------------------------------------------------------------------------------
// Public APIs

void emberAfGetMfgString(uint8_t* returnData)
{
  halCommonGetMfgToken(returnData, TOKEN_MFG_STRING);
}

EmberNodeId emberAfGetNodeId(void)
{
  return emberGetNodeId();
}

EmberPanId emberAfGetPanId(void)
{
  return emberGetPanId();
}

EmberNetworkStatus emberAfNetworkState(void)
{
  return emberNetworkState();
}

uint8_t emberAfGetRadioChannel(void)
{
  return emberGetRadioChannel();
}

uint8_t emberAfGetBindingIndex(void)
{
  return emberGetBindingIndex();
}

uint8_t emberAfGetStackProfile(void)
{
  return emberStackProfile();
}

uint8_t emberAfGetAddressIndex(void)
{
  EmberEUI64 longId;
  EmberEUI64 longIdIterator;
  uint8_t i;

  if ((emberGetSenderEui64(longId)) == EMBER_SUCCESS) {
    for (i = 0; i < EMBER_AF_PLUGIN_ADDRESS_TABLE_SIZE; i++) {
      emberGetAddressTableRemoteEui64(i, longIdIterator);
      if (MEMCOMPARE(longIdIterator, longId, EUI64_SIZE) == 0) {
        return i;
      }
    }
  } else {
    EmberNodeId nodeId = emberGetSender();
    if (nodeId == EMBER_NULL_NODE_ID) {
      return EMBER_NULL_ADDRESS_TABLE_INDEX;
    }
    for (i = 0; i < EMBER_AF_PLUGIN_ADDRESS_TABLE_SIZE; i++) {
      if (emberGetAddressTableRemoteNodeId(i) == nodeId) {
        return i;
      }
    }
  }
  return EMBER_NULL_ADDRESS_TABLE_INDEX;
}

EmberStatus emberAfSendEndDeviceBind(uint8_t endpoint)
{
  EmberStatus status;
  EmberApsOption options = ((EMBER_AF_DEFAULT_APS_OPTIONS
                             | EMBER_APS_OPTION_SOURCE_EUI64)
                            & ~EMBER_APS_OPTION_RETRY);

  status = emberAfPushEndpointNetworkIndex(endpoint);
  if (status != EMBER_SUCCESS) {
    return status;
  }

  emberAfZdoPrintln("send %x %2x", endpoint, options);
  status = emberEndDeviceBindRequest(endpoint, options);
  emberAfZdoPrintln("done: %x.", status);
  emberAfZdoFlush();

  (void) emberAfPopNetworkIndex();
  return status;
}

uint8_t emberGetEndpoint(uint8_t index)
{
  uint8_t endpoint = EMBER_AF_INVALID_ENDPOINT;
  if (emberAfGetEndpointByIndexCallback(index, &endpoint)) {
    return endpoint;
  }
  return (((emberAfNetworkIndexFromEndpointIndex(index)
            == emberGetCallbackNetwork())
           && emberAfEndpointIndexIsEnabled(index))
          ? emberAfEndpointFromIndex(index)
          : 0xFF);
}

// must return the endpoint desc of the endpoint specified
bool emberGetEndpointDescription(uint8_t endpoint,
                                 EmberEndpointDescription *result)
{
  if (emberAfGetEndpointDescriptionCallback(endpoint, result)) {
    return true;
  }
  uint8_t endpointIndex = emberAfIndexFromEndpoint(endpoint);
  if (endpointIndex == 0xFF
      || (emberAfNetworkIndexFromEndpointIndex(endpointIndex)
          != emberGetCallbackNetwork())) {
    return false;
  }
  result->profileId          = emberAfProfileIdFromIndex(endpointIndex);
  result->deviceId           = emberAfDeviceIdFromIndex(endpointIndex);
  result->deviceVersion      = emberAfDeviceVersionFromIndex(endpointIndex);
  result->inputClusterCount  = emberAfClusterCount(endpoint, true);
  result->outputClusterCount = emberAfClusterCount(endpoint, false);
  return true;
}

// must return the clusterId at listIndex in the list specified for the
// endpoint specified
uint16_t emberGetEndpointCluster(uint8_t endpoint,
                                 EmberClusterListId listId,
                                 uint8_t listIndex)
{
  EmberAfCluster *cluster = NULL;
  uint8_t endpointIndex = emberAfIndexFromEndpoint(endpoint);
  if (endpointIndex == 0xFF
      || (emberAfNetworkIndexFromEndpointIndex(endpointIndex)
          != emberGetCallbackNetwork())) {
    return 0xFFFF;
  } else if (listId == EMBER_INPUT_CLUSTER_LIST) {
    cluster = emberAfGetNthCluster(endpoint, listIndex, true);
  } else if (listId == EMBER_OUTPUT_CLUSTER_LIST) {
    cluster = emberAfGetNthCluster(endpoint, listIndex, false);
  } else {
    // MISRA requires ..else if.. to have terminating else.
  }
  return (cluster == NULL ? 0xFFFF : cluster->clusterId);
}

void emberAfGetEui64(EmberEUI64 returnEui64)
{
  MEMMOVE(returnEui64, emberGetEui64(), EUI64_SIZE);
}

EmberStatus emberAfGetNetworkParameters(EmberNodeType* nodeType,
                                        EmberNetworkParameters* parameters)
{
  emberGetNetworkParameters(parameters);
  return emberGetNodeType(nodeType);
}

EmberStatus emberAfGetNodeType(EmberNodeType *nodeType)
{
  return emberGetNodeType(nodeType);
}

uint8_t emberAfGetSecurityLevel(void)
{
  return emberSecurityLevel();
}

uint8_t emberAfGetKeyTableSize(void)
{
  return EMBER_KEY_TABLE_SIZE;
}

uint8_t emberAfGetBindingTableSize(void)
{
  return EMBER_BINDING_TABLE_SIZE;
}

uint8_t emberAfGetAddressTableSize(void)
{
  return EMBER_ADDRESS_TABLE_SIZE;
}

uint8_t emberAfGetChildTableSize(void)
{
  return EMBER_CHILD_TABLE_SIZE;
}

uint8_t emberAfGetNeighborTableSize(void)
{
  return EMBER_NEIGHBOR_TABLE_SIZE;
}

uint8_t emberAfGetRouteTableSize(void)
{
  return EMBER_ROUTE_TABLE_SIZE;
}

uint8_t emberAfGetSleepyMulticastConfig(void)
{
  return EMBER_SEND_MULTICASTS_TO_SLEEPY_ADDRESS;
}

EmberStatus emberAfGetSourceRouteTableEntry(
  uint8_t index,
  EmberNodeId *destination,
  uint8_t *closerIndex)
{
  return emberGetSourceRouteTableEntry(index,
                                       destination,
                                       closerIndex);
}

uint8_t emberAfGetSourceRouteTableTotalSize(void)
{
  return emberGetSourceRouteTableTotalSize();
}

uint8_t emberAfGetSourceRouteTableFilledSize(void)
{
  return emberGetSourceRouteTableFilledSize();
}

EmberStatus emberAfGetChildData(uint8_t index,
                                EmberChildData *childData)
{
  return emberGetChildData(index,
                           childData);
}

//------------------------------------------------------------------------------
// Internal APIs

EmberStatus emAfSend(EmberOutgoingMessageType type,
                     uint16_t indexOrDestination,
                     EmberApsFrame *apsFrame,
                     uint8_t messageLength,
                     uint8_t *message,
                     uint16_t *messageTag,
                     EmberNodeId alias,
                     uint8_t sequence)
{
  EmberMessageBuffer payload = emberFillLinkedBuffers(message, messageLength);
  if (payload == EMBER_NULL_MESSAGE_BUFFER) {
    return EMBER_NO_BUFFERS;
  } else {
    EmberStatus status;

    *messageTag = calculateMessageTagHash(message, messageLength);

    switch (type) {
      case EMBER_OUTGOING_DIRECT:
      case EMBER_OUTGOING_VIA_ADDRESS_TABLE:
      case EMBER_OUTGOING_VIA_BINDING:
        status = emberSendUnicast(type, indexOrDestination, apsFrame, payload);
        break;
      case EMBER_OUTGOING_MULTICAST:
        status = emberSendMulticast(apsFrame,
                                    ZA_MAX_HOPS, // radius
                                    ZA_MAX_HOPS, // nonmember radius
                                    payload);
        break;
      case EMBER_OUTGOING_MULTICAST_WITH_ALIAS:
        status = emberSendMulticastWithAlias(apsFrame,
                                             apsFrame->radius, //radius
                                             apsFrame->radius, //nonmember radius
                                             payload,
                                             alias,
                                             sequence);
        break;
      case EMBER_OUTGOING_BROADCAST:
        status = emberSendBroadcast(indexOrDestination,
                                    apsFrame,
                                    ZA_MAX_HOPS, // radius
                                    payload);
        break;
      case EMBER_OUTGOING_BROADCAST_WITH_ALIAS:
        status = emberProxyBroadcast(alias,
                                     indexOrDestination,
                                     sequence,
                                     apsFrame,
                                     apsFrame->radius, // radius
                                     payload);
        break;
      default:
        status = EMBER_BAD_ARGUMENT;
        break;
    }

    emberReleaseMessageBuffer(payload);

    return status;
  }
}

uint8_t emAfGetPacketBufferFreeCount(void)
{
  return emBufferBytesRemaining() / PACKET_BUFFER_SIZE;
}

uint8_t emAfGetPacketBufferTotalCount(void)
{
  return emBufferBytesTotal() / PACKET_BUFFER_SIZE;
}

uint8_t emberAfGetOpenNetworkDurationSec(void)
{
  return sli_zigbee_get_permit_joining_remaining_duration_sec();
}

void emAfCliVersionCommand(void)
{
  emAfParseAndPrintVersion(emberVersion);
}

void emAfPrintEzspEndpointFlags(uint8_t endpoint)
{
  // Not applicable for SOC
}

//------------------------------------------------------------------------------
// Static functions

#define INVALID_MESSAGE_TAG 0x0000
static uint16_t calculateMessageTagHash(uint8_t *messageContents,
                                        uint8_t messageLength)
{
  uint8_t temp[EMBER_ENCRYPTION_KEY_SIZE];
  uint16_t hashReturn = 0;
  emberAesHashSimple(messageLength, messageContents, temp);
  for (uint8_t i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i += 2) {
    hashReturn ^= *((uint16_t *)(temp + i));
  }
  if (hashReturn == INVALID_MESSAGE_TAG) {
    hashReturn = 1;
  }
  return hashReturn;
}
