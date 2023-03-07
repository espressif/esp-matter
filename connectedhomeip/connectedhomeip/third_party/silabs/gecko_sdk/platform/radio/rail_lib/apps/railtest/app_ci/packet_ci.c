/***************************************************************************//**
 * @file
 * @brief This file implements the packet commands for RAILtest applications.
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
#include <string.h>

#include "response_print.h"
#include "circular_queue.h"
#include "buffer_pool_allocator.h"

#include "rail.h"
#include "rail_types.h"
#include "app_common.h"
#include "app_trx.h"

void printTxPacket(sl_cli_command_arg_t *args)
{
  // Use the packet print helper to print out the transmit payload
  printPacket(sl_cli_get_command_string(args, 0),
              txData,
              txDataLen,
              NULL);
}

static bool setTxPayloadHelper(sl_cli_command_arg_t *args)
{
  uint16_t offset = sl_cli_get_argument_uint16(args, 0);

  // Read as many bytes as have been supplied and set them
  for (int i = 2; i < sl_cli_get_argument_count(args) + 1; i++) {
    uint32_t index = offset + i - 2;
    uint8_t value = sl_cli_get_argument_uint8(args, i - 1);

    // Make sure this fits in the txData buffer
    if (index >= sizeof(txData)) {
      responsePrintError(sl_cli_get_command_string(args, 0), 5, "Data overflows txData buffer");
      return false;
    }
    txData[index] = value;
  }

  if (railDataConfig.txMethod == PACKET_MODE) {
    RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
  }
  return true;
}

void setTxPayload(sl_cli_command_arg_t *args)
{
  if (setTxPayloadHelper(args)) {
    args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
    printTxPacket(args);
  }
}

void setTxPayloadQuiet(sl_cli_command_arg_t *args)
{
  if (setTxPayloadHelper(args)) {
    uint16_t start = sl_cli_get_argument_uint16(args, 0);
    // Ignore first two arguments in end (command name, start index)
    responsePrint(sl_cli_get_command_string(args, 0), "start:%u,end:%u", start, start + sl_cli_get_argument_count(args) - 1);
  }
}

void setRfSenseTxPayload(sl_cli_command_arg_t *args)
{
  RAIL_Status_t status = RAIL_STATUS_INVALID_CALL;
  if (sl_cli_get_argument_count(args) >= 2) {
    uint8_t syncwordNumBytes = sl_cli_get_argument_uint8(args, 0);
    uint32_t syncword = sl_cli_get_argument_uint32(args, 1);
    if (railDataConfig.txMethod == PACKET_MODE) {
      status = RAIL_SetRfSenseSelectiveOokWakeupPayload(railHandle,
                                                        syncwordNumBytes,
                                                        syncword);
    }
    responsePrint(sl_cli_get_command_string(args, 0), "RFSense Payload:%s",
                  (status == RAIL_STATUS_NO_ERROR) ? "Set" : "Not Set");
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x5, "Invalid inputs");
  }
}

void setTxLength(sl_cli_command_arg_t *args)
{
  uint32_t length = sl_cli_get_argument_uint32(args, 0);

  if (length > sizeof(txData)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 6, "Invalid length %d", length);
    return;
  }

  txDataLen = length;
  if (railDataConfig.txMethod == PACKET_MODE) {
    length = RAIL_WriteTxFifo(railHandle, txData, txDataLen, true);
  }
  responsePrint(sl_cli_get_command_string(args, 0), "TxLength:%d,TxLength Written:%d", txDataLen, length);
}

void printAckPacket(sl_cli_command_arg_t *args)
{
  // Use the packet print helper to print out the transmit payload
  printPacket(sl_cli_get_command_string(args, 0),
              ackData,
              ackDataLen,
              NULL);
}

void setAckPayload(sl_cli_command_arg_t *args)
{
  uint16_t offset = sl_cli_get_argument_uint16(args, 0);

  // Read as many bytes as have been supplied and set them
  for (int i = 2; i < sl_cli_get_argument_count(args) + 1; i++) {
    uint32_t index = offset + i - 2;
    uint8_t value = sl_cli_get_argument_uint8(args, i - 1);

    // Make sure this fits in the txData buffer
    if (index >= sizeof(ackData)) {
      responsePrintError(sl_cli_get_command_string(args, 0), 5, "Data overflows ackData buffer");
      return;
    }
    ackData[index] = value;
  }

  RAIL_WriteAutoAckFifo(railHandle, ackData, ackDataLen);
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  printAckPacket(args);
}

void setAckLength(sl_cli_command_arg_t *args)
{
  uint32_t length = sl_cli_get_argument_uint32(args, 0);

  if (length > sizeof(ackData)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 6, "Invalid length %d", length);
    return;
  }

  // Make sure we're using the txData array and set the length
  ackDataLen = length;
  RAIL_WriteAutoAckFifo(railHandle, ackData, ackDataLen);
  responsePrint(sl_cli_get_command_string(args, 0), "TxLength:%d", ackDataLen);
}

void setFixedLength(sl_cli_command_arg_t *args)
{
  if ((!inAppMode(NONE, sl_cli_get_command_string(args, 0)))
      || (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0)))) {
    return;
  }
  uint16_t fixedLength = sl_cli_get_argument_uint16(args, 0);
  fixedLength = RAIL_SetFixedLength(railHandle, fixedLength);
  configRxLengthSetting(fixedLength);

  // Print configured length
  responsePrint(sl_cli_get_command_string(args, 0), "FixedLength:%d", fixedLength);
}

void dataConfig(sl_cli_command_arg_t *args)
{
  if ((!inAppMode(NONE, sl_cli_get_command_string(args, 0)))
      || (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0)))) {
    return;
  }

  RAIL_DataConfig_t newConfig = railDataConfig; // local struct copy

  if (memcmp(sl_cli_get_argument_string(args, 0), "pkt", 3) == 0) {
    // Packet mode
    newConfig.txMethod = PACKET_MODE;
  } else if (memcmp(sl_cli_get_argument_string(args, 0), "fifo", 4) == 0) {
    newConfig.txMethod = FIFO_MODE;
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x50, "Invalid Data Method selection.");
  }

  if (memcmp(sl_cli_get_argument_string(args, 1), "pkt", 3) == 0) {
    // Packet mode
    newConfig.rxMethod = PACKET_MODE;
  } else if (memcmp(sl_cli_get_argument_string(args, 1), "fifo", 4) == 0) {
    newConfig.rxMethod = FIFO_MODE;
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x50, "Invalid Data Method selection.");
  }

  newConfig.rxSource = RX_PACKET_DATA; // Default
  if (sl_cli_get_argument_count(args) >= 3) {
    newConfig.rxSource = (RAIL_RxDataSource_t)sl_cli_get_argument_uint8(args, 2);
  }

#if RAIL_SUPPORTS_MFM
  newConfig.txSource = TX_PACKET_DATA; // Default
  if (sl_cli_get_argument_count(args) >= 4) {
    newConfig.txSource = (RAIL_TxDataSource_t)sl_cli_get_argument_uint8(args, 3);
  }
#else
  // newConfig.txSource is unchanged
#endif

  RAIL_Status_t status = RAIL_ConfigData(railHandle, &newConfig);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x50, "Failed to successfully call RAIL_DataConfig: %d", status);
    return;
  }

  // Reset RX FIFO and rxRawSourceBytes counter for a new rxSource
  if (railDataConfig.rxSource != newConfig.rxSource) {
    counters.rxRawSourceBytes = 0;
    RAIL_ResetFifo(railHandle, false, true);
  }

  railDataConfig = newConfig; // global struct copy

  responsePrint(sl_cli_get_command_string(args, 0),
                "TxMethod:%s,"
                "RxMethod:%s,"
                "RxSource:%u",
                newConfig.txMethod == PACKET_MODE ? "pkt" : "fifo",
                newConfig.rxMethod == PACKET_MODE ? "pkt" : "fifo",
                newConfig.rxSource);
}

static union {
  // Used to align this buffer as needed
  RAIL_FIFO_ALIGNMENT_TYPE align[SL_RAIL_TEST_RX_BUFFER_SIZE / RAIL_FIFO_ALIGNMENT];
  uint8_t fifo[SL_RAIL_TEST_RX_BUFFER_SIZE];
} rxFifo;

void setRxFifo(sl_cli_command_arg_t *args)
{
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  uint16_t rxFifoSize = sl_cli_get_argument_uint16(args, 0);
  if (rxFifoSize > SL_RAIL_TEST_RX_BUFFER_SIZE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x54, "Max RX FIFO size in this build is %u bytes",
                       SL_RAIL_TEST_RX_BUFFER_SIZE);
    return;
  }
  RAIL_Status_t status = RAIL_SetRxFifo(railHandle, rxFifo.fifo, &rxFifoSize);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x55, "Setting RX FIFO failed; Min RX FIFO size is 64 bytes");
    return;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "RxFifoSize:%u", rxFifoSize);
}

RAIL_Status_t RAILCb_SetupRxFifo(RAIL_Handle_t railHandle)
{
  uint16_t rxFifoSize = SL_RAIL_TEST_RX_BUFFER_SIZE;
  RAIL_Status_t status = RAIL_SetRxFifo(railHandle, rxFifo.fifo, &rxFifoSize);
  if (status == RAIL_STATUS_INVALID_STATE) {
    // Allow failures due to multiprotocol
    return RAIL_STATUS_NO_ERROR;
  }
  return status;
}

void fifoModeTestOptions(sl_cli_command_arg_t *args)
{
  char *outputStr[] = { "Disabled", "Enabled" };
  txFifoManual = !!sl_cli_get_argument_uint8(args, 0);
  rxFifoManual = !!sl_cli_get_argument_uint8(args, 1);

  responsePrint(sl_cli_get_command_string(args, 0),
                "TxFifoManual:%s,"
                "RxFifoManual:%s",
                outputStr[txFifoManual],
                outputStr[rxFifoManual]);
}

/**
 * Specify how many bytes to read out of the fifo and print to terminal
 */
void rxFifoManualRead(sl_cli_command_arg_t *args)
{
  if (!rxFifoManual) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x51, "Must be in rx fifo manual mode (fifoModeTestOptions).");
  } else {
    bool readAppendedInfo = !!sl_cli_get_argument_uint8(args, 0);
    uint16_t bytesToRead = sl_cli_get_argument_uint16(args, 1);
    bool printTimingInfo = (sl_cli_get_argument_count(args) >= 3) ? !!sl_cli_get_argument_uint8(args, 2) : false;
    void *rxPacketHandle = memoryAllocate(bytesToRead + sizeof(RailAppEvent_t));
    RailAppEvent_t *packetData = (RailAppEvent_t *)memoryPtrFromHandle(rxPacketHandle);

    if (packetData == NULL) {
      RAIL_ReleaseRxPacket(railHandle, RAIL_RX_PACKET_HANDLE_OLDEST);
      memoryFree(rxPacketHandle);
      return;
    }
    RAIL_RxPacketDetails_t *appendedInfo = &packetData->rxPacket.appendedInfo;

    // dataLength is number of bytes read from the fifo
    packetData->type = RX_PACKET;
    packetData->rxPacket.dataPtr = (uint8_t *)&packetData[1];
    packetData->rxPacket.dataLength = RAIL_ReadRxFifo(railHandle, packetData->rxPacket.dataPtr,
                                                      bytesToRead);
    packetData->rxPacket.freqOffset = getRxFreqOffset();
    if (readAppendedInfo) {
      RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
      // Note the packet's status
      RAIL_RxPacketInfo_t packetInfo;
      RAIL_RxPacketHandle_t packetHandle
        = RAIL_GetRxPacketInfo(railHandle, RAIL_RX_PACKET_HANDLE_OLDEST,
                               &packetInfo);
      // assert(packetHandle != NULL);
      packetData->rxPacket.packetStatus = packetInfo.packetStatus;

      if (printTimingInfo) {
        RAIL_PacketTimePosition_t positions[] = {
          RAIL_PACKET_TIME_AT_PREAMBLE_START,
          RAIL_PACKET_TIME_AT_SYNC_END,
          RAIL_PACKET_TIME_AT_PACKET_END,
        };
        typedef RAIL_Status_t (*Adjustment)(RAIL_Handle_t, uint16_t, RAIL_Time_t *);
        Adjustment funcs[] = {
          RAIL_GetRxTimePreambleStart,
          RAIL_GetRxTimeSyncWordEnd,
          RAIL_GetRxTimeFrameEnd,
        };
        #define NUM_POSITIONS (sizeof(positions) / sizeof(positions[0]))
        RAIL_Time_t times[2 * NUM_POSITIONS];

        appendedInfo->timeReceived.totalPacketBytes = packetData->rxPacket.dataLength;
        for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
          appendedInfo->timeReceived.timePosition = positions[i];
          RAIL_GetRxPacketDetails(railHandle, packetHandle, appendedInfo);
          times[i] = appendedInfo->timeReceived.packetTime;
        }
        // Get the appended info details and release this packet
        RAIL_GetRxPacketDetailsAlt(railHandle, packetHandle, appendedInfo);
        for (uint8_t i = 0; i < NUM_POSITIONS; i++) {
          times[i + NUM_POSITIONS] = appendedInfo->timeReceived.packetTime;
          funcs[i](railHandle, packetData->rxPacket.dataLength, &times[i + NUM_POSITIONS]);
        }
        responsePrint(sl_cli_get_command_string(args, 0),
                      "Pre:%u,Sync:%u,End:%u,PreAlt:%u,SyncAlt:%u,EndAlt:%u",
                      times[0], times[1], times[2],
                      times[3], times[4], times[5]);
      } else {
        // Get the appended info details and release this packet
        status = RAIL_GetRxPacketDetailsAlt(railHandle, packetHandle, appendedInfo);
        if (status == RAIL_STATUS_NO_ERROR) {
          RAIL_Time_t *sync = &appendedInfo->timeReceived.packetTime;
          status = RAIL_GetRxTimeSyncWordEnd(railHandle,
                                             packetData->rxPacket.dataLength, sync);
        }
      }

      RAIL_ReleaseRxPacket(railHandle, packetHandle);

      // Make sure there was a valid packet
      if (status != RAIL_STATUS_NO_ERROR) {
        memset(&packetData->rxPacket.appendedInfo, 0, sizeof(RAIL_RxPacketDetails_t));
        packetData->rxPacket.appendedInfo.rssi = RAIL_RSSI_INVALID_DBM;
        if (packetData->rxPacket.dataLength == 0) {
          responsePrintError(sl_cli_get_command_string(args, 0), 0x52, "No packet found in rx fifo!");
          memoryFree(rxPacketHandle);
          return;
        }
      }
    }

    queueAdd(&railAppEventQueue, rxPacketHandle);
  }
}

void fifoReset(sl_cli_command_arg_t *args)
{
  char *outputStr[] = { "DoNothing", "Reset" };
  bool txReset = !!sl_cli_get_argument_uint8(args, 0);
  bool rxReset = !!sl_cli_get_argument_uint8(args, 1);

  RAIL_ResetFifo(railHandle, txReset, rxReset);

  responsePrint(sl_cli_get_command_string(args, 0),
                "TxFifo:%s,"
                "RxFifo:%s",
                outputStr[txReset],
                outputStr[rxReset]);
}

void txFifoManualLoad(sl_cli_command_arg_t *args)
{
  if (!txFifoManual) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x51, "Must be in tx fifo manual mode (fifoModeTestOptions).");
  } else {
    // Test loading unaligned data
    loadTxData(txData, 1U);
    loadTxData(&txData[1], txDataLen - 1U);
    responsePrint(sl_cli_get_command_string(args, 0), "Status:Fifo Written");
  }
}

void abortRxPacket(sl_cli_command_arg_t *args)
{
  abortRxDelay = sl_cli_get_argument_uint32(args, 0);

  responsePrint(sl_cli_get_command_string(args, 0), "TimeAfterSyncToAbort:%d", abortRxDelay);
}

void peekRx(sl_cli_command_arg_t *args)
{
  uint16_t offset = 0;
  uint16_t bytesToPeek = sl_cli_get_argument_uint16(args, 0);
  uint8_t pDst[10];
  if (bytesToPeek > 10) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x52, "Can't peek more than 10 bytes");
    return;
  }
  if (sl_cli_get_argument_count(args) >= 2) {
    offset = sl_cli_get_argument_uint16(args, 1);
  }
  if (RAIL_PeekRxPacket(railHandle, RAIL_RX_PACKET_HANDLE_OLDEST,
                        pDst, bytesToPeek, offset) != bytesToPeek) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x53, "Requested bytes not in receive buffer");
    return;
  }
  printPacket(sl_cli_get_command_string(args, 0), pDst, bytesToPeek, NULL);
}
