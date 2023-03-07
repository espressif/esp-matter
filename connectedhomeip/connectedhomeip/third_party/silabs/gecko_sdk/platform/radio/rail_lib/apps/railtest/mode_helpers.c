/***************************************************************************//**
 * @file
 * @brief This file contains helpers for transitioning into the various
 *   AppModes
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

#include <stdlib.h>
#include "rail.h"
#include "rail_types.h"
#include "response_print.h"
#include "app_common.h"

// Guard for CI functions to ensure a certain AppMode before running
bool inAppMode(AppMode_t appMode, char *command)
{
  bool ret = (currentAppMode() == appMode);
  if (!ret && command) {
    responsePrintError(command, 0x16,
                       "Need to be in %s mode, not %s, for this command",
                       appModeNames(appMode),
                       appModeNames(currentAppMode()));
  }
  return ret;
}

// Setting semantics for enabling AppMode
void setNextAppMode(AppMode_t next, char *command)
{
  if (next == NONE) {
    enableAppMode(currentAppMode(), false, command);
  } else {
    enableAppMode(next, true, command);
  }
}

// Used when the AppMode is needed to be enabled to continue the function
bool enableAppModeSync(AppMode_t mode, bool enable, char *command)
{
  changeAppModeIfPending();
  enableAppMode(mode, enable, command);
  changeAppModeIfPending();
  // inAppMode shouldn't print, since enableAppMode will cause a print
  return inAppMode(enable ? mode : NONE, NULL);
}

void scheduleNextTx(void)
{
  // Schedule the next tx if there are more coming
  if (txCount > 0 || currentAppMode() == TX_CONTINUOUS) {
    if (enableRandomTxDelay) {
      float randBetween0and1 = ((float) rand()) / (((uint32_t) RAND_MAX) + 1);
      float fTxDelay = continuousTransferPeriod * 1000 * randBetween0and1;
      RAIL_SetTimer(railHandle, (uint32_t) fTxDelay, RAIL_TIME_DELAY, &RAILCb_TimerExpired);
    } else {
      RAIL_SetTimer(railHandle, continuousTransferPeriod * 1000, RAIL_TIME_DELAY, &RAILCb_TimerExpired);
    }
  } else if (currentAppMode() == TX_N_PACKETS || currentAppMode() == TX_SCHEDULED
             || currentAppMode() == TX_UNDERFLOW || currentAppMode() == TX_CANCEL) {
    setNextAppMode(NONE, NULL);
  } else {
    // If we automatically transitioned to SCHTX_AFTER_RX, or don't know how we
    // got here, just finish whatever transmission just happened
    pendFinishTxSequence();
  }
}

void radioTransmit(uint32_t iterations, char *command)
{
  // Do the normal work of just increasing the tx count
  txCount = iterations;

  if (iterations > 0) {
    if (currentAppMode() == TX_UNDERFLOW) {
      setNextAppMode(TX_UNDERFLOW, command); // Get a printout for TX
      pendPacketTx(); // Start transmitting without waiting for the mode change
    } else {
      // Make a change if we're supposed to transmit more packets
      setNextAppMode(TX_N_PACKETS, command);
    }
  } else {
    // A tx count of 0 will stop a continuous or fixed transmit if we're in the
    // middle of one. Otherwise, we'll enable continuous mode.
    if (currentAppMode() == TX_N_PACKETS || currentAppMode() == TX_CONTINUOUS
        || currentAppMode() == TX_UNDERFLOW) {
      setNextAppMode(NONE, command);
    } else {
      setNextAppMode(TX_CONTINUOUS, command);
    }
  }
}
