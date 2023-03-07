/***************************************************************************//**
 * @file
 * @brief RAILtest transmit and receive events
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rail.h"
#include "rail_types.h"

#include "buffer_pool_allocator.h"
#include "circular_queue.h"

#include "app_common.h"
#include "app_trx.h"
#include "rail_ble.h"
#include "rail_ieee802154.h"
#include "response_print.h"
/******************************************************************************
 * Variables
 *****************************************************************************/
RAIL_TxPacketDetails_t previousTxAppendedInfo = { .isAck = false, };
RAIL_TxPacketDetails_t previousTxAckAppendedInfo = { .isAck = true, };

static uint16_t dataLeft = 0;
static uint8_t *dataLeftPtr = NULL;

// Variables to keep track of the receive packet
static uint16_t rxLengthTarget;
static uint16_t rxLengthCount;
static void *rxFifoPacketHandle = 0;
static RailAppEvent_t *rxFifoPacketData;
static uint8_t *currentRxFifoPacketPtr;

uint32_t abortRxDelay = 0;

// Used for wakeup from sleep
volatile bool rxPacketEvent = false;

// fifo mode test variables
/*
 * This test bit is used to prevent RAILtest from automatically loading packet
 * data into transmit fifo
 */
bool txFifoManual = false;
/*
 * This test bit is used to prevent RAILtest from automatically reading out the
 * packet from the fifo
 */
bool rxFifoManual = false;

// To enable power manager sleep from the main loop.
extern volatile bool allowPowerManagerSleep;

// Used to track the FEM protection configuration
#ifdef SL_RAIL_UTIL_EFF_DEVICE
RAIL_FemProtectionConfig_t femConfig = {
  .PMaxContinuousTx = RAIL_UTIL_EFF_MAX_TX_CONTINUOUS_POWER_DBM,
  .txDutyCycle = RAIL_UTIL_EFF_MAX_TX_DUTY_CYCLE
};
#endif

/******************************************************************************
 * Static
 *****************************************************************************/
static void packetMode_RxPacketAborted(RAIL_Handle_t railHandle)
{
  if (!printRxErrorPackets) {
    return;
  }
  RAIL_RxPacketInfo_t packetInfo;
  RAIL_RxPacketHandle_t packetHandle
    = RAIL_GetRxPacketInfo(railHandle, RAIL_RX_PACKET_HANDLE_NEWEST,
                           &packetInfo);
  // assert(packetHandle != NULL);
  // assert(packetInfo.packetBytes == 0);
  // Get a memory buffer for the received packet details
  void *rxPacketMemoryHandle = memoryAllocate(sizeof(RailAppEvent_t));
  RailAppEvent_t *rxPacket = (RailAppEvent_t *)memoryPtrFromHandle(rxPacketMemoryHandle);
  if (rxPacket != NULL) {
    rxPacket->type = RX_PACKET;
    rxPacket->rxPacket.dataPtr = NULL;
    rxPacket->rxPacket.packetStatus = packetInfo.packetStatus;
    rxPacket->rxPacket.dataLength = 0U;
    rxPacket->rxPacket.freqOffset = getRxFreqOffset();
    rxPacket->rxPacket.filterMask = packetInfo.filterMask;
    // Read what packet details are available into our packet structure
    if (RAIL_GetRxPacketDetailsAlt(railHandle, packetHandle,
                                   &rxPacket->rxPacket.appendedInfo)
        != RAIL_STATUS_NO_ERROR) {
      // assert(false);
      memset(&rxPacket->rxPacket.appendedInfo, 0, sizeof(rxPacket->rxPacket.appendedInfo));
    }
    if (logLevel & ASYNC_RESPONSE) {
      // Take an extra reference to this rx packet pointer so it's not released
      memoryTakeReference(rxPacketMemoryHandle);
      // Copy this received packet into our circular queue
      queueAdd(&railAppEventQueue, rxPacketMemoryHandle);
    }
    // Do not include Rx Error packets in PER's RSSI stats:
    // updateStats(rxPacket->appendedInfo.rssi, &counters.rssi);
  } else {
    counters.noRxBuffer++;
    eventsMissed++;
  }
  // Free the allocated memory now that we're done with it
  memoryFree(rxPacketMemoryHandle);
}

static void packetMode_RxPacketReceived(RAIL_Handle_t railHandle)
{
  if ((rxSuccessTransition == RAIL_RF_STATE_TX)
      || (RAIL_IsAutoAckEnabled(railHandle) && afterRxUseTxBufferForAck)) {
    // Load packet for either the non-AutoACK RXSUCCESS => TX transition,
    // or for the ACK transition when we intend to use the TX buffer
    // We don't do this in other circumstances in case of an RX2TX
    // transition to send a packet that's already been loaded,
    // which could cause TX underflow if we were to disturb it.
    RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
  }

  if (rxHeld) {
    (void)RAIL_HoldRxPacket(railHandle);
    packetsHeld++;
  } else {
    (void)processRxPacket(railHandle, RAIL_RX_PACKET_HANDLE_NEWEST);
  }
}

RAIL_RxPacketHandle_t processRxPacket(RAIL_Handle_t railHandle,
                                      RAIL_RxPacketHandle_t packetHandle)
{
  RAIL_Status_t status;
  RAIL_RxPacketDetails_t details;
  RAIL_RxPacketInfo_t packetInfo;
  packetHandle = RAIL_GetRxPacketInfo(railHandle, packetHandle, &packetInfo);
  if (packetHandle == RAIL_RX_PACKET_HANDLE_INVALID) {
    return packetHandle;
  }
  if (packetInfo.packetStatus == RAIL_RX_PACKET_READY_CRC_ERROR) {
    counters.receiveCrcErrDrop++; // counters.receive still counts such too
  }
  uint16_t length = packetInfo.packetBytes;
  void *rxPacketMemoryHandle = memoryAllocate(sizeof(RailAppEvent_t) + length);
  RailAppEvent_t *rxPacket = (RailAppEvent_t *)memoryPtrFromHandle(rxPacketMemoryHandle);
  uint8_t *rxPacketData = (uint8_t *)&rxPacket[1];

  // Read the appended info into our packet structure
  status = RAIL_GetRxPacketDetailsAlt(railHandle, packetHandle, &details);
  if (status == RAIL_STATUS_NO_ERROR) {
    if (details.subPhyId < RAIL_BLE_RX_SUBPHY_COUNT) {
      counters.subPhyCount[details.subPhyId]++;
    }
  }
  // Count packets that we received but had no memory to store
  if (rxPacket == NULL) {
    counters.noRxBuffer++;
  } else {
    rxPacket->type = RX_PACKET;
    rxPacket->rxPacket.dataPtr = rxPacketData;
    rxPacket->rxPacket.packetStatus = packetInfo.packetStatus;
    // Read packet data into our packet structure
    RAIL_CopyRxPacket(rxPacketData, &packetInfo);
    rxPacket->rxPacket.filterMask = packetInfo.filterMask;
    rxPacket->rxPacket.dataLength = length;
    rxPacket->rxPacket.freqOffset = getRxFreqOffset();
    rxPacket->rxPacket.appendedInfo = details;

    if (status == RAIL_STATUS_NO_ERROR) {
      // Note that this does not take into account CRC bytes unless
      // RAIL_RX_OPTION_STORE_CRC is used
      rxPacket->rxPacket.appendedInfo.timeReceived.totalPacketBytes = length;
      (void) (*rxTimePosition)(railHandle, &rxPacket->rxPacket.appendedInfo);
    }
    if (status != RAIL_STATUS_NO_ERROR) {
      memset(&rxPacket->rxPacket.appendedInfo, 0, sizeof(rxPacket->rxPacket.appendedInfo));
    }

    // If we have just received an ACK, don't respond with an ACK
    if (rxPacketData[2] == 0xF1) {
      RAIL_CancelAutoAck(railHandle);
    }

    // Cancel ack if user requested
    if (afterRxCancelAck) {
      afterRxCancelAck = false;
      RAIL_CancelAutoAck(railHandle);
    }

    // Use Tx Buffer for Ack if user requested
    if (afterRxUseTxBufferForAck) {
      afterRxUseTxBufferForAck = false;
      RAIL_UseTxFifoForAutoAck(railHandle);
    }

    if (currentAppMode() == SCHTX_AFTER_RX) {
      // Schedule the next transmit after this receive
      RAIL_ScheduleTxConfig_t scheduledTxOptions = {
        //TODO: packetTime depends on rxTimePosition;
        //      this code assumes default position (SYNC_END).
        .when = rxPacket->rxPacket.appendedInfo.timeReceived.packetTime
                + txAfterRxDelay,
        .mode = RAIL_TIME_ABSOLUTE,
        .txDuringRx = RAIL_SCHEDULED_TX_DURING_RX_POSTPONE_TX
      };

      setNextPacketTime(&scheduledTxOptions);

      txCount = 1;
      pendPacketTx();
    }

    if (phySwitchToRx.enable) {
      // Be careful when setting timeDelta, because it's the delta from
      // whatever timestamp position is currently configured
      uint32_t timestamp = rxPacket->rxPacket.appendedInfo.timeReceived.packetTime;
      if (phySwitchToRx.extraDelayUs > 0) {
        RAIL_DelayUs(phySwitchToRx.extraDelayUs);
      }
      (void) RAIL_BLE_PhySwitchToRx(railHandle,
                                    phySwitchToRx.phy,
                                    phySwitchToRx.physicalChannel,
                                    phySwitchToRx.timeDelta + timestamp,
                                    phySwitchToRx.crcInit,
                                    phySwitchToRx.accessAddress,
                                    phySwitchToRx.logicalChannel,
                                    phySwitchToRx.disableWhitening);
      phySwitchToRx.enable = false;
    }

    if (logLevel & ASYNC_RESPONSE) {
      updateGraphics();

      // Take an extra reference to this rx packet pointer so it's not released
      memoryTakeReference(rxPacketMemoryHandle);

      // Copy this received packet into our circular queue
      queueAdd(&railAppEventQueue, rxPacketMemoryHandle);
    }

    updateStats(rxPacket->rxPacket.appendedInfo.rssi, &counters.rssi);
  }

  // Track the state of scheduled Rx to figure out when it ends
  if (inAppMode(RX_SCHEDULED, NULL) && schRxStopOnRxEvent) {
    enableAppMode(RX_SCHEDULED, false, NULL);
  }

  // In Rx overflow test mode hang in this ISR to prevent processing new
  // packets to force an overflow
  if ((currentAppMode() == RX_OVERFLOW)) {
    enableAppMode(RX_OVERFLOW, false, NULL); // Switch back after the overflow
    changeAppModeIfPending();
    // Trigger an overflow by waiting in the interrupt handler
    usDelay(rxOverflowDelay);
  }

  // Free the allocated memory now that we're done with it
  memoryFree(rxPacketMemoryHandle);
  return packetHandle;
}

// Only support fixed length
static void fifoMode_RxPacketReceived(void)
{
  uint16_t bytesRead;
  // Discard incoming data stream in BER mode.
  if (currentAppMode() == BER) {
    RAIL_ResetFifo(railHandle, true, true);
  } else {
    // Note the packet's status
    RAIL_RxPacketInfo_t packetInfo;
    RAIL_RxPacketHandle_t packetHandle
      = RAIL_GetRxPacketInfo(railHandle, RAIL_RX_PACKET_HANDLE_OLDEST,
                             &packetInfo);
    // assert(packetHandle != NULL);
    if (packetInfo.packetStatus == RAIL_RX_PACKET_READY_CRC_ERROR) {
      counters.receiveCrcErrDrop++; // counters.receive still counts such too
    }
    if (printRxErrorPackets
        || (packetInfo.packetStatus == RAIL_RX_PACKET_READY_CRC_ERROR)
        || (packetInfo.packetStatus == RAIL_RX_PACKET_READY_SUCCESS)) {
      // Read the rest of the bytes out of the fifo
      bytesRead = RAIL_ReadRxFifo(railHandle, currentRxFifoPacketPtr, rxLengthCount);
      rxLengthCount -= bytesRead;
      if (currentRxFifoPacketPtr != NULL) {
        currentRxFifoPacketPtr += bytesRead;
      }

      if (rxFifoPacketData != NULL) {
        // Keep and display this frame
        rxFifoPacketData->rxPacket.packetStatus = packetInfo.packetStatus;
        rxFifoPacketData->rxPacket.filterMask = packetInfo.filterMask;

        // Configure how many bytes were received
        rxFifoPacketData->rxPacket.dataLength = rxLengthTarget;

        // Get the appended info details
        if (RAIL_GetRxPacketDetailsAlt(railHandle, packetHandle,
                                       &rxFifoPacketData->rxPacket.appendedInfo)
            != RAIL_STATUS_NO_ERROR) {
          // assert(false);
          memset(&rxFifoPacketData->rxPacket.appendedInfo, 0, sizeof(rxFifoPacketData->rxPacket.appendedInfo));
        } else {
          if (rxFifoPacketData->rxPacket.appendedInfo.subPhyId < RAIL_BLE_RX_SUBPHY_COUNT) {
            counters.subPhyCount[rxFifoPacketData->rxPacket.appendedInfo.subPhyId]++;
          }
        }
        // Note that this does not take into account CRC bytes unless
        // RAIL_RX_OPTION_STORE_CRC is used
        rxFifoPacketData->rxPacket.appendedInfo.timeReceived.totalPacketBytes = rxLengthTarget;
        RAIL_GetRxTimeSyncWordEndAlt(railHandle, &rxFifoPacketData->rxPacket.appendedInfo);
        queueAdd(&railAppEventQueue, rxFifoPacketHandle);
      }
    } else {
      // Toss this frame and any of its data accumlated so far
      memoryFree(rxFifoPacketHandle);
    }
  }
}

/**
 * In Fifo mode, prepare for the upcoming rx
 *
 * Grab a buffer to store rx data
 * Keep track of writing data to this buffer
 */
void rxFifoPrep(void)
{
  // Don't allocate memory to save incoming data in BER mode
  // or non-packet-data sources
  if ((railDataConfig.rxMethod == FIFO_MODE)
      && (railDataConfig.rxSource == RX_PACKET_DATA)
      && (currentAppMode() != BER)
      && !rxFifoManual) {
    rxLengthCount = rxLengthTarget;
    rxFifoPacketHandle = memoryAllocate(sizeof(RailAppEvent_t) + rxLengthTarget);
    rxFifoPacketData = (RailAppEvent_t *)memoryPtrFromHandle(rxFifoPacketHandle);
    if (rxFifoPacketData == NULL) {
      currentRxFifoPacketPtr = NULL;
      counters.noRxBuffer++;
    } else {
      uint8_t *rxPacketData = (uint8_t *)&rxFifoPacketData[1];
      rxFifoPacketData->type = RX_PACKET;
      rxFifoPacketData->rxPacket.dataPtr = rxPacketData;
      rxFifoPacketData->rxPacket.freqOffset = getRxFreqOffset();
      rxFifoPacketData->rxPacket.filterMask = 0U;
      currentRxFifoPacketPtr = rxPacketData;
    }
  }
}

/******************************************************************************
 * Public
 *****************************************************************************/
void loadTxData(uint8_t *data, uint16_t dataLen)
{
  uint16_t dataWritten;

  if (railDataConfig.txMethod == PACKET_MODE) {
    RAIL_WriteTxFifo(railHandle, data, dataLen, true);
  } else {
    dataWritten = RAIL_WriteTxFifo(railHandle, data, dataLen, false);
    dataLeft = dataLen - dataWritten;
    dataLeftPtr = data + dataWritten;
  }
}

void configRxLengthSetting(uint16_t rxLength)
{
  if (railDataConfig.rxMethod == FIFO_MODE) {
    rxLengthTarget = rxLength;
  }
}

/******************************************************************************
 * RAIL Callback Implementation
 *****************************************************************************/
void RAILCb_TxPacketSent(RAIL_Handle_t railHandle, bool isAck)
{
#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
  if (modeSwitchState == TX_MS_PACKET || modeSwitchState == TX_ON_NEW_PHY) { // Packet has been sent in a MS context
    if (txCountAfterModeSwitchId == 0) { // Sent packet was MS packet
      if (RAIL_IsValidChannel(railHandle, modeSwitchNewChannel)
          == RAIL_STATUS_NO_ERROR) {
        changeChannel(modeSwitchNewChannel);
        modeSwitchState = TX_ON_NEW_PHY;
      }

      // Restore first 2 bytes overwritten by Mode Switch PHR
      txData[0] = txData_2B[0];
      txData[1] = txData_2B[1];

      if (txCountAfterModeSwitch != 0) {
        // Send next packets asap
        txCount = txCountAfterModeSwitch;
        pendPacketTx();
        sendPacketIfPending(); // txCount is decremented in this function
        txCountAfterModeSwitchId++;
      } else {
        endModeSwitchSequence();
      }
    } else {
      if (txCountAfterModeSwitchId < txCountAfterModeSwitch) { // Sent packet was not the last data packet to be tx
        txCountAfterModeSwitchId++;
        internalTransmitCounter++;
        // previousTxAppendedInfo.isAck already initialized false
        previousTxAppendedInfo.timeSent.totalPacketBytes = txDataLen;
        (void) RAIL_GetTxPacketDetailsAlt2(railHandle, &previousTxAppendedInfo);
        (void) (*txTimePosition)(railHandle, &previousTxAppendedInfo);
        scheduleNextTx();
      } else {
        modeSwitchSequenceId++;
        if (modeSwitchSequenceId < modeSwitchSequenceIterations) {
          txCountAfterModeSwitchId = 0;
          // Start timer if needed
          if (modeSwitchDelayUs > 0) {
            RAIL_SetMultiTimer(&modeSwitchMultiTimer,
                               modeSwitchDelayUs,
                               RAIL_TIME_DELAY,
                               &RAILCb_ModeSwitchMultiTimerExpired,
                               NULL);
          } else {
            restartModeSwitchSequence();
          }
        } else {
          endModeSwitchSequence();
          internalTransmitCounter++;
          // previousTxAppendedInfo.isAck already initialized false
          previousTxAppendedInfo.timeSent.totalPacketBytes = txDataLen;
          (void) RAIL_GetTxPacketDetailsAlt2(railHandle, &previousTxAppendedInfo);
          (void) (*txTimePosition)(railHandle, &previousTxAppendedInfo);
          scheduleNextTx();
        }
      }
    }
  } else
#endif
  {
    // Store the previous tx time for printing later
    if (isAck) {
      sentAckPackets++;
      // previousTxAckAppendedInfo.isAck already initialized true
      previousTxAckAppendedInfo.timeSent.totalPacketBytes
        = RAIL_IEEE802154_IsEnabled(railHandle) ? 4U : ackDataLen;
      (void) RAIL_GetTxPacketDetailsAlt2(railHandle, &previousTxAckAppendedInfo);
      (void) (*txTimePosition)(railHandle, &previousTxAckAppendedInfo);
      pendFinishTxAckSequence();
    } else {
      internalTransmitCounter++;
      // previousTxAppendedInfo.isAck already initialized false
      previousTxAppendedInfo.timeSent.totalPacketBytes = txDataLen;
      (void) RAIL_GetTxPacketDetailsAlt2(railHandle, &previousTxAppendedInfo);
      (void) (*txTimePosition)(railHandle, &previousTxAppendedInfo);
      scheduleNextTx();
    }
  }

  // Move visualization update in order not to delay mode switching
  LedToggle(1);
  updateGraphics();
}

void RAILCb_RxPacketAborted(RAIL_Handle_t railHandle)
{
  if (railDataConfig.rxMethod == PACKET_MODE) {
    packetMode_RxPacketAborted(railHandle);
  } else if (!rxFifoManual) {
    fifoMode_RxPacketReceived();
  }
}

void RAILCb_RxPacketReceived(RAIL_Handle_t railHandle)
{
  counters.receive++;
  LedToggle(0);
  rxPacketEvent = true;

  if (railDataConfig.rxMethod == PACKET_MODE) {
    packetMode_RxPacketReceived(railHandle);
  } else if (!rxFifoManual) {
    fifoMode_RxPacketReceived();
  }
}

void RAILCb_RxChannelHoppingComplete(RAIL_Handle_t railHandle)
{
  RAIL_Time_t periodicWakeupUs = 0U;

  // Only schedule the next wakeup if:
  // 1. RX Duty Cycle Schedule Wakeup is enabled
  // 2. Sleep duration > 0
  // 3. Sleep has not been interrupted via a serial wakeup.
  if ((getRxDutyCycleSchedWakeupEnable(&periodicWakeupUs))
      && (periodicWakeupUs > 0U) && (!serEvent)) {
    // Configure the next scheduled RX
    RAIL_ScheduleRxConfig_t rxCfg = {
      .start = periodicWakeupUs,
      .startMode = RAIL_TIME_DELAY,
      .end = 0U,
      .endMode = RAIL_TIME_DISABLED,
      .rxTransitionEndSchedule = 0U,
      .hardWindowEnd = 0U
    };

    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
    RAIL_ScheduleRx(railHandle, channel, &rxCfg, NULL);
    allowPowerManagerSleep = true;
  }
}

void RAILCb_TxFifoAlmostEmpty(RAIL_Handle_t railHandle)
{
  uint16_t dataWritten;
  counters.txFifoAlmostEmpty++;

  if (dataLeft) {
    dataWritten = RAIL_WriteTxFifo(railHandle, dataLeftPtr, dataLeft, false);
    dataLeft -= dataWritten;
    dataLeftPtr += dataWritten;
  }
}

// count number of 1s in a byte without a loop
static uint8_t countBits(uint8_t num)
{
  uint8_t count = 0;
  static const uint8_t nibblebits[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
  count += nibblebits[num & 0x0F];
  count += nibblebits[num >> 4];
  return count;
}

static void berSource_RxFifoAlmostFull(uint16_t bytesAvailable)
{
  (void)bytesAvailable;
  // All incoming bytes are received and validated here.
  uint16_t numBytes;
  bool stopBerRx = false;

  // If rxOfEvent is > 0, then we're overflowing the incoming RX buffer
  // probably because the BER test isn't processing incoming bits fast
  // enough. The test will automatically try to re-synchronize and read in bits
  // from the stream, but the bits under test will not be continuous. Abort
  // testing and notify the user if this is the case.
  if (counters.rxOfEvent) {
    stopBerRx = true;
  }

  while ((RAIL_GetRxFifoBytesAvailable(railHandle) > RAIL_GetRxFifoThreshold(railHandle))
         && !stopBerRx) {
    // Read multiple bytes in if they're available.
    // Reuse the txData[SL_RAIL_TEST_MAX_PACKET_LENGTH] array since we won't be
    // transmitting in BER Test mode anyway.
    numBytes = RAIL_ReadRxFifo(railHandle, txData, SL_RAIL_TEST_MAX_PACKET_LENGTH);

    for (uint16_t x = 0; x < numBytes && !stopBerRx; x++) {
      // Update BER statistics
      if (berStats.bytesTested < berStats.bytesTotal) {
        // Counters will not overflow since bytesTotal max value is capped.
        berStats.bitErrors += countBits(txData[x]);
        berStats.bytesTested++;
      } else {
        stopBerRx = true; // statistics are all gathered - stop now
      }
    }
  }
  // disregard decimal point
  berStats.rssi = (int8_t)(RAIL_GetRssiAlt(railHandle, RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT) / 4);

  // stop RXing when enough bits are acquired or an error (i.e. RX overflow)
  if (stopBerRx) {
    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN, true);
    RAIL_ResetFifo(railHandle, true, true);
    berTestModeEnabled = false;
  }
}

// @TODO It would be better if we could 'register' callback contents
static void packetSource_RxFifoAlmostFull(uint16_t bytesAvailable)
{
  uint16_t bytesRead;
  if (rxLengthCount > 0) {
    // Amount to read is either bytes avilable or number of bytes remaining in packet
    bytesRead = (rxLengthCount > bytesAvailable) ? bytesAvailable : rxLengthCount;
    bytesRead = RAIL_ReadRxFifo(railHandle, currentRxFifoPacketPtr, bytesRead);
    rxLengthCount -= bytesRead;
    if (currentRxFifoPacketPtr != NULL) {
      currentRxFifoPacketPtr += bytesRead;
    }
  }
}

void RAILCb_RxFifoAlmostFull(RAIL_Handle_t railHandle)
{
  uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);

  if (berTestModeEnabled) {
    berSource_RxFifoAlmostFull(bytesAvailable);
  } else if (RAIL_BLE_IsEnabled(railHandle)) {
    RAIL_ConfigEvents(railHandle, RAIL_EVENT_RX_FIFO_ALMOST_FULL,
                      RAIL_EVENTS_NONE); // Disable this event
  } else if (railDataConfig.rxSource != RX_PACKET_DATA) {
    // Just consume (flush) the data
    counters.rxRawSourceBytes += RAIL_ReadRxFifo(railHandle, NULL, bytesAvailable);
    rxDataSourceEventState++;
    if (rxDataSourceEventState >= RX_DATA_SOURCE_EVENT_STATE_SUSPENDED) {
      // Temporarily disable RX_FIFO events to let main-line code run
      // in case these events are so frequent nothing gets done.
      RAIL_ConfigEvents(railHandle, RAIL_EVENT_RX_FIFO_ALMOST_FULL,
                        RAIL_EVENTS_NONE);
      // Separate calls each of which uses fast-path code in library
      RAIL_ConfigEvents(railHandle, (RAIL_EVENT_RX_FIFO_OVERFLOW
                                     | RAIL_EVENT_RX_FIFO_FULL),
                        RAIL_EVENTS_NONE);
    }
  } else {
    packetSource_RxFifoAlmostFull(bytesAvailable);
  }
}

void setNextPacketTime(RAIL_ScheduleTxConfig_t *scheduledTxOptions)
{
  memcpy(&nextPacketTxTime, scheduledTxOptions, sizeof(RAIL_ScheduleTxConfig_t));
}
