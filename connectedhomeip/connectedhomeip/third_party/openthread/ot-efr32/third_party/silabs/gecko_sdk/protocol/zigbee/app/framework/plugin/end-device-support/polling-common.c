/***************************************************************************//**
 * @file
 * @brief Common routines for the Polling plugin, which controls an end device's
 *        polling behavior.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/

#include "app/framework/include/af.h"
#include "app/framework/plugin/end-device-support/end-device-support.h"

// The polling task is used in SoC or on the host if Polling callback is defined.
#if !defined (EZSP_HOST) || defined (ENABLE_POLL_COMPLETED_CALLBACK)
  #define ADD_POLLING_TASK()    emberAfAddToCurrentAppTasks(EMBER_AF_LAST_POLL_GOT_DATA)
  #define REMOVE_POLLING_TASK() emberAfRemoveFromCurrentAppTasks(EMBER_AF_LAST_POLL_GOT_DATA)
  #define REMOVE_SHORT_POLLING_FOR_PARENT_CONNECTIVITY_TASK() emberAfRemoveFromCurrentAppTasks(EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY)
#else
  #define ADD_POLLING_TASK()
  #define REMOVE_POLLING_TASK()
  #define REMOVE_SHORT_POLLING_FOR_PARENT_CONNECTIVITY_TASK()
#endif

EmAfPollingState emAfPollingStates[EMBER_SUPPORTED_NETWORKS];

// This function is called when a poll completes and explains what happend with
// the poll.  If the number of sequential data polls not ACKed by the parent
// exceeds the threshold, we will try to find a new parent.
void emAfPollCompleteHandler(EmberStatus status, uint8_t limit)
{
  EmAfPollingState *state;
  uint8_t networkIndex;

  (void) emberAfPushCallbackNetworkIndex();
  networkIndex = emberGetCurrentNetwork();
  state = &emAfPollingStates[networkIndex];

  if (emAfEnablePollCompletedCallback) {
    emberAfPluginEndDeviceSupportPollCompletedCallback(status);
  }

  switch (status) {
    case EMBER_SUCCESS:
      REMOVE_SHORT_POLLING_FOR_PARENT_CONNECTIVITY_TASK();
      ADD_POLLING_TASK();
      emberAfDebugPrintln("poll nwk %d: got data", networkIndex);
      state->numPollsFailing = 0;
      break;
    case EMBER_MAC_NO_DATA:
      REMOVE_SHORT_POLLING_FOR_PARENT_CONNECTIVITY_TASK();
      REMOVE_POLLING_TASK();
      emberAfDebugPrintln("poll nwk %d: no data", networkIndex);
      state->numPollsFailing = 0;
      break;
    case EMBER_PHY_TX_BLOCKED:
    case EMBER_PHY_TX_SCHED_FAIL:
    case EMBER_PHY_TX_CCA_FAIL:
      // This means the air was busy, which we don't count as a failure.
      REMOVE_POLLING_TASK();
      emberAfDebugPrintln("poll nwk %d: channel is busy", networkIndex);
      break;
    case EMBER_MAC_NO_ACK_RECEIVED:
      // If we are performing key establishment, we can ignore this since the
      // parent could go away for long periods of time while doing ECC processes.
      if (emberAfPerformingKeyEstablishment()) {
        break;
      }
      // Count failures until we hit the limit, then we try a rejoin. If rejoin
      // fails, it will trigger a move.
      state->numPollsFailing++;
      REMOVE_POLLING_TASK();
      emberAfDebugPrintln("Number of short poll attempts made to resolve parent connectivity: %d", state->numPollsFailing);
      emberAfAddToCurrentAppTasksCallback(EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY);
      if (limit <= state->numPollsFailing) {
        if (!emberAfPluginEndDeviceSupportLostParentConnectivityCallback()) {
          emberAfStartMoveCallback();
        }
      }
      break;
    default:
      emberAfDebugPrintln("poll nwk %d: 0x%x", networkIndex, status);
  }

  (void) emberAfPopNetworkIndex();
}
