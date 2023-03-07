/***************************************************************************//**
 * @file
 * @brief Code common to SOC and host to handle managing polling
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
#ifndef UC_BUILD
#include "callback.h"
#endif
#include "app/framework/util/util.h"
#include "app/framework/util/common.h"
#include "app/framework/util/af-event.h"

#include "app/framework/plugin/end-device-support/end-device-support.h"

#if defined(EMBER_SCRIPTED_TEST)
  #include "app/framework/util/af-event-test.h"

  #define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_LONG_POLL_INTERVAL_SECONDS 15
  #define EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_SHORT_POLL_INTERVAL_SECONDS 1
#endif

// *****************************************************************************
// Globals

typedef struct {
  EmberAfApplicationTask currentAppTasks;
  EmberAfApplicationTask wakeTimeoutBitmask;
  uint32_t longPollIntervalMs;
  bool longPollIntervalAlreadySet;
  uint16_t shortPollIntervalMs;
  bool shortPollIntervalAlreadySet;
  uint16_t wakeTimeoutMs;
  uint16_t lastAppTaskScheduleTime;
  EmberAfEventPollControl pollControl;
} State;
static State states[EMBER_SUPPORTED_NETWORKS] = { { 0 } };

#if defined(ENABLE_POLL_COMPLETED_CALLBACK)
  #define ENABLE_POLL_COMPLETED_CALLBACK_DEFAULT true
#else
  #define ENABLE_POLL_COMPLETED_CALLBACK_DEFAULT false
#endif
bool emAfEnablePollCompletedCallback = ENABLE_POLL_COMPLETED_CALLBACK_DEFAULT;

#ifndef EMBER_AF_HAS_END_DEVICE_NETWORK
  #error "End device support only allowed on end devices."
#endif

// *****************************************************************************
// Functions

void emberAfPluginEndDeviceSupportInitCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    states[i].currentAppTasks = 0;
    states[i].wakeTimeoutBitmask = EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_BITMASK;
    if (false == states[i].longPollIntervalAlreadySet) {
      states[i].longPollIntervalMs =
        (EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_LONG_POLL_INTERVAL_SECONDS
         * MILLISECOND_TICKS_PER_SECOND);
    }
    if (false == states[i].shortPollIntervalAlreadySet) {
      states[i].shortPollIntervalMs =
        (EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_SHORT_POLL_INTERVAL_SECONDS
         * MILLISECOND_TICKS_PER_SECOND);
    }
    states[i].wakeTimeoutMs = (EMBER_AF_PLUGIN_END_DEVICE_SUPPORT_WAKE_TIMEOUT_SECONDS
                               * MILLISECOND_TICKS_PER_SECOND);
    states[i].lastAppTaskScheduleTime = 0;
    states[i].pollControl = EMBER_AF_LONG_POLL;
  }

#ifndef UC_BUILD
  // In UC these are templated directly to the event_init context.
  emAfPluginEndDeviceSupportMoveInit();
  emAfPluginEndDeviceSupportPollingInit();
#endif // !UC_BUILD
}

void emberAfAddToCurrentAppTasksCallback(EmberAfApplicationTask tasks)
{
  if (emAfProIsCurrentNetwork()
      && (EMBER_SLEEPY_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType
          || tasks == EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY)) {
    // Allow short poll on non-sleepy/EMBER_END_DEVICE as well only if device is
    // in EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY task.
    State *state = &states[emberGetCurrentNetwork()];
    state->currentAppTasks |= tasks;
    if (tasks & state->wakeTimeoutBitmask) {
      state->lastAppTaskScheduleTime = halCommonGetInt16uMillisecondTick();
    }
  }
}

void emberAfRemoveFromCurrentAppTasksCallback(EmberAfApplicationTask tasks)
{
  State *state = &states[emberGetCurrentNetwork()];
  state->currentAppTasks &= (~tasks);
}

uint32_t emberAfGetCurrentAppTasksCallback(void)
{
  State *state = &states[emberGetCurrentNetwork()];
  return state->currentAppTasks;
}

uint32_t emberAfGetLongPollIntervalMsCallback(void)
{
  State *state = &states[emberGetCurrentNetwork()];
  return state->longPollIntervalMs;
}

uint32_t emberAfGetLongPollIntervalQsCallback(void)
{
  return (emberAfGetLongPollIntervalMsCallback()
          / MILLISECOND_TICKS_PER_QUARTERSECOND);
}

void emberAfSetLongPollIntervalMsCallback(uint32_t longPollIntervalMs)
{
  State *state = &states[emberGetCurrentNetwork()];
  state->longPollIntervalMs = longPollIntervalMs;
  state->longPollIntervalAlreadySet = true;
}

void emberAfSetLongPollIntervalQsCallback(uint32_t longPollIntervalQs)
{
  emberAfSetLongPollIntervalMsCallback(longPollIntervalQs
                                       * MILLISECOND_TICKS_PER_QUARTERSECOND);
}

uint16_t emberAfGetShortPollIntervalMsCallback(void)
{
  State *state = &states[emberGetCurrentNetwork()];
  return state->shortPollIntervalMs;
}

uint16_t emberAfGetShortPollIntervalQsCallback(void)
{
  return (emberAfGetShortPollIntervalMsCallback()
          / MILLISECOND_TICKS_PER_QUARTERSECOND);
}

void emberAfSetShortPollIntervalMsCallback(uint16_t shortPollIntervalMs)
{
  State *state = &states[emberGetCurrentNetwork()];
  state->shortPollIntervalMs = shortPollIntervalMs;
  state->shortPollIntervalAlreadySet = true;
}

void emberAfSetShortPollIntervalQsCallback(uint16_t shortPollIntervalQs)
{
  emberAfSetShortPollIntervalMsCallback(shortPollIntervalQs
                                        * MILLISECOND_TICKS_PER_QUARTERSECOND);
}

#ifdef EZSP_HOST
  #define emberOkToLongPoll() true
#endif

uint32_t emberAfGetCurrentPollIntervalMsCallback(void)
{
  if (emAfProIsCurrentNetwork()
      && (EMBER_SLEEPY_END_DEVICE <= emAfCurrentZigbeeProNetwork->nodeType
          || (emberAfGetCurrentAppTasksCallback() & EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY))) {
    // Allow short poll on non-sleepy/EMBER_END_DEVICE as well only if device is
    // in EMBER_AF_FORCE_SHORT_POLL_FOR_PARENT_CONNECTIVITY task.
    State *state = &states[emberGetCurrentNetwork()];
    if (elapsedTimeInt16u(state->lastAppTaskScheduleTime,
                          halCommonGetInt16uMillisecondTick())
        > state->wakeTimeoutMs) {
      state->currentAppTasks &= ~state->wakeTimeoutBitmask;
    }
    if (!emberOkToLongPoll()
        || state->currentAppTasks != 0
        || emberAfGetCurrentPollControlCallback() == EMBER_AF_SHORT_POLL) {
      return emberAfGetShortPollIntervalMsCallback();
    }
  }
  return emberAfGetLongPollIntervalMsCallback();
}

uint32_t emberAfGetCurrentPollIntervalQsCallback(void)
{
  return (emberAfGetCurrentPollIntervalMsCallback()
          / MILLISECOND_TICKS_PER_QUARTERSECOND);
}

uint16_t emberAfGetWakeTimeoutMsCallback(void)
{
  State *state = &states[emberGetCurrentNetwork()];
  return state->wakeTimeoutMs;
}

uint16_t emberAfGetWakeTimeoutQsCallback(void)
{
  return (emberAfGetWakeTimeoutMsCallback()
          / MILLISECOND_TICKS_PER_QUARTERSECOND);
}

void emberAfSetWakeTimeoutMsCallback(uint16_t wakeTimeoutMs)
{
  State *state = &states[emberGetCurrentNetwork()];
  state->wakeTimeoutMs = wakeTimeoutMs;
}

void emberAfSetWakeTimeoutQsCallback(uint16_t wakeTimeoutQs)
{
  emberAfSetWakeTimeoutMsCallback(wakeTimeoutQs
                                  * MILLISECOND_TICKS_PER_QUARTERSECOND);
}

EmberAfApplicationTask emberAfGetWakeTimeoutBitmaskCallback(void)
{
  State *state = &states[emberGetCurrentNetwork()];
  return state->wakeTimeoutBitmask;
}

void emberAfSetWakeTimeoutBitmaskCallback(EmberAfApplicationTask tasks)
{
  State *state = &states[emberGetCurrentNetwork()];
  state->wakeTimeoutBitmask = tasks;
}

#ifdef UC_BUILD
#include "zap-event.h"

EmberAfEventPollControl emberAfGetCurrentPollControlCallback(void)
{
  uint8_t networkIndex = emberGetCurrentNetwork();
  EmberAfEventPollControl pollControl = states[networkIndex].pollControl;
#if EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT > 0
  uint8_t i;
  for (i = 0; i < sli_zigbee_app_event_context_length; i++) {
    sl_zigbee_event_context_t *context = &sli_zigbee_app_event_context[i];
    if (networkIndex == emberAfNetworkIndexFromEndpoint(context->endpoint)
        && sl_zigbee_event_is_scheduled(context->event)
        && pollControl < context->pollControl) {
      pollControl = context->pollControl;
    }
  }
#endif
  return pollControl;
}

#else // !UC_BUILD

EmberAfEventPollControl emberAfGetCurrentPollControlCallback(void)
{
  uint8_t networkIndex = emberGetCurrentNetwork();
  EmberAfEventPollControl pollControl = states[networkIndex].pollControl;
#ifdef EMBER_AF_GENERATED_EVENT_CONTEXT
  uint8_t i;
  for (i = 0; i < emAfAppEventContextLength; i++) {
    EmberAfEventContext *context = &emAfAppEventContext[i];
    if (networkIndex == emberAfNetworkIndexFromEndpoint(context->endpoint)
        && emberEventControlGetActive(*context->eventControl)
        && pollControl < context->pollControl) {
      pollControl = context->pollControl;
    }
  }
#endif
  return pollControl;
}

#endif // UC_BUILD

EmberAfEventPollControl emberAfGetDefaultPollControlCallback(void)
{
  State *state = &states[emberGetCurrentNetwork()];
  return state->pollControl;
}

void emberAfSetDefaultPollControlCallback(EmberAfEventPollControl pollControl)
{
  State *state = &states[emberGetCurrentNetwork()];
  state->pollControl = pollControl;
}
