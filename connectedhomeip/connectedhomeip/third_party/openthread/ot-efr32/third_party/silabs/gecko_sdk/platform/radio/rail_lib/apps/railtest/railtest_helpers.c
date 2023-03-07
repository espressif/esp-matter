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

#include "app_common.h"

#include "response_print.h"
#include "buffer_pool_allocator.h"

#include "rail.h"
#include "em_core.h"
#include "em_system.h"

const char *getRfStateName(RAIL_RadioState_t state)
{
  switch (state) {
    case RAIL_RF_STATE_IDLE:
      return "Idle";
    case RAIL_RF_STATE_RX:
      return "Rx";
    case RAIL_RF_STATE_TX:
      return "Tx";
    case RAIL_RF_STATE_RX_ACTIVE:
      return "RxActive";
    case RAIL_RF_STATE_TX_ACTIVE:
      return "TxActive";
    case RAIL_RF_STATE_INACTIVE:
      return "Inactive";
    default:
      //Check individual rail state bits if RAIL state is unknown
      return "Unknown";
  }
}

// Note this function should be supplied with a buffer of minimum char size 23
// to avoid potential RAM corruption for worse case (though invalid) 22 character
// string "ToIdleActiveNoFrameLbt" + string termination character.
char *getRfStateDetailName(RAIL_RadioStateDetail_t state, char *buffer)
{
  if (state == RAIL_RF_STATE_DETAIL_INACTIVE) {
    strcpy(buffer, "Inactive");
    return buffer;
  }
  buffer[0] = '\0';
  if ((state & RAIL_RF_STATE_DETAIL_TRANSITION) != 0) {
    strcpy(buffer, "To");
  }
  switch (state & RAIL_RF_STATE_DETAIL_CORE_STATE_MASK) {
    case RAIL_RF_STATE_DETAIL_IDLE_STATE:
      strcat(buffer, "Idle");
      break;
    case RAIL_RF_STATE_DETAIL_RX_STATE:
      strcat(buffer, "Rx");
      break;
    case RAIL_RF_STATE_DETAIL_TX_STATE:
      strcat(buffer, "Tx");
      break;
    default:
      // If we reach here and either no core radio state bits are set,
      // or multiple core radio state bits are set, then unknown state.
      strcpy(buffer, "Unknown");
      return buffer;
  }
  if ((state & RAIL_RF_STATE_DETAIL_ACTIVE) != 0) {
    strcat(buffer, "Active");
  }
  if ((state & RAIL_RF_STATE_DETAIL_NO_FRAMES) != 0) {
    strcat(buffer, "NoFrame");
  }
  if ((state & RAIL_RF_STATE_DETAIL_LBT) != 0) {
    strcat(buffer, "Lbt");
  }
  return buffer;
}

// Guard for CI functions to ensure a certain radio state before running
bool inRadioState(RAIL_RadioState_t state, char *command)
{
  RAIL_RadioState_t currentState = RAIL_GetRadioState(railHandle);
  bool ret;
  switch (state) {
    case RAIL_RF_STATE_INACTIVE:  // Match exactly
    case RAIL_RF_STATE_TX_ACTIVE: // Match exactly
    case RAIL_RF_STATE_RX_ACTIVE: // Match exactly
      ret = (currentState == state);
      break;
    case RAIL_RF_STATE_IDLE:      // Match IDLE or INACTIVE
      ret = (currentState <= RAIL_RF_STATE_IDLE);
      break;
    case RAIL_RF_STATE_RX:        // Match RX or RX_ACTIVE
    case RAIL_RF_STATE_TX:        // Match TX or TX_ACTIVE
      ret = ((currentState & state) == state);
      break;
    default:                      // Illegal state!?
      ret = false;
      break;
  }
  if (!ret && command) {
    responsePrintError(command, 0x17,
                       "Need to be in %s radio state for this command",
                       getRfStateName(state));
  }
  return ret;
}

const char *configuredRxAntenna(RAIL_RxOptions_t rxOptions)
{
  switch (rxOptions & (RAIL_RX_OPTION_ANTENNA_AUTO)) {
    case (RAIL_RX_OPTION_ANTENNA_AUTO): {
      return "Auto";
      break;
    }
    case (RAIL_RX_OPTION_ANTENNA0): {
      return "Antenna0";
      break;
    }
    case (RAIL_RX_OPTION_ANTENNA1): {
      return "Antenna1";
      break;
    }
    default: {
      return "Any";
      break;
    }
  }
}

void printRailAppEvents(void)
{
  // Print any newly received app events
  if (!queueIsEmpty(&railAppEventQueue)) {
    void *railtestEventHandle = queueRemove(&railAppEventQueue);
    RailAppEvent_t *railtestEvent =
      (RailAppEvent_t*) memoryPtrFromHandle(railtestEventHandle);
    // Print the received packet and appended info
    if (railtestEvent != NULL) {
      if (railtestEvent->type == RX_PACKET) {
        char *cmdName;
        uint8_t *dataPtr = NULL;
        switch (railtestEvent->rxPacket.packetStatus) {
          case RAIL_RX_PACKET_ABORT_FORMAT:
            cmdName = "rxErrFmt";
            break;
          case RAIL_RX_PACKET_ABORT_FILTERED:
            cmdName = "rxErrFlt";
            break;
          case RAIL_RX_PACKET_ABORT_ABORTED:
            cmdName = "rxErrAbt";
            break;
          case RAIL_RX_PACKET_ABORT_OVERFLOW:
            cmdName = "rxErrOvf";
            break;
          case RAIL_RX_PACKET_ABORT_CRC_ERROR:
            cmdName = "rxErrCrc";
            break;
          case RAIL_RX_PACKET_READY_CRC_ERROR:
          case RAIL_RX_PACKET_READY_SUCCESS:
            cmdName = "rxPacket";
            dataPtr = railtestEvent->rxPacket.dataPtr;
            break;
          default:
            cmdName = "rxErr???";
            break;
        }
        printPacket(cmdName,
                    dataPtr,
                    railtestEvent->rxPacket.dataLength,
                    &railtestEvent->rxPacket);
      } else if (railtestEvent->type == BEAM_PACKET) {
        // Now print the most recent packet we may have received in Z-Wave mode
        responsePrint("ZWaveBeamFrame", "nodeId:0x%x,channelHopIdx:%d,lrBeam:%s,lrBeamTxPower:%d,beamRssi:%d",
                      railtestEvent->beamPacket.nodeId, railtestEvent->beamPacket.channelIndex,
                      (railtestEvent->beamPacket.lrBeamTxPower == RAIL_ZWAVE_LR_BEAM_TX_POWER_INVALID)
                      ? "No" : "Yes",
                      railtestEvent->beamPacket.lrBeamTxPower,
                      railtestEvent->beamPacket.beamRssi);
      } else if (railtestEvent->type == RAIL_EVENT) {
        printRailEvents(&railtestEvent->railEvent);
      } else if (railtestEvent->type == MULTITIMER) {
        responsePrint("multiTimerCb",
                      "TimerExpiredCallback:%u,"
                      "ConfiguredExpireTime:%u,"
                      "MultiTimerIndex:%d",
                      railtestEvent->multitimer.currentTime,
                      railtestEvent->multitimer.expirationTime,
                      railtestEvent->multitimer.index);
      } else if (railtestEvent->type == AVERAGE_RSSI) {
        char bufAverageRssi[10];
        averageRssi = (float)railtestEvent->rssi.rssi / 4;
        if (railtestEvent->rssi.rssi == RAIL_RSSI_INVALID) {
          responsePrint("getAvgRssi", "Could not read RSSI.");
        } else {
          sprintfFloat(bufAverageRssi, sizeof(bufAverageRssi), averageRssi, 2);
          responsePrint("getAvgRssi", "rssi:%s", bufAverageRssi);
        }
      }
#if RAIL_IEEE802154_SUPPORTS_G_MODESWITCH && defined(WISUN_MODESWITCHPHRS_ARRAY_SIZE)
      else if (railtestEvent->type == MODE_SWITCH_CHANGE_CHANNEL) {
        responsePrint("modeSwitchChangeChannel", "channel:%u",
                      railtestEvent->modeSwitchChangeChannel.channel);
      }
#endif
      memoryFree(railtestEventHandle);
    }
  }
  uint32_t eventsMissedCache = 0;
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();
  eventsMissedCache = eventsMissed;
  eventsMissed = 0;
  CORE_EXIT_CRITICAL();
  if (eventsMissedCache > 0) {
    responsePrint("missedRailAppEvents", "count:%u", eventsMissedCache);
  }
}

void printChipInfo(void)
{
#define EVALIT(a, b)  a # b
#define PASTEIT(a, b) EVALIT(a, b)
#if defined(_SILICON_LABS_32B_SERIES_1)
 #if   (_SILICON_LABS_32B_SERIES_1_CONFIG == 1)
  #define FAMILY_NAME "EFR32XG1"
 #else
  #define FAMILY_NAME PASTEIT("EFR32XG1", _SILICON_LABS_32B_SERIES_1_CONFIG)
 #endif
#elif defined(_SILICON_LABS_32B_SERIES_2)
  #define FAMILY_NAME PASTEIT("EFR32XG2", _SILICON_LABS_32B_SERIES_2_CONFIG)
#else
  #define FAMILY_NAME "??"
#endif
#ifndef _SILICON_LABS_GECKO_INTERNAL_SDID
  #define _SILICON_LABS_GECKO_INTERNAL_SDID 0U
#endif

#if defined(_SILICON_LABS_32B_SERIES_2)
  uint32_t moduleName32[8] = {
    DEVINFO->MODULENAME0, DEVINFO->MODULENAME1, DEVINFO->MODULENAME2,
    DEVINFO->MODULENAME3, DEVINFO->MODULENAME4, DEVINFO->MODULENAME5,
    DEVINFO->MODULENAME6, 0UL
  };
  char *moduleName = (char *) moduleName32;
  for (uint8_t i = 0U; i < sizeof(moduleName32); i++) {
    if ((moduleName[i] == '\0') || (moduleName[i] == '\xFF')) {
      moduleName[i] = '\0';
      break;
    }
  }
  responsePrint("radio", "FreqHz:%u,ModuleInfo:0x%08x,ModuleName:%s",
                RAIL_GetRadioClockFreqHz(railHandle),
                DEVINFO->MODULEINFO,
                ((moduleName[0] == '\0') ? "N/A" : moduleName));
#else
  responsePrint("radio", "FreqHz:%u,ModuleInfo:0x%08x",
                RAIL_GetRadioClockFreqHz(railHandle),
                DEVINFO->MODULEINFO);
#endif
  SYSTEM_ChipRevision_TypeDef chipRev = { 0, };
  SYSTEM_ChipRevisionGet(&chipRev);

  // SYSTEM_ChipRevision_TypeDef defines either a partNumber field or a family field.
  // If _SYSCFG_CHIPREV_PARTNUMBER_MASK is defined, there is a partNumber field.
  // Otherwise, there is a family field.
  responsePrint("system", "Family:%s,"
#if defined(_SYSCFG_CHIPREV_PARTNUMBER_MASK)
                "Part#:%u,"
#else
                "Fam#:%u,"
#endif
                "ChipRev:%u.%u,sdid:%u,Part:0x%08x",
                FAMILY_NAME,
#if defined(_SYSCFG_CHIPREV_PARTNUMBER_MASK)
                chipRev.partNumber,
#else
                chipRev.family,
#endif
                chipRev.major,
                chipRev.minor,
                _SILICON_LABS_GECKO_INTERNAL_SDID,
                DEVINFO->PART);
}
