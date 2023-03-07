/***************************************************************************//**
 * @file
 * @brief This is the states machine for the base test application. It handles
 *   transmit, receive, and various debug modes.
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
#include "response_print.h"
#include "app_common.h"

// At any given time, at least one of (currAppMode, prevAppMode)
// should be NONE, due to the way that setAppModeInternal works
volatile char nextCommandBuf[16];
volatile char *nextCommand;
volatile bool enableMode = true;
volatile AppMode_t nextAppMode = NONE;
volatile AppMode_t currAppMode = NONE;
volatile AppMode_t prevAppMode = NONE;
volatile bool transitionPend = false;
RAIL_TxOptions_t antOptions = RAIL_TX_OPTIONS_DEFAULT;
RAIL_StreamMode_t streamMode = RAIL_STREAM_PN9_STREAM;

AppMode_t currentAppMode(void)
{
  return currAppMode;
}

AppMode_t previousAppMode(void)
{
  return prevAppMode;
}

void enableAppMode(AppMode_t next, bool enable, char *command)
{
  // Should disable current mode instead of enabling NONE
  if (!transitionPend && (next != NONE)) {
    transitionPend = true;
    nextAppMode = next;
    enableMode = enable;
    if (command == NULL) {
      nextCommand = (logLevel & ASYNC_RESPONSE) ? "appMode" : NULL;
    } else {
      memcpy((char *)&nextCommandBuf[0], command, sizeof(nextCommandBuf));
      nextCommand = &nextCommandBuf[0];
    }
  }
}

const char *streamModeNames(RAIL_StreamMode_t streamMode)
{
  char *streamModes[] = { "Tone", "PN9", "10Stream", "TonePhaseNoise", "RampStream", "ToneShifted" };
  return streamModes[streamMode];
}

const char *appModeNames(AppMode_t appMode)
{
  char *appModes[] = { "None", "Stream", "Tone", "ContinuousTx", "DirectMode",
                       "PacketTx", "ScheduledTx", "SchTxAfterRx", "RxOverflow",
                       "TxUnderflow", "TxCancel", "RfSense", "PER", "BER",
                       "ScheduledRx" };
  return appModes[appMode];
}
// Has the logic for disabling and enabling AppMode
//  Note that at least one of (nextAppMode, currAppMode) should be NONE,
//  due to the way we handling AppMode changing
static void transitionAppMode(AppMode_t nextAppMode)
{
  if (currAppMode == NONE && nextAppMode != TX_SCHEDULED) {
    RAIL_CancelTimer(railHandle);
  } else if (currAppMode == TX_STREAM) {
    RAIL_StopTxStream(railHandle);
  } else if (currAppMode == DIRECT) {
    RAIL_EnableDirectModeAlt(railHandle, false, false);
  } else if (currAppMode == TX_CONTINUOUS   || currAppMode == TX_N_PACKETS
             || currAppMode == TX_SCHEDULED || currAppMode == TX_UNDERFLOW
             || currAppMode == TX_CANCEL) {
    // Disable timer just in case
    RAIL_CancelTimer(railHandle);
    txCount = 0;
    pendFinishTxSequence();
  } else if (currAppMode == RF_SENSE) {
    (void) RAIL_StartRfSense(railHandle, RAIL_RFSENSE_OFF, 0, NULL);
  } else if (currAppMode == PER) {
    RAIL_CancelTimer(railHandle);
  } else if (currAppMode == BER) {
    RAIL_Idle(railHandle, RAIL_IDLE_ABORT, false);
  }

  if (nextAppMode == TX_STREAM) {
    RAIL_StartTxStreamAlt(railHandle, channel, streamMode, antOptions);
  } else if (nextAppMode == DIRECT) {
    RAIL_EnableDirectModeAlt(railHandle, true, true);
  } else if (nextAppMode == TX_CONTINUOUS || nextAppMode == TX_N_PACKETS) {
    pendPacketTx();
  } else if (nextAppMode == TX_SCHEDULED || nextAppMode == TX_CANCEL) {
    txCount = 1;
    pendPacketTx();
  } else if (nextAppMode == SCHTX_AFTER_RX || nextAppMode == RX_OVERFLOW) {
    RAIL_StartRx(railHandle, channel, NULL);
  }
  prevAppMode = currAppMode;
  currAppMode = nextAppMode;
}

// This has the rules of when we are allowed to change the AppMode, along
// with the printouts that should happen when a AppMode change happens
static void setAppModeInternal(void)
{
  AppMode_t next = enableMode ? nextAppMode : NONE;
  char *paramNextCommand = (char *)nextCommand;
  AppMode_t paramNextAppMode = nextAppMode;
  AppMode_t paramCurrAppMode = currAppMode;
  bool paramEnableMode = enableMode;
  // TX_STREAM is special-cased to let one switch from one stream mode to
  // another (or the same) without having to first disable the mode.
  if ((paramCurrAppMode == paramNextAppMode)
      && paramEnableMode
      && (paramNextAppMode != TX_STREAM)) {
    if (paramNextCommand) {
      responsePrint(paramNextCommand, "%s:Enabled", appModeNames(next));
    }
  } else if (((paramCurrAppMode == paramNextAppMode)
              && (!paramEnableMode || (paramNextAppMode == TX_STREAM)))
             || ((paramCurrAppMode == NONE) && paramEnableMode)) {
    if (paramNextCommand) {
      if (next == TX_STREAM) {
        if (paramCurrAppMode == TX_STREAM) {
          responsePrint(paramNextCommand,
                        "%s:Enabled,StreamMode:%s,Time:%u",
                        appModeNames(next),
                        streamModeNames(streamMode),
                        RAIL_GetTime());
        } else {
          responsePrint(paramNextCommand,
                        "%s:Enabled,%s:Disabled,StreamMode:%s,Time:%u",
                        appModeNames(next),
                        appModeNames(paramCurrAppMode),
                        streamModeNames(streamMode),
                        RAIL_GetTime());
        }
      } else {
        responsePrint(paramNextCommand,
                      "%s:Enabled,%s:Disabled,Time:%u",
                      appModeNames(next),
                      appModeNames(paramCurrAppMode),
                      RAIL_GetTime());
      }
    }
    transitionAppMode(next);
  } else { // Ignore mode change request
    if (paramNextCommand) {
      responsePrintError(paramNextCommand, 1, "Can't %s %s during %s",
                         paramEnableMode ? "enable" : "disable",
                         appModeNames(paramNextAppMode), appModeNames(paramCurrAppMode));
    }
  }
}

// This should be called from a main loop, to update the AppMode
void changeAppModeIfPending(void)
{
  if (transitionPend) {
    transitionPend = false;
    setAppModeInternal();
  }
}
