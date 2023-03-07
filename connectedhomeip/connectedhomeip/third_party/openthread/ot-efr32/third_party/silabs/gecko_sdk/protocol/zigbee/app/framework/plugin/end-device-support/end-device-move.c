/***************************************************************************//**
 * @file
 * @brief Code common to SOC and host to handle moving (i.e. rejoining) to a new
 * parent device.
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
#include "app/framework/util/util.h"
#include "end-device-move.h"

#ifdef UC_BUILD
#include "end-device-support.h"
#include "sl_component_catalog.h"
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
#include "network-find.h"
#endif
#ifdef SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_COMMON_PRESENT
#include "zll-commissioning-common.h"
#endif
sl_zigbee_event_t emberAfPluginEndDeviceSupportMoveNetworkEvents[EMBER_SUPPORTED_NETWORKS];
void emberAfPluginEndDeviceSupportMoveNetworkEventHandler(SLXU_UC_EVENT);
#else // !UC_BUILD
#ifdef EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ
  #include "app/framework/plugin/network-find/network-find.h"
  #define SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_ZLL_COMMISSIONING_COMMON
  #include "app/framework/plugin/zll-commissioning-common/zll-commissioning-common.h"
  #define SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_COMMON_PRESENT
#endif
extern EmberEventControl emberAfPluginEndDeviceSupportMoveNetworkEventControls[];
#endif // UC_BUILD

// *****************************************************************************
// Globals

#if defined(EMBER_SCRIPTED_TEST)
uint8_t emAfRejoinAttemptsMax = 3;
  #define EMBER_AF_REJOIN_ATTEMPTS_MAX emAfRejoinAttemptsMax
#endif

typedef struct {
  uint8_t moveAttempts; // counts *completed* attempts, i.e. 0 on 1st attempt
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  // If sub-GHz network find is enabled, we try to rejoin on each page in turn.
  // Depending on the find mode setting in the Network Find (Sub-GHz) plugin,
  // this may be limited to 2.4 GHz only, sub-GHz only or both.
  // The rejoining sequence uses a sliding scale:
  // * moveAttempts == 0 ... the current channel only
  // * moveAttempts == 1 ... the current page only
  // * moveAttempts > 1 .... all pages (subject to find mode setting)
  // NOTE 1: 'page' is needed *only* if sub-GHz support is enabled, as otherwise
  // "moveAttempts == 1" and "moveAttempts > 1" are functionally identical.
  // NOTE 2: if EMBER_AF_REJOIN_ATTEMPTS_MAX is 2 or less, we skip rejoining
  // on the current page and go from the current channel straight to all pages.
  uint8_t page;
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
} State;
static State states[EMBER_SUPPORTED_NETWORKS];

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  #if EMBER_AF_REJOIN_ATTEMPTS_MAX > 2
    #define MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES 2
  #else
    #define MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES 1
  #endif
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT

#define NEVER_STOP_ATTEMPTING_REJOIN 0xFF
#define MOVE_DELAY_QS (10 * 4)

// *****************************************************************************
// Functions

static void scheduleMoveEvent(void)
{
  uint8_t networkIndex = emberGetCurrentNetwork();
  State *state = &states[networkIndex];

  if (EMBER_AF_REJOIN_ATTEMPTS_MAX == NEVER_STOP_ATTEMPTING_REJOIN
      || state->moveAttempts < EMBER_AF_REJOIN_ATTEMPTS_MAX) {
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
    if (state->moveAttempts >= MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES) {
      emberAfAppPrintln("%p %d: %d, page %d", "Schedule move nwk",
                        networkIndex,
                        state->moveAttempts,
                        state->page);
    } else
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
    {
      emberAfAppPrintln("%s %d: %d", "Schedule move nwk",
                        networkIndex,
                        state->moveAttempts);
    }
#ifdef UC_BUILD
    sl_zigbee_event_set_delay_qs(emberAfPluginEndDeviceSupportMoveNetworkEvents,
                                 (state->moveAttempts == 0
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
                                  || (state->moveAttempts >= MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES
                                      && state->page > 0)
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
                                  ? 0
                                  : MOVE_DELAY_QS));
#else // !UC_BUILD
    emberAfNetworkEventControlSetDelayQS(emberAfPluginEndDeviceSupportMoveNetworkEventControls,
                                         (state->moveAttempts == 0
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
                                          || (state->moveAttempts >= MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES
                                              && state->page > 0)
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
                                          ? 0
                                          : MOVE_DELAY_QS));
#endif // UC_BUILD
  } else {
    emberAfAppPrintln("Max move limit reached nwk %d: %d",
                      networkIndex,
                      state->moveAttempts);
    emberAfStopMoveCallback();
  }
}

void emAfPluginEndDeviceSupportMoveInit(void)
{
  #ifdef UC_BUILD
  sl_zigbee_network_event_init(emberAfPluginEndDeviceSupportMoveNetworkEvents,
                               emberAfPluginEndDeviceSupportMoveNetworkEventHandler);
  #endif // UC_BUILD
}

bool emberAfMoveInProgressCallback(void)
{
#ifdef UC_BUILD
  return sl_zigbee_event_is_scheduled(emberAfPluginEndDeviceSupportMoveNetworkEvents);
#else
  return emberAfNetworkEventControlGetActive(emberAfPluginEndDeviceSupportMoveNetworkEventControls);
#endif  // UC_BUILD
}

bool emberAfStartMoveCallback(void)
{
  // The consumer of the emberAfPluginEndDeviceSupportPreNetworkMoveCallback
  // callback is given a chance to not issue a network move
  if (false == emberAfPluginEndDeviceSupportPreNetworkMoveCallback()) {
    if (!emberAfMoveInProgressCallback()) {
      scheduleMoveEvent();
      return true;
    }
  }
  return false;
}

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
// The first and last channel page to try to rejoin on depends on the find mode
// (2.4 GHz only, sub-GHz only or both).

static uint8_t getFirstPage()
{
  if (emberAfGetFormAndJoinSearchMode() == FIND_AND_JOIN_MODE_ALLOW_SUB_GHZ) {
    // In the sub-GHz only mode, start from the first sub-GHz page
    return EMBER_MIN_SUGBHZ_PAGE_NUMBER;
  } else {
    // In all other modes, start from page 0
    return 0;
  }
}

static uint8_t getLastPage()
{
  if (emberAfGetFormAndJoinSearchMode() == FIND_AND_JOIN_MODE_ALLOW_2_4_GHZ) {
    // In the 2.4 GHz only mode, page 0 is also the last page
    return 0;
  } else {
    // In all other modes, the last page is page the top sub-GHz page
    return EMBER_MAX_SUGBHZ_PAGE_NUMBER;
  }
}
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT

void emberAfStopMoveCallback(void)
{
  uint8_t networkIndex = emberGetCurrentNetwork();
  states[networkIndex].moveAttempts = 0;
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  states[networkIndex].page = getFirstPage();
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT

#ifdef UC_BUILD
  sl_zigbee_event_set_inactive(emberAfPluginEndDeviceSupportMoveNetworkEvents);
#else // !UC_BUILD
  emberEventControlSetInactive(emberAfPluginEndDeviceSupportMoveNetworkEventControls[networkIndex]);
#endif // UC_BUILD
}

static bool checkForWellKnownTrustCenterLinkKey(void)
{
#if !defined(ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY)
  EmberKeyStruct keyStruct;
  EmberStatus status = emberGetKey(EMBER_TRUST_CENTER_LINK_KEY, &keyStruct);

  const EmberKeyData smartEnergyWellKnownTestKey = SE_SECURITY_TEST_LINK_KEY;
  const EmberKeyData zigbeeAlliance09Key = ZIGBEE_PROFILE_INTEROPERABILITY_LINK_KEY;

  if (status != EMBER_SUCCESS) {
    // Assume by default we have a well-known key if we failed to retrieve it.
    // This will prevent soliciting a TC rejoin that might expose the network
    // key such that a passive attacker can obtain the key.  Better to be
    // conservative in this circumstance.
    return true;
  }

  if ((0 == MEMCOMPARE(emberKeyContents(&(keyStruct.key)),
                       emberKeyContents(&(smartEnergyWellKnownTestKey)),
                       EMBER_ENCRYPTION_KEY_SIZE))
      || (0 == MEMCOMPARE(emberKeyContents(&(keyStruct.key)),
                          emberKeyContents(&(zigbeeAlliance09Key)),
                          EMBER_ENCRYPTION_KEY_SIZE))) {
    return true;
  }
#endif // ALLOW_REJOINS_WITH_WELL_KNOWN_LINK_KEY

  return false;
}

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
typedef struct {
  uint8_t page;
  uint8_t channel;
} PgChan;

// A getChannelMask() helper, factored out of getChannelMask() because it is
// only needed in some cases.
static PgChan getNetworkParamsForChannelMask(void)
{
  EmberNodeType nodeTypeResult = EMBER_UNKNOWN_DEVICE;
  EmberNetworkParameters networkParams = { 0 };
  emberAfGetNetworkParameters(&nodeTypeResult, &networkParams);
  PgChan pgChan;

  pgChan.page = emberAfGetPageFrom8bitEncodedChanPg(networkParams.radioChannel);
  pgChan.channel = emberAfGetChannelFrom8bitEncodedChanPg(networkParams.radioChannel);
  return pgChan;
}

// Get the channel mask for the current move attempt, following the sequence
// outlined in the comment in the struct State definition above.
static uint32_t getChannelMask(const State *state)
{
  PgChan pgChan;
  uint32_t pageMask;

  switch (state->moveAttempts) {
    case 0:
      // The first rejoin attempt. Use the current channel.
      pgChan = getNetworkParamsForChannelMask();
      return EMBER_PAGE_CHANNEL_MASK_FROM_CHANNEL_NUMBER(pgChan.page, pgChan.channel);
#if MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES > 1
    case 1:
      // The second attempt. Try rejoining on the current page.
      pgChan = getNetworkParamsForChannelMask();
      break;
#endif
    default:
      // Any further attempt. Try rejoining on all pages in turn.
      // The state machine determines the page for this round.
      pgChan.page = state->page;
      break;
  }

  // Ask the Network Find plugin what the channel mask is for the current page.
  // This code is conditionally included only if Network Find (Sub-GHz) exists
  // so this functionality is guaranteed to be present.
  pageMask = emberAfGetFormAndJoinChannelMask(pgChan.page);
  if (pageMask == 0xFFFFFFFFU) {
    // emberAfGetFormAndJoinChannelMask() returned an invalid page mask.
    // Return 0 for the current channel only. Rejoin will probably fail,
    // especially on sub-GHz, but since this is an impossible case anyway,
    // it does not really matter. Case included only for MISRA compliance.
    return 0;
  }
  return EMBER_PAGE_CHANNEL_MASK_FROM_CHANNEL_MASK(pgChan.page, pageMask);
}
#endif // NETWORK_FIND_SUB_GHZ

void emberAfPluginEndDeviceSupportMoveNetworkEventHandler(SLXU_UC_EVENT)
{
  const uint8_t networkIndex = emberGetCurrentNetwork();
  State *state = &states[networkIndex];
  EmberStatus status;
  bool secure = (state->moveAttempts == 0);
  if (!secure && checkForWellKnownTrustCenterLinkKey()) {
    emberAfDebugPrintln("Will not attempt TC rejoin due to well-known key.");
    secure = true;
  }

  EmberCurrentSecurityState securityState;
  if (emberGetCurrentSecurityState(&securityState) == EMBER_SUCCESS
      && (securityState.bitmask & EMBER_DISTRIBUTED_TRUST_CENTER_MODE) != 0) {
    emberAfDebugPrintln("A node on a distributed network cannot do a TC rejoin.");
    secure = true;
  }

#ifdef SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_COMMON_PRESENT
  if (emberZllOperationInProgress()
      || emberAfZllTouchLinkInProgress()) {
    // If we are trying to touchlink, then reschedule the rejoin attempt.
    emberAfDebugPrintln("Rescheduling move due to ZLL operation.");
#ifdef UC_BUILD
    sl_zigbee_event_set_delay_qs(emberAfPluginEndDeviceSupportMoveNetworkEvents,
                                 MOVE_DELAY_QS * 2);
#else // !UC_BUILD
    emberAfNetworkEventControlSetDelayQS(emberAfPluginEndDeviceSupportMoveNetworkEventControls,
                                         MOVE_DELAY_QS * 2);
#endif // UC_BUILD
    return;
  }
#endif // SL_CATALOG_ZIGBEE_ZLL_COMMISSIONING_COMMON_PRESENT

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  const uint32_t channels = getChannelMask(state);
#else
  const uint32_t channels = (state->moveAttempts == 0
                             ? 0 // current channel
                             : EMBER_ALL_802_15_4_CHANNELS_MASK);
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT

  status = emberFindAndRejoinNetworkWithReason(secure,
                                               channels,
                                               EMBER_AF_REJOIN_DUE_TO_END_DEVICE_MOVE);
  emberAfDebugPrintln("Move attempt %d nwk %d, channel mask 0x%4x: 0x%x",
                      state->moveAttempts,
                      networkIndex,
                      channels,
                      status);
  if (status == EMBER_SUCCESS) {
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
    if (state->moveAttempts < MOVE_ATTEMPTS_BEFORE_TRYING_ALL_PAGES
        || state->page == getLastPage()) {
      state->moveAttempts++;
      state->page = getFirstPage();
    } else if (state->page == 0) {
      // We can use hard-coded values in this block since we get here ONLY if
      // both modes are enabled and we need to switch from 2.4 GHz to sub-GHz.
      state->page = EMBER_MIN_SUGBHZ_PAGE_NUMBER;
    } else {
      state->page++;
    }
#else // !SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
    state->moveAttempts++;
#endif // SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  } else {
    scheduleMoveEvent();
  }
}

void emberAfPluginEndDeviceSupportStackStatusCallback(EmberStatus status)
{
  if (status == EMBER_NETWORK_UP) {
    emberAfStopMoveCallback();
    return;
  }
  if (!emberStackIsPerformingRejoin()) {
    EmberNetworkStatus state = emberAfNetworkState();
    if (state == EMBER_JOINED_NETWORK_NO_PARENT) {
      if (!emberAfPluginEndDeviceSupportLostParentConnectivityCallback()) {
        emberAfStartMoveCallback();
      }
    } else if (state == EMBER_NO_NETWORK) {
      emberAfStopMoveCallback();
    }
  }
}
