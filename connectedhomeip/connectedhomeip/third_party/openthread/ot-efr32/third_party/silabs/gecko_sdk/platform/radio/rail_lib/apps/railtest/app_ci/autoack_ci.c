/***************************************************************************//**
 * @file
 * @brief This file implements the autoack commands in RAILtest apps.
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

#include "rail.h"
#include "app_common.h"

static RAIL_AutoAckConfig_t config;

void autoAckConfig(sl_cli_command_arg_t *args)
{
  config.enable = true;

  if (!inRadioState(RAIL_RF_STATE_IDLE, sl_cli_get_command_string(args, 0))) {
    return;
  }

  if (memcmp(sl_cli_get_argument_string(args, 0), "idle", 4) == 0) {
    config.rxTransitions.success = RAIL_RF_STATE_IDLE;
    config.txTransitions.success = RAIL_RF_STATE_IDLE;
  } else if (memcmp(sl_cli_get_argument_string(args, 0), "rx", 2) == 0) {
    config.rxTransitions.success = RAIL_RF_STATE_RX;
    config.txTransitions.success = RAIL_RF_STATE_RX;
  } else {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x20, "Unknown auto ack default state.");
    return;
  }

  RAIL_Status_t status;
  RAIL_TransitionTime_t timing;
  RAIL_StateTiming_t timings = { 0U, };

  timing = (RAIL_TransitionTime_t)sl_cli_get_argument_uint16(args, 1);
  if (timing > RAIL_MAXIMUM_TRANSITION_US) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x21, "Invalid idle timing.");
    return;
  } else {
    timings.idleToTx = timing;
    timings.idleToRx = timing;
  }

  timing = (RAIL_TransitionTime_t)sl_cli_get_argument_uint16(args, 2);
  if (timing > RAIL_MAXIMUM_TRANSITION_US) {
    responsePrintError(sl_cli_get_command_string(args, 0), 0x22, "Invalid turnaround timing");
    return;
  } else {
    timings.rxToTx = timing;
    // Make txToRx a little lower than desired. See documentation
    // on RAIL_ConfigAutoAck.
    if (timing > 10) {
      timings.txToRx = timing - 10U;
    } else {
      timings.txToRx = 0U;
    }
  }

  timing = sl_cli_get_argument_uint16(args, 3);
  config.ackTimeout = timing;

  RAIL_SetStateTiming(railHandle, &timings);
  status = RAIL_ConfigAutoAck(railHandle, &config);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), status, "Call to RAIL_ConfigAutoAck returned an error");
  } else {
    responsePrint(sl_cli_get_command_string(args, 0),
                  "rxDefaultState:%s,"
                  "txDefaultState:%s,"
                  "idleTiming:%u,"
                  "turnaroundTime:%u,"
                  "ackTimeout:%u",
                  getRfStateName(config.rxTransitions.success),
                  getRfStateName(config.txTransitions.success),
                  timings.idleToTx,
                  timings.rxToTx,
                  config.ackTimeout);
  }
}

void getAutoAck(sl_cli_command_arg_t *args)
{
  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  bool filteringEnabled = RAIL_IsAutoAckEnabled(railHandle);
  bool rxPaused = RAIL_IsRxAutoAckPaused(railHandle);
  bool txPaused = RAIL_IsTxAutoAckPaused(railHandle);

  responsePrint(sl_cli_get_command_string(args, 0),
                "AutoAck:%s,"
                "RxAutoAckStatus:%s,"
                "TxAutoAckStatus:%s",
                filteringEnabled ? "Enabled" : "Disabled",
                rxPaused ? "Paused" : "Unpaused",
                txPaused ? "Paused" : "Unpaused");
}

void autoAckDisable(sl_cli_command_arg_t *args)
{
  RAIL_StateTransitions_t transitions = {
    .success = RAIL_RF_STATE_IDLE,
    .error = RAIL_RF_STATE_IDLE
  };

  RAIL_AutoAckConfig_t autoAckConfig = {
    false,
    0,
    transitions,
    transitions
  };

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));
  RAIL_Status_t status = RAIL_ConfigAutoAck(railHandle, &autoAckConfig);
  if (status != RAIL_STATUS_NO_ERROR) {
    responsePrintError(sl_cli_get_command_string(args, 0), status, "Error disabling AutoAck");
  }
  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getAutoAck(args);
}

void autoAckPause(sl_cli_command_arg_t *args)
{
  uint8_t rxPause = sl_cli_get_argument_uint8(args, 0);
  uint8_t txPause = sl_cli_get_argument_uint8(args, 1);

  CHECK_RAIL_HANDLE(sl_cli_get_command_string(args, 0));

  RAIL_PauseRxAutoAck(railHandle, !!rxPause);

  RAIL_PauseTxAutoAck(railHandle, !!txPause);

  args->argc = sl_cli_get_command_count(args); /* only reference cmd str */
  getAutoAck(args);
}

void setTxAckOptions(sl_cli_command_arg_t *args)
{
  uint8_t cancelAck = sl_cli_get_argument_uint8(args, 0);
  uint8_t useTxBuffer = sl_cli_get_argument_uint8(args, 1);

  afterRxCancelAck = ((cancelAck != 0) ? true : false);
  afterRxUseTxBufferForAck = ((useTxBuffer != 0) ? true : false);

  responsePrint(sl_cli_get_command_string(args, 0),
                "CancelAck:%s,"
                "UseTxBuffer:%s",
                afterRxCancelAck ? "True" : "False",
                afterRxUseTxBufferForAck ? "True" : "False");
}
