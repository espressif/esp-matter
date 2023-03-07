/***************************************************************************//**
 * @file
 * @brief SoC routines for the Polling plugin, which controls an end device's
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

#ifdef UC_BUILD
#include "end-device-support-config.h"
#endif

// *****************************************************************************
// Globals

#ifdef UC_BUILD
sl_zigbee_event_t emberAfPluginEndDeviceSupportPollingNetworkEvents[EMBER_SUPPORTED_NETWORKS];
void emberAfPluginEndDeviceSupportPollingNetworkEventHandler(SLXU_UC_EVENT);
#else
extern EmberEventControl emberAfPluginEndDeviceSupportPollingNetworkEventControls[];
#endif

// *****************************************************************************
// Functions

void emAfPluginEndDeviceSupportPollingInit(void)
{
  slxu_zigbee_network_event_init(emberAfPluginEndDeviceSupportPollingNetworkEvents,
                                 emberAfPluginEndDeviceSupportPollingNetworkEventHandler);
}

// This is called to scheduling polling events for the network(s).  We only
// care about end device networks.  For each of those, a polling event will be
// scheduled for joined networks or canceled otherwise.
void emberAfPluginEndDeviceSupportTickCallback(void)
{
  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    (void) emberAfPushNetworkIndex(i);
    if (emAfProIsCurrentNetwork()
        && EMBER_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType) {
      if (emberAfNetworkState() == EMBER_JOINED_NETWORK) {
        EmAfPollingState *state = &emAfPollingStates[i];
        uint32_t lastPollIntervalMs = state->pollIntervalMs;
        state->pollIntervalMs = emberAfGetCurrentPollIntervalMsCallback();
        if (state->pollIntervalMs != lastPollIntervalMs
#ifdef UC_BUILD
            || !sl_zigbee_event_is_scheduled(emberAfPluginEndDeviceSupportPollingNetworkEvents)) {
#else
            || !emberAfNetworkEventControlGetActive(emberAfPluginEndDeviceSupportPollingNetworkEventControls)) {
#endif
          emberAfDebugPrintln("Next poll nwk %d in %l ms",
                              i,
                              state->pollIntervalMs);
#ifdef UC_BUILD
          sl_zigbee_event_set_delay_ms(emberAfPluginEndDeviceSupportPollingNetworkEvents,
                                       state->pollIntervalMs);
#else
          emberAfNetworkEventControlSetDelayMS(emberAfPluginEndDeviceSupportPollingNetworkEventControls,
                                               state->pollIntervalMs);
#endif
        }
      } else {
#ifdef UC_BUILD
        sl_zigbee_event_set_inactive(emberAfPluginEndDeviceSupportPollingNetworkEvents);
#else
        emberAfNetworkEventControlSetInactive(emberAfPluginEndDeviceSupportPollingNetworkEventControls);
#endif
      }
    }
    (void) emberAfPopNetworkIndex();
  }
}

// Whenever the polling event fires for a network, a MAC data poll is sent.
void emberAfPluginEndDeviceSupportPollingNetworkEventHandler(SLXU_UC_EVENT)
{
  EmberNetworkStatus state = emberAfNetworkState();
  if (state == EMBER_JOINED_NETWORK) {
    EmberStatus status = emberPollForData();
    if (status != EMBER_SUCCESS) {
      emberAfCorePrintln("poll nwk %d: 0x%x", emberGetCurrentNetwork(), status);
    }
  }
}

// This function is called when a poll completes and explains what happend with
// the poll.  If the number of sequential data polls not ACKed by the parent
// exceeds the threshold, we will try to find a new parent.
#ifdef UC_BUILD
void emAfPluginEndDeviceSupportPollCompleteCallback(EmberStatus status)
#else
void emberPollCompleteHandler(EmberStatus status)
#endif
{
  emAfPollCompleteHandler(status,
                          EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_MAX_MISSED_POLLS);
}
