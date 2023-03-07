/***************************************************************************//**
 * @file
 * @brief Host routines for the Polling plugin, which controls an end device's
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
#include "app/framework/util/af-event.h"
#include "app/framework/plugin/end-device-support/end-device-support.h"

// *****************************************************************************
// Globals

static uint8_t numPollsFailingLimit;
static bool enablePollCompletedCallback;

// *****************************************************************************
// Functions

#ifndef UC_BUILD
// Stub no longer needed for UC
void emAfPluginEndDeviceSupportPollingInit(void)
{
}
#endif // UC_BUILD

// This is called to scheduling polling events for the network(s).  We only
// care about end device networks.  For each of those, the NCP will be told to
// poll for joined networks or not to poll otherwise.
void emberAfPluginEndDeviceSupportTickCallback(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    (void) emberAfPushNetworkIndex(i);
    if (emAfProIsCurrentNetwork()
        && EMBER_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
      EmAfPollingState *state = &emAfPollingStates[i];
      uint32_t lastPollIntervalMs = state->pollIntervalMs;
      if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
        state->pollIntervalMs = emberAfGetCurrentPollIntervalMsCallback();
      } else {
        state->pollIntervalMs = 0;
      }

      // schedule for poll when following attr changes state:
      // 1) poll interval
      // 2) enablePollCompletedCallback
      if (state->pollIntervalMs != lastPollIntervalMs
          || emAfEnablePollCompletedCallback != enablePollCompletedCallback) {
        EmberStatus status;
        uint16_t duration;
        EmberEventUnits units;
        uint8_t ncpFailureLimit;

        enablePollCompletedCallback = emAfEnablePollCompletedCallback;
        if (emAfEnablePollCompletedCallback) {
          ncpFailureLimit = 0;
          numPollsFailingLimit = EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS;
        } else {
          ncpFailureLimit = EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS;
          numPollsFailingLimit = 0;
        }

        emAfGetTimerDurationAndUnitFromMS(state->pollIntervalMs,
                                          &duration,
                                          &units);
        status = ezspPollForData(duration, units, ncpFailureLimit);
        if (status != EMBER_SUCCESS) {
          emberAfCorePrintln("poll nwk %d: 0x%x", i, status);
        } else {
          emberAfCorePrintln("Next poll nwk %d in %l ms",
                             i,
                             state->pollIntervalMs);
        }
      }
    }
    (void) emberAfPopNetworkIndex();
  }
}

// The NCP schedules and manages polling, so we do not schedule our own events
// and therefore this handler should never fire.
void emberAfPluginEndDeviceSupportPollingNetworkEventHandler(void)
{
}

// This function is called when a poll completes and explains what happend with
// the poll.  If no ACKs are received from the parent, we will try to find a
// new parent.
#ifdef UC_BUILD
void emAfPluginEndDeviceSupportPollCompleteCallback(EmberStatus status)
#else
void ezspPollCompleteHandler(EmberStatus status)
#endif
{
  emAfPollCompleteHandler(status, numPollsFailingLimit);
}

void emberAfPreNcpResetCallback(void)
{
  // Reset the poll intervals so the NCP will be instructed to poll if
  // necessary.
  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    emAfPollingStates[i].pollIntervalMs = 0;
    emAfPollingStates[i].numPollsFailing = 0;
  }
  numPollsFailingLimit = EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS;
  enablePollCompletedCallback = false;
}
