/***************************************************************************//**
 * @file
 * @brief Routines for the Network Creator Security plugin, which configures
 *        network permit join state and key policy for a Zigbee 3.0 network.
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

#include "network-creator-security.h"

#include "app/framework/security/af-security.h" // emAfAllowTrustCenterRejoins
#include "app/util/zigbee-framework/zigbee-device-common.h" // emberLeaveRequest

#ifdef UC_BUILD
#include "network-creator-security-config.h"
#include "sl_component_catalog.h"
#if (EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_HA_DEVICES_TO_STAY == 1)
#define ALLOW_HA_DEVICES_TO_STAY
#endif
#if (EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_BDB_JOIN_USES_INSTALL_CODE_KEY == 1)
#define BDB_JOIN_USES_INSTALL_CODE_KEY
#endif
#if (EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY == 1)
#define ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY
#endif
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_HA_DEVICES_TO_STAY
#define ALLOW_HA_DEVICES_TO_STAY
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_BDB_JOIN_USES_INSTALL_CODE_KEY
#define BDB_JOIN_USES_INSTALL_CODE_KEY
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY
#define ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY
#endif
#ifdef EMBER_AF_PLUGIN_TEST_HARNESS_Z3
#define SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT
#endif
#endif // UC_BUILD

#ifdef EZSP_HOST
// NCP
  #define allowTrustCenterLinkKeyRequests() \
  ezspSetPolicy(EZSP_TC_KEY_REQUEST_POLICY, EZSP_ALLOW_TC_KEY_REQUESTS_AND_SEND_CURRENT_KEY)
  #define allowTrustCenterLinkKeyRequestsAndGenerateNewKeys() \
  ezspSetPolicy(EZSP_TC_KEY_REQUEST_POLICY, EZSP_ALLOW_TC_KEY_REQUEST_AND_GENERATE_NEW_KEY)
  #define setTransientKeyTimeout(timeS) \
  ezspSetValue(EZSP_VALUE_TRANSIENT_KEY_TIMEOUT_S, 2, (uint8_t *)(&timeS));
  #define setTcRejoinsUsingWellKnownKeyAllowed(allow) \
  (void)ezspSetPolicy(EZSP_TC_REJOINS_USING_WELL_KNOWN_KEY_POLICY, (allow))
  #define setTcRejoinsUsingWellKnownKeyTimeout(timeout) \
  (void)ezspSetConfigurationValue(EZSP_CONFIG_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_S, (timeout))
#else
// SoC
extern uint16_t emAllowTcRejoinsUsingWellKnownKeyTimeoutSec;
  #define allowTrustCenterLinkKeyRequests() \
  emberTrustCenterLinkKeyRequestPolicy = EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_SEND_CURRENT_KEY
  #define allowTrustCenterLinkKeyRequestsAndGenerateNewKeys() \
  emberTrustCenterLinkKeyRequestPolicy = EMBER_ALLOW_TC_LINK_KEY_REQUEST_AND_GENERATE_NEW_KEY
  #define setTransientKeyTimeout(timeS) \
  emberTransientKeyTimeoutS = (timeS)
  #define setTcRejoinsUsingWellKnownKeyAllowed(allow) \
  emberSetTcRejoinsUsingWellKnownKeyAllowed((allow))
  #define setTcRejoinsUsingWellKnownKeyTimeout(timeout) \
  emAllowTcRejoinsUsingWellKnownKeyTimeoutSec = (timeout)
#endif

#define ZIGBEE_3_CENTRALIZED_SECURITY_LINK_KEY         \
  {                                                    \
    { 0x5A, 0x69, 0x67, 0x42, 0x65, 0x65, 0x41, 0x6C,  \
      0x6C, 0x69, 0x61, 0x6E, 0x63, 0x65, 0x30, 0x39 } \
  }

#define ZIGBEE_3_DISTRIBUTED_SECURITY_LINK_KEY         \
  {                                                    \
    { 0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7,  \
      0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF } \
  }

#ifndef EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_NETWORK_OPEN_TIME_S
  #define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_NETWORK_OPEN_TIME_S (300)
#endif

#define NETWORK_OPEN_TIME_S (EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_NETWORK_OPEN_TIME_S)

#if defined(ALLOW_HA_DEVICES_TO_STAY)
  #define ALLOW_HA_DEVICES true
#else
  #define ALLOW_HA_DEVICES false
#endif

// -----------------------------------------------------------------------------
// Globals

bool allowHaDevices = ALLOW_HA_DEVICES;
EmberKeyData distributedKey = ZIGBEE_3_DISTRIBUTED_SECURITY_LINK_KEY;

// -----------------------------------------------------------------------------
// Internal Declarations

#ifdef UC_BUILD
void emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEventHandler(SLXU_UC_EVENT);
sl_zigbee_event_t emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEvents[EMBER_SUPPORTED_NETWORKS];
#define openNetworkEventControl (emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEvents)
#else
extern EmberEventControl emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEventControls[];
#define openNetworkEventControl (emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEventControls)
#endif // UC_BUILD

static uint16_t openNetworkTimeRemainingS;

// -----------------------------------------------------------------------------
// Framework Callbacks

void emberAfPluginNetworkCreatorSecurityInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

#if defined(EZSP_HOST) && defined(BDB_JOIN_USES_INSTALL_CODE_KEY)
  EzspStatus status = emberAfSetEzspPolicy(EZSP_TRUST_CENTER_POLICY,
                                           (EZSP_DECISION_ALLOW_JOINS | EZSP_DECISION_JOINS_USE_INSTALL_CODE_KEY),
                                           "Trust Center Policy",
                                           "Joins using install code only");

  if (EZSP_SUCCESS != status) {
    emberAfCorePrintln("%s: %s: 0x%X",
                       EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                       "failed to configure joining using install code only",
                       status);
  }
#endif // EZSP_HOST && BDB_JOIN_USES_INSTALL_CODE_KEY

  slxu_zigbee_network_event_init(openNetworkEventControl,
                                 emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEventHandler);
}

// TODO: renamed for naming consistency purposes
#ifdef UC_BUILD
void emAfPluginNetworkCreatorSecurityStackStatusCallback(EmberStatus status)
#else
void emberAfPluginNetworkCreatorSecurityStackStatusCallback(EmberStatus status)
#endif
{
#ifdef EMBER_AF_HAS_COORDINATOR_NETWORK
  if (status == EMBER_NETWORK_UP
      && emberAfGetNodeId() == EMBER_TRUST_CENTER_NODE_ID) {
    EmberExtendedSecurityBitmask extended;

    // If we form a centralized network, make sure our trust center policy is
    // correctly set to respond to a key request.
#ifdef ZCL_USING_SL_WWAH_CLUSTER_CLIENT
    allowTrustCenterLinkKeyRequestsAndGenerateNewKeys();
#else
    allowTrustCenterLinkKeyRequests();
#endif

    // This bit is not saved to a token, so make sure that our security bitmask
    // has this bit set on reboot.
    emberGetExtendedSecurityBitmask(&extended);
    extended |= EMBER_NWK_LEAVE_REQUEST_NOT_ALLOWED;
    emberSetExtendedSecurityBitmask(extended);
  }
#endif /* EMBER_AF_HAS_COORDINATOR_NETWORK */
}

// -----------------------------------------------------------------------------
// Stack Callbacks

#if (defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT))
static bool isWildcardEui64(EmberEUI64 eui64)
{
  for (uint8_t i = 0; i < EUI64_SIZE; i++) {
    if (eui64[i] != 0xFF) {
      return false;
    }
  }
  return true;
}
#endif // defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)

#if defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)
extern uint8_t emAfPluginTestHarnessZ3ServerMaskHigh;
#endif // defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)

void emberAfPluginNetworkCreatorSecurityZigbeeKeyEstablishmentCallback(EmberEUI64 eui64,
                                                                       EmberKeyStatus keyStatus)
{
#if (defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT))
  // If we are notified that a joining node failed to verify their
  // TCLK properly, then we are going to kick them off the network,
  // as they pose a potential security hazard.

#if defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)
  // Do nothing if we are pretending to be r20 or lower.
  if (emAfPluginTestHarnessZ3ServerMaskHigh == 0) {
    return;
  }
#endif // defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)

  if (!isWildcardEui64(eui64)
      && ((keyStatus == EMBER_TC_REQUESTER_VERIFY_KEY_TIMEOUT && !allowHaDevices)
          || keyStatus == EMBER_TC_REQUESTER_VERIFY_KEY_FAILURE)) {
    EmberStatus status = EMBER_NOT_FOUND;
    EmberNodeId destinationId = emberLookupNodeIdByEui64(eui64);
    if (destinationId != EMBER_NULL_NODE_ID) {
      // We do not tell the node to rejoin, since they may be malicious.
      status = emberLeaveRequest(destinationId,
                                 eui64,
                                 0, // no leave flags
                                 (EMBER_APS_OPTION_RETRY
                                  | EMBER_APS_OPTION_ENABLE_ROUTE_DISCOVERY));
    }
    emberAfCorePrintln("%s: Remove node 0x%2X for failed key verification: 0x%X",
                       EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                       destinationId,
                       status);
  }
#endif // defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)
}

// -----------------------------------------------------------------------------
// API

EmberStatus emberAfPluginNetworkCreatorSecurityStart(bool centralizedNetwork)
{
  EmberInitialSecurityState state;
  EmberExtendedSecurityBitmask extended;
  EmberStatus status = EMBER_SUCCESS;

  MEMSET(&state, 0, sizeof(state));
  state.bitmask = (EMBER_TRUST_CENTER_GLOBAL_LINK_KEY
                   | EMBER_HAVE_PRECONFIGURED_KEY
                   | EMBER_HAVE_NETWORK_KEY
                   | EMBER_NO_FRAME_COUNTER_RESET
                   | EMBER_REQUIRE_ENCRYPTED_KEY);

  extended = EMBER_JOINER_GLOBAL_LINK_KEY;

  if (!centralizedNetwork) {
    MEMMOVE(&(state.preconfiguredKey),
            emberKeyContents(&distributedKey),
            EMBER_ENCRYPTION_KEY_SIZE);

    // Use distributed trust center mode.
    state.bitmask |= EMBER_DISTRIBUTED_TRUST_CENTER_MODE;
  }
#if (defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT))
  else { // centralizedNetwork
    // Generate a random global link key.
    // This is the key the trust center will send to a joining node when it
    // updates its link key.
    status = emberAfGenerateRandomKey(&(state.preconfiguredKey));
    if (status != EMBER_SUCCESS) {
      goto kickout;
    }

    // Use hashed link keys for improved storage and speed.
    state.bitmask |= EMBER_TRUST_CENTER_USES_HASHED_LINK_KEY;

    // Tell the trust center to ignore leave requests.
    extended |= EMBER_NWK_LEAVE_REQUEST_NOT_ALLOWED;
  }
#else
  else { // centralizedNetwork
    // in case device doesn't support centralized network we should return EMBER_INVALID_CALL immediately
    status = EMBER_INVALID_CALL;
    goto kickout;
  }
#endif // defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)

  // Generate a random network key.
  status = emberAfGenerateRandomKey(&(state.networkKey));
  if (status != EMBER_SUCCESS) {
    goto kickout;
  }

  // Set the initial security data.
  status = emberSetInitialSecurityState(&state);
  if (status != EMBER_SUCCESS) {
    goto kickout;
  }
  status = emberSetExtendedSecurityBitmask(extended);

  kickout:
  emberAfCorePrintln("%s: %s: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Start",
                     status);
  return status;
}

EmberStatus emberAfPluginNetworkCreatorSecurityOpenNetwork(void)
{
  EmberStatus status = EMBER_SUCCESS;
  EmberCurrentSecurityState securityState;

  if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
    return EMBER_ERR_FATAL;
  }

  emberGetCurrentSecurityState(&securityState);

  // For a distributed network, don't store the ZIGBEE_3_CENTRALIZED_SECURITY_LINK_KEY
  // in the transient link key table.
  if (!(securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE)) {
    #if defined(ALLOW_TC_REJOIN_WITH_WELL_KNOWN_KEY)
    #if defined(EMBER_TEST)
    #define EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_SEC 0
    #endif
    setTcRejoinsUsingWellKnownKeyTimeout(EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_ALLOW_TC_REJOINS_USING_WELL_KNOWN_KEY_TIMEOUT_SEC);
    setTcRejoinsUsingWellKnownKeyAllowed(true);
    #endif

    #if (defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT))
    if (emberAfGetNodeId() == EMBER_TRUST_CENTER_NODE_ID) {
      EmberEUI64 wildcardEui64 = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, };
      EmberKeyData centralizedKey = ZIGBEE_3_CENTRALIZED_SECURITY_LINK_KEY;
      status = emberAddTransientLinkKey(wildcardEui64, &centralizedKey);
    }
  #endif // defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)
  }

  if (status == EMBER_SUCCESS) {
    openNetworkTimeRemainingS = NETWORK_OPEN_TIME_S;
    setTransientKeyTimeout(openNetworkTimeRemainingS);
#ifdef UC_BUILD
    sl_zigbee_event_set_active(openNetworkEventControl);
#else
    emberAfNetworkEventControlSetActive(openNetworkEventControl);
#endif
  }

  return status;
}

EmberStatus emberAfPluginNetworkCreatorSecurityCloseNetwork(void)
{
  EmberStatus status = EMBER_ERR_FATAL;

  if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
    emberClearTransientLinkKeys();
#ifdef UC_BUILD
    sl_zigbee_event_set_inactive(openNetworkEventControl);
#else
    emberAfNetworkEventControlSetInactive(openNetworkEventControl);
#endif
    zaTrustCenterSetJoinPolicy(EMBER_ALLOW_REJOINS_ONLY);
    status = emberAfPermitJoin(0, true); // broadcast
  }

  return status;
}

EmberStatus emberAfPluginNetworkCreatorSecurityOpenNetworkWithKeyPair(EmberEUI64 eui64,
                                                                      EmberKeyData keyData)
{
  EmberStatus status = EMBER_SUCCESS;

  if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
    return EMBER_ERR_FATAL;
  }

#if (defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT))
  if (emberAfGetNodeId() == EMBER_TRUST_CENTER_NODE_ID) {
    status = emberAddTransientLinkKey(eui64, &keyData);
  }
#endif // defined(EMBER_AF_HAS_COORDINATOR_NETWORK) || defined(SL_CATALOG_ZIGBEE_TEST_HARNESS_Z3_PRESENT)

  if (status == EMBER_SUCCESS) {
    openNetworkTimeRemainingS = NETWORK_OPEN_TIME_S;
    setTransientKeyTimeout(openNetworkTimeRemainingS);
#ifdef UC_BUILD
    sl_zigbee_event_set_active(openNetworkEventControl);
#else
    emberAfNetworkEventControlSetActive(openNetworkEventControl);
#endif
  }

  return status;
}

// -----------------------------------------------------------------------------
// Internal Definitions

void emberAfPluginNetworkCreatorSecurityOpenNetworkNetworkEventHandler(SLXU_UC_EVENT)
{
  EmberStatus status = EMBER_SUCCESS;
  uint8_t permitJoinTime;

#ifdef UC_BUILD
  sl_zigbee_event_set_inactive(openNetworkEventControl);
#else
  emberAfNetworkEventControlSetInactive(openNetworkEventControl);
#endif

  // If we have left the network, then we don't need to proceed further.
  if (emberAfNetworkState() != EMBER_JOINED_NETWORK) {
    return;
  }

  if (openNetworkTimeRemainingS > EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT) {
    permitJoinTime = EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT;
    openNetworkTimeRemainingS -= EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT;
#ifdef UC_BUILD
    sl_zigbee_event_set_delay_qs(openNetworkEventControl,
                                 (EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT << 2));
#else
    emberAfNetworkEventControlSetDelayQS(openNetworkEventControl,
                                         (EMBER_AF_PERMIT_JOIN_MAX_TIMEOUT << 2));
#endif
  } else {
    permitJoinTime = openNetworkTimeRemainingS;
    openNetworkTimeRemainingS = 0;
  }

  if (status == EMBER_SUCCESS) {
    zaTrustCenterSetJoinPolicy(EMBER_USE_PRECONFIGURED_KEY);
    status = emberAfPermitJoin(permitJoinTime, true); // broadcast permit join
  }

  emberAfCorePrintln("%s: %s: 0x%X",
                     EMBER_AF_PLUGIN_NETWORK_CREATOR_SECURITY_PLUGIN_NAME,
                     "Open network",
                     status);
}
