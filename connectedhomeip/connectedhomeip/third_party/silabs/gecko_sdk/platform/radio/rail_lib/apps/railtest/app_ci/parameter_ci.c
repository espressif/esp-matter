/***************************************************************************//**
 * @file
 * @brief This file implements the parameter commands for RAILtest applications.
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
#include <stdio.h>

#include "response_print.h"

#include "rail.h"
#include "rail_types.h"
#include "app_common.h"
#include "em_core.h"
#include "pa_conversions_efr32.h"

static const char *powerModes[] = RAIL_TX_POWER_MODE_NAMES;

void getChannel(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (RAIL_GetDebugMode(railHandle) & RAIL_DEBUG_MODE_FREQ_OVERRIDE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x12, "Channels are not valid in Debug Mode");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "channel:%d", getLikelyChannel());
  }
}

void setChannel(sl_cli_command_arg_t *args)
{
  uint16_t proposedChannel = sl_cli_get_argument_uint16(args, 0);
  bool success = false;

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  // Make sure this is a valid channel
  if (RAIL_IsValidChannel(railHandle, proposedChannel)
      == RAIL_STATUS_NO_ERROR) {
    changeChannel(proposedChannel);
    success = true;
  }

  if (!success) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x11, "Invalid channel '%d'", proposedChannel);
    return;
  }

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getChannel(args);
}

void setFreqOffset(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  static RAIL_FrequencyOffset_t currentFreqOffset = 0;
  if (sl_cli_get_argument_count(args) >= 1) {
    RAIL_FrequencyOffset_t freqOffset = sl_cli_get_argument_int32(args, 0);

    if ((freqOffset < RAIL_FREQUENCY_OFFSET_MIN)
        || (freqOffset > RAIL_FREQUENCY_OFFSET_MAX)) {
      responsePrintError(sl_cli_get_command_string(args, 0), RAIL_STATUS_INVALID_PARAMETER,
                         "Offset %d outside range of [%d, %d]",
                         freqOffset,
                         RAIL_FREQUENCY_OFFSET_MIN,
                         RAIL_FREQUENCY_OFFSET_MAX);
      return;
    }
    RAIL_Status_t status = RAIL_SetFreqOffset(railHandle, freqOffset);
    if (status != RAIL_STATUS_NO_ERROR) {
      responsePrintError(sl_cli_get_command_string(args, 0), status, "Could not set frequency offset");
      return;
    }
    currentFreqOffset = freqOffset;
  }
  responsePrint(sl_cli_get_command_string(args, 0), "freqOffset:%d", currentFreqOffset);
}

void getPowerConfig(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_TxPowerConfig_t config;
  RAIL_Status_t status = RAIL_GetTxPowerConfig(railHandle, &config);

  responsePrint(sl_cli_get_command_string(args, 0), "success:%s,mode:%s,voltage:%d,rampTime:%d",
                status == RAIL_STATUS_NO_ERROR ? "true" : "false",
                powerModes[config.mode], config.voltage, config.rampTime);
}

void getPowerLimits(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));

  RAIL_TxPowerMode_t powerMode = RAIL_TX_POWER_MODE_NONE;
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;
  if (sl_cli_get_argument_count(args) >= 1) {
    powerMode = sl_cli_get_argument_uint8(args, 0);
  } else {
    RAIL_TxPowerConfig_t config;
    status = RAIL_GetTxPowerConfig(railHandle, &config);
    powerMode = config.mode;
  }
  if (powerMode >= RAIL_TX_POWER_MODE_NONE || status != RAIL_STATUS_NO_ERROR ) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x13,
                       "Invalid PA enum value selected: %d", powerMode);
    return;
  }
  RAIL_TxPowerLevel_t maxPowerlevel = RAIL_TX_POWER_LEVEL_INVALID;
  RAIL_TxPowerLevel_t minPowerlevel = RAIL_TX_POWER_LEVEL_INVALID;
  bool success = RAIL_SupportsTxPowerModeAlt(railHandle, &powerMode,
                                             &maxPowerlevel, &minPowerlevel);
  responsePrint(sl_cli_get_command_string(args, 0),
                "success:%s,powerMode:%s,minPowerLevel:%d,maxPowerLevel:%d",
                success ? "Success" : "Failure",
                powerModes[powerMode], minPowerlevel, maxPowerlevel);
  return;
}

void setPowerConfig(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_TxPowerMode_t mode = sl_cli_get_argument_uint8(args, 0);
  if (mode >= RAIL_TX_POWER_MODE_NONE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x13, "Invalid PA enum value selected: %d", mode);
    return;
  }

  RAIL_TxPowerConfig_t *txPowerConfigPtr;

#if _SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_DUALBAND
#if defined(_SILICON_LABS_32B_SERIES_1)
  if (mode >= RAIL_TX_POWER_MODE_SUBGIG) {
#elif defined(_SILICON_LABS_32B_SERIES_2)
  if (mode >= RAIL_TX_POWER_MODE_SUBGIG_HP) {
#endif
    txPowerConfigPtr = sl_rail_util_pa_get_tx_power_config_subghz();
  } else {
    txPowerConfigPtr = sl_rail_util_pa_get_tx_power_config_2p4ghz();
  }
#elif _SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_2G4HZ
  txPowerConfigPtr = sl_rail_util_pa_get_tx_power_config_2p4ghz();
#elif _SILICON_LABS_EFR32_RADIO_TYPE == _SILICON_LABS_EFR32_RADIO_SUBGHZ
  txPowerConfigPtr = sl_rail_util_pa_get_tx_power_config_subghz();
#endif

  // Make a backup of the TX Power Config before it's changed.
  RAIL_TxPowerConfig_t txPowerConfigBackup = {
    .mode = txPowerConfigPtr->mode,
    .voltage = txPowerConfigPtr->voltage,
    .rampTime = txPowerConfigPtr->rampTime
  };

  uint16_t voltage = sl_cli_get_argument_uint16(args, 1);
  uint16_t rampTime = sl_cli_get_argument_uint16(args, 2);

  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  txPowerConfigPtr->mode = mode;
  txPowerConfigPtr->voltage = voltage;
  txPowerConfigPtr->rampTime = rampTime;
  CORE_EXIT_CRITICAL();

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_ConfigTxPower(railHandle, txPowerConfigPtr);
  // Restore the backup TX Power Config on error.
  if (status != RAIL_STATUS_NO_ERROR) {
    CORE_DECLARE_IRQ_STATE;
    CORE_ENTER_CRITICAL();
    txPowerConfigPtr->mode = txPowerConfigBackup.mode;
    txPowerConfigPtr->voltage = txPowerConfigBackup.voltage;
    txPowerConfigPtr->rampTime = txPowerConfigBackup.rampTime;
    CORE_EXIT_CRITICAL();
  }

  if (status == RAIL_STATUS_NO_ERROR) {
    args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
    getPowerConfig(args);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x26, "Could not set power config");
  }
}

void getPower(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0),
                "powerLevel:%d,power:%d",
                RAIL_GetTxPower(railHandle),
                RAIL_GetTxPowerDbm(railHandle));
}

void setPower(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  bool setPowerError = false;

  if (sl_cli_get_argument_count(args) >= 2 && strcmp(sl_cli_get_argument_string(args, 1), "raw") == 0) {
    RAIL_TxPowerLevel_t rawLevel = sl_cli_get_argument_uint8(args, 0);

    // Set the power and update the RAW level global
    if (RAIL_SetTxPower(railHandle, rawLevel) != RAIL_STATUS_NO_ERROR) {
      setPowerError = true;
    }
  } else {
    RAIL_TxPowerConfig_t tempCfg;
    RAIL_TxPower_t powerDbm = sl_cli_get_argument_int16(args, 0);

    // Set the power in dBm and figure out what RAW level to store based on what
    // was requested NOT what is actually applied to the hardware after limits.
    if ((RAIL_SetTxPowerDbm(railHandle, powerDbm)
         != RAIL_STATUS_NO_ERROR)
        || (RAIL_GetTxPowerConfig(railHandle, &tempCfg)
            != RAIL_STATUS_NO_ERROR)) {
      setPowerError = true;
    }
  }

  if (setPowerError) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x23, "Could not set power.");
  } else {
    // Get and print out the actual applied power and power level
    args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
    getPower(args);
  }
}

void sweepTxPower(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  responsePrint(sl_cli_get_command_string(args, 0), "Sweeping:Started,Instructions:'q' to quit or 'enter' to continue.");
  RAIL_TxPowerConfig_t txPowerConfig;

  RAIL_GetTxPowerConfig(railHandle, &txPowerConfig);

  RAIL_TxPowerLevel_t start = 1;
  RAIL_TxPowerLevel_t end = 255;

  switch (txPowerConfig.mode) {
#ifdef RAIL_TX_POWER_MODE_2P4_HP
    case RAIL_TX_POWER_MODE_2P4_HP:
      start = RAIL_TX_POWER_LEVEL_2P4_HP_MIN;
      end = RAIL_TX_POWER_LEVEL_2P4_HP_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_2P4_MP
    case RAIL_TX_POWER_MODE_2P4_MP:
      start = RAIL_TX_POWER_LEVEL_2P4_MP_MIN;
      end = RAIL_TX_POWER_LEVEL_2P4_MP_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_2P4_LP
    case RAIL_TX_POWER_MODE_2P4_LP:
      start = RAIL_TX_POWER_LEVEL_2P4_LP_MIN;
      end = RAIL_TX_POWER_LEVEL_2P4_LP_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_HP
    case RAIL_TX_POWER_MODE_SUBGIG_HP:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_HP_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_HP_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_MP
    case RAIL_TX_POWER_MODE_SUBGIG_MP:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_MP_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_MP_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_LP
    case RAIL_TX_POWER_MODE_SUBGIG_LP:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_LP_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_LP_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_LLP
    case RAIL_TX_POWER_MODE_SUBGIG_LLP:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_LLP_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_LLP_MAX;
      break;
#endif
#if (defined(RAIL_TX_POWER_MODE_SUBGIG) && defined(_SILICON_LABS_32B_SERIES_1))
    case RAIL_TX_POWER_MODE_SUBGIG:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM
    case RAIL_TX_POWER_MODE_SUBGIG_EFF_30DBM:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_EFF_30DBM_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_EFF_30DBM_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM
    case RAIL_TX_POWER_MODE_SUBGIG_EFF_25DBM:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_EFF_25DBM_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_EFF_25DBM_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM
    case RAIL_TX_POWER_MODE_SUBGIG_EFF_20DBM:
      start = RAIL_TX_POWER_LEVEL_SUBGIG_EFF_20DBM_MIN;
      end = RAIL_TX_POWER_LEVEL_SUBGIG_EFF_20DBM_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_OFDM_PA
    case RAIL_TX_POWER_MODE_OFDM_PA:
      start = RAIL_TX_POWER_LEVEL_OFDM_PA_MIN;
      end = RAIL_TX_POWER_LEVEL_OFDM_PA_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM
    case RAIL_TX_POWER_MODE_OFDM_PA_EFF_30DBM:
      start = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_30DBM_MIN;
      end = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_30DBM_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM
    case RAIL_TX_POWER_MODE_OFDM_PA_EFF_25DBM:
      start = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_25DBM_MIN;
      end = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_25DBM_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM
    case RAIL_TX_POWER_MODE_OFDM_PA_EFF_20DBM:
      start = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_20DBM_MIN;
      end = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_20DBM_MAX;
      break;
#endif
#ifdef RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM
    case RAIL_TX_POWER_MODE_OFDM_PA_EFF_MAXDBM:
      start = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_MAXDBM_MIN;
      end = RAIL_TX_POWER_LEVEL_OFDM_PA_EFF_MAXDBM_MAX;
      break;
#endif
    default:
      responsePrintError(sl_cli_get_command_string(args, 0), 0x21, "PA not configured.");
      return;
  }

  char input;
  RAIL_TxPowerLevel_t i;

  for (i = start; i <= end; i++) {
    responsePrint(sl_cli_get_command_string(args, 0), "PowerLevel:%u", i);
    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
    RAIL_SetTxPower(railHandle, i);
    RAIL_StartTxStream(railHandle, channel, RAIL_STREAM_CARRIER_WAVE);

    input = getchar();

    while (1) {
      if (input == '\n' || input == '\r') {
        break;
      }
      if (input == 'q') {
        responsePrintError(sl_cli_get_command_string(args, 0), 0x20, "Sweep Aborted.");
        RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
        return;
      }
      input = getchar();
    }

    RAIL_Idle(railHandle, RAIL_IDLE_FORCE_SHUTDOWN_CLEAR_FLAGS, true);
  }

  responsePrint(sl_cli_get_command_string(args, 0), "Sweeping:Complete");
}

void getCtune(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint32_t ctune = RAIL_GetTune(railHandle);
#ifdef _SILICON_LABS_32B_SERIES_1
  responsePrint(sl_cli_get_command_string(args, 0), "CTUNE:0x%.3x", ctune);
#else
  responsePrint(sl_cli_get_command_string(args, 0),
                "CTUNEXIANA:0x%.3x,CTUNEXOANA:0x%.3x",
                ctune,
                (ctune + RAIL_GetTuneDelta(railHandle)));
#endif
}

void setCtune(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  if (RAIL_SetTune(railHandle, sl_cli_get_argument_uint32(args, 0))
      != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x28, "Invalid CTUNE value provided");
    return;
  }

  // Read out and print the current CTUNE value
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getCtune(args);
}

void getCtuneDelta(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  int32_t delta = RAIL_GetTuneDelta(railHandle);

  responsePrint(sl_cli_get_command_string(args, 0), "CTuneDelta:%d", delta);
}

void setCtuneDelta(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_SetTuneDelta(railHandle, sl_cli_get_argument_uint32(args, 0));

  // Read out and print the current CTUNE delta value
  getCtuneDelta(args);
}

void setPaCtune(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status;
  uint8_t txVal = sl_cli_get_argument_uint8(args, 0);
  uint8_t rxVal = sl_cli_get_argument_uint8(args, 1);

  status = RAIL_SetPaCTune(railHandle, txVal, rxVal);

  if (status == RAIL_STATUS_NO_ERROR) {
    responsePrint(sl_cli_get_command_string(args, 0), "PACTUNETX:%d,PACTUNERX:%d", txVal, rxVal);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), status, "Error");
  }
}

void enablePaCal(sl_cli_command_arg_t *args)
{
  uint8_t enable = sl_cli_get_argument_uint8(args, 0);
  RAIL_EnablePaCal(enable);
  responsePrint(sl_cli_get_command_string(args, 0), "paCal:%s", (enable ? "Enabled" : "Disabled"));
}

// Helper to convert a string to a RAIL RadioState
static int8_t stringToState(char *string, RAIL_RadioState_t *state)
{
  switch (string[0]) {
    case 'i': case 'I':
      *state =  RAIL_RF_STATE_IDLE;
      break;
    case 'r': case 'R':
      *state =  RAIL_RF_STATE_RX;
      break;
    case 't': case 'T':
      *state =  RAIL_RF_STATE_TX;
      break;
    default:
      return 1;
  }
  return 0;
}

void setTxTransitions(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_RadioState_t states[2];
  if (stringToState(sl_cli_get_argument_string(args, 0), &states[0])
      || stringToState(sl_cli_get_argument_string(args, 1), &states[1])) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x16, "Invalid states");
    return;
  }

  RAIL_StateTransitions_t transitions = {
    .success = states[0],
    .error = states[1]
  };

  RAIL_Status_t ret = RAIL_SetTxTransitions(railHandle, &transitions);
  responsePrint(sl_cli_get_command_string(args, 0), "TxTransitions:%s", (ret ? "Error" : "Set"));
}

void setRxTransitions(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }
  RAIL_RadioState_t states[2];
  if (stringToState(sl_cli_get_argument_string(args, 0), &states[0])
      || stringToState(sl_cli_get_argument_string(args, 1), &states[1])) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x16, "Invalid states");
    return;
  }
  RAIL_StateTransitions_t transitions = {
    .success = states[0],
    .error = states[1]
  };
  RAIL_Status_t ret = RAIL_SetRxTransitions(railHandle, &transitions);
  if (ret == RAIL_STATUS_NO_ERROR) {
    rxSuccessTransition = states[0];
  }
  responsePrint(sl_cli_get_command_string(args, 0), "RxTransitions:%s", (ret ? "Error" : "Set"));
}

void getRxTransitions(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_StateTransitions_t transitions;
  RAIL_Status_t ret = RAIL_GetRxTransitions(railHandle, &transitions);

  if (ret == RAIL_STATUS_NO_ERROR) {
    const char *success = getRfStateName(transitions.success);
    const char *error = getRfStateName(transitions.error);
    responsePrint(sl_cli_get_command_string(args, 0), "Success:%s,Error:%s", success, error);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x28, "Get rx transitions failed");
  }
}

void getTxTransitions(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_StateTransitions_t transitions;
  RAIL_Status_t ret = RAIL_GetTxTransitions(railHandle, &transitions);

  if (ret == RAIL_STATUS_NO_ERROR) {
    const char *success = getRfStateName(transitions.success);
    const char *error = getRfStateName(transitions.error);
    responsePrint(sl_cli_get_command_string(args, 0), "Success:%s,Error:%s", success, error);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x27, "Get tx transitions failed");
  }
}

void setTimings(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_StateTiming_t timings = {
    .idleToRx = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 0),
    .txToRx   = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 1),
    .idleToTx = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 2),
  };
  switch (sl_cli_get_argument_count(args)) {
    default:
    case 7:
      timings.txToTx = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 6);
    // Fall through
    case 6:
      timings.txToRxSearchTimeout = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 5);
    // Fall through
    case 5:
      timings.rxSearchTimeout = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 4);
    // Fall through
    case 4:
      timings.rxToTx = (RAIL_TransitionTime_t)sl_cli_get_argument_int32(args, 3);
    // Fall through
    case 3: // .idleToTx not optional, already set above
    case 2: // .txToRx   not optional, already set above
    case 1: // .idleToRx not optional, already set above
    case 0: // Should be impossible
      break;
  }
  if (!RAIL_SetStateTiming(railHandle, &timings)) {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "IdleToRx:%u,RxToTx:%u,IdleToTx:%u,TxToRx:%u,"
                  "RxSearch:%u,Tx2RxSearch:%u,Tx2Tx:%u",
                  timings.idleToRx, timings.rxToTx, timings.idleToTx,
                  timings.txToRx, timings.rxSearchTimeout,
                  timings.txToRxSearchTimeout, timings.txToTx);
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x18, "Setting timings failed");
  }
}

void setTxFifoThreshold(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (railDataConfig.txMethod != FIFO_MODE) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x19, "Tx is not in FIFO mode");
    return;
  }

  uint16_t txFifoThreshold = sl_cli_get_argument_uint16(args, 0);
  txFifoThreshold = RAIL_SetTxFifoThreshold(railHandle, txFifoThreshold);
  responsePrint(sl_cli_get_command_string(args, 0), "TxFifoThreshold:%d", txFifoThreshold);
}

void setRxFifoThreshold(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint16_t rxFifoThreshold = sl_cli_get_argument_uint16(args, 0);
  rxFifoThreshold = RAIL_SetRxFifoThreshold(railHandle, rxFifoThreshold);
  responsePrint(sl_cli_get_command_string(args, 0), "RxFifoThreshold:%d", rxFifoThreshold);
}

void setEventConfig(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Events_t eventMask = sl_cli_get_argument_uint32(args, 0);
  RAIL_Events_t eventConfig = sl_cli_get_argument_uint32(args, 1);

  if (sl_cli_get_argument_count(args) >= 4) {
    eventMask |= (((RAIL_Events_t)sl_cli_get_argument_uint32(args, 2)) << 32U);
    eventConfig |= (((RAIL_Events_t)sl_cli_get_argument_uint32(args, 3)) << 32U);
  }

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_ConfigEvents(railHandle, eventMask, eventConfig);
  // Avoid use of %ll long-long formats due to iffy printf library support
  if (sl_cli_get_argument_count(args) >= 4) {
    responsePrint(sl_cli_get_command_string(args, 0), "Mask:0x%x%08x,Values:0x%x%08x",
                  (uint32_t)(eventMask >> 32),
                  (uint32_t)eventMask,
                  (uint32_t)(eventConfig >> 32),
                  (uint32_t)eventConfig);
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "Mask:0x%x,Values:0x%x",
                  (uint32_t)eventMask,
                  (uint32_t)eventConfig);
  }
}

void delayUs(sl_cli_command_arg_t *args)
{
  uint32_t delayUs = sl_cli_get_argument_uint32(args, 0);

  // Do not measure any interrupt processing overhead during the delay.
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();

  // Measure the actual delay vs expected.
  uint32_t startTime = RAIL_GetTime();
  RAIL_Status_t status = RAIL_DelayUs(delayUs);
  uint32_t actualDelay = RAIL_GetTime() - startTime;

  CORE_EXIT_CRITICAL();
  responsePrint(sl_cli_get_command_string(args, 0), "Success:%s,ActualDelay:%d",
                status == RAIL_STATUS_NO_ERROR ? "True" : "False",
                actualDelay);
}

#ifdef RAIL_PA_AUTO_MODE
#include "pa_auto_mode.h"
static bool paAutoModeEnable = false;
RAIL_PaAutoModeConfigEntry_t *paAutoModeConfig = NULL;
void configPaAutoMode(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  if (!paAutoModeEnable) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "PA auto mode should be enabled to set configs.");
    return;
  }

  if (sl_cli_get_argument_count(args) % 4U != 1U) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x14,
                       "Insufficient arguments. Must provide min, max, mode and band for each PA auto mode config entry.");
  }

  // uint8_t index = sl_cli_get_argument_uint8(args, 0);
  RAIL_Status_t status = RAIL_STATUS_NO_ERROR;

  uint8_t numOfConfigs = 0U;
  numOfConfigs = (sl_cli_get_argument_count(args) - 1U) / 4U;
  if (numOfConfigs == 0U) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x01,
                       "There are no PA auto mode configs provided.");
    return;
  }
  if (paAutoModeConfig != NULL) {
    free(paAutoModeConfig);
  }
  paAutoModeConfig = (RAIL_PaAutoModeConfigEntry_t *)malloc(numOfConfigs * sizeof(RAIL_PaAutoModeConfigEntry_t));
  if (paAutoModeConfig == NULL) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x01,
                       "The PA auto mode configs are not configured.");
    return;
  }

  for (uint8_t i = 0U; i < numOfConfigs; i++) {
    paAutoModeConfig[i].min = (int16_t)sl_cli_get_argument_int32(args, ((i * 4) + 1));
    paAutoModeConfig[i].max = (int16_t)sl_cli_get_argument_int32(args, ((i * 4) + 2));
    uint8_t mode = (uint8_t)sl_cli_get_argument_int32(args, ((i * 4) + 3));
    // Make sure the mode is valid
    if (mode > RAIL_TX_POWER_MODE_NONE) {
      responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "Invalid mode (%d) specified", mode);
      return;
    }
    paAutoModeConfig[i].mode = mode;
    paAutoModeConfig[i].band = (uint8_t)sl_cli_get_argument_int32(args, ((i * 4) + 4));
  }

  status = RAIL_ConfigPaAutoEntry(railHandle, paAutoModeConfig);
  responsePrint(sl_cli_get_command_string(args, 0), "numberOfPaAutoModeEntries:%d,Success:%s",
                numOfConfigs,
                status == RAIL_STATUS_NO_ERROR ? "True" : "False");
}

void enablePaAutoMode(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  paAutoModeEnable = !!sl_cli_get_argument_uint8(args, 0);
  if ((!paAutoModeEnable) && (paAutoModeConfig != NULL)) {
    free(paAutoModeConfig);
    paAutoModeConfig = NULL;
  }
  RAIL_EnablePaAutoMode(railHandle, paAutoModeEnable);

  responsePrint(sl_cli_get_command_string(args, 0), "enable:%s", paAutoModeEnable ? "True" : "False");
}
#else
void configPaAutoMode(sl_cli_command_arg_t *args)
{
  responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "PA Auto Mode plugin must be enabled to use this feature!");
}

void enablePaAutoMode(sl_cli_command_arg_t *args)
{
  responsePrintError(sl_cli_get_command_string(args, 0), 0x01, "PA Auto Mode plugin must be enabled to use this feature!");
}
#endif

void setRetimeOption(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  uint32_t option = sl_cli_get_argument_uint8(args, 0);
  RAIL_RetimeOptions_t finalRetimeOption;
  RAIL_Status_t status;

  status = RAIL_ConfigRetimeOptions(railHandle,
                                    RAIL_RETIME_OPTIONS_ALL,
                                    option);

  // Report the current enabled status
  if (status == RAIL_STATUS_NO_ERROR) {
    RAIL_GetRetimeOptions(railHandle,
                          &finalRetimeOption);
    responsePrint(sl_cli_get_command_string(args, 0),
#ifdef _SILICON_LABS_32B_SERIES_2
                  "LCD: %s, HFXO: %s, HFRCO: %s, DCDC: %s",
                  ((finalRetimeOption & RAIL_RETIME_OPTION_LCD) != 0U) ? "Enabled" : "Disabled",
#else
                  "HFXO: %s, HFRCO: %s, DCDC: %s",
#endif
                  ((finalRetimeOption & RAIL_RETIME_OPTION_HFXO) != 0U) ? "Enabled" : "Disabled",
                  ((finalRetimeOption & RAIL_RETIME_OPTION_HFRCO) != 0U) ? "Enabled" : "Disabled",
                  ((finalRetimeOption & RAIL_RETIME_OPTION_DCDC) != 0U) ? "Enabled" : "Disabled");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0), "Status:%d", status);
  }
}
