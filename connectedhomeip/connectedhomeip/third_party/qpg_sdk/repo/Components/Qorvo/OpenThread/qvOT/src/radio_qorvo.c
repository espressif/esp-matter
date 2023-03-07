/*
 * Copyright (c) 2017-2020, Qorvo Inc
 *
 * radio_qorvo.c
 *   This file contains the implementation of the qorvo radio api for openthread.
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

#define GP_COMPONENT_ID GP_COMPONENT_ID_APP

// #define GP_LOCAL_LOG
#define LOG_PREFIX "[Q] Radio---------: "

// #define DEBUG

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#include "platform_qorvo.h"
#include <openthread/platform/radio.h>
#include <openthread/platform/diag.h>

#include "global.h"
#include "radio_qorvo.h"
#include "alarm_qorvo.h"
#include "gpMacDispatcher.h"
#include "gpMacCore_defsDefines.h"
#include "gpLog.h"
#include "gpEncryption.h"
#include "gpAssert.h"
#include "gpSched.h"
#include "gpPd.h"

/*****************************************************************************
 *                    Compile Time Verifications
 *****************************************************************************/

#ifndef GP_SCHED_FREE_CPU_TIME
GP_COMPILE_TIME_VERIFY(HAL_DEFAULT_GOTOSLEEP_THRES>0);
#endif //GP_SCHED_FREE_CPU_TIME

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


#define QVOT_IS_MAC_2015_FRAME(frameControl)    (gpMacCore_MacVersion2015 == (MACCORE_FRAMECONTROL_FRAMEVERSION_GET((frameControl))))
#define QVOT_IS_ACK_FRAME(frameControl)         (gpMacCore_FrameTypeAcknowledge == (MACCORE_FRAMECONTROL_FRAMETYPE_GET((frameControl))))
#define QVOT_ACKED_WITH_FP(frameControl)        (MACCORE_FRAMECONTROL_FRAMEPENDING_GET((frameControl)))
#define QVOT_SECURITY_ENABLED(frameControl)     (MACCORE_FRAMECONTROL_SECURITY_GET((frameControl)))

/*****************************************************************************
 *                    Static Function Prototypes
 *****************************************************************************/

static void gpMacDispatcher_cbDataIndication(const gpMacCore_AddressInfo_t* pSrcAddrInfo, const gpMacCore_AddressInfo_t* pDstAddrInfo, uint8_t dsn,
                                             gpMacCore_Security_t* pSecOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId);
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
static void gpMacDispatcher_cbScanConfirm(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, uint32_t unscannedChannels,
                                          UInt8 resultListSize, UInt8* pResultList, gpMacCore_StackId_t stackId);
#endif // GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
static void gpMacDispatcher_cbDataConfirm(gpMacCore_Result_t status, uint8_t msduHandle, gpMacCore_StackId_t stackId);

static void gpMacDispatcher_cbDriverResetIndication(gpMacCore_Result_t status, gpMacCore_StackId_t stackId);

static void gpMacDispatcher_cbSecurityFrameCounterIndication(uint32_t framecounter, gpMacDispatcher_StackId_t stackId);

gpMacDispatcher_Callbacks_t mac802154_callbacks = {
    gpMacDispatcher_cbDataIndication,
    gpMacDispatcher_cbDataConfirm,
    NULL, // gpMacDispatcher_cbPollIndication,
    NULL, // gpMacDispatcher_cbPollConfirm,
    NULL, // gpMacDispatcher_cbPurgeConfirm,
    NULL, // gpMacDispatcher_cbBeaconNotifyIndication,
#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    gpMacDispatcher_cbScanConfirm,
#else
#error "Thread Interfaces MUST perform ED and Active scans"
#endif // GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
    NULL, // gpMacDispatcher_cbAssocIndication,
    NULL, // gpMacDispatcher_cbAssocConfirm,
    NULL, // gpMacDispatcher_cbOrphanIndication_t
    NULL, // gpMacDispatcher_cbSecurityFailureCommStatusIndication,
    NULL, // gpMacDispatcher_cbAssociateCommStatusIndication_t
    NULL, // gpMacDispatcher_cbOrphanCommStatusIndication_t
    gpMacDispatcher_cbDriverResetIndication,
    NULL, // gpMacDispatcher_cbPollNotify,
    gpMacDispatcher_cbSecurityFrameCounterIndication
};

static gpMacCore_StackId_t qorvoGetStackId(void);
static void qorvoSetStackId(gpMacCore_StackId_t stackId);
static bool qorvoValidStackId(gpMacCore_StackId_t stackId);
static otError qorvoToThreadError(gpMacCore_Result_t res);
static uint8_t qorvoGetFrameCounterIndex(uint8_t* frame);

/*****************************************************************************
 *                    Type Definitions
 *****************************************************************************/

/*****************************************************************************
 *                    Static Data Definitions
 *****************************************************************************/

static otRadioFrame sReceiveFrame;

static uint8_t sTransmitPsdu[OT_RADIO_FRAME_MAX_SIZE];
static uint8_t sReceivePsdu[OT_RADIO_FRAME_MAX_SIZE];
static otError sTransmitStatus;
static uint32_t sFrameCounter;
static bool sFrameCounterSet;

#ifdef QVOT_THREAD_1_2
#define QVOT_THREAD_1_2_ENABLED 1
#else
#define QVOT_THREAD_1_2_ENABLED 0
#endif // QVOT_THREAD_1_2

#ifdef QVOT_THREAD_1_2
static bool qorvoDataConfirmReceived;
static bool qorvoDataIndicationReceived;
#endif // QVOT_THREAD_1_2


// Storage of MAC settings changed by OT
static MACAddress_t qorvoOriginalExtendedMac;
static MACAddress_t qorvoRadioExtendedMac;
static uint16_t qorvoRadioPanId;
static uint16_t qorvoRadioShortAddress;
static bool qorvoRadioRxOnWhenIdle;

static uint8_t qorvoScanResult[1];
static uint8_t qorvoThreadChannel;

#define OPENTHREAD_STRING_IDENTIFIER OTHR
static gpMacCore_StackId_t openThreadStackId = GP_MAC_DISPATCHER_INVALID_STACK_ID;


/*****************************************************************************
 *                    Public Data Definitions
 *****************************************************************************/

// Accessed from radio.c OpenThread glue
otRadioFrame sTransmitFrame;


/*****************************************************************************
 *                    Static Function Definitions
 *****************************************************************************/

gpMacCore_StackId_t qorvoGetStackId(void)
{
    return openThreadStackId;
}

void qorvoSetStackId(gpMacCore_StackId_t stackId)
{
    openThreadStackId = stackId;
}

static bool qorvoValidStackId(gpMacCore_StackId_t stackId)
{
    bool result = true;
    if(stackId != qorvoGetStackId())
    {
        GP_LOG_SYSTEM_PRINTF(LOG_PREFIX "CRIT: stackId %u != qorvoStackId %u", 0, stackId, qorvoGetStackId());
        GP_ASSERT_DEV_INT(0);
        result = false;
    }
    return result;
}

otError qorvoToThreadError(gpMacCore_Result_t res)
{
    switch(res)
    {
        case gpMacCore_ResultSuccess:
        {
            return OT_ERROR_NONE;
            break;
        }
        case gpMacCore_ResultTransactionOverflow:
        {
            return OT_ERROR_BUSY;
            break;
        }
        case gpMacCore_ResultInvalidParameter:
        {
            return OT_ERROR_INVALID_ARGS;
            break;
        }
        case gpMacCore_ResultChannelAccessFailure:
        {
            return OT_ERROR_CHANNEL_ACCESS_FAILURE;
            break;
        }
        case gpMacCore_ResultNoAck:
        {
            return OT_ERROR_NO_ACK;
            break;
        }
        default:
        {
            return OT_ERROR_GENERIC;
            break;
        }
    }
}

static uint8_t qorvoGetFrameCounterIndex(uint8_t* frame)
{
    uint8_t idx = 4; // FrameControl(2) + SequenceNumber(1) + SecurityHeader(1)

    uint16_t frameControl = (frame[1] << 8) + frame[0];

    // if there is a panid it is in bytes 3 and 4
    // we ignore the possibility of a second panid here
    if ((frame[4] << 8) + frame[3] == qorvoRadioPanId)
    {
        idx += 2;
    }

    if (MACCORE_FRAMECONTROL_DSTADDRMODE_GET(frameControl) == gpMacCore_AddressModeShortAddress)
    {
        idx += 2;
    }
    else if (MACCORE_FRAMECONTROL_DSTADDRMODE_GET(frameControl) == gpMacCore_AddressModeExtendedAddress)
    {
        idx += 8;
    }

    if (MACCORE_FRAMECONTROL_SRCADDRMODE_GET(frameControl) == gpMacCore_AddressModeShortAddress)
    {
        idx += 2;
    }
    else if (MACCORE_FRAMECONTROL_SRCADDRMODE_GET(frameControl) == gpMacCore_AddressModeExtendedAddress)
    {
        idx += 8;
    }
    return idx;
}



/*****************************************************************************
 *                    gpMacDispatcher callbacks
 *****************************************************************************/

void gpMacDispatcher_cbDataIndication(const gpMacCore_AddressInfo_t* pSrcAddrInfo, const gpMacCore_AddressInfo_t* pDstAddrInfo, uint8_t dsn,
                                      gpMacCore_Security_t* pSecOptions, gpPd_Loh_t pdLoh, gpMacCore_StackId_t stackId)
{
    // Note: the cbDataIndication is used for reporting
    // * Received data/command frames
    // * Received Enhanced Acks

    NOT_USED(pSecOptions);
    NOT_USED(pSrcAddrInfo);
    NOT_USED(pDstAddrInfo);
    NOT_USED(dsn);

    if(!qorvoValidStackId(stackId))
    {
        return;
    }

    // Copy packet into OT structure
    gpPd_ReadByteStream(pdLoh.handle, pdLoh.offset, pdLoh.length, &sReceiveFrame.mPsdu[0]);

    // Frame Control bytes of the received frame
    uint16_t fctrlPsdu = (sReceiveFrame.mPsdu[1] << 8) + sReceiveFrame.mPsdu[0];

    sReceiveFrame.mPsdu[pdLoh.length] = 0x00;     // dummy crc byte
    sReceiveFrame.mPsdu[pdLoh.length + 1] = 0x00; // dummy crc byte
    sReceiveFrame.mLength = pdLoh.length + 2;
    sReceiveFrame.mChannel = gpPd_GetRxChannel(pdLoh.handle);
    sReceiveFrame.mInfo.mRxInfo.mRssi = gpPd_GetRssi(pdLoh.handle);
    sReceiveFrame.mInfo.mRxInfo.mLqi = gpPd_GetLqi(pdLoh.handle);
#ifdef QVOT_THREAD_1_2
    UInt32 rxTs = gpPd_GetRxTimestampChip(pdLoh.handle);
    sReceiveFrame.mInfo.mRxInfo.mTimestamp =
        (uint64_t)gpPd_GetRxTimestampChip(pdLoh.handle) + ((uint64_t)(qorvoTimeWrapAroundCounter + ((rxTs < qorvoTimePreviousValue ? 1 : 0))) << 32);
#else
    sReceiveFrame.mInfo.mRxInfo.mTimestamp = (uint64_t)gpPd_GetRxTimestampChip(pdLoh.handle);
#endif // QVOT_THREAD_1_2

#ifdef QVOT_THREAD_1_2
    uint16_t fctrlAck = 0;
#endif // QVOT_THREAD_1_2

    if(!QVOT_IS_ACK_FRAME(fctrlPsdu))
    {
#ifdef QVOT_THREAD_1_2
        // Frame Control bytes of the transmitted Ack frame (in response the received pdLoh)
        fctrlAck = gpPd_GetFrameControlFromTxAckAfterRx(pdLoh.handle);

        sReceiveFrame.mInfo.mRxInfo.mAckedWithFramePending = QVOT_ACKED_WITH_FP(fctrlAck);
        sReceiveFrame.mInfo.mRxInfo.mAckedWithSecEnhAck = QVOT_SECURITY_ENABLED(fctrlAck);
        sReceiveFrame.mInfo.mRxInfo.mAckFrameCounter = gpPd_GetFrameCounterFromTxAckAfterRx(pdLoh.handle);
#else
        sReceiveFrame.mInfo.mRxInfo.mAckedWithFramePending = true;
        sReceiveFrame.mInfo.mRxInfo.mAckedWithSecEnhAck = false;
#endif // QVOT_THREAD_1_2
    }

    gpPd_FreePd(pdLoh.handle);

    GP_LOG_PRINTF(LOG_PREFIX "cbDataInd: sn:%u ch:%u t:%u fp:%u secAck:%u ts: %llu", 0,
            sReceiveFrame.mPsdu[2],
            sReceiveFrame.mChannel,
            MACCORE_FRAMECONTROL_FRAMETYPE_GET(fctrlPsdu),
#ifdef QVOT_THREAD_1_2
            QVOT_ACKED_WITH_FP((!QVOT_IS_ACK_FRAME(fctrlPsdu)) ? fctrlAck : fctrlPsdu),
#else
            false,
#endif // QVOT_THREAD_1_2
            sReceiveFrame.mInfo.mRxInfo.mAckedWithSecEnhAck,
            sReceiveFrame.mInfo.mRxInfo.mTimestamp);

#ifdef QVOT_THREAD_1_2
    bool isEnhancedAckFrame = (QVOT_IS_MAC_2015_FRAME(fctrlPsdu) && QVOT_IS_ACK_FRAME(fctrlPsdu));


    if(isEnhancedAckFrame)
    {
        if(qorvoDataConfirmReceived)
        {
            cbQorvoRadioTransmitDone_AckFrame(&sTransmitFrame, &sReceiveFrame, sTransmitStatus);
            qorvoDataIndicationReceived = false;
            qorvoDataConfirmReceived = false;
        }
        else
        {
            qorvoDataIndicationReceived = true;
            qorvoDataConfirmReceived = false;
        }
    }
    else
#endif // QVOT_THREAD_1_2
    {
        cbQorvoRadioReceiveDone(&sReceiveFrame, OT_ERROR_NONE);
    }
}

void gpMacDispatcher_cbDataConfirm(gpMacCore_Result_t status, uint8_t msduHandle, gpMacCore_StackId_t stackId)
{
    if(!qorvoValidStackId(stackId))
    {
        return;
    }

#if defined(GP_DIVERSITY_PD_USE_PBM_VARIANT)
    sTransmitFrame.mChannel = gpPd_GetTxChannel(msduHandle);
#else
    // We reuse the value from the DataRequest, which is already written
    // in @sTransmitFrame
#endif // defined(GP_DIVERSITY_PD_USE_PBM_VARIANT)

    sTransmitStatus = qorvoToThreadError(status);
    if((sTransmitStatus == OT_ERROR_NONE) ||
       (sTransmitStatus == OT_ERROR_NO_ACK))
    {
        // mIsHeaderUpdated needs to be set, when we have updated
        // the frame with e.g. the frame counter or csl IEs
        sTransmitFrame.mInfo.mTxInfo.mIsHeaderUpdated = true;
    }
    else
    {
        sTransmitFrame.mInfo.mTxInfo.mIsHeaderUpdated = false;
    }

    // Copy the transmitted frame from the Pd to the sTransmitFrame
    // But the length of the transmitted frame was already stored in the sTransmitFrame
    // when receiving the transmit request
    gpPd_ReadByteStream(msduHandle, 0, sTransmitFrame.mLength, sTransmitFrame.mPsdu);

    uint16_t fctrl;
    gpPd_ReadByteStream(msduHandle, 0, 2, (uint8_t*)&fctrl);
    GP_LOG_PRINTF(LOG_PREFIX "cbDataCfm: sn:%u res:%x l:%u ch:%u", 0,
            gpPd_ReadByte(msduHandle, 2),
            status,
            sTransmitFrame.mLength,
            sTransmitFrame.mChannel);

#ifdef QVOT_THREAD_1_2
    if (QVOT_IS_MAC_2015_FRAME(fctrl))
    {
        if(qorvoDataIndicationReceived)
        {
            cbQorvoRadioTransmitDone_AckFrame(&sTransmitFrame, &sReceiveFrame, sTransmitStatus);
            qorvoDataConfirmReceived = false;
            qorvoDataIndicationReceived = false;
        }
        else if(status != gpMacCore_ResultSuccess)
        {
            // Transmission failed, we don't expect the ack anymore
            cbQorvoRadioTransmitDone_AckFrame(&sTransmitFrame, &sReceiveFrame, sTransmitStatus);
            qorvoDataConfirmReceived = false;
            qorvoDataIndicationReceived = false;
        }
        else
        {
            qorvoDataConfirmReceived = true;
            qorvoDataIndicationReceived = false;
        }
    }
    else
#endif // QVOT_THREAD_1_2
    {
        bool aFramePending = true;
        uint8_t fp;

        fp = gpPd_GetFramePendingAfterTx(msduHandle);
        /* use the fp value unless it's invalid (0xff), then use 'true' */
        aFramePending = (fp == 0xff) ? true : (fp == 1);

        if(sFrameCounterSet)
        {
            // Write the correct frame counter in `sTransmitFrame`
            uint8_t fc_idx = qorvoGetFrameCounterIndex(sTransmitFrame.mPsdu);
            MEMCPY(&sTransmitFrame.mPsdu[fc_idx], &sFrameCounter, sizeof(sFrameCounter));
            sFrameCounterSet = false;
        }
        cbQorvoRadioTransmitDone(&sTransmitFrame, aFramePending, sTransmitStatus);
    }
    gpPd_FreePd(msduHandle);
}

#ifdef GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR
void gpMacDispatcher_cbScanConfirm(gpMacCore_Result_t status, gpMacCore_ScanType_t scanType, uint32_t unscannedChannels,
                                   uint8_t resultListSize, uint8_t* pResultList, gpMacCore_StackId_t stackId)
{
    NOT_USED(status);
    NOT_USED(scanType);
    NOT_USED(unscannedChannels);
    NOT_USED(resultListSize);
    NOT_USED(pResultList);

    if(!qorvoValidStackId(stackId))
    {
        return;
    }

    int8_t aEnergyScanMaxRssi = ((uint8_t*)pResultList)[0];

    cbQorvoRadioEnergyScanDone(aEnergyScanMaxRssi);
}
#endif // GP_MACCORE_DIVERSITY_SCAN_ORIGINATOR

void gpMacDispatcher_cbDriverResetIndication(gpMacCore_Result_t status, gpMacCore_StackId_t stackId)
{
    NOT_USED(status);

    if(!qorvoValidStackId(stackId))
    {
        return;
    }

    gpMacDispatcher_StringIdentifier_t openThreadStringId = {{XSTRINGIFY(OPENTHREAD_STRING_IDENTIFIER)}};

    //register NWK layer to MAC layer
    qorvoSetStackId(gpMacDispatcher_RegisterNetworkStack(&openThreadStringId));
    gpMacDispatcher_RegisterCallbacks(qorvoGetStackId(), &mac802154_callbacks);

    qorvoRadioReset();
    gpMacDispatcher_SetRxOnWhenIdle(qorvoRadioRxOnWhenIdle, qorvoGetStackId());
    gpMacDispatcher_SetExtendedAddress(&qorvoRadioExtendedMac, qorvoGetStackId());
    gpMacDispatcher_SetShortAddress(qorvoRadioShortAddress, qorvoGetStackId());
    gpMacDispatcher_SetPanId(qorvoRadioPanId, qorvoGetStackId());
}

/*****************************************************************************
 *                    Public Function Definitions
 *****************************************************************************/

void qorvoRadioReset(void)
{
    gpMacDispatcher_Reset(true, qorvoGetStackId());

    GP_LOG_PRINTF(LOG_PREFIX "otst=%d bl=%x", 0, qorvoGetStackId(), 0xFF);
    gpMacDispatcher_SetStackInRawMode(1, qorvoGetStackId());

    gpMacDispatcher_SetRetransmitOnCcaFail(QVOT_THREAD_1_2_ENABLED, qorvoGetStackId());
    gpMacDispatcher_SetRetransmitRandomBackoff(QVOT_THREAD_1_2_ENABLED, qorvoGetStackId());
    gpMacDispatcher_EnableEnhancedFramePending(QVOT_THREAD_1_2_ENABLED, qorvoGetStackId());

    gpMacDispatcher_SetTransactionPersistenceTime(0, qorvoGetStackId());
    gpMacDispatcher_SetMacVersion(gpMacCore_MacVersion2006, qorvoGetStackId());

#ifdef QVOT_THREAD_1_2
    qorvoTimePreviousValue = gpMacDispatcher_GetCurrentTimeUs();
    qorvoTimeWrapAroundCounter = 0;
#endif // QVOT_THREAD_1_2
    qorvoThreadChannel = GP_MACCORE_INVALID_CHANNEL;

}

void qorvoRadioInit(void)
{
    gpMacDispatcher_StringIdentifier_t openThreadStringId = {{XSTRINGIFY(OPENTHREAD_STRING_IDENTIFIER)}};

    sTransmitFrame.mLength = 0;
    sTransmitFrame.mPsdu = sTransmitPsdu;
    sReceiveFrame.mLength = 0;
    sReceiveFrame.mPsdu = sReceivePsdu;

    //register NWK layer to MAC layer
    qorvoSetStackId(gpMacDispatcher_RegisterNetworkStack(&openThreadStringId));
    gpMacDispatcher_RegisterCallbacks(qorvoGetStackId(), &mac802154_callbacks);

    qorvoRadioReset();

    // Save initial IEEE MAC address
    gpMacDispatcher_GetExtendedAddress(&qorvoOriginalExtendedMac, qorvoGetStackId());
    MEMCPY(&qorvoRadioExtendedMac, &qorvoOriginalExtendedMac, sizeof(MACAddress_t));
    qorvoRadioShortAddress = GP_MACCORE_SHORT_ADDR_UNALLOCATED;
    qorvoRadioPanId = GP_MACCORE_SHORT_ADDR_UNALLOCATED;

    // Set sleep behavior
    hal_SleepSetGotoSleepEnable(true);

}

otError qorvoRadioTransmit(otRadioFrame* aFrame)
{
    uint8_t offset = 0;
    gpPd_Loh_t pdLoh;
    gpMacCore_Security_t secOptions;
    gpMacCore_MultiChannelOptions_t multiChannelOptions;
    gpMacCore_AddressMode_t srcAddrMode;
    gpMacCore_AddressInfo_t dstAddrInfo;
    uint8_t txOptions = GP_MACCORE_TX_OPT_NONE;

    pdLoh.handle = gpPd_GetPd();
    if(pdLoh.handle == GP_PD_INVALID_HANDLE)
    {
        GP_LOG_SYSTEM_PRINTF(LOG_PREFIX "WARN: no more pd handles!", 0);
        return OT_ERROR_NO_BUFS;
    }

    pdLoh.length = aFrame->mLength - offset;
    pdLoh.length -= 2; // drop the 2 crc bytes

    pdLoh.offset = 0;
    gpPd_WriteByteStream(pdLoh.handle, pdLoh.offset, pdLoh.length, aFrame->mPsdu + offset);

    if (aFrame->mChannel < 11 || aFrame->mChannel > 26)
    {
        GP_ASSERT_DEV_INT(aFrame->mChannel >= 11);
        GP_ASSERT_DEV_INT(aFrame->mChannel <= 26);
        return OT_ERROR_INVALID_ARGS;
    }
    multiChannelOptions.channel[0] = aFrame->mChannel;
    multiChannelOptions.channel[1] = GP_MACCORE_INVALID_CHANNEL;
    multiChannelOptions.channel[2] = GP_MACCORE_INVALID_CHANNEL;

    MEMSET(&secOptions, 0, sizeof(gpMacCore_Security_t));

    uint16_t fctrl = (aFrame->mPsdu[1] << 8) + aFrame->mPsdu[0];
    srcAddrMode = MACCORE_FRAMECONTROL_SRCADDRMODE_GET(fctrl);
    dstAddrInfo.addressMode = MACCORE_FRAMECONTROL_DSTADDRMODE_GET(fctrl);

    // Handle mIsSecurityProcessed
    if(QVOT_SECURITY_ENABLED(fctrl) && !aFrame->mInfo.mTxInfo.mIsSecurityProcessed)
    {
        secOptions.securityLevel = gpEncryption_SecLevelENC_MIC32;
    }
    else
    {
        // No absolute need to set it, the memset took care or this, but it's clearer
        secOptions.securityLevel = gpEncryption_SecLevelNothing;
    }

    // Set the Raw option for the frame
    txOptions |= GP_MACCORE_TX_OPT_RAW;

    gpMacDispatcher_SetNumberOfRetries(aFrame->mInfo.mTxInfo.mMaxFrameRetries, qorvoGetStackId());

    {
        gpMacDispatcher_SetMaxCsmaBackoffs(aFrame->mInfo.mTxInfo.mMaxCsmaBackoffs, qorvoGetStackId());
    }

    GP_LOG_PRINTF(LOG_PREFIX "DataReq: sn:%u fctrl:%04x l:%u csma:%u csl:%u sec:%u|%u hdrU:%u retr:%u bT:%lu dt:%lu", 0,
            aFrame->mPsdu[2],
            fctrl,
            aFrame->mLength,
            aFrame->mInfo.mTxInfo.mCsmaCaEnabled,
            aFrame->mInfo.mTxInfo.mCslPresent,
            aFrame->mInfo.mTxInfo.mIsSecurityProcessed,
            QVOT_SECURITY_ENABLED(fctrl),
            aFrame->mInfo.mTxInfo.mIsHeaderUpdated,
            aFrame->mInfo.mTxInfo.mMaxFrameRetries,
            (unsigned long)aFrame->mInfo.mTxInfo.mTxDelayBaseTime,
            (unsigned long)aFrame->mInfo.mTxInfo.mTxDelay);

    // Handle mIsHeaderUpdated
    if(aFrame->mInfo.mTxInfo.mIsHeaderUpdated)
    {
        // OpenThread has set the CSL IEs and/or FrameCounter,
        // MacCore/RT should not update those anymore
        txOptions |= GP_MACCORE_TX_OPT_RAW_KEEP_FRAMECOUNTER;
    }

    sFrameCounterSet = false; // Reset FrameCounter

    gpMacDispatcher_DataRequest(srcAddrMode, &dstAddrInfo, txOptions, &secOptions, multiChannelOptions, pdLoh, qorvoGetStackId());


    sTransmitFrame.mLength = aFrame->mLength;
    return OT_ERROR_NONE;
}

void gpMacDispatcher_cbSecurityFrameCounterIndication(uint32_t frameCounter, gpMacDispatcher_StackId_t stackId)
{
    if(!qorvoValidStackId(stackId))
    {
        return;
    }
    sFrameCounter = frameCounter;
    sFrameCounterSet = true;
}

void qorvoRadioProcess(void)
{
}

void qorvoRadioGetIeeeEui64(uint8_t* aIeeeEui64)
{
    for(uint8_t i = 0; i < OT_EXT_ADDRESS_SIZE; i++)
    {
        aIeeeEui64[i] = ((uint8_t*)(&qorvoOriginalExtendedMac))[7 - i];
    }
    GP_LOG_PRINTF(LOG_PREFIX "otPlatRadioGetIeeeEui64 = %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", 0,
                  aIeeeEui64[0], aIeeeEui64[1], aIeeeEui64[2], aIeeeEui64[3],
                  aIeeeEui64[4], aIeeeEui64[5], aIeeeEui64[6], aIeeeEui64[7]);
}

void qorvoRadioSetCurrentChannel(uint8_t channel)
{
    if (qorvoThreadChannel == GP_MACCORE_INVALID_CHANNEL)
    {
        // No Thread channel known, accept channel
        GP_LOG_PRINTF(LOG_PREFIX "initiate channel to %u", 0, channel);
        gpMacDispatcher_SetCurrentChannel(channel, qorvoGetStackId());
        qorvoThreadChannel = channel;
    }
    else if (qorvoThreadChannel != gpMacDispatcher_GetCurrentChannel(qorvoGetStackId()))
    {
        // We assume CSL is ongoing, so we store the new channel, but do not switch
        qorvoThreadChannel = channel;
        GP_LOG_PRINTF(LOG_PREFIX "update thread channel to %u, working on csl channel %u", 0, channel, gpMacDispatcher_GetCurrentChannel(qorvoGetStackId()));
    }
    else if (qorvoThreadChannel != channel)
    {
        GP_LOG_PRINTF(LOG_PREFIX "update channel to %u", 0, channel);
        gpMacDispatcher_SetCurrentChannel(channel, qorvoGetStackId());
        qorvoThreadChannel = channel;
    }
    else
    {
        // Channel doesn't change
    }
}

void qorvoRadioSetRxOnWhenIdle(bool rxOnWhenIdle)
{
    qorvoRadioRxOnWhenIdle = rxOnWhenIdle;
    // GP_LOG_PRINTF(LOG_PREFIX "RxOnWhenIdle: %u", 0, rxOnWhenIdle);
    gpMacDispatcher_SetRxOnWhenIdle(rxOnWhenIdle, qorvoGetStackId());
}

void qorvoRadioSetPanId(uint16_t panid)
{
    GP_LOG_PRINTF(LOG_PREFIX "SetPanId: 0x%04x", 0, panid);
    qorvoRadioPanId = panid;
    gpMacDispatcher_SetPanId(panid, qorvoGetStackId());
}

void qorvoRadioSetExtendedAddress(const uint8_t* address)
{
    MEMCPY(&qorvoRadioExtendedMac, address, sizeof(MACAddress_t));
    GP_LOG_PRINTF(LOG_PREFIX "SetExtendedAddress: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", 0,
                  address[0], address[1], address[2], address[3],
                  address[4], address[5], address[6], address[7]);
    gpMacDispatcher_SetExtendedAddress(&qorvoRadioExtendedMac, qorvoGetStackId());
}

void qorvoRadioSetShortAddress(uint16_t address)
{
    GP_LOG_PRINTF(LOG_PREFIX "SetShortAddress: 0x%04x", 0, address);
    qorvoRadioShortAddress = address;
    gpMacDispatcher_SetShortAddress(address, qorvoGetStackId());
}

void qorvoRadioEnableSrcMatch(bool aEnable)
{
    gpMacCore_Result_t res;

    if(aEnable)
    {
        res = gpMacDispatcher_SetDataPendingMode(gpMacCore_DataPendingModeMac802154, qorvoGetStackId());
        GP_ASSERT_DEV_INT(res == gpMacCore_ResultSuccess);
    }
    else
    {
        res = gpMacDispatcher_SetDataPendingMode(gpMacCore_DataPendingModeForNonNeighbourDevices, qorvoGetStackId());
        GP_ASSERT_DEV_INT(res == gpMacCore_ResultSuccess);
    }
}

otError qorvoRadioAddSrcMatchShortEntry(const uint16_t aShortAddress, uint16_t panid)
{
    gpMacCore_Result_t res;
    gpMacCore_AddressInfo_t addrInfo;

    addrInfo.addressMode = gpMacCore_AddressModeShortAddress;
    addrInfo.panId = panid;
    addrInfo.address.Short = aShortAddress;

    res = gpMacDispatcher_DataPending_QueueAdd(&addrInfo, qorvoGetStackId());
    GP_LOG_PRINTF(LOG_PREFIX "Add SrcMatchEntry (res: %u): panid: %04x, addr: %04x", 0,
                  res, panid, aShortAddress);
    return qorvoToThreadError(res);
}

otError qorvoRadioAddSrcMatchExtEntry(const uint8_t* aExtAddress, uint16_t panid)
{
    gpMacCore_Result_t res;
    gpMacCore_AddressInfo_t addrInfo;

    addrInfo.addressMode = gpMacCore_AddressModeExtendedAddress;
    addrInfo.panId = panid;
    MEMCPY(&addrInfo.address.Extended, aExtAddress, sizeof(MACAddress_t));

    res = gpMacDispatcher_DataPending_QueueAdd(&addrInfo, qorvoGetStackId());
    GP_LOG_PRINTF(LOG_PREFIX "Add SrcMatchEntry (res: %u): panid: %04x, addr: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", 0,
                  res, panid,
                  aExtAddress[0], aExtAddress[1], aExtAddress[2], aExtAddress[3],
                  aExtAddress[4], aExtAddress[5], aExtAddress[6], aExtAddress[7]);
    return qorvoToThreadError(res);
}

otError qorvoRadioClearSrcMatchShortEntry(const uint16_t aShortAddress, uint16_t panid)
{
    gpMacCore_Result_t res;
    gpMacCore_AddressInfo_t addrInfo;

    addrInfo.addressMode = gpMacCore_AddressModeShortAddress;
    addrInfo.panId = panid;
    addrInfo.address.Short = aShortAddress;

    res = gpMacDispatcher_DataPending_QueueRemove(&addrInfo, qorvoGetStackId());
    GP_LOG_PRINTF(LOG_PREFIX "Del SrcMatchEntry (res: %u): panid: %04x, addr: %04x", 0,
                  res, panid, aShortAddress);
    return qorvoToThreadError(res);
}

otError qorvoRadioClearSrcMatchExtEntry(const uint8_t* aExtAddress, uint16_t panid)
{
    gpMacCore_Result_t res;
    gpMacCore_AddressInfo_t addrInfo;

    addrInfo.addressMode = gpMacCore_AddressModeExtendedAddress;
    addrInfo.panId = panid;
    MEMCPY(&addrInfo.address.Extended, aExtAddress, sizeof(MACAddress_t));

    res = gpMacDispatcher_DataPending_QueueRemove(&addrInfo, qorvoGetStackId());
    GP_LOG_PRINTF(LOG_PREFIX "Del SrcMatchEntry (res: %u): panid: %04x, addr: %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x", 0,
                  res, panid,
                  aExtAddress[0], aExtAddress[1], aExtAddress[2], aExtAddress[3],
                  aExtAddress[4], aExtAddress[5], aExtAddress[6], aExtAddress[7]);
    return qorvoToThreadError(res);
}

void qorvoRadioClearSrcMatchEntries(void)
{
    /* clear both short and extended addresses here */
    GP_LOG_PRINTF(LOG_PREFIX "Clear all SrcMatchEntries", 0);
    gpMacDispatcher_DataPending_QueueClear(qorvoGetStackId());
}

otError qorvoRadioGetTransmitPower(int8_t* aPower)
{
    uint8_t channel = gpMacDispatcher_GetCurrentChannel(qorvoGetStackId());
    if(channel == GP_MACCORE_INVALID_CHANNEL)
    {
        return OT_ERROR_INVALID_STATE;
    }
    *aPower = gpMacDispatcher_GetTransmitPower(qorvoGetStackId());
    return OT_ERROR_NONE;
}

otError qorvoRadioSetTransmitPower(int8_t aPower)
{
    uint8_t channel = gpMacDispatcher_GetCurrentChannel(qorvoGetStackId());
    if(channel == GP_MACCORE_INVALID_CHANNEL)
    {
        return OT_ERROR_INVALID_STATE;
    }
    gpMacDispatcher_SetTransmitPower(aPower, qorvoGetStackId());
    return OT_ERROR_NONE;
}

otError qorvoRadioEnergyScan(uint8_t aScanChannel, uint16_t aScanDuration)
{
    uint32_t scanChannels = 1 << aScanChannel;
    uint8_t scanDuration = 0;
    uint32_t durationInUs = (((uint32_t)GP_MACCORE_BASE_SUPERFRAME_DURATION * ((uint32_t)(((uint16_t)1) << scanDuration) + 1)) * GP_MACCORE_SYMBOL_DURATION);
    uint8_t resultListSize = 1;
    uint8_t* pResultList = qorvoScanResult;

    GP_ASSERT_DEV_INT(scanDuration < 16);

    while((durationInUs < ((uint32_t)aScanDuration * 1000)) && (scanDuration < 15))
    {
        scanDuration++;
        durationInUs = (((uint32_t)GP_MACCORE_BASE_SUPERFRAME_DURATION * ((uint32_t)(((uint16_t)1) << scanDuration) + 1)) * GP_MACCORE_SYMBOL_DURATION);
    }

    gpMacDispatcher_ScanRequest(gpMacCore_ScanTypeED, scanChannels, scanDuration, resultListSize, pResultList, qorvoGetStackId());

    return OT_ERROR_NONE;
}

bool qorvoRadioGetPromiscuous(void)
{
    return gpMacDispatcher_GetPromiscuousMode(qorvoGetStackId());
}

void qorvoRadioSetPromiscuous(bool aEnable)
{
    gpMacDispatcher_SetPromiscuousMode(aEnable, qorvoGetStackId());
}

void qorvoRadioSetMacKey(uint8_t        aKeyIdMode,
                         uint8_t        aKeyId,
                         const uint8_t *aPrevKey,
                         const uint8_t *aCurrKey,
                         const uint8_t *aNextKey,
                         uint8_t        aKeyType)
{
    NOT_USED(aPrevKey);
    NOT_USED(aNextKey);
    NOT_USED(aKeyIdMode);
    NOT_USED(aKeyId);
    NOT_USED(aCurrKey);
    NOT_USED(aKeyType);
}

uint64_t qorvoRadioGetNow(void)
{
    return UINT64_MAX;
}

otError qorvoRadioReceiveAt(uint8_t aChannel, uint32_t aStart, uint32_t aDuration)
{
    NOT_USED(aChannel);
    NOT_USED(aStart);
    NOT_USED(aDuration);
    GP_LOG_PRINTF(LOG_PREFIX "RxWindows not implemented", 0);
    return OT_ERROR_NOT_IMPLEMENTED;
}


void qorvoRadioSetMacFrameCounter(uint32_t aMacFrameCounter)
{
    NOT_USED(aMacFrameCounter);
}

otError qorvoRadioEnableCsl(uint32_t       aCslPeriod,
                            uint16_t       aShortAddr,
                            const uint8_t *aExtAddr)
{
    NOT_USED(aShortAddr);
    NOT_USED(aExtAddr);
    NOT_USED(aCslPeriod);
    return OT_ERROR_NOT_IMPLEMENTED;
}

void qorvoRadioUpdateCslSampleTime(uint32_t aCslSampleTime)
{
    NOT_USED(aCslSampleTime);
}

otError qorvoRadioConfigureEnhAckProbing(otLinkMetrics  aLinkMetrics,
                                         uint16_t       aShortAddress,
                                         const uint8_t *aExtAddress)
{
#ifndef QVOT_THREAD_1_2
    NOT_USED(aLinkMetrics);
    NOT_USED(aShortAddress);
    NOT_USED(aExtAddress);
    return OT_ERROR_NOT_IMPLEMENTED;
#else
    if (aExtAddress == NULL)
    {
        GP_LOG_PRINTF(LOG_PREFIX "WARN: Invalid extended address", 0);
        return OT_ERROR_INVALID_ARGS;
    }

    gpMacCore_Result_t res;

    MACAddress_t extendedAddress;
    MEMCPY(&extendedAddress, aExtAddress, sizeof(MACAddress_t));

    uint8_t link_metrics = 0;
    if(aLinkMetrics.mPduCount)
    {
        BIT_SET(link_metrics, 0);
    }
    if(aLinkMetrics.mLqi)
    {
        BIT_SET(link_metrics, 1);
    }
    if(aLinkMetrics.mLinkMargin)
    {
        BIT_SET(link_metrics, 2);
    }
    if(aLinkMetrics.mRssi)
    {
        BIT_SET(link_metrics, 3);
    }

    res = gpMacDispatcher_ConfigureEnhAckProbing(link_metrics, &extendedAddress, (uint16_t)aShortAddress, qorvoGetStackId());
#ifdef GP_LOCAL_LOG
    uint8_t* a = (uint8_t*)aExtAddress;
    if (res == gpMacCore_ResultSuccess)
    {
        GP_LOG_PRINTF(LOG_PREFIX "link metrics %02x configuration for 0x%04x | %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x succeeded", 0,
            link_metrics, aShortAddress, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7]);
    }
    else
    {
        GP_LOG_PRINTF(LOG_PREFIX "link metrics %02x configuration for 0x%04x | %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x failed: 0x%02x", 0,
            link_metrics, aShortAddress, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], res);
    }
#endif // GP_LOCAL_LOG
    return qorvoToThreadError(res);
#endif // QVOT_THREAD_1_2
}

otRadioCaps qorvoRadioGetCaps(void)
{
    otRadioCaps caps = OT_RADIO_CAPS_NONE;  ///< Radio supports no capability.
    caps |= OT_RADIO_CAPS_ACK_TIMEOUT;      ///< Radio supports AckTime event.
    caps |= OT_RADIO_CAPS_TRANSMIT_RETRIES; ///< Radio supports tx retry logic with collision avoidance (CSMA).
    caps |= OT_RADIO_CAPS_CSMA_BACKOFF;     ///< Radio supports CSMA backoff for frame transmission (but no retry).
    caps |= OT_RADIO_CAPS_ENERGY_SCAN;      ///< Radio supports Energy Scans.

    return caps;
}
