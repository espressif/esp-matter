/*
 *  Copyright (c) 2021, The OpenThread Authors.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the copyright holder nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file
 *   This file implements the OpenThread platform abstraction for radio communication.
 *
 */

#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

#include <assert.h>
#include "openthread-system.h"
#include <openthread/config.h>
#include <openthread/link.h>
#include <openthread/platform/alarm-micro.h>
#include <openthread/platform/alarm-milli.h>
#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>
#include <openthread/platform/time.h>

#include "common/logging.hpp"
#include "utils/code_utils.h"
#include "utils/link_metrics.h"
#include "utils/mac_frame.h"

#include "utils/soft_source_match_table.h"

#include "board_config.h"
#include "em_core.h"
#include "em_system.h"
#include "pa_conversions_efr32.h"
#include "platform-band.h"
#include "platform-efr32.h"
#include "rail.h"
#include "rail_config.h"
#include "rail_ieee802154.h"
#include "sl_packet_utils.h"
#include "ieee802154mac.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
#include "sl_rail_mux_rename.h"
#endif

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
#include "sl_rail_util_ant_div.h"
#include "sl_rail_util_ant_div_config.h"
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
#include "coexistence-802154.h"
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
#include "sl_rail_util_ieee802154_stack_event.h"
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT
#include "sl_rail_util_ieee802154_phy_select.h"
#endif // #ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT
//------------------------------------------------------------------------------
// Enums, macros and static variables

#ifndef LOW_BYTE
#define LOW_BYTE(n) ((uint8_t)((n) & 0xFF))
#endif //LOW_BTE

#ifndef HIGH_BYTE
#define HIGH_BYTE(n) ((uint8_t)(LOW_BYTE((n) >> 8)))
#endif //HIGH_BYTE

//Intentionally maintaining separate groups for series-1 and series-2 devices
//This gives flexibility to add new elements to be read, like CCA Thresholds. 
#if defined(_SILICON_LABS_32B_SERIES_1)
#define USERDATA_MFG_CUSTOM_EUI_64          (2)
#elif defined(_SILICON_LABS_32B_SERIES_2)
#define USERDATA_MFG_CUSTOM_EUI_64          (2)
#else
#error "UNSUPPORTED DEVICE"
#endif

#ifndef USERDATA_END
#define USERDATA_END                        (USERDATA_BASE + FLASH_PAGE_SIZE)
#endif

#define EFR32_RECEIVE_SENSITIVITY           -100 // dBm
#define EFR32_RSSI_AVERAGING_TIME           16   // us
#define EFR32_RSSI_AVERAGING_TIMEOUT        300  // us

// Internal flags
#define FLAG_RADIO_INIT_DONE                0x0001
#define FLAG_ONGOING_TX_DATA                0x0002
#define FLAG_ONGOING_TX_ACK                 0x0004
#define FLAG_WAITING_FOR_ACK                0x0008
#define FLAG_SYMBOL_TIMER_RUNNING           0x0010     // Not used
#define FLAG_CURRENT_TX_USE_CSMA            0x0020
#define FLAG_DATA_POLL_FRAME_PENDING_SET    0x0040
#define FLAG_CALIBRATION_NEEDED             0x0080     // Not used
#define FLAG_IDLE_PENDING                   0x0100     // Not used

#define TX_COMPLETE_RESULT_SUCCESS          0x00       // Not used
#define TX_COMPLETE_RESULT_CCA_FAIL         0x01
#define TX_COMPLETE_RESULT_OTHER_FAIL       0x02
#define TX_COMPLETE_RESULT_NONE             0xFF       // Not used

#define TX_WAITING_FOR_ACK                  0x00
#define TX_NO_ACK                           0x01

#define ONGOING_TX_FLAGS   (FLAG_ONGOING_TX_DATA | FLAG_ONGOING_TX_ACK)

#define QUARTER_DBM_IN_DBM 4
#define US_IN_MS 1000

/* FilterMask provided by RAIL is structured as follows:
 * |  Bit:7  |  Bit:6  |  Bit:5  |    Bit:4    |  Bit:3  |  Bit:2  |  Bit:1  |    Bit:0     |
 * |  Addr2  |  Addr1  |  Addr0  |  Bcast Addr | Pan Id2 | Pan Id1 | Pan Id0 | Bcast PanId  |
 * | Matched | Matched | Matched |   Matched   | Matched | Matched | Matched |   Matched    |
 */

#define RADIO_BCAST_IID                      (0)
#define RADIO_GET_FILTER_MASK(iid)           (1 << (iid))

#define RADIO_BCAST_PANID_FILTER_MASK        RADIO_GET_FILTER_MASK(0)
#define RADIO_INDEX0_PANID_FILTER_MASK       RADIO_GET_FILTER_MASK(1)
#define RADIO_INDEX1_PANID_FILTER_MASK       RADIO_GET_FILTER_MASK(2)
#define RADIO_INDEX2_PANID_FILTER_MASK       RADIO_GET_FILTER_MASK(3)

#define RADIO_BCAST_ADDR_FILTER_MASK         (RADIO_BCAST_PANID_FILTER_MASK  << 4)
#define RADIO_INDEX0_ADDR_FILTER_MASK        (RADIO_INDEX0_PANID_FILTER_MASK << 4)
#define RADIO_INDEX1_ADDR_FILTER_MASK        (RADIO_INDEX1_PANID_FILTER_MASK << 4)
#define RADIO_INDEX2_ADDR_FILTER_MASK        (RADIO_INDEX2_PANID_FILTER_MASK << 4)

#define RADIO_BCAST_PANID                    (0xFFFF)
#define INVALID_VALUE                        (0xFF)

#if defined(_SILICON_LABS_32B_SERIES_1)
#define DEVICE_CAPABILITY_MCU_EN        (1)
#else
#define DEVICE_CAPABILITY_MCU_EN        (DEVINFO->SWCAPA1 & _DEVINFO_SWCAPA1_RFMCUEN_MASK)
#endif


// Energy Scan
typedef enum
{
    ENERGY_SCAN_STATUS_IDLE,
    ENERGY_SCAN_STATUS_IN_PROGRESS,
    ENERGY_SCAN_STATUS_COMPLETED
} energyScanStatus;

typedef enum
{
    ENERGY_SCAN_MODE_SYNC,
    ENERGY_SCAN_MODE_ASYNC
} energyScanMode;

static volatile energyScanStatus sEnergyScanStatus;
static volatile int8_t           sEnergyScanResultDbm;
static energyScanMode            sEnergyScanMode;

static bool              sIsSrcMatchEnabled = false;

// Receive
static uint8_t           sReceivePsdu[IEEE802154_MAX_LENGTH];
static uint8_t           sReceiveAckPsdu[IEEE802154_MAX_LENGTH];
static otRadioFrame      sReceiveFrame;
static otRadioFrame      sReceiveAckFrame;
static otError           sReceiveError;

// Transmit
static otRadioFrame      sTransmitFrame;
static uint8_t           sTransmitPsdu[IEEE802154_MAX_LENGTH];
static volatile otError  sTransmitError;
static volatile bool     sTransmitBusy = false;
static otRadioFrame      *sTxFrame = NULL;

// Radio
#define CCA_THRESHOLD_UNINIT 127
#define CCA_THRESHOLD_DEFAULT  -75 // dBm  - default for 2.4GHz 802.15.4

static bool              sPromiscuous  = false;
static otRadioState      sState        = OT_RADIO_STATE_DISABLED;

static efr32CommonConfig sCommonConfig;
static efr32BandConfig   sBandConfig;
static efr32BandConfig   *sCurrentBandConfig = NULL;

static int8_t            sCcaThresholdDbm = CCA_THRESHOLD_DEFAULT;

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
static efr32RadioCounters sRailDebugCounters;
#endif

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
extern uint8_t otNcpPlatGetCurCommandIid(void);
static uint8_t sRailFilterMask = RADIO_BCAST_PANID_FILTER_MASK;
#else
#define otNcpPlatGetCurCommandIid() 0
#endif

// RAIL

#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
RAIL_Handle_t gRailHandle;
#else
RAIL_Handle_t emPhyRailHandle;
#endif //SL_CATALOG_RAIL_MULTIPLEXER_PRESENT

static const RAIL_IEEE802154_Config_t sRailIeee802154Config = {
    NULL, // addresses
    {
        // ackConfig
        true, // ackConfig.enable
        672,  // ackConfig.ackTimeout
        {
            // ackConfig.rxTransitions
            RAIL_RF_STATE_RX, // ackConfig.rxTransitions.success
            RAIL_RF_STATE_RX, // ackConfig.rxTransitions.error
        },
        {
            // ackConfig.txTransitions
            RAIL_RF_STATE_RX, // ackConfig.txTransitions.success
            RAIL_RF_STATE_RX, // ackConfig.txTransitions.error
        },
    },
    {
        // timings
        100,      // timings.idleToRx
        192 - 10, // timings.txToRx
        100,      // timings.idleToTx
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        256,      // timings.rxToTx - accommodate enhanced ACKs
#else
        192,      // timings.rxToTx
#endif
        0,        // timings.rxSearchTimeout
        0,        // timings.txToRxSearchTimeout
        0,        // timings.txToTx
    },
    RAIL_IEEE802154_ACCEPT_STANDARD_FRAMES, // framesMask
    false,                                  // promiscuousMode
    false,                                  // isPanCoordinator
    false,                                  // defaultFramePendingInOutgoingAcks
};

#if     RADIO_CONFIG_SUBGHZ_SUPPORT
#define PHY_HEADER_SIZE 2
#else
#define PHY_HEADER_SIZE 1
#endif

// Misc
static volatile uint16_t    miscInternalFlags = 0;
static bool                 emPendingData = false;

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
enum {
    RHO_INACTIVE = 0,
    RHO_EXT_ACTIVE,
    RHO_INT_ACTIVE, // Not used
    RHO_BOTH_ACTIVE,
};

static uint8_t      rhoActive = RHO_INACTIVE;
static bool         ptaGntEventReported;
static bool         sRadioCoexEnabled = true;

#if SL_OPENTHREAD_COEX_COUNTER_ENABLE
static uint32_t sCoexCounters[SL_RAIL_UTIL_COEX_EVENT_COUNT] = {0};
#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE

#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
#define RADIO_INTERFACE_COUNT   4
#define RADIO_EXT_ADDR_COUNT    (RADIO_INTERFACE_COUNT - 1)
#else
#define RADIO_INTERFACE_COUNT   1
#define RADIO_EXT_ADDR_COUNT    (RADIO_INTERFACE_COUNT)
#endif

static otExtAddress  sExtAddress[RADIO_EXT_ADDR_COUNT];

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
#define UNINITIALIZED_POWER -128
#define INITIAL_POWER 0
static int8_t sTxPower[RADIO_INTERFACE_COUNT];
static int8_t maxMultipanTxPower(void)
{
    int8_t powerDbm = UNINITIALIZED_POWER;
    for (uint8_t i = 1; i < RADIO_INTERFACE_COUNT; i++) {
        if (sTxPower[i] > powerDbm) {
            powerDbm = sTxPower[i];
        }
    }
    return (powerDbm == UNINITIALIZED_POWER) ? INITIAL_POWER : powerDbm;
}
#endif

#if RADIO_CONFIG_ENABLE_CUSTOM_EUI_SUPPORT
/*
 * This API reads the UserData page on the given EFR device.
 */
static int readUserData(void *buffer, uint16_t index, int len, bool changeByteOrder)
{
    uint8_t *readLocation   = (uint8_t *)USERDATA_BASE + index;
    uint8_t *writeLocation  = (uint8_t *)buffer;

    // Sanity check to verify if the ouput buffer is valid and the index and len are valid.
    // If invalid, change the len to -1 and return.
    otEXPECT_ACTION((writeLocation != NULL) && ((readLocation + len) <= (uint8_t *)USERDATA_END), len = -1);

    // Copy the contents of flash into output buffer.
    
    for (int idx = 0; idx < len; idx++)
    {
        if (changeByteOrder) {
            writeLocation[idx] = readLocation[(len - 1) - idx];
        }
        else {
            writeLocation[idx] = readLocation[idx];
        }
    }

exit:
    // Return len, len was changed to -1 to indicate failure.
    return len;
}
#endif

/*
 * This API converts the FilterMask to appropriate IID. If there are any errors, it will fallback on bcast IID.
 *
 */
static inline uint8_t getIidFromFilterMask(uint8_t mask)
{
    uint8_t iid = INVALID_VALUE;

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    //We need only the Pan Id masks here, as we are not matching the addresses.
    //Also mask all the unused indices.
    mask &= sRailFilterMask;

    //The only acceptable values for mask at this point are:
    //1 - BCAST PANID   - IID(0)
    //2 - INDEX 0       - IID(1)
    //4 - INDEX 1       - IID(2)
    //8 - INDEX 2       - IID(3)
    //
    //The packet should either be directed to one of the PANs or Bcast.
    //(mask & (mask -1) is a simplistic way of testing if the mask is a power of 2.
    otEXPECT_ACTION(((mask != 0) && (mask & (mask -1)) == 0), iid = 0);

    while(mask)
    {
        iid++;
        mask >>= 1;
    }

exit:
#else
    (void)mask;
    iid = RADIO_BCAST_IID;
#endif
    return iid;
}

/*
 * This API validates the received FilterMask by checking if the destination address
 * in the received packet corresponds to destination PanID.
 */
static bool isFilterMaskValid(uint8_t mask)
{
    bool    valid = false;

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1

    /* Packet will be considered as a valid packet in 3 cases:
     * Case 1: If the packet was directed towards bcast address or bcase panid
     *
     * Case 2: If the packet was directed to right address corresponding to the panid
     *         (If Address filter and pan filter in the received packet match).
     *
     * Case 3: We dont have either the destination addressing feild or destination PanId
     *         in the received packet to determine if the dest address and dest pan match.
     */
    if( ((mask & RADIO_BCAST_PANID_FILTER_MASK) || (mask & RADIO_BCAST_ADDR_FILTER_MASK)) ||    //Case 1
        ((mask & 0x0F) == (mask >> 4)) ||                                                       //Case 2
        (((mask & 0x0F) == 0) || ((mask >> 4) == 0)) )                                          //Case 3
    {
        valid = true;
    }
#else
    (void)mask;
    valid = true;
#endif

    return valid;
}

/*
 * RAIL accepts 3 pan indices 0, 1 or 2. But valid IIDs are 1, 2 and 3 (0 is reserved for bcast).
 * This API validates the passed IID and converts it into usable PanIndex.
 */

static inline uint8_t getPanIndexFromIid(uint8_t iid)
{
    uint8_t panIndex = 0;

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    otEXPECT_ACTION(((iid < RADIO_INTERFACE_COUNT) && (iid != 0)), panIndex = INVALID_VALUE);
    panIndex = iid - 1;
exit:
#else
    panIndex = iid;
#endif
    return panIndex;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

enum {
  MAC_KEY_PREV,
  MAC_KEY_CURRENT,
  MAC_KEY_NEXT,
  MAC_KEY_COUNT
};

typedef struct securityMaterial
{
    uint8_t          ackKeyId;
    uint8_t          keyId;
    uint32_t         macFrameCounter;
    uint32_t         ackFrameCounter;
    otMacKeyMaterial keys[MAC_KEY_COUNT];
} securityMaterial;

// Transmit Security
static securityMaterial sMacKeys[RADIO_INTERFACE_COUNT];

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

#define CSL_TX_UNCERTAINTY 20 // Uncertainty of scheduling a CSL transmission, in ±10 us units.

// CSL parameters
static uint32_t      sCslPeriod;
static uint32_t      sCslSampleTime;

static uint16_t getCslPhase()
{
    uint32_t curTime       = otPlatAlarmMicroGetNow();
    uint32_t cslPeriodInUs = sCslPeriod * OT_US_PER_TEN_SYMBOLS;
    uint32_t diff = ((sCslSampleTime % cslPeriodInUs) - (curTime % cslPeriodInUs) + cslPeriodInUs) % cslPeriodInUs;

    return (uint16_t)(diff / OT_US_PER_TEN_SYMBOLS);
}
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

// Enhanced ACK IE data
static uint8_t       sAckIeData[OT_ACK_IE_MAX_SIZE];
static uint8_t       sAckIeDataLength = 0;

static uint8_t generateAckIeData(uint8_t *aLinkMetricsIeData, uint8_t aLinkMetricsIeDataLen)
{
    OT_UNUSED_VARIABLE(aLinkMetricsIeData);
    OT_UNUSED_VARIABLE(aLinkMetricsIeDataLen);

    uint8_t offset = 0;

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (sCslPeriod > 0)
    {
        offset += otMacFrameGenerateCslIeTemplate(sAckIeData);
    }
#endif

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    if (aLinkMetricsIeData != NULL && aLinkMetricsIeDataLen > 0)
    {
        offset += otMacFrameGenerateEnhAckProbingIe(sAckIeData, aLinkMetricsIeData, aLinkMetricsIeDataLen);
    }
#endif

    return offset;
}

static otError radioProcessTransmitSecurity(otRadioFrame *aFrame, uint8_t iid)
{
    otError error = OT_ERROR_NONE;
    uint8_t keyId;
    uint8_t keyToUse;
    uint8_t panIndex = getPanIndexFromIid(iid);

    otEXPECT(otMacFrameIsSecurityEnabled(aFrame) && otMacFrameIsKeyIdMode1(aFrame) &&
             !aFrame->mInfo.mTxInfo.mIsSecurityProcessed);

    assert(panIndex != INVALID_VALUE);

    if (otMacFrameIsAck(aFrame))
    {
        keyId = otMacFrameGetKeyId(aFrame);

        otEXPECT_ACTION(keyId != 0, error = OT_ERROR_FAILED);

        if (keyId == sMacKeys[iid].keyId - 1)
        {
            keyToUse = MAC_KEY_PREV;
        }
        else if (keyId == sMacKeys[iid].keyId)
        {
            keyToUse = MAC_KEY_CURRENT;
        }
        else if (keyId == sMacKeys[iid].keyId + 1)
        {
            keyToUse = MAC_KEY_NEXT;
        }
        else
        {
            error = OT_ERROR_SECURITY;
            otEXPECT(false);
        }
    }
    else
    {
        keyId = sMacKeys[iid].keyId;
        keyToUse = MAC_KEY_CURRENT;
    }

    aFrame->mInfo.mTxInfo.mAesKey = &sMacKeys[iid].keys[keyToUse];

    if (!aFrame->mInfo.mTxInfo.mIsHeaderUpdated)
    {
        if (otMacFrameIsAck(aFrame))
        {
            // Store ack frame counter and ack key ID for receive frame
            // TODO: What's a better solution?  (With flags like
            // frame pending or enh-secure, we have used reserved bits)
            sMacKeys[iid].ackKeyId        = keyId;
            sMacKeys[iid].ackFrameCounter = sMacKeys[iid].macFrameCounter;
        }

        otMacFrameSetKeyId(aFrame, keyId);
        otMacFrameSetFrameCounter(aFrame, sMacKeys[iid].macFrameCounter++);
    }

#if defined(_SILICON_LABS_32B_SERIES_2) && OPENTHREAD_RADIO
    efr32PlatProcessTransmitAesCcm(aFrame, &sExtAddress[panIndex]);
#else
    otMacFrameProcessTransmitAesCcm(aFrame, &sExtAddress[panIndex]);
#endif

exit:
    return error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

static uint8_t readInitialPacketData(RAIL_RxPacketInfo_t *packetInfo,
                                     uint8_t expected_data_bytes_max,
                                     uint8_t expected_data_bytes_min,
                                     uint8_t *buffer,
                                     uint8_t buffer_len )
{
    uint8_t packetBytesRead = 0;

    //Check if we have enough buffer
    assert((buffer_len >= expected_data_bytes_max) || (packetInfo != NULL));

    //Read the packet info
    RAIL_GetRxIncomingPacketInfo(gRailHandle, packetInfo);

    //We are trying to get the packet info of a packet before it is completely received.
    //We do this to evaluate the FP bit in response and add IEs to ACK if needed.
    //Check to see if we have received atleast minimum number of bytes requested.
    otEXPECT_ACTION(packetInfo->packetBytes >= expected_data_bytes_min, packetBytesRead = 0);

    // Only extract what we care about
    if (packetInfo->packetBytes > expected_data_bytes_max)
    {
        packetInfo->packetBytes = expected_data_bytes_max;
        //Check if the initial portion of the packet received so far exceeds the max value requested.
        if (packetInfo->firstPortionBytes >= expected_data_bytes_max)
        {
            //If we have received more, make sure to copy only the required bytes into the buffer.
            packetInfo->firstPortionBytes = expected_data_bytes_max;
            packetInfo->lastPortionData = NULL;
        }
    }

    //Copy number of bytes as indicated in `packetInfo->firstPortionBytes` into the buffer.
    RAIL_CopyRxPacket(buffer, packetInfo);
    packetBytesRead = packetInfo->packetBytes;

exit:
    return packetBytesRead;
}

//------------------------------------------------------------------------------
// Forward Declarations

static void RAILCb_Generic(RAIL_Handle_t aRailHandle, RAIL_Events_t aEvents);

static void efr32PhyStackInit(void);

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
static void updateIeInfoTxFrame(void);
#endif

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
static void efr32CoexInit(void);
// Try to transmit the current outgoing frame subject to MAC-level PTA
static void tryTxCurrentPacket(void);
#else
// Transmit the current outgoing frame.
void txCurrentPacket(void);
#define tryTxCurrentPacket txCurrentPacket
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

static void txFailedCallback(bool isAck, uint8_t status);

static bool validatePacketDetails(RAIL_RxPacketHandle_t packetHandle,
                                  RAIL_RxPacketDetails_t *pPacketDetails,
                                  RAIL_RxPacketInfo_t *pPacketInfo,
                                  uint16_t *packetLength);
static bool validatePacketTimestamp(RAIL_RxPacketDetails_t *pPacketDetails,
                                    uint16_t packetLength);
static void updateRxFrameDetails(RAIL_RxPacketDetails_t *pPacketDetails,
                                 bool securedOutgoingEnhancedAck,
                                 bool framePendingSetInOutgoingAck,
                                 uint8_t iid);

static otError skipRxPacketLengthBytes(RAIL_RxPacketInfo_t *pPacketInfo);

//------------------------------------------------------------------------------
// Helper Functions

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

static bool phyStackEventIsEnabled(void)
{
    bool result = false;

#if (defined(SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT) \
     && SL_RAIL_UTIL_ANT_DIV_RX_RUNTIME_PHY_SELECT)
    result = true;
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (sRadioCoexEnabled) {
        result |= sl_rail_util_coex_is_enabled();
#ifdef SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT
        result |= SL_RAIL_UTIL_COEX_RUNTIME_PHY_SELECT;
#endif
    }
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    return result;
}

static RAIL_Events_t currentEventConfig = RAIL_EVENTS_NONE;
static void updateEvents(RAIL_Events_t mask, RAIL_Events_t values)
{
    RAIL_Status_t status;
    RAIL_Events_t newEventConfig = (currentEventConfig & ~mask) | (values & mask);
    if (newEventConfig != currentEventConfig) {
        currentEventConfig = newEventConfig;
        status = RAIL_ConfigEvents(gRailHandle, mask, values);
        assert(status == RAIL_STATUS_NO_ERROR);
  }
}

static sl_rail_util_ieee802154_stack_event_t
    handlePhyStackEvent(sl_rail_util_ieee802154_stack_event_t stackEvent,
                        uint32_t supplement)
{
    return (phyStackEventIsEnabled()
#ifdef SL_CATALOG_RAIL_MULTIPLEXER_PRESENT
          ? sl_rail_mux_ieee802154_on_event(gRailHandle, stackEvent, supplement)
#else
          ? sl_rail_util_ieee802154_on_event(stackEvent, supplement)
#endif       
            : 0);
}
#else
static void updateEvents(RAIL_Events_t mask, RAIL_Events_t values)
{
    RAIL_Status_t status;
    status = RAIL_ConfigEvents(gRailHandle, mask, values);
    assert(status == RAIL_STATUS_NO_ERROR);
}

#define handlePhyStackEvent(event, supplement) 0
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

// Set or clear the passed flag.
static inline void setInternalFlag(uint16_t flag, bool val)
{
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    miscInternalFlags = (val
                         ? (miscInternalFlags | flag)
                         : (miscInternalFlags & ~flag));
    CORE_EXIT_ATOMIC();
}
// Returns true if the passed flag is set, false otherwise.
static inline bool getInternalFlag(uint16_t flag)
{
  return ((miscInternalFlags & flag) != 0);
}

static inline bool txWaitingForAck(void)
{
    return (sTransmitBusy == true
            && ((sTransmitFrame.mPsdu[0] & IEEE802154_FRAME_FLAG_ACK_REQUIRED) != 0));
}

static bool txIsDataRequest(void)
{
    uint16_t fcf  = sTransmitFrame.mPsdu[IEEE802154_FCF_OFFSET]
                    | (sTransmitFrame.mPsdu[IEEE802154_FCF_OFFSET + 1] << 8);

    return(sTransmitBusy == true
           && (fcf & IEEE802154_FRAME_TYPE_MASK) == IEEE802154_FRAME_TYPE_COMMAND);
}

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
static inline bool isReceivingFrame(void)
{
    return (RAIL_GetRadioState(gRailHandle) & RAIL_RF_STATE_RX_ACTIVE)
            == RAIL_RF_STATE_RX_ACTIVE;
}
#endif

static void radioSetIdle(void)
{
    if (RAIL_GetRadioState(gRailHandle) != RAIL_RF_STATE_IDLE) {
        RAIL_Idle(gRailHandle, RAIL_IDLE, true);
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_IDLED, 0U);
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED, 0U);
    }
    RAIL_YieldRadio(gRailHandle);
    sState = OT_RADIO_STATE_SLEEP;
}

static otError radioSetRx(uint8_t aChannel)
{
    otError error = OT_ERROR_NONE;
    RAIL_Status_t status;

    RAIL_SchedulerInfo_t bgRxSchedulerInfo = {
        .priority = RADIO_SCHEDULER_BACKGROUND_RX_PRIORITY,
        // sliptime/transaction time is not used for bg rx
    };

    status = RAIL_StartRx(gRailHandle, aChannel, &bgRxSchedulerInfo);
    otEXPECT_ACTION(status == RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN, 0U);
    sState = OT_RADIO_STATE_RECEIVE;

    otLogInfoPlat("State=OT_RADIO_STATE_RECEIVE", NULL);
exit:
    return error;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
static otError radioScheduleRx(uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    otError error = OT_ERROR_NONE;
    RAIL_Status_t status;

    RAIL_SchedulerInfo_t bgRxSchedulerInfo = {
        .priority = RADIO_SCHEDULER_BACKGROUND_RX_PRIORITY,
        // sliptime/transaction time is not used for bg rx
    };

    // Configure scheduled receive as requested
    // Q: Verify these parameters and configuration
    //
    RAIL_ScheduleRxConfig_t rxCfg = { .start = aStart,
                                      .startMode = RAIL_TIME_ABSOLUTE,
                                      .end = aDuration,
                                      .endMode = RAIL_TIME_DELAY,
                                      .rxTransitionEndSchedule = 1, // This lets us idle after a scheduled-rx
                                      .hardWindowEnd = 0 };         // This lets us receive a packet near a window-end-event

    status = RAIL_ScheduleRx(gRailHandle, aChannel, &rxCfg, &bgRxSchedulerInfo);
    otEXPECT_ACTION(status == RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_LISTEN, 0U);
exit:
    return error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

//------------------------------------------------------------------------------
// Radio Initialization
static RAIL_Handle_t efr32RailInit(efr32CommonConfig *aCommonConfig)
{
    RAIL_Status_t status;
    RAIL_Handle_t handle;

#if !OPENTHREAD_RADIO
    assert(DEVICE_CAPABILITY_MCU_EN);
#endif    

    handle = RAIL_Init(&aCommonConfig->mRailConfig, NULL);
    assert(handle != NULL);

#if defined(SL_CATALOG_POWER_MANAGER_PRESENT)
    status = RAIL_InitPowerManager();
    assert(status == RAIL_STATUS_NO_ERROR);
#endif // SL_CATALOG_POWER_MANAGER_PRESENT

    status = RAIL_ConfigCal(handle, RAIL_CAL_ALL);
    assert(status == RAIL_STATUS_NO_ERROR);

    status = RAIL_SetPtiProtocol(handle, RAIL_PTI_PROTOCOL_THREAD);
    assert(status == RAIL_STATUS_NO_ERROR);

    status = RAIL_IEEE802154_Init(handle, &sRailIeee802154Config);
    assert(status == RAIL_STATUS_NO_ERROR);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    // Enhanced Frame Pending
    status = RAIL_IEEE802154_EnableEarlyFramePending(handle, true);
    assert(status == RAIL_STATUS_NO_ERROR);

    status = RAIL_IEEE802154_EnableDataFramePending(handle, true);
    assert(status == RAIL_STATUS_NO_ERROR);

    // Copies of MAC keys for encrypting at the radio layer
    memset(sMacKeys, 0, sizeof(sMacKeys));
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

    uint16_t actualLength = RAIL_SetTxFifo(handle, aCommonConfig->mRailTxFifo.fifo, 0, sizeof(aCommonConfig->mRailTxFifo.fifo));
    assert(actualLength == sizeof(aCommonConfig->mRailTxFifo.fifo));

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    memset(sTxPower, UNINITIALIZED_POWER, sizeof(sTxPower));
#endif

    return handle;
}

static void efr32RailConfigLoad(efr32BandConfig *aBandConfig)
{
    RAIL_Status_t status;
    RAIL_TxPowerConfig_t txPowerConfig = {SL_RAIL_UTIL_PA_SELECTION_2P4GHZ, SL_RAIL_UTIL_PA_VOLTAGE_MV, 10};

    if (aBandConfig->mChannelConfig != NULL)
    {
        status = RAIL_IEEE802154_SetPtiRadioConfig(gRailHandle, RAIL_IEEE802154_PTI_RADIO_CONFIG_915MHZ_R23_NA_EXT);
        assert(status == RAIL_STATUS_NO_ERROR);

        uint16_t firstChannel = RAIL_ConfigChannels(gRailHandle, aBandConfig->mChannelConfig, NULL);
        assert(firstChannel == aBandConfig->mChannelMin);

        txPowerConfig.mode = SL_RAIL_UTIL_PA_SELECTION_SUBGHZ;
        status = RAIL_IEEE802154_ConfigGOptions(gRailHandle,
                                                RAIL_IEEE802154_G_OPTION_GB868,
                                                RAIL_IEEE802154_G_OPTION_GB868);
        assert(status == RAIL_STATUS_NO_ERROR);

    }
    else
    {
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT
        status = sl_rail_util_ieee802154_config_radio(gRailHandle);
#else
        status = RAIL_IEEE802154_Config2p4GHzRadio(gRailHandle);
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT
        assert(status == RAIL_STATUS_NO_ERROR);
    }

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // 802.15.4E support (only on platforms that support it, so error checking is disabled)
    // Note: This has to be called after RAIL_IEEE802154_Config2p4GHzRadio due to a bug where this call
    // can overwrite options set below.
    RAIL_IEEE802154_ConfigEOptions(gRailHandle,
                                   (RAIL_IEEE802154_E_OPTION_GB868 | RAIL_IEEE802154_E_OPTION_ENH_ACK),
                                   (RAIL_IEEE802154_E_OPTION_GB868 | RAIL_IEEE802154_E_OPTION_ENH_ACK));
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

    status = RAIL_ConfigTxPower(gRailHandle, &txPowerConfig);
    assert(status == RAIL_STATUS_NO_ERROR);
}

static void efr32RadioSetTxPower(int8_t aPowerDbm)
{
    RAIL_Status_t status;
    sl_rail_util_pa_init();

    status = RAIL_SetTxPowerDbm(gRailHandle, ((RAIL_TxPower_t)aPowerDbm) * 10);
    assert(status == RAIL_STATUS_NO_ERROR);
}

static efr32BandConfig *efr32RadioGetBandConfig(uint8_t aChannel)
{
    efr32BandConfig *config = NULL;

    if ((sBandConfig.mChannelMin <= aChannel) && (aChannel <= sBandConfig.mChannelMax))
    {
        config = &sBandConfig;
    }

    return config;
}

static void efr32ConfigInit(void (*aEventCallback)(RAIL_Handle_t railHandle, RAIL_Events_t events))
{
    sCommonConfig.mRailConfig.eventsCallback = aEventCallback;
    sCommonConfig.mRailConfig.protocol       = NULL; // only used by Bluetooth stack
#if RADIO_CONFIG_DMP_SUPPORT
    sCommonConfig.mRailConfig.scheduler = &(sCommonConfig.mRailSchedState);
#else
    sCommonConfig.mRailConfig.scheduler = NULL; // only needed for DMP
#endif

#if RADIO_CONFIG_2P4GHZ_OQPSK_SUPPORT
    sBandConfig.mChannelConfig = NULL;
    sBandConfig.mChannelMin    = OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN;
    sBandConfig.mChannelMax    = OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX;

#elif RADIO_CONFIG_SUBGHZ_SUPPORT
    sBandConfig.mChannelConfig = channelConfigs[0];
    sBandConfig.mChannelMin    = OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN;
    sBandConfig.mChannelMax    = OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX;

#elif RADIO_CONFIG_915MHZ_OQPSK_SUPPORT // Not supported
    sBandConfig.mChannelConfig = channelConfigs[0];
    sBandConfig.mChannelMin    = OT_RADIO_915MHZ_OQPSK_CHANNEL_MIN;
    sBandConfig.mChannelMax    = OT_RADIO_915MHZ_OQPSK_CHANNEL_MAX;
#endif

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    memset(&sRailDebugCounters, 0x00, sizeof(efr32RadioCounters));
#endif

    gRailHandle = efr32RailInit(&sCommonConfig);
    assert(gRailHandle != NULL);

    updateEvents(RAIL_EVENTS_ALL,
                 (0
                  | RAIL_EVENT_RX_ACK_TIMEOUT
                  | RAIL_EVENT_RX_PACKET_RECEIVED
                  | RAIL_EVENT_SCHEDULED_RX_STARTED
                  | RAIL_EVENT_RX_SCHEDULED_RX_END
                  | RAIL_EVENTS_TXACK_COMPLETION
                  | RAIL_EVENTS_TX_COMPLETION
                  | RAIL_EVENT_RSSI_AVERAGE_DONE
                  | RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT || RADIO_CONFIG_DMP_SUPPORT
                  | RAIL_EVENT_CONFIG_SCHEDULED
                  | RAIL_EVENT_CONFIG_UNSCHEDULED
                  | RAIL_EVENT_SCHEDULER_STATUS
#endif
                  | RAIL_EVENT_CAL_NEEDED));

    efr32RailConfigLoad(&(sBandConfig));
}

void efr32RadioInit(void)
{
    if (getInternalFlag(FLAG_RADIO_INIT_DONE)) {
        return;
    }
    RAIL_Status_t status;

    // check if RAIL_TX_FIFO_SIZE is power of two..
    assert((RAIL_TX_FIFO_SIZE & (RAIL_TX_FIFO_SIZE - 1)) == 0);

    // check the limits of the RAIL_TX_FIFO_SIZE.
    assert((RAIL_TX_FIFO_SIZE >= 64) || (RAIL_TX_FIFO_SIZE <= 4096));

    efr32ConfigInit(RAILCb_Generic);
    setInternalFlag(FLAG_RADIO_INIT_DONE, true);

    status = RAIL_ConfigSleep(gRailHandle, RAIL_SLEEP_CONFIG_TIMERSYNC_ENABLED);
    assert(status == RAIL_STATUS_NO_ERROR);

    sReceiveFrame.mLength       = 0;
    sReceiveFrame.mPsdu         = sReceivePsdu;
    sReceiveAckFrame.mLength    = 0;
    sReceiveAckFrame.mPsdu      = sReceiveAckPsdu;
    sTransmitFrame.mLength      = 0;
    sTransmitFrame.mPsdu        = sTransmitPsdu;

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    otLinkMetricsInit(EFR32_RECEIVE_SENSITIVITY);
#endif
    sCurrentBandConfig = efr32RadioGetBandConfig(OPENTHREAD_CONFIG_DEFAULT_CHANNEL);
    assert(sCurrentBandConfig != NULL);

    efr32RadioSetTxPower(OPENTHREAD_CONFIG_DEFAULT_TRANSMIT_POWER);

    assert(RAIL_ConfigRxOptions(gRailHandle,
                                RAIL_RX_OPTION_TRACK_ABORTED_FRAMES,
                                RAIL_RX_OPTION_TRACK_ABORTED_FRAMES)
           == RAIL_STATUS_NO_ERROR);
    efr32PhyStackInit();
    efr32RadioSetCcaMode(SL_OPENTHREAD_RADIO_CCA_MODE);

    sEnergyScanStatus = ENERGY_SCAN_STATUS_IDLE;
    sTransmitError    = OT_ERROR_NONE;
    sTransmitBusy     = false;

    otLogInfoPlat("Initialized", NULL);
}

void efr32RadioDeinit(void)
{
    RAIL_Status_t status;

    RAIL_Idle(gRailHandle, RAIL_IDLE_ABORT, true);
    status = RAIL_ConfigEvents(gRailHandle, RAIL_EVENTS_ALL, 0);
    assert(status == RAIL_STATUS_NO_ERROR);

    sCurrentBandConfig = NULL;
}

//------------------------------------------------------------------------------
// Energy Scan support

static void energyScanComplete(int8_t scanResultDbm)
{
    sEnergyScanResultDbm =  scanResultDbm;
    sEnergyScanStatus = ENERGY_SCAN_STATUS_COMPLETED;
}

static otError efr32StartEnergyScan(energyScanMode aMode, uint16_t aChannel, RAIL_Time_t aAveragingTimeUs)
{
    RAIL_Status_t    status = RAIL_STATUS_NO_ERROR;
    otError          error  = OT_ERROR_NONE;
    efr32BandConfig *config = NULL;

    otEXPECT_ACTION(sEnergyScanStatus == ENERGY_SCAN_STATUS_IDLE, error = OT_ERROR_BUSY);

    sEnergyScanStatus = ENERGY_SCAN_STATUS_IN_PROGRESS;
    sEnergyScanMode   = aMode;

    RAIL_Idle(gRailHandle, RAIL_IDLE, true);

    config = efr32RadioGetBandConfig(aChannel);
    otEXPECT_ACTION(config != NULL, error = OT_ERROR_INVALID_ARGS);

    if (sCurrentBandConfig != config)
    {
        efr32RailConfigLoad(config);
        sCurrentBandConfig = config;
    }

    RAIL_SchedulerInfo_t scanSchedulerInfo = {.priority        = RADIO_SCHEDULER_CHANNEL_SCAN_PRIORITY,
                                              .slipTime        = RADIO_SCHEDULER_CHANNEL_SLIP_TIME,
                                              .transactionTime = aAveragingTimeUs};

    status = RAIL_StartAverageRssi(gRailHandle, aChannel, aAveragingTimeUs, &scanSchedulerInfo);
    otEXPECT_ACTION(status == RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

exit:
    if (status != RAIL_STATUS_NO_ERROR) {
        energyScanComplete(OT_RADIO_RSSI_INVALID);
    }
    return error;
}

//------------------------------------------------------------------------------
// Stack support

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
  OT_UNUSED_VARIABLE(aInstance);

  return otPlatTimeGet();
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    OT_UNUSED_VARIABLE(aInstance);

#if RADIO_CONFIG_ENABLE_CUSTOM_EUI_SUPPORT
    // Invalid EUI
    uint8_t nullEui[] = { 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU };

    // Read the Custom EUI and compare it to nullEui
    if ((readUserData(aIeeeEui64, USERDATA_MFG_CUSTOM_EUI_64, OT_EXT_ADDRESS_SIZE, true) == -1) || 
        (memcmp(aIeeeEui64, nullEui, OT_EXT_ADDRESS_SIZE) == 0))
#endif
    {
        uint64_t eui64;
        uint8_t *eui64Ptr = NULL;

        eui64    = SYSTEM_GetUnique();
        eui64Ptr = (uint8_t *)&eui64;

        for (uint8_t i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
        {
            aIeeeEui64[i] = eui64Ptr[(OT_EXT_ADDRESS_SIZE - 1) - i];
        }
    }
}

void otPlatRadioSetPanId(otInstance *aInstance, uint16_t aPanId)
{
    OT_UNUSED_VARIABLE(aInstance);

    RAIL_Status_t status;
    uint8_t iid = otNcpPlatGetCurCommandIid();
    uint8_t panIndex = getPanIndexFromIid(iid);

    assert(panIndex != INVALID_VALUE);
    otLogInfoPlat("PANID=%X index=%u IID=%d", aPanId, panIndex, iid);
    utilsSoftSrcMatchSetPanId(iid, aPanId);

    status = RAIL_IEEE802154_SetPanId(gRailHandle, aPanId, panIndex);
    assert(status == RAIL_STATUS_NO_ERROR);

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    //We already have IID 0 enabled in filtermask to track BCAST Packets, so
    //track only unique PanIds.
    if(aPanId != RADIO_BCAST_PANID)
    {
        sRailFilterMask |= RADIO_GET_FILTER_MASK(iid);
    }
#endif

}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *aAddress)
{
    RAIL_Status_t status;
    uint8_t panIndex = getPanIndexFromIid(otNcpPlatGetCurCommandIid());

    OT_UNUSED_VARIABLE(aInstance);
    assert(panIndex != INVALID_VALUE);

    for (size_t i = 0; i < sizeof(*aAddress); i++)
    {
        sExtAddress[panIndex].m8[i] = aAddress->m8[sizeof(*aAddress) - 1 - i];
    }

    otLogInfoPlat("ExtAddr=%X%X%X%X%X%X%X%X index=%u", aAddress->m8[7], aAddress->m8[6], aAddress->m8[5], aAddress->m8[4],
                  aAddress->m8[3], aAddress->m8[2], aAddress->m8[1], aAddress->m8[0], panIndex);

    status = RAIL_IEEE802154_SetLongAddress(gRailHandle, (uint8_t *)aAddress->m8, panIndex);
    assert(status == RAIL_STATUS_NO_ERROR);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, uint16_t aAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    RAIL_Status_t status;
    uint8_t panIndex = getPanIndexFromIid(otNcpPlatGetCurCommandIid());

    assert(panIndex != INVALID_VALUE);
    otLogInfoPlat("ShortAddr=%X index=%u", aAddress, panIndex);

    status = RAIL_IEEE802154_SetShortAddress(gRailHandle, aAddress, panIndex);
    assert(status == RAIL_STATUS_NO_ERROR);
}

otRadioState otPlatRadioGetState(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sState;
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (sState != OT_RADIO_STATE_DISABLED);
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    otEXPECT(!otPlatRadioIsEnabled(aInstance));

    otLogInfoPlat("State=OT_RADIO_STATE_SLEEP", NULL);
    sState = OT_RADIO_STATE_SLEEP;

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    uint8_t iid = otNcpPlatGetCurCommandIid();
    if (sTxPower[iid] == UNINITIALIZED_POWER) {
        sTxPower[iid] = INITIAL_POWER;
    }
#endif

exit:
    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    otEXPECT(otPlatRadioIsEnabled(aInstance));

    otLogInfoPlat("State=OT_RADIO_STATE_DISABLED", NULL);
    sState = OT_RADIO_STATE_DISABLED;

exit:
    return OT_ERROR_NONE;
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION((sState != OT_RADIO_STATE_TRANSMIT) && (sState != OT_RADIO_STATE_DISABLED),
                    error = OT_ERROR_INVALID_STATE);

    otLogInfoPlat("State=OT_RADIO_STATE_SLEEP", NULL);
    radioSetIdle();
exit:
    return error;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    otError          error = OT_ERROR_NONE;
    RAIL_Status_t    status;
    efr32BandConfig *config;

    OT_UNUSED_VARIABLE(aInstance);
    otEXPECT_ACTION(sState != OT_RADIO_STATE_DISABLED, error = OT_ERROR_INVALID_STATE);

    config = efr32RadioGetBandConfig(aChannel);
    otEXPECT_ACTION(config != NULL, error = OT_ERROR_INVALID_ARGS);

    if (sCurrentBandConfig != config)
    {
        RAIL_Idle(gRailHandle, RAIL_IDLE, true);
        efr32RailConfigLoad(config);
        sCurrentBandConfig = config;
    }

    status = radioSetRx(aChannel);
    otEXPECT_ACTION(status == RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    sReceiveFrame.mChannel = aChannel;
    sReceiveAckFrame.mChannel = aChannel;

exit:
    return error;
}

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    otError          error = OT_ERROR_NONE;
    RAIL_Status_t    status;
    efr32BandConfig *config;

    OT_UNUSED_VARIABLE(aInstance);
    otEXPECT_ACTION(sState != OT_RADIO_STATE_DISABLED, error = OT_ERROR_INVALID_STATE);

    config = efr32RadioGetBandConfig(aChannel);
    otEXPECT_ACTION(config != NULL, error = OT_ERROR_INVALID_ARGS);

    if (sCurrentBandConfig != config)
    {
        RAIL_Idle(gRailHandle, RAIL_IDLE, true);
        efr32RailConfigLoad(config);
        sCurrentBandConfig = config;
    }

    status = radioScheduleRx(aChannel, aStart, aDuration);
    otEXPECT_ACTION(status == RAIL_STATUS_NO_ERROR, error = OT_ERROR_FAILED);

    sReceiveFrame.mChannel = aChannel;
    sReceiveAckFrame.mChannel = aChannel;

exit:
    return error;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    otError error = OT_ERROR_NONE;
    efr32BandConfig * config;

    otEXPECT_ACTION((sState != OT_RADIO_STATE_DISABLED) && (sState != OT_RADIO_STATE_TRANSMIT),
                    error = OT_ERROR_INVALID_STATE);

    config = efr32RadioGetBandConfig(aFrame->mChannel);
    otEXPECT_ACTION(config != NULL, error = OT_ERROR_INVALID_ARGS);
    if (sCurrentBandConfig != config)
    {
        RAIL_Idle(gRailHandle, RAIL_IDLE, true);
        efr32RailConfigLoad(config);
        sCurrentBandConfig = config;
    }

    assert(sTransmitBusy == false);
    sState         = OT_RADIO_STATE_TRANSMIT;
    sTransmitError = OT_ERROR_NONE;
    sTransmitBusy  = true;
    sTxFrame       = aFrame;

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    uint8_t iid = 0;
#endif

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    iid            = aFrame->mIid;
#endif

    setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, aFrame->mInfo.mTxInfo.mCsmaCaEnabled);

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
    updateIeInfoTxFrame();
    // Note - we need to call this outside of txCurrentPacket as for Series 2,
    // this results in calling the SE interface from a critical section which is not permitted.
    otEXPECT_ACTION(radioProcessTransmitSecurity(sTxFrame, iid) == OT_ERROR_NONE,
                    error = OT_ERROR_INVALID_STATE);
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    setInternalFlag(FLAG_ONGOING_TX_DATA, true);
    tryTxCurrentPacket();
    CORE_EXIT_ATOMIC();

    if (sTransmitError == OT_ERROR_NONE) {
        otPlatRadioTxStarted(aInstance, aFrame);
    }
exit:
    return error;
}

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
void updateIeInfoTxFrame(void)
{
    assert(sTxFrame != NULL);

#if OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT && OPENTHREAD_CONFIG_TIME_SYNC_ENABLE
    // Seek the time sync offset and update the rendezvous time
    if (sTxFrame->mInfo.mTxInfo.mIeInfo->mTimeIeOffset != 0)
    {
        uint8_t *timeIe = sTxFrame->mPsdu + sTxFrame->mInfo.mTxInfo.mIeInfo->mTimeIeOffset;
        uint64_t time   = RAIL_GetTime() + sTxFrame->mInfo.mTxInfo.mIeInfo->mNetworkTimeOffset;

        *timeIe = sTxFrame->mInfo.mTxInfo.mIeInfo->mTimeSyncSeq;

        *(++timeIe) = (uint8_t)(time & 0xff);
        for (uint8_t i = 1; i < sizeof(uint64_t); i++)
        {
            time        = time >> 8;
            *(++timeIe) = (uint8_t)(time & 0xff);
        }
    }
#endif // OPENTHREAD_CONFIG_MAC_HEADER_IE_SUPPORT && OPENTHREAD_CONFIG_TIME_SYNC_ENABLE

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    // Update IE data in the 802.15.4 header with the newest CSL period / phase
    if (sCslPeriod > 0 && !sTxFrame->mInfo.mTxInfo.mIsHeaderUpdated)
    {
        otMacFrameSetCslIe(sTxFrame, (uint16_t)sCslPeriod, getCslPhase());
    }
#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
}
#endif // OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2

void txCurrentPacket(void)
{
    assert(getInternalFlag(FLAG_ONGOING_TX_DATA));
    assert(sTxFrame != NULL);

    RAIL_CsmaConfig_t csmaConfig = RAIL_CSMA_CONFIG_802_15_4_2003_2p4_GHz_OQPSK_CSMA;
    RAIL_TxOptions_t  txOptions  = RAIL_TX_OPTIONS_DEFAULT;
    RAIL_Status_t     status = RAIL_STATUS_INVALID_STATE;
    uint8_t           frameLength;
    bool              ackRequested;

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    sRailDebugCounters.mRailPlatTxTriggered++;
#endif
    // signalling this event earlier, as this event can assert REQ (expecially for a
    // non-CSMA transmit) giving the Coex master a little more time to grant or deny.
    if (getInternalFlag(FLAG_CURRENT_TX_USE_CSMA)) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY, (uint32_t) true);
    }
    else {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_PHY, (uint32_t) false);
    }

    frameLength = (uint8_t)sTxFrame->mLength;

    if (PHY_HEADER_SIZE == 1) {
        RAIL_WriteTxFifo(gRailHandle, &frameLength, sizeof frameLength, true);
    }
    else { // 2 byte PHR for Sub-GHz
        uint8_t PHRByte1 =  (0x08U /*FCS=2byte*/ | 0x10U /*Whiten=enabled*/);
        uint8_t PHRByte2 = (uint8_t)(__RBIT(frameLength) >> 24);

        RAIL_WriteTxFifo(gRailHandle, &PHRByte1, sizeof PHRByte1, true);
        RAIL_WriteTxFifo(gRailHandle, &PHRByte2, sizeof PHRByte2, false);
    }
    RAIL_WriteTxFifo(gRailHandle, sTxFrame->mPsdu, frameLength - 2, false);

    RAIL_SchedulerInfo_t txSchedulerInfo = {
        .priority        = RADIO_SCHEDULER_TX_PRIORITY,
        .slipTime        = RADIO_SCHEDULER_CHANNEL_SLIP_TIME,
        .transactionTime = 0, // will be calculated later if DMP is used
    };

    ackRequested = (sTxFrame->mPsdu[0] & IEEE802154_FRAME_FLAG_ACK_REQUIRED);
    if (ackRequested)
    {
        txOptions |= RAIL_TX_OPTION_WAIT_FOR_ACK;

#if RADIO_CONFIG_DMP_SUPPORT
        // time we wait for ACK
        if (RAIL_GetSymbolRate(gRailHandle) > 0)
        {
            txSchedulerInfo.transactionTime += 12 * 1e6 / RAIL_GetSymbolRate(gRailHandle);
        }
        else
        {
            txSchedulerInfo.transactionTime += 12 * RADIO_TIMING_DEFAULT_SYMBOLTIME_US;
        }
#endif
    }

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    // Update Tx options to use currently-selected antenna.
    // If antenna diverisity on Tx is disabled, leave both options 0
    // so Tx antenna tracks Rx antenna.
    if (sl_rail_util_ant_div_get_tx_antenna_mode()
        !=  SL_RAIL_UTIL_ANT_DIV_DISABLED) {
        txOptions |= ((sl_rail_util_ant_div_get_tx_antenna_selected() == SL_RAIL_UTIL_ANTENNA_SELECT_ANTENNA1)
                      ? RAIL_TX_OPTION_ANTENNA0
                      : RAIL_TX_OPTION_ANTENNA1);
    }
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#if RADIO_CONFIG_DMP_SUPPORT
    // time needed for the frame itself
    // 4B preamble, 1B SFD, 1B PHR is not counted in frameLength
    if (RAIL_GetBitRate(gRailHandle) > 0)
    {
        txSchedulerInfo.transactionTime += (frameLength + 4 + 1 + 1) * 8 * 1e6 / RAIL_GetBitRate(gRailHandle);
    }
    else
    { // assume 250kbps
        txSchedulerInfo.transactionTime += (frameLength + 4 + 1 + 1) * RADIO_TIMING_DEFAULT_BYTETIME_US;
    }
#endif

    if (getInternalFlag(FLAG_CURRENT_TX_USE_CSMA))
    {
#if RADIO_CONFIG_DMP_SUPPORT
        // time needed for CSMA/CA
        txSchedulerInfo.transactionTime += RADIO_TIMING_CSMA_OVERHEAD_US;
#endif
        csmaConfig.csmaTries    = sTxFrame->mInfo.mTxInfo.mMaxCsmaBackoffs;
        csmaConfig.ccaThreshold = sCcaThresholdDbm;

// Note: We don't support scheduled-tx, but here's placeholder code
// if we ever implement it.
/*
#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        // Delayed tx in radio, such as for CSL
        if (sTxFrame->mInfo.mTxInfo.mTxDelay != 0)
        {
            int32_t remainingUs = (sTxFrame->mInfo.mTxInfo.mTxDelayBaseTime
                                   + sTxFrame->mInfo.mTxInfo.mTxDelay
                                   - RAIL_GetTime());
            if (remainingUs <= 0)
            {
                otSysEventSignalPending();
            }
            else
            {
                RAIL_ScheduleTxConfig_t scheduleTxOptions = {
                    .when = remainingUs,
                    .mode = RAIL_TIME_DELAY,
                    .txDuringRx = RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX
                };
                status = RAIL_StartScheduledCcaCsmaTx(gRailHandle,
                                                      sTxFrame->mChannel,
                                                      txOptions,
                                                      &scheduleTxOptions,
                                                      &csmaConfig,
                                                      &txSchedulerInfo);
            }
        }
        else
#endif
*/
        {
            status = RAIL_StartCcaCsmaTx(gRailHandle,
                                         sTxFrame->mChannel,
                                         txOptions,
                                         &csmaConfig,
                                         &txSchedulerInfo);
        }
    }
    else
    {
        status = RAIL_StartTx(gRailHandle, sTxFrame->mChannel, txOptions, &txSchedulerInfo);
        if (status == RAIL_STATUS_NO_ERROR) {
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED, 0U);
        }
    }

    if (status == RAIL_STATUS_NO_ERROR)
    {
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailTxStarted++;
#endif
    }
    else
    {
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailTxStartFailed++;
#endif
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED, (uint32_t) ackRequested);
        txFailedCallback(false, TX_COMPLETE_RESULT_OTHER_FAIL);

        otSysEventSignalPending();
    }
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return &sTransmitFrame;
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    otError  error;
    uint32_t start;
    int8_t   rssi = OT_RADIO_RSSI_INVALID;

    OT_UNUSED_VARIABLE(aInstance);

    error = efr32StartEnergyScan(ENERGY_SCAN_MODE_SYNC, sReceiveFrame.mChannel, EFR32_RSSI_AVERAGING_TIME);
    otEXPECT(error == OT_ERROR_NONE);

    start = RAIL_GetTime();

    // waiting for the event RAIL_EVENT_RSSI_AVERAGE_DONE
    while (sEnergyScanStatus == ENERGY_SCAN_STATUS_IN_PROGRESS &&
           ((RAIL_GetTime() - start) < EFR32_RSSI_AVERAGING_TIMEOUT))
        ;

    if (sEnergyScanStatus == ENERGY_SCAN_STATUS_COMPLETED)
    {
        rssi = sEnergyScanResultDbm;
    }

    sEnergyScanStatus = ENERGY_SCAN_STATUS_IDLE;
exit:
    return rssi;
}

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otRadioCaps capabilities = (OT_RADIO_CAPS_ACK_TIMEOUT
                                | OT_RADIO_CAPS_CSMA_BACKOFF
                                | OT_RADIO_CAPS_ENERGY_SCAN
                                | OT_RADIO_CAPS_SLEEP_TO_TX);

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    capabilities |= OT_RADIO_CAPS_TRANSMIT_SEC;
#endif

    // Currently not supporting delayed tx in radio because of single tx / callback design
    // capabilities |= OT_RADIO_CAPS_TRANSMIT_TIMING;

    // Supporting otPlatRadioReceiveAt function in radio
    capabilities |= OT_RADIO_CAPS_RECEIVE_TIMING;

    return capabilities;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sPromiscuous;
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    RAIL_Status_t status;

    sPromiscuous = aEnable;

    status = RAIL_IEEE802154_SetPromiscuousMode(gRailHandle, aEnable);
    assert(status == RAIL_STATUS_NO_ERROR);
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    // set Frame Pending bit for all outgoing ACKs if aEnable is false
    sIsSrcMatchEnabled = aEnable;
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(aPower != NULL, error = OT_ERROR_INVALID_ARGS);
    // RAIL_GetTxPowerDbm() returns power in deci-dBm (0.1dBm)
    // Divide by 10 because aPower is supposed be in units dBm
    *aPower = RAIL_GetTxPowerDbm(gRailHandle) / 10;

exit:
    return error;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    RAIL_Status_t status;

#if OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    sTxPower[otNcpPlatGetCurCommandIid()] = aPower;
    aPower = maxMultipanTxPower();
#endif
    
    // RAIL_SetTxPowerDbm() takes power in units of deci-dBm (0.1dBm)
    // Divide by 10 because aPower is supposed be in units dBm
    status = RAIL_SetTxPowerDbm(gRailHandle, ((RAIL_TxPower_t)aPower) * 10);
    assert(status == RAIL_STATUS_NO_ERROR);

    return OT_ERROR_NONE;
}

// Required for RCP error recovery
// See src/lib/spinel/radio_spinel_impl.hpp::RestoreProperties()
otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel, int8_t aMaxPower)
{
    uint16_t currentChannel;
    otError error = OT_ERROR_NONE;

#if RADIO_CONFIG_2P4GHZ_OQPSK_SUPPORT
    otEXPECT_ACTION(aChannel >= OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MIN && aChannel <= OT_RADIO_2P4GHZ_OQPSK_CHANNEL_MAX,
                    error = OT_ERROR_INVALID_ARGS);
#elif RADIO_CONFIG_SUBGHZ_SUPPORT
    otEXPECT_ACTION(aChannel >= OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MIN
                    && aChannel <= OPENTHREAD_CONFIG_PLATFORM_RADIO_PROPRIETARY_CHANNEL_MAX,
                    error = OT_ERROR_INVALID_ARGS);
#elif RADIO_CONFIG_915MHZ_OQPSK_SUPPORT // Not supported
    otEXPECT_ACTION(aChannel >= OT_RADIO_915MHZ_OQPSK_CHANNEL_MIN && aChannel <= OT_RADIO_915MHZ_OQPSK_CHANNEL_MAX,
                    error = OT_ERROR_INVALID_ARGS);
#endif

    RAIL_GetChannel(gRailHandle, &currentChannel);

    if (aChannel == currentChannel)
    {
        otPlatRadioSetTransmitPower(aInstance, aMaxPower);
    }

exit:
    return error;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_NONE;
    otEXPECT_ACTION(aThreshold != NULL, error = OT_ERROR_INVALID_ARGS);

    *aThreshold = sCcaThresholdDbm;

exit:
    return error;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);

    sCcaThresholdDbm = aThreshold;

    return OT_ERROR_NONE;
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return EFR32_RECEIVE_SENSITIVITY;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    OT_UNUSED_VARIABLE(aInstance);

    return efr32StartEnergyScan(ENERGY_SCAN_MODE_ASYNC, aScanChannel, (RAIL_Time_t)aScanDuration * US_IN_MS);
}

//------------------------------------------------------------------------------
// Radio Config: Thread 1.2 transmit security support

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
void otPlatRadioSetMacKey(otInstance *              aInstance,
                          uint8_t                   aKeyIdMode,
                          uint8_t                   aKeyId,
                          const otMacKeyMaterial    *aPrevKey,
                          const otMacKeyMaterial    *aCurrKey,
                          const otMacKeyMaterial    *aNextKey,
                          otRadioKeyType            aKeyType)
{
    uint8_t iid = otNcpPlatGetCurCommandIid();

    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aKeyIdMode);
    OT_UNUSED_VARIABLE(aKeyType);

    assert(aPrevKey != NULL && aCurrKey != NULL && aNextKey != NULL);

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    sMacKeys[iid].keyId = aKeyId;
    memcpy(&sMacKeys[iid].keys[MAC_KEY_PREV],    aPrevKey, sizeof(otMacKeyMaterial));
    memcpy(&sMacKeys[iid].keys[MAC_KEY_CURRENT], aCurrKey, sizeof(otMacKeyMaterial));
    memcpy(&sMacKeys[iid].keys[MAC_KEY_NEXT],    aNextKey, sizeof(otMacKeyMaterial));

    CORE_EXIT_ATOMIC();
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    uint8_t iid = otNcpPlatGetCurCommandIid();

    OT_UNUSED_VARIABLE(aInstance);

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    sMacKeys[iid].macFrameCounter = aMacFrameCounter;

    CORE_EXIT_ATOMIC();
}

//------------------------------------------------------------------------------
// Radio Config: Enhanced Acks, CSL

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
otError otPlatRadioEnableCsl(otInstance *        aInstance,
                             uint32_t            aCslPeriod,
                             otShortAddress      aShortAddr,
                             const otExtAddress *aExtAddr)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aShortAddr);
    OT_UNUSED_VARIABLE(aExtAddr);

    sCslPeriod = aCslPeriod;

    return OT_ERROR_NONE;
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);

    sCslSampleTime = aCslSampleTime;
}

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otPlatTimeGetXtalAccuracy();
}

uint8_t otPlatRadioGetCslUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return CSL_TX_UNCERTAINTY;
}

#endif // OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE

//------------------------------------------------------------------------------
// Radio Config: Link Metrics

#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
otError otPlatRadioConfigureEnhAckProbing(otInstance *         aInstance,
                                          otLinkMetrics        aLinkMetrics,
                                          const otShortAddress aShortAddress,
                                          const otExtAddress * aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return otLinkMetricsConfigureEnhAckProbing(aShortAddress, aExtAddress, aLinkMetrics);
}
#endif
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

#if OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE
otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled)
{
    OT_UNUSED_VARIABLE(aInstance);

    if (aEnabled && !sl_rail_util_coex_is_enabled()) {
        otLogInfoPlat("Coexistence GPIO configurations not set");
        return OT_ERROR_FAILED;
    }
    sRadioCoexEnabled = aEnabled;
    return OT_ERROR_NONE;
}

bool otPlatRadioIsCoexEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);
    return (sRadioCoexEnabled && sl_rail_util_coex_is_enabled());
}

otError otPlatRadioGetCoexMetrics(otInstance *aInstance, otRadioCoexMetrics *aCoexMetrics)
{
    OT_UNUSED_VARIABLE(aInstance);
    otError error = OT_ERROR_NONE;

    otEXPECT_ACTION(aCoexMetrics != NULL, error = OT_ERROR_INVALID_ARGS);

    memset(aCoexMetrics, 0, sizeof(otRadioCoexMetrics));
    // TO DO:
    // Tracking coex metrics with detailed granularity currently
    // not implemented.
    // memcpy(aCoexMetrics, &sCoexMetrics, sizeof(otRadioCoexMetrics));
exit:
    return error;
}
#endif // OPENTHREAD_CONFIG_PLATFORM_RADIO_COEX_ENABLE

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
//------------------------------------------------------------------------------
// Radio implementation: Enhanced ACKs, CSL

// Return false if we should generate an immediate ACK
// Return true otherwise
static bool writeIeee802154EnhancedAck( RAIL_Handle_t       aRailHandle,
                                        RAIL_RxPacketInfo_t *packetInfoForEnhAck, 
                                        uint8_t             *initialPktReadBytes, 
                                        uint8_t             *receivedPsdu)
{
    // RAIL will generate an Immediate ACK for us.
    // For an Enhanced ACK, we need to generate the whole packet ourselves.

    // An 802.15.4 packet from RAIL should look like:
    // 1/2 |   1/2  | 0/1  |  0/2   | 0/2/8  |  0/2   | 0/2/8  |   14
    // PHR | MacFCF | Seq# | DstPan | DstAdr | SrcPan | SrcAdr | SecHdr

    // With RAIL_IEEE802154_EnableEarlyFramePending(), RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND
    // is triggered after receiving through the SrcAdr field of Version 0/1 packets,
    // and after receiving through the SecHdr for Version 2 packets.

    otRadioFrame receivedFrame, enhAckFrame;
    uint8_t      enhAckPsdu[IEEE802154_MAX_LENGTH];

    #define EARLY_FRAME_PENDING_EXPECTED_BYTES (2U + 2U + 1U + 2U + 8U + 2U + 8U + 14U)
    #define FINAL_PACKET_LENGTH_WITH_IE (EARLY_FRAME_PENDING_EXPECTED_BYTES + OT_ACK_IE_MAX_SIZE)

    otEXPECT((packetInfoForEnhAck != NULL) && (initialPktReadBytes != NULL) && (receivedPsdu != NULL));

    *initialPktReadBytes = readInitialPacketData(   packetInfoForEnhAck,
                                                    EARLY_FRAME_PENDING_EXPECTED_BYTES,
                                                    (PHY_HEADER_SIZE + 2),
                                                    receivedPsdu,
                                                    FINAL_PACKET_LENGTH_WITH_IE);

    uint8_t iid = getIidFromFilterMask(packetInfoForEnhAck->filterMask);

    if (*initialPktReadBytes == 0U) {
        return true; // Nothing to read, which means generating an immediate ACK is also pointless
    }

    receivedFrame.mPsdu     = receivedPsdu + PHY_HEADER_SIZE;
    receivedFrame.mLength   = *initialPktReadBytes - PHY_HEADER_SIZE;
    enhAckFrame.mPsdu       = enhAckPsdu + PHY_HEADER_SIZE;

    if (! otMacFrameIsVersion2015(&receivedFrame))
    {
        return false;
    }

    otMacAddress aSrcAddress;
    uint8_t      linkMetricsDataLen = 0;
    uint8_t      *dataPtr           = NULL;
    bool         setFramePending    = false;

    otMacFrameGetSrcAddr(&receivedFrame, &aSrcAddress);

    if (sIsSrcMatchEnabled && (aSrcAddress.mType != OT_MAC_ADDRESS_TYPE_NONE))
    {
#if _SILICON_LABS_32B_SERIES_1_CONFIG == 1 && OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1        
        if (iid == 0) // on MG1 the RAIL filter mask doesn't work so search all tables
        {
            for (uint8_t i = 1; i <= RADIO_CONFIG_SRC_MATCH_PANID_NUM; i++)
            {
                setFramePending = (aSrcAddress.mType == OT_MAC_ADDRESS_TYPE_EXTENDED
                                   ? (utilsSoftSrcMatchExtFindEntry(i , &aSrcAddress.mAddress.mExtAddress) >= 0)
                                   : (utilsSoftSrcMatchShortFindEntry(i, aSrcAddress.mAddress.mShortAddress) >= 0));
                if (setFramePending)
                {
                    break;
                }
            }
        }
        else
#endif
        {
            setFramePending = (aSrcAddress.mType == OT_MAC_ADDRESS_TYPE_EXTENDED
                               ? (utilsSoftSrcMatchExtFindEntry(iid , &aSrcAddress.mAddress.mExtAddress) >= 0)
                               : (utilsSoftSrcMatchShortFindEntry(iid, aSrcAddress.mAddress.mShortAddress) >= 0));
        }
    }

#if _SILICON_LABS_32B_SERIES_1_CONFIG == 1 && OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1
    otPanId      destPanId;

    destPanId = efr32GetDstPanId(&receivedFrame);
    iid = utilsSoftSrcMatchFindIidFromPanId(destPanId);
#endif

    // Generate our IE header.
    // Write IE data for enhanced ACK (link metrics + allocate bytes for CSL)


#if OPENTHREAD_CONFIG_MLE_LINK_METRICS_SUBJECT_ENABLE
    uint8_t      linkMetricsData[OT_ENH_PROBING_IE_DATA_MAX_SIZE];

    linkMetricsDataLen = otLinkMetricsEnhAckGenData(&aSrcAddress,
                                                    sReceiveFrame.mInfo.mRxInfo.mLqi,
                                                    sReceiveFrame.mInfo.mRxInfo.mRssi,
                                                    linkMetricsData);

    if (linkMetricsDataLen > 0)
    {
        dataPtr = linkMetricsData;
    }
#endif

    sAckIeDataLength = generateAckIeData(dataPtr, linkMetricsDataLen);

    otEXPECT(otMacFrameGenerateEnhAck(&receivedFrame, setFramePending, sAckIeData,
                                          sAckIeDataLength, &enhAckFrame) == OT_ERROR_NONE);

#if OPENTHREAD_CONFIG_MAC_CSL_RECEIVER_ENABLE
    if (sCslPeriod > 0)
    {
        // Update IE data in the 802.15.4 header with the newest CSL period / phase
        otMacFrameSetCslIe(&enhAckFrame, (uint16_t)sCslPeriod, getCslPhase());
    }
#endif

    if (otMacFrameIsSecurityEnabled(&enhAckFrame))
    {
        otEXPECT(radioProcessTransmitSecurity(&enhAckFrame, iid) == OT_ERROR_NONE);
    }

    // Before we're done, store some important info in reserved bits in the
    // MAC header (cleared later)
    // Check whether frame pending is set.
    // Check whether enhanced ACK is secured.
    otEXPECT((skipRxPacketLengthBytes(packetInfoForEnhAck)) == OT_ERROR_NONE);
    uint8_t *macFcfPointer = ((packetInfoForEnhAck->firstPortionBytes == 0)
                               ? (uint8_t *) packetInfoForEnhAck->lastPortionData
                               : (uint8_t *) packetInfoForEnhAck->firstPortionData);

    if (otMacFrameIsSecurityEnabled(&enhAckFrame))
    {
        *macFcfPointer |= IEEE802154_SECURED_OUTGOING_ENHANCED_ACK;
    }

    if (setFramePending)
    {
        *macFcfPointer |= IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK;
    }

    // Fill in PHR now that we know Enh-ACK's length
    if (PHY_HEADER_SIZE == 2U) // Not true till SubGhz implementation is in place
    {
        enhAckPsdu[0] = (0x08U /*FCS=2byte*/ | 0x10U /*Whiten=enabled*/);
        enhAckPsdu[1] = (uint8_t)(__RBIT(enhAckFrame.mLength) >> 24);
    }
    else
    {
        enhAckPsdu[0] = enhAckFrame.mLength;
    }

    RAIL_Status_t enhAckStatus = RAIL_IEEE802154_WriteEnhAck(aRailHandle, enhAckPsdu, enhAckFrame.mLength);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    otEXPECT_ACTION(enhAckStatus == RAIL_STATUS_NO_ERROR, sRailDebugCounters.mRailEventsEnhAckTxFailed++);
#else
    otEXPECT(enhAckStatus == RAIL_STATUS_NO_ERROR);
#endif

exit:
    return true;
}
#endif // (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)

//------------------------------------------------------------------------------
// RAIL callbacks

static void dataRequestCommandCallback(RAIL_Handle_t aRailHandle)
{
    #define MAX_EXPECTED_BYTES (2U + 2U + 1U)           //PHR + FCF + DSN

    uint8_t             receivedPsdu[IEEE802154_MAX_LENGTH];
    uint8_t             pktOffset = PHY_HEADER_SIZE;
    uint8_t             initialPktReadBytes;
    RAIL_RxPacketInfo_t packetInfo;

    // This callback occurs after the address fields of an incoming
    // ACK-requesting CMD or DATA frame have been received and we
    // can do a frame pending check.  We must also figure out what
    // kind of ACK is being requested -- Immediate or Enhanced.

#if (OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2)
    if (writeIeee802154EnhancedAck(aRailHandle, &packetInfo, &initialPktReadBytes, receivedPsdu))
    {
        // We also return true above if there were failures in
        // generating an enhanced ACK.
        return ;
    }
#else
    initialPktReadBytes = readInitialPacketData(&packetInfo,
                                                MAX_EXPECTED_BYTES,
                                                pktOffset + 2,
                                                receivedPsdu,
                                                MAX_EXPECTED_BYTES);    
#endif

    // Calculate frame pending for immediate-ACK
    // If not, RAIL will send an immediate ACK, but we need to do FP lookup.
    RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

    //Check if we read the FCF, if not, set macFcf to 0
    uint16_t macFcf = (initialPktReadBytes <= pktOffset) ? 0U : receivedPsdu[pktOffset];

    bool framePendingSet = false;

    if (sIsSrcMatchEnabled)
    {
        RAIL_IEEE802154_Address_t sourceAddress;

        status = RAIL_IEEE802154_GetAddress(aRailHandle, &sourceAddress);
        otEXPECT(status == RAIL_STATUS_NO_ERROR);

        uint8_t iid = getIidFromFilterMask(packetInfo.filterMask);
#if _SILICON_LABS_32B_SERIES_1_CONFIG == 1 && OPENTHREAD_RADIO && OPENTHREAD_CONFIG_MULTIPAN_RCP_ENABLE == 1        
        if (iid == 0) // on MG1 the RAIL filter mask doesn't work so search all tables
        {
            for (uint8_t i = 1; i <= RADIO_CONFIG_SRC_MATCH_PANID_NUM; i++)
            {
                framePendingSet = (sourceAddress.length == RAIL_IEEE802154_LongAddress
                                   ? (utilsSoftSrcMatchExtFindEntry(i , (otExtAddress *)sourceAddress.longAddress) >= 0)
                                   : (utilsSoftSrcMatchShortFindEntry(i, sourceAddress.shortAddress) >= 0));
                if (framePendingSet)
                {   
                    status = RAIL_IEEE802154_SetFramePending(aRailHandle);
                    otEXPECT(status == RAIL_STATUS_NO_ERROR);
                    break;
                }
            }
        }
        else
#endif
        if ((sourceAddress.length == RAIL_IEEE802154_LongAddress &&
             utilsSoftSrcMatchExtFindEntry(iid, (otExtAddress *)sourceAddress.longAddress) >= 0) ||
            (sourceAddress.length == RAIL_IEEE802154_ShortAddress &&
             utilsSoftSrcMatchShortFindEntry(iid, sourceAddress.shortAddress) >= 0))
        {
            status = RAIL_IEEE802154_SetFramePending(aRailHandle);
            otEXPECT(status == RAIL_STATUS_NO_ERROR);
            framePendingSet = true;
        }
    }
    else if((macFcf & IEEE802154_FRAME_TYPE_MASK) != IEEE802154_FRAME_TYPE_DATA)
    {
        status = RAIL_IEEE802154_SetFramePending(aRailHandle);
        otEXPECT(status == RAIL_STATUS_NO_ERROR);
        framePendingSet = true;
    }

    if (framePendingSet)
    {
        // Store whether frame pending was set in the outgoing ACK in a reserved
        // bit of the MAC header (cleared later)

        otEXPECT((skipRxPacketLengthBytes(&packetInfo)) == OT_ERROR_NONE);
        uint8_t *macFcfPointer = ((packetInfo.firstPortionBytes == 0)
                                  ? (uint8_t *) packetInfo.lastPortionData
                                  : (uint8_t *) packetInfo.firstPortionData);
        *macFcfPointer |= IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK;
    }

exit:
    if (status == RAIL_STATUS_INVALID_STATE)
    {
        otLogWarnPlat("Too late to modify outgoing FP");
    }
    else
    {
        assert(status == RAIL_STATUS_NO_ERROR);
    }
}

static void packetReceivedCallback(RAIL_RxPacketHandle_t packetHandle)
{
    RAIL_RxPacketInfo_t    packetInfo;
    RAIL_RxPacketDetails_t packetDetails;
    uint16_t               length;
    bool                   framePendingInAck = false;
    bool                   rxCorrupted = false;
    uint8_t                iid = 0;

    packetHandle = RAIL_GetRxPacketInfo(gRailHandle, packetHandle, &packetInfo);
    otEXPECT_ACTION((packetHandle != RAIL_RX_PACKET_HANDLE_INVALID
                     && packetInfo.packetStatus == RAIL_RX_PACKET_READY_SUCCESS),
                    rxCorrupted = true);

    otEXPECT_ACTION(validatePacketDetails(packetHandle, &packetDetails, &packetInfo, &length),
                    rxCorrupted = true);

    otEXPECT_ACTION((skipRxPacketLengthBytes(&packetInfo)) == OT_ERROR_NONE,
                    rxCorrupted = true);

    uint8_t macFcf = ((packetInfo.firstPortionBytes == 0)
                      ? packetInfo.lastPortionData[0]
                      : packetInfo.firstPortionData[0]);

    iid = getIidFromFilterMask(packetInfo.filterMask);

    if (packetDetails.isAck)
    {
        otEXPECT_ACTION((length >= IEEE802154_MIN_LENGTH
                         && (macFcf & IEEE802154_FRAME_TYPE_MASK)
                             == IEEE802154_FRAME_TYPE_ACK),
                        rxCorrupted = true);

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventAcksReceived++;
#endif
        // read packet
        RAIL_CopyRxPacket(sReceiveAckFrame.mPsdu, &packetInfo);
        sReceiveAckFrame.mLength = length;

        // Releasing the ACK frames here, ensures that the main thread (processNextRxPacket)
        // is not wasting cycles, releasing the ACK frames from the Rx FIFO queue.
        RAIL_ReleaseRxPacket(gRailHandle, packetHandle);

        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED,
                                   (uint32_t) isReceivingFrame());

        if (txWaitingForAck()
            && (sReceiveAckFrame.mPsdu[IEEE802154_DSN_OFFSET] == sTransmitFrame.mPsdu[IEEE802154_DSN_OFFSET]))
        {
            otEXPECT_ACTION(validatePacketTimestamp(&packetDetails, length), rxCorrupted = true);
            updateRxFrameDetails(&packetDetails, false, false, iid);

            // Processing the ACK frame in ISR context avoids the Tx state to be messed up,
            // in case the Rx FIFO queue gets wiped out in a DMP situation.
            sTransmitBusy = false;
            sTransmitError = OT_ERROR_NONE;
            setInternalFlag(FLAG_WAITING_FOR_ACK, false);

            framePendingInAck = ((macFcf & IEEE802154_FRAME_FLAG_FRAME_PENDING) != 0);
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_RECEIVED,
                                       (uint32_t) framePendingInAck);

            if (txIsDataRequest() && framePendingInAck) {
                emPendingData = true;
            }
        }
        // Yield the radio upon receiving an ACK as long as it is not related to
        // a data request.
        if (!txIsDataRequest()) {
            RAIL_YieldRadio(gRailHandle);
        }
    }
    else
    {
        otEXPECT_ACTION(sPromiscuous || (length >= IEEE802154_MIN_DATA_LENGTH), rxCorrupted = true);

        if (macFcf & IEEE802154_FRAME_FLAG_ACK_REQUIRED) {
            (void) handlePhyStackEvent((RAIL_IsRxAutoAckPaused(gRailHandle)
                                        ? SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED
                                        : SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACKING),
                                        (uint32_t) isReceivingFrame());
            setInternalFlag(FLAG_ONGOING_TX_ACK, true);
        }
        else {
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ENDED,
                                       (uint32_t) isReceivingFrame());
            // We received a frame that does not require an ACK as result of a data
            // poll: we yield the radio here.
            if (emPendingData) {
                RAIL_YieldRadio(gRailHandle);
                emPendingData = false;
            }
        }
    }
exit:
    if (rxCorrupted) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED,
                                   (uint32_t) isReceivingFrame());
    }
}

static void packetSentCallback(bool isAck)
{
    if (isAck) {
        // We successfully sent out an ACK.
        setInternalFlag(FLAG_ONGOING_TX_ACK, false);
         // We acked the packet we received after a poll: we can yield now.
        if (emPendingData) {
            RAIL_YieldRadio(gRailHandle);
            emPendingData = false;
        }

    } else if (getInternalFlag(FLAG_ONGOING_TX_DATA)) {
        setInternalFlag(FLAG_ONGOING_TX_DATA, false);

        if (txWaitingForAck()) {
            setInternalFlag(FLAG_WAITING_FOR_ACK, true);
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_WAITING, 0U);
        }
        else {
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ENDED, 0U);
            RAIL_YieldRadio(gRailHandle);
            sTransmitError = OT_ERROR_NONE;
            sTransmitBusy  = false;
        }
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventPacketSent++;
#endif
    }
}

static void txFailedCallback(bool isAck, uint8_t status)
{
    if (isAck) {
        setInternalFlag(FLAG_ONGOING_TX_ACK, false);
    }
    else if (getInternalFlag(FLAG_ONGOING_TX_DATA)) {
        if (status == TX_COMPLETE_RESULT_CCA_FAIL) {
            sTransmitError = OT_ERROR_CHANNEL_ACCESS_FAILURE;
            setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, false);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            sRailDebugCounters.mRailEventChannelBusy++;
#endif
        } else {
            sTransmitError = OT_ERROR_ABORT;
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            sRailDebugCounters.mRailEventTxAbort++;
#endif
        }
        setInternalFlag(FLAG_ONGOING_TX_DATA, false);
        RAIL_YieldRadio(gRailHandle);
        sTransmitBusy  = false;
    }
}

static void ackTimeoutCallback(void)
{
    assert(txWaitingForAck());
    assert(getInternalFlag(FLAG_WAITING_FOR_ACK));

    sTransmitError = OT_ERROR_NO_ACK;
    sTransmitBusy  = false;
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    sRailDebugCounters.mRailEventNoAck++;
#endif

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    // If antenna diversity is enabled toggle the selected antenna.
    sl_rail_util_ant_div_toggle_tx_antenna();
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    // TO DO: Check if we have an OT function that
    // provides the number of mac retry attempts left
    (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ACK_TIMEDOUT, 0);

    setInternalFlag(FLAG_WAITING_FOR_ACK, false);
    RAIL_YieldRadio(gRailHandle);
    emPendingData = false;
}

static void schedulerEventCallback(RAIL_Handle_t aRailHandle)
{
    RAIL_SchedulerStatus_t status = RAIL_GetSchedulerStatus(aRailHandle);
    assert(status != RAIL_SCHEDULER_STATUS_INTERNAL_ERROR);

    if (status == RAIL_SCHEDULER_STATUS_CCA_CSMA_TX_FAIL
        || status == RAIL_SCHEDULER_STATUS_SINGLE_TX_FAIL
        || status == RAIL_SCHEDULER_STATUS_SCHEDULED_TX_FAIL
        || (status == RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL && sTransmitBusy)
        || (status == RAIL_SCHEDULER_STATUS_EVENT_INTERRUPTED && sTransmitBusy)) {

        if (getInternalFlag(FLAG_ONGOING_TX_ACK)) {
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED,
                                       (uint32_t) isReceivingFrame());
            txFailedCallback(true, 0xFF);
        }
        // We were in the process of TXing a data frame, treat it as a CCA_FAIL.
        if (getInternalFlag(FLAG_ONGOING_TX_DATA)) {
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED,
                                       (uint32_t) txWaitingForAck());
            txFailedCallback(false, TX_COMPLETE_RESULT_CCA_FAIL);
        }

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventSchedulerStatusError++;
#endif
    }
    else if (status == RAIL_SCHEDULER_STATUS_AVERAGE_RSSI_FAIL
             || (status == RAIL_SCHEDULER_STATUS_SCHEDULE_FAIL
                 && sEnergyScanStatus == ENERGY_SCAN_STATUS_IN_PROGRESS))
        {
            energyScanComplete(OT_RADIO_RSSI_INVALID);
        }
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
    else if (sTransmitBusy) {
            sRailDebugCounters.mRailEventsSchedulerStatusLastStatus = status;
            sRailDebugCounters.mRailEventsSchedulerStatusTransmitBusy++;
    }
#endif
}

static void configUnscheduledCallback(void)
{
  // We are waiting for an ACK: we will never get the ACK we were waiting for.
  // We want to call ackTimeoutCallback() only if the PACKET_SENT event
  // already fired (which would clear the FLAG_ONGOING_TX_DATA flag).
  if (getInternalFlag(FLAG_WAITING_FOR_ACK)) {
    ackTimeoutCallback();
  }

  // We are about to send an ACK, which it won't happen.
  if (getInternalFlag(FLAG_ONGOING_TX_ACK)) {
    txFailedCallback(true, 0xFF);
  }
}

static void RAILCb_Generic(RAIL_Handle_t aRailHandle, RAIL_Events_t aEvents)
{
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    if (aEvents & (RAIL_EVENT_RX_SYNC1_DETECT
                  | RAIL_EVENT_RX_SYNC2_DETECT)) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_STARTED, (uint32_t) isReceivingFrame());
    }
#endif// SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    if (aEvents & RAIL_EVENT_SIGNAL_DETECTED) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_SIGNAL_DETECTED, 0U);
    }
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

    if ((aEvents & RAIL_EVENT_IEEE802154_DATA_REQUEST_COMMAND)
#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
        && !RAIL_IsRxAutoAckPaused(aRailHandle)
#endif// SL_CATALOG_RAIL_UTIL_COEX_PRESENT
        ) {
        dataRequestCommandCallback(aRailHandle);
    }

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    if (aEvents & RAIL_EVENT_RX_FILTER_PASSED) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACCEPTED, (uint32_t) isReceivingFrame());
    }
#endif// SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

    if (aEvents & RAIL_EVENT_TX_PACKET_SENT) {
        packetSentCallback(false);
    }
    else if (aEvents & RAIL_EVENT_TX_CHANNEL_BUSY) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED, (uint32_t) txWaitingForAck());
        txFailedCallback(false, TX_COMPLETE_RESULT_CCA_FAIL);

    } else if (aEvents & RAIL_EVENT_TX_BLOCKED) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_BLOCKED,
                                   (uint32_t) txWaitingForAck());
        txFailedCallback(false, TX_COMPLETE_RESULT_OTHER_FAIL);

    } else if (aEvents & (RAIL_EVENT_TX_UNDERFLOW | RAIL_EVENT_TX_ABORTED)) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_ABORTED,
                                   (uint32_t) txWaitingForAck());
        txFailedCallback(false, TX_COMPLETE_RESULT_OTHER_FAIL);

    } else {
    // Pre-completion aEvents are processed in their logical order:
#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
        if (aEvents & RAIL_EVENT_TX_START_CCA) {
            // We are starting RXWARM for a CCA check
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_SOON, 0U);
        }
        if (aEvents & RAIL_EVENT_TX_CCA_RETRY) {
            // We failed a CCA check and need to retry
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_CCA_BUSY, 0U);
        }
        if (aEvents & RAIL_EVENT_TX_CHANNEL_CLEAR) {
            // We're going on-air
            (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_STARTED, 0U);
        }
#endif//SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    }

    if (aEvents & RAIL_EVENT_SCHEDULED_RX_STARTED) {
        sState = OT_RADIO_STATE_RECEIVE;
    }

    if (aEvents & RAIL_EVENT_RX_SCHEDULED_RX_END) {
        radioSetIdle();
    }

    if (aEvents & RAIL_EVENT_RX_PACKET_RECEIVED)
    {
        packetReceivedCallback(RAIL_HoldRxPacket(aRailHandle));
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventPacketReceived++;
#endif
    }

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    if (aEvents & RAIL_EVENT_RX_FRAME_ERROR) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_CORRUPTED, (uint32_t) isReceivingFrame());
    }
    // The following 3 events cause us to not receive a packet
    if (aEvents & (RAIL_EVENT_RX_PACKET_ABORTED
                  | RAIL_EVENT_RX_ADDRESS_FILTERED
                  | RAIL_EVENT_RX_FIFO_OVERFLOW)) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_FILTERED, (uint32_t) isReceivingFrame());
    }
#endif// SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

    if (aEvents & RAIL_EVENT_TXACK_PACKET_SENT) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_SENT, (uint32_t) isReceivingFrame());
        packetSentCallback(true);
    }
    if (aEvents & (RAIL_EVENT_TXACK_ABORTED | RAIL_EVENT_TXACK_UNDERFLOW)) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_ABORTED, (uint32_t) isReceivingFrame());
        txFailedCallback(true, 0xFF);
    }
    if (aEvents & RAIL_EVENT_TXACK_BLOCKED) {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_ACK_BLOCKED, (uint32_t) isReceivingFrame());
        txFailedCallback(true, 0xFF);
    }
    // Deal with ACK timeout after possible RX completion in case RAIL
    // notifies us of the ACK and the timeout simultaneously -- we want
    // the ACK to win over the timeout.
    if (aEvents & RAIL_EVENT_RX_ACK_TIMEOUT) {
        if (getInternalFlag(FLAG_WAITING_FOR_ACK)) {
            ackTimeoutCallback();
        }
    }

    if (aEvents & RAIL_EVENT_CONFIG_UNSCHEDULED)
    {
        (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_RX_IDLED, 0U);
        configUnscheduledCallback();
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventConfigUnScheduled++;
#endif
    }

    if (aEvents & RAIL_EVENT_CONFIG_SCHEDULED)
    {
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventConfigScheduled++;
#endif
    }

    if (aEvents & RAIL_EVENT_SCHEDULER_STATUS) {
        schedulerEventCallback(aRailHandle);
    }

    if (aEvents & RAIL_EVENT_CAL_NEEDED)
    {
        RAIL_Status_t status;

        status = RAIL_Calibrate(aRailHandle, NULL, RAIL_CAL_ALL_PENDING);
        // TODO: Non-RTOS DMP case fails
#if (!defined(SL_CATALOG_BLUETOOTH_PRESENT) || defined(SL_CATALOG_KERNEL_PRESENT))
        assert(status == RAIL_STATUS_NO_ERROR);
#else
        OT_UNUSED_VARIABLE(status);
#endif

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventCalNeeded++;
#endif
    }

    if (aEvents & RAIL_EVENT_RSSI_AVERAGE_DONE)
    {
        const int16_t energyScanResultQuarterDbm = RAIL_GetAverageRssi(aRailHandle);
        RAIL_YieldRadio(aRailHandle);

        energyScanComplete(energyScanResultQuarterDbm == RAIL_RSSI_INVALID
                           ? OT_RADIO_RSSI_INVALID
                           : (energyScanResultQuarterDbm / QUARTER_DBM_IN_DBM));
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailPlatRadioEnergyScanDoneCbCount++;
#endif
    }

    otSysEventSignalPending();
}

//------------------------------------------------------------------------------
// Main thread packet handling

static bool validatePacketDetails(RAIL_RxPacketHandle_t packetHandle,
                                  RAIL_RxPacketDetails_t *pPacketDetails,
                                  RAIL_RxPacketInfo_t *pPacketInfo,
                                  uint16_t *packetLength)
{
    bool pktValid = true;

    otEXPECT_ACTION((RAIL_GetRxPacketDetailsAlt(gRailHandle, packetHandle, pPacketDetails)
                     == RAIL_STATUS_NO_ERROR),
                    pktValid = false);

    otEXPECT_ACTION(isFilterMaskValid(pPacketInfo->filterMask), pktValid = false);

    // RAIL's packetBytes includes the (1 or 2 byte) PHY header but not the 2-byte CRC.
    // We want *packetLength to match the PHY header length so we add 2 for CRC
    // and subtract the PHY header size.
    *packetLength = pPacketInfo->packetBytes + 2U - PHY_HEADER_SIZE;

    if (PHY_HEADER_SIZE == 1) {
        otEXPECT_ACTION(*packetLength == pPacketInfo->firstPortionData[0], pktValid = false);
    } else {
        uint8_t lengthByte = ((pPacketInfo->firstPortionBytes > 1)
                              ? pPacketInfo->firstPortionData[1]
                              : pPacketInfo->lastPortionData[0]);
        otEXPECT_ACTION(*packetLength == (uint16_t)(__RBIT(lengthByte) >> 24), pktValid = false);
    }

    // check the length validity of recv packet; RAIL should take care of this.
    otEXPECT_ACTION((*packetLength >= IEEE802154_MIN_LENGTH
                     && *packetLength <= IEEE802154_MAX_LENGTH),
                    pktValid = false);
exit:
    return pktValid;
}

static bool validatePacketTimestamp(RAIL_RxPacketDetails_t *pPacketDetails, uint16_t packetLength)
{
    bool rxTimestampValid = true;

    // Get the timestamp when the SFD was received
    otEXPECT_ACTION(pPacketDetails->timeReceived.timePosition != RAIL_PACKET_TIME_INVALID,
                    rxTimestampValid = false);

    // + 1 for the 1-byte PHY header
    pPacketDetails->timeReceived.totalPacketBytes = packetLength + 1;

    otEXPECT_ACTION((RAIL_GetRxTimeSyncWordEndAlt(gRailHandle, pPacketDetails)
                     == RAIL_STATUS_NO_ERROR),
                    rxTimestampValid = false);
exit:
    return rxTimestampValid;
}

static void updateRxFrameDetails(RAIL_RxPacketDetails_t *pPacketDetails,
                                 bool                   securedOutgoingEnhancedAck,
                                 bool                   framePendingSetInOutgoingAck,
                                 uint8_t                iid)
{
    assert(pPacketDetails != NULL);

    if (pPacketDetails->isAck) {
        sReceiveAckFrame.mInfo.mRxInfo.mRssi = pPacketDetails->rssi;
        sReceiveAckFrame.mInfo.mRxInfo.mLqi  = pPacketDetails->lqi;
        sReceiveAckFrame.mInfo.mRxInfo.mTimestamp = pPacketDetails->timeReceived.packetTime;
        sReceiveAckFrame.mIid = iid;
    } else {
        sReceiveFrame.mInfo.mRxInfo.mRssi = pPacketDetails->rssi;
        sReceiveFrame.mInfo.mRxInfo.mLqi  = pPacketDetails->lqi;
        sReceiveFrame.mInfo.mRxInfo.mTimestamp = pPacketDetails->timeReceived.packetTime;
        // Set this flag only when the packet is really acknowledged with a secured enhanced ACK.
        sReceiveFrame.mInfo.mRxInfo.mAckedWithSecEnhAck = securedOutgoingEnhancedAck;
        // Set this flag only when the packet is really acknowledged with frame pending set.
        sReceiveFrame.mInfo.mRxInfo.mAckedWithFramePending = framePendingSetInOutgoingAck;
        sReceiveFrame.mIid = iid;

#if OPENTHREAD_CONFIG_THREAD_VERSION >= OT_THREAD_VERSION_1_2
        // Use stored values for these
        sReceiveFrame.mInfo.mRxInfo.mAckKeyId = sMacKeys[iid].ackKeyId;
        sReceiveFrame.mInfo.mRxInfo.mAckFrameCounter = sMacKeys[iid].ackFrameCounter;
#endif
    }
}

static otError skipRxPacketLengthBytes(RAIL_RxPacketInfo_t *pPacketInfo)
{
    otError error = OT_ERROR_NONE;
    otEXPECT_ACTION(pPacketInfo->firstPortionBytes > 0, error = OT_ERROR_FAILED);

    pPacketInfo->firstPortionData += PHY_HEADER_SIZE;
    pPacketInfo->packetBytes -= PHY_HEADER_SIZE;

    if (PHY_HEADER_SIZE == 1 || pPacketInfo->firstPortionBytes > 1) {
        pPacketInfo->firstPortionBytes -= PHY_HEADER_SIZE;
    } else {
        pPacketInfo->firstPortionBytes = 0U;
        // Increment lastPortionData to skip the second byte of the PHY header
        otEXPECT_ACTION(pPacketInfo->lastPortionData != NULL, error = OT_ERROR_FAILED);
        pPacketInfo->lastPortionData++;
    }

exit:
    return error;
}

static void processNextRxPacket(otInstance *aInstance)
{
    RAIL_RxPacketHandle_t  packetHandle = RAIL_RX_PACKET_HANDLE_INVALID;
    RAIL_RxPacketInfo_t    packetInfo;
    RAIL_RxPacketDetails_t packetDetails;
    RAIL_Status_t          status;
    uint16_t               length;
    bool                   rxProcessDone = false;
    uint8_t                iid = 0;

    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    packetHandle = RAIL_GetRxPacketInfo(gRailHandle, RAIL_RX_PACKET_HANDLE_OLDEST_COMPLETE, &packetInfo);
    otEXPECT_ACTION((packetHandle != RAIL_RX_PACKET_HANDLE_INVALID
                     && packetInfo.packetStatus == RAIL_RX_PACKET_READY_SUCCESS),
                    packetHandle = RAIL_RX_PACKET_HANDLE_INVALID);

    iid = getIidFromFilterMask(packetInfo.filterMask);

    otEXPECT(validatePacketDetails(packetHandle, &packetDetails, &packetInfo, &length));

    otEXPECT((skipRxPacketLengthBytes(&packetInfo)) == OT_ERROR_NONE);

    // As received ACK frames are already processed in packetReceivedCallback,
    // we only need to read and process the non-ACK frames here.
    otEXPECT(sPromiscuous
             || (!packetDetails.isAck && (length >= IEEE802154_MIN_DATA_LENGTH)));

    // read packet
    RAIL_CopyRxPacket(sReceiveFrame.mPsdu, &packetInfo);
    sReceiveFrame.mLength = length;

    uint8_t *macFcfPointer = sReceiveFrame.mPsdu;
    sReceiveFrame.mChannel = packetDetails.channel;

    // Check the reserved bits in the MAC header, then clear them.

    // If we sent an enhanced ACK, check if it was secured.
    bool securedOutgoingEnhancedAck = ((*macFcfPointer & IEEE802154_SECURED_OUTGOING_ENHANCED_ACK) != 0);
    *macFcfPointer &= ~IEEE802154_SECURED_OUTGOING_ENHANCED_ACK;

    // Check whether frame pendinng bit was set in the outgoing ACK.
    bool framePendingSetInOutgoingAck = ((*macFcfPointer & IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK) != 0);
    *macFcfPointer &= ~IEEE802154_FRAME_PENDING_SET_IN_OUTGOING_ACK;

    status = RAIL_ReleaseRxPacket(gRailHandle, packetHandle);
    if (status == RAIL_STATUS_NO_ERROR)
    {
        packetHandle = RAIL_RX_PACKET_HANDLE_INVALID;
    }

    otEXPECT(validatePacketTimestamp(&packetDetails, length));
    updateRxFrameDetails(&packetDetails, securedOutgoingEnhancedAck, framePendingSetInOutgoingAck, iid);
    rxProcessDone = true;

exit:
    if (packetHandle != RAIL_RX_PACKET_HANDLE_INVALID)
    {
        RAIL_ReleaseRxPacket(gRailHandle, packetHandle);
    }
    CORE_EXIT_ATOMIC();

    // signal MAC layer
    if (rxProcessDone)
    {
        sReceiveError = OT_ERROR_NONE;

#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet())
        {
            otPlatDiagRadioReceiveDone(aInstance, &sReceiveFrame, sReceiveError);
        }
        else
#endif
        {
            otLogInfoPlat("Received %d bytes", sReceiveFrame.mLength);
            otPlatRadioReceiveDone(aInstance, &sReceiveFrame, sReceiveError);
#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
            sRailDebugCounters.mRailPlatRadioReceiveDoneCbCount++;
#endif
        }
        otSysEventSignalPending();
    }
}

static void processTxComplete(otInstance *aInstance)
{
    if (sState == OT_RADIO_STATE_TRANSMIT && sTransmitBusy == false)
    {
        if (sTransmitError != OT_ERROR_NONE)
        {
            otLogDebgPlat("Transmit failed ErrorCode=%d", sTransmitError);
        }
        sState = OT_RADIO_STATE_RECEIVE;
#if OPENTHREAD_CONFIG_DIAG_ENABLE
        if (otPlatDiagModeGet())
        {
            otPlatDiagRadioTransmitDone(aInstance, &sTransmitFrame, sTransmitError);
        }
        else
#endif
        if (((sTransmitFrame.mPsdu[0] & IEEE802154_FRAME_FLAG_ACK_REQUIRED) == 0) || (sTransmitError != OT_ERROR_NONE))
        {
            otPlatRadioTxDone(aInstance, &sTransmitFrame, NULL, sTransmitError);
        }
        else
        {
            otPlatRadioTxDone(aInstance, &sTransmitFrame, &sReceiveAckFrame, sTransmitError);
        }

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailPlatRadioTxDoneCbCount++;
#endif
        otSysEventSignalPending();
    }
}

void efr32RadioProcess(otInstance *aInstance)
{
    (void) handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TICK, 0U);

    // We should process the received packet first. Adding it at the end of this function,
    // will delay the stack notification until the next call to efr32RadioProcess()
    processNextRxPacket(aInstance);
    processTxComplete(aInstance);

    if (sEnergyScanMode == ENERGY_SCAN_MODE_ASYNC && sEnergyScanStatus == ENERGY_SCAN_STATUS_COMPLETED)
    {
        sEnergyScanStatus = ENERGY_SCAN_STATUS_IDLE;
        otPlatRadioEnergyScanDone(aInstance, sEnergyScanResultDbm);
        otSysEventSignalPending();

#if RADIO_CONFIG_DEBUG_COUNTERS_SUPPORT
        sRailDebugCounters.mRailEventEnergyScanCompleted++;
#endif
    }
}

//------------------------------------------------------------------------------
// Antenna Diversity, Wifi coexistence and Runtime PHY select support

RAIL_Status_t efr32RadioSetCcaMode(uint8_t aMode)
{
    return RAIL_IEEE802154_ConfigCcaMode(gRailHandle, aMode);
}

RAIL_IEEE802154_PtiRadioConfig_t efr32GetPtiRadioConfig(void)
{
    return (RAIL_IEEE802154_GetPtiRadioConfig(gRailHandle));
}

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT

otError setRadioState(otRadioState state)
{
    otError error = OT_ERROR_NONE;

    // Defer idling the radio if we have an ongoing TX task
    otEXPECT_ACTION((!getInternalFlag(ONGOING_TX_FLAGS)), error = OT_ERROR_FAILED);

    switch (state) {
    case OT_RADIO_STATE_RECEIVE:
        otEXPECT_ACTION(radioSetRx(sReceiveFrame.mChannel) == OT_ERROR_NONE, error = OT_ERROR_FAILED);
        break;
    case OT_RADIO_STATE_SLEEP:
        radioSetIdle();
        break;
    default:
        error = OT_ERROR_FAILED;
    }
exit:
    return error;
}

void sl_ot_update_active_radio_config(void)
{
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    // Proceed with PHY selection only if 2.4 GHz band is used
    otEXPECT(sBandConfig.mChannelConfig == NULL);

    otRadioState currentState = sState;
    otEXPECT(setRadioState(OT_RADIO_STATE_SLEEP) == OT_ERROR_NONE);
    sl_rail_util_ieee802154_config_radio(gRailHandle);
    otEXPECT(setRadioState(currentState) == OT_ERROR_NONE);

exit:
    CORE_EXIT_ATOMIC();
    return;
}
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_PHY_SELECT_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
void efr32AntennaConfigInit(void)
{
    RAIL_Status_t status;
    sl_rail_util_ant_div_init();
    status = sl_rail_util_ant_div_update_antenna_config();
    assert(status == RAIL_STATUS_NO_ERROR);
}
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

static void changeDynamicEvents(void)
{
    const RAIL_Events_t eventMask = RAIL_EVENTS_NONE
                                    | RAIL_EVENT_RX_SYNC1_DETECT
                                    | RAIL_EVENT_RX_SYNC2_DETECT
                                    | RAIL_EVENT_RX_FRAME_ERROR
                                    | RAIL_EVENT_RX_FIFO_OVERFLOW
                                    | RAIL_EVENT_RX_ADDRESS_FILTERED
                                    | RAIL_EVENT_RX_PACKET_ABORTED
                                    | RAIL_EVENT_RX_FILTER_PASSED
                                    | RAIL_EVENT_TX_CHANNEL_CLEAR
                                    | RAIL_EVENT_TX_CCA_RETRY
                                    | RAIL_EVENT_TX_START_CCA
                                    | RAIL_EVENT_SIGNAL_DETECTED;
    RAIL_Events_t eventValues = RAIL_EVENTS_NONE;

    if (phyStackEventIsEnabled()) {
        eventValues |= eventMask;
    }
    updateEvents(eventMask, eventValues);
}
#endif // SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT

static void efr32PhyStackInit(void)
{
#ifdef SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT
    efr32AntennaConfigInit();
#endif // SL_CATALOG_RAIL_UTIL_ANT_DIV_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT
    efr32CoexInit();
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT

#ifdef SL_CATALOG_RAIL_UTIL_IEEE802154_STACK_EVENT_PRESENT
    changeDynamicEvents();
#endif
}

#ifdef SL_CATALOG_RAIL_UTIL_COEX_PRESENT

static void emRadioEnableAutoAck(void)
{
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();

    if (getInternalFlag(FLAG_RADIO_INIT_DONE)) {

        if ((rhoActive >= RHO_INT_ACTIVE) // Internal always holds ACKs
            || ((rhoActive > RHO_INACTIVE) && ((sl_rail_util_coex_get_options()
                                                & SL_RAIL_UTIL_COEX_OPT_ACK_HOLDOFF)
                                               != SL_RAIL_UTIL_COEX_OPT_DISABLED))) {
            RAIL_PauseRxAutoAck(gRailHandle, true);
        } else
        {
            RAIL_PauseRxAutoAck(gRailHandle, false);
        }
    }
    CORE_EXIT_ATOMIC();
}

static void emRadioEnablePta(bool enable)
{
    halInternalInitPta();

    // When PTA is enabled, we want to negate PTA_REQ as soon as an incoming
    // frame is aborted, e.g. due to filtering.  To do that we must turn off
    // the TRACKABFRAME feature that's normally on to benefit sniffing on PTI.
    assert(RAIL_ConfigRxOptions(gRailHandle,
                                RAIL_RX_OPTION_TRACK_ABORTED_FRAMES,
                                (enable
                                 ? RAIL_RX_OPTIONS_NONE
                                 : RAIL_RX_OPTION_TRACK_ABORTED_FRAMES))
           == RAIL_STATUS_NO_ERROR);
}

static void efr32CoexInit(void)
{
    sl_rail_util_coex_options_t coexOptions = sl_rail_util_coex_get_options();

#if SL_OPENTHREAD_COEX_MAC_HOLDOFF_ENABLE
    coexOptions |= SL_RAIL_UTIL_COEX_OPT_MAC_HOLDOFF;
#endif // SL_OPENTHREAD_COEX_MAC_HOLDOFF_ENABLE

    sl_rail_util_coex_set_options(coexOptions);

    emRadioEnableAutoAck(); // Might suspend AutoACK if RHO already in effect
    emRadioEnablePta(sl_rail_util_coex_is_enabled());
}

// Managing radio transmission
static void onPtaGrantTx(sl_rail_util_coex_req_t ptaStatus)
{
    // Only pay attention to first PTA Grant callback, ignore any further ones
    if (ptaGntEventReported) {
        return;
    }
    ptaGntEventReported = true;

    assert(ptaStatus == SL_RAIL_UTIL_COEX_REQCB_GRANTED);
    // PTA is telling us we've gotten GRANT and should send ASAP *without* CSMA
    setInternalFlag(FLAG_CURRENT_TX_USE_CSMA, false);
    txCurrentPacket();
}

static void tryTxCurrentPacket(void)
{
    assert(getInternalFlag(FLAG_ONGOING_TX_DATA));

    ptaGntEventReported = false;
    sl_rail_util_ieee802154_stack_event_t ptaStatus
        = handlePhyStackEvent(SL_RAIL_UTIL_IEEE802154_STACK_EVENT_TX_PENDED_MAC,
                              (uint32_t) &onPtaGrantTx);
    if (ptaStatus == SL_RAIL_UTIL_IEEE802154_STACK_STATUS_SUCCESS) {
        // Normal case where PTA allows us to start the (CSMA) transmit below
        txCurrentPacket();
    } else if (ptaStatus == SL_RAIL_UTIL_IEEE802154_STACK_STATUS_CB_PENDING) {
        // onPtaGrantTx() callback will take over (and might already have)
    } else if (ptaStatus == SL_RAIL_UTIL_IEEE802154_STACK_STATUS_HOLDOFF) {
        txFailedCallback(false, TX_COMPLETE_RESULT_OTHER_FAIL);
    }
}

// Managing CCA Threshold
static void setCcaThreshold(void)
{
    if (sCcaThresholdDbm == CCA_THRESHOLD_UNINIT) {
        sCcaThresholdDbm = CCA_THRESHOLD_DEFAULT;
    }
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_ATOMIC();
    int8_t thresholddBm = sCcaThresholdDbm;

    if (getInternalFlag(FLAG_RADIO_INIT_DONE)) {
        if (rhoActive > RHO_INACTIVE) {
            thresholddBm = RAIL_RSSI_INVALID_DBM;
        }
        assert(RAIL_SetCcaThreshold(gRailHandle, thresholddBm) == RAIL_STATUS_NO_ERROR);
    }
    CORE_EXIT_ATOMIC();
}

static void emRadioHoldOffInternalIsr(uint8_t active)
{
    if (active != rhoActive) {
        rhoActive = active; // Update rhoActive early
        if (getInternalFlag(FLAG_RADIO_INIT_DONE)) {
            setCcaThreshold();
            emRadioEnableAutoAck();
        }
    }
}

// External API used by Coex Component
SL_WEAK void emRadioHoldOffIsr(bool active)
{
    emRadioHoldOffInternalIsr((uint8_t) active | (rhoActive & ~RHO_EXT_ACTIVE));
}

#if SL_OPENTHREAD_COEX_COUNTER_ENABLE

void sl_rail_util_coex_counter_on_event(sl_rail_util_coex_event_t event)
{
    otEXPECT(event < SL_RAIL_UTIL_COEX_EVENT_COUNT);
    sCoexCounters[event] += 1;
exit:
    return;
}

void efr32RadioGetCoexCounters(uint32_t (*aCoexCounters)[SL_RAIL_UTIL_COEX_EVENT_COUNT])
{
    memset((void*)aCoexCounters, 0, sizeof(*aCoexCounters));
    memcpy(aCoexCounters, sCoexCounters, sizeof(*aCoexCounters));
}

void efr32RadioClearCoexCounters(void)
{
    memset((void*)sCoexCounters, 0, sizeof(sCoexCounters));
}

#endif // SL_OPENTHREAD_COEX_COUNTER_ENABLE
#endif // SL_CATALOG_RAIL_UTIL_COEX_PRESENT


#if OPENTHREAD_CONFIG_DIAG_ENABLE

otError otPlatDiagTxStreamRandom(void)
{
    RAIL_Status_t status;
    uint16_t streamChannel;

    RAIL_GetChannel(gRailHandle, &streamChannel);

    otLogInfoPlat("Diag Stream PN9 Process", NULL);

    status = RAIL_StartTxStream(gRailHandle, streamChannel, RAIL_STREAM_PN9_STREAM);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamTone(void)
{
    RAIL_Status_t status;
    uint16_t streamChannel;

    RAIL_GetChannel(gRailHandle, &streamChannel);

    otLogInfoPlat("Diag Stream CARRIER-WAVE Process", NULL);

    status = RAIL_StartTxStream(gRailHandle, streamChannel, RAIL_STREAM_CARRIER_WAVE);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamStop(void)
{
    RAIL_Status_t status;

    otLogInfoPlat("Diag Stream STOP Process", NULL);
    
    status = RAIL_StopTxStream(gRailHandle);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamAddrMatch(uint8_t enable)
{
    RAIL_Status_t status;

    otLogInfoPlat("Diag Stream Disable addressMatch", NULL);
    
    status = RAIL_IEEE802154_SetPromiscuousMode(gRailHandle,
                                       !enable);
    assert(status == RAIL_STATUS_NO_ERROR);

    return status;
}

otError otPlatDiagTxStreamAutoAck(uint8_t autoAckEnabled)
{
    RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

    otLogInfoPlat("Diag Stream Disable autoAck", NULL);
    
    RAIL_PauseRxAutoAck(gRailHandle, !autoAckEnabled);

    return status;
}

#endif // OPENTHREAD_CONFIG_DIAG_ENABLE