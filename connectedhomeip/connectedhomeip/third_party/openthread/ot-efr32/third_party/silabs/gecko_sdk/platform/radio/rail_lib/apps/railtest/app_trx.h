/***************************************************************************//**
 * @file
 * @brief RAILtest transmit and receive
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

#ifdef __cplusplus
extern "C" {
#endif

typedef struct BerStatus{
  uint32_t bytesTotal; /**< Number of bytes to receive */
  uint32_t bytesTested; /**< Number of bytes currently tested */
  uint32_t bitErrors; /**< Number of bits errors detected */
  int8_t   rssi; /**< Current RSSI value during pattern acquisition */
} BerStatus_t;

// Variables for TX
extern volatile RAIL_Events_t lastTxStatus;
extern volatile RAIL_Events_t lastTxAckStatus;
extern volatile uint32_t sentAckPackets;
extern RAIL_TxPacketDetails_t previousTxAppendedInfo;
extern RAIL_TxPacketDetails_t previousTxAckAppendedInfo;

// Variables for RX
extern uint32_t abortRxDelay;
extern RAIL_ScheduleTxConfig_t nextPacketTxTime;

// Variables for BER testing
extern BerStatus_t berStats;
extern bool berTestModeEnabled;

void configRxLengthSetting(uint16_t rxLength);

void loadTxData(uint8_t *data, uint16_t dataLen);
void setNextPacketTime(RAIL_ScheduleTxConfig_t *scheduledTxOptions);

#ifdef __cplusplus
}
#endif
