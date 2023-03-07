/***************************************************************************//**
 * @file
 * @brief Routines for the Green Power Server plugin.
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

#include "app/framework/include/af.h"
#include "app/framework/util/af-main.h"
#include "app/framework/util/common.h"
#include "stack/include/gp-types.h"
#include "stack/gp/gp-sink-table.h"
#include "green-power-server.h"

#ifdef UC_BUILD
#include "green-power-server-config.h"
#include "sl_component_catalog.h"
#include "green-power-common.h"
#include "zap-cluster-command-parser.h"
#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#include "green-power-translation-table.h"
#endif
#else // !UC_BUILD
#include EMBER_AF_API_GREEN_POWER_COMMON
#ifdef EMBER_AF_PLUGIN_GREEN_POWER_TRANSLATION_TABLE
#include EMBER_AF_API_GREEN_POWER_TRANSLATION_TABLE
#define SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_GREEN_POWER_CLIENT
#define SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
#endif
// In Appbuilder, the EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS is defined
// (in auto generated AF header) if-and-only-if the token is enabled by user,
// so just redefine it to 1 else to 0.
#ifdef EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS
#undef EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS 1
#else
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS 0
#endif // EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS
#endif // UC_BUILD

#define GP_NON_MANUFACTURER_ZCL_HEADER_LENGTH           (3)
#ifndef EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES
#define EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES 1
#endif

typedef struct {
  uint8_t gpdCommand;
  ZigbeeCluster cluster;
  uint8_t endpoints[MAX_ENDPOINT_COUNT];
  uint8_t numberOfEndpoints;
} SupportedGpdCommandClusterEndpointMap;

#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEvent;
void emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEvent;
void emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emberAfPluginGreenPowerServerCommissioningWindowTimeoutEvent;
void emberAfPluginGreenPowerServerCommissioningWindowTimeoutEventHandler(SLXU_UC_EVENT);
#define genericSwitchCommissioningTimeout (&emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEvent)
#define multiSensorCommissioningTimeout (&emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEvent)
#define commissioningWindowTimeout (&emberAfPluginGreenPowerServerCommissioningWindowTimeoutEvent)
#else // !UC_BUILD
EmberEventControl emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEventControl;
EmberEventControl emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEventControl;
EmberEventControl emberAfPluginGreenPowerServerCommissioningWindowTimeoutEventControl;
#define genericSwitchCommissioningTimeout emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEventControl
#define multiSensorCommissioningTimeout emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEventControl
#define commissioningWindowTimeout emberAfPluginGreenPowerServerCommissioningWindowTimeoutEventControl
#endif // UC_BUILD

#ifndef EZSP_HOST
extern void emberHmacAesHash(const uint8_t *key,
                             const uint8_t *data,
                             uint8_t dataLength,
                             uint8_t *result);
#else
#define emberHmacAesHash(key,        \
                         data,       \
                         dataLength, \
                         result)
#endif

static GpCommDataSaved gpdCommDataSaved[EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES] = { 0 };
static uint8_t nullEui64[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static uint8_t zeroLengthZclOctateString[] = { 0 };
static uint8_t commissionedEndPoints[MAX_ENDPOINT_COUNT] = { 0 };
static uint8_t noOfCommissionedEndpoints = 0;
static EmberAfGreenPowerServerCommissioningState commissioningState = { 0 };
static bool greenPowerServerInitialised = false;

static bool gpdAddrZero(EmberGpAddress *gpdAddr)
{
  EmberGpAddress gpdAddrZero = { 0 };
  gpdAddrZero.applicationId = gpdAddr->applicationId;
  gpdAddrZero.endpoint = gpdAddr->endpoint;
  return emberAfGreenPowerCommonGpAddrCompare(&gpdAddrZero, gpdAddr);
}

static uint8_t gpNumberOfSinkEntriesInSinkTable(void)
{
  uint8_t validEntriesCount = 0;
  EmberGpSinkTableEntry entry;
  for (int entryIndex = 0; entryIndex < EMBER_GP_SINK_TABLE_SIZE; entryIndex++) {
    if (emberGpSinkTableGetEntry(entryIndex, &entry) == EMBER_SUCCESS) {
      if (entry.status == EMBER_GP_SINK_TABLE_ENTRY_STATUS_ACTIVE) {
        validEntriesCount++;
      }
    }
  }
  return validEntriesCount;
}

static uint8_t gpNumberOfFreeSinkEntries(void)
{
  return (EMBER_GP_SINK_TABLE_SIZE - gpNumberOfSinkEntriesInSinkTable());
}

static uint8_t gpNumberOfTempAllocatedCommisioningInstances(void)
{
  uint8_t allocatedEntries = 0;
  for (int i = 0; i < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES; i++) {
    if (!gpdAddrZero(&gpdCommDataSaved[i].addr)) {
      allocatedEntries++;
    }
  }
  return allocatedEntries;
}

static GpCommDataSaved *allocateCommissioningGpdInstance(EmberGpAddress *gpdAddr)
{
  // Check if there are any more sink tables availalable to allow participation of this new GPD
  if (!(gpNumberOfFreeSinkEntries() > gpNumberOfTempAllocatedCommisioningInstances())) {
    // No more allocation, because the current temporary allocations need an entry confirmed as one
    return NULL;
  }
  if (gpdAddr != NULL) {
    for (int i = 0; i < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES; i++) {
      if (gpdAddrZero(&gpdCommDataSaved[i].addr)) {
        // gpd address 0 means free
        return (&gpdCommDataSaved[i]);
      }
    }
  }
  // allocation failed
  return NULL;
}

static GpCommDataSaved *findOrAllocateCommissioningGpdInstance(EmberGpAddress *gpdAddr)
{
  if (gpdAddr == NULL) {
    return NULL;
  }
  if (gpdAddrZero(gpdAddr)) {
    return NULL;
  }
  GpCommDataSaved *gpd = emberAfGreenPowerServerFindCommissioningGpdInstance(gpdAddr);
  if (gpd != NULL) {
    return gpd;
  }
  // not found hence allocate one
  return allocateCommissioningGpdInstance(gpdAddr);
}

// Writes the TCInvolved bit - this should only be called on first joining of the node.
static void writeInvolveTCBit(void)
{
  // Proceed with processing the involveTC
  uint8_t gpsSecurityLevelAttribute = 0;
  uint8_t type;
  EmberAfStatus secLevelStatus = emberAfReadAttribute(GP_ENDPOINT,
                                                      ZCL_GREEN_POWER_CLUSTER_ID,
                                                      ZCL_GP_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_ID,
                                                      (CLUSTER_MASK_SERVER),
                                                      (uint8_t*)&gpsSecurityLevelAttribute,
                                                      sizeof(uint8_t),
                                                      &type);
  if (secLevelStatus != EMBER_ZCL_STATUS_SUCCESS) {
    return; //  No security Level attribute ? Don't proceed
  }
  emberAfGreenPowerClusterPrintln("");
  emberAfGreenPowerClusterPrint("GPS in ");
  // Find the security state of the node to examine the network security.
  EmberCurrentSecurityState securityState;
  // Distributed network - return with out any change to InvolveTc bit
  if (emberGetCurrentSecurityState(&securityState) == EMBER_SUCCESS
      && (securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE)) {
    emberAfGreenPowerClusterPrint("Distributed Network");
    // reset the InvolveTC bit
    gpsSecurityLevelAttribute &= ~(GREEN_POWER_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_FIELD_INVOLVE_TC);
  } else {
    // If centralised - checkif default TC link key is used
    const EmberKeyData linkKey = { GP_DEFAULT_LINK_KEY };
    EmberKeyStruct keyStruct;
    EmberStatus keyReadStatus = emberGetKey(EMBER_TRUST_CENTER_LINK_KEY, &keyStruct);
    if (keyReadStatus == EMBER_SUCCESS
        && MEMCOMPARE(keyStruct.key.contents, linkKey.contents, EMBER_ENCRYPTION_KEY_SIZE)) {
      emberAfGreenPowerClusterPrint("Centralised Network : Non Default TC Key used - Set InvoveTc bit");
      // Set the InvolveTC bit
      gpsSecurityLevelAttribute |= GREEN_POWER_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_FIELD_INVOLVE_TC;
    }
  }
  emberAfGreenPowerClusterPrintln("");
  secLevelStatus = emberAfWriteAttribute(GP_ENDPOINT,
                                         ZCL_GREEN_POWER_CLUSTER_ID,
                                         ZCL_GP_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_ID,
                                         (CLUSTER_MASK_SERVER),
                                         (uint8_t*)&gpsSecurityLevelAttribute,
                                         type);
  emberAfGreenPowerClusterPrintln("Security Level writen = %d, Status = %d",
                                  gpsSecurityLevelAttribute,
                                  secLevelStatus);
}

// Update the GPS Node state in non volatile token
static void clearGpsStateInToken(void)
{
  #if (EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS == 1) && !defined(EZSP_HOST)
  GpsNetworkState gpsNodeState = GREEN_POWER_SERVER_GPS_NODE_STATE_NOT_IN_NETWORK;
  halCommonSetToken(TOKEN_GPS_NETWORK_STATE, &gpsNodeState);
  #endif
}

// Checks if the node is just joined checking from the tokens
static bool updateInvolveTCNeeded(void)
{
#if (EMBER_AF_PLUGIN_GREEN_POWER_SERVER_USE_TOKENS == 1) && !defined(EZSP_HOST)
  GpsNetworkState gpsNodeState;
  halCommonGetToken(&gpsNodeState, TOKEN_GPS_NETWORK_STATE);
  if (gpsNodeState == GREEN_POWER_SERVER_GPS_NODE_STATE_IN_NETWORK) {
    return false;
  } else {
    gpsNodeState = GREEN_POWER_SERVER_GPS_NODE_STATE_IN_NETWORK;
    halCommonSetToken(TOKEN_GPS_NETWORK_STATE, &gpsNodeState);
  }
  return true;
#else
  return false;
#endif
}

// Process the update based on statck state
static void updateInvolveTC(EmberStatus status)
{
  // Pre process network status to decide if a InvolveTC is needed
  if (status == EMBER_NETWORK_DOWN
      && emberStackIsPerformingRejoin() == FALSE) {
    clearGpsStateInToken();
    return;
  } else if (status == EMBER_NETWORK_UP) {
    if (!updateInvolveTCNeeded()) {
      return;
    }
    writeInvolveTCBit();
  }
}

// Send device announcement
static void sendDeviceAnncement(uint16_t nodeId)
{
  EmberApsFrame apsFrameDevAnnce;
  apsFrameDevAnnce.sourceEndpoint = EMBER_ZDO_ENDPOINT;
  apsFrameDevAnnce.destinationEndpoint = EMBER_ZDO_ENDPOINT;
  apsFrameDevAnnce.clusterId = END_DEVICE_ANNOUNCE;
  apsFrameDevAnnce.profileId = EMBER_ZDO_PROFILE_ID;
  apsFrameDevAnnce.options = EMBER_APS_OPTION_SOURCE_EUI64;
  apsFrameDevAnnce.options |= EMBER_APS_OPTION_USE_ALIAS_SEQUENCE_NUMBER;
  apsFrameDevAnnce.groupId = 0;
  uint8_t messageContents[GP_DEVICE_ANNOUNCE_SIZE];
  uint8_t apsSequence = 0;
  // Form the APS message for Bcast
  messageContents[0] = apsSequence; //Sequence
  messageContents[1] = (uint8_t)nodeId; //NodeId
  messageContents[2] = (uint8_t)(nodeId >> 8); //NodeId
  MEMSET(&messageContents[3], 0xFF, 8); //IEEE Address
  messageContents[11] = 0; // Capability
  uint8_t length = GP_DEVICE_ANNOUNCE_SIZE;
#ifndef EZSP_HOST
  EmberMessageBuffer message = emberFillLinkedBuffers(messageContents, length);
  if (message == EMBER_NULL_MESSAGE_BUFFER) {
    return;
  }
  emberProxyBroadcast(nodeId, //EmberNodeId source,
                      0xFFFD, //EmberNodeId destination,
                      0, //uint8_t nwkSequence,
                      &apsFrameDevAnnce, //EmberApsFrame *apsFrame,
                      0xFF,     // use maximum radius
                      message);
  emberReleaseMessageBuffer(message);
#else
  ezspProxyBroadcast(nodeId,//EmberNodeId source,
                     0xFFFD,//EmberNodeId destination,
                     0,//uint8_t nwkSequence,
                     &apsFrameDevAnnce,//EmberApsFrame *apsFrame,
                     0xFF,//uint8_t radius,
                     0xFF,// Tag Id
                     length,//uint8_t messageLength,
                     messageContents,//uint8_t *messageContents,
                     &apsSequence);
#endif
}

// Internal functions used to maintain the group table within the context
// of the binding table.
//
// In the binding:
// The first two bytes of the identifier is set to the groupId
// The local endpoint is set to the endpoint that is mapped to this group
static uint8_t findGroupInBindingTable(uint8_t endpoint, uint16_t groupId)
{
  for (uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
    EmberBindingTableEntry binding;
    if (emberGetBinding(i, &binding) == EMBER_SUCCESS
        && binding.type == EMBER_MULTICAST_BINDING
        && binding.identifier[0] == LOW_BYTE(groupId)
        && binding.identifier[1] == HIGH_BYTE(groupId)
        && binding.local == endpoint) {
      return i;
    }
  }
  return 0xFF;
}

static EmberAfStatus removeFromApsGroup(uint8_t endpoint, uint16_t groupId)
{
  uint8_t index = findGroupInBindingTable(endpoint, groupId);
  if (index == 0xFF) {
    return EMBER_ZCL_STATUS_NOT_FOUND;
  }
  // search returned a valid index, so delete that
  EmberStatus status = emberDeleteBinding(index);
  return ((status == EMBER_SUCCESS) ? EMBER_ZCL_STATUS_SUCCESS : EMBER_ZCL_STATUS_FAILURE);
}

// Finds and returns the Gp Controlable application endpoint in the APS group
static uint16_t findAppEndpointGroupId(uint8_t endpoint)
{
  for (uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
    EmberBindingTableEntry binding;
    if (emberGetBinding(i, &binding) == EMBER_SUCCESS
        && binding.type == EMBER_MULTICAST_BINDING
        && binding.local == endpoint) {
      uint16_t groupId = (binding.identifier[1] << 8) | binding.identifier[0];
      return groupId;
    }
  }
  return 0;
}

static bool isValidAppEndpoint(uint8_t endpoint)
{
  if (endpoint == GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS
      || endpoint == GREEN_POWER_SERVER_SINK_DERIVES_ENDPOINTS) {
    return true;
  }
  for (uint8_t i = 0; i < FIXED_ENDPOINT_COUNT; i++) {
    if (emAfEndpoints[i].endpoint == endpoint) {
      return true;
    }
  }
  return false;
}

static bool isCommissioningAppEndpoint(uint8_t endpoint)
{
  // Gp Controlable end points are all valid enabled endpoints
  if (endpoint >= GREEN_POWER_SERVER_MIN_VALID_APP_ENDPOINT
      && endpoint <= GREEN_POWER_SERVER_MAX_VALID_APP_ENDPOINT
      && (endpoint == commissioningState.endpoint
          || commissioningState.endpoint == GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS      // All Endpoints - Sink derives
          || commissioningState.endpoint == GREEN_POWER_SERVER_SINK_DERIVES_ENDPOINTS  // Sink Derives the end points
          || commissioningState.endpoint == GREEN_POWER_SERVER_RESERVED_ENDPOINTS      // No Endpoint specified
          || commissioningState.endpoint == GREEN_POWER_SERVER_NO_PAIRED_ENDPOINTS)) { // No endpoint specified
    return true;
  }
  emberAfGreenPowerClusterPrintln("Endpoint %d Not in Commissioning Endpoint", endpoint);
  return false;
}

static uint8_t getGpCommissioningEndpoint(uint8_t *endpoints)
{
  uint8_t numberOfAppEndpoints = 0;
  if (commissioningState.endpoint == GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS         // All Endpoints - Sink derives
      || commissioningState.endpoint == GREEN_POWER_SERVER_SINK_DERIVES_ENDPOINTS  // Sink Derives the end points
      || commissioningState.endpoint == GREEN_POWER_SERVER_RESERVED_ENDPOINTS      // No Endpoint specified
      || commissioningState.endpoint == GREEN_POWER_SERVER_NO_PAIRED_ENDPOINTS) {  // No endpoint specified
    for (uint8_t i = 0; i < FIXED_ENDPOINT_COUNT; i++) {
      // Check only valid application endpoint ranges does not include GP EP
      if (emAfEndpoints[i].endpoint >= GREEN_POWER_SERVER_MIN_VALID_APP_ENDPOINT
          && emAfEndpoints[i].endpoint <= GREEN_POWER_SERVER_MAX_VALID_APP_ENDPOINT) {
        endpoints[numberOfAppEndpoints++] = emAfEndpoints[i].endpoint;
      }
    }
  } else {
    // else just one endpoint in commissioning
    endpoints[0] = commissioningState.endpoint;
    numberOfAppEndpoints = 1;
  }
  return numberOfAppEndpoints;
}

static uint8_t getAllSinkEndpoints(uint8_t *endpoints)
{
  return getGpCommissioningEndpoint(endpoints);
}

static void getGroupListBasedonAppEp(uint8_t *list)
{
  uint8_t *count = list;
  uint8_t *grouplist = list + 1;
  *count = 0;
  for (uint8_t i = 0; i < FIXED_ENDPOINT_COUNT; i++) {
    if (isCommissioningAppEndpoint(emAfEndpoints[i].endpoint)) {
      uint16_t groupId = findAppEndpointGroupId(emAfEndpoints[i].endpoint);
      if (0 != groupId) {
        (*count)++;
        *grouplist = (uint8_t)groupId;
        grouplist++;
        *grouplist = (uint8_t)(groupId >> 8);
        grouplist++;
        grouplist++; // alias bytes
        grouplist++; // allias
      }
    }
  }
}

static void resetOfMultisensorDataSaved(bool completeReset,
                                        GpCommDataSaved *gpd)
{
  // The reset could be complete or partial - back up the switch information
  EmberGpSwitchInformation tempSwitchInformationStruct = gpd->switchInformationStruct;
  if (completeReset) {
    // reset all data belonging to this Gpd
    MEMSET(gpd, 0, sizeof(GpCommDataSaved));
    // if the switch timer was active, just copy it back
    if (slxu_zigbee_event_is_active(genericSwitchCommissioningTimeout)) {
      gpd->switchInformationStruct = tempSwitchInformationStruct;
    }
    return;
  }
  // minimal reset (partial part):
  MEMSET(gpd->reportsStorage, 0, SIZE_OF_REPORT_STORAGE);
  gpd->lastIndex = 0;
  gpd->numberOfReports = 0;
  gpd->totalNbOfReport = 0;
}

static bool sinkFunctionalitySupported(uint32_t mask)
{
  uint32_t gpsFuntionnalityAttribut = 0;
  uint32_t gpsActiveFunctionnalityAttribut = 0;
  EmberAfAttributeType type;
  emberAfReadAttribute(GP_ENDPOINT,
                       ZCL_GREEN_POWER_CLUSTER_ID,
                       ZCL_GP_SERVER_GPS_FUNCTIONALITY_ATTRIBUTE_ID,
                       (CLUSTER_MASK_SERVER),
                       (uint8_t*)&gpsFuntionnalityAttribut,
                       sizeof(uint32_t),
                       &type);
  emberAfReadAttribute(GP_ENDPOINT,
                       ZCL_GREEN_POWER_CLUSTER_ID,
                       ZCL_GP_SERVER_GPS_ACTIVE_FUNCTIONALITY_ATTRIBUTE_ID,
                       (CLUSTER_MASK_SERVER),
                       (uint8_t*)&gpsActiveFunctionnalityAttribut,
                       sizeof(uint32_t),
                       &type);
  if (mask & (gpsFuntionnalityAttribut & gpsActiveFunctionnalityAttribut)) {
    return true;
  }
  return false;
}

static EmberStatus sendGpPairingMessage(EmberOutgoingMessageType type,
                                        uint16_t indexOrDestination,
                                        uint32_t options,
                                        EmberGpAddress *gpdAddr,
                                        uint16_t sinkGroupId,
                                        uint8_t deviceId,
                                        uint32_t gpdSecurityFrameCounter,
                                        uint8_t *gpdKey,
                                        uint16_t assignedAlias,
                                        uint8_t groupcastRadius)
{
  EmberEUI64 ourEUI;
  emberAfGetEui64(ourEUI);
  emberAfFillCommandGreenPowerClusterGpPairingSmart(options,
                                                    gpdAddr->id.sourceId,
                                                    gpdAddr->id.gpdIeeeAddress,
                                                    gpdAddr->endpoint,
                                                    ourEUI,
                                                    emberGetNodeId(),
                                                    sinkGroupId,
                                                    deviceId,
                                                    gpdSecurityFrameCounter,
                                                    gpdKey,
                                                    assignedAlias,
                                                    groupcastRadius);
  EmberApsFrame *apsFrame;
  apsFrame = emberAfGetCommandApsFrame();
  apsFrame->sourceEndpoint = GP_ENDPOINT;
  apsFrame->destinationEndpoint = GP_ENDPOINT;

  EmberStatus status = EMBER_ERR_FATAL;
  if (type == EMBER_OUTGOING_BROADCAST) {
    status = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
  } else if (type == EMBER_OUTGOING_DIRECT) {
    status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, indexOrDestination);
  }

  #if (defined SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT && !defined EZSP_HOST)
  if (type == EMBER_OUTGOING_DIRECT
      && indexOrDestination != emberGetNodeId()) {
    // Not a BCAST or Not addressed to this proxy
    return status;
  }
  emberGpProxyTableProcessGpPairing(options,
                                    gpdAddr,
                                    ((options & EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE) \
                                     >> EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE_OFFSET),
                                    emberGetNodeId(),
                                    sinkGroupId,
                                    assignedAlias,
                                    ourEUI,
                                    (EmberKeyData *) gpdKey,
                                    gpdSecurityFrameCounter,
                                    groupcastRadius);

  #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT && !ESZP_HOST
  return status;
}

// decommission a gpd
static void decommissionGpd(uint8_t secLvl,
                            uint8_t keyType,
                            EmberGpAddress *gpdAddr,
                            bool setRemoveGpdflag)
{
  uint8_t sinkEntryIndex = emberGpSinkTableLookup(gpdAddr);
  if (sinkEntryIndex != 0xFF) {
    EmberGpSinkTableEntry entry = { 0 };
    if (emberGpSinkTableGetEntry(sinkEntryIndex, &entry) != EMBER_SUCCESS) {
      return;
    }
    // Delet the entry in TT
    #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
    // to delete custom Translation table entry for this GPD
    emGpTransTableDeletePairedDevicefromTranslationTableEntry(gpdAddr);
    #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT

    uint8_t gpsCommunicationMode = (entry.options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE)
                                   >> EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE_OFFSET;
    uint16_t dGroupId = 0xFFFF;
    uint32_t pairingOptions = (setRemoveGpdflag ? EMBER_AF_GP_PAIRING_OPTION_REMOVE_GPD : 0)
                              | (gpdAddr->applicationId & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID)
                              | (gpsCommunicationMode << EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE_OFFSET)
                              | (secLvl << EMBER_AF_GP_PAIRING_OPTION_SECURITY_LEVEL_OFFSET)
                              | (keyType << EMBER_AF_GP_PAIRING_OPTION_SECURITY_KEY_TYPE_OFFSET);

    if (gpsCommunicationMode == EMBER_GP_SINK_TYPE_D_GROUPCAST) {
      dGroupId = emGpdAlias(gpdAddr);
    }
    EmberStatus retval = sendGpPairingMessage(EMBER_OUTGOING_BROADCAST,
                                              EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS,
                                              pairingOptions,
                                              gpdAddr,
                                              dGroupId,
                                              0xFF,
                                              0xFFFFFFFF,
                                              NULL,
                                              0xFFFF,
                                              0xFF);
    emberAfGreenPowerClusterPrintln("Gp Pairing for Decommissing send returned %d", retval);

    // In case of Sink was supporting groupcast for the GPD, then send a Gp Pairing config.
    if (gpsCommunicationMode == EMBER_GP_SINK_TYPE_GROUPCAST
        && (sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_SINK_TABLE_BASED_GROUPCAST_FORWARDING)
            || sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_PRE_COMMISSIONED_GROUPCAST_COMMUNICATION))) {
      emberAfFillCommandGreenPowerClusterGpPairingConfigurationSmart(EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REMOVE_GPD,
                                                                     0,
                                                                     gpdAddr->id.sourceId,
                                                                     gpdAddr->id.gpdIeeeAddress,
                                                                     gpdAddr->endpoint,
                                                                     entry.deviceId,
                                                                     0,
                                                                     NULL,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     NULL,
                                                                     0xFE,
                                                                     NULL,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     NULL,
                                                                     0,
                                                                     NULL,
                                                                     NULL,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     NULL);
      EmberApsFrame *apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      uint8_t retval = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
      emberAfGreenPowerClusterPrintln("Gp Pairing Config for Decommissing send returned %d", retval);
    }
    removeFromApsGroup(GP_ENDPOINT, dGroupId);
    if (emberAfPluginGreenPowerServerSinkTableAccessNotificationCallback(gpdAddr,
                                                                         GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_REMOVE_GPD)) {
      emberGpSinkTableRemoveEntry(sinkEntryIndex);
    }
  }
}

static void gppGpdLinkUnpack(uint8_t gppLink,
                             int8_t *rssi,
                             uint8_t *linkQuality)
{
  *rssi = (gppLink & EMBER_AF_GP_GPP_GPD_LINK_RSSI);
  // since gp 1.0b-14-
  // "The RSSI sub-field of the GPP-GPD link field encodes the RSSI from the range <+8 ; -109> [dBm],
  // with 2dBm granularity.", then
  // 0 represent -109dbm
  // 1 represent -107dbm
  // 2 represent -105dbm
  // ...
  // 54 represent -1dbm
  // 55 represent  0dbm // "110 is add to capped RSSI value, to obtain a non-negative value" gp1.0b 14-
  // 56 represent  2dbm
  // 57 represent  4dbm
  // 58 represent  6dbm
  // 59 represent  8dbm
  // 60 is value not used
  if ( *rssi < 55 ) {
    *rssi = -109 + 2 * (*rssi);
  } else {
    *rssi = ((*rssi) - 55) * 2;
  }
  *linkQuality = (gppLink & EMBER_AF_GP_GPP_GPD_LINK_LINK_QUALITY)
                 >> EMBER_AF_GP_GPP_GPD_LINK_LINK_QUALITY_OFFSET;
}

static void sendGpPairingConfigBasedOnSinkCommunicationMode(GpCommDataSaved *commissioningGpd)
{
  emberAfGreenPowerClusterPrintln("Communication mode to send GpPairingConfig %d", commissioningGpd->communicationMode);
  if (commissioningGpd->communicationMode != EMBER_GP_SINK_TYPE_GROUPCAST) {
    return;
  }
  // Send the Gp Configuration if the sink supports
  // - sink table-based forwarding
  if (sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_SINK_TABLE_BASED_GROUPCAST_FORWARDING)
      || sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_PRE_COMMISSIONED_GROUPCAST_COMMUNICATION) ) {
    // Send GP Pairing Config with
    // EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE
    // | EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SEQUENCE_NUMBER_CAPABILITIES
    // | (communication mode = GroupCast Forwarding (2))
    uint16_t pairigConfigOptions = GP_PAIRING_CONFIGURATION_FIXED_FLAG;
    pairigConfigOptions |= ((commissioningGpd->applicationInfo.applInfoBitmap)
                            ? EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_APPLICATION_INFORMATION_PRESENT : 0);
    uint8_t groupList[] = { 1, 1, 0, 0, 0, 1, 0, 0, 0 };
    getGroupListBasedonAppEp(groupList);
    uint8_t securityOptions = commissioningGpd->securityLevel
                              | (commissioningGpd->securityKeyType << EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET);

    emberAfFillCommandGreenPowerClusterGpPairingConfigurationSmart(EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_EXTEND_SINK_TABLE_ENTRY,
                                                                   pairigConfigOptions,
                                                                   commissioningGpd->addr.id.sourceId,
                                                                   commissioningGpd->addr.id.gpdIeeeAddress,
                                                                   commissioningGpd->addr.endpoint,
                                                                   commissioningGpd->applicationInfo.deviceId,
                                                                   ((4 * groupList[0]) + 1),
                                                                   groupList,
                                                                   0,
                                                                   0,
                                                                   securityOptions,
                                                                   commissioningGpd->outgoingFrameCounter,
                                                                   commissioningGpd->key.contents,
                                                                   0xFE,
                                                                   NULL,
                                                                   commissioningGpd->applicationInfo.applInfoBitmap,
                                                                   0,
                                                                   0,
                                                                   0,
                                                                   NULL,
                                                                   0,
                                                                   NULL,
                                                                   NULL,
                                                                   commissioningGpd->switchInformationStruct.switchInfoLength,
                                                                   (commissioningGpd->switchInformationStruct.nbOfContacts  \
                                                                    + (commissioningGpd->switchInformationStruct.switchType \
                                                                       << EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE_OFFSET)),
                                                                   commissioningGpd->switchInformationStruct.currentContact,
                                                                   0,
                                                                   0,
                                                                   NULL);
    EmberApsFrame *apsFrame;
    apsFrame = emberAfGetCommandApsFrame();
    apsFrame->sourceEndpoint = GP_ENDPOINT;
    apsFrame->destinationEndpoint = GP_ENDPOINT;

    uint8_t retval = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
    emberAfGreenPowerClusterPrintln("Gp Pairing Config Extend Sink send returned %d", retval);
    if (commissioningGpd->applicationInfo.applInfoBitmap
        & EMBER_AF_GP_APPLICATION_INFORMATION_APPLICATION_DESCRIPTION_PRESENT) {
      emberAfFillCommandGreenPowerClusterGpPairingConfigurationSmart(EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_APPLICATION_DESCRIPTION,
                                                                     0,
                                                                     commissioningGpd->addr.id.sourceId,
                                                                     commissioningGpd->addr.id.gpdIeeeAddress,
                                                                     commissioningGpd->addr.endpoint,
                                                                     commissioningGpd->applicationInfo.deviceId,
                                                                     0,
                                                                     NULL,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     NULL,
                                                                     0xFE,
                                                                     NULL,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     NULL,
                                                                     0,
                                                                     NULL,
                                                                     NULL,
                                                                     0,
                                                                     0,
                                                                     0,
                                                                     commissioningGpd->totalNbOfReport,
                                                                     commissioningGpd->numberOfReports,
                                                                     commissioningGpd->reportsStorage);
      apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      retval = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
      emberAfGreenPowerClusterPrintln("Gp Pairing Config App Description send returned %d", retval);
    }
  }
}

static uint8_t reportLength(const uint8_t *inReport)
{
  uint8_t length = 0; // 0 - position of reportId
  length += 1;  // 1 - position of reportOption
  if (inReport[length]
      & EMBER_AF_GP_GPD_APPLICATION_DESCRIPTION_COMMAND_REPORT_OPTIONS_TIMEOUT_PERIOD_PRESENT) {
    length += 3; // 2,3 - position of timeout and 4 position of length
  } else {
    length += 1; // 2 position of length excluding itself
  }
  length += inReport[length]; // sum up length of report descriptor remaining length
  length += 1; // add the length byte itself
  return length;
}

static bool saveReportDescriptor(uint8_t totalNumberOfReports,
                                 uint8_t numberOfReports,
                                 uint8_t *reports,
                                 GpCommDataSaved *commissioningGpd)
{
  if ((totalNumberOfReports == 0)
      || ((commissioningGpd->numberOfReports == totalNumberOfReports)
          && (commissioningGpd->numberOfReports == commissioningGpd->totalNbOfReport))) {
    return true; // the report was already collected
  }
  uint8_t *temp = reports;
  uint8_t length = 0; // 0 - position of reportId
  for (uint8_t noOfReport = 0; noOfReport < numberOfReports; noOfReport++) {
    length = reportLength(temp);
    // Negative test - drop the packet if the record is of bad length - 4.4.1.12
    // Minimum length for 1 report that has 1 option record in that 1 attribute record =
    // reportId(1) + option(1) + length(1) of option records
    // Option Record = option (1) + clusterid (2)
    // Attribute record = attrId (2) + datatype (1) + att option(1)
    if (length < GREEN_POWER_SERVER_MIN_REPORT_LENGTH ) {
      // Error, so return without saving
      return false;
    }
    // The report is not already collected
    if (NULL ==  emGpFindReportId(*temp,
                                  commissioningGpd->numberOfReports,
                                  commissioningGpd->reportsStorage)) {
      // now save the entire payload, appending to last received reports if any
      MEMCOPY(&(commissioningGpd->reportsStorage[commissioningGpd->lastIndex]),
              temp,
              length);
      commissioningGpd->numberOfReports++;
      commissioningGpd->lastIndex += length;
      commissioningGpd->totalNbOfReport = totalNumberOfReports;
    } else {
      emberAfGreenPowerClusterPrintln("Duplicate ReportId %d - Not Saved", *temp);
    }
    temp = temp + length;
  }
  return true;
}

static const uint8_t *findDataPointDescriptorInReport(uint8_t dataPointDescriptorId,
                                                      const uint8_t *report)
{
  uint8_t remainingLength;
  const uint8_t *finger;
  if (report[1]) {
    remainingLength = report[4]; // with timeout
    finger = &report[5];
  } else {
    remainingLength = report[2]; // without timeout
    finger = &report[3];
  }
  uint8_t tempDatapointDescrId = 0;
  for (int index = 0; index < remainingLength - 1; index++) {
    // travel through each data point descriptor from here until the input is
    // found
    if (tempDatapointDescrId == dataPointDescriptorId) {
      return &finger[index];
    }
    uint8_t datapointOptions = finger[index];
    index += 3 + ((datapointOptions & 0x10) ? 2 : 0); //for ManufactureId
    // traverse the attribute records in each data point
    for (int i = 0; i < ((datapointOptions & 0x07) + 1); i++) {
      index += 3;
      uint8_t attributeOptions = finger[index];
      index += ((attributeOptions & 0x0f) + 1);
    }
    tempDatapointDescrId++;
  }
  return NULL;
}

//function return true if the param_in communication mode is supported by the sink
static bool emGpCheckCommunicationModeSupport(uint8_t communicationModeToCheck)
{
  uint32_t gpsFuntionnalityAttribut = 0;
  uint32_t gpsActiveFunctionnalityAttribut = 0;
  EmberAfAttributeType type;
  emberAfReadAttribute(GP_ENDPOINT,
                       ZCL_GREEN_POWER_CLUSTER_ID,
                       ZCL_GP_SERVER_GPS_FUNCTIONALITY_ATTRIBUTE_ID,
                       CLUSTER_MASK_SERVER,
                       (uint8_t*)&gpsFuntionnalityAttribut,
                       sizeof(uint32_t),
                       &type);
  emberAfReadAttribute(GP_ENDPOINT,
                       ZCL_GREEN_POWER_CLUSTER_ID,
                       ZCL_GP_SERVER_GPS_ACTIVE_FUNCTIONALITY_ATTRIBUTE_ID,
                       (CLUSTER_MASK_SERVER),
                       (uint8_t*)&gpsActiveFunctionnalityAttribut,
                       sizeof(uint32_t),
                       &type);
  uint32_t currentFunctionnalities = (gpsFuntionnalityAttribut & gpsActiveFunctionnalityAttribut);
  if ((communicationModeToCheck == EMBER_GP_SINK_TYPE_FULL_UNICAST
       && (currentFunctionnalities & EMBER_AF_GP_GPS_FUNCTIONALITY_FULL_UNICAST_COMMUNICATION))
      || (communicationModeToCheck == EMBER_GP_SINK_TYPE_D_GROUPCAST
          && (currentFunctionnalities & EMBER_AF_GP_GPS_FUNCTIONALITY_DERIVED_GROUPCAST_COMMUNICATION))
      || (communicationModeToCheck == EMBER_GP_SINK_TYPE_GROUPCAST
          && (currentFunctionnalities & EMBER_AF_GP_GPS_FUNCTIONALITY_PRE_COMMISSIONED_GROUPCAST_COMMUNICATION))
      || (communicationModeToCheck == EMBER_GP_SINK_TYPE_LW_UNICAST
          && (currentFunctionnalities & EMBER_AF_GP_GPS_FUNCTIONALITY_LIGHTWEIGHT_UNICAST_COMMUNICATION))) {
    return true;
  }
  return false;
}

static void sendGpPairingFromSinkEntry(EmberGpSinkTableEntry *sinkEntry,
                                       uint16_t gppShortAddress)
{
  uint8_t appId = (sinkEntry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_APPLICATION_ID);
  uint8_t sinkCommunicationMode = (sinkEntry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE)
                                  >> EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE_OFFSET;
  bool gpdFixed = (sinkEntry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_FIXED_LOCATION)
                  ? true : false;
  bool gpdMacCapabilities = (sinkEntry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SEQUENCE_NUM_CAPABILITIES)
                            ? true : false;
  bool sinkTableAssignedAliasNeeded = (sinkEntry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_ASSIGNED_ALIAS)
                                      ? true : false;
  uint8_t securityLevel = (sinkEntry->securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);
  uint8_t securityKeyType = ((sinkEntry->securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE)
                             >> EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET);
  uint32_t pairingOptions = 0;
  pairingOptions = EMBER_AF_GP_PAIRING_OPTION_ADD_SINK
                   | (appId & EMBER_AF_GP_PAIRING_OPTION_APPLICATION_ID)
                   | (sinkCommunicationMode << EMBER_AF_GP_PAIRING_OPTION_COMMUNICATION_MODE_OFFSET)
                   | (gpdFixed << EMBER_AF_GP_PAIRING_OPTION_GPD_FIXED_OFFSET)
                   | (gpdMacCapabilities << EMBER_AF_GP_PAIRING_OPTION_GPD_MAC_SEQUENCE_NUMBER_CAPABILITIES_OFFSET)
                   | (securityLevel << EMBER_AF_GP_PAIRING_OPTION_SECURITY_LEVEL_OFFSET)
                   | (securityKeyType << EMBER_AF_GP_PAIRING_OPTION_SECURITY_KEY_TYPE_OFFSET)
                   | ((gpdMacCapabilities || securityLevel) ? EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_FRAME_COUNTER_PRESENT : 0)
                   | ((securityLevel && securityKeyType) ? EMBER_AF_GP_PAIRING_OPTION_GPD_SECURITY_KEY_PRESENT : 0)
                   | (sinkTableAssignedAliasNeeded << EMBER_AF_GP_PAIRING_OPTION_ASSIGNED_ALIAS_PRESENT_OFFSET)
                   | ((sinkCommunicationMode == EMBER_GP_SINK_TYPE_D_GROUPCAST
                       || sinkCommunicationMode == EMBER_GP_SINK_TYPE_GROUPCAST)
                      ? EMBER_AF_GP_PAIRING_OPTION_GROUPCAST_RADIUS_PRESENT : 0);
  uint16_t groupId = 0xFFFF;
  EmberOutgoingMessageType type = EMBER_OUTGOING_BROADCAST;
  uint16_t indexOrDestination = EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS;
  if (sinkCommunicationMode == EMBER_GP_SINK_TYPE_D_GROUPCAST) {
    groupId = emGpdAlias(&(sinkEntry->gpd));
  } else if (sinkCommunicationMode == EMBER_GP_SINK_TYPE_GROUPCAST) {
    groupId = sinkEntry->sinkList[0].target.groupcast.groupID;
  } else if (sinkCommunicationMode == EMBER_GP_SINK_TYPE_LW_UNICAST
             && commissioningState.sendGpPairingInUnicastMode) {
    type = EMBER_OUTGOING_DIRECT;
    indexOrDestination = gppShortAddress;
  }
  EmberStatus retval = sendGpPairingMessage(type,
                                            indexOrDestination,
                                            pairingOptions,
                                            &(sinkEntry->gpd),
                                            groupId,
                                            sinkEntry->deviceId,
                                            sinkEntry->gpdSecurityFrameCounter,
                                            sinkEntry->gpdKey.contents,
                                            sinkEntry->assignedAlias,
                                            sinkEntry->groupcastRadius);
  emberAfGreenPowerClusterPrintln("pairing send returned %d", retval);
}

static void sendGpPairingLookingUpAddressInSinkEntry(EmberGpAddress *gpdAddr)
{
  if (gpdAddr == NULL) {
    return;
  }
  uint8_t sinkEntryIndex = emberGpSinkTableLookup(gpdAddr);
  if (sinkEntryIndex == 0xFF) {
    return;
  }
  // Start updating sink table
  EmberGpSinkTableEntry sinkEntry = { 0 };
  if (emberGpSinkTableGetEntry(sinkEntryIndex, &sinkEntry) != EMBER_SUCCESS) {
    return;
  }
  sendGpPairingFromSinkEntry(&sinkEntry,
                             EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
}

// Builds a list of all the gpd commands from incoming commissioning req
static uint8_t formGpdCommandListFromIncommingCommReq(uint8_t gpdDeviceId,
                                                      uint8_t *inGpdCommandList,
                                                      uint8_t inCommandListLength,
                                                      uint8_t *gpdCommandList)
{
  uint8_t gpdCommandListLength = 0;
  // if there is already a command list present in incoming payload copy that
  // Realign A0-A3 as AF if TT compacting is enabled and filter out duplicate AF
  bool foundAlready = false;
  for (int i = 0; i < inCommandListLength; i++) {
    if (inGpdCommandList[i] == EMBER_ZCL_GP_GPDF_ATTRIBUTE_REPORTING
        || inGpdCommandList[i] == EMBER_ZCL_GP_GPDF_MULTI_CLUSTER_RPTG
        || inGpdCommandList[i] == EMBER_ZCL_GP_GPDF_MFR_SP_ATTR_RPTG
        || inGpdCommandList[i] == EMBER_ZCL_GP_GPDF_MFR_SP_MULTI_CLUSTER_RPTG) {
      if (!foundAlready) {
        foundAlready = true;
        gpdCommandList[gpdCommandListLength++] = EMBER_ZCL_GP_GPDF_ANY_GPD_SENSOR_CMD;
      }
    } else {
      gpdCommandList[gpdCommandListLength++] = inGpdCommandList[i];
    }
  }
  //Now extend the command list for the device id, take out the duplicate if any
  if (gpdDeviceId != 0xFE) {
    // look up the device Id derive the commands if there is no commands supplied
    if (gpdCommandListLength == 0) {
      gpdCommandListLength += emGetCommandListFromDeviceIdLookup(gpdDeviceId,
                                                                 gpdCommandList);
    }
  }
  return gpdCommandListLength;
}

// resolves and relates endpoint - cluster from the given the number of paired endpoints
// cluster list (present in application info and appl description)
static uint8_t buildSinkSupportedClusterEndPointListForGpdCommands(uint8_t numberOfPairedEndpoints,
                                                                   uint8_t *pairedEndpoints,
                                                                   uint8_t numberOfClusters,
                                                                   ZigbeeCluster *clusterList,
                                                                   SupportedGpdCommandClusterEndpointMap *gpdCommandClusterEpMap)
{
  //for (int i = 0; i < numberOfClusters; i++) {
  //  emberAfGreenPowerClusterPrint("clusterList[%d] = %x ", i, clusterList[i]);
  //}
  //emberAfGreenPowerClusterPrintln("");
  uint8_t tempEndpointsOnSink[MAX_ENDPOINT_COUNT] = { 0 };
  uint8_t tempSupportedEndpointCount = 0;
  if (numberOfPairedEndpoints == GREEN_POWER_SERVER_NO_PAIRED_ENDPOINTS
      || numberOfPairedEndpoints == GREEN_POWER_SERVER_RESERVED_ENDPOINTS) {
    // The target endpoint for validation is the Current commissioning Ep
    tempSupportedEndpointCount = getGpCommissioningEndpoint(tempEndpointsOnSink);
  } else if (numberOfPairedEndpoints == GREEN_POWER_SERVER_SINK_DERIVES_ENDPOINTS
             || numberOfPairedEndpoints == GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS) {
    tempSupportedEndpointCount = getAllSinkEndpoints(tempEndpointsOnSink);
  } else {
    for (int i = 0; i < MAX_ENDPOINT_COUNT; i++) {
      for (int j = 0; j < numberOfPairedEndpoints; j++) {
        emberAfGreenPowerClusterPrintln("Checking PairedEp[%d]=%x with SinkEp = %d ",
                                        j, pairedEndpoints[j], emAfEndpoints[i].endpoint);
        if (pairedEndpoints[j] == emAfEndpoints[i].endpoint) {
          tempEndpointsOnSink[tempSupportedEndpointCount] = emAfEndpoints[i].endpoint;
          tempSupportedEndpointCount++;
          // Found, so break inner loop
          // - this is ensure the temp endpoint list is sorted without duplicate
          break;
        }
      }
    }
  }
  // Here, the endpoints are filtered, those, only are supported out of input paired
  // endpoint list - run through all the clusters for each end point to filter
  // (endpoint, cluster) pair that is supported.
  uint8_t count = 0;
  for (int i = 0; i < tempSupportedEndpointCount; i++) {
    for (int j = 0; j < numberOfClusters; j++) {
      if (0xFFFF != clusterList[j].clusterId) {
        if (emGpEndpointAndClusterIdValidation(tempEndpointsOnSink[i],
                                               clusterList[j].serverClient,
                                               clusterList[j].clusterId)) {
          gpdCommandClusterEpMap->endpoints[count] = tempEndpointsOnSink[i];
          count++;
          break;
        }
      }
    }
  }
  noOfCommissionedEndpoints = 0;
  gpdCommandClusterEpMap->numberOfEndpoints = 0;
  if (count <= MAX_ENDPOINT_COUNT) {
    gpdCommandClusterEpMap->numberOfEndpoints = count;
    noOfCommissionedEndpoints = count;
    MEMCOPY(commissionedEndPoints, gpdCommandClusterEpMap->endpoints, count);
  }
  return count;
}

static uint8_t appendClusterSupportForGenericSwitch(ZigbeeCluster *clusterPtr,
                                                    EmberGpSwitchInformation *switchInformationStruct)
{
  clusterPtr[0].clusterId = ZCL_ON_OFF_CLUSTER_ID;
  clusterPtr[0].serverClient = 1;
  return 1;
}

static bool validateSwitchCommand(uint8_t *validatedCommands,
                                  uint8_t commandIndex,
                                  uint8_t commandId,
                                  EmberGpApplicationInfo *applicationInfo,
                                  SupportedGpdCommandClusterEndpointMap *gpdCommandClusterEpMap,
                                  GpCommDataSaved *commissioningGpd)
{
  uint8_t noOfClusters = 0;
  ZigbeeCluster cluster[30];
  // handle switch validation
  // from user get a set of clusters that generic switch wants to use
  if (commandId == EMBER_ZCL_GP_GPDF_8BITS_VECTOR_PRESS
      || commandId == EMBER_ZCL_GP_GPDF_8BITS_VECTOR_RELEASE) {
    gpdCommandClusterEpMap[commandIndex].gpdCommand = commandId;
    ZigbeeCluster *clusterPtr = &cluster[noOfClusters];
    noOfClusters += appendClusterSupportForGenericSwitch(clusterPtr,
                                                         &(commissioningGpd->switchInformationStruct));
    uint8_t endpointCount = 0;
    endpointCount = buildSinkSupportedClusterEndPointListForGpdCommands(applicationInfo->numberOfPairedEndpoints,
                                                                        applicationInfo->pairedEndpoints,
                                                                        noOfClusters,
                                                                        cluster,
                                                                        &gpdCommandClusterEpMap[commandIndex]);
    if (endpointCount) {
      *validatedCommands += 1;
    }
    return true;
  }
  return false;
}

static uint8_t appendClustersFromApplicationDescription(ZigbeeCluster *clusterPtr,
                                                        bool reverseDirection,
                                                        GpCommDataSaved *commissioningGpd)
{
  uint8_t count = 0;
  for (int index = 0; index < commissioningGpd->numberOfReports; index++) {
    const uint8_t *report = emGpFindReportId(index,
                                             commissioningGpd->numberOfReports,
                                             commissioningGpd->reportsStorage);
    if (report) {
      for (int j = 0;; j++) {
        const uint8_t *datapointOption = findDataPointDescriptorInReport(j,
                                                                         report);
        if (datapointOption) {
          clusterPtr[count].serverClient = (datapointOption[0] & 0x08) >> 3;
          if (reverseDirection) {
            clusterPtr[count].serverClient = !(clusterPtr[count].serverClient);
          }
          clusterPtr[count].clusterId = datapointOption[1] + (((uint16_t)datapointOption[2]) << 8);
          count++;
        } else {
          break; // break the inner for loop if datapointOption is null
        }
      }
    }
  }
  return count;
}

static bool validateCARCommand(uint8_t *validatedCommands,
                               uint8_t commandIndex,
                               uint8_t commandId,
                               EmberGpApplicationInfo *applicationInfo,
                               SupportedGpdCommandClusterEndpointMap *gpdCommandClusterEpMap,
                               GpCommDataSaved *commissioningGpd)
{
  uint8_t noOfClusters = 0;
  ZigbeeCluster cluster[30];
  // Handle Compact Attribute Reporting command validation, check the clusterts present in the
  // stored report descriptor and validate those cluster to see if sink has it on the eintended
  // endpoint on sink.
  if (commandId == EMBER_ZCL_GP_GPDF_COMPACT_ATTRIBUTE_REPORTING) {
    gpdCommandClusterEpMap[commandIndex].gpdCommand = commandId;
    ZigbeeCluster *clusterPtr = &cluster[noOfClusters];
    noOfClusters += appendClustersFromApplicationDescription(clusterPtr,
                                                             true,
                                                             commissioningGpd);
    uint8_t endpointCount = 0;
    endpointCount = buildSinkSupportedClusterEndPointListForGpdCommands(applicationInfo->numberOfPairedEndpoints,
                                                                        applicationInfo->pairedEndpoints,
                                                                        noOfClusters,
                                                                        cluster,
                                                                        &gpdCommandClusterEpMap[commandIndex]);
    if (endpointCount) {
      *validatedCommands += 1;
    }
    return true;
  }
  return false;
}

// final list of mapping of gpdCommand-Cluster-EndpointList that is supported
static uint8_t validateGpdCommandSupportOnSink(uint8_t *gpdCommandList,
                                               uint8_t gpdCommandListLength,
                                               EmberGpApplicationInfo *applicationInfo,
                                               SupportedGpdCommandClusterEndpointMap *gpdCommandClusterEpMap,
                                               GpCommDataSaved *commissioningGpd)
{
  uint8_t validatedCommands = 0;
  for (int commandIndex = 0; commandIndex < gpdCommandListLength; commandIndex++) {
    // First see if it is switch command - validate it else do rest of
    // the command validation
    if (validateSwitchCommand(&validatedCommands,
                              commandIndex,
                              gpdCommandList[commandIndex],
                              applicationInfo,
                              gpdCommandClusterEpMap,
                              commissioningGpd)) {
    } else if (validateCARCommand(&validatedCommands,
                                  commandIndex,
                                  gpdCommandList[commandIndex],
                                  applicationInfo,
                                  gpdCommandClusterEpMap,
                                  commissioningGpd)) {
    } else {
      // default sub translation table has the (gpdCommnd<->ClusterId) mapping,
      // use the look up to decide when the mapped clusterId = 0xFFFF, then it
      // need to use the cluster list from the incoming message payload.
      // pick up clusters to validate against the supplied pair of end points
      // and available endpoints in sink;
      gpdCommandClusterEpMap[commandIndex].gpdCommand = gpdCommandList[commandIndex];
      // Prepare a cluster list that will be input for next level
      uint8_t noOfClusters = 0;
      ZigbeeCluster cluster[30] = { 0 };
      if ((applicationInfo->numberOfGpdClientCluster != 0)
          || (applicationInfo->numberOfGpdServerCluster != 0)) {
        // Now a generic cluster - read, write, reporting etc
        // many clusters are in for the command
        // prepare a list of all the clusters in the incoming message
        // pick from appInfo
        for (int i = 0; i < applicationInfo->numberOfGpdClientCluster; i++) {
          cluster[noOfClusters].clusterId = applicationInfo->clientClusters[i];
          cluster[noOfClusters].serverClient = 1; // reverse for the match
          noOfClusters++;
        }
        for (int i = 0; i < applicationInfo->numberOfGpdServerCluster; i++) {
          cluster[noOfClusters].clusterId = applicationInfo->serverClusters[i];
          cluster[noOfClusters].serverClient = 0; // reverse for the match
          noOfClusters++;
        }
      } else {
        ZigbeeCluster gpdCluster;
        if (!emGetClusterListFromCmdIdLookup(gpdCommandList[commandIndex],
                                             &gpdCluster)) {
          continue;
        }

        // Apend the cluster found in the TT (default or customised) if available
        if (0xFFFF != gpdCluster.clusterId) {
          cluster[noOfClusters].clusterId = gpdCluster.clusterId;
          cluster[noOfClusters].serverClient = gpdCluster.serverClient;
          noOfClusters += 1;
        } else {
          // Now, get the list of clusters from the device Id if the cluster associated with this
          // command in the translation table is reserved.
          noOfClusters += emGetClusterListFromDeviceIdLookup(applicationInfo->deviceId,
                                                             &cluster[noOfClusters]);
        }
      }
      uint8_t endpointCount = 0;
      endpointCount = buildSinkSupportedClusterEndPointListForGpdCommands(applicationInfo->numberOfPairedEndpoints,
                                                                          applicationInfo->pairedEndpoints,
                                                                          noOfClusters,
                                                                          cluster,
                                                                          &gpdCommandClusterEpMap[commandIndex]);
      if (endpointCount) {
        validatedCommands += 1;
      }
    }
  }
  return validatedCommands;
}

// Checks if GP can support some or all of the commands and clusters
static uint8_t getMatchedSinkFunctionality(SupportedGpdCommandClusterEndpointMap *gpdCommandClusterEpMap,
                                           GpCommDataSaved *commissioningGpd)
{
  EmberGpApplicationInfo *applicationInfo = &(commissioningGpd->applicationInfo);
  // to hold the temporary list of gpd commands derived from deviceId and added
  // from the gpdCommandList from app info
  uint8_t gpdCommandList[50] = { 0 };
  // Build a list of all the commands - present and derived from GPD ID look up
  uint8_t gpdCommandListLength = formGpdCommandListFromIncommingCommReq(applicationInfo->deviceId,
                                                                        applicationInfo->gpdCommands,
                                                                        applicationInfo->numberOfGpdCommands,
                                                                        gpdCommandList);
  // Simulate the commands as per the spec from other information in absence of
  // command list and device Id
  if (gpdCommandListLength == 0
      && 0xFE == applicationInfo->deviceId) {
    uint8_t commandIndex = 0;
    if (applicationInfo->applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_APPLICATION_DESCRIPTION_PRESENT) {
      // this command is for a CAR
      gpdCommandList[commandIndex++] = EMBER_ZCL_GP_GPDF_COMPACT_ATTRIBUTE_REPORTING;
    }
    if (applicationInfo->applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_CLUSTER_LIST_PRESENT) {
      // Add the AF to compact the Translation table A0/A1
      gpdCommandList[commandIndex++] = EMBER_ZCL_GP_GPDF_ANY_GPD_SENSOR_CMD;
      // Add the Tunneling as an implicit to adding reporting A0/A1
      gpdCommandList[commandIndex++] = EMBER_ZCL_GP_GPDF_ZCL_TUNNELING_WITH_PAYLOAD;
    }
    gpdCommandListLength = commandIndex;
  }

  uint8_t validatedCommands = 0;
  validatedCommands = validateGpdCommandSupportOnSink(gpdCommandList,
                                                      gpdCommandListLength,
                                                      applicationInfo,
                                                      gpdCommandClusterEpMap,
                                                      commissioningGpd);
  return validatedCommands;
}

static bool handleGpdMatchingSinkFunctionality(GpCommDataSaved *commissioningGpd)
{
  // Let user to take decission first
  //1. User handles completely validates the matching functionality( plugin does not process anything) - this is the case of a Gateway Combo
  //     a. Found matching functionality, so that a commissioning state changes (like a commissioning reply queued)
  //     b. Not Found
  //2. Plugin completely handles the functionality matching - Most of the cases of an SoC Combo.
  //     In some cases - user would like to just manipulated the appInfo and lets the plugin to handle rest.

  bool matchFound = true;
  if (emberAfPluginGreenPowerServerGpdCommissioningCallback(&(commissioningGpd->applicationInfo), &matchFound)) {
    // return based on user handling
    return matchFound;
  }

  // Relation between GP deviceID and Translation table content creation
  // for example : GPD_DeviceID 0x02 (on/off)
  // means TT entries ZbProfile ZHA, ZbCluster 0x0006, ZbCmd ON+OFF+TOGGLE,
  // for Gps endpoint which match this cluster On/Off 0x0006
  uint8_t gpdMatch = 0;
  SupportedGpdCommandClusterEndpointMap gpdCommandClusterEpMap[50] = { 0 };
  gpdMatch = getMatchedSinkFunctionality(gpdCommandClusterEpMap,
                                         commissioningGpd);
  return (gpdMatch ? true : false);
}

static void sendCommissioningReply(GpCommDataSaved *commissioningGpd)
{
  uint8_t gpsSecurityKeyTypeAtrribute = 0;
  EmberKeyData gpsKeyAttribute = { { 0 } };
  EmberAfStatus status = emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(&gpsSecurityKeyTypeAtrribute,
                                                                                 &gpsKeyAttribute,
                                                                                 &(commissioningGpd->addr));

  uint8_t incomingGpdKeyType = (((commissioningGpd->gpdfExtendedOptions) & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE)
                                >> EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET);
  bool incomingGpdKeyEncryption = (((commissioningGpd->gpdfExtendedOptions) & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION)
                                   ? true : false);
  uint8_t sendSecurityKeyType = 0;
  bool sendKeyinReply = false;
  if (gpsSecurityKeyTypeAtrribute == 0
      && ((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_GP_SECURITY_KEY_REQUEST)) {
    // Use the proposed key, that has come in the commissioning gfdf and saved in commissioningGpd
    sendKeyinReply = false;
    sendSecurityKeyType = incomingGpdKeyType;
  }
  EmberKeyData sendKey = { { 0 } };
  bool sendKeyEncryption = false;
  if (status == EMBER_ZCL_STATUS_SUCCESS
      && ((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_GP_SECURITY_KEY_REQUEST)
      && gpsSecurityKeyTypeAtrribute != incomingGpdKeyType) {
    sendSecurityKeyType = gpsSecurityKeyTypeAtrribute;
    MEMCOPY(sendKey.contents, gpsKeyAttribute.contents, EMBER_ENCRYPTION_KEY_SIZE);
    sendKeyEncryption = incomingGpdKeyEncryption;
    sendKeyinReply = true;
    // Set up same key and key type for the Gp Pairing
    commissioningGpd->securityKeyType = sendSecurityKeyType;
    MEMCOPY(commissioningGpd->key.contents, sendKey.contents, EMBER_ENCRYPTION_KEY_SIZE);
  }
  // If the GPD proposes a OOB key, and the key request is `false`, it means the commissioning
  // is expected to use that OOB only. No need to send the key back.
  if (!((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_GP_SECURITY_KEY_REQUEST)) {
    sendSecurityKeyType = incomingGpdKeyType;
    sendKeyinReply = false;
  }
  if (incomingGpdKeyType == (gpsSecurityKeyTypeAtrribute & GPS_ATTRIBUTE_KEY_TYPE_MASK)
      && 0 == MEMCOMPARE(commissioningGpd->key.contents,
                         gpsKeyAttribute.contents,
                         EMBER_ENCRYPTION_KEY_SIZE)
      && ((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_GP_SECURITY_KEY_REQUEST)
      && gpsSecurityKeyTypeAtrribute != 0) {
    sendSecurityKeyType = gpsSecurityKeyTypeAtrribute;
    sendKeyinReply = false;
    sendKeyEncryption = false;
  }

  uint8_t commReplyOptions = 0;
  emberAfGreenPowerClusterPrintln("Added securityLevel %d", commissioningGpd->securityLevel);
  commReplyOptions |= (commissioningGpd->securityLevel << EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_SECURITY_LEVEL_OFFSET);
  if ((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_PAN_ID_REQUEST) {
    commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_PAN_ID_PRESENT;
    emberAfGreenPowerClusterPrintln("Added PanId");
  }
  emberAfGreenPowerClusterPrintln("Added Key Type = %d", sendSecurityKeyType);
  commReplyOptions |= (sendSecurityKeyType
                       << EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_KEY_TYPE_OFFSET);

  if (sendKeyinReply) {
    emberAfGreenPowerClusterPrintln("Added Key Present");
    commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPD_SECURITY_KEY_PRESENT;
    if (sendKeyEncryption) { // GPDF GPKeyEncryption Subfeild
      emberAfGreenPowerClusterPrintln("Added Key Encrypted");
      commReplyOptions |= EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPDKEY_ENCRYPTION;
    }
  }
  // Prepare the payload
  uint8_t commReplyPayload[COMM_REPLY_PAYLOAD_SIZE] = { 0 };
  commReplyPayload[0] = commReplyOptions;
  uint8_t index = 1;
  if (commReplyOptions & EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_PAN_ID_PRESENT) {
    emberStoreLowHighInt16u(commReplyPayload + index, emberAfGetPanId());
    index += 2;
  }

  if (commReplyOptions & EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPD_SECURITY_KEY_PRESENT) {
    if (commReplyOptions & EMBER_AF_GP_GPD_COMMISSIONING_REPLY_OPTIONS_GPDKEY_ENCRYPTION) {
      uint8_t mic[4] = { 0 };
      uint32_t fc = commissioningGpd->outgoingFrameCounter + 1;
      emberAfGreenPowerClusterPrintln("Key :");
      for (int i = 0; i < EMBER_ENCRYPTION_KEY_SIZE; i++) {
        emberAfGreenPowerClusterPrint("%x", sendKey.contents[i]);
      }
      emberAfGreenPowerClusterPrintln("");
      // Encrypt the Commissioning Reply (outgoing) Key
      const EmberKeyData linkKey[] = { GP_DEFAULT_LINK_KEY };
      emGpKeyTcLkDerivation(&(commissioningGpd->addr),
                            fc,
                            mic,
                            linkKey,
                            &sendKey,
                            false);
      MEMCOPY(commReplyPayload + index, sendKey.contents, EMBER_ENCRYPTION_KEY_SIZE);
      index += EMBER_ENCRYPTION_KEY_SIZE;
      MEMCOPY(commReplyPayload + index, mic, 4);
      index += 4;
      MEMCOPY(commReplyPayload + index, &fc, 4);
      index += 4;
    } else {
      MEMCOPY(commReplyPayload + index, sendKey.contents, EMBER_ENCRYPTION_KEY_SIZE);
      index += EMBER_ENCRYPTION_KEY_SIZE;
    }
  }
  emberAfGreenPowerClusterPrintln("GpResponse : Sending Commissioning Reply");
  emberAfFillCommandGreenPowerClusterGpResponseSmart(commissioningGpd->addr.applicationId,
                                                     commissioningGpd->gppShortAddress,
                                                     (emberAfGetRadioChannel() - 11),
                                                     commissioningGpd->addr.id.sourceId,
                                                     commissioningGpd->addr.id.gpdIeeeAddress,
                                                     commissioningGpd->addr.endpoint,
                                                     EMBER_ZCL_GP_GPDF_COMMISSIONING_REPLY,
                                                     index,
                                                     commReplyPayload);
  EmberApsFrame *apsFrame;
  apsFrame = emberAfGetCommandApsFrame();
  apsFrame->sourceEndpoint = GP_ENDPOINT;
  apsFrame->destinationEndpoint = GP_ENDPOINT;
  EmberStatus UNUSED retval;
  if (commissioningState.unicastCommunication) { //  based on the commission mode as decided by sink
    retval = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, commissioningGpd->gppShortAddress);
  } else {
    retval = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
  }
  emberAfGreenPowerClusterPrintln("GpResponse send returned %d", retval);
}

static bool handleAddingTranstionTableEntry(GpCommDataSaved *commissioningGpd)
{
  if (!sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_TRANSLATION_TABLE)) {
    return true;
  }
  uint8_t gpdMatch = 0;

  SupportedGpdCommandClusterEndpointMap gpdCommandClusterEpMap[50] = { 0 };
  gpdMatch = getMatchedSinkFunctionality(gpdCommandClusterEpMap,
                                         commissioningGpd);

  // In case the sink supports the gpd parameters, create an TT entry for each gpd command
  for (uint8_t cmdIndex = 0; cmdIndex < gpdMatch; cmdIndex++) {
    for (uint8_t i = 0; i < gpdCommandClusterEpMap[cmdIndex].numberOfEndpoints; i++) {
      emberAfGreenPowerClusterPrintln("gpdCommandClusterEpMap[%d].gpdCommand = %d gpdCommandClusterEpMap[%d].endpoints[%d] = %d",
                                      cmdIndex,
                                      gpdCommandClusterEpMap[cmdIndex].gpdCommand,
                                      cmdIndex,
                                      i,
                                      gpdCommandClusterEpMap[cmdIndex].endpoints[i]);
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      emGpPairingDoneThusSetCustomizedTranslationTable(&(commissioningGpd->addr),
                                                       gpdCommandClusterEpMap[cmdIndex].gpdCommand,
                                                       gpdCommandClusterEpMap[cmdIndex].endpoints[i]);
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
    }
  }
  return (gpdMatch ? true : false);
}

static void handleSinkEntryAndPairing(GpCommDataSaved *commissioningGpd)
{
  EmberGpAddress *gpdAddr = &(commissioningGpd->addr);
  bool gpdFixed = ((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_FIXED_LOCATION) \
                  ? true : false;
  bool gpdMacCapabilities = ((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_MAC_SEQ_NUM_CAP) \
                            ? true : false;

  EmberGpSinkType sinkCommunicationMode = commissioningGpd->communicationMode;
  // by default full unicast and derived groupcast communication mode
  // will use derived alias instead assigned alias,
  bool sinkTableAssignedAliasNeeded = commissioningGpd->useGivenAssignedAlias;

  uint32_t newSinkTableOptions = (gpdAddr->applicationId & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_APPLICATION_ID)
                                 | (sinkCommunicationMode << EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE_OFFSET)
                                 | (gpdMacCapabilities << EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SEQUENCE_NUM_CAPABILITIES_OFFSET)
                                 | ((((commissioningGpd->gpdfOptions) & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_RX_ON_CAP) >> EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_RX_ON_CAP_OFFSET) << EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_RX_ON_CAPABILITY_OFFSET)
                                 | (gpdFixed << EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_FIXED_LOCATION_OFFSET)
                                 | (sinkTableAssignedAliasNeeded << EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_ASSIGNED_ALIAS_OFFSET)
                                 | ((((commissioningGpd->gpdfOptions) & 0x80) \
                                     && (commissioningGpd->gpdfExtendedOptions)) ? EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SECURITY_USE : 0);
  // SecurityUse bit always set to 0b1, so securityOptions field always present
  // also to indicate level 0 or key NONE, cause it will be extremly rare that a GPD is SecLvl=0,
  uint8_t newSinkTableSecurityOptions = commissioningGpd->securityLevel
                                        | (commissioningGpd->securityKeyType << EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET);
  uint8_t sinkEntryIndex = emberGpSinkTableFindOrAllocateEntry(gpdAddr);
  // must have a valid sinkEntryIndex by now, else indicates issue with sink table such as sink table full.
  if (sinkEntryIndex == 0xFF) {
    return;
  }
  // Start updating sink table
  EmberGpSinkTableEntry sinkEntry;
  if (emberGpSinkTableGetEntry(sinkEntryIndex, &sinkEntry) != EMBER_SUCCESS) {
    return;
  }
  sinkEntry.options = newSinkTableOptions;
  sinkEntry.deviceId = commissioningGpd->applicationInfo.deviceId;
  sinkEntry.assignedAlias = emGpdAlias(gpdAddr);
  if (commissioningGpd->useGivenAssignedAlias) {
    sinkEntry.assignedAlias = commissioningGpd->givenAlias;
  }
  if (sinkEntry.groupcastRadius == 0xff
      || sinkEntry.groupcastRadius < commissioningGpd->groupcastRadius) {
    sinkEntry.groupcastRadius = commissioningGpd->groupcastRadius;
  }
  // Add the Group
  uint16_t groupId = 0xFFFF;
  if (sinkCommunicationMode == EMBER_GP_SINK_TYPE_D_GROUPCAST) {
    groupId = emGpdAlias(gpdAddr);
    EmberAfStatus status = emGpAddToApsGroup(GP_ENDPOINT, groupId);
    emberAfCorePrintln("Added to Group %d Status = %d", groupId, status);
  } else if (sinkCommunicationMode == EMBER_GP_SINK_TYPE_GROUPCAST) {
    uint8_t groupList[] = { 1, 1, 0, 0, 0, 1, 0, 0, 0 };
    getGroupListBasedonAppEp(groupList);
    for (uint8_t index = 0; index < groupList[0]; index++) {
      sinkEntry.sinkList[index].type = EMBER_GP_SINK_TYPE_GROUPCAST;
      sinkEntry.sinkList[index].target.groupcast.groupID = (groupList[2] << 8) + groupList[1];
      sinkEntry.sinkList[index].target.groupcast.alias = (groupList[4] << 8) + groupList[3];
    }
    groupId = sinkEntry.sinkList[0].target.groupcast.groupID;
    EmberAfStatus UNUSED status = emGpAddToApsGroup(GP_ENDPOINT, groupId);
  }
  sinkEntry.securityOptions = newSinkTableSecurityOptions;
  // carefull, take the gpd outgoing FC (not the framecounter of the commissioning frame "gpdSecurityFrameCounter")
  sinkEntry.gpdSecurityFrameCounter = commissioningGpd->outgoingFrameCounter;
  MEMCOPY(sinkEntry.gpdKey.contents, commissioningGpd->key.contents, EMBER_ENCRYPTION_KEY_SIZE);
  if (emberAfPluginGreenPowerServerSinkTableAccessNotificationCallback(gpdAddr,
                                                                       GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_ADD_GPD)) {
    emberGpSinkTableSetEntry(sinkEntryIndex, &sinkEntry);
  }
  // End of Sink Table Update

  if (commissioningGpd->doNotSendGpPairing) {
    commissioningGpd->doNotSendGpPairing = false;
    return;
  }

  sendGpPairingFromSinkEntry(&sinkEntry,
                             commissioningGpd->gppShortAddress);

  // Send GpPairing Configuration Based on Sink Communication mode
  sendGpPairingConfigBasedOnSinkCommunicationMode(commissioningGpd);
  // If a pairing was added, the sink SHALL send a Device_annce command
  // for the alias (with the exception of lightweight unicast communication mode).
  if (sinkCommunicationMode != EMBER_GP_SINK_TYPE_LW_UNICAST) {
    sendDeviceAnncement(sinkEntry.assignedAlias);
  }
}

static void handleClosingCommissioningSessionOnFirstPairing(GpCommDataSaved *commissioningGpd)
{
  uint8_t gpsCommissioningExitMode;
  EmberAfStatus statusExitMode;
  EmberAfAttributeType type;
  statusExitMode = emberAfReadAttribute(GP_ENDPOINT,
                                        ZCL_GREEN_POWER_CLUSTER_ID,
                                        ZCL_GP_SERVER_GPS_COMMISSIONING_EXIT_MODE_ATTRIBUTE_ID,
                                        (CLUSTER_MASK_SERVER),
                                        (uint8_t*)&gpsCommissioningExitMode,
                                        sizeof(uint8_t),
                                        &type);
  if (statusExitMode == EMBER_ZCL_STATUS_SUCCESS
      && (gpsCommissioningExitMode & EMBER_AF_GP_SINK_COMMISSIONING_MODE_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS)) {
    // if commissioning mode is ON and received frame set it to OFF, exit comissioning mode
    commissioningState.inCommissioningMode = false;

    // commissioning session ended here,clean up stored information
    resetOfMultisensorDataSaved(true, commissioningGpd);

    if (commissioningState.proxiesInvolved) {
      uint8_t UNUSED retval;
      uint8_t proxyOptions = 0;
      if (commissioningState.unicastCommunication) { //  based on the commission mode as decided by sink
        proxyOptions |= 0x20;
      }
      emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(proxyOptions,
                                                                       0,
                                                                       0);
      EmberApsFrame *apsFrame;
      apsFrame = emberAfGetCommandApsFrame();
      apsFrame->sourceEndpoint = GP_ENDPOINT;
      apsFrame->destinationEndpoint = GP_ENDPOINT;
      retval = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
      emberAfGreenPowerClusterPrintln("SinkCommissioningModeCallback(Exit) send returned %d", retval);
      // add a call for user application to let it know that the commissioning session ends
      // for application perspective this should be as same as a commissioning windows tiemout expiration
      // then the following user callback is used:
      emberAfGreenPowerServerCommissioningTimeoutCallback(COMMISSIONING_TIMEOUT_TYPE_COMMISSIONING_WINDOW_TIMEOUT,
                                                          noOfCommissionedEndpoints,
                                                          commissionedEndPoints);
    }
  }
}

static void finalisePairing(GpCommDataSaved *commissioningGpd)
{
  // Check if the Sink can support the functionality of GPD
  if (handleGpdMatchingSinkFunctionality(commissioningGpd)) {
    // If bidirectional flag is set, queue a Commissioning Reply
    // with Gp Response.
    if (commissioningGpd->commissionState == GP_SINK_COMM_STATE_SEND_COMM_REPLY) {
      emberAfGreenPowerClusterPrintln("Sink Supports GPD - Calling GP Response");
      // Finalize the sink entry upon reception of success
      sendCommissioningReply(commissioningGpd);
      commissioningGpd->commissionState = GP_SINK_COMM_STATE_WAIT_FOR_SUCCESS;
    } else if (commissioningGpd->commissionState == GP_SINK_COMM_STATE_FINALISE_PAIRING) {
      emberAfGreenPowerClusterPrintln("Calling Adding TT and Sink ");
      if (handleAddingTranstionTableEntry(commissioningGpd)) {
        handleSinkEntryAndPairing(commissioningGpd);
        handleClosingCommissioningSessionOnFirstPairing(commissioningGpd);
        commissioningGpd->commissionState = GP_SINK_COMM_STATE_PAIRING_DONE;
        // Call the user to inform the pairing is finalised.
        emberAfGreenPowerServerPairingCompleteCallback(noOfCommissionedEndpoints,
                                                       commissionedEndPoints);
        emberAfGreenPowerServerDeleteCommissioningGpdInstance(&(commissioningGpd->addr));
      } else {
        emberAfGreenPowerClusterPrintln("Finalize Pairing : Translation Table Additon Failed.");
      }
    }
  } else {
    emberAfGreenPowerClusterPrintln("Finalize Pairing : Sink Functionality Does Not Support the GPD");
  }
}

static bool gpCommissioningNotificationCommissioningGpdf(uint16_t commNotificationOptions,
                                                         EmberGpAddress *gpdAddr,
                                                         uint32_t gpdSecurityFrameCounter,
                                                         uint8_t gpdCommandId,
                                                         uint8_t *gpdCommandPayload,
                                                         uint16_t gppShortAddress,
                                                         GpCommDataSaved *commissioningGpd)
{
  emberAfGreenPowerClusterPrintln("Process GP CN");
  uint8_t gpsSecurityLevelAttribute = 0;
  EmberAfStatus status;
  EmberAfAttributeType type;
  status = emberAfReadAttribute(GP_ENDPOINT,
                                ZCL_GREEN_POWER_CLUSTER_ID,
                                ZCL_GP_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_ID,
                                (CLUSTER_MASK_SERVER),
                                (uint8_t*)&gpsSecurityLevelAttribute,
                                sizeof(uint8_t),
                                &type);
  // Reject the req if InvolveTC is set in the attribute
  if (gpsSecurityLevelAttribute & 0x08) { // Involve TC is set in the Sink
    emberAfGreenPowerClusterPrintln("DROP - GP CN : Involve TC bit is set in gpsSecurityLevelAttribute!");
    return true;
  }

  EmberGpApplicationInfo appInfoStruct = { 0 };
  uint8_t index = 1; // gpdCommandPayload [0] is length, hence set to 1
  appInfoStruct.deviceId = gpdCommandPayload[index++];
  uint8_t gpdfOptions = gpdCommandPayload[index++];
  // parse option and ext option from gpdCommandPayload of the commissioning command
  uint8_t gpdfExtendedOptions = 0;
  if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_EXTENDED_OPTIONS_FIELD) {
    gpdfExtendedOptions = gpdCommandPayload[index++];
  }
  // Parse security key type and level
  uint8_t securityLevel = (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_SECURITY_LEVEL_CAPABILITIES);

  if (securityLevel == EMBER_GP_SECURITY_LEVEL_RESERVED
      || (status == EMBER_ZCL_STATUS_SUCCESS
          && (((gpsSecurityLevelAttribute & 0x03) > securityLevel) // GPD Security Level is lower than Sink supports
              || ((gpsSecurityLevelAttribute & 0x04) // Sink requires the GPD to support key encryption
                  && (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT) // Key Present but not encrypted
                  && !(gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION))))) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN : Sec Level < gpsSecurityLevelAttribute!");
    return true;
  }
  uint8_t securityKeyType = ((gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE)
                             >> EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET);
  // If security Level is none 0 and Gpd is in commissioning process currently
  // It has following combinitions for key negotiation
  // Unidirectional (rxAfterTx = 0) : shall send a key in one of the commissioning frames (current or last one)
  //                                  if it does not have then drop it.
  // Bidirectional  (rxAfterTx = 1) : may send a key and/or request one
  if (securityLevel != 0
      && !(commissioningGpd->gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT)  // has it received key from GPD in past, for first commissioning frame - this will be false
      && !(gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT)) { // No Key in this frame
    emberAfGreenPowerClusterPrintln("Sec Level > 0, No Key Received Yet");
    if (!(commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX)) {
      emberAfGreenPowerClusterPrintln("DROP - GP CN : No Key Present, Unidirectional RxAfterTx = 0");
      return true;
    } else {
      // RxAfterTx is set but no request key, then Drop
      if (!(gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_GP_SECURITY_KEY_REQUEST)) {
        emberAfGreenPowerClusterPrintln("DROP - GP CN : No Key Request, Bidirectional RxAfterTx = 1");
        return true;
      }
    }
  }
  EmberKeyData key = { 0 };
  if (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT) {
    MEMMOVE(key.contents, (gpdCommandPayload + index), EMBER_ENCRYPTION_KEY_SIZE);
    index += EMBER_ENCRYPTION_KEY_SIZE;
    // If the incoming key is encrypted, check its authenticity
    if (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION) {
      EmberKeyData linkKey = { GP_DEFAULT_LINK_KEY };
      uint8_t mic[4] = { 0 };
      // Read in reverse for comparison with generated calculated mic array
      uint32_t keyMic = emberFetchLowHighInt32u(gpdCommandPayload + index);
      index += 4;
      // Decrypt and derive the incomming Key
      emGpKeyTcLkDerivation(gpdAddr,
                            0,
                            mic,
                            &linkKey,
                            &key,
                            true);
      emberAfGreenPowerClusterPrintln("rx MIC %4x", keyMic);
      emberAfGreenPowerClusterPrint("Calculated MIC :");
      emberAfGreenPowerClusterPrintBuffer(mic, 4, 0);
      emberAfGreenPowerClusterPrint("\nKey : ");
      emberAfGreenPowerClusterPrintBuffer(key.contents, 16, 0);
      emberAfGreenPowerClusterPrintln("");
      if (0 != MEMCOMPARE(mic, (uint8_t*)(&keyMic), 4)) {
        emberAfGreenPowerClusterPrintln("DROP - GP CN : Supplied Key MIC mismatch");
        return true;
      }
    }
  }
  // Initialize framecounter with gpdSecurityFrameCounter,
  // then if gpdf has security info pick it from gpd command payload.
  uint32_t outgoingFrameCounter = gpdSecurityFrameCounter;
  if (securityLevel >= EMBER_GP_SECURITY_LEVEL_FC_MIC
      && (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_OUTGOING_COUNTER_PRESENT)) {
    outgoingFrameCounter = emberFetchLowHighInt32u(gpdCommandPayload + index);
    index += 4;
  }

  if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_APPLICATION_INFORMATION_PRESENT) {
    appInfoStruct.applInfoBitmap = gpdCommandPayload[index++];
  }

  if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_MANUFACTURE_ID_PRESENT) {
    appInfoStruct.manufacturerId = emberFetchLowHighInt16u(gpdCommandPayload + index);
    index += 2;
  }
  if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_MODEL_ID_PRESENT) {
    appInfoStruct.modelId = emberFetchLowHighInt16u(gpdCommandPayload + index);
    index += 2;
  }
  // Parse the Commands and flag the generic command
  bool genericCmdPresent = false;
  if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_COMMANDS_PRESENT) {
    appInfoStruct.numberOfGpdCommands = gpdCommandPayload[index++];
    for (int i = 0; i < appInfoStruct.numberOfGpdCommands; i++) {
      appInfoStruct.gpdCommands[i] = gpdCommandPayload[index];
      if ((appInfoStruct.gpdCommands[i] == EMBER_ZCL_GP_GPDF_8BITS_VECTOR_PRESS)
          || (appInfoStruct.gpdCommands[i] == EMBER_ZCL_GP_GPDF_8BITS_VECTOR_RELEASE)) {
        genericCmdPresent = true;
      }
      index++;
    }
  }
  // Parse the cluster list
  if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_CLUSTER_LIST_PRESENT) {
    uint8_t numClusters = gpdCommandPayload[index++];
    appInfoStruct.numberOfGpdServerCluster = numClusters & 0x0F;
    for (int i = 0; i < appInfoStruct.numberOfGpdServerCluster; i++) {
      appInfoStruct.serverClusters[i] = emberFetchLowHighInt16u(gpdCommandPayload + index);
      index += 2;
    }
    appInfoStruct.numberOfGpdClientCluster = numClusters >> 4;
    for (int i = 0; i < appInfoStruct.numberOfGpdClientCluster; i++) {
      appInfoStruct.clientClusters[i] = emberFetchLowHighInt16u(gpdCommandPayload + index);
      index += 2;
    }
  }

  EmberGpSwitchInformation switchInformationStruct = { 0 };
  if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_PRESENT) {
    switchInformationStruct.switchInfoLength = gpdCommandPayload[index++];
    switchInformationStruct.nbOfContacts = (gpdCommandPayload[index]
                                            & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_NB_OF_CONTACT);
    switchInformationStruct.switchType = ((gpdCommandPayload[index++]
                                           & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE)
                                          >> EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE_OFFSET);
    switchInformationStruct.currentContact = gpdCommandPayload[index++];
  }

  if ((appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_APPLICATION_DESCRIPTION_COMMAND_FOLLOWS)
      && (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX)) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN : rxAfterTx and Application Description follows both are set");
    return true;
  }

  if (commissioningGpd->commissionState == GP_SINK_COMM_STATE_COLLECT_REPORTS
      && (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX)) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN : rxAfterTx while report collection in progress");
    return true;
  }

  if (index > gpdCommandPayload[0] + 1) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN : Short payload");
    //we ran off the end of the payload
    return true;
  }

  emberAfGreenPowerClusterPrintln("GP CN : Saving the GPD");
  MEMCOPY(&(commissioningGpd->addr), gpdAddr, sizeof(EmberGpAddress));
  commissioningGpd->applicationInfo.deviceId = appInfoStruct.deviceId;
  commissioningGpd->gpdfOptions = gpdfOptions;
  if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_EXTENDED_OPTIONS_FIELD) {
    commissioningGpd->gpdfExtendedOptions = gpdfExtendedOptions;
    commissioningGpd->securityKeyType = securityKeyType;
    commissioningGpd->securityLevel = securityLevel;
    if (gpdfExtendedOptions & EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT) {
      MEMCOPY(commissioningGpd->key.contents, key.contents, EMBER_ENCRYPTION_KEY_SIZE);
    }
  }
  if (gpdfOptions & EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_APPLICATION_INFORMATION_PRESENT) {
    MEMCOPY(&(commissioningGpd->applicationInfo), &appInfoStruct, sizeof(EmberGpApplicationInfo));
    if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_PRESENT) {
      MEMCOPY(&(commissioningGpd->switchInformationStruct), &switchInformationStruct, sizeof(EmberGpSwitchInformation));
    }
  }
  commissioningGpd->outgoingFrameCounter = outgoingFrameCounter;

  // Use the sinkDefault communication mode as the Gpdf has no option to provide one
  emberAfReadAttribute(GP_ENDPOINT,
                       ZCL_GREEN_POWER_CLUSTER_ID,
                       ZCL_GP_SERVER_GPS_COMMUNICATION_MODE_ATTRIBUTE_ID,
                       (CLUSTER_MASK_SERVER),
                       (uint8_t*)&(commissioningGpd->communicationMode),
                       sizeof(uint8_t),
                       &type);
  // No assigned alias as the Gpdf has no option to provide one
  commissioningGpd->useGivenAssignedAlias = false;

  if ((appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_PRESENT)
      && (appInfoStruct.deviceId == EMBER_GP_DEVICE_ID_GPD_GENERIC_SWITCH
          || genericCmdPresent == true)) {
    // on each reception of a Generic Switch commissioning command start a 60s
    // timeout to collect the PRESS and RELEASE order frames to extend
    // GenericSwitch Translation Table created for this GPD
    emberAfGreenPowerClusterPrintln("GP CN : Start Switch Commissioning Timeout");
    uint32_t delay = EMBER_AF_PLUGIN_GREEN_POWER_SERVER_GENERIC_SWITCH_COMMISSIONING_TIMEOUT_IN_SEC;
    slxu_zigbee_event_set_delay_ms(genericSwitchCommissioningTimeout,
                                   delay * MILLISECOND_TICKS_PER_SECOND);
  }

  if (appInfoStruct.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_APPLICATION_DESCRIPTION_COMMAND_FOLLOWS) {
    if (!slxu_zigbee_event_is_active(multiSensorCommissioningTimeout)) {
      emberAfGreenPowerClusterPrintln("GP CN : Reseting Partial data - Only Report Data");
      resetOfMultisensorDataSaved(false, commissioningGpd);
    }
    emberAfGreenPowerClusterPrintln("GP CN : Start MS/CAR Commissioning Timeout and waiting for ADCF");
    uint32_t delay = EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_TIMEOUT_IN_SEC;
    slxu_zigbee_event_set_delay_ms(multiSensorCommissioningTimeout,
                                   delay * MILLISECOND_TICKS_PER_SECOND);
    // All set to collect the report descriptors
    commissioningGpd->commissionState = GP_SINK_COMM_STATE_COLLECT_REPORTS;
    return true;
  }
  // Check if bidirectional commissioning, flag the state to finalize the pairing.
  if (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX) {
    // Send Commissioning reply and finalize the sink entry upon reception of success
    commissioningGpd->gppShortAddress = gppShortAddress;
    commissioningGpd->commissionState = GP_SINK_COMM_STATE_SEND_COMM_REPLY;
  } else {
    // No reports to collect AND no commReply to be sent, then finalise pairing
    commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
  }
  // Based on the states the following function either sends a commReply or finalises pairing
  finalisePairing(commissioningGpd);
  return true;
}

static bool gpCommissioningNotificationApplicationDescriptionGpdf(uint16_t commNotificationOptions,
                                                                  uint32_t gpdSecurityFrameCounter,
                                                                  uint8_t *gpdCommandPayload,
                                                                  uint16_t gppShortAddress,
                                                                  GpCommDataSaved *commissioningGpd)
{
  if (gpdCommandPayload == NULL) {
    return true;
  }
  if (gpdCommandPayload[0] < 2) { // ADCF payload has minimum 2 entries
    return true;
  }
  static bool lastReportIdHasRxAfterTx = false;
  emberAfGreenPowerClusterPrintln("GP CN : Process ADCF");
  if (commissioningGpd->commissionState != GP_SINK_COMM_STATE_COLLECT_REPORTS
      && !lastReportIdHasRxAfterTx) {
    emberAfGreenPowerClusterPrintln("ADCF DROP : Not in report collection state (only allow bidirectional frames)");
    return true;
  }
  // Process this command if the MS timer is still running that was started in a commissioning gpdf
  // else reject command or the node has collected the reports just allow the bidirectional frames to pull a
  // commissioning reply out
  if (!slxu_zigbee_event_is_active(multiSensorCommissioningTimeout)
      && !lastReportIdHasRxAfterTx) {
    emberAfGreenPowerClusterPrintln("ADCF DROP : MS timer has expired");
    return true;
  }
  // The first Application description frame received,start the timeout
  if (commissioningGpd->totalNbOfReport == 0) {
    lastReportIdHasRxAfterTx = false;
    // start a timeout to collect the ReportDescriptors thanks to the application description frames
    uint32_t delay =  EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_TIMEOUT_IN_SEC;
    slxu_zigbee_event_set_delay_ms(multiSensorCommissioningTimeout,
                                   delay * MILLISECOND_TICKS_PER_SECOND);
  }
  // Collect the report descriptors in RAM with short payload error check
  if (!saveReportDescriptor(gpdCommandPayload[1],    // Total Number of Reports
                            gpdCommandPayload[2],    // Number of reports in this frame
                            &gpdCommandPayload[3],
                            commissioningGpd)) { // Start of report data
    // Error in Application Description payload
    emberAfGreenPowerClusterPrintln("ADCF DROP : Error in Report Desc.");
    return true;
  }
  // just remeber if the lastfarme during the report collection has indicated a rx capability when out of order
  if ((commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX)
      && emGpFindReportId((gpdCommandPayload[1] - 1),
                          gpdCommandPayload[2],
                          &gpdCommandPayload[3]) != NULL) {
    lastReportIdHasRxAfterTx = true;
  }
  // Check if all the reports received else keep collecting
  if (commissioningGpd->numberOfReports != commissioningGpd->totalNbOfReport) {
    emberAfGreenPowerClusterPrintln("ADCF : Wait for % d more reports",
                                    (commissioningGpd->totalNbOfReport - commissioningGpd->totalNbOfReport));
    return true;
  }
  emberAfGreenPowerClusterPrintln("ADCF : Collected All the reports -- looking for last reportId");
  // Find the last reportId (= total reports - 1) in this frame it it was decided that it was a bidirectional
  // with rx capability
  if (lastReportIdHasRxAfterTx
      && emGpFindReportId((gpdCommandPayload[1] - 1),
                          gpdCommandPayload[2],
                          &gpdCommandPayload[3]) == NULL) {
    return true;
  }
  emberAfGreenPowerClusterPrintln("ADCF : Last reportId found in this frame");
  slxu_zigbee_event_set_inactive(multiSensorCommissioningTimeout);
  // If this frame with last reportId has the rxAfterTx set send a commissioning reply and the pairing to be finalized
  // in Success
  if (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX) {
    // Send Commissioning reply and finalize the sink entry upon reception of success
    commissioningGpd->gppShortAddress = gppShortAddress;
    commissioningGpd->commissionState = GP_SINK_COMM_STATE_SEND_COMM_REPLY;
  } else {
    commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
  }

  finalisePairing(commissioningGpd);
  return true;
}

static bool gpCommissioningNotificationSuccessGpdf(uint16_t commNotificationOptions,
                                                   uint32_t gpdSecurityFrameCounter,
                                                   GpCommDataSaved *commissioningGpd)
{
  uint8_t securityLevel = ((commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_LEVEL)
                           >> EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET);

  if (securityLevel != commissioningGpd->securityLevel) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN Success: Sec Level mismatch");
    return true;
  }
  // security level has passed so check the FC
  if (commissioningGpd->outgoingFrameCounter > gpdSecurityFrameCounter) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN Success: Frame Counter Too Low");
    return true;
  }
  // Save FC
  commissioningGpd->outgoingFrameCounter = gpdSecurityFrameCounter;
  emberAfGreenPowerClusterPrint("GP CN : Process Success - Finalise Pairing");
  commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
  finalisePairing(commissioningGpd);
  return true;
}

static bool gpCommissioningNotificationDecommissioningGpdf(uint16_t commNotificationOptions,
                                                           EmberGpAddress *gpdAddr)
{
  emberAfGreenPowerClusterPrintln("Processing Decommissioning");
  uint8_t secLvl = ((commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_LEVEL)
                    >> EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET);
  uint8_t keyType = ((commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_KEY_TYPE)
                     >> EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_KEY_TYPE_OFFSET);
  decommissionGpd(secLvl, keyType, gpdAddr, true);
  return true;
}

static void handleChannelRequest(uint16_t options,
                                 uint16_t gppShortAddress,
                                 bool rxAfterTx,
                                 uint8_t *gpdCommandPayload)
{
  if (gpdCommandPayload == NULL) {
    return; // Channel request has always one byte payload
  }
  if (!rxAfterTx) {
    // DROP as the rxAfterTx is not set of this notification
    emberAfGreenPowerClusterPrintln("DROP : Channel Request NO - rxAfterTx");
    return;
  }
  //Basic sink just runs with the first one, doesn't select the best
  uint8_t responseOption = options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID; // Application Id
  uint8_t nextChannel = (gpdCommandPayload[1]
                         & EMBER_AF_GP_GPD_CHANNEL_REQUEST_CHANNEL_TOGGLING_BEHAVIOR_RX_CHANNEL_NEXT_ATTEMPT);
  uint8_t channelConfigPayload;
  channelConfigPayload = (emberAfGetRadioChannel() - 11) & EMBER_AF_GP_GPD_CHANNEL_CONFIGURATION_CHANNEL_OPERATIONAL_CHANNEL;
  channelConfigPayload |= (1 << EMBER_AF_GP_GPD_CHANNEL_CONFIGURATION_CHANNEL_BASIC_OFFSET); // BASIC
  emberAfFillCommandGreenPowerClusterGpResponseSmart(responseOption,
                                                     gppShortAddress,
                                                     nextChannel,
                                                     0,
                                                     NULL,
                                                     0,
                                                     EMBER_ZCL_GP_GPDF_CHANNEL_CONFIGURATION,
                                                     sizeof(uint8_t),
                                                     &channelConfigPayload);
  EmberApsFrame *apsFrame;
  apsFrame = emberAfGetCommandApsFrame();
  apsFrame->sourceEndpoint = GP_ENDPOINT;
  apsFrame->destinationEndpoint = GP_ENDPOINT;
  if (commissioningState.unicastCommunication) { //  based on the commission mode as decided by sink
    emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, gppShortAddress);
  } else {
    emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
  }
}

static uint16_t storeSinkTableEntryInBuffer(EmberGpSinkTableEntry *entry,
                                            uint8_t *buffer)
{
  uint8_t *finger = buffer;
  uint8_t securityLevel = (entry->securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);
  uint8_t securityKeyType = (entry->securityOptions
                             & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE)
                            >> EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET;

  emberAfGreenPowerClusterPrintln("GP SERVER - STORE SINK TABLE ENTRY into a buffer");
  // copy options field
  emberAfCopyInt16u(finger, 0, (uint16_t)(entry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_MASK));
  finger += sizeof(uint16_t);
  // copy 32bit or 64bit address field
  if (entry->gpd.applicationId == EMBER_GP_APPLICATION_SOURCE_ID) {
    emberAfCopyInt32u(finger, 0, entry->gpd.id.sourceId);
    finger += sizeof(uint32_t);
  } else if (entry->gpd.applicationId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    MEMMOVE(finger, entry->gpd.id.gpdIeeeAddress, EUI64_SIZE);
    finger += EUI64_SIZE;
  }
  // copy ieee endpoint field
  if (entry->gpd.applicationId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
    emberAfCopyInt8u(finger, 0, entry->gpd.endpoint);
    finger += sizeof(uint8_t);
  }
  // DeviceID field
  emberAfGreenPowerClusterPrintln("GPD deviceId %x", entry->deviceId);
  emberAfCopyInt8u(finger, 0, entry->deviceId);
  finger += sizeof(uint8_t);

  // copy Group List field if present
  EmberGpSinkType sinkCommunicationMode = (EmberGpSinkType)((entry->options
                                                             & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE)
                                                            >> EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_COMMUNICATION_MODE_OFFSET);
  if (sinkCommunicationMode == EMBER_GP_SINK_TYPE_GROUPCAST) {
    // let's count
    uint8_t index = 0;
    uint8_t *entryCount = finger;
    emberAfCopyInt8u(finger, 0, 0x00);
    finger += sizeof(uint8_t);
    for (index = 0; index < GP_SINK_LIST_ENTRIES; index++) {
      // table is available
      EmberGpSinkListEntry *sinkEntry = &entry->sinkList[index];
      if (sinkEntry->type == EMBER_GP_SINK_TYPE_GROUPCAST) {
        emberAfCopyInt16u(finger, 0, sinkEntry->target.groupcast.groupID);
        finger += sizeof(uint16_t);
        emberAfCopyInt16u(finger, 0, sinkEntry->target.groupcast.alias);
        finger += sizeof(uint16_t);
        (*entryCount)++;
      } else {
        continue;
      }
    }
  }
  // copy GPD assigned alias field
  if (entry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_ASSIGNED_ALIAS) {
    emberAfGreenPowerClusterPrintln("assigned alias %2x", entry->assignedAlias);
    emberAfCopyInt16u(finger, 0, entry->assignedAlias);
    finger += sizeof(uint16_t);
  }
  // copy Groupcast radius field
  emberAfCopyInt8u(finger, 0, entry->groupcastRadius);
  finger += sizeof(uint8_t);

  // copy Security Options field
  if (entry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SECURITY_USE) {
    emberAfGreenPowerClusterPrintln("security options %1x", entry->securityOptions);
    emberAfCopyInt8u(finger, 0, entry->securityOptions);
    finger += sizeof(uint8_t);

    // copy GPD Sec Frame Counter (if lvl>0 or if SeqNumCapability=0b1)
    // Lvl>0 is check cause it is allow to set SecurityUse=0b1 and tell SecLvl=0
    if ( (securityLevel > EMBER_GP_SECURITY_LEVEL_NONE)
         || (entry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SEQUENCE_NUM_CAPABILITIES) ) {
      emberAfGreenPowerClusterPrintln("security frame counter %4x", entry->gpdSecurityFrameCounter);
      emberAfCopyInt32u(finger, 0, entry->gpdSecurityFrameCounter);
      finger += sizeof(uint32_t);

      // If SecurityLevel is 0b00 or if the SecurityKeyType has value:
      // - 0b001 (NWK key),
      // - 0b010 (GPD group key)
      // - 0b111 (derived individual GPD key),
      // the GPDkey parameter MAY be omitted and the key MAY be stored in the
      // gpSharedSecurityKey parameter instead.
      // If SecurityLevel has value other than 0b00 and the SecurityKeyType has
      // value 0b111 (derived individual GPD key), the sink table GPDkey parameter MAY be
      // omitted and the key MAY calculated on the fly, based on the value stored
      // in the gpSharedSecurityKey parameter (=groupKey).
      if ( !(securityLevel == EMBER_GP_SECURITY_LEVEL_NONE) ) {
        if ( !(securityKeyType == EMBER_ZCL_GP_SECURITY_KEY_TYPE_ZIGBEE_NETWORK_KEY)
             && !(securityKeyType == EMBER_ZCL_GP_SECURITY_KEY_TYPE_GPD_GROUP_KEY)
             && !(securityKeyType == EMBER_ZCL_GP_SECURITY_KEY_TYPE_DERIVED_INDIVIDUAL_GPD_KEY) ) {
          // if keyType is OOB and NK, else it is possible to use "gpsSharedSecurityKey" attribut
          // to save space in the sink table
          MEMMOVE(finger, entry->gpdKey.contents, EMBER_ENCRYPTION_KEY_SIZE);
          finger += EMBER_ENCRYPTION_KEY_SIZE;
        } else {
          // key is present in the reponse message but not store in the sink table but in
          // the gpSharedSecurityKey attribut, so read "gpSharedSecurityKey" attribut
          EmberKeyData gpSharedSecurityKey;
          EmberAfAttributeType type;
          EmberAfStatus status = emberAfReadAttribute(GP_ENDPOINT,
                                                      ZCL_GREEN_POWER_CLUSTER_ID,
                                                      ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_ATTRIBUTE_ID,
                                                      CLUSTER_MASK_SERVER,
                                                      gpSharedSecurityKey.contents,
                                                      EMBER_ENCRYPTION_KEY_SIZE,
                                                      &type);
          if (status == EMBER_ZCL_STATUS_SUCCESS) {
            MEMMOVE(finger, gpSharedSecurityKey.contents, EMBER_ENCRYPTION_KEY_SIZE);
          } else {
            // optional "gpsSharedSecurityKey" attribut is not supported
            // thus key is always into the sink table even if it is shared
            MEMMOVE(finger, entry->gpdKey.contents, EMBER_ENCRYPTION_KEY_SIZE);
          }
          finger += EMBER_ENCRYPTION_KEY_SIZE;
        }
      }
    }
  } else if (entry->options & EMBER_AF_GP_SINK_TABLE_ENTRY_OPTIONS_SEQUENCE_NUM_CAPABILITIES) {
    // gpdSecurityFrameCounter is mandatory if Security use = 0b1 (treated above)
    //      or Sequence number capabilities = 0b1 and Security use = 0b0
    emberAfGreenPowerClusterPrintln("security frame counter %4x", entry->gpdSecurityFrameCounter);
    emberAfCopyInt32u(finger, 0, entry->gpdSecurityFrameCounter);
    finger += sizeof(uint32_t);
  } else {
    // if no security and no seq number capability, nothing more to copy into the response
  }
  return (uint16_t)(finger - buffer);
}

static bool processCommNotificationsWithSecurityProcessingFailedFlag(uint16_t commNotificationOptions,
                                                                     EmberGpAddress *gpdAddr,
                                                                     uint32_t gpdSecurityFrameCounter,
                                                                     uint8_t *gpdCommandId,
                                                                     uint8_t *gpdCommandPayload,
                                                                     uint32_t commissioningNotificationMic,
                                                                     EmberKeyData *key,
                                                                     uint8_t securityKeyType)
{
  emberAfGreenPowerClusterPrintln("\nGP CN Security Processing Failed is set");
  // MIC is only present if security processing failed is set, so validate MIC
  uint8_t mic[4] = { 0 };
  uint8_t secLevel = (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_LEVEL)
                     >> EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET;
  bool securityProcessing = emGpCalculateIncomingCommandMic(gpdAddr,
                                                            ((commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX) ? true : false),
                                                            ((securityKeyType == EMBER_ZCL_GP_SECURITY_KEY_TYPE_INDIVIDIGUAL_GPD_KEY
                                                              || securityKeyType == EMBER_ZCL_GP_SECURITY_KEY_TYPE_DERIVED_INDIVIDUAL_GPD_KEY) \
                                                             ? EMBER_AF_GREEN_POWER_GP_INDIVIDUAL_KEY : EMBER_AF_GREEN_POWER_GP_SHARED_KEY),
                                                            secLevel,
                                                            gpdSecurityFrameCounter,
                                                            *gpdCommandId,
                                                            gpdCommandPayload,
                                                            ((secLevel > EMBER_GP_SECURITY_LEVEL_FC_MIC) ? true : false),
                                                            mic,
                                                            key);
  emberAfGreenPowerClusterPrint("\n GP CN Sec Level = %d, App Id = %d, MIC Validation : %p"
                                "\n GP CN Calculated Mic : %4x"
                                "\n GP CN Received Mic   : %4x\n",
                                secLevel,
                                gpdAddr->applicationId,
                                (securityProcessing ? "SUCCESS" : "FAILED"),
                                (*(uint32_t*)mic),
                                commissioningNotificationMic);
  if (!securityProcessing
      || ((*(uint32_t*)mic) != commissioningNotificationMic)) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN MIC Mismatch");
    emberAfPluginGreenPowerServerGpdSecurityFailureCallback(gpdAddr);
    return false;
  }

  if (EMBER_GP_SECURITY_LEVEL_FC_MIC_ENCRYPTED == secLevel) {
    // Temp Payload to be decrypted without payloadLengthe in between in the form of : {commandId,{command payload}}
    uint8_t payload[GP_COMMISSIONING_MAX_BYTES + 1] = { 0 };
    payload[0] = *gpdCommandId;
    uint8_t length = 1;
    if ((gpdCommandPayload != NULL)
        && (gpdCommandPayload[0] < GP_COMMISSIONING_MAX_BYTES)) {
      MEMCOPY((payload + 1), gpdCommandPayload + 1, gpdCommandPayload[0]);
      length += gpdCommandPayload[0];
    }
    emGpCalculateIncomingCommandDecrypt(gpdAddr,
                                        gpdSecurityFrameCounter,
                                        length,
                                        payload,
                                        key);
    // Copy back the decrypted command with octate string payload format.
    *gpdCommandId = payload[0];
    if ((gpdCommandPayload != NULL)
        && (gpdCommandPayload[0] < GP_COMMISSIONING_MAX_BYTES)) {
      MEMCOPY((gpdCommandPayload + 1), (payload + 1), gpdCommandPayload[0]);
    }
  }
  return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global Functions
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
const uint8_t *emGpFindReportId(uint8_t reportId,
                                uint8_t numberOfReports,
                                const uint8_t *reports)
{
  const uint8_t *temp = reports;
  uint8_t length = 0; // 0 - position of reportId
  for (uint8_t noOfReport = 0; noOfReport < numberOfReports; noOfReport++) {
    // calculate the length of each report in the payload
    if (*temp == reportId) {
      return temp;
    }
    length = reportLength(temp);
    temp = temp + length;
  }
  return NULL;
}

// Add the endpoint to APS group with GroupId
EmberAfStatus emGpAddToApsGroup(uint8_t endpoint, uint16_t groupId)
{
  if (0xFF != findGroupInBindingTable(endpoint, groupId)) {
    // search returned an valid index, so duplicate exists
    return EMBER_ZCL_STATUS_SUCCESS; // per ZCL8; was DUPLICATE_EXISTS
  }
  // No duplicate entry, try adding
  // Look for an empty binding slot.
  for (uint8_t i = 0; i < EMBER_BINDING_TABLE_SIZE; i++) {
    EmberBindingTableEntry binding;
    if (emberGetBinding(i, &binding) == EMBER_SUCCESS
        && binding.type == EMBER_UNUSED_BINDING) {
      binding.type = EMBER_MULTICAST_BINDING;
      binding.identifier[0] = LOW_BYTE(groupId);
      binding.identifier[1] = HIGH_BYTE(groupId);
      binding.local = endpoint;

      EmberStatus status = emberSetBinding(i, &binding);
      if (status == EMBER_SUCCESS) {
        return EMBER_ZCL_STATUS_SUCCESS;
      } else {
        emberAfCorePrintln("ERR: Failed to create binding (0x%x)", status);
        return EMBER_ZCL_STATUS_FAILURE;
      }
    }
  }
  emberAfCorePrintln("ERR: Binding table is full");
  return EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
}

// Allow the sink to decided if it match with the GP frame it received
bool emGpEndpointAndClusterIdValidation(uint8_t endpoint,
                                        bool server,
                                        EmberAfClusterId clusterId)
{
  if (isCommissioningAppEndpoint(endpoint)
      && ((server && emberAfContainsServer(endpoint, clusterId))
          || (!server && emberAfContainsClient(endpoint, clusterId)))) {
    return true;
  }
  return false;
}

bool emAfPluginGreenPowerServerRetrieveAttributeAndCraftResponse(uint8_t endpoint,
                                                                 EmberAfClusterId clusterId,
                                                                 EmberAfAttributeId attrId,
                                                                 uint8_t mask,
                                                                 uint16_t manufacturerCode,
                                                                 uint16_t readLength)
{
  uint8_t sinkTableEntryAppResponseData[EMBER_AF_RESPONSE_BUFFER_LEN];
  uint8_t zclStatus = EMBER_ZCL_STATUS_SUCCESS;
  uint16_t stringDataOffsetStart = 0;
  uint16_t stringLength = 0;
  bool    status = false;

  if (endpoint != GP_ENDPOINT
      || clusterId != ZCL_GREEN_POWER_CLUSTER_ID
      || attrId != ZCL_GP_SERVER_SINK_TABLE_ATTRIBUTE_ID
      || mask != CLUSTER_MASK_SERVER
      || manufacturerCode != EMBER_AF_NULL_MANUFACTURER_CODE) {
    // do nothing but return false
  } else if (readLength < 6) {
    emberAfGreenPowerClusterPrintln("ERROR, Buffer length supplied %d is too small", readLength);
    // Can't fit the ZCL header in available length so exit with error
    // do nothing but return true as we are processing the correct type of packet
  } else {
    (void) emberAfPutInt16uInResp(attrId);
    // The sink table attribute is a long string ZCL attribute type, which means
    // it is encoded starting with a 2-byte length. We fill in the real length
    // after we have encoded the whole sink table.
    // Four bytes extra = 2byte length + 1 byte ZCl Status + 1 byte Attribute Type
    uint16_t stringDataOffset =  appResponseLength + 4;
    stringDataOffsetStart = stringDataOffset;
    // Search the sink table and respond with entries
    for (uint8_t i = 0; i < EMBER_GP_SINK_TABLE_SIZE; i++) {
      EmberGpSinkTableEntry entry;
      if (emberGpSinkTableGetEntry(i, &entry) == EMBER_SUCCESS) {
        emberAfGreenPowerClusterPrintln("Craft Response - Encode Sink Table %d", i);
        // Have a valid entry so encode response in temp buffer and add if it fits
        uint16_t sinkTableEntryLength = storeSinkTableEntryInBuffer(&entry, sinkTableEntryAppResponseData);
        if ((sinkTableEntryLength + stringDataOffset) > readLength) {
          // String is too big so
          zclStatus = EMBER_ZCL_STATUS_INSUFFICIENT_SPACE;
          emberAfGreenPowerClusterPrintln("Sink Table Attribute read INSUFFICIENT SPACE");
          break;
        } else {
          emberAfGreenPowerClusterPrintln("SAVE ENTRY %d", i);
          MEMMOVE(&appResponseData[stringDataOffset], sinkTableEntryAppResponseData, sinkTableEntryLength);
          stringDataOffset += sinkTableEntryLength;
        }
      }
    }
    // calculate string length
    stringLength = stringDataOffset - stringDataOffsetStart;
    if (zclStatus == EMBER_ZCL_STATUS_SUCCESS) {
      (void) emberAfPutInt8uInResp(zclStatus);
      (void) emberAfPutInt8uInResp(ZCL_LONG_OCTET_STRING_ATTRIBUTE_TYPE);
      (void) emberAfPutInt16uInResp(stringLength);
      emberAfGreenPowerClusterPrintln(" calculated string length = %d", (stringDataOffsetStart - stringDataOffset));
    } else {
      (void) emberAfPutInt8uInResp(zclStatus);
    }
    appResponseLength += stringLength;
    status = true;
  }
  emberAfGreenPowerClusterPrintln(" String length = %d ", stringLength);
  return status;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Green Power Server Commissioning Event Handlers.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Switch commissioning timeout handler
void emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEventHandler(SLXU_UC_EVENT)
{
  // stop the delay
  slxu_zigbee_event_set_inactive(genericSwitchCommissioningTimeout);
  emberAfGreenPowerServerCommissioningTimeoutCallback(COMMISSIONING_TIMEOUT_TYPE_GENERIC_SWITCH,
                                                      noOfCommissionedEndpoints,
                                                      commissionedEndPoints);
  resetOfMultisensorDataSaved(true, &gpdCommDataSaved[0]);
}

// MS commissioning timeout handler
void emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEventHandler(SLXU_UC_EVENT)
{
  // stop the delay
  slxu_zigbee_event_set_inactive(multiSensorCommissioningTimeout);
  emberAfGreenPowerServerCommissioningTimeoutCallback(COMMISSIONING_TIMEOUT_TYPE_MULTI_SENSOR,
                                                      noOfCommissionedEndpoints,
                                                      commissionedEndPoints);
  // reset RAM data saved concerning multisensor
  resetOfMultisensorDataSaved(true, &gpdCommDataSaved[0]);
}

// Green Power Server Commissioning Window
void emberAfPluginGreenPowerServerCommissioningWindowTimeoutEventHandler(SLXU_UC_EVENT)
{
  slxu_zigbee_event_set_inactive(commissioningWindowTimeout);
  emberAfGreenPowerServerCommissioningTimeoutCallback(COMMISSIONING_TIMEOUT_TYPE_COMMISSIONING_WINDOW_TIMEOUT,
                                                      noOfCommissionedEndpoints,
                                                      commissionedEndPoints);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Green Power Server Initialisation and stack status callbacks.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef UC_BUILD
void emberAfGreenPowerServerSinkTableInit(void)
{
  emberAfGreenPowerClusterPrintln("SinkTable Init..");
  emberGpSinkTableInit();
  greenPowerServerInitialised = true;
}

void emberAfPluginGreenPowerServerInitCallback(uint8_t init_level)
{
  switch (init_level) {
    case SL_ZIGBEE_INIT_LEVEL_EVENT:
    {
      slxu_zigbee_event_init(genericSwitchCommissioningTimeout,
                             emberAfPluginGreenPowerServerGenericSwitchCommissioningTimeoutEventHandler);
      slxu_zigbee_event_init(multiSensorCommissioningTimeout,
                             emberAfPluginGreenPowerServerMultiSensorCommissioningTimeoutEventHandler);
      slxu_zigbee_event_init(commissioningWindowTimeout,
                             emberAfPluginGreenPowerServerCommissioningWindowTimeoutEventHandler);
      break;
    }

    case SL_ZIGBEE_INIT_LEVEL_LOCAL_DATA:
    {
      #ifndef EZSP_HOST
      // Bring up the Sink table here in case of SoC, for NCP-Host
      // the same must be called after the NCP is initialised and the configured.
      emberAfGreenPowerServerSinkTableInit();
      #endif
      // A test to see the security upon reset
      // uncomment the assert to just run the security test
      //emGpTestSecurity();
      //assert(false);
      break;
    }
  }
}

#else // !UC_BUILD

void emberAfPluginGreenPowerServerInitCallback(void)
{
  // Bring up the Sink table
  emberAfGreenPowerClusterPrintln("SinkTable Init..");
  emberGpSinkTableInit();
  greenPowerServerInitialised = true;
  // A test to see the security upon reset
  // uncomment the assert to just run the security test
  //emGpTestSecurity();
  //assert(false);
}

#endif // UC_BUILD

void emberAfPluginGreenPowerServerStackStatusCallback(EmberStatus status)
{
  emberAfGreenPowerClusterPrintln("Green Power Server Stack Status Callback status = %x", status);
  if (status == EMBER_NETWORK_DOWN
      && emberStackIsPerformingRejoin() == FALSE) {
    emberGpSinkTableClearAll();
    greenPowerServerInitialised = false;
  }
  if (!emberAfGreenPowerServerUpdateInvolveTCCallback(status)) {
    updateInvolveTC(status);
  }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Following Section has all the command callback implementation.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// The callbacks are called fromt he call-command-handler.c, which is an autogenerated module baased on the
// device configuration.
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool emAfPluginGreenPowerServerGpSinkCommissioningModeCommandHandler(uint8_t options,
                                                                     uint16_t gpmAddrForSecurity,
                                                                     uint16_t gpmAddrForPairing,
                                                                     uint8_t sinkEndpoint)
{
  // Test 4..4.6 - not a sink ep or bcast ep - drop
  if (!isValidAppEndpoint(sinkEndpoint)) {
    emberAfGreenPowerClusterPrintln("DROP - Comm Mode Callback: Sink EP not supported");
    return false;
  }
  if ((options & EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_GPM_IN_SECURITY)
      || (options & EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_GPM_IN_PAIRING)) {
    return false;
  }
  EmberAfAttributeType type;
  uint8_t gpsSecurityLevelAttribute = 0;
  EmberAfStatus secLevelStatus = emberAfReadAttribute(GP_ENDPOINT,
                                                      ZCL_GREEN_POWER_CLUSTER_ID,
                                                      ZCL_GP_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_ID,
                                                      (CLUSTER_MASK_SERVER),
                                                      (uint8_t*)&gpsSecurityLevelAttribute,
                                                      sizeof(uint8_t),
                                                      &type);
  // Reject the req if InvolveTC is et in the attribute
  if (secLevelStatus == EMBER_ZCL_STATUS_SUCCESS
      && (gpsSecurityLevelAttribute & 0x08)) {
    return false;
  }
  uint16_t commissioningWindow = 0;
  uint8_t proxyOptions = 0;
  if (options & EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_ACTION) {
    commissioningState.inCommissioningMode = true;
    commissioningState.endpoint = sinkEndpoint; // The commissioning endpoint can be 0xFF as well
    //enter commissioning mode
    if (options
        & (EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_GPM_IN_SECURITY
           | EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_GPM_IN_PAIRING)) {
      //these SHALL be 0 for now
      //TODO also check involve-TC
      return false;
    }
    if (options & EMBER_AF_GP_SINK_COMMISSIONING_MODE_OPTIONS_INVOLVE_PROXIES) {
      commissioningState.proxiesInvolved = true;
    } else {
      commissioningState.proxiesInvolved = false;
    }

    // default 180s of GP specification
    commissioningWindow = EMBER_AF_ZCL_CLUSTER_GP_GPS_COMMISSIONING_WINDOWS_DEFAULT_TIME_S;

    uint8_t gpsCommissioningExitMode;
    uint16_t gpsCommissioningWindows;
    EmberAfStatus statusExitMode = emberAfReadAttribute(GP_ENDPOINT,
                                                        ZCL_GREEN_POWER_CLUSTER_ID,
                                                        ZCL_GP_SERVER_GPS_COMMISSIONING_EXIT_MODE_ATTRIBUTE_ID,
                                                        CLUSTER_MASK_SERVER,
                                                        (uint8_t*)&gpsCommissioningExitMode,
                                                        sizeof(uint8_t),
                                                        &type);
    EmberAfStatus statusCommWindow = emberAfReadAttribute(GP_ENDPOINT,
                                                          ZCL_GREEN_POWER_CLUSTER_ID,
                                                          ZCL_GP_SERVER_GPS_COMMISSIONING_WINDOW_ATTRIBUTE_ID,
                                                          CLUSTER_MASK_SERVER,
                                                          (uint8_t*)&gpsCommissioningWindows,
                                                          sizeof(uint16_t),
                                                          &type);
    proxyOptions = EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_ACTION;

    // set commissioningWindow based on "gpsCommissioningWindows" attribute if different from
    // commissioningWindow will be part of the frame if it is different from default 180s
    // in any case there is a timeout running into the GPPs to exit,
    // the default one (180s) not include or the one include in the ProxyCommMode(Enter) frame
    if (statusCommWindow == EMBER_ZCL_STATUS_SUCCESS
        && (gpsCommissioningExitMode
            & EMBER_AF_GP_SINK_COMMISSIONING_MODE_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION)) {
      commissioningWindow = gpsCommissioningWindows;
      proxyOptions |= EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION;
    }
    slxu_zigbee_event_set_delay_ms(commissioningWindowTimeout,
                                   commissioningWindow * MILLISECOND_TICKS_PER_SECOND);
    // Only one of the 2 next flag shall be set at the same time
    if (statusExitMode == EMBER_ZCL_STATUS_SUCCESS
        && (gpsCommissioningExitMode
            & EMBER_AF_GP_SINK_COMMISSIONING_MODE_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS)
        && !(gpsCommissioningExitMode
             & EMBER_AF_GP_SINK_COMMISSIONING_MODE_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT)) {
      proxyOptions |= EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS;
    } else if ((statusExitMode == EMBER_ZCL_STATUS_SUCCESS)
               && (gpsCommissioningExitMode
                   & EMBER_AF_GP_SINK_COMMISSIONING_MODE_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT)
               && !(gpsCommissioningExitMode
                    & EMBER_AF_GP_SINK_COMMISSIONING_MODE_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS)) {
      proxyOptions |= EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_GP_PROXY_COMMISSIONING_MODE_EXIT;
    } else {
      // In case both are cleared or both are set, nothing more to do
    }
  } else if (commissioningState.inCommissioningMode) {
    // If commissioning mode is ON and received frame set it to OFF, exit commissioning mode
    commissioningState.inCommissioningMode = false;
    // Deactive the timer
    slxu_zigbee_event_set_inactive(commissioningWindowTimeout);
    if (commissioningState.proxiesInvolved) {
      //involve proxies
      proxyOptions = 0;
    }
  }
  // On commissioning entry or exist, reset the number of report config received
  for (int i = 0; i < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES; i++) {
    resetOfMultisensorDataSaved(true, &gpdCommDataSaved[i]);
  }
  if (commissioningState.unicastCommunication) { //  based on the commission mode as decided by sink
    proxyOptions |= 0x20; // Flag unicast communication
  }
  emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(proxyOptions,
                                                                   commissioningWindow,
                                                                   0);
  EmberApsFrame *apsFrame;
  apsFrame = emberAfGetCommandApsFrame();
  apsFrame->sourceEndpoint = GP_ENDPOINT;
  apsFrame->destinationEndpoint = GP_ENDPOINT;
  if (commissioningState.proxiesInvolved) {
    EmberStatus status = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
    // Callback to inform the status of message submission to network
    emberAfGreenPowerClusterCommissioningMessageStatusNotificationCallback(&commissioningState,
                                                                           apsFrame,
                                                                           EMBER_OUTGOING_BROADCAST,
                                                                           EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS,
                                                                           status);
  } else {
    #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
    // Put the proxy instance on this node commissioning mode so that it can accept a pairing from itself.
    // This is to ensure the node will be able to handle gpdf commands after pairig.
    EmberStatus status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberAfGetNodeId());
    // Callback to inform the status of message submission to network
    emberAfGreenPowerClusterCommissioningMessageStatusNotificationCallback(&commissioningState,
                                                                           apsFrame,
                                                                           EMBER_OUTGOING_DIRECT,
                                                                           emberAfGetNodeId(),
                                                                           status);
    #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
  }
  return true;
}

#ifdef UC_BUILD

//Green Power Cluster Gp Notification
bool emberAfGreenPowerClusterGpNotificationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_green_power_cluster_gp_notification_command_t cmd_data;

  if (zcl_decode_green_power_cluster_gp_notification_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // If the green power server initialisation has not been called,
  // do not process a Gp Notification.
  if (!greenPowerServerInitialised) {
    return true;
  }
  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (cmd_data.gpdIeee  == NULL) {
    cmd_data.gpdIeee = nullEui64;
  }
  if (cmd_data.gpdCommandPayload == NULL) {
    // add a 0 length ZCL OCTATE string type
    cmd_data.gpdCommandPayload = zeroLengthZclOctateString;
  }

  EmberGpAddress gpdAddr;
  if (!emGpMakeAddr(&gpdAddr,
                    (cmd_data.options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID),
                    cmd_data.gpdSrcId,
                    cmd_data.gpdIeee,
                    cmd_data.gpdEndpoint)) {
    return true; // Handled, but dropped because of the bad GPD addressing
  }

  emberAfGreenPowerClusterPrintln("command %d", cmd_data.gpdCommandId);
  //if (cmd_data.gpdCommandPayload != NULL) { // Ensure gpdCommandPayload is not NULL to print the payload
  //  emberAfGreenPowerClusterPrint("payload: ");
  //  emberAfGreenPowerClusterPrintBuffer(&cmd_data.gpdCommandPayload[1],
  //                                      cmd_data.gpdCommandPayload[0],
  //                                      true);
  //  emberAfGreenPowerClusterPrintln("");
  //}

  // Find the sink entry and update the security frame counter from gpd
  uint8_t sinkIndex = emberGpSinkTableLookup(&gpdAddr);
  if (sinkIndex != 0xFF) {
    EmberGpSinkTableEntry entry = { 0 };
    EmberStatus status = emberGpSinkTableGetEntry(sinkIndex, &entry);
    // GPD Security validation, if fails drop!
    if (status == EMBER_SUCCESS) {
      uint8_t receivedSecLevel = ((cmd_data.options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL)
                                  >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET);
      uint8_t receivedKeyType = ((cmd_data.options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE)
                                 >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE_OFFSET);
      uint8_t sinkSecLevel = (entry.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);
      uint8_t sinkKeyType = ((entry.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE)
                             >> EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET);
      if (sinkSecLevel > 0) {
        if (sinkSecLevel > receivedSecLevel
            || sinkKeyType != receivedKeyType
            || entry.gpdSecurityFrameCounter > cmd_data.gpdSecurityFrameCounter) {
          // DROP
          emberAfGreenPowerClusterPrintln("Gp Notif : DROP - SecLevel, Key type or framecounter mismatch");
          return false;
        }
      }
    }
    emberGpSinkTableSetSecurityFrameCounter(sinkIndex, cmd_data.gpdSecurityFrameCounter);
  }
  if (cmd_data.gpdCommandId == EMBER_ZCL_GP_GPDF_DECOMMISSIONING) {
    decommissionGpd(((cmd_data.options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL) >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET),
                    ((cmd_data.options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE) >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE_OFFSET),
                    &gpdAddr,
                    true);
  } else if (cmd_data.gpdCommandId == EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST) {
    handleChannelRequest(cmd_data.options,
                         cmd_data.gppShortAddress,
                         ((cmd_data.options & EMBER_AF_GP_NOTIFICATION_OPTION_RX_AFTER_TX) ? true : false),
                         cmd_data.gpdCommandPayload);
  } else {
    // Call user first to give a chance to handle the notification.
    if (emberAfGreenPowerClusterGpNotificationForwardCallback(cmd_data.options,
                                                              &gpdAddr,
                                                              cmd_data.gpdSecurityFrameCounter,
                                                              cmd_data.gpdCommandId,
                                                              cmd_data.gpdCommandPayload,
                                                              cmd_data.gppShortAddress,
                                                              cmd_data.gppDistance)) {
      return true;
    }
    // Check if sink has the translation table support in the gpsFunctionality attribute?
    if (sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_TRANSLATION_TABLE)) {
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      emGpForwardGpdCommandBasedOnTranslationTable(&gpdAddr,
                                                   cmd_data.gpdCommandId,
                                                   cmd_data.gpdCommandPayload);
      #else
      emGpForwardGpdCommandDefault(&gpdAddr,
                                   cmd_data.gpdCommandId,
                                   cmd_data.gpdCommandPayload);
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
    }
  }
  return true;
}

//Green Power Cluster Gp Commissioning Notification
bool emberAfGreenPowerClusterGpCommissioningNotificationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_green_power_cluster_gp_commissioning_notification_command_t cmd_data;

  if (zcl_decode_green_power_cluster_gp_commissioning_notification_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (!(commissioningState.inCommissioningMode)) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN : Sink not in commissioning!");
    return true;
  }

  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (cmd_data.gpdIeee == NULL) {
    cmd_data.gpdIeee = nullEui64;
  }
  if (cmd_data.gpdCommandPayload == NULL) {
    // add a 0 length ZCL OCTATE string type. 1st byte holds the length of rest of the octate string.
    cmd_data.gpdCommandPayload = zeroLengthZclOctateString;
  }

  // Bad address with respect to application type - return back
  EmberGpAddress gpdAddr;
  if (!emGpMakeAddr(&gpdAddr,
                    (cmd_data.options & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_APPLICATION_ID),
                    cmd_data.gpdSrcId,
                    cmd_data.gpdIeee,
                    cmd_data.endpoint)) {
    return true;
  }

  if (gpdAddrZero(&gpdAddr)
      && (cmd_data.gpdCommandId != EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST)) {
    // Address 0 for all other GPDF commands except the channel request.
    return true;
  }
  GpCommDataSaved *commissioningGpd = emberAfGreenPowerServerFindCommissioningGpdInstance(&gpdAddr);
  // When the security processing failed sub-field is set, try validating. It can only be validated if there are
  // keys sent by GPD in a previous commissioning message which will be temporary collected in the commissioningGpd
  // instance.
  if (cmd_data.options & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_PROCESSING_FAILED) {
    if (commissioningGpd == NULL) {
      // In this case, the commissioning notification can not be processed because:
      // It is a protected message without earlier information of its security keys.
      return true;
    }
    // Attempt to process the secure GPDF using its earlier credentials
    if (commissioningGpd != NULL
        && !processCommNotificationsWithSecurityProcessingFailedFlag(cmd_data.options,
                                                                     &gpdAddr,
                                                                     cmd_data.gpdSecurityFrameCounter,
                                                                     &cmd_data.gpdCommandId,
                                                                     cmd_data.gpdCommandPayload,
                                                                     cmd_data.mic,
                                                                     &(commissioningGpd->key),
                                                                     commissioningGpd->securityKeyType)) {
      return true;
    }
  }

  int8_t rssi = 0;
  uint8_t linkQuality = 0;
  uint8_t gppDistance = 0;
  if (cmd_data.options & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX ) {
    gppDistance = cmd_data.gppLink;
  } else {
    gppGpdLinkUnpack(cmd_data.gppLink, &rssi, &linkQuality);
  }

  if (emberAfPluginGreenPowerServerGpdCommissioningNotificationCallback(cmd_data.gpdCommandId,
                                                                        cmd_data.options,
                                                                        &gpdAddr,
                                                                        cmd_data.gpdSecurityFrameCounter,
                                                                        cmd_data.gpdCommandId,
                                                                        cmd_data.gpdCommandPayload,
                                                                        cmd_data.gppShortAddress,
                                                                        rssi,
                                                                        linkQuality,
                                                                        gppDistance,
                                                                        cmd_data.mic)) {
    // User application handled
    return true;
  }
  // If the Application did not handle the notification through the above callback
  // The plugin handles it from here.
  if (cmd_data.gpdCommandId == EMBER_ZCL_GP_GPDF_DECOMMISSIONING) {
    gpCommissioningNotificationDecommissioningGpdf(cmd_data.options,
                                                   &gpdAddr);
  } else if (cmd_data.gpdCommandId == EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST) {
    handleChannelRequest(cmd_data.options,
                         cmd_data.gppShortAddress,
                         ((cmd_data.options & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX) ? true : false),
                         cmd_data.gpdCommandPayload);
  } else if (cmd_data.gpdCommandId == EMBER_ZCL_GP_GPDF_COMMISSIONING) {
    if (commissioningGpd == NULL) { // there was no previous temporary transaction, its a new GPD.
      commissioningGpd = allocateCommissioningGpdInstance(&gpdAddr); // Allocate one instance if available.
      if (commissioningGpd == NULL) {
        return true;
      }
    }
    gpCommissioningNotificationCommissioningGpdf(cmd_data.options,
                                                 &gpdAddr,
                                                 cmd_data.gpdSecurityFrameCounter,
                                                 cmd_data.gpdCommandId,
                                                 cmd_data.gpdCommandPayload,
                                                 cmd_data.gppShortAddress,
                                                 commissioningGpd);
  } else {
    // All other commands can only be processed if they have an instance else not.
    if (commissioningGpd == NULL) {
      return true;
    }
    if (cmd_data.gpdCommandId == EMBER_ZCL_GP_GPDF_APPLICATION_DESCRIPTION) {
      gpCommissioningNotificationApplicationDescriptionGpdf(cmd_data.options,
                                                            cmd_data.gpdSecurityFrameCounter,
                                                            cmd_data.gpdCommandPayload,
                                                            cmd_data.gppShortAddress,
                                                            commissioningGpd);
    } else {
      // Success or any other GPDF needs a GPD instance to be already present to proceed.
      // Any valid command that can be processed should behave as success
      gpCommissioningNotificationSuccessGpdf(cmd_data.options,
                                             cmd_data.gpdSecurityFrameCounter,
                                             commissioningGpd);
    }
  }
  return true;
}

// Commissioning Mode callback
bool emberAfGreenPowerClusterGpSinkCommissioningModeCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_green_power_cluster_gp_sink_commissioning_mode_command_t cmd_data;

  if (zcl_decode_green_power_cluster_gp_sink_commissioning_mode_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  return emAfPluginGreenPowerServerGpSinkCommissioningModeCommandHandler(cmd_data.options,
                                                                         cmd_data.gpmAddrForSecurity,
                                                                         cmd_data.gpmAddrForPairing,
                                                                         cmd_data.sinkEndpoint);
}

#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
bool emAfPluginGreenPowerClientGpProxyCommissioningModeCommandHandler(uint8_t options,
                                                                      uint16_t commissioningWindow,
                                                                      uint8_t channel,
                                                                      bool localCommandLoopback);
#endif // SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT

bool emberAfGreenPowerClusterGpPairingConfigurationCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_green_power_cluster_gp_pairing_configuration_command_t cmd_data;

  if (zcl_decode_green_power_cluster_gp_pairing_configuration_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  if (cmd->source == emberGetNodeId()) {
    // Silent Drop : Loopback message - return true to ensure no action from the framework.
    return true;
  }
  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (cmd_data.gpdIeee  == NULL) {
    cmd_data.gpdIeee = nullEui64;
  }
  uint8_t gpdAppId = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_APPLICATION_ID);
  EmberGpAddress gpdAddr;
  if (!emGpMakeAddr(&gpdAddr, gpdAppId, cmd_data.gpdSrcId, cmd_data.gpdIeee, cmd_data.endpoint)) {
    return true; // Silent Drop : Address not valid.
  }
  // Silent Drop for the gpd addr = 0
  if (gpdAddrZero(&gpdAddr)) {
    emberAfGreenPowerClusterPrintln("DROP - GP Pairing Config : GPD Address is 0!");
    return true;
  }
  if ((cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE)
      && ((cmd_data.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL) == EMBER_GP_SECURITY_LEVEL_RESERVED)) {
    emberAfGreenPowerClusterPrintln("DROP - GP Pairing Config : Security Level reserved !");
    return true;
  }

  uint8_t gpConfigAtion = (cmd_data.actions & EMBER_AF_GP_PAIRING_CONFIGURATION_ACTIONS_ACTION);

  // Action 0b000 (No Action)
  // Input(s) - SendGpPairingBit , Gpd Address
  if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_NO_ACTION) {
    if (cmd_data.actions & EMBER_AF_GP_PAIRING_CONFIGURATION_ACTIONS_SEND_GP_PAIRING) {
      sendGpPairingLookingUpAddressInSinkEntry(&gpdAddr);
    }
    return true;
  }
  // Action = 0b100 (Remove GPD)
  // Input(s) - Gpd Address
  if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REMOVE_GPD) {
    decommissionGpd(0, 0, &gpdAddr, true);
    return true;
  }
  // Action = 0b011 (Remove Pairing)
  // Input(s) - Gpd Address, Communication Mode, GroupList, Number Of Paired Eps , Paired Ep List
  //            Application Information, Switch Information, Additional Block Information
  if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REMOVE_A_PAIRING) {
    if (((gpdAddr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID)
         && gpdAddr.id.sourceId == GP_ADDR_SRC_ID_WILDCARD)
        || ((gpdAddr.applicationId == EMBER_GP_APPLICATION_IEEE_ADDRESS)
            && emberAfMemoryByteCompare(gpdAddr.id.gpdIeeeAddress, EUI64_SIZE, 0xFF))) {
      // TODO: apply action to all GPD with this particular applicationID (SrcId or IEEE)
      return true;
    }
    if (emberGpSinkTableLookup(&gpdAddr) != 0xFF) {
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      // Search and delete the GPD Id if it matches provided information
      uint8_t outIndex = GP_TRANSLATION_TABLE_ENTRY_INVALID_INDEX;
      uint8_t status = emGpTransTableFindMatchingTranslationTableEntry(GP_TRANSLATION_TABLE_SCAN_LEVEL_GPD_ID, //levelOfScan
                                                                       false, //infoBlockPresent
                                                                       &gpdAddr, //gpAddr
                                                                       0,  // gpdCommandId
                                                                       0,  // zbEndpoint
                                                                       NULL, //gpdCmdPayload
                                                                       NULL, // addInfoBlock
                                                                       &outIndex,
                                                                       0);
      if (status == GP_TRANSLATION_TABLE_STATUS_SUCCESS) {
        decommissionGpd(0, 0, &gpdAddr, false);
      }
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
    }
    return true;
  }
  // All other command actions would need a temporary storage
  GpCommDataSaved *commissioningGpd = findOrAllocateCommissioningGpdInstance(&gpdAddr);
  if (commissioningGpd == NULL) {
    return true;
  }
  MEMCOPY(&(commissioningGpd->addr), &gpdAddr, sizeof(EmberGpAddress));

  uint8_t gpPairingConfigCommunicationMode = (cmd_data.options
                                              & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_COMMUNICATION_MODE)
                                             >> EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_COMMUNICATION_MODE_OFFSET;
  bool appInformationPresent = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_APPLICATION_INFORMATION_PRESENT)
                               ? true : false;
  if (appInformationPresent) { // application info present
    commissioningGpd->applicationInfo.applInfoBitmap = cmd_data.applicationInformation;
    if (cmd_data.applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_MANUFACTURE_ID_PRESENT) { // ManufactureId Present
      commissioningGpd->applicationInfo.manufacturerId = cmd_data.manufacturerId;
    }
    if (cmd_data.applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_MODEL_ID_PRESENT) { // ModelId Present
      commissioningGpd->applicationInfo.modelId = cmd_data.modeId;
    }
    if (cmd_data.applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_COMMANDS_PRESENT) {   // gpd Commands present
      commissioningGpd->applicationInfo.numberOfGpdCommands = cmd_data.numberOfGpdCommands;
      MEMCOPY(commissioningGpd->applicationInfo.gpdCommands,
              cmd_data.gpdCommandIdList,
              cmd_data.numberOfGpdCommands);
    }
    if (cmd_data.applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_CLUSTER_LIST_PRESENT) { // Cluster List present
      commissioningGpd->applicationInfo.numberOfGpdClientCluster = cmd_data.clusterIdListCount >> 4;
      MEMCOPY(commissioningGpd->applicationInfo.clientClusters,
              cmd_data.clusterListClient,
              (commissioningGpd->applicationInfo.numberOfGpdClientCluster * 2));
      commissioningGpd->applicationInfo.numberOfGpdServerCluster = cmd_data.clusterIdListCount & 0x0f;
      MEMCOPY(commissioningGpd->applicationInfo.serverClusters,
              cmd_data.clusterListServer,
              (commissioningGpd->applicationInfo.numberOfGpdServerCluster * 2));
    }
    if (cmd_data.applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_PRESENT) { // Switch Information
      commissioningGpd->switchInformationStruct.switchInfoLength = cmd_data.switchInformationLength;
      commissioningGpd->switchInformationStruct.nbOfContacts = (cmd_data.switchConfiguration
                                                                & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_NB_OF_CONTACT);
      commissioningGpd->switchInformationStruct.switchType = (cmd_data.switchConfiguration
                                                              & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE)
                                                             >> EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE_OFFSET;
      commissioningGpd->switchInformationStruct.currentContact = cmd_data.currentContactStatus;

      // Start the generic switch timeout
      uint32_t delay = EMBER_AF_PLUGIN_GREEN_POWER_SERVER_GENERIC_SWITCH_COMMISSIONING_TIMEOUT_IN_SEC;
      slxu_zigbee_event_set_delay_ms(genericSwitchCommissioningTimeout,
                                     delay * MILLISECOND_TICKS_PER_SECOND);
    }
  }

  bool assignedAllias = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_ASSIGNED_ALIAS) ? true : false;
  if (assignedAllias) {
    commissioningGpd->useGivenAssignedAlias = true;
    commissioningGpd->givenAlias = cmd_data.gpdAssignedAlias;
  }

  // Always present in the GpPairingConfig payload
  commissioningGpd->applicationInfo.deviceId = cmd_data.deviceId;
  commissioningGpd->applicationInfo.numberOfPairedEndpoints = cmd_data.numberOfPairedEndpoints;
  if (cmd_data.numberOfPairedEndpoints < 0xFD) {
    MEMCOPY(commissioningGpd->applicationInfo.pairedEndpoints,
            cmd_data.pairedEndpoints,
            cmd_data.numberOfPairedEndpoints);
  }

  bool securityUse = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE) ? true : false;
  if (securityUse) {
    uint8_t gpdfExtendedOptions = 0;
    commissioningGpd->securityLevel = (cmd_data.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);
    commissioningGpd->securityKeyType = (cmd_data.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE)
                                        >> EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET;
    gpdfExtendedOptions = commissioningGpd->securityLevel
                          | (commissioningGpd->securityKeyType << EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET);
    if (cmd_data.gpdSecurityKey) {
      gpdfExtendedOptions |= EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT;
      gpdfExtendedOptions |= ((commissioningGpd->securityLevel == 3)
                              ? EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION : 0);
      MEMCOPY(commissioningGpd->key.contents, cmd_data.gpdSecurityKey, EMBER_ENCRYPTION_KEY_SIZE);
    }
    commissioningGpd->gpdfExtendedOptions |= gpdfExtendedOptions;
  }

  bool seqNumberCapability = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SEQUENCE_NUMBER_CAPABILITIES) ? true : false;
  if (securityUse
      || (!securityUse
          && seqNumberCapability)) {
    commissioningGpd->outgoingFrameCounter = cmd_data.gpdSecurityFrameCounter; // save/update the frame counter
  }

  bool rxOnCapability = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_RX_ON_CAPABILITY) ? true : false;
  bool fixedLocation = (cmd_data.options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_FIXED_LOCATION) ? true : false;
  uint8_t gpdfOptions = (seqNumberCapability ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_MAC_SEQ_NUM_CAP : 0)
                        | (rxOnCapability ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_RX_ON_CAP : 0)
                        | (appInformationPresent ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_APPLICATION_INFORMATION_PRESENT : 0)
                        | (fixedLocation ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_FIXED_LOCATION : 0)
                        | (securityUse ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_EXTENDED_OPTIONS_FIELD : 0);
  commissioningGpd->gpdfOptions |= gpdfOptions;
  commissioningGpd->groupcastRadius = cmd_data.groupcastRadius;
  uint8_t gpPairingConfigSecurityLevel = (cmd_data.securityOptions
                                          & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);

  uint8_t gpsSecurityLevelAttribute = 0;
  EmberAfStatus UNUSED status;
  EmberAfAttributeType type;
  status = emberAfReadAttribute(GP_ENDPOINT,
                                ZCL_GREEN_POWER_CLUSTER_ID,
                                ZCL_GP_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_ID,
                                (CLUSTER_MASK_SERVER),
                                (uint8_t*)&gpsSecurityLevelAttribute,
                                sizeof(uint8_t),
                                &type);

  gpsSecurityLevelAttribute = gpsSecurityLevelAttribute & 0x03; // mask the encryption part
  if ((cmd_data.groupListCount == 0xFF || cmd_data.groupList == NULL)
      || (sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_SINK_TABLE_BASED_GROUPCAST_FORWARDING)
          || sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_PRE_COMMISSIONED_GROUPCAST_COMMUNICATION))) {
    if (cmd->type != EMBER_INCOMING_BROADCAST
        && gpPairingConfigSecurityLevel == EMBER_GP_SECURITY_LEVEL_RESERVED) {
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_FAILURE);
      return true;
    }
    // Action = 0b001
    // Action = 0b010
    if ((gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_EXTEND_SINK_TABLE_ENTRY
         || gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REPLACE_SINK_TABLE_ENTRY)
        && (gpPairingConfigSecurityLevel != EMBER_GP_SECURITY_LEVEL_RESERVED
            && (gpPairingConfigSecurityLevel >= gpsSecurityLevelAttribute
                && emGpCheckCommunicationModeSupport(gpPairingConfigCommunicationMode)))) {
      emberAfGreenPowerClusterPrintln("Replace or Extend Sink");
      uint8_t sinkEntryIndex = emberGpSinkTableLookup(&gpdAddr);
      if (sinkEntryIndex != 0xFF) {
        #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
        // To delete custom Translation table entry for this GPD
        emGpTransTableDeletePairedDevicefromTranslationTableEntry(&gpdAddr);
        #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
        if (emberAfPluginGreenPowerServerSinkTableAccessNotificationCallback(&gpdAddr,
                                                                             GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_REMOVE_GPD)) {
          emberGpSinkTableRemoveEntry(sinkEntryIndex);
        }
      }
      commissioningGpd->communicationMode = gpPairingConfigCommunicationMode;
      // Set/reset the doNotSendGpPairing flag to be used when finaliing paring, Note : the negation.
      commissioningGpd->doNotSendGpPairing = ((cmd_data.actions & EMBER_AF_GP_PAIRING_CONFIGURATION_ACTIONS_SEND_GP_PAIRING)
                                              ? false : true);
      if ((commissioningGpd->applicationInfo.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_APPLICATION_DESCRIPTION_COMMAND_FOLLOWS)) {
        if (!slxu_zigbee_event_is_active(multiSensorCommissioningTimeout)) {
          emberAfGreenPowerClusterPrintln("Gp PairingConfig : Reseting Partial data - Only Report Data");
          resetOfMultisensorDataSaved(false, commissioningGpd);
        }
        emberAfGreenPowerClusterPrintln("Gp PairingConfig : Start MS/CAR Commissioning Timeout and waiting for ADCF");
        uint32_t delay = EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_TIMEOUT_IN_SEC;
        slxu_zigbee_event_set_delay_ms(multiSensorCommissioningTimeout,
                                       delay * MILLISECOND_TICKS_PER_SECOND);
        // All set to collect the report descriptors
        commissioningGpd->commissionState = GP_SINK_COMM_STATE_COLLECT_REPORTS;
        return true;
      }
      // Replace or Extend Sink with new application information, then create TT entries as well - if the application description is not following
      // Makes proxy on this node enter commissioning so that it can creat a proxy entry when Gp Pairing is heard
      if (!(commissioningState.inCommissioningMode)) {
        // The sink is not in commissioning mode for the CT based commissioning.
        // The endpoints are supplied by the CT, hence the sink need to match all the endpoints on it.
        commissioningState.endpoint = GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS;
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
        emAfPluginGreenPowerClientGpProxyCommissioningModeCommandHandler((EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_ACTION // Enter Commissioning
                                                                          | EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS), // Exit on First pairing
                                                                         0,   // No Commissioning window
                                                                         0,   // No channel present
                                                                         true);
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
      }
      commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
      finalisePairing(commissioningGpd);
      return true;
    }
    // Action = 0b101
    if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_APPLICATION_DESCRIPTION) {
      // if total number of report is 0 then it is the first gp pairing
      if (commissioningGpd->totalNbOfReport == 0) {
        // first sensor to follow, reset structure for the report descriptors
        resetOfMultisensorDataSaved(false, commissioningGpd);
        MEMCOPY(&commissioningGpd->addr, &gpdAddr, sizeof(EmberGpAddress));
        commissioningGpd->totalNbOfReport = cmd_data.totalNumberOfReports;
        commissioningGpd->commissionState = GP_SINK_COMM_STATE_COLLECT_REPORTS;
      }
      // if the timer is running then process else just drop
      if (!slxu_zigbee_event_is_active(multiSensorCommissioningTimeout)) {
        emberAfGreenPowerClusterPrintln("Gp PairingConfig App Description - DROP : MS timer has expired");
        return false;
      }
      if (commissioningGpd->commissionState != GP_SINK_COMM_STATE_COLLECT_REPORTS) {
        // not in right state
        return false;
      }
      if (!saveReportDescriptor(cmd_data.totalNumberOfReports,
                                cmd_data.numberOfReports,
                                cmd_data.reportDescriptor,
                                commissioningGpd)) {
        // error in the application desc payload
        return false;
      }
      if (commissioningGpd->numberOfReports != commissioningGpd->totalNbOfReport) {
        // still to collect more reports
        return true;
      }
      // Report collection is over - finalise the pairing
      commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
      finalisePairing(commissioningGpd);
      return true;
    }
  }
  return true;
}

bool emberAfGreenPowerClusterGpSinkTableRequestCallback(EmberAfClusterCommand *cmd)
{
  sl_zcl_green_power_cluster_gp_sink_table_request_command_t cmd_data;

  if (zcl_decode_green_power_cluster_gp_sink_table_request_command(cmd, &cmd_data)
      != EMBER_ZCL_STATUS_SUCCESS) {
    return false;
  }

  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (cmd_data.gpdIeee  == NULL) {
    cmd_data.gpdIeee = nullEui64;
  }
  uint8_t entryIndex = 0;
  uint8_t appId = (cmd_data.options & EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_APPLICATION_ID);
  uint8_t requestType = (cmd_data.options & EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TYPE)
                        >> EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TYPE_OFFSET;
  EmberGpSinkTableEntry entry;

  emberAfGreenPowerClusterPrintln("Got sink table request with options %1x and index %1x", cmd_data.options, cmd_data.index);
  // only respond to unicast messages.
  if (cmd->type != EMBER_INCOMING_UNICAST) {
    emberAfGreenPowerClusterPrintln("Not unicast");
    goto kickout;
  }

  // the device SHALL check if it implements a Sink Table.
  // If not, it SHALL generate a ZCL Default Response command,
  // with the Status code field carrying UNSUP_COMMAND, subject to the rules as specified in sec. 2.4.12 of [3]
  if (EMBER_GP_SINK_TABLE_SIZE == 0) {
    emberAfGreenPowerClusterPrintln("Unsup cluster command");
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_UNSUP_COMMAND);
    return true;
  }

  if (emberAfCurrentEndpoint() != GP_ENDPOINT) {
    emberAfGreenPowerClusterPrintln("Drop frame due to unknown endpoint: %X", emberAfCurrentEndpoint());
    return false;
  }

  uint8_t validEntriesCount = gpNumberOfSinkEntriesInSinkTable();

  // If its Sink Table is empty, and the triggering GP Sink Table Request was received in unicast,
  // then the GP Sink Table Response SHALL be sent with Status NOT_FOUND,
  // Total number of non-empty Sink Table entries carrying 0x00,
  // Start index carrying 0xFF (in case of request by GPD ID) or
  // the Index value from the triggering GP Sink Table Request (in case of request by index),
  // Entries count field set to 0x00, and any Sink Table entry fields absent
  if (validEntriesCount == 0) {
    // "index" is already 0xFF if search by ID
    // or already set to the value from "SinkTableRequest" triggered frame in case it is search by INDEX
    emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_NOT_FOUND,
                                                                0,
                                                                cmd_data.index,
                                                                0);
    emberAfSendResponse();
    return true;
  } else {
    // Valid Entries are present!
    if (requestType == EMBER_ZCL_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TABLE_ENTRIES_BY_GPD_ID) {
      EmberGpAddress gpdAddr;
      emGpMakeAddr(&gpdAddr, appId, cmd_data.gpdSrcId, cmd_data.gpdIeee, cmd_data.endpoint);
      entryIndex = emberGpSinkTableLookup(&gpdAddr);
      if (entryIndex == 0xFF) {
        // Valid entries present but none for this gpdAddr - Send NOT FOUND sesponse.
        emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_NOT_FOUND,
                                                                    validEntriesCount,
                                                                    entryIndex,
                                                                    0);
        emberAfSendResponse();
        goto kickout;
      } else {
        // A valid entry with the ID is present
        if (emberGpSinkTableGetEntry(entryIndex, &entry) == EMBER_SUCCESS) {
          // If the triggering GP Sink Table Request command contained a GPD ID field, the device SHALL check
          // if it has a Sink Table entry for this GPD ID (and Endpoint, if ApplicationID = 0b010). If yes, the device
          // SHALL create a GP Sink Table Response with Status SUCCESS, Total number of non-empty Sink Table
          // entries carrying the total number of non-empty Sink Table entries on this device, Start index set to
          // 0xff, Entries count field set to 0x01, and one Sink Table entry field for the requested GPD ID (and
          // Endpoint, if ApplicationID = 0b010), formatted as specified in sec. A.3.3.2.2.1, present.
          emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_SUCCESS,
                                                                      validEntriesCount,
                                                                      0xff,
                                                                      1);
          appResponseLength += storeSinkTableEntryInBuffer(&entry, (appResponseData + appResponseLength));
          emberAfSendResponse();
        } else {
          // Not found status to go out.
        }
        goto kickout;
      }
    } else if (requestType == EMBER_ZCL_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TABLE_ENTRIES_BY_INDEX) {
      if (cmd_data.index >= validEntriesCount) {
        emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_NOT_FOUND,
                                                                    validEntriesCount,
                                                                    cmd_data.index,
                                                                    0);
        emberAfSendResponse();
        return true;
      } else {
        // return the sink table entry content into the reponse payload from indicated
        // index and nexts until these are consistant (adress type, etc) and
        // as long as it feet into one message
        emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_SUCCESS,
                                                                    validEntriesCount,
                                                                    cmd_data.index,
                                                                    0xff);
        validEntriesCount = 0;
        uint16_t entriesCount = 0;
        for (entryIndex = 0; entryIndex < EMBER_GP_SINK_TABLE_SIZE; entryIndex++) {
          if (emberGpSinkTableGetEntry(entryIndex, &entry) != EMBER_SUCCESS) {
            continue;
          }

          uint8_t  tempDatabuffer[EMBER_AF_RESPONSE_BUFFER_LEN];
          uint16_t tempDataLength = 0;

          if (entry.status != EMBER_GP_SINK_TABLE_ENTRY_STATUS_UNUSED
              && entry.status != 0) {
            validEntriesCount++;
            if (validEntriesCount > cmd_data.index) {
              // Copy to a temp buffer and add if there is space
              tempDataLength = storeSinkTableEntryInBuffer(&entry, tempDatabuffer);
              // If space add to buffer
              if (sizeof(appResponseData) > (appResponseLength + tempDataLength)) {
                MEMMOVE(&appResponseData[appResponseLength], tempDatabuffer, tempDataLength);
                appResponseLength +=  tempDataLength;
                entriesCount++;
              } else {
                break;
              }
            }
          }
        }
        //Insert the number of entries actually included
        appResponseData[GP_SINK_TABLE_RESPONSE_ENTRIES_OFFSET + GP_NON_MANUFACTURER_ZCL_HEADER_LENGTH] = entriesCount;
        EmberStatus status = emberAfSendResponse();
        if (status == EMBER_MESSAGE_TOO_LONG) {
          emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_SUCCESS,
                                                                      validEntriesCount,
                                                                      cmd_data.index,
                                                                      0);
          emberAfSendResponse();
        }
        goto kickout;
      }
    } else {
      // nothing, other value of requestType are reserved
    }
  }
  kickout: return true;
}

#else // !UC_BUILD

//Green Power Cluster Gp Notification
bool emberAfGreenPowerClusterGpNotificationCallback(uint16_t options,
                                                    uint32_t gpdSrcId,
                                                    uint8_t *gpdIeee,
                                                    uint8_t  gpdEndpoint,
                                                    uint32_t gpdSecurityFrameCounter,
                                                    uint8_t  gpdCommandId,
                                                    uint8_t *gpdCommandPayload,
                                                    uint16_t gppShortAddress,
                                                    uint8_t  gppDistance)
{
  // If the green power server initialisation has not been called,
  // do not process a Gp Notification.
  if (!greenPowerServerInitialised) {
    return true;
  }
  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (gpdIeee  == NULL) {
    gpdIeee = nullEui64;
  }
  if (gpdCommandPayload == NULL) {
    // add a 0 length ZCL OCTATE string type
    gpdCommandPayload = zeroLengthZclOctateString;
  }

  EmberGpAddress gpdAddr;
  if (!emGpMakeAddr(&gpdAddr,
                    (options & EMBER_AF_GP_NOTIFICATION_OPTION_APPLICATION_ID),
                    gpdSrcId,
                    gpdIeee,
                    gpdEndpoint)) {
    return true; // Handled, but dropped because of the bad GPD addressing
  }

  emberAfGreenPowerClusterPrintln("command %d", gpdCommandId);
  //if (gpdCommandPayload != NULL) { // Ensure gpdCommandPayload is not NULL to print the payload
  //  emberAfGreenPowerClusterPrint("payload: ");
  //  emberAfGreenPowerClusterPrintBuffer(&gpdCommandPayload[1],
  //                                      gpdCommandPayload[0],
  //                                      true);
  //  emberAfGreenPowerClusterPrintln("");
  //}

  // Find the sink entry and update the security frame counter from gpd
  uint8_t sinkIndex = emberGpSinkTableLookup(&gpdAddr);
  if (sinkIndex != 0xFF) {
    EmberGpSinkTableEntry entry = { 0 };
    EmberStatus status = emberGpSinkTableGetEntry(sinkIndex, &entry);
    // GPD Security validation, if fails drop!
    if (status == EMBER_SUCCESS) {
      uint8_t receivedSecLevel = ((options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL)
                                  >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET);
      uint8_t receivedKeyType = ((options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE)
                                 >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE_OFFSET);
      uint8_t sinkSecLevel = (entry.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);
      uint8_t sinkKeyType = ((entry.securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE)
                             >> EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET);
      if (sinkSecLevel > 0) {
        if (sinkSecLevel > receivedSecLevel
            || sinkKeyType != receivedKeyType
            || entry.gpdSecurityFrameCounter > gpdSecurityFrameCounter) {
          // DROP
          emberAfGreenPowerClusterPrintln("Gp Notif : DROP - SecLevel, Key type or framecounter mismatch");
          return false;
        }
      }
    }
    emberGpSinkTableSetSecurityFrameCounter(sinkIndex, gpdSecurityFrameCounter);
  }
  if (gpdCommandId == EMBER_ZCL_GP_GPDF_DECOMMISSIONING) {
    decommissionGpd(((options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL) >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_LEVEL_OFFSET),
                    ((options & EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE) >> EMBER_AF_GP_NOTIFICATION_OPTION_SECURITY_KEY_TYPE_OFFSET),
                    &gpdAddr,
                    true);
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST) {
    handleChannelRequest(options,
                         gppShortAddress,
                         ((options & EMBER_AF_GP_NOTIFICATION_OPTION_RX_AFTER_TX) ? true : false),
                         gpdCommandPayload);
  } else {
    // Call user first to give a chance to handle the notification.
    if (emberAfGreenPowerClusterGpNotificationForwardCallback(options,
                                                              &gpdAddr,
                                                              gpdSecurityFrameCounter,
                                                              gpdCommandId,
                                                              gpdCommandPayload,
                                                              gppShortAddress,
                                                              gppDistance)) {
      return true;
    }
    // Check if sink has the translation table support in the gpsFunctionality attribute?
    if (sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_TRANSLATION_TABLE)) {
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      emGpForwardGpdCommandBasedOnTranslationTable(&gpdAddr,
                                                   gpdCommandId,
                                                   gpdCommandPayload);
      #else
      emGpForwardGpdCommandDefault(&gpdAddr,
                                   gpdCommandId,
                                   gpdCommandPayload);
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
    }
  }
  return true;
}

//Green Power Cluster Gp Commissioning Notification
bool emberAfGreenPowerClusterGpCommissioningNotificationCallback(uint16_t commNotificationOptions,
                                                                 uint32_t gpdSrcId,
                                                                 uint8_t *gpdIeee,
                                                                 uint8_t gpdEndpoint,
                                                                 uint32_t gpdSecurityFrameCounter,
                                                                 uint8_t gpdCommandId,
                                                                 uint8_t *gpdCommandPayload,
                                                                 uint16_t gppShortAddress,
                                                                 uint8_t gppLink,
                                                                 uint32_t commissioningNotificationMic)
{
  if (!(commissioningState.inCommissioningMode)) {
    emberAfGreenPowerClusterPrintln("DROP - GP CN : Sink not in commissioning!");
    return true;
  }

  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (gpdIeee == NULL) {
    gpdIeee = nullEui64;
  }
  if (gpdCommandPayload == NULL) {
    // add a 0 length ZCL OCTATE string type. 1st byte holds the length of rest of the octate string.
    gpdCommandPayload = zeroLengthZclOctateString;
  }

  // Bad address with respect to application type - return back
  EmberGpAddress gpdAddr;
  if (!emGpMakeAddr(&gpdAddr,
                    (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_APPLICATION_ID),
                    gpdSrcId,
                    gpdIeee,
                    gpdEndpoint)) {
    return true;
  }

  if (gpdAddrZero(&gpdAddr)
      && (gpdCommandId != EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST)) {
    // Address 0 for all other GPDF commands except the channel request.
    return true;
  }
  GpCommDataSaved *commissioningGpd = emberAfGreenPowerServerFindCommissioningGpdInstance(&gpdAddr);
  // When the security processing failed sub-field is set, try validating. It can only be validated if there are
  // keys sent by GPD in a previous commissioning message which will be temporary collected in the commissioningGpd
  // instance.
  if (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_SECURITY_PROCESSING_FAILED) {
    if (commissioningGpd == NULL) {
      // In this case, the commissioning notification can not be processed because:
      // It is a protected message without earlier information of its security keys.
      return true;
    }
    // Attempt to process the secure GPDF using its earlier credentials
    if (commissioningGpd != NULL
        && !processCommNotificationsWithSecurityProcessingFailedFlag(commNotificationOptions,
                                                                     &gpdAddr,
                                                                     gpdSecurityFrameCounter,
                                                                     &gpdCommandId,
                                                                     gpdCommandPayload,
                                                                     commissioningNotificationMic,
                                                                     &(commissioningGpd->key),
                                                                     commissioningGpd->securityKeyType)) {
      return true;
    }
  }

  int8_t rssi = 0;
  uint8_t linkQuality = 0;
  uint8_t gppDistance = 0;
  if (commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX ) {
    gppDistance = gppLink;
  } else {
    gppGpdLinkUnpack(gppLink, &rssi, &linkQuality);
  }

  if (emberAfPluginGreenPowerServerGpdCommissioningNotificationCallback(gpdCommandId,
                                                                        commNotificationOptions,
                                                                        &gpdAddr,
                                                                        gpdSecurityFrameCounter,
                                                                        gpdCommandId,
                                                                        gpdCommandPayload,
                                                                        gppShortAddress,
                                                                        rssi,
                                                                        linkQuality,
                                                                        gppDistance,
                                                                        commissioningNotificationMic)) {
    // User application handled
    return true;
  }
  // If the Application did not handle the notification through the above callback
  // The plugin handles it from here.
  if (gpdCommandId == EMBER_ZCL_GP_GPDF_DECOMMISSIONING) {
    gpCommissioningNotificationDecommissioningGpdf(commNotificationOptions,
                                                   &gpdAddr);
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_CHANNEL_REQUEST) {
    handleChannelRequest(commNotificationOptions,
                         gppShortAddress,
                         ((commNotificationOptions & EMBER_AF_GP_COMMISSIONING_NOTIFICATION_OPTION_RX_AFTER_TX) ? true : false),
                         gpdCommandPayload);
  } else if (gpdCommandId == EMBER_ZCL_GP_GPDF_COMMISSIONING) {
    if (commissioningGpd == NULL) { // there was no previous temporary transaction, its a new GPD.
      commissioningGpd = allocateCommissioningGpdInstance(&gpdAddr); // Allocate one instance if available.
      if (commissioningGpd == NULL) {
        return true;
      }
    }
    gpCommissioningNotificationCommissioningGpdf(commNotificationOptions,
                                                 &gpdAddr,
                                                 gpdSecurityFrameCounter,
                                                 gpdCommandId,
                                                 gpdCommandPayload,
                                                 gppShortAddress,
                                                 commissioningGpd);
  } else {
    // All other commands can only be processed if they have an instance else not.
    if (commissioningGpd == NULL) {
      return true;
    }
    if (gpdCommandId == EMBER_ZCL_GP_GPDF_APPLICATION_DESCRIPTION) {
      gpCommissioningNotificationApplicationDescriptionGpdf(commNotificationOptions,
                                                            gpdSecurityFrameCounter,
                                                            gpdCommandPayload,
                                                            gppShortAddress,
                                                            commissioningGpd);
    } else {
      // Success or any other GPDF needs a GPD instance to be already present to proceed.
      // Any valid command that can be processed should behave as success
      gpCommissioningNotificationSuccessGpdf(commNotificationOptions,
                                             gpdSecurityFrameCounter,
                                             commissioningGpd);
    }
  }
  return true;
}

// Commissioning Mode callback
bool emberAfGreenPowerClusterGpSinkCommissioningModeCallback(uint8_t options,
                                                             uint16_t gpmAddrForSecurity,
                                                             uint16_t gpmAddrForPairing,
                                                             uint8_t sinkEndpoint)
{
  return emAfPluginGreenPowerServerGpSinkCommissioningModeCommandHandler(options,
                                                                         gpmAddrForSecurity,
                                                                         gpmAddrForPairing,
                                                                         sinkEndpoint);
}

bool emberAfGreenPowerClusterGpPairingConfigurationCallback(uint8_t actions,
                                                            uint16_t options,
                                                            uint32_t gpdSrcId,
                                                            uint8_t *gpdIeee,
                                                            uint8_t gpdEndpoint,
                                                            uint8_t deviceId,
                                                            uint8_t groupListCount,
                                                            uint8_t *groupList,
                                                            uint16_t gpdAssignedAlias,
                                                            uint8_t groupcastRadius,
                                                            uint8_t securityOptions,
                                                            uint32_t gpdSecurityFrameCounter,
                                                            uint8_t *gpdSecurityKey,
                                                            uint8_t numberOfPairedEndpoints,
                                                            uint8_t *pairedEndpoints,
                                                            uint8_t applicationInformation,
                                                            uint16_t manufacturerId,
                                                            uint16_t modeId,
                                                            uint8_t numberOfGpdCommands,
                                                            uint8_t *gpdCommandIdList,
                                                            uint8_t clusterIdListCount,
                                                            uint8_t *clusterListServer,
                                                            uint8_t *clusterListClient,
                                                            uint8_t switchInformationLength,
                                                            uint8_t genericSwitchConfiguration,
                                                            uint8_t currentContactStatus,
                                                            uint8_t totalNumberOfReports,
                                                            uint8_t numberOfReports,
                                                            uint8_t *reportDescriptor)
{
  EmberAfClusterCommand *cmd = emberAfCurrentCommand();
  if (cmd->source == emberGetNodeId()) {
    // Silent Drop : Loopback message - return true to ensure no action from the framework.
    return true;
  }
  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (gpdIeee  == NULL) {
    gpdIeee = nullEui64;
  }
  uint8_t gpdAppId = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_APPLICATION_ID);
  EmberGpAddress gpdAddr;
  if (!emGpMakeAddr(&gpdAddr, gpdAppId, gpdSrcId, gpdIeee, gpdEndpoint)) {
    return true; // Silent Drop : Address not valid.
  }
  // Silent Drop for the gpd addr = 0
  if (gpdAddrZero(&gpdAddr)) {
    emberAfGreenPowerClusterPrintln("DROP - GP Pairing Config : GPD Address is 0!");
    return true;
  }
  if ((options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE)
      && ((securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL) == EMBER_GP_SECURITY_LEVEL_RESERVED)) {
    emberAfGreenPowerClusterPrintln("DROP - GP Pairing Config : Security Level reserved !");
    return true;
  }

  uint8_t gpConfigAtion = (actions & EMBER_AF_GP_PAIRING_CONFIGURATION_ACTIONS_ACTION);

  // Action 0b000 (No Action)
  // Input(s) - SendGpPairingBit , Gpd Address
  if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_NO_ACTION) {
    if (actions & EMBER_AF_GP_PAIRING_CONFIGURATION_ACTIONS_SEND_GP_PAIRING) {
      sendGpPairingLookingUpAddressInSinkEntry(&gpdAddr);
    }
    return true;
  }
  // Action = 0b100 (Remove GPD)
  // Input(s) - Gpd Address
  if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REMOVE_GPD) {
    decommissionGpd(0, 0, &gpdAddr, true);
    return true;
  }
  // Action = 0b011 (Remove Pairing)
  // Input(s) - Gpd Address, Communication Mode, GroupList, Number Of Paired Eps , Paired Ep List
  //            Application Information, Switch Information, Additional Block Information
  if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REMOVE_A_PAIRING) {
    if (((gpdAddr.applicationId == EMBER_GP_APPLICATION_SOURCE_ID)
         && gpdAddr.id.sourceId == GP_ADDR_SRC_ID_WILDCARD)
        || ((gpdAddr.applicationId == EMBER_GP_APPLICATION_IEEE_ADDRESS)
            && emberAfMemoryByteCompare(gpdAddr.id.gpdIeeeAddress, EUI64_SIZE, 0xFF))) {
      // TODO: apply action to all GPD with this particular applicationID (SrcId or IEEE)
      return true;
    }
    if (emberGpSinkTableLookup(&gpdAddr) != 0xFF) {
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      // Search and delete the GPD Id if it matches provided information
      uint8_t outIndex = GP_TRANSLATION_TABLE_ENTRY_INVALID_INDEX;
      uint8_t status = emGpTransTableFindMatchingTranslationTableEntry(GP_TRANSLATION_TABLE_SCAN_LEVEL_GPD_ID, //levelOfScan
                                                                       false, //infoBlockPresent
                                                                       &gpdAddr, //gpAddr
                                                                       0,  // gpdCommandId
                                                                       0,  // zbEndpoint
                                                                       NULL, //gpdCmdPayload
                                                                       NULL, // addInfoBlock
                                                                       &outIndex,
                                                                       0);
      if (status == GP_TRANSLATION_TABLE_STATUS_SUCCESS) {
        decommissionGpd(0, 0, &gpdAddr, false);
      }
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
    }
    return true;
  }
  // All other command actions would need a temporary storage
  GpCommDataSaved *commissioningGpd = findOrAllocateCommissioningGpdInstance(&gpdAddr);
  if (commissioningGpd == NULL) {
    return true;
  }
  MEMCOPY(&(commissioningGpd->addr), &gpdAddr, sizeof(EmberGpAddress));

  uint8_t gpPairingConfigCommunicationMode = (options
                                              & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_COMMUNICATION_MODE)
                                             >> EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_COMMUNICATION_MODE_OFFSET;
  bool appInformationPresent = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_APPLICATION_INFORMATION_PRESENT)
                               ? true : false;
  if (appInformationPresent) { // application info present
    commissioningGpd->applicationInfo.applInfoBitmap = applicationInformation;
    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_MANUFACTURE_ID_PRESENT) { // ManufactureId Present
      commissioningGpd->applicationInfo.manufacturerId = manufacturerId;
    }
    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_MODEL_ID_PRESENT) { // ModelId Present
      commissioningGpd->applicationInfo.modelId = modeId;
    }
    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_COMMANDS_PRESENT) {   // gpd Commands present
      commissioningGpd->applicationInfo.numberOfGpdCommands = numberOfGpdCommands;
      MEMCOPY(commissioningGpd->applicationInfo.gpdCommands,
              gpdCommandIdList,
              numberOfGpdCommands);
    }
    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_CLUSTER_LIST_PRESENT) { // Cluster List present
      commissioningGpd->applicationInfo.numberOfGpdClientCluster = clusterIdListCount >> 4;
      MEMCOPY(commissioningGpd->applicationInfo.clientClusters,
              clusterListClient,
              (commissioningGpd->applicationInfo.numberOfGpdClientCluster * 2));
      commissioningGpd->applicationInfo.numberOfGpdServerCluster = clusterIdListCount & 0x0f;
      MEMCOPY(commissioningGpd->applicationInfo.serverClusters,
              clusterListServer,
              (commissioningGpd->applicationInfo.numberOfGpdServerCluster * 2));
    }
    if (applicationInformation & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_PRESENT) { // Switch Information
      commissioningGpd->switchInformationStruct.switchInfoLength = switchInformationLength;
      commissioningGpd->switchInformationStruct.nbOfContacts = (genericSwitchConfiguration
                                                                & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_NB_OF_CONTACT);
      commissioningGpd->switchInformationStruct.switchType = (genericSwitchConfiguration
                                                              & EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE)
                                                             >> EMBER_AF_GP_APPLICATION_INFORMATION_SWITCH_INFORMATION_CONFIGURATION_SWITCH_TYPE_OFFSET;
      commissioningGpd->switchInformationStruct.currentContact = currentContactStatus;

      // Start the generic switch timeout
      uint32_t delay = EMBER_AF_PLUGIN_GREEN_POWER_SERVER_GENERIC_SWITCH_COMMISSIONING_TIMEOUT_IN_SEC;
      slxu_zigbee_event_set_delay_ms(genericSwitchCommissioningTimeout,
                                     delay * MILLISECOND_TICKS_PER_SECOND);
    }
  }

  bool assignedAllias = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_ASSIGNED_ALIAS) ? true : false;
  if (assignedAllias) {
    commissioningGpd->useGivenAssignedAlias = true;
    commissioningGpd->givenAlias = gpdAssignedAlias;
  }

  // Always present in the GpPairingConfig payload
  commissioningGpd->applicationInfo.deviceId = deviceId;
  commissioningGpd->applicationInfo.numberOfPairedEndpoints = numberOfPairedEndpoints;
  if (numberOfPairedEndpoints < 0xFD) {
    MEMCOPY(commissioningGpd->applicationInfo.pairedEndpoints,
            pairedEndpoints,
            numberOfPairedEndpoints);
  }

  bool securityUse = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SECURITY_USE) ? true : false;
  if (securityUse) {
    uint8_t gpdfExtendedOptions = 0;
    commissioningGpd->securityLevel = (securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);
    commissioningGpd->securityKeyType = (securityOptions & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE)
                                        >> EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_KEY_TYPE_OFFSET;
    gpdfExtendedOptions = commissioningGpd->securityLevel
                          | (commissioningGpd->securityKeyType << EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_KEY_TYPE_OFFSET);
    if (gpdSecurityKey) {
      gpdfExtendedOptions |= EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_PRESENT;
      gpdfExtendedOptions |= ((commissioningGpd->securityLevel == 3)
                              ? EMBER_AF_GP_GPD_COMMISSIONING_EXTENDED_OPTIONS_GPD_KEY_ENCRYPTION : 0);
      MEMCOPY(commissioningGpd->key.contents, gpdSecurityKey, EMBER_ENCRYPTION_KEY_SIZE);
    }
    commissioningGpd->gpdfExtendedOptions |= gpdfExtendedOptions;
  }

  bool seqNumberCapability = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_SEQUENCE_NUMBER_CAPABILITIES) ? true : false;
  if (securityUse
      || (!securityUse
          && seqNumberCapability)) {
    commissioningGpd->outgoingFrameCounter = gpdSecurityFrameCounter; // save/update the frame counter
  }

  bool rxOnCapability = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_RX_ON_CAPABILITY) ? true : false;
  bool fixedLocation = (options & EMBER_AF_GP_PAIRING_CONFIGURATION_OPTION_FIXED_LOCATION) ? true : false;
  uint8_t gpdfOptions = (seqNumberCapability ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_MAC_SEQ_NUM_CAP : 0)
                        | (rxOnCapability ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_RX_ON_CAP : 0)
                        | (appInformationPresent ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_APPLICATION_INFORMATION_PRESENT : 0)
                        | (fixedLocation ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_FIXED_LOCATION : 0)
                        | (securityUse ? EMBER_AF_GP_GPD_COMMISSIONING_OPTIONS_EXTENDED_OPTIONS_FIELD : 0);
  commissioningGpd->gpdfOptions |= gpdfOptions;
  commissioningGpd->groupcastRadius = groupcastRadius;
  uint8_t gpPairingConfigSecurityLevel = (securityOptions
                                          & EMBER_AF_GP_SINK_TABLE_ENTRY_SECURITY_OPTIONS_SECURITY_LEVEL);

  uint8_t gpsSecurityLevelAttribute = 0;
  EmberAfStatus UNUSED status;
  EmberAfAttributeType type;
  status = emberAfReadAttribute(GP_ENDPOINT,
                                ZCL_GREEN_POWER_CLUSTER_ID,
                                ZCL_GP_SERVER_GPS_SECURITY_LEVEL_ATTRIBUTE_ID,
                                (CLUSTER_MASK_SERVER),
                                (uint8_t*)&gpsSecurityLevelAttribute,
                                sizeof(uint8_t),
                                &type);

  gpsSecurityLevelAttribute = gpsSecurityLevelAttribute & 0x03; // mask the encryption part
  if ((groupListCount == 0xFF || groupList == NULL)
      || (sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_SINK_TABLE_BASED_GROUPCAST_FORWARDING)
          || sinkFunctionalitySupported(EMBER_AF_GP_GPS_FUNCTIONALITY_PRE_COMMISSIONED_GROUPCAST_COMMUNICATION))) {
    if (cmd->type != EMBER_INCOMING_BROADCAST
        && gpPairingConfigSecurityLevel == EMBER_GP_SECURITY_LEVEL_RESERVED) {
      emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_FAILURE);
      return true;
    }
    // Action = 0b001
    // Action = 0b010
    if ((gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_EXTEND_SINK_TABLE_ENTRY
         || gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_REPLACE_SINK_TABLE_ENTRY)
        && (gpPairingConfigSecurityLevel != EMBER_GP_SECURITY_LEVEL_RESERVED
            && (gpPairingConfigSecurityLevel >= gpsSecurityLevelAttribute
                && emGpCheckCommunicationModeSupport(gpPairingConfigCommunicationMode)))) {
      emberAfGreenPowerClusterPrintln("Replace or Extend Sink");
      uint8_t sinkEntryIndex = emberGpSinkTableLookup(&gpdAddr);
      if (sinkEntryIndex != 0xFF) {
        #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
        // To delete custom Translation table entry for this GPD
        emGpTransTableDeletePairedDevicefromTranslationTableEntry(&gpdAddr);
        #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
        if (emberAfPluginGreenPowerServerSinkTableAccessNotificationCallback(&gpdAddr,
                                                                             GREEN_POWER_SERVER_SINK_TABLE_ACCESS_TYPE_REMOVE_GPD)) {
          emberGpSinkTableRemoveEntry(sinkEntryIndex);
        }
      }
      commissioningGpd->communicationMode = gpPairingConfigCommunicationMode;
      // Set/reset the doNotSendGpPairing flag to be used when finaliing paring, Note : the negation.
      commissioningGpd->doNotSendGpPairing = ((actions & EMBER_AF_GP_PAIRING_CONFIGURATION_ACTIONS_SEND_GP_PAIRING)
                                              ? false : true);
      if ((commissioningGpd->applicationInfo.applInfoBitmap & EMBER_AF_GP_APPLICATION_INFORMATION_GPD_APPLICATION_DESCRIPTION_COMMAND_FOLLOWS)) {
        if (!slxu_zigbee_event_is_active(multiSensorCommissioningTimeout)) {
          emberAfGreenPowerClusterPrintln("Gp PairingConfig : Reseting Partial data - Only Report Data");
          resetOfMultisensorDataSaved(false, commissioningGpd);
        }
        emberAfGreenPowerClusterPrintln("Gp PairingConfig : Start MS/CAR Commissioning Timeout and waiting for ADCF");
        uint32_t delay = EMBER_AF_PLUGIN_GREEN_POWER_SERVER_MULTI_SENSOR_COMMISSIONING_TIMEOUT_IN_SEC;
        slxu_zigbee_event_set_delay_ms(multiSensorCommissioningTimeout,
                                       delay * MILLISECOND_TICKS_PER_SECOND);
        // All set to collect the report descriptors
        commissioningGpd->commissionState = GP_SINK_COMM_STATE_COLLECT_REPORTS;
        return true;
      }
      // Replace or Extend Sink with new application information, then create TT entries as well - if the application description is not following
      // Makes proxy on this node enter commissioning so that it can creat a proxy entry when Gp Pairing is heard
      if (!(commissioningState.inCommissioningMode)) {
        // The sink is not in commissioning mode for the CT based commissioning.
        // The endpoints are supplied by the CT, hence the sink need to match all the endpoints on it.
        commissioningState.endpoint = GREEN_POWER_SERVER_ALL_SINK_ENDPOINTS;
      #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
        emberAfGreenPowerClusterGpProxyCommissioningModeCallback((EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_ACTION // Enter Commissioning
                                                                  | EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_FIRST_PAIRING_SUCCESS), // Exit on First pairing
                                                                 0,   // No Commissioning window
                                                                 0);  // No channel present
      #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
      }
      commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
      finalisePairing(commissioningGpd);
      return true;
    }
    // Action = 0b101
    if (gpConfigAtion == EMBER_ZCL_GP_PAIRING_CONFIGURATION_ACTION_APPLICATION_DESCRIPTION) {
      // if total number of report is 0 then it is the first gp pairing
      if (commissioningGpd->totalNbOfReport == 0) {
        // first sensor to follow, reset structure for the report descriptors
        resetOfMultisensorDataSaved(false, commissioningGpd);
        MEMCOPY(&commissioningGpd->addr, &gpdAddr, sizeof(EmberGpAddress));
        commissioningGpd->totalNbOfReport = totalNumberOfReports;
        commissioningGpd->commissionState = GP_SINK_COMM_STATE_COLLECT_REPORTS;
      }
      // if the timer is running then process else just drop
      if (!slxu_zigbee_event_is_active(multiSensorCommissioningTimeout)) {
        emberAfGreenPowerClusterPrintln("Gp PairingConfig App Description - DROP : MS timer has expired");
        return false;
      }
      if (commissioningGpd->commissionState != GP_SINK_COMM_STATE_COLLECT_REPORTS) {
        // not in right state
        return false;
      }
      if (!saveReportDescriptor(totalNumberOfReports,
                                numberOfReports,
                                reportDescriptor,
                                commissioningGpd)) {
        // error in the application desc payload
        return false;
      }
      if (commissioningGpd->numberOfReports != commissioningGpd->totalNbOfReport) {
        // still to collect more reports
        return true;
      }
      // Report collection is over - finalise the pairing
      commissioningGpd->commissionState = GP_SINK_COMM_STATE_FINALISE_PAIRING;
      finalisePairing(commissioningGpd);
      return true;
    }
  }
  return true;
}

bool emberAfGreenPowerClusterGpSinkTableRequestCallback(uint8_t options,
                                                        uint32_t gpdSrcId,
                                                        uint8_t *gpdIeee,
                                                        uint8_t endpoint,
                                                        uint8_t index)
{
  // Null ieee pointer reassignment for MISRA compliance by pointing to an ieee address with 0s.
  if (gpdIeee  == NULL) {
    gpdIeee = nullEui64;
  }
  uint8_t entryIndex = 0;
  uint8_t appId = (options & EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_APPLICATION_ID);
  uint8_t requestType = (options & EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TYPE)
                        >> EMBER_AF_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TYPE_OFFSET;
  EmberGpSinkTableEntry entry;

  emberAfGreenPowerClusterPrintln("Got sink table request with options %1x and index %1x", options, index);
  // only respond to unicast messages.
  if (emberAfCurrentCommand()->type != EMBER_INCOMING_UNICAST) {
    emberAfGreenPowerClusterPrintln("Not unicast");
    goto kickout;
  }

  // the device SHALL check if it implements a Sink Table.
  // If not, it SHALL generate a ZCL Default Response command,
  // with the Status code field carrying UNSUP_COMMAND, subject to the rules as specified in sec. 2.4.12 of [3]
  if (EMBER_GP_SINK_TABLE_SIZE == 0) {
    emberAfGreenPowerClusterPrintln("Unsup cluster command");
    emberAfSendImmediateDefaultResponse(EMBER_ZCL_STATUS_UNSUP_COMMAND);
    return true;
  }

  if (emberAfCurrentEndpoint() != GP_ENDPOINT) {
    emberAfGreenPowerClusterPrintln("Drop frame due to unknown endpoint: %X", emberAfCurrentEndpoint());
    return false;
  }

  uint8_t validEntriesCount = gpNumberOfSinkEntriesInSinkTable();

  // If its Sink Table is empty, and the triggering GP Sink Table Request was received in unicast,
  // then the GP Sink Table Response SHALL be sent with Status NOT_FOUND,
  // Total number of non-empty Sink Table entries carrying 0x00,
  // Start index carrying 0xFF (in case of request by GPD ID) or
  // the Index value from the triggering GP Sink Table Request (in case of request by index),
  // Entries count field set to 0x00, and any Sink Table entry fields absent
  if (validEntriesCount == 0) {
    // "index" is already 0xFF if search by ID
    // or already set to the value from "SinkTableRequest" triggered frame in case it is search by INDEX
    emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_NOT_FOUND,
                                                                0,
                                                                index,
                                                                0);
    emberAfSendResponse();
    return true;
  } else {
    // Valid Entries are present!
    if (requestType == EMBER_ZCL_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TABLE_ENTRIES_BY_GPD_ID) {
      EmberGpAddress gpdAddr;
      emGpMakeAddr(&gpdAddr, appId, gpdSrcId, gpdIeee, endpoint);
      entryIndex = emberGpSinkTableLookup(&gpdAddr);
      if (entryIndex == 0xFF) {
        // Valid entries present but none for this gpdAddr - Send NOT FOUND sesponse.
        emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_NOT_FOUND,
                                                                    validEntriesCount,
                                                                    entryIndex,
                                                                    0);
        emberAfSendResponse();
        goto kickout;
      } else {
        // A valid entry with the ID is present
        if (emberGpSinkTableGetEntry(entryIndex, &entry) == EMBER_SUCCESS) {
          // If the triggering GP Sink Table Request command contained a GPD ID field, the device SHALL check
          // if it has a Sink Table entry for this GPD ID (and Endpoint, if ApplicationID = 0b010). If yes, the device
          // SHALL create a GP Sink Table Response with Status SUCCESS, Total number of non-empty Sink Table
          // entries carrying the total number of non-empty Sink Table entries on this device, Start index set to
          // 0xff, Entries count field set to 0x01, and one Sink Table entry field for the requested GPD ID (and
          // Endpoint, if ApplicationID = 0b010), formatted as specified in sec. A.3.3.2.2.1, present.
          emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_SUCCESS,
                                                                      validEntriesCount,
                                                                      0xff,
                                                                      1);
          appResponseLength += storeSinkTableEntryInBuffer(&entry, (appResponseData + appResponseLength));
          emberAfSendResponse();
        } else {
          // Not found status to go out.
        }
        goto kickout;
      }
    } else if (requestType == EMBER_ZCL_GP_SINK_TABLE_REQUEST_OPTIONS_REQUEST_TABLE_ENTRIES_BY_INDEX) {
      if (index >= validEntriesCount) {
        emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_NOT_FOUND,
                                                                    validEntriesCount,
                                                                    index,
                                                                    0);
        emberAfSendResponse();
        return true;
      } else {
        // return the sink table entry content into the reponse payload from indicated
        // index and nexts until these are consistant (adress type, etc) and
        // as long as it feet into one message
        emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_SUCCESS,
                                                                    validEntriesCount,
                                                                    index,
                                                                    0xff);
        validEntriesCount = 0;
        uint16_t entriesCount = 0;
        for (entryIndex = 0; entryIndex < EMBER_GP_SINK_TABLE_SIZE; entryIndex++) {
          if (emberGpSinkTableGetEntry(entryIndex, &entry) != EMBER_SUCCESS) {
            continue;
          }

          uint8_t  tempDatabuffer[EMBER_AF_RESPONSE_BUFFER_LEN];
          uint16_t tempDataLength = 0;

          if (entry.status != EMBER_GP_SINK_TABLE_ENTRY_STATUS_UNUSED
              && entry.status != 0) {
            validEntriesCount++;
            if (validEntriesCount > index) {
              // Copy to a temp buffer and add if there is space
              tempDataLength = storeSinkTableEntryInBuffer(&entry, tempDatabuffer);
              // If space add to buffer
              if (sizeof(appResponseData) > (appResponseLength + tempDataLength)) {
                MEMMOVE(&appResponseData[appResponseLength], tempDatabuffer, tempDataLength);
                appResponseLength +=  tempDataLength;
                entriesCount++;
              } else {
                break;
              }
            }
          }
        }
        //Insert the number of entries actually included
        appResponseData[GP_SINK_TABLE_RESPONSE_ENTRIES_OFFSET + GP_NON_MANUFACTURER_ZCL_HEADER_LENGTH] = entriesCount;
        EmberStatus status = emberAfSendResponse();
        if (status == EMBER_MESSAGE_TOO_LONG) {
          emberAfFillCommandGreenPowerClusterGpSinkTableResponseSmart(EMBER_ZCL_GP_SINK_TABLE_RESPONSE_STATUS_SUCCESS,
                                                                      validEntriesCount,
                                                                      index,
                                                                      0);
          emberAfSendResponse();
        }
        goto kickout;
      }
    } else {
      // nothing, other value of requestType are reserved
    }
  }
  kickout: return true;
}

#endif // UC_BUILD

GpCommDataSaved *emberAfGreenPowerServerFindCommissioningGpdInstance(EmberGpAddress *gpdAddr)
{
  if (gpdAddr != NULL) {
    for (int i = 0; i < EMBER_AF_PLUGIN_GREEN_POWER_SERVER_COMMISSIONING_GPD_INSTANCES; i++) {
      if (emberAfGreenPowerCommonGpAddrCompare(gpdAddr, &gpdCommDataSaved[i].addr)) {
        return &gpdCommDataSaved[i];
      }
    }
  }
  return NULL;
}

void emberAfGreenPowerServerDeleteCommissioningGpdInstance(EmberGpAddress *gpdAddr)
{
  if (gpdAddr != NULL) {
    GpCommDataSaved *gpd = emberAfGreenPowerServerFindCommissioningGpdInstance(gpdAddr);
    if (gpd != NULL) {
      MEMSET(gpd, 0, sizeof(GpCommDataSaved));
    }
  }
}

EmberAfStatus emberAfGreenPowerServerDeriveSharedKeyFromSinkAttribute(uint8_t *gpsSecurityKeyTypeAtrribute,
                                                                      EmberKeyData *gpSharedKeyAttribute,
                                                                      EmberGpAddress *gpdAddr)
{
  EmberAfAttributeType type;
  EmberAfStatus status = emberAfReadAttribute(GP_ENDPOINT,
                                              ZCL_GREEN_POWER_CLUSTER_ID,
                                              ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_TYPE_ATTRIBUTE_ID,
                                              CLUSTER_MASK_SERVER,
                                              gpsSecurityKeyTypeAtrribute,
                                              sizeof(uint8_t),
                                              &type);

  if (status == EMBER_ZCL_STATUS_SUCCESS) {
    if ((*gpsSecurityKeyTypeAtrribute == EMBER_ZCL_GP_SECURITY_KEY_TYPE_ZIGBEE_NETWORK_KEY)
        || (*gpsSecurityKeyTypeAtrribute == EMBER_ZCL_GP_SECURITY_KEY_TYPE_NETWORK_DERIVED_GROUP_KEY)) {
      EmberKeyStruct nwkKey;
      // Get NWK Key from NIB
      if (EMBER_SUCCESS == emberGetKey(EMBER_CURRENT_NETWORK_KEY,
                                       &nwkKey)) {
        if (*gpsSecurityKeyTypeAtrribute == EMBER_ZCL_GP_SECURITY_KEY_TYPE_NETWORK_DERIVED_GROUP_KEY) {
          // if derived hash it with 'ZGP'
          uint8_t result[EMBER_ENCRYPTION_KEY_SIZE] = { 0 };
          emberHmacAesHash(emberKeyContents(&nwkKey.key),
                           (uint8_t *)"ZGP",
                           3,
                           result);
          MEMCOPY(gpSharedKeyAttribute->contents, result, EMBER_ENCRYPTION_KEY_SIZE);
        } else {
          MEMCOPY(emberKeyContents(gpSharedKeyAttribute), emberKeyContents(&nwkKey.key), sizeof(EmberKeyData));
        }
        return EMBER_ZCL_STATUS_SUCCESS;
      }
    } else if ((*gpsSecurityKeyTypeAtrribute == EMBER_ZCL_GP_SECURITY_KEY_TYPE_GPD_GROUP_KEY)
               || (*gpsSecurityKeyTypeAtrribute == EMBER_ZCL_GP_SECURITY_KEY_TYPE_DERIVED_INDIVIDUAL_GPD_KEY)) {
      // Get the Group Key from gps shared key attribute
      status = emberAfReadAttribute(GP_ENDPOINT,
                                    ZCL_GREEN_POWER_CLUSTER_ID,
                                    ZCL_GP_SERVER_GP_SHARED_SECURITY_KEY_ATTRIBUTE_ID,
                                    CLUSTER_MASK_SERVER,
                                    gpSharedKeyAttribute->contents,
                                    EMBER_ENCRYPTION_KEY_SIZE,
                                    &type);
      if (status == EMBER_ZCL_STATUS_SUCCESS) {
        // If derived, use the HMAC based on GPD address.
        if (*gpsSecurityKeyTypeAtrribute == EMBER_ZCL_GP_SECURITY_KEY_TYPE_DERIVED_INDIVIDUAL_GPD_KEY) {
          uint8_t result[EMBER_ENCRYPTION_KEY_SIZE] = { 0 };
          // The size is always 20 because the header is of 4 bytes and the Key is 16 bytes
          uint8_t data[8] = { 0 };
          if (gpdAddr->applicationId == EMBER_GP_APPLICATION_IEEE_ADDRESS) {
            MEMMOVE(data, gpdAddr->id.gpdIeeeAddress, 8);
          } else {
            emberStoreLowHighInt32u(data, gpdAddr->id.sourceId);
          }
          emberHmacAesHash(gpSharedKeyAttribute->contents,
                           data,
                           ((gpdAddr->applicationId == EMBER_GP_APPLICATION_IEEE_ADDRESS) ? 8 : 4),
                           result);
          MEMCOPY(gpSharedKeyAttribute->contents, result, EMBER_ENCRYPTION_KEY_SIZE);
        }
        return EMBER_ZCL_STATUS_SUCCESS;
      }
    } else {
      //any other key type is not valid that the shared key type attribute can hold.
    }
  }
  return EMBER_ZCL_STATUS_FAILURE;
}

void emberAfGreenPowerClusterGpSinkCommissioningWindowExtend(uint16_t commissioningWindow)
{
  if (!(commissioningState.inCommissioningMode)) {
    return; // If not in commissioning mode no action.
  }
  slxu_zigbee_event_set_delay_ms(commissioningWindowTimeout,
                                 commissioningWindow * MILLISECOND_TICKS_PER_SECOND);

  uint8_t proxyOptions = EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_ACTION \
                         | EMBER_AF_GP_PROXY_COMMISSIONING_MODE_EXIT_MODE_ON_COMMISSIONING_WINDOW_EXPIRATION;

  if (commissioningState.unicastCommunication) { //  based on the commission mode as decided by sink
    proxyOptions |= EMBER_AF_GP_PROXY_COMMISSIONING_MODE_OPTION_UNICAST_COMMUNICATION;
  }
  emberAfFillCommandGreenPowerClusterGpProxyCommissioningModeSmart(proxyOptions,
                                                                   commissioningWindow,
                                                                   0);
  EmberApsFrame *apsFrame;
  apsFrame = emberAfGetCommandApsFrame();
  apsFrame->sourceEndpoint = GP_ENDPOINT;
  apsFrame->destinationEndpoint = GP_ENDPOINT;
  if (commissioningState.proxiesInvolved) {
    EmberStatus status = emberAfSendCommandBroadcast(EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS);
    // Callback to inform the status of message submission to network
    emberAfGreenPowerClusterCommissioningMessageStatusNotificationCallback(&commissioningState,
                                                                           apsFrame,
                                                                           EMBER_OUTGOING_BROADCAST,
                                                                           EMBER_RX_ON_WHEN_IDLE_BROADCAST_ADDRESS,
                                                                           status);
    emberAfGreenPowerClusterPrintln("Extended Commissioning Status = %d", status);
  } else {
    #ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
    // Put the proxy instance on this node commissioning mode so that it can accept a pairing from itself.
    // This is to ensure the node will be able to handle gpdf commands after pairig.
    EmberStatus status = emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, emberAfGetNodeId());
    // Callback to inform the status of message submission to network
    emberAfGreenPowerClusterCommissioningMessageStatusNotificationCallback(&commissioningState,
                                                                           apsFrame,
                                                                           EMBER_OUTGOING_DIRECT,
                                                                           emberAfGetNodeId(),
                                                                           status);
    emberAfGreenPowerClusterPrintln("Extended Commissioning Status = %d", status);
    #endif // SL_CATALOG_ZIGBEE_GREEN_POWER_CLIENT_PRESENT
  }
}

void emberAfGreenPowerClusterGpSinkCommissioningUnicastMode(bool unicastCommunication)
{
  commissioningState.unicastCommunication = unicastCommunication;
}

void emberAfGreenPowerClusterGpSinkGpPairingUnicastMode(bool sendGpPairingInUnicastMode)
{
  commissioningState.sendGpPairingInUnicastMode = sendGpPairingInUnicastMode;
}
// Returns the commissioning state of the sink.
EmberAfGreenPowerServerCommissioningState *emberAfGreenPowerClusterGetServerCommissioningSate(void)
{
  return (&commissioningState);
}

#ifdef UC_BUILD
#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
bool emberAfGreenPowerClusterGpTranslationTableUpdateCallback(EmberAfClusterCommand *cmd);
bool emberAfGreenPowerClusterGpTranslationTableRequestCallback(EmberAfClusterCommand *cmd);
#endif

uint32_t emberAfGreenPowerClusterServerCommandParse(sl_service_opcode_t opcode,
                                                    sl_service_function_context_t *context)
{
  (void)opcode;

  EmberAfClusterCommand *cmd = (EmberAfClusterCommand *)context->data;
  bool wasHandled = false;

  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
      case ZCL_GP_NOTIFICATION_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpNotificationCallback(cmd);
        break;
      }
      case ZCL_GP_COMMISSIONING_NOTIFICATION_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpCommissioningNotificationCallback(cmd);
        break;
      }
      case ZCL_GP_SINK_COMMISSIONING_MODE_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpSinkCommissioningModeCallback(cmd);
        break;
      }
#ifdef SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      // When the translation table component is supported process the commands related to
      // translation table.
      case ZCL_GP_TRANSLATION_TABLE_UPDATE_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpTranslationTableUpdateCallback(cmd);
        break;
      }
      case ZCL_GP_TRANSLATION_TABLE_REQUEST_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpTranslationTableRequestCallback(cmd);
        break;
      }
#endif // SL_CATALOG_ZIGBEE_GREEN_POWER_TRANSLATION_TABLE_PRESENT
      case ZCL_GP_PAIRING_CONFIGURATION_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpPairingConfigurationCallback(cmd);
        break;
      }
      case ZCL_GP_SINK_TABLE_REQUEST_COMMAND_ID:
      {
        wasHandled = emberAfGreenPowerClusterGpSinkTableRequestCallback(cmd);
        break;
      }
    }
  }

  return ((wasHandled)
          ? EMBER_ZCL_STATUS_SUCCESS
          : EMBER_ZCL_STATUS_UNSUP_COMMAND);
}

#endif // UC_BUILD
