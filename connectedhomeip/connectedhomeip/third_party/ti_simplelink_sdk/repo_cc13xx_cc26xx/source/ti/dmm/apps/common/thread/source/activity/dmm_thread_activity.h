/*
 *  Copyright (c) 2016, The OpenThread Authors.
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
#ifndef DMM_THREAD_ACTIVITY_H_
#define DMM_THREAD_ACTIVITY_H_

#include <otinstance.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @file
 * @brief
 *   This file defines the DMM Thread activity tracking interface
 *
 */
/* Thread Activity table priority derivation */
#define CALC_ACTIVITY_PRIORITY(ACTIVITY, PRIORITY) ((ACTIVITY << 16) | PRIORITY)

/* Thread Activity priorities */
#define THREAD_ACTIVITY_PRI_NORMAL_INDEX  (0)
#define THREAD_ACTIVITY_PRI_HIGH_INDEX    (1)
#define THREAD_ACTIVITY_PRI_URGENT_INDEX  (2)

#define THREAD_ACTIVITY_THRESHOLD_PRI_NORMAL (1)
#define THREAD_ACTIVITY_THRESHOLD_PRI_HIGH   (2)
#define THREAD_ACTIVITY_THRESHOLD_PRI_URGENT (3)

typedef enum
{
    THREAD_ACTIVITY_TX_DATA     = 0x0001,
    THREAD_ACTIVITY_TX_POLL     = 0x0002,
    THREAD_ACTIVITY_RX_POLL     = 0x0003,
    THREAD_ACTIVITY_RX_SCAN     = 0x0004,
    THREAD_ACTIVITY_RX_IDLE     = 0x0005,
} threadActivity_t;

typedef struct {
    uint8_t numMissedDataTxFrames; /* Number of missed direct/indirect data frames */
    uint8_t numMissedPollDataTxFrames; /* Number of missed polled data frames */
    threadActivity_t activity; /* Current activity associated with the Transmitter */
} threadTxActivityData_t;

typedef struct {
    uint8_t numRxPollAbort; /* Number of aborted polling Rx commands */
    uint8_t numRxScanAbort; /* Number of aborted scan commands */
    uint8_t numRxIdleAbort; /* Number of aborted Idle commands */
    threadActivity_t activity; /* Current activity associated with the Receiver */
} threadRxActivityData_t;

void dmmSetActivityTrackingRx(bool success);
void dmmSetActivityTrackingTx(bool success);
void dmmSetThreadActivityRx(otInstance *aInstance, bool isPolling);
void dmmSetThreadActivityTx(otInstance *aInstance, bool isPolling);
uint32_t dmmGetActivityPriorityTx(void);
uint32_t dmmGetActivityPriorityRx(void);

#ifdef __cplusplus
} // end of extern "C"
#endif

#endif // DMM_THREAD_ACTIVITY_H_
