/***************************************************************************//**
 * @file
 * @brief This file implements informational commands for RAILtest applications.
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
#include "response_print.h"

#include "rail.h"
#include "rail_types.h"
#include "rail_features.h"
#include "rail_ieee802154.h"
#include "app_common.h"

uint16_t getLikelyChannel(void)
{
  uint16_t chan = channel; // Default to last 'offline' setChannel setting
  if ((RAIL_GetRadioState(railHandle)
       & (RAIL_RF_STATE_RX | RAIL_RF_STATE_TX)) != 0U) {
    (void) RAIL_GetChannel(railHandle, &chan); // capture online channel
    // Note: this may not reflect a channel-hopping channel
  }
  return chan;
}

static void printBitField(uint32_t bitField,
                          char** fieldNames,
                          uint8_t numFields)
{
  uint8_t i;
  for (i = 0; i < numFields; ++i) {
    responsePrintContinue("%s:%u", fieldNames[i], bitField & 1);
    bitField >>= 1;
  }
}

static void printRailState(void)
{
  static char* railStates[] = { "RAIL_state_active",
                                "RAIL_state_rx",
                                "RAIL_state_tx" };
  RAIL_RadioState_t rfState = RAIL_GetRadioState(railHandle);
  responsePrintContinue("RfState:%s",
                        getRfStateName(rfState));
  printBitField(rfState,
                railStates,
                sizeof(railStates) / sizeof(railStates[0])
                );
}

const char *getStatusMessage(RAIL_Status_t status)
{
  switch (status) {
    case RAIL_STATUS_NO_ERROR:
      return "Success";
    case RAIL_STATUS_INVALID_PARAMETER:
      return "InvalidParameter";
    case RAIL_STATUS_INVALID_STATE:
      return "InvalidState";
    case RAIL_STATUS_INVALID_CALL:
      return "InvalidCall";
    default:
      return "Unknown";
  }
}

void getStatus(sl_cli_command_arg_t *args)
{
  responsePrintStart(sl_cli_get_command_string(args, 0));
  responsePrintContinue("UserTxCount:%u,"
                        "AckTxCount:%u,"
                        "UserTxAborted:%u,"
                        "AckTxAborted:%u,"
                        "UserTxBlocked:%u,"
                        "AckTxBlocked:%u,"
                        "UserTxUnderflow:%u,"
                        "AckTxUnderflow:%u,"
                        "RxCount:%u,"
                        "RxCrcErrDrop:%u,"
                        "SyncDetect:%u,"
                        "NoRxBuffer:%u",
                        counters.userTx,
                        counters.ackTx,
                        counters.userTxAborted,
                        counters.ackTxAborted,
                        counters.userTxBlocked,
                        counters.ackTxBlocked,
                        counters.userTxUnderflow,
                        counters.ackTxUnderflow,
                        counters.receive,
                        counters.receiveCrcErrDrop,
                        counters.syncDetect,
                        counters.noRxBuffer
                        );
  responsePrintContinue("TxRemainErrs:%u,"
                        "RfSensed:%u,"
                        "ackTimeout:%u,"
                        "ackTxFpSet:%u,"
                        "ackTxFpFail:%u,"
                        "ackTxFpAddrFail:%u",
                        counters.userTxRemainingErrors,
                        counters.rfSensedEvent,
                        counters.ackTimeout,
                        counters.ackTxFpSet,
                        counters.ackTxFpFail,
                        counters.ackTxFpAddrFail
                        );
  printRailState();
  responsePrintContinue("Channel:%u,"
                        "AppMode:%s,"
                        "TimingLost:%u,"
                        "TimingDetect:%u,"
                        "FrameErrors:%u,"
                        "RxFifoFull:%u,"
                        "RxOverflow:%u,"
                        "AddrFilt:%u,"
                        "Aborted:%u,"
                        "RxBeams:%u,"
                        "DataRequests:%u",
                        getLikelyChannel(),
                        appModeNames(currentAppMode()),
                        counters.timingLost,
                        counters.timingDetect,
                        counters.frameError,
                        counters.rxFifoFull,
                        counters.rxOfEvent,
                        counters.addrFilterEvent,
                        counters.rxFail,
                        counters.rxBeams,
                        counters.dataRequests
                        );
  // Avoid use of %ll long-long formats due to iffy printf library support
  responsePrintEnd("Calibrations:%u,"
                   "TxChannelBusy:%u,"
                   "TxClear:%u,"
                   "TxCca:%u,"
                   "TxRetry:%u,"
                   "UserTxStarted:%u,"
                   "PaProtect:%u,"
                   "SubPhy0:%u,"
                   "SubPhy1:%u,"
                   "SubPhy2:%u,"
                   "SubPhy3:%u,"
                   "rxRawSourceBytes:0x%x%08x",
                   counters.calibrations,
                   counters.txChannelBusy,
                   counters.lbtSuccess,
                   counters.lbtStartCca,
                   counters.lbtRetry,
                   counters.userTxStarted,
                   counters.paProtect,
                   counters.subPhyCount[0],
                   counters.subPhyCount[1],
                   counters.subPhyCount[2],
                   counters.subPhyCount[3],
                   (uint32_t)(counters.rxRawSourceBytes >> 32),
                   (uint32_t)(counters.rxRawSourceBytes)
                   );
}

void fifoStatus(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint16_t spaceCount =  RAIL_GetTxFifoSpaceAvailable(railHandle);
  uint16_t byteCount = RAIL_GetRxFifoBytesAvailable(railHandle);
  responsePrint(sl_cli_get_command_string(args, 0),
                "TxSpaceCount:%u,"
                "RxByteCount:%u,"
                "TxFifoThreshold:%d,"
                "RxFifoThreshold:%d,"
                "TxFifoAlmostEmpty:%u,"
                "RxFifoAlmostFull:%u,"
                "RxFifoFull:%u,"
                "RxOverflow:%u,"
                "UserTxUnderflow:%u,"
                "AckTxUnderflow:%u",
                spaceCount,
                byteCount,
                RAIL_GetTxFifoThreshold(railHandle),
                RAIL_GetRxFifoThreshold(railHandle),
                counters.txFifoAlmostEmpty,
                counters.rxFifoAlmostFull,
                counters.rxFifoFull,
                counters.rxOfEvent,
                counters.userTxUnderflow,
                counters.ackTxUnderflow
                );
}

static void printRailVersion(sl_cli_command_arg_t *args, bool verbose)
{
  RAIL_Version_t rail_ver;
  RAIL_GetVersion(&rail_ver, verbose);
  responsePrintStart(sl_cli_get_command_string(args, 0));
  responsePrintContinue("App:%d.%d.%d",
                        rail_ver.major, rail_ver.minor, rail_ver.rev);
  if (verbose) {
    responsePrintContinue("RAIL:%d.%d.%d.%d,"
                          "hash:0x%.8X,"
                          "flags:0x%.2X",
                          rail_ver.major, rail_ver.minor, rail_ver.rev, rail_ver.build,
                          rail_ver.hash,
                          rail_ver.flags);
  } else {
    responsePrintContinue("RAIL:%d.%d.%d",
                          rail_ver.major, rail_ver.minor, rail_ver.rev);
  }
  responsePrintEnd("Multiprotocol:%s,"
                   "Built:%s",
                   rail_ver.multiprotocol ? "True" : "False",
                   buildDateTime);
}

void getVersion(sl_cli_command_arg_t *args)
{
  printRailVersion(args, false);
}

void getVersionVerbose(sl_cli_command_arg_t *args)
{
  printRailVersion(args, true);
}

static const char *ptiModes[] = {
  "Disabled", "3-wire-SPI", "2-wire-UART", "1-wire-UART", "??",
};

void getPti(sl_cli_command_arg_t *args)
{
  RAIL_PtiConfig_t ptiConfig;
  CHECK_RAIL_HANDLE((args == NULL) ? "pti" : sl_cli_get_command_string(args, 0));
  // Get the current config and change the baud rate as requested
  RAIL_GetPtiConfig(railHandle, &ptiConfig);
  if (ptiConfig.mode >= (sizeof(ptiModes) / sizeof(*ptiModes))) {
    ptiConfig.mode = (sizeof(ptiModes) / sizeof(*ptiModes)) - 1U;
  }
  responsePrint(((args == NULL) ? "pti" : sl_cli_get_command_string(args, 0)),
                "mode:%s,baud:%u,protocol:%u,radioConfig:0x%02x",
                ptiModes[ptiConfig.mode],
                ptiConfig.baud,
                RAIL_GetPtiProtocol(railHandle),
                RAIL_IEEE802154_GetPtiRadioConfig(railHandle));
}

void setPtiProtocol(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  (void) RAIL_SetPtiProtocol(railHandle, (RAIL_PtiProtocol_t) sl_cli_get_argument_uint8(args, 0));
  // success (or not) will be reflected in getPti() output
  getPti(args);
}

void offsetLqi(sl_cli_command_arg_t *args)
{
  int32_t offset = sl_cli_get_argument_int32(args, 0);
  if ((offset > 0xFF) || (offset < -0xFF)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x08,
                       "Invalid value. Must be between -255 and 255.");
    return;
  }
  lqiOffset = (int16_t)offset;
  responsePrint(sl_cli_get_command_string(args, 0), "lqiOffset:%d", lqiOffset);
}

void getRssi(sl_cli_command_arg_t *args)
{
  char bufRssi[10];
  uint32_t waitTimeout = RAIL_GET_RSSI_NO_WAIT;

  if (sl_cli_get_argument_count(args) == 1) {
    waitTimeout = sl_cli_get_argument_uint8(args, 0);
    // For backwards compatability, map the value 1 to a no timeout wait
    if (waitTimeout == 1) {
      waitTimeout = RAIL_GET_RSSI_WAIT_WITHOUT_TIMEOUT;
    }
  }
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  int16_t rssi = RAIL_GetRssiAlt(railHandle, waitTimeout);

  // The lowest negative value is used to indicate an error reading the RSSI
  if (rssi == RAIL_RSSI_INVALID) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x08, "Could not read RSSI. Ensure Rx is enabled");
    return;
  }

  sprintfFloat(bufRssi, sizeof(bufRssi), ((float) rssi / 4), 2);
  if (RAIL_IEEE802154_IsEnabled(railHandle)) {
    uint8_t energyDetect = RAIL_IEEE802154_ConvertRssiToEd(rssi / 4);
    responsePrint(sl_cli_get_command_string(args, 0), "rssi:%s,ed154:%u", bufRssi, energyDetect);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "rssi:%s", bufRssi);
  }
}

void startAvgRssi(sl_cli_command_arg_t *args)
{
  uint32_t averageTimeUs = sl_cli_get_argument_uint32(args, 0);
  uint16_t avgChannel = channel;
  if (sl_cli_get_argument_count(args) == 2) {
    avgChannel = sl_cli_get_argument_uint16(args, 1);
  }
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, NULL)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x08, "Could not read RSSI. Ensure RX is disabled.");
    return;
  }
  RAIL_Time_t startTime = RAIL_GetTime();
  if (RAIL_StartAverageRssi(railHandle, avgChannel, averageTimeUs, NULL) != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x08, "Could not read RSSI.");
    return;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "Time:%d", startTime);
}
void getAvgRssi(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  int16_t rssi = RAIL_GetAverageRssi(railHandle);
  char bufRssi[10];
  if (rssi == RAIL_RSSI_INVALID) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x08, "Invalid RSSI. Make sure startAvgRssi ran successfully.");
    return;
  }
  sprintfFloat(bufRssi, sizeof(bufRssi), ((float) rssi / 4), 2);
  responsePrint(sl_cli_get_command_string(args, 0), "rssi:%s", bufRssi);
}
void setRssiOffset(sl_cli_command_arg_t *args)
{
  int8_t rssiOffset = sl_cli_get_argument_int8(args, 0);
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (RAIL_STATUS_NO_ERROR != RAIL_SetRssiOffset(railHandle, rssiOffset)) {
    responsePrint(sl_cli_get_command_string(args, 0), "Error setting the rssiOffset");
  } else {
    rssiOffset = RAIL_GetRssiOffset(railHandle);
    responsePrint(sl_cli_get_command_string(args, 0), "rssiOffset:%d", rssiOffset);
  }
}
void getRssiOffset(sl_cli_command_arg_t *args)
{
  // Radio specific RSSI offset
  int8_t radioRssiOffset = RAIL_GetRssiOffset(RAIL_EFR32_HANDLE);
  if ((sl_cli_get_argument_count(args) >= 1)
      && !!sl_cli_get_argument_uint8(args, 0)) {
    responsePrint(sl_cli_get_command_string(args, 0), "radioRssiOffset:%d", radioRssiOffset);
  } else {
    CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
    // Protocol specific RSSI offset
    int8_t protocolRssiOffset = RAIL_GetRssiOffset(railHandle);
    responsePrint(sl_cli_get_command_string(args, 0), "rssiOffset:%d,radioRssiOffset:%d,totalRssiOffset:%d", \
                  protocolRssiOffset,
                  radioRssiOffset,
                  (protocolRssiOffset + radioRssiOffset));
  }
}
void getRssiDetectThreshold(sl_cli_command_arg_t *args)
{
  int8_t rssiDetectThresholdDbm = RAIL_GetRssiDetectThreshold(railHandle);
  responsePrint(sl_cli_get_command_string(args, 0), "rssiDetectThresholdDbm:%d,status:%s",
                rssiDetectThresholdDbm,
                rssiDetectThresholdDbm == RAIL_RSSI_INVALID_DBM ? "Disabled" : "Enabled");
}
void setRssiDetectThreshold(sl_cli_command_arg_t *args)
{
  int8_t rssiDetectThresholdDbm = sl_cli_get_argument_int8(args, 0);
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!RAIL_SupportsRssiDetectThreshold(railHandle)) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x0A, "RSSI threshold detection is not supported on this chip.");
  } else {
    if (RAIL_STATUS_NO_ERROR != RAIL_SetRssiDetectThreshold(railHandle, rssiDetectThresholdDbm)) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x0B, "Error setting the rssiDetectThreshold.");
    } else {
      getRssiDetectThreshold(args);
    }
  }
}
void sweepPower(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  int32_t lowPower = sl_cli_get_argument_int32(args, 0);
  int32_t hiPower = sl_cli_get_argument_int32(args, 1);
  int32_t period = sl_cli_get_argument_int32(args, 2);
  int32_t halfPeriodStepUs = period / 2;
  uint32_t expired = RAIL_GetTime() + (uint32_t)5000000;
  while (expired > RAIL_GetTime()) {
    RAIL_StopTxStream(railHandle);
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
    RAIL_SetTxPower(railHandle, lowPower);
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);
    usDelay(halfPeriodStepUs);
    RAIL_StopTxStream(railHandle);
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
    RAIL_SetTxPower(railHandle, hiPower);
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);
    usDelay(halfPeriodStepUs);
  }
  RAIL_StopTxStream(railHandle);
}

void isRssiRdy(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (RAIL_IsAverageRssiReady(railHandle)) {
    responsePrint(sl_cli_get_command_string(args, 0), "isReady:True");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "isReady:False");
  }
  return;
}

void resetCounters(sl_cli_command_arg_t *args)
{
  memset(&counters, 0, sizeof(counters));
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getStatus(args);
}

void getTime(sl_cli_command_arg_t *args)
{
  responsePrint(sl_cli_get_command_string(args, 0), "Time:%u", RAIL_GetTime());
}

void setTime(sl_cli_command_arg_t *args)
{
  uint32_t timeUs = sl_cli_get_argument_uint32(args, 0);
  if (RAIL_STATUS_NO_ERROR == RAIL_SetTime(timeUs)) {
    responsePrint(sl_cli_get_command_string(args, 0), "Status:Success,CurrentTime:%u", RAIL_GetTime());
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "Status:Error,CurrentTime:%u", RAIL_GetTime());
  }
}

void printChipFeatures(sl_cli_command_arg_t *args)
{
  RAIL_TxPowerLevel_t maxPowerLevel;
  responsePrintHeader(sl_cli_get_command_string(args, 0), "Feature:%s,CompileTime:%s,RunTime:%s");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_DUAL_BAND",
                     RAIL_SUPPORTS_DUAL_BAND ? "Yes" : "No",
                     RAIL_SupportsDualBand(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_2P4GHZ_BAND",
                     RAIL_SUPPORTS_2P4GHZ_BAND ? "Yes" : "No",
                     RAIL_Supports2p4GHzBand(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_SUBGHZ_BAND",
                     RAIL_SUPPORTS_SUBGHZ_BAND ? "Yes" : "No",
                     RAIL_SupportsSubGHzBand(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_OFDM_PA",
                     RAIL_SUPPORTS_OFDM_PA ? "Yes" : "No",
                     RAIL_SupportsOFDMPA(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_ALTERNATE_TX_POWER",
                     RAIL_SUPPORTS_ALTERNATE_TX_POWER ? "Yes" : "No",
                     RAIL_SupportsAlternateTxPower(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_ANTENNA_DIVERSITY",
                     RAIL_SUPPORTS_ANTENNA_DIVERSITY ? "Yes" : "No",
                     RAIL_SupportsAntennaDiversity(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_CHANNEL_HOPPING",
                     RAIL_SUPPORTS_CHANNEL_HOPPING ? "Yes" : "No",
                     RAIL_SupportsChannelHopping(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_DUAL_SYNC_WORDS",
                     RAIL_SUPPORTS_DUAL_SYNC_WORDS ? "Yes" : "No",
                     RAIL_SupportsDualSyncWords(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_EXTERNAL_THERMISTOR",
                     RAIL_SUPPORTS_EXTERNAL_THERMISTOR ? "Yes" : "No",
                     RAIL_SupportsExternalThermistor(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_AUXADC",
                     RAIL_SUPPORTS_AUXADC ? "Yes" : "No",
                     RAIL_SupportsAuxAdc(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_PRECISION_LFRCO",
                     RAIL_SUPPORTS_PRECISION_LFRCO ? "Yes" : "No",
                     RAIL_SupportsPrecisionLFRCO(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_RADIO_ENTROPY",
                     RAIL_SUPPORTS_RADIO_ENTROPY ? "Yes" : "No",
                     RAIL_SupportsRadioEntropy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_RFSENSE_ENERGY_DETECTION",
                     RAIL_SUPPORTS_RFSENSE_ENERGY_DETECTION ? "Yes" : "No",
                     RAIL_SupportsRfSenseEnergyDetection(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_RFSENSE_SELECTIVE_OOK",
                     RAIL_SUPPORTS_RFSENSE_SELECTIVE_OOK ? "Yes" : "No",
                     RAIL_SupportsRfSenseSelectiveOok(railHandle) ? "Yes" : "No");
 #ifdef  RAIL_TX_POWER_MODE_2P4GIG_HIGHEST
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_2P4GIG_HIGHEST,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_2P4GIG_HIGHEST", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_2P4GIG_HIGHEST", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_2P4GIG_HIGHEST", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_2P4GIG_HIGHEST
 #ifdef  RAIL_TX_POWER_MODE_SUBGIG_HIGHEST
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_HIGHEST,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_HIGHEST", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_HIGHEST", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_HIGHEST", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_HIGHEST
 #ifdef  RAIL_TX_POWER_MODE_2P4GIG_HP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_2P4GIG_HP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_2P4GIG_HP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_2P4GIG_HP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_2P4GIG_HP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_2P4GIG_HP
 #ifdef  RAIL_TX_POWER_MODE_2P4GIG_MP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_2P4GIG_MP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_2P4GIG_MP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_2P4GIG_MP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_2P4GIG_MP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_2P4GIG_LP
 #ifdef  RAIL_TX_POWER_MODE_2P4GIG_LP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_2P4GIG_LP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_2P4GIG_LP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_2P4GIG_LP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_2P4GIG_LP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_2P4GIG_LP
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_HP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_HP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_HP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_HP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_HP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_HP
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_MP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_MP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_MP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_MP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_MP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_MP
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_LP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_LP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_LP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_LP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_LP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_LP
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_LLP
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_LLP,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_LLP", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_LLP", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_LLP", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_LLP
 #ifdef RAIL_TX_POWER_MODE_SUBGIG
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM
 #ifdef RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM
 #ifdef RAIL_TX_POWER_MODE_OFDM_PA
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_OFDM_PA,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_OFDM_PA", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_OFDM_PA", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_OFDM_PA", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_OFDM_PA
 #ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM
 #ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM
 #ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM
 #ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM
  if (RAIL_SupportsTxPowerMode(railHandle,
                               RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM,
                               &maxPowerLevel)
      && (maxPowerLevel != RAIL_TX_POWER_LEVEL_INVALID)) {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%u",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM", "Yes",
                       maxPowerLevel);
  } else {
    responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                       "RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM", "Yes", "No");
  }
 #else
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM", "N/A", "N/A");
 #endif//RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_PROTOCOL_BLE",
                     RAIL_SUPPORTS_PROTOCOL_BLE ? "Yes" : "No",
                     RAIL_SupportsProtocolBLE(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_1MBPS_NON_VITERBI",
                     RAIL_BLE_SUPPORTS_1MBPS_NON_VITERBI ? "Yes" : "No",
                     RAIL_BLE_Supports1MbpsNonViterbi(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_1MBPS_VITERBI",
                     RAIL_BLE_SUPPORTS_1MBPS_VITERBI ? "Yes" : "No",
                     RAIL_BLE_Supports1MbpsViterbi(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_1MBPS",
                     RAIL_BLE_SUPPORTS_1MBPS ? "Yes" : "No",
                     RAIL_BLE_Supports1Mbps(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_2MBPS_NON_VITERBI",
                     RAIL_BLE_SUPPORTS_2MBPS_NON_VITERBI ? "Yes" : "No",
                     RAIL_BLE_Supports2MbpsNonViterbi(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_2MBPS_VITERBI",
                     RAIL_BLE_SUPPORTS_2MBPS_VITERBI ? "Yes" : "No",
                     RAIL_BLE_Supports2MbpsViterbi(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_2MBPS",
                     RAIL_BLE_SUPPORTS_2MBPS ? "Yes" : "No",
                     RAIL_BLE_Supports2Mbps(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_ANTENNA_SWITCHING",
                     RAIL_BLE_SUPPORTS_ANTENNA_SWITCHING ? "Yes" : "No",
                     RAIL_BLE_SupportsAntennaSwitching(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_CODED_PHY",
                     RAIL_BLE_SUPPORTS_CODED_PHY ? "Yes" : "No",
                     RAIL_BLE_SupportsCodedPhy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_CTE",
                     RAIL_BLE_SUPPORTS_CTE ? "Yes" : "No",
                     RAIL_BLE_SupportsCte(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_QUUPPA",
                     RAIL_BLE_SUPPORTS_QUUPPA ? "Yes" : "No",
                     RAIL_BLE_SupportsQuuppa(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_IQ_SAMPLING",
                     RAIL_BLE_SUPPORTS_IQ_SAMPLING ? "Yes" : "No",
                     RAIL_BLE_SupportsIQSampling(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_BLE_SUPPORTS_PHY_SWITCH_TO_RX",
                     RAIL_BLE_SUPPORTS_PHY_SWITCH_TO_RX ? "Yes" : "No",
                     RAIL_BLE_SupportsPhySwitchToRx(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_PROTOCOL_IEEE802154",
                     RAIL_SUPPORTS_PROTOCOL_IEEE802154 ? "Yes" : "No",
                     RAIL_SupportsProtocolIEEE802154(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_COEX_PHY",
                     RAIL_IEEE802154_SUPPORTS_COEX_PHY ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsCoexPhy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_E_SUBSET_GB868",
                     RAIL_IEEE802154_SUPPORTS_E_SUBSET_GB868 ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsESubsetGB868(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_E_ENHANCED_ACK",
                     RAIL_IEEE802154_SUPPORTS_E_ENHANCED_ACK ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsEEnhancedAck(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_G_SUBSET_GB868",
                     RAIL_IEEE802154_SUPPORTS_G_SUBSET_GB868 ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsGSubsetGB868(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_G_DYNFEC",
                     RAIL_IEEE802154_SUPPORTS_G_DYNFEC ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsGDynFec(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_G_MODESWITCH",
                     RAIL_IEEE802154_SUPPORTS_G_MODESWITCH ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsGModeSwitch(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_RX",
                     RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_RX ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsGUnwhitenedRx(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_TX",
                     RAIL_IEEE802154_SUPPORTS_G_UNWHITENED_TX ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsGUnwhitenedTx(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_G_4BYTE_CRC",
                     RAIL_IEEE802154_SUPPORTS_G_4BYTE_CRC ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsG4ByteCrc(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_CANCEL_FRAME_PENDING_LOOKUP",
                     RAIL_IEEE802154_SUPPORTS_CANCEL_FRAME_PENDING_LOOKUP ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsCancelFramePendingLookup(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_EARLY_FRAME_PENDING_LOOKUP",
                     RAIL_IEEE802154_SUPPORTS_EARLY_FRAME_PENDING_LOOKUP ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsEarlyFramePendingLookup(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_E_MULTIPURPOSE_FRAMES",
                     RAIL_IEEE802154_SUPPORTS_E_MULTIPURPOSE_FRAMES ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsEMultipurposeFrames(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_DUAL_PA_CONFIG",
                     RAIL_IEEE802154_SUPPORTS_DUAL_PA_CONFIG ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsDualPaConfig(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY",
                     RAIL_IEEE802154_SUPPORTS_CUSTOM1_PHY ? "Yes" : "No",
                     RAIL_IEEE802154_SupportsCustom1Phy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_PROTOCOL_ZWAVE",
                     RAIL_SUPPORTS_PROTOCOL_ZWAVE ? "Yes" : "No",
                     RAIL_SupportsProtocolZWave(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_ZWAVE_SUPPORTS_REGION_PTI",
                     RAIL_ZWAVE_SUPPORTS_REGION_PTI ? "Yes" : "No",
                     RAIL_ZWAVE_SupportsRegionPti(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_ZWAVE_SUPPORTS_ED_PHY",
                     RAIL_ZWAVE_SUPPORTS_ED_PHY ? "Yes" : "No",
                     RAIL_ZWAVE_SupportsEnergyDetectPhy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_ZWAVE_SUPPORTS_CONC_PHY",
                     RAIL_ZWAVE_SUPPORTS_CONC_PHY ? "Yes" : "No",
                     RAIL_ZWAVE_SupportsConcPhy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_SQ_PHY",
                     RAIL_SUPPORTS_SQ_PHY ? "Yes" : "No",
                     RAIL_SupportsSQPhy(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_RX_RAW_DATA",
                     RAIL_SUPPORTS_RX_RAW_DATA ? "Yes" : "No",
                     RAIL_SupportsRxRawData(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_DIRECT_MODE",
                     RAIL_SUPPORTS_DIRECT_MODE ? "Yes" : "No",
                     RAIL_SupportsDirectMode(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK",
                     RAIL_SUPPORTS_ADDR_FILTER_ADDRESS_BIT_MASK ? "Yes" : "No",
                     RAIL_SupportsAddrFilterAddressBitMask(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_MFM",
                     RAIL_SUPPORTS_MFM ? "Yes" : "No",
                     RAIL_SupportsMfm(railHandle) ? "Yes" : "No");
  responsePrintMulti("Feature:%s,CompileTime:%s,RunTime:%s",
                     "RAIL_SUPPORTS_EFF",
                     RAIL_SUPPORTS_EFF ? "Yes" : "No",
                     RAIL_SupportsEff(railHandle) ? "Yes" : "No");
}

void cliSeparatorHack(sl_cli_command_arg_t *args)
{
  (void) args;
}
