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
 *   This file includes the declarations of the radio functions from the Qorvo library.
 *
 */

#ifndef _RADIO_QORVO_H_
#define _RADIO_QORVO_H_

#include <stdbool.h>
#include <stdint.h>

#include <openthread/error.h>
#include <openthread/platform/radio.h>

/**
 * This function initializes the radio.
 *
 */
void qorvoRadioInit(void);

/**
 * This function resets the radio.
 *
 */
void qorvoRadioReset(void);

/**
 * This function processes event to/from the radio.
 *
 */
void qorvoRadioProcess(void);

/**
 * This function starts an ED scan.
 *
 * @param[in]  aScanChannel  The channel which needs to be scanned.
 * @param[in]  aScanDuration The amount of time in ms which needs to be scanned.
 *
 */
otError qorvoRadioEnergyScan(uint8_t aScanChannel, uint16_t aScanDuration);

/**
 * This function sets the current channel.
 *
 * @param[in]  channel   The channel index.
 *
 */
void qorvoRadioSetCurrentChannel(uint8_t channel);

/**
 * This function sets the idle behaviour of the radio.
 *
 * @param[in]  rxOnWhenIdle  If true, the radio remains on which not transmitting.
 *
 */
void qorvoRadioSetRxOnWhenIdle(bool rxOnWhenIdle);

/**
 * This function retrieves the MAC address of the radio.
 *
 * @param[out]  aIeeeEui64  The MAC address of the radio.
 *
 */
void qorvoRadioGetIeeeEui64(uint8_t *aIeeeEui64);

/**
 * This function transmits a frame.
 *
 * @param[in]  aFrame  The frame which needs to be transmitted.
 *
 */
otError qorvoRadioTransmit(otRadioFrame *aFrame);

/**
 * This function sets the PanId.
 *
 * @param[in]  panid  The panId.
 *
 */
void qorvoRadioSetPanId(uint16_t panid);

/**
 * This function sets the short address.
 *
 * @param[in]  address  The short address.
 *
 */
void qorvoRadioSetShortAddress(uint16_t address);

/**
 * This function sets the extended address.
 *
 * @param[in]  address  The extended address.
 *
 */
void qorvoRadioSetExtendedAddress(const uint8_t *address);

/**
 * This function enables source address matching for indirect transmit.
 *
 * @param[in]  aEnable  if True will enable source address matching, false will disable.
 *
 */
void qorvoRadioEnableSrcMatch(bool aEnable);

/**
 * This function clears all entries from the source address match list.
 *
 */
void qorvoRadioClearSrcMatchEntries(void);

/**
 * This function adds an short address plus panid to the source address match list.
 *
 * @param[in]  aShortAddress  The short address which should be added.
 * @param[in]  panid          The panid.
 *
 */
otError qorvoRadioAddSrcMatchShortEntry(uint16_t aShortAddress, uint16_t panid);

/**
 * This function adds an extended address plus panid to the source address match list.
 *
 * @param[in]  aExtAddress    The extended address which should be added.
 * @param[in]  panid          The panid.
 *
 */
otError qorvoRadioAddSrcMatchExtEntry(const uint8_t *aExtAddress, uint16_t panid);

/**
 * This function removes an short address plus panid from the source address match list.
 *
 * @param[in]  aShortAddress  The short address which should be removed.
 * @param[in]  panid          The panid.
 *
 */
otError qorvoRadioClearSrcMatchShortEntry(uint16_t aShortAddress, uint16_t panid);

/**
 * This function removes an extended address plus panid from the source address match list.
 *
 * @param[in]  aExtAddress    The extended address which should be removed.
 * @param[in]  panid          The panid.
 *
 */
otError qorvoRadioClearSrcMatchExtEntry(const uint8_t *aExtAddress, uint16_t panid);

/**
 * This function gets the transmit power for current channel
 *
 * @param[out]  aPower  The transmit power
 *
 */
otError qorvoRadioGetTransmitPower(int8_t *aPower);

/**
 * This function sets the transmit power for current channel
 *
 * @param[in]  aPower  The transmit power
 *
 */
otError qorvoRadioSetTransmitPower(int8_t aPower);

/**
 * Get the status of promiscuous mode.
 *
 * @retval TRUE   Promiscuous mode is enabled.
 * @retval FALSE  Promiscuous mode is disabled.
 *
 */
bool qorvoRadioGetPromiscuous(void);

/**
 * Enable or disable promiscuous mode.
 *
 * @param[in]  aEnable   TRUE to enable or FALSE to disable promiscuous mode.
 *
 */
void qorvoRadioSetPromiscuous(bool aEnable);

/**
 * Update MAC keys and key index
 *
 * This function is used when radio provides OT_RADIO_CAPS_TRANSMIT_SEC capability.
 *
 * @param[in]  aKeyIdMode  The key ID mode.
 * @param[in]  aKeyId      Current MAC key index.
 * @param[in]  aPrevKey    A pointer to the previous MAC key.
 * @param[in]  aCurrKey    A pointer to the current MAC key.
 * @param[in]  aNextKey    A pointer to the next MAC key.
 * @param[in]  aKeyType    Key Type used.
 *
 */
void qorvoRadioSetMacKey(uint8_t        aKeyIdMode,
                         uint8_t        aKeyId,
                         const uint8_t *aPrevKey,
                         const uint8_t *aCurrKey,
                         const uint8_t *aNextKey,
                         uint8_t        aKeyType);

/**
 * This method sets the current MAC frame counter value.
 *
 * This function is used when radio provides `OT_RADIO_CAPS_TRANSMIT_SEC` capability.
 *
 * @param[in]   aMacFrameCounter  The MAC frame counter value.
 *
 */
void qorvoRadioSetMacFrameCounter(uint32_t aMacFrameCounter);

/**
 * Enable or disable CSL receiver.
 *
 * @param[in]  aCslPeriod    CSL period, 0 for disabling CSL.
 * @param[in]  aShortAddr    The short source address of CSL receiver's peer.
 * @param[in]  aExtAddr      The extended source address of CSL receiver's peer.
 *
 * @note Platforms should use CSL peer addresses to include CSL IE when generating enhanced acks.
 *
 * @retval  kErrorNotImplemented Radio driver doesn't support CSL.
 * @retval  kErrorFailed         Other platform specific errors.
 * @retval  kErrorNone           Successfully enabled or disabled CSL.
 *
 */
otError qorvoRadioEnableCsl(uint32_t       aCslPeriod,
                            uint16_t       aShortAddr,
                            const uint8_t *aExtAddr);

/**
 * Update CSL sample time in radio driver.
 *
 * Sample time is stored in radio driver as a copy to calculate phase when sending ACK with CSL IE.
 *
 * @param[in]  aCslSampleTime    The latest sample time.
 *
 */
void qorvoRadioUpdateCslSampleTime(uint32_t aCslSampleTime);

/**
 * Enable/disable or update Enhanced-ACK Based Probing in radio for a specific Initiator.
 *
 * After Enhanced-ACK Based Probing is configured by a specific Probing Initiator, the Enhanced-ACK sent to that
 * node should include Vendor-Specific IE containing Link Metrics data. This method informs the radio to start/stop to
 * collect Link Metrics data and include Vendor-Specific IE that containing the data in Enhanced-ACK sent to that
 * Probing Initiator.
 *
 * @param[in]  aLinkMetrics  This parameter specifies what metrics to query. Per spec 4.11.3.4.4.6, at most 2 metrics
 *                           can be specified. The probing would be disabled if @p `aLinkMetrics` is bitwise 0.
 * @param[in]  aShortAddr    The short address of the Probing Initiator.
 * @param[in]  aExtAddr      The extended source address of the Probing Initiator. @p aExtAddr MUST NOT be `NULL`.
 *
 * @retval  OT_ERROR_NONE            Successfully configured the Enhanced-ACK Based Probing.
 * @retval  OT_ERROR_INVALID_ARGS    @p aExtAddress is `NULL`.
 * @retval  OT_ERROR_NOT_FOUND       The Initiator indicated by @p aShortAddress is not found when trying to clear.
 * @retval  OT_ERROR_NO_BUFS         No more Initiator can be supported.
 *
 */
otError qorvoRadioConfigureEnhAckProbing(otLinkMetrics  aLinkMetrics,
                                         uint16_t       aShortAddress,
                                         const uint8_t *aExtAddress);

/**
 * Get the current estimated time (in microseconds) of the radio chip.
 *
 * This microsecond timer must be a free-running timer. The timer must continue to advance with microsecond precision
 * even when the radio is in the sleep state.
 *
 * @returns The current time in microseconds. UINT64_MAX when platform does not support or radio time is not ready.
 *
 */
uint64_t qorvoRadioGetNow(void);

/**
 * Schedule a radio reception window at a specific time and duration.
 *
 * @param[in]  aChannel   The radio channel on which to receive.
 * @param[in]  aStart     The receive window start time, in microseconds.
 * @param[in]  aDuration  The receive window duration, in microseconds
 *
 * @retval OT_ERROR_NONE    Successfully scheduled receive window.
 * @retval OT_ERROR_FAILED  The receive window could not be scheduled.
 */
otError qorvoRadioReceiveAt(uint8_t aChannel, uint32_t aStart, uint32_t aDuration);

/**
 * Retrieve the radio capabilities of the radio
 *
 * @returns    caps       The radio capabilities as described in include/openthread/platform/radio.h
 */
otRadioCaps qorvoRadioGetCaps(void);

/**
 * This callback is called when the energy scan is finished.
 *
 * @param[in]  aEnergyScanMaxRssi  The amount of energy detected during the ED scan.
 *
 */
void cbQorvoRadioEnergyScanDone(int8_t aEnergyScanMaxRssi);

/**
 * This callback is called after a transmission is completed (and if required an ACK is received).
 *
 * @param[in]  aFrame         The packet which was transmitted.
 * @param[in]  aFramePending  Indicates if the FP bit was set in the ACK frame or not.
 * @param[in]  aError         Indicates if an error occurred during transmission.
 *
 */
void cbQorvoRadioTransmitDone(otRadioFrame *aFrame, bool aFramePending, otError aError);

/**
 * This callback is called after a transmission is completed (and if required an ACK is received).
 *
 * @param[in]  aFrame         The packet which was transmitted.
 * @param[in]  aAckFrame      The complete 802.15.4-2015 enhanced ack frame.
 * @param[in]  aError         Indicates if an error occurred during transmission.
 *
 */
void cbQorvoRadioTransmitDone_AckFrame(otRadioFrame *aFrame, otRadioFrame *aAckFrame, otError aError);

/**
 * This callback is called after a frame is received.
 *
 * @param[in]  aFrame   The packet which was received.
 * @param[in]  aError   Any error which occurred during reception of the packet.
 *
 */
void cbQorvoRadioReceiveDone(otRadioFrame *aFrame, otError aError);

/**
 * This callback is called to notify OpenThread that the transmission has started.
 *
 * @note  This function should be called by the same thread that executes all of the other OpenThread code. It should
 *        not be called by ISR or any other task.
 *
 * @param[in]  aFrame     A pointer to the frame that is being transmitted.
 *
 */
void cbQorvoRadioTxStarted(otRadioFrame *aFrame);

#endif // _RADIO_QORVO_H_
