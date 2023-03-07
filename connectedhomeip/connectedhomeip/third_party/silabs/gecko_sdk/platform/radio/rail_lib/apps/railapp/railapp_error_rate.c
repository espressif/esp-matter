/***************************************************************************//**
 * @file
 * @brief Source file for RAIL error rate functionality
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

#include <string.h>

#include "rail.h"
#include "em_core.h"
#include "app_main.h"
#include "railapp_rx.h"
#include "railapp_tx.h"
#include "railapp_error_rate.h"

#ifndef PTE_LIGHTRAIL

// Variables for BER testing
BerStatus_t berStats = { 0 };
bool berTestModeEnabled = false;
uint32_t berBytesToTest;

//----------------------------------------------------------------------------
// Error Rate Commands
//-----------------------------------------------------------------------------
/***************************************************************************//**
 * @addtogroup error_rate Error Rate Commands
 * @brief APIs to configure BER Mode
 * @{
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *   Reset LIGHTRAIL counters.
 ******************************************************************************/
void RAILAPP_ResetCounters(void)
{
  memset(&counters, 0, sizeof(counters));
}

/***************************************************************************//**
 * @brief
 *   Resets the berStats global variable.
 ******************************************************************************/
void RAILAPP_BerResetStats(uint32_t numBytes)
{
  // Reset test statistics
  memset(&berStats, 0, sizeof(BerStatus_t));

  // 0x1FFFFFFF bytes (0xFFFFFFF8 bits) is max number of bytes that can be
  // tested without uint32_t math rollover; numBytes = 0 is same as max
  if ((0 == numBytes) || (numBytes > 0x1FFFFFFF)) {
    numBytes = 0x1FFFFFFF;
  }
  berStats.bytesTotal = numBytes;
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

/***************************************************************************//**
 * @brief
 *   Congifures the board for BER Testing and sets the # of bytes to test.
 *
 * @param berBytesToTest Number of bytes to test.
 * @return berBytesToTest Returns berBytesToTest configured, or -1 for error.
 ******************************************************************************/
uint32_t RAILAPP_BerConfigSet(uint32_t berBytesToTest)
{
  RAIL_Status_t status;
  uint16_t rxThreshold, packetLength;

  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
  RAIL_ResetFifo(railHandle, true, true);

  // configure radio
  railDataConfig.txSource = TX_PACKET_DATA;
  // railDataConfig.rxSource = RX_BER_STREAM;
  railDataConfig.rxSource = RX_PACKET_DATA;
  railDataConfig.txMethod = FIFO_MODE;
  railDataConfig.rxMethod = FIFO_MODE;
  status = RAIL_ConfigData(railHandle, &railDataConfig);
  if (status) {
    return -1;
  }

  // configure RX FIFO
  rxThreshold = 100;
  rxThreshold = RAIL_SetRxFifoThreshold(railHandle, rxThreshold);

  // specify overall packet length info (infinite)
  packetLength = 0;
  packetLength = RAIL_SetFixedLength(railHandle, packetLength);

  RAIL_EnablePti(railHandle, false);

  RAILAPP_BerResetStats(berBytesToTest);

  return berStats.bytesTotal;
}

/***************************************************************************//**
 * @brief
 *   Start BER test mode on board. Resets counters and FIFO and Starts RX.
 *
 * @param enable Whether to start RX and enable BER testing.
 ******************************************************************************/
void RAILAPP_BerRx(bool enable)
{
  RAILAPP_ResetCounters();

  RAIL_Idle(railHandle, RAIL_IDLE_ABORT, true);
  RAIL_ResetFifo(railHandle, true, true);
  if (enable) {
    RAIL_EnablePti(railHandle, false);
    RAILAPP_BerResetStats(berStats.bytesTotal);
    berTestModeEnabled = true;
    uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
    RAIL_StartRx(railHandle, channel, NULL);
  }
}

/***************************************************************************//**
 * @brief
 *   Get BER status, including bytesTotal, bytesTested, bitErrors, and rssi.
 *
 * @return BerStatus_t BER Status struct.
 ******************************************************************************/
BerStatus_t RAILAPP_BerStatusGet(void)
{
  return berStats;
}

/** @} */ // end of group error_rate

// Internal functions

static void RAILAPP_BerSource_RxFifoAlmostFull(uint16_t bytesAvailable)
{
  // All incoming bytes are received and validated here.
  uint16_t numBytes;
  bool stopBerRx = false;

  while ((RAIL_GetRxFifoBytesAvailable(railHandle) > RAIL_GetRxFifoThreshold(railHandle))
         && !stopBerRx) {
    // Read multiple bytes in if they're available.
    // Reuse the pktData[APP_MAX_PACKET_LENGTH] array since we won't be
    // transmitting in BER Test mode anyway.
    numBytes = RAIL_ReadRxFifo(railHandle, pktData, APP_MAX_PACKET_LENGTH);

    for (uint16_t x = 0; x < numBytes && !stopBerRx; x++) {
      // Update BER statistics
      if (berStats.bytesTested < berStats.bytesTotal) {
        // Counters will not overflow since bytesTotal max value is capped.
        berStats.bitErrors += countBits(pktData[x]);
        berStats.bytesTested++;
      } else {
        stopBerRx = true; // statistics are all gathered - stop now
      }
    }
  }
  berStats.rssi = (int8_t)(RAIL_GetRssi(railHandle, true) / 4); // disregard decimal point

  // stop RXing when enough bits are acquired or an error (i.e. RX overflow)
  if (stopBerRx) {
    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN, true);
    RAIL_ResetFifo(railHandle, true, true);
    berTestModeEnabled = false;
  }
}

void RAILCb_BerRxFifoAlmostFull(RAIL_Handle_t railHandle)
{
  uint16_t bytesAvailable = RAIL_GetRxFifoBytesAvailable(railHandle);
  counters.rxFifoAlmostFull++;

  if (berTestModeEnabled) {
    RAILAPP_BerSource_RxFifoAlmostFull(bytesAvailable);
  }
}

#endif //PTE_LIGHTRAIL
