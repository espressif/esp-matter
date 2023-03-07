/***************************************************************************//**
 * @file
 * @brief Application event code that is common to both the SOC and EZSP platforms.
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

#include PLATFORM_HEADER     // Micro and compiler specific typedefs and macros
#include "../include/af.h"

#ifndef UC_BUILD
#include "callback.h"
#endif

#include "af-event.h"
#include "stack/include/error.h"
#include "../security/crypto-state.h"
#include "app/framework/util/service-discovery.h"

#ifdef UC_BUILD
#include "sl_component_catalog.h"
#ifdef SL_ZIGBEE_TEST_HARNESS_PRESENT
#include "test-harness.h"
#endif
#else // !UC_BUILD
#include "app/framework/plugin/test-harness/test-harness.h"
#endif // UC_BUILD

#include "app/framework/util/attribute-storage.h"

//------------------------------------------------------------------------------
// UC Globals

#ifdef UC_BUILD

#include "zap-event.h"

#ifdef EMBER_AF_GENERATED_UC_EVENTS_DEF
EMBER_AF_GENERATED_UC_EVENTS_DEF
#endif // EMBER_AF_GENERATED_UC_EVENTS_DEF

#if defined(EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT) && EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT > 0
sl_zigbee_event_context_t sli_zigbee_app_event_context[] = {
  EMBER_AF_GENERATED_UC_EVENT_CONTEXT
};
uint16_t sli_zigbee_app_event_context_length = sizeof(sli_zigbee_app_event_context) / sizeof(sl_zigbee_event_context_t);
#endif //EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT

#else // !UC_BUILD

#ifdef EMBER_AF_PLUGIN_FRAGMENTATION
#include "app/framework/plugin/fragmentation/fragmentation.h"
#endif
//------------------------------------------------------------------------------
// AppBuilder Globals

// Task ids used to run events through idling
EmberTaskId emAfTaskId;

#ifdef EMBER_AF_GENERATED_EVENT_CODE
EMBER_AF_GENERATED_EVENT_CODE
#endif //EMBER_AF_GENERATED_EVENT_CODE

EmberEventData emAfEvents[] = {
  EM_AF_SERVICE_DISCOVERY_EVENTS

#ifdef EMBER_AF_GENERATED_EVENTS
  EMBER_AF_GENERATED_EVENTS
#endif

#ifdef EMBER_AF_PLUGIN_FRAGMENTATION
  EMBER_AF_FRAGMENTATION_EVENTS
#endif

  EMBER_KEY_ESTABLISHMENT_TEST_HARNESS_EVENT

  { NULL, NULL }
};

const char * emAfEventStrings[] = {
  EM_AF_SERVICE_DISCOVERY_EVENT_STRINGS

#ifdef EMBER_AF_GENERATED_EVENTS
  EMBER_AF_GENERATED_EVENT_STRINGS
#endif

#ifdef EMBER_AF_PLUGIN_FRAGMENTATION
  EMBER_AF_FRAGMENTATION_EVENT_STRINGS
#endif

  EMBER_AF_TEST_HARNESS_EVENT_STRINGS
};

const char emAfStackEventString[] = "Stack";

#if defined(EMBER_AF_GENERATED_EVENT_CONTEXT)
uint16_t emAfAppEventContextLength = EMBER_AF_EVENT_CONTEXT_LENGTH;
EmberAfEventContext emAfAppEventContext[] = {
  EMBER_AF_GENERATED_EVENT_CONTEXT
};
#endif //EMBER_AF_GENERATED_EVENT_CONTEXT

#endif // UC_BUILD

//------------------------------------------------------------------------------
// UC functions

#ifdef UC_BUILD

void emAfZclFrameworkCoreInitEventsCallback(SLXU_INIT_ARG)
{
  SLXU_INIT_UNUSED_ARG;

#ifdef EMBER_AF_GENERATED_UC_EVENTS_INIT
  EMBER_AF_GENERATED_UC_EVENTS_INIT
#endif
}

static sl_zigbee_event_context_t *find_event_context(uint8_t endpoint,
                                                     EmberAfClusterId clusterId,
                                                     bool isClient)
{
#if defined(EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT) && EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT > 0
  uint8_t i;
  for (i = 0; i < sli_zigbee_app_event_context_length; i++) {
    sl_zigbee_event_context_t *context = &(sli_zigbee_app_event_context[i]);
    if (context->endpoint == endpoint
        && context->clusterId == clusterId
        && context->isClient == isClient) {
      return context;
    }
  }
#endif //EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT
  return NULL;
}
uint32_t emberAfMsToNextEventExtended(uint32_t maxMs, uint8_t* returnIndex)
{
  return emberMsToNextQueueEvent(&emAppEventQueue);
}

EmberStatus sl_zigbee_zcl_schedule_tick_extended(uint8_t endpoint,
                                                 EmberAfClusterId clusterId,
                                                 bool isClient,
                                                 uint32_t delayMs,
                                                 EmberAfEventPollControl pollControl,
                                                 EmberAfEventSleepControl sleepControl)
{
  sl_zigbee_event_context_t *context = find_event_context(endpoint,
                                                          clusterId,
                                                          isClient);

  // Disabled endpoints cannot schedule events.  This will catch the problem in
  // simulation.
  EMBER_TEST_ASSERT(emberAfEndpointIsEnabled(endpoint));

  if (context != NULL
      && emberAfEndpointIsEnabled(endpoint)) {
    sl_zigbee_event_set_delay_ms(context->event, delayMs);
    context->pollControl = pollControl;
    context->sleepControl = sleepControl;
    return EMBER_SUCCESS;
  }
  return EMBER_BAD_ARGUMENT;
}

EmberStatus sl_zigbee_zcl_schedule_cluster_tick(uint8_t endpoint,
                                                EmberAfClusterId clusterId,
                                                bool isClient,
                                                uint32_t delayMs,
                                                EmberAfEventSleepControl sleepControl)
{
  return sl_zigbee_zcl_schedule_tick_extended(endpoint,
                                              clusterId,
                                              isClient,
                                              delayMs,
                                              (sleepControl == EMBER_AF_OK_TO_HIBERNATE
                                               ? EMBER_AF_LONG_POLL
                                               : EMBER_AF_SHORT_POLL),
                                              (sleepControl == EMBER_AF_STAY_AWAKE
                                               ? EMBER_AF_STAY_AWAKE
                                               : EMBER_AF_OK_TO_SLEEP));
}

EmberStatus sl_zigbee_zcl_schedule_client_tick_extended(uint8_t endpoint,
                                                        EmberAfClusterId clusterId,
                                                        uint32_t delayMs,
                                                        EmberAfEventPollControl pollControl,
                                                        EmberAfEventSleepControl sleepControl)
{
  return sl_zigbee_zcl_schedule_tick_extended(endpoint,
                                              clusterId,
                                              EMBER_AF_CLIENT_CLUSTER_TICK,
                                              delayMs,
                                              pollControl,
                                              sleepControl);
}

EmberStatus sl_zigbee_zcl_schedule_client_tick(uint8_t endpoint,
                                               EmberAfClusterId clusterId,
                                               uint32_t delayMs)
{
  return sl_zigbee_zcl_schedule_client_tick_extended(endpoint,
                                                     clusterId,
                                                     delayMs,
                                                     EMBER_AF_LONG_POLL,
                                                     EMBER_AF_OK_TO_SLEEP);
}

EmberStatus sl_zigbee_zcl_schedule_server_tick_extended(uint8_t endpoint,
                                                        EmberAfClusterId clusterId,
                                                        uint32_t delayMs,
                                                        EmberAfEventPollControl pollControl,
                                                        EmberAfEventSleepControl sleepControl)
{
  return sl_zigbee_zcl_schedule_tick_extended(endpoint,
                                              clusterId,
                                              EMBER_AF_SERVER_CLUSTER_TICK,
                                              delayMs,
                                              pollControl,
                                              sleepControl);
}

EmberStatus sl_zigbee_zcl_schedule_server_tick(uint8_t endpoint,
                                               EmberAfClusterId clusterId,
                                               uint32_t delayMs)
{
  return sl_zigbee_zcl_schedule_server_tick_extended(endpoint,
                                                     clusterId,
                                                     delayMs,
                                                     EMBER_AF_LONG_POLL,
                                                     EMBER_AF_OK_TO_SLEEP);
}

EmberStatus sl_zigbee_zcl_deactivate_cluster_tick(uint8_t endpoint,
                                                  EmberAfClusterId clusterId,
                                                  bool isClient)
{
  sl_zigbee_event_context_t *context = find_event_context(endpoint,
                                                          clusterId,
                                                          isClient);
  if (context != NULL) {
    sl_zigbee_event_set_inactive(context->event);
    return EMBER_SUCCESS;
  }
  return EMBER_BAD_ARGUMENT;
}

EmberStatus sl_zigbee_zcl_deactivate_client_tick(uint8_t endpoint,
                                                 EmberAfClusterId clusterId)
{
  return sl_zigbee_zcl_deactivate_cluster_tick(endpoint,
                                               clusterId,
                                               EMBER_AF_CLIENT_CLUSTER_TICK);
}

EmberStatus sl_zigbee_zcl_deactivate_server_tick(uint8_t endpoint,
                                                 EmberAfClusterId clusterId)
{
  return sl_zigbee_zcl_deactivate_cluster_tick(endpoint,
                                               clusterId,
                                               EMBER_AF_SERVER_CLUSTER_TICK);
}

#ifndef EMBER_AF_NCP
// There is only idling on NCP hence sleep control code is not needed.
static EmberAfEventSleepControl defaultSleepControl = EMBER_AF_OK_TO_SLEEP;
// Checks all application events to see if any active events require the micro to stay awake
EmberAfEventSleepControl emberAfGetCurrentSleepControlCallback(void)
{
  EmberAfEventSleepControl sleepControl = defaultSleepControl;
#if defined(EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT) && EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT > 0
  uint8_t i;
  for (i = 0; i < sli_zigbee_app_event_context_length; i++) {
    sl_zigbee_event_context_t *context = &sli_zigbee_app_event_context[i];
    if ( (0 == sl_zigbee_event_get_remaining_ms(&(*context->event))) //equivalent to emberEventControlGetActive
         && sleepControl < context->sleepControl) {
      sleepControl = context->sleepControl;
    }
  }
#endif // EMBER_AF_GENERATED_UC_EVENT_CONTEXT_COUNT
  return sleepControl;
}
EmberAfEventSleepControl emberAfGetDefaultSleepControlCallback(void)
{
  return defaultSleepControl;
}

void emberAfSetDefaultSleepControlCallback(EmberAfEventSleepControl sleepControl)
{
  defaultSleepControl = sleepControl;
}
#endif // EMBER_AF_NCP

#else // !UC_BUILD

//------------------------------------------------------------------------------
// AppBuilder functions

void emAfInitEvents(void)
{
  emberTaskEnableIdling(true);
  emAfTaskId = emberTaskInit(emAfEvents);
}

void emberAfRunEvents(void)
{
  // Don't run events while crypto operation is in progress
  // (BUGZID: 12127)
  if (emAfIsCryptoOperationInProgress()) {
    // DEBUG Bugzid: 11944
    emberAfCoreFlush();
    return;
  }
  emberRunTask(emAfTaskId);
}

const char * emberAfGetEventString(uint8_t index)
{
  return (index == 0XFF
          ? emAfStackEventString
          : emAfEventStrings[index]);
}

static EmberAfEventContext *findEventContext(uint8_t endpoint,
                                             EmberAfClusterId clusterId,
                                             bool isClient)
{
#if defined(EMBER_AF_GENERATED_EVENT_CONTEXT)
  uint8_t i;
  for (i = 0; i < emAfAppEventContextLength; i++) {
    EmberAfEventContext *context = &(emAfAppEventContext[i]);
    if (context->endpoint == endpoint
        && context->clusterId == clusterId
        && context->isClient == isClient) {
      return context;
    }
  }
#endif //EMBER_AF_GENERATED_EVENT_CONTEXT
  return NULL;
}

EmberStatus emberAfEventControlSetDelayMS(EmberEventControl *control,
                                          uint32_t delayMs)
{
  if (delayMs == 0) {
    emberEventControlSetActive(*control);
  } else if (delayMs <= EMBER_MAX_EVENT_CONTROL_DELAY_MS) {
    emberEventControlSetDelayMS(*control, delayMs);
  } else {
    return EMBER_BAD_ARGUMENT;
  }
  return EMBER_SUCCESS;
}

EmberStatus emberAfEventControlSetDelayQS(EmberEventControl *control,
                                          uint32_t delayQs)
{
  if (delayQs <= EMBER_MAX_EVENT_CONTROL_DELAY_QS) {
    return emberAfEventControlSetDelayMS(control, delayQs << 8);
  } else {
    return EMBER_BAD_ARGUMENT;
  }
}

EmberStatus emberAfEventControlSetDelayMinutes(EmberEventControl *control,
                                               uint16_t delayM)
{
  if (delayM <= EMBER_MAX_EVENT_CONTROL_DELAY_MINUTES) {
    return emberAfEventControlSetDelayMS(control, delayM << 16);
  } else {
    return EMBER_BAD_ARGUMENT;
  }
}

EmberStatus emberAfScheduleTickExtended(uint8_t endpoint,
                                        EmberAfClusterId clusterId,
                                        bool isClient,
                                        uint32_t delayMs,
                                        EmberAfEventPollControl pollControl,
                                        EmberAfEventSleepControl sleepControl)
{
  EmberAfEventContext *context = findEventContext(endpoint,
                                                  clusterId,
                                                  isClient);

  // Disabled endpoints cannot schedule events.  This will catch the problem in
  // simulation.
  EMBER_TEST_ASSERT(emberAfEndpointIsEnabled(endpoint));

  if (context != NULL
      && emberAfEndpointIsEnabled(endpoint)
      && (emberAfEventControlSetDelayMS(context->eventControl, delayMs)
          == EMBER_SUCCESS)) {
    context->pollControl = pollControl;
    context->sleepControl = sleepControl;
    return EMBER_SUCCESS;
  }
  return EMBER_BAD_ARGUMENT;
}

EmberStatus emberAfScheduleClusterTick(uint8_t endpoint,
                                       EmberAfClusterId clusterId,
                                       bool isClient,
                                       uint32_t delayMs,
                                       EmberAfEventSleepControl sleepControl)
{
  return emberAfScheduleTickExtended(endpoint,
                                     clusterId,
                                     isClient,
                                     delayMs,
                                     (sleepControl == EMBER_AF_OK_TO_HIBERNATE
                                      ? EMBER_AF_LONG_POLL
                                      : EMBER_AF_SHORT_POLL),
                                     (sleepControl == EMBER_AF_STAY_AWAKE
                                      ? EMBER_AF_STAY_AWAKE
                                      : EMBER_AF_OK_TO_SLEEP));
}

EmberStatus emberAfScheduleClientTickExtended(uint8_t endpoint,
                                              EmberAfClusterId clusterId,
                                              uint32_t delayMs,
                                              EmberAfEventPollControl pollControl,
                                              EmberAfEventSleepControl sleepControl)
{
  return emberAfScheduleTickExtended(endpoint,
                                     clusterId,
                                     EMBER_AF_CLIENT_CLUSTER_TICK,
                                     delayMs,
                                     pollControl,
                                     sleepControl);
}

EmberStatus emberAfScheduleClientTick(uint8_t endpoint,
                                      EmberAfClusterId clusterId,
                                      uint32_t delayMs)
{
  return emberAfScheduleClientTickExtended(endpoint,
                                           clusterId,
                                           delayMs,
                                           EMBER_AF_LONG_POLL,
                                           EMBER_AF_OK_TO_SLEEP);
}

EmberStatus emberAfScheduleServerTickExtended(uint8_t endpoint,
                                              EmberAfClusterId clusterId,
                                              uint32_t delayMs,
                                              EmberAfEventPollControl pollControl,
                                              EmberAfEventSleepControl sleepControl)
{
  return emberAfScheduleTickExtended(endpoint,
                                     clusterId,
                                     EMBER_AF_SERVER_CLUSTER_TICK,
                                     delayMs,
                                     pollControl,
                                     sleepControl);
}

EmberStatus emberAfScheduleServerTick(uint8_t endpoint,
                                      EmberAfClusterId clusterId,
                                      uint32_t delayMs)
{
  return emberAfScheduleServerTickExtended(endpoint,
                                           clusterId,
                                           delayMs,
                                           EMBER_AF_LONG_POLL,
                                           EMBER_AF_OK_TO_SLEEP);
}

uint32_t emberAfMsToNextEventExtended(uint32_t maxMs, uint8_t* returnIndex)
{
  return emberMsToNextEventExtended(emAfEvents, maxMs, returnIndex);
}

uint32_t emberAfMsToNextEvent(uint32_t maxMs)
{
  return emberAfMsToNextEventExtended(maxMs, NULL);
}

EmberStatus emberAfDeactivateClusterTick(uint8_t endpoint,
                                         EmberAfClusterId clusterId,
                                         bool isClient)
{
  EmberAfEventContext *context = findEventContext(endpoint,
                                                  clusterId,
                                                  isClient);
  if (context != NULL) {
    emberEventControlSetInactive((*(context->eventControl)));
    return EMBER_SUCCESS;
  }
  return EMBER_BAD_ARGUMENT;
}

EmberStatus emberAfDeactivateClientTick(uint8_t endpoint,
                                        EmberAfClusterId clusterId)
{
  return emberAfDeactivateClusterTick(endpoint,
                                      clusterId,
                                      EMBER_AF_CLIENT_CLUSTER_TICK);
}

EmberStatus emberAfDeactivateServerTick(uint8_t endpoint,
                                        EmberAfClusterId clusterId)
{
  return emberAfDeactivateClusterTick(endpoint,
                                      clusterId,
                                      EMBER_AF_SERVER_CLUSTER_TICK);
}

#endif // UC_BUILD

//------------------------------------------------------------------------------
// Common functions

#define MS_TO_QS(ms)    ((ms) >> 8)
#define MS_TO_MIN(ms)   ((ms) >> 16)
#define QS_TO_MS(qs)    ((qs) << 8)
#define MIN_TO_MS(min) ((min) << 16)

// Used to calculate the duration and unit used by the host to set the sleep timer
void emAfGetTimerDurationAndUnitFromMS(uint32_t durationMs,
                                       uint16_t *duration,
                                       EmberEventUnits *units)
{
  if (durationMs <= MAX_TIMER_UNITS_HOST) {
    *duration = (uint16_t)durationMs;
    *units = EMBER_EVENT_MS_TIME;
  } else if (MS_TO_QS(durationMs) <= MAX_TIMER_UNITS_HOST) {
    *duration = (uint16_t)(MS_TO_QS(durationMs));
    *units = EMBER_EVENT_QS_TIME;
  } else {
    *duration = (MS_TO_MIN(durationMs) <= MAX_TIMER_UNITS_HOST
                 ? (uint16_t)(MS_TO_MIN(durationMs))
                 : MAX_TIMER_UNITS_HOST);
    *units = EMBER_EVENT_MINUTE_TIME;
  }
}

uint32_t emAfGetMSFromTimerDurationAndUnit(uint16_t duration,
                                           EmberEventUnits units)
{
  uint32_t ms;
  if (units == EMBER_EVENT_MS_TIME) {
    ms = duration;
  } else if (units == EMBER_EVENT_QS_TIME) {
    ms = QS_TO_MS(duration);
  } else if (units == EMBER_EVENT_MINUTE_TIME) {
    ms = MIN_TO_MS(duration);
  } else if (units == EMBER_EVENT_ZERO_DELAY) {
    ms = 0;
  } else {
    ms = MAX_INT32U_VALUE;
  }
  return ms;
}
