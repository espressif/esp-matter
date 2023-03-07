/*
 *  Copyright (c) 2018, The OpenThread Authors.
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
 *   This file defines the frequency band configuration structure for efr32.
 *
 */

#ifndef PLATFORM_BAND_H_
#define PLATFORM_BAND_H_

#include <openthread/platform/radio.h>

#include "rail.h"
#include "rail_config.h"
#include "rail_ieee802154.h"

#define RAIL_TX_FIFO_SIZE (OT_RADIO_FRAME_MAX_SIZE + 1)

#define RADIO_SCHEDULER_BACKGROUND_RX_PRIORITY 255
#define RADIO_SCHEDULER_CHANNEL_SCAN_PRIORITY 255
#define RADIO_SCHEDULER_CHANNEL_SLIP_TIME 500000UL
#define RADIO_SCHEDULER_TX_PRIORITY 100
#define RADIO_SCHEDULER_TX_SLIP_TIME 500000UL

#define RADIO_TIMING_CSMA_OVERHEAD_US 500
#define RADIO_TIMING_DEFAULT_BYTETIME_US 32   // only used if RAIL_GetBitRate returns 0
#define RADIO_TIMING_DEFAULT_SYMBOLTIME_US 16 // only used if RAIL_GetSymbolRate returns 0

typedef struct efr32RadioCounters
{
    uint32_t mRailPlatTxTriggered;
    uint32_t mRailPlatRadioReceiveDoneCbCount;
    uint32_t mRailPlatRadioEnergyScanDoneCbCount;
    uint32_t mRailPlatRadioTxDoneCbCount;
    uint32_t mRailTxStarted;
    uint32_t mRailTxStartFailed;
    uint32_t mRailEventAcksReceived;
    uint32_t mRailEventConfigScheduled;
    uint32_t mRailEventConfigUnScheduled;
    uint32_t mRailEventPacketSent;
    uint32_t mRailEventChannelBusy;
    uint32_t mRailEventEnergyScanCompleted;
    uint32_t mRailEventCalNeeded;
    uint32_t mRailEventPacketReceived;
    uint32_t mRailEventNoAck;
    uint32_t mRailEventTxAbort;
    uint32_t mRailEventSchedulerStatusError;
    uint32_t mRailEventsSchedulerStatusTransmitBusy;
    uint32_t mRailEventsSchedulerStatusLastStatus;
    uint32_t mRailEventsEnhAckTxFailed;
} efr32RadioCounters;


typedef struct efr32CommonConfig
{
    RAIL_Config_t mRailConfig;
#if RADIO_CONFIG_DMP_SUPPORT
    RAILSched_Config_t mRailSchedState;
#endif
    union {
        // Used to align this buffer as needed
        RAIL_FIFO_ALIGNMENT_TYPE align[RAIL_TX_FIFO_SIZE / RAIL_FIFO_ALIGNMENT];
        uint8_t fifo[RAIL_TX_FIFO_SIZE]; // must be 2 power between 64 and 4096, and bigger than OT_RADIO_FRAME_MAX_SIZE
    } mRailTxFifo;
} efr32CommonConfig;

typedef struct efr32BandConfig
{
    const RAIL_ChannelConfig_t *mChannelConfig;
    uint8_t                     mChannelMin;
    uint8_t                     mChannelMax;
} efr32BandConfig;

#endif // PLATFORM_BAND_H_
