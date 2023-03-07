/***************************************************************************//**
 * @brief ZigBee Application Framework common code.
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
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

#include PLATFORM_HEADER
#include "hal.h"
#include "stack/include/ember.h"
#include "zigbee_app_framework_common.h"
#include "zigbee_app_framework_callback.h"

#include "sl_component_catalog.h"

#if (defined(SL_CATALOG_ZIGBEE_ZCL_FRAMEWORK_CORE_PRESENT) || (defined(EMBER_SCRIPTED_TEST)))
extern EmberStatus emberAfPushNetworkIndex(uint8_t networkIndex);
extern EmberStatus emberAfPushEndpointNetworkIndex(uint8_t endpoint);
extern EmberStatus emberAfPopNetworkIndex(void);
extern uint8_t emberAfIndexFromEndpoint(uint8_t endpoint);
#else
#define emberAfPushNetworkIndex(networkIndex)
#define emberAfPushEndpointNetworkIndex(endpoint)
#define emberAfPopNetworkIndex()
#define emberAfIndexFromEndpoint(endpoint) (0xFF)
#endif

extern void emberAfMainInitCallback(void);
extern void emberAfMainTickCallback(void);

EmberEventQueue emAppEventQueue;

//------------------------------------------------------------------------------
// Init and tick callbacks

#ifndef EZSP_HOST
void sli_zigbee_stack_init_callback(void)
{
  EmberStatus status;

  // Initialize the radio and the stack.  If this fails, we have to assert
  // because something is wrong.
  status = emberInit();
  assert(status == EMBER_SUCCESS);
}
#endif // EZSP_HOST

extern void emAfEventInit(void);
extern void emAfLocalDataInit(void);
extern void emAfInitDone(void);

void sli_zigbee_app_framework_init_callback(void)
{
  // Init the event queue.
  emInitializeEventQueue(&emAppEventQueue);

  // Call the application init callback.
  emberAfMainInitCallback();

  // Call the generated init callbacks.
  emAfEventInit();
  emAfLocalDataInit();
  emAfInitDone();
}

#ifndef EZSP_HOST
void sli_zigbee_stack_tick_callback(void)
{
  // Pet the watchdog.
  halResetWatchdog();
  // Call the stack tick API.
  emberTick();
}
#endif // EZSP_HOST

extern void emAfTick(void);

void sli_zigbee_app_framework_tick_callback(void)
{
  // Pet the watchdog.
  halResetWatchdog();

  // Call the application tick callback.
  emberAfMainTickCallback();

  // Call the generated tick callback (tick the components that subscribed to it).
  emAfTick();

  // Run the application event queue.
  emberRunEventQueue(&emAppEventQueue);
}

//------------------------------------------------------------------------------
// Event System

void sli_zigbee_event_common_handler(sl_zigbee_event_t *event)
{
  bool is_network_event = sli_zigbee_event_is_network_event(event);
  bool is_endpoint_event = sli_zigbee_event_is_endpoint_event(event);

  assert(!(is_network_event && is_endpoint_event));

  if (is_network_event) {
    emberAfPushNetworkIndex(sli_zigbee_event_get_network_index(event));
  } else if (is_endpoint_event) {
    emberAfPushEndpointNetworkIndex(sli_zigbee_event_get_endpoint(event));
  }

  assert(event->dataPtr);

  emberEventSetInactive(event);

  // Endpoint events pass the endpoint to the handler, other events pass the
  // event itself.
  // TODO: we should clean this up by always pass the event end let the code
  // in the handler retrieve the endpoint.
  if (is_endpoint_event) {
    ((void (*)(uint8_t))(event->dataPtr))(sli_zigbee_event_get_endpoint(event));
  } else {
    ((void (*)(sl_zigbee_event_t *))(event->dataPtr))(event);
  }

  if (is_network_event || is_endpoint_event) {
    emberAfPopNetworkIndex();
  }
}

void sli_zigbee_event_init(sl_zigbee_event_t *event,
                           void *handler,
                           uint8_t network_index,
                           uint8_t endpoint)
{
  // We do not support events that are multiplexed by network and endpoint
  // simultaneously.
  assert(!(network_index < 0xFF && endpoint < 0xFF));

  event->next = NULL;
  event->actions.queue = &emAppEventQueue;
  event->actions.handler = sli_zigbee_event_common_handler;
  event->actions.marker = NULL;
  event->dataPtr = handler;
  event->data = 0;

  if (network_index < 0xFF) {
    sli_zigbee_event_set_network_event(event);
    sli_zigbee_event_set_network_index(event, network_index);
  }
  if (endpoint < 0xFF) {
    sli_zigbee_event_set_endpoint_event(event);
    sli_zigbee_event_set_endpoint(event, endpoint);
  }
}

void sli_zigbee_network_event_init(sl_zigbee_event_t *event,
                                   void (*handler)(sl_zigbee_event_t *))
{
  uint8_t i;
  for (i = 0; i < EMBER_SUPPORTED_NETWORKS; i++) {
    sli_zigbee_event_init(event, (void*)handler, i, 0xFF);
    event++;
  }
}

sl_zigbee_event_t* sli_zigbee_get_event_ptr(sl_zigbee_event_t *event,
                                            uint8_t endpoint)
{
  bool is_network_event = sli_zigbee_event_is_network_event(event);
  bool is_endpoint_event = sli_zigbee_event_is_endpoint_event(event);

  // We do not support events that are multiplexed by network and endpoint
  // simultaneously.
  assert(!(is_network_event && is_endpoint_event));

  if (endpoint < 0xFF) {
    // If we are passing a valid endpoint to this API, we enforce that the event
    // is actually an endpoint event.
    assert(is_endpoint_event);

    uint8_t endpoint_index = emberAfIndexFromEndpoint(endpoint);
    assert(endpoint_index < 0xFF);
    return event + endpoint_index;
  }

  if (is_network_event) {
    // We enforce that the network-0 event is the one passed in.
    assert(sli_zigbee_event_get_network_index(event) == 0);
    event = event + emberGetCurrentNetwork();
  }

  return event;
}

//------------------------------------------------------------------------------
// Callbacks stubs

WEAK(void emberAfMainInitCallback(void))
{
}

WEAK(void emberAfMainTickCallback(void))
{
}
