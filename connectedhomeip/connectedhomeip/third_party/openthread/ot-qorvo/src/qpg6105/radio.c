/*
 *  Copyright (c) 2019, The OpenThread Authors.
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

#include <string.h>

#include <openthread/platform/diag.h>
#include <openthread/platform/radio.h>

#include "utils/code_utils.h"

#include "radio_qorvo.h"

enum
{
    QPG_RECEIVE_SENSITIVITY   = -100, // dBm
    QPG_CSL_ACCURACY          = 50,   // ppm
    QPG_CSL_CLOCK_UNCERTAINTY = 255,  // us
};

enum
{
    IEEE802154_MIN_LENGTH      = 5,
    IEEE802154_MAX_LENGTH      = 127,
    IEEE802154_ACK_LENGTH      = 5,
    IEEE802154_FRAME_TYPE_MASK = 0x7,
    IEEE802154_FRAME_TYPE_ACK  = 0x2,
    IEEE802154_FRAME_PENDING   = 1 << 4,
    IEEE802154_ACK_REQUEST     = 1 << 5,
    IEEE802154_DSN_OFFSET      = 2,
};

enum
{
    QORVO_RSSI_OFFSET  = 73,
    QORVO_CRC_BIT_MASK = 0x80,
    QORVO_LQI_BIT_MASK = 0x7f,
};

extern otRadioFrame sTransmitFrame;

static otRadioState sState;
static otInstance * pQorvoInstance;

typedef struct otCachedSettings_s
{
    uint16_t panid;
} otCachedSettings_t;

static otCachedSettings_t otCachedSettings;

/* Upper layer relies on txpower could be set before receive, but MAC have per-channel config for it.
   Store txpower until channel set in Receive(). */
#define PENDING_TX_POWER_NONE (-1)
static int8_t pendingTxPower = PENDING_TX_POWER_NONE;

static uint8_t sScanstate         = 0;
static int8_t  sLastReceivedPower = 127;

otRadioCaps otPlatRadioGetCaps(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioGetCaps();
}

const char *otPlatRadioGetVersionString(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return "OPENTHREAD/Qorvo/0.0";
}

int8_t otPlatRadioGetReceiveSensitivity(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return QPG_RECEIVE_SENSITIVITY;
}

void otPlatRadioGetIeeeEui64(otInstance *aInstance, uint8_t *aIeeeEui64)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioGetIeeeEui64(aIeeeEui64);
}

void otPlatRadioSetPanId(otInstance *aInstance, otPanId aPanId)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioSetPanId((uint16_t)aPanId);
    otCachedSettings.panid = aPanId;
}

void otPlatRadioSetExtendedAddress(otInstance *aInstance, const otExtAddress *address)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioSetExtendedAddress(address->m8);
}

void otPlatRadioSetShortAddress(otInstance *aInstance, otShortAddress aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioSetShortAddress((uint16_t)aShortAddress);
}

otError otPlatRadioGetTransmitPower(otInstance *aInstance, int8_t *aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError result;

    if (aPower == NULL)
    {
        return OT_ERROR_INVALID_ARGS;
    }

    if ((sState == OT_RADIO_STATE_DISABLED) || (sScanstate != 0))
    {
        *aPower = (pendingTxPower == PENDING_TX_POWER_NONE) ? 0 : pendingTxPower;
        return OT_ERROR_NONE;
    }

    result = qorvoRadioGetTransmitPower(aPower);

    if (result == OT_ERROR_INVALID_STATE)
    {
        // Channel was not set, so txpower is ambigious
        *aPower = (pendingTxPower == PENDING_TX_POWER_NONE) ? 0 : pendingTxPower;
        return OT_ERROR_NONE;
    }

    return result;
}

otError otPlatRadioSetTransmitPower(otInstance *aInstance, int8_t aPower)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError result;

    if ((sState == OT_RADIO_STATE_DISABLED) || (sScanstate != 0))
    {
        pendingTxPower = aPower;
        return OT_ERROR_NONE;
    }

    result = qorvoRadioSetTransmitPower(aPower);

    if (result == OT_ERROR_INVALID_STATE)
    {
        // Channel was not set, so txpower is ambigious
        pendingTxPower = aPower;
        result         = OT_ERROR_NONE;
    }

    return result;
}

otError otPlatRadioGetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t *aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aThreshold);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetCcaEnergyDetectThreshold(otInstance *aInstance, int8_t aThreshold)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aThreshold);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioGetFemLnaGain(otInstance *aInstance, int8_t *aGain)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aGain);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetFemLnaGain(otInstance *aInstance, int8_t aGain)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aGain);

    return OT_ERROR_NOT_IMPLEMENTED;
}

bool otPlatRadioGetPromiscuous(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioGetPromiscuous();
}

void otPlatRadioSetPromiscuous(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioSetPromiscuous(aEnable);
}

void otPlatRadioSetMacKey(otInstance *            aInstance,
                          uint8_t                 aKeyIdMode,
                          uint8_t                 aKeyId,
                          const otMacKeyMaterial *aPrevKey,
                          const otMacKeyMaterial *aCurrKey,
                          const otMacKeyMaterial *aNextKey,
                          otRadioKeyType          aKeyType)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioSetMacKey(aKeyIdMode, aKeyId, aPrevKey->mKeyMaterial.mKey.m8, aCurrKey->mKeyMaterial.mKey.m8,
                        aNextKey->mKeyMaterial.mKey.m8, aKeyType);
}

void otPlatRadioSetMacFrameCounter(otInstance *aInstance, uint32_t aMacFrameCounter)
{
    qorvoRadioSetMacFrameCounter(aMacFrameCounter);
}

uint64_t otPlatRadioGetNow(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioGetNow();
}

uint32_t otPlatRadioGetBusSpeed(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 0;
}

otError otPlatRadioEnable(otInstance *aInstance)
{
    pQorvoInstance = aInstance;
    memset(&otCachedSettings, 0x00, sizeof(otCachedSettings_t));

    if (!otPlatRadioIsEnabled(aInstance))
    {
        sState = OT_RADIO_STATE_SLEEP;
    }

    return OT_ERROR_NONE;
}

otError otPlatRadioDisable(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otEXPECT(otPlatRadioIsEnabled(aInstance));

    if (sState == OT_RADIO_STATE_RECEIVE)
    {
        qorvoRadioSetRxOnWhenIdle(false);
    }

    sState = OT_RADIO_STATE_DISABLED;

exit:
    return OT_ERROR_NONE;
}

bool otPlatRadioIsEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return (sState != OT_RADIO_STATE_DISABLED);
}

otError otPlatRadioSleep(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    otError error = OT_ERROR_INVALID_STATE;

    if (sState == OT_RADIO_STATE_RECEIVE || sState == OT_RADIO_STATE_SLEEP)
    {
        qorvoRadioSetRxOnWhenIdle(false);
        error  = OT_ERROR_NONE;
        sState = OT_RADIO_STATE_SLEEP;
    }
    return error;
}

otError otPlatRadioReceive(otInstance *aInstance, uint8_t aChannel)
{
    otError error = OT_ERROR_INVALID_STATE;

    pQorvoInstance = aInstance;

    if ((sState != OT_RADIO_STATE_DISABLED) && (sScanstate == 0))
    {
        qorvoRadioSetCurrentChannel(aChannel);
        if (pendingTxPower != PENDING_TX_POWER_NONE)
        {
            qorvoRadioSetTransmitPower(pendingTxPower);
            pendingTxPower = PENDING_TX_POWER_NONE;
        }
        error = OT_ERROR_NONE;
    }

    if (sState == OT_RADIO_STATE_SLEEP)
    {
        qorvoRadioSetRxOnWhenIdle(true);
        error  = OT_ERROR_NONE;
        sState = OT_RADIO_STATE_RECEIVE;
    }

    return error;
}

otError otPlatRadioReceiveAt(otInstance *aInstance, uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioReceiveAt(aChannel, aStart, aDuration);
}

void cbQorvoRadioReceiveDone(otRadioFrame *aFrame, otError aError)
{
    if (aError == OT_ERROR_NONE)
    {
        sLastReceivedPower = aFrame->mInfo.mRxInfo.mRssi;
    }

    otPlatRadioReceiveDone(pQorvoInstance, aFrame, aError);
}

void cbQorvoDiagRadioReceiveDone(otRadioFrame *aFrame, otError aError)
{
    otPlatDiagRadioReceiveDone(pQorvoInstance, aFrame, aError);
}

otRadioFrame *otPlatRadioGetTransmitBuffer(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return &sTransmitFrame;
}

otError otPlatRadioTransmit(otInstance *aInstance, otRadioFrame *aFrame)
{
    otError err = OT_ERROR_NONE;

    pQorvoInstance = aInstance;

    otEXPECT_ACTION(sState != OT_RADIO_STATE_DISABLED, err = OT_ERROR_INVALID_STATE);

    err = qorvoRadioTransmit(aFrame);

exit:
    return err;
}

void cbQorvoRadioTxStarted(otRadioFrame *aFrame)
{
    otPlatRadioTxStarted(pQorvoInstance, aFrame);
}

void cbQorvoRadioTransmitDone_AckFrame(otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError)
{
    otPlatRadioTxDone(pQorvoInstance, aFrame, aAckFrame, aError);
}

void cbQorvoRadioTransmitDone(otRadioFrame *aFrame, bool aFramePending, otError aError)
{
    // TODO: pass received ACK frame instead of generating one.
    otRadioFrame ackFrame;
    uint8_t      psdu[IEEE802154_ACK_LENGTH];

    ackFrame.mPsdu    = psdu;
    ackFrame.mLength  = IEEE802154_ACK_LENGTH;
    ackFrame.mPsdu[0] = IEEE802154_FRAME_TYPE_ACK;

    if (aFramePending)
    {
        ackFrame.mPsdu[0] |= IEEE802154_FRAME_PENDING;
    }

    ackFrame.mPsdu[1] = 0;
    ackFrame.mPsdu[2] = aFrame->mPsdu[IEEE802154_DSN_OFFSET];

    otPlatRadioTxDone(pQorvoInstance, aFrame, &ackFrame, aError);
}

void cbQorvoDiagRadioTransmitDone(otRadioFrame *aFrame, otError aError)
{
    otPlatDiagRadioTransmitDone(pQorvoInstance, aFrame, aError);
}

int8_t otPlatRadioGetRssi(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return sLastReceivedPower;
}

otError otPlatRadioEnergyScan(otInstance *aInstance, uint8_t aScanChannel, uint16_t aScanDuration)
{
    OT_UNUSED_VARIABLE(aInstance);

    sScanstate = 1;
    return qorvoRadioEnergyScan(aScanChannel, aScanDuration);
}

void cbQorvoRadioEnergyScanDone(int8_t aEnergyScanMaxRssi)
{
    sScanstate = 0;
    otPlatRadioEnergyScanDone(pQorvoInstance, aEnergyScanMaxRssi);
}

void otPlatRadioEnableSrcMatch(otInstance *aInstance, bool aEnable)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioEnableSrcMatch(aEnable);
}

otError otPlatRadioAddSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioAddSrcMatchShortEntry((uint16_t)aShortAddress, otCachedSettings.panid);
}

otError otPlatRadioAddSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioAddSrcMatchExtEntry(aExtAddress->m8, otCachedSettings.panid);
}

otError otPlatRadioClearSrcMatchShortEntry(otInstance *aInstance, otShortAddress aShortAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioClearSrcMatchShortEntry((uint16_t)aShortAddress, otCachedSettings.panid);
}

otError otPlatRadioClearSrcMatchExtEntry(otInstance *aInstance, const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioClearSrcMatchExtEntry(aExtAddress->m8, otCachedSettings.panid);
}

void otPlatRadioClearSrcMatchShortEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    /* clear both short and extended addresses here */
    qorvoRadioClearSrcMatchEntries();
}

void otPlatRadioClearSrcMatchExtEntries(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    /* not implemented */
    /* assumes clearing of short and extended entries is done simultaniously by the openthread stack */
}
uint32_t otPlatRadioGetSupportedChannelMask(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 0x03fff800;
}

uint32_t otPlatRadioGetPreferredChannelMask(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return 0x03fff800;
}

otError otPlatRadioSetCoexEnabled(otInstance *aInstance, bool aEnabled)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aEnabled);

    return OT_ERROR_NOT_IMPLEMENTED;
}

bool otPlatRadioIsCoexEnabled(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return false;
}

otError otPlatRadioGetCoexMetrics(otInstance *aInstance, otRadioCoexMetrics *aCoexMetrics)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aCoexMetrics);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioEnableCsl(otInstance *        aInstance,
                             uint32_t            aCslPeriod,
                             otShortAddress      aShortAddr,
                             const otExtAddress *aExtAddr)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioEnableCsl(aCslPeriod, (uint16_t)aShortAddr, aExtAddr->m8);
}

void otPlatRadioUpdateCslSampleTime(otInstance *aInstance, uint32_t aCslSampleTime)
{
    OT_UNUSED_VARIABLE(aInstance);

    qorvoRadioUpdateCslSampleTime(aCslSampleTime);
}

uint8_t otPlatRadioGetCslAccuracy(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return QPG_CSL_ACCURACY;
}

uint8_t otPlatRadioGetCslClockUncertainty(otInstance *aInstance)
{
    OT_UNUSED_VARIABLE(aInstance);

    return QPG_CSL_CLOCK_UNCERTAINTY;
}

otError otPlatRadioSetChannelMaxTransmitPower(otInstance *aInstance, uint8_t aChannel, int8_t aMaxPower)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aChannel);
    OT_UNUSED_VARIABLE(aMaxPower);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioSetRegion(otInstance *aInstance, uint16_t aRegionCode)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aRegionCode);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioGetRegion(otInstance *aInstance, uint16_t *aRegionCode)
{
    OT_UNUSED_VARIABLE(aInstance);
    OT_UNUSED_VARIABLE(aRegionCode);

    return OT_ERROR_NOT_IMPLEMENTED;
}

otError otPlatRadioConfigureEnhAckProbing(otInstance *        aInstance,
                                          otLinkMetrics       aLinkMetrics,
                                          otShortAddress      aShortAddress,
                                          const otExtAddress *aExtAddress)
{
    OT_UNUSED_VARIABLE(aInstance);

    return qorvoRadioConfigureEnhAckProbing(aLinkMetrics, (uint16_t)aShortAddress, aExtAddress->m8);
}
