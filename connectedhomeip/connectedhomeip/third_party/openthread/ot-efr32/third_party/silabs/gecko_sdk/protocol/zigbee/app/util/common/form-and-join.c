/***************************************************************************//**
 * @file
 * @brief Utilities for forming and joining networks.
 * See form-and-join.h for a description of the exported interface.
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

#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros
#include "app/framework/include/af.h"
#include "app/framework/util/util.h"

#define __FORM_AND_JOIN_C__

#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
  #include "stack/include/ember-types.h"
  #include "app/util/ezsp/ezsp-protocol.h"
  #include "app/util/ezsp/ezsp.h"
  #include "app/util/ezsp/ezsp-utils.h"
  #include "app/util/ezsp/serial-interface.h"
#else
// Includes needed for ember related functions for the EM250
  #include "stack/include/ember-types.h"
#endif // EZSP_HOST

#include "stack/include/error.h"
#include "hal/hal.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#include "zigbee_stack_callback_dispatcher.h"
#include "zigbee_app_framework_callback.h"
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT
#include "network-find.h"
#if (EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_CALLBACK == 1)
#define RADIO_TX_CALLBACK
#endif
#endif
#else // !UC_BUILD
#include "plugin/serial/serial.h"
#ifdef EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ
#define SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
#endif
#ifdef EMBER_AF_PLUGIN_NETWORK_FIND
#define SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT
#include "app/framework/plugin/network-find/network-find.h"
#ifdef EMBER_AF_PLUGIN_NETWORK_FIND_RADIO_TX_CALLBACK
#define RADIO_TX_CALLBACK
#endif
#endif
#endif // UC_BUILD

#include "form-and-join.h"
#include "form-and-join-adapter.h"

#if defined RADIO_TX_CALLBACK
  #define GET_PRO2PLUS_RADIO_TX_POWER(chanPg)   emberAfPluginNetworkFindGetRadioPowerForChannelCallback(chanPg)
#elif defined SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
#if UC_BUILD
#include "network-find-sub-ghz-config.h"
#endif // UC_BUILD
  #define GET_PRO2PLUS_RADIO_TX_POWER(chanPg)   EMBER_AF_PLUGIN_NETWORK_FIND_SUB_GHZ_RADIO_TX_POWER
#else
// The radio power options are defined in the Network Find plugin, which may
// or may not be included. If it isn't, then revert to some sensible default.
  #define GET_PRO2PLUS_RADIO_TX_POWER(chanPg)   0
#endif

WEAK(void emberUnusedPanIdFoundHandler(
       // The unused panID which has been found.
       EmberPanId panId,
       // The channel that the unused panID was found on.
       uint8_t channel)){
}

// We can't include ember.h or ezsp.h from this file since it is used
// for both host and node-size libraries.  However the emberStartScan()
// API is identical in both.
extern sl_status_t emberStartScan(uint8_t scanType,
                                  uint32_t channelMask,
                                  uint8_t duraiton);

enum {
  FORM_AND_JOIN_NOT_SCANNING,
  FORM_AND_JOIN_NEXT_NETWORK,
  FORM_AND_JOIN_ENERGY_SCAN,
  FORM_AND_JOIN_PAN_ID_SCAN,
  FORM_AND_JOIN_JOINABLE_SCAN,
};

static uint8_t formAndJoinScanType = FORM_AND_JOIN_NOT_SCANNING;

struct ChannelCandidate {
  uint8_t chanPg;       // 8-bit encoded page and channel
  int8_t rssi;          // channel energy (RSSI) in dBm
};

static uint8_t *dataContents;
#define panIdCandidates         ((uint16_t *) dataContents)
#define channelCandidates       ((struct ChannelCandidate*) dataContents)

// The number of candidate on the list is limited by the size of the buffer (32)
// and the size of each candidate
#define USABLE_CHANNEL_CANDIDATE_LIMIT  (32 / sizeof(struct ChannelCandidate))

static uint8_t extendedPanIdCache[8];
static uint32_t channelMaskCache;
static uint8_t channelCache;    // 8-bit encoded in forming mode
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
static uint8_t scanDurationCache;
#endif
static bool ignoreExtendedPanId;
static uint8_t networkCount;    // The number of NetworkInfo records.

// The minimum significant difference between energy scan results.
// Results that differ by less than this are treated as identical.
#define ENERGY_SCAN_FUZZ 25

#define NUM_PAN_ID_CANDIDATES 16  // Must fit into one packet buffer on the node

#define INVALID_PAN_ID 0xFFFF

// The scan duration differs on 2.4 GHz and on sub-GHz.
// Use the top bits of the (32-bit) page mask to work out which one to use.
#define ACTIVE_SCAN_DURATION(pageMask)  ((pageMask) == 0              \
                                         ? EMBER_ACTIVE_SCAN_DURATION \
                                         : EMBER_SUB_GHZ_SCAN_DURATION)

//------------------------------------------------------------------------------
// Macros for enabling or disabling debug print messages.
// #define SCAN_DEBUG_PRINT_ENABLE

#ifdef SCAN_DEBUG_PRINT_ENABLE
// define this if printing messages -- note serial port used is 1
   #define SCAN_DEBUG_MSG(x, y) emberSerialPrintf(1, x, y); emberSerialWaitSend(1)
   #define SCAN_DEBUG(x) emberSerialPrintf(1, x); emberSerialWaitSend(1)

   #define SCAN_DEBUG_XPAN_PRINT(xpan)                            \
  do {                                                            \
    (void) emberSerialPrintf(1, "%x%x%x%x%x%x%x%x",               \
                             xpan[0], xpan[1], xpan[2], xpan[3],  \
                             xpan[4], xpan[5], xpan[6], xpan[7]); \
    (void) emberSerialWaitSend(1);                                \
  } while (false)

#elif defined(EMBER_SCRIPTED_TEST)
void debug(const char* format, ...);
bool isTestFrameworkDebugOn(void);

  #define SCAN_DEBUG_MSG(x, y) debug(x, y)
  #define SCAN_DEBUG(x) debug(x)
  #define SCAN_DEBUG_XPAN_PRINT(xpan)          \
  do {                                         \
    debug("%x%x%x%x%x%x%x%x",                  \
          xpan[0], xpan[1], xpan[2], xpan[3],  \
          xpan[4], xpan[5], xpan[6], xpan[7]); \
  } while (false)

#else
// define this to turn off printing messages
   #define SCAN_DEBUG_MSG(x, ...)
   #define SCAN_DEBUG(x)
   #define SCAN_DEBUG_XPAN_PRINT(x)
#endif

//------------------------------------------------------------------------------
// Forward declarations for the benefit of the compiler.

static void saveNetwork(EmberZigbeeNetwork *network, uint8_t lqi, int8_t rssi);
static bool setup(uint8_t scanType);
static EmberStatus startScan(EmberNetworkScanType type, uint32_t mask, uint8_t duration);
static void startPanIdScan(void);
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
static EmberStatus startSecondInterface(void);
#endif
static bool isArrayZero(uint8_t* array, uint8_t size);

static void printChannelMaskInfo(uint32_t channelMask)
{
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  emberAfAppPrint("Page %d %p", channelMask >> EMBER_MAX_CHANNELS_PER_PAGE, "channels: ");
#else
  emberAfAppPrint("Channels: ");
#endif
  emberAfAppDebugExec(emberAfPrintChannelListFromMask(channelMask));
  emberAfAppPrintln("");
}

//------------------------------------------------------------------------------
// Finding unused PAN ids.

static EmberStatus startScanForUnusedPanId(uint32_t channelMask, uint8_t duration)
{
  printChannelMaskInfo(channelMask);
  channelMaskCache = channelMask;
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  scanDurationCache = duration;
#endif
  return startScan(EMBER_ENERGY_SCAN, channelMask, duration);
}

EmberStatus emberScanForUnusedPanId(uint32_t channelMask, uint8_t duration)
{
  EmberStatus status = EMBER_ERR_FATAL;
  if (setup(FORM_AND_JOIN_ENERGY_SCAN)) {
    uint8_t i;
    for (i = 0; i < USABLE_CHANNEL_CANDIDATE_LIMIT; i++) {
      channelCandidates[i].chanPg = 0;
      channelCandidates[i].rssi = 127;          // highest positive 8-bit number
    }
    status = startScanForUnusedPanId(channelMask, duration);
  }
  return status;
}

// Add a new candidate into the list of candidates.
// The idea is to always replace the candidate with the highest energy level
// with the new candidate, as long as the new candidate is quieter.
// The net effect may well be that the list remains unchanged, if all channels
// currently on the list are quitere than the new candidate.
// This algorithm assumes that candidate list is preinitialized with all energy
// levels set to the highest positive numbers before the first call.

static void addCandidate(uint8_t channel, int8_t rssi)
{
  uint8_t highestEnergyIndex = 0;
  int8_t highestEnergy = -128;                  // lowest negative 8-bit number
  uint8_t i;

  for (i = 0; i < USABLE_CHANNEL_CANDIDATE_LIMIT; i++) {
    if (channelCandidates[i].rssi > highestEnergy) {
      highestEnergyIndex = i;
      highestEnergy = channelCandidates[i].rssi;
    }
  }

  if (rssi < highestEnergy) {
    channelCandidates[highestEnergyIndex].chanPg = channel;
    channelCandidates[highestEnergyIndex].rssi = rssi;
  }
}

// Pick a channel from among those with the lowest energy and then look for
// a PAN ID not in use on that channel.
//
// The energy scans are not particularly accurate, especially as we don't run
// them for very long, so we add in some slop to the measurements and then pick
// a random channel from the least noisy ones.  This avoids having several
// coordinators pick the same slightly quieter channel.

static void energyScanComplete(void)
{
  uint8_t channelIndex;
  int8_t lowestEnergy;
  int8_t cutoff;
  uint8_t candidateCount;
  uint8_t i;

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  uint8_t currentPage = channelMaskCache >> EMBER_MAX_CHANNELS_PER_PAGE;
  SCAN_DEBUG_MSG("Scan complete on channel %d,", currentPage);
  SCAN_DEBUG_MSG(" mask 0x%4x\n", channelMaskCache & 0x07FFFFFFUL);

  // Decide whether we need to proceed to the next channel page.
  // This can be a bit tricky. We have the flexibility of the search mode,
  // we have two different way of forming the network on single and dual-PHY
  // architectures, and we have the forming code spread across two files,
  // one of which is an optional plugin.
  switch (emberAfGetFormAndJoinSearchMode()) {
    case FIND_AND_JOIN_MODE_ALLOW_BOTH:
      if (currentPage == 0) {
        const uint8_t phyInterfaceCount = emberGetPhyInterfaceCount();
        if (phyInterfaceCount > 1 && phyInterfaceCount != 0xFF) {
          // That's it, proceed to form the network on 2.4 GHz
          break;
        } else {
          // Continue on the first sub-GHz page (minus 1 because we increment
          // after the fall-through)
          currentPage = EMBER_MIN_SUGBHZ_PAGE_NUMBER - 1;
        }
      }
    // Deliberately fall through...

    case FIND_AND_JOIN_MODE_ALLOW_SUB_GHZ:
      // Finished scanning the current page, try the next page if available
      if (currentPage < EMBER_MAX_SUGBHZ_PAGE_NUMBER) {
        currentPage++;
        SCAN_DEBUG_MSG("%p on page ", "Continue energy scan");
        SCAN_DEBUG_MSG("%d\n", currentPage);
        if (startScanForUnusedPanId((((uint32_t)currentPage) << EMBER_MAX_CHANNELS_PER_PAGE) | emAfGetSearchForUnusedNetworkChannelMask(currentPage),
                                    scanDurationCache) == EMBER_SUCCESS) {
          return;
        }
      }
      break;

    case FIND_AND_JOIN_MODE_ALLOW_2_4_GHZ:
    default:
      // Nothing else to do, proceed to form the network
      break;
  }
#endif

  // Energy scan of all pages complete. Form the network.
  SCAN_DEBUG("Energy scan complete for all pages\n");

  // No more pages to scan. Weed out the candidate list.
  // We only consider the candidates in the narrow band between the lowest
  // energy level and the lowest level plus the "fuzz" value.
  lowestEnergy = channelCandidates[0].rssi;
  for (i = 1; i < USABLE_CHANNEL_CANDIDATE_LIMIT; i++) {
    if (channelCandidates[i].rssi < lowestEnergy) {
      lowestEnergy = channelCandidates[i].rssi;
    }
  }

  // Count how many candidates are left below the cut-off point.
  // If for some reason we never got any energy scan results
  // then our candidate count will be 0.  We want to avoid that case and
  // bail out (since we would do a divide by 0 next)
  cutoff = (lowestEnergy <= 127 - ENERGY_SCAN_FUZZ)
           ? lowestEnergy + ENERGY_SCAN_FUZZ
           : 127;       // redundant safety to prevent integer overflow
  candidateCount = 0;
  for (i = 0; i < USABLE_CHANNEL_CANDIDATE_LIMIT; i++) {
    if (channelCandidates[i].rssi < cutoff) {
      candidateCount++;
      SCAN_DEBUG_MSG("Candidate %d: ", candidateCount);         // 1-based index
      SCAN_DEBUG_MSG("%d ", channelCandidates[i].chanPg);       // 8-bit endoced channel + page
      SCAN_DEBUG_MSG("(0x%x), ", channelCandidates[i].chanPg);  // same in hex
      SCAN_DEBUG_MSG("%d dBm\n", channelCandidates[i].rssi);    // RSSI
    }
  }

  if (candidateCount == 0) {
#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT
    // The cleanup eventually calls emberAfScanErrorCallback(). If Network Find
    // is present, it handles that callback, using EMBER_NOT_FOUND as a special
    // case. Otherwise, use EMBER_ERR_FATAL for backwards compatibility.
    emberFormAndJoinCleanup(EMBER_NOT_FOUND);
#else
    emberFormAndJoinCleanup(EMBER_ERR_FATAL);
#endif
    return;
  }

  // Choose a random channel from the candidates and form the network.
  // The results of the choice are stored in channelMaskCache and channelCache
  // and used by startPanIdScan() or startSecondInterface().
  channelIndex = emberGetPseudoRandomNumber() % candidateCount;
  SCAN_DEBUG_MSG("Choosing candidate %d from the list\n", channelIndex + 1);

  for (i = 0; i < USABLE_CHANNEL_CANDIDATE_LIMIT; i++) {
    if (channelCandidates[i].rssi < cutoff) {
      if (channelIndex == 0) {
        channelCache = channelCandidates[i].chanPg;
        break;
      } else {
        channelIndex -= 1;
      }
    }
  }

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
  if (channelCache > EMBER_MAX_802_15_4_CHANNEL_NUMBER) {
    const uint8_t phyInterfaceCount = emberGetPhyInterfaceCount();
    if (phyInterfaceCount > 1 && phyInterfaceCount != 0xFF) {
      // We should only get here when forming a network on a multi-PHY device,
      // when we have already formed the network on 2.4 GHz and are about to
      // start the second interface.
      // The additional check for 0xFF is a sanity check to cover any potential
      // EZSP errors (EZSP calls return 0xFF to indicate some failures).
      EmberStatus status = startSecondInterface();
      emberFormAndJoinCleanup(status);
      return;
    }
  }
#endif

  SCAN_DEBUG("Starting PAN scan\n");
  startPanIdScan();
}

// PAN IDs can be 0..0xFFFE.  We pick some trial candidates and then do a scan
// to find one that is not in use.

static void startPanIdScan(void)
{
  // Convert 8-bit encoded channelCache into a 32-bit channel mask required by
  // startScan().
  const uint32_t pageMask = emberAfGetPageFrom8bitEncodedChanPg(channelCache) << EMBER_MAX_CHANNELS_PER_PAGE;
  uint8_t i = 0;

  while (i < NUM_PAN_ID_CANDIDATES) {
    uint16_t panId = emberGetPseudoRandomNumber() & 0xFFFF;
    if (panId != 0xFFFF) {
      SCAN_DEBUG_MSG("panIdCandidate: 0x%2X\n", panId);
      panIdCandidates[i] = panId;
      i++;
    }
  }

  formAndJoinScanType = FORM_AND_JOIN_PAN_ID_SCAN;
  startScan(EMBER_ACTIVE_SCAN,
            pageMask | BIT32(emberAfGetChannelFrom8bitEncodedChanPg(channelCache)),
            ACTIVE_SCAN_DURATION(pageMask));
}

// Form a network using one of the unused PAN IDs.  If we got unlucky we
// pick some more and try again.
#ifdef UC_BUILD
static void panIdScanComplete(void)
{
  uint8_t i;

  for (i = 0; i < NUM_PAN_ID_CANDIDATES; i++) {
    if (panIdCandidates[i] != 0xFFFF) {
      const EmberPanId panId = panIdCandidates[i];
#ifdef EZSP_HOST
      ezspUnusedPanIdFoundHandler(panId, channelCache);
#else // !EZSP_HOST
      emberUnusedPanIdFoundHandler(panId, channelCache);
#endif  // EZSP_HOST
      return;
    }
  }
  // XXX: Do we care this could keep happening forever?
  // In practice there couldn't be as many PAN IDs heard that
  // conflict with ALL our randomly selected set of candidate PANs.
  // But in theory we could get the same random set of numbers
  // (more likely due to a bug) and we could hear the same set of
  // PAN IDs that conflict with our random set.

  startPanIdScan();     // Start over with new candidates.
}
#else
static void panIdScanComplete(void)
{
  uint8_t i;

  for (i = 0; i < NUM_PAN_ID_CANDIDATES; i++) {
    if (panIdCandidates[i] != 0xFFFF) {
      const EmberPanId panId = panIdCandidates[i];
      emberFormAndJoinCleanup(EMBER_SUCCESS);
      emberAfUnusedPanIdFoundCallback(panId, channelCache);
      return;
    }
  }

  // XXX: Do we care this could keep happening forever?
  // In practice there couldn't be as many PAN IDs heard that
  // conflict with ALL our randomly selected set of candidate PANs.
  // But in theory we could get the same random set of numbers
  // (more likely due to a bug) and we could hear the same set of
  // PAN IDs that conflict with our random set.

  startPanIdScan();     // Start over with new candidates.
}
#endif // UC_BUILD

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_SUB_GHZ_PRESENT
static EmberStatus startSecondInterface(void)
{
  EmberStatus status;
  const uint8_t page = emberAfGetPageFrom8bitEncodedChanPg(channelCache);
  const uint8_t channel = emberAfGetChannelFrom8bitEncodedChanPg(channelCache);

  emberAfAppPrint("Starting 2nd interface on page %d, channel %d... ",
                  page,
                  channel);

  status = emberMultiPhyStart(PHY_INDEX_PRO2PLUS,
                              page,
                              channel,
                              GET_PRO2PLUS_RADIO_TX_POWER(channelCache),
                              0 /*bitmask*/);
  if (status == EMBER_INVALID_CALL) {   // interface already running
    status = emberMultiPhySetRadioChannel(PHY_INDEX_PRO2PLUS,
                                          page,
                                          channel);
  }

  if (status == EMBER_SUCCESS) {
    emberAfAppPrintln("Success!");
  } else {
    emberAfAppPrintln("Error 0x%x", status);
  }

  emAfSecondaryInterfaceFormedCallback(status);
  return status;
}
#endif

//------------------------------------------------------------------------------
// Finding joinable networks

EmberStatus emberScanForJoinableNetwork(uint32_t channelMask, uint8_t* extendedPanId)
{
  if (!setup(FORM_AND_JOIN_NEXT_NETWORK)) {
    return EMBER_INVALID_CALL;
  }

  if ((channelMask & EMBER_ALL_CHANNEL_PAGE_MASK) == 0) {
    channelCache = EMBER_MIN_802_15_4_CHANNEL_NUMBER;   // 2.4 GHz
  } else {
    channelCache = EMBER_MIN_SUBGHZ_CHANNEL_NUMBER;     // sub-GHz
  }

  printChannelMaskInfo(channelMask);
  channelMaskCache = channelMask;

  if (extendedPanId == NULL
      || isArrayZero(extendedPanId, EXTENDED_PAN_ID_SIZE)) {
    ignoreExtendedPanId = true;
  } else {
    ignoreExtendedPanId = false;
    MEMMOVE(extendedPanIdCache, extendedPanId, EXTENDED_PAN_ID_SIZE);
  }

  return emberScanForNextJoinableNetwork();
}

EmberStatus emberScanForNextJoinableNetwork(void)
{
  if (formAndJoinScanType != FORM_AND_JOIN_NEXT_NETWORK) {
    emberScanErrorHandler(EMBER_INVALID_CALL);
    return EMBER_INVALID_CALL;
  }

  // Check for cached networks first.
  while (networkCount != 0U) {
    NetworkInfo *finger = formAndJoinGetNetworkPointer(--networkCount);
    if (finger->network.panId != 0xFFFF) {
      emberJoinableNetworkFoundHandler(&(finger->network), finger->lqi, finger->rssi);
      formAndJoinSetBufferLength(networkCount);
      formAndJoinSetCleanupTimeout();
      return EMBER_SUCCESS;
    }
    formAndJoinSetBufferLength(networkCount);
  }

  // Find the next channel in the mask and start scanning.
  // This may be slightly complicated by the fact that we now have 5 pages:
  // one 2.4GHz and four sub-GHz, and some pages may have a different number of
  // channels than others. The simplest solution is to always go to the highest
  // channel and apply the mask that varies from page to page.
  while (channelCache < EMBER_MAX_CHANNELS_PER_PAGE) {
    const uint32_t bitMask = BIT32(channelCache);
    ++channelCache;
    if ((bitMask & channelMaskCache) != 0U) {
      const uint32_t pageMask = channelMaskCache & EMBER_ALL_CHANNEL_PAGE_MASK;
      formAndJoinScanType = FORM_AND_JOIN_JOINABLE_SCAN;
      return startScan(EMBER_ACTIVE_SCAN,
                       pageMask | bitMask,
                       ACTIVE_SCAN_DURATION(pageMask));
    }
  }

  // Notify the app we're completely out of networks.
  emberFormAndJoinCleanup(EMBER_NO_BEACONS);
  return EMBER_SUCCESS;
}

//------------------------------------------------------------------------------
// Callbacks

#ifdef UC_BUILD
void emAfPluginFormAndJoinScanCompleteCallback(uint8_t channel, EmberStatus status)
{
  if (!emberFormAndJoinIsScanning()) {
    return;
  }

  if (FORM_AND_JOIN_ENERGY_SCAN != formAndJoinScanType) {
    // This scan is an Active Scan.
    // Active Scans potentially report transmit failures through this callback.
    if (EMBER_SUCCESS != status) {
      // The Active Scan is still in progress.  This callback is informing us
      // about a failure to transmit the beacon request on this channel.
      // If necessary we could save this failing channel number and start
      // another Active Scan on this channel later (after this current scan is
      // complete).
      return;
    }
  }

  switch (formAndJoinScanType) {
    case FORM_AND_JOIN_ENERGY_SCAN:
      energyScanComplete();
      break;

    case FORM_AND_JOIN_PAN_ID_SCAN:
      panIdScanComplete();
      break;

    case FORM_AND_JOIN_JOINABLE_SCAN:
      formAndJoinScanType = FORM_AND_JOIN_NEXT_NETWORK;
      emberScanForNextJoinableNetwork();
      break;

    default:
      // MISRA requires default case.
      break;
  }
}
#else
bool emberFormAndJoinScanCompleteHandler(uint8_t channel, EmberStatus status)
{
  if (!emberFormAndJoinIsScanning()) {
    return false;
  }

  if (FORM_AND_JOIN_ENERGY_SCAN != formAndJoinScanType) {
    // This scan is an Active Scan.
    // Active Scans potentially report transmit failures through this callback.
    if (EMBER_SUCCESS != status) {
      // The Active Scan is still in progress.  This callback is informing us
      // about a failure to transmit the beacon request on this channel.
      // If necessary we could save this failing channel number and start
      // another Active Scan on this channel later (after this current scan is
      // complete).
      return false;
    }
  }

  switch (formAndJoinScanType) {
    case FORM_AND_JOIN_ENERGY_SCAN:
      energyScanComplete();
      break;

    case FORM_AND_JOIN_PAN_ID_SCAN:
      panIdScanComplete();
      break;

    case FORM_AND_JOIN_JOINABLE_SCAN:
      formAndJoinScanType = FORM_AND_JOIN_NEXT_NETWORK;
      emberScanForNextJoinableNetwork();
      break;

    default:
      // MISRA requires default case.
      break;
  }
  return true;
}
#endif // UC_BUILD

// We are either looking for PAN IDs or for joinable networks.  In the first
// case we just check the found PAN ID against our list of candidates.
#ifdef UC_BUILD
void emAfPluginFormAndJoinNetworkFoundCallback(EmberZigbeeNetwork *networkFound,
                                               uint8_t lqi,
                                               int8_t rssi)
{
  uint8_t i;

  SCAN_DEBUG_MSG("SCAN: nwk found ch %d, ", networkFound->channel);
  SCAN_DEBUG_MSG("panID 0x%04X, xpan: ", networkFound->panId);
  SCAN_DEBUG_XPAN_PRINT(networkFound->extendedPanId);
  SCAN_DEBUG_MSG(", lqi %d", lqi);
  SCAN_DEBUG_MSG(", profile: %d", networkFound->stackProfile);
  SCAN_DEBUG_MSG(", pjoin: %d", networkFound->allowingJoin);
  SCAN_DEBUG("\n");

  switch (formAndJoinScanType) {
    case FORM_AND_JOIN_PAN_ID_SCAN:
      for (i = 0; i < NUM_PAN_ID_CANDIDATES; i++) {
        if (panIdCandidates[i] == networkFound->panId) {
          panIdCandidates[i] = 0xFFFF;
        }
      }
      break;
    case FORM_AND_JOIN_JOINABLE_SCAN:
      // check for a beacon with permit join on...
      if (networkFound->allowingJoin
          // ...the same stack profile as this application...
          && networkFound->stackProfile == formAndJoinStackProfile()
          && (// ...and ignore the Extended PAN ID, or...
            ignoreExtendedPanId
            // ...a matching Extended PAN ID
            || (MEMCOMPARE(extendedPanIdCache,
                           networkFound->extendedPanId,
                           EXTENDED_PAN_ID_SIZE) == 0))) {
        saveNetwork(networkFound, lqi, rssi);
        SCAN_DEBUG(": MATCH\r\n");
      } else {
        SCAN_DEBUG(": NO match\r\n");
      }
      break;
    default:
      // MISRA requires default case.
      break;
  }
}
#else
bool emberFormAndJoinNetworkFoundHandler(EmberZigbeeNetwork *networkFound,
                                         uint8_t lqi,
                                         int8_t rssi)
{
  uint8_t i;

  SCAN_DEBUG_MSG("SCAN: nwk found ch %d, ", networkFound->channel);
  SCAN_DEBUG_MSG("panID 0x%2x, xpan: ", networkFound->panId);
  SCAN_DEBUG_XPAN_PRINT(networkFound->extendedPanId);
  SCAN_DEBUG_MSG(", lqi %d", lqi);
  SCAN_DEBUG_MSG(", profile: %d", networkFound->stackProfile);
  SCAN_DEBUG_MSG(", pjoin: %d", networkFound->allowingJoin);
  SCAN_DEBUG("\n");

  switch (formAndJoinScanType) {
    case FORM_AND_JOIN_PAN_ID_SCAN:
      for (i = 0; i < NUM_PAN_ID_CANDIDATES; i++) {
        if (panIdCandidates[i] == networkFound->panId) {
          panIdCandidates[i] = 0xFFFF;
        }
      }
      break;

    case FORM_AND_JOIN_JOINABLE_SCAN:
      // check for a beacon with permit join on...
      if (networkFound->allowingJoin
          // ...the same stack profile as this application...
          && networkFound->stackProfile == formAndJoinStackProfile()
          && (// ...and ignore the Extended PAN ID, or...
            ignoreExtendedPanId
            // ...a matching Extended PAN ID
            || (MEMCOMPARE(extendedPanIdCache,
                           networkFound->extendedPanId,
                           EXTENDED_PAN_ID_SIZE) == 0))) {
        saveNetwork(networkFound, lqi, rssi);
        SCAN_DEBUG(": MATCH\r\n");
      } else {
        SCAN_DEBUG(": NO match\r\n");
      }
      break;

    default:
      // MISRA requires default case.
      break;
  }
  return emberFormAndJoinIsScanning();
}
#endif // UC_BUILD

// Just remember the result.
#ifdef UC_BUILD
void emAfPluginFormAndJoinEnergyScanResultCallback(uint8_t channel, int8_t maxRssiValue)
{
  if (!emberFormAndJoinIsScanning()) {
    return;
  }

  SCAN_DEBUG_MSG("SCAN: found energy %d dBm on ", maxRssiValue);
  SCAN_DEBUG_MSG("channel 0x%x", channel);

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT
  if (emberAfPluginNetworkFindGetEnableScanningAllChannelsCallback()
      && !emAfIsCurrentSearchForUnusedNetworkScanningAllChannels()
      && maxRssiValue > emberAfPluginNetworkFindGetEnergyThresholdForChannelCallback(channel)) {
    SCAN_DEBUG(", not suitable (above cut-off value)");
    SCAN_DEBUG("\r\n");
    return;
  }
#endif

  addCandidate(channel, maxRssiValue);
  SCAN_DEBUG("\r\n");
}
#else
bool emberFormAndJoinEnergyScanResultHandler(uint8_t channel, int8_t maxRssiValue)
{
  if (emberFormAndJoinIsScanning()) {
    SCAN_DEBUG_MSG("SCAN: found energy %d dBm on ", maxRssiValue);
    SCAN_DEBUG_MSG("channel 0x%x", channel);

#ifdef SL_CATALOG_ZIGBEE_NETWORK_FIND_PRESENT
    if (emberAfPluginNetworkFindGetEnableScanningAllChannelsCallback()
        && !emAfIsCurrentSearchForUnusedNetworkScanningAllChannels()
        && maxRssiValue > emberAfPluginNetworkFindGetEnergyThresholdForChannelCallback(channel)) {
      SCAN_DEBUG(", not suitable (above cut-off value)");
    } else
#endif
    {
      addCandidate(channel, maxRssiValue);
    }

    SCAN_DEBUG("\r\n");
    return true;
  }
  return false;
}
#endif // UC_BUILD

#ifdef UC_BUILD
void emAfPluginFormAndJoinUnusedPanIdFoundCallback(EmberPanId panId, uint8_t channel)
{
  if (emberFormAndJoinIsScanning()) {
    emberFormAndJoinCleanup(EMBER_SUCCESS);
  }
}
#else
bool emberFormAndJoinUnusedPanIdFoundHandler(EmberPanId panId, uint8_t channel)
{
  if (emberFormAndJoinIsScanning()) {
    emberFormAndJoinCleanup(EMBER_SUCCESS);
    emberAfUnusedPanIdFoundCallback(panId, channel);
    return(true);
  }
  return(false);
}
#endif // UC_BUILD

//------------------------------------------------------------------------------
// Helper functions

static void saveNetwork(EmberZigbeeNetwork *network, uint8_t lqi, int8_t rssi)
{
  uint8_t i;
  NetworkInfo *finger;

  // See if we already have that network.
  for (i = 0; i < networkCount; i++) {
    finger = formAndJoinGetNetworkPointer(i);
    if (MEMCOMPARE(finger->network.extendedPanId,
                   network->extendedPanId,
                   EXTENDED_PAN_ID_SIZE) == 0) {
      return;
    }
  }

  if (formAndJoinSetBufferLength(networkCount + 1) != EMBER_SUCCESS) {
    return;
  }
  finger = formAndJoinGetNetworkPointer(networkCount);
  networkCount += 1;
  MEMMOVE(finger, network, sizeof(EmberZigbeeNetwork));
  finger->lqi = lqi;
  finger->rssi = rssi;
}

bool emberFormAndJoinIsScanning(void)
{
  return (formAndJoinScanType > FORM_AND_JOIN_NEXT_NETWORK);
}

bool emberFormAndJoinCanContinueJoinableNetworkScan(void)
{
  return (formAndJoinScanType == FORM_AND_JOIN_NEXT_NETWORK);
}

static bool setup(uint8_t scanType)
{
  if (emberFormAndJoinIsScanning()) {
    emberScanErrorHandler(EMBER_MAC_SCANNING);
    return false;
  }

  // Case 12903: Need to reset the cleanup timeout when initiating a new scan
  // since a previous scan process may have concluded before the cleanup event
  // timer ran out, and we don't want it triggering in the middle of our new
  // scan and cleaning out legitimate data.
  // This is a special cause because emberFormAndJoinCleanup is occurring
  // directly rather than as a result of the cleanup event firing, so the
  // cleanup event timer isn't being deactivated like it would from the event
  // handler.  Since our only interface to manipulate the cleanup event
  // timer is to restart it (rather than deactivate it), we do that here.
  formAndJoinSetCleanupTimeout();
  emberFormAndJoinCleanup(EMBER_SUCCESS);  // In case we were in NEXT_NETWORK mode.
  networkCount = 0;
  formAndJoinScanType = scanType;
  dataContents = formAndJoinAllocateBuffer();
  if (dataContents == NULL) {
    emberFormAndJoinCleanup(EMBER_NO_BUFFERS);
    return false;
  }
  return true;
}

void emberFormAndJoinCleanup(EmberStatus status)
{
  formAndJoinScanType = FORM_AND_JOIN_NOT_SCANNING;
  SCAN_DEBUG("formAndJoinReleaseBuffer()\n");
  formAndJoinReleaseBuffer();
  if (status != EMBER_SUCCESS) {
    emberScanErrorHandler(status);
  }
}

static EmberStatus startScan(EmberNetworkScanType type, uint32_t mask, uint8_t duration)
{
  EmberStatus status = emberStartScan(type, mask, duration);
  SCAN_DEBUG_MSG("SCAN: start scan, status 0x%x\r\n", status);
  if (status != EMBER_SUCCESS) {
    emberFormAndJoinCleanup(status);
  }
  return status;
}

bool isArrayZero(uint8_t* array, uint8_t size)
{
  uint8_t i;
  for (i = 0; i < size; i++) {
    if (array[i] != 0) {
      return false;
    }
  }
  return true;
}
