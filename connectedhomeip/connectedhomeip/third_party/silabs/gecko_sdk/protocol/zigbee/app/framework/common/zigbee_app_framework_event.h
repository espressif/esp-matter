/***************************************************************************//**
 * @file zigbee_app_framework_event.h
 * @brief Zigbee Application Framework Event releated macros and definitions.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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

#ifndef ZIGBEE_APP_FRAMEWORK_EVENT_H
#define ZIGBEE_APP_FRAMEWORK_EVENT_H

#include PLATFORM_HEADER
#include "event_queue/event-queue.h"
#include "stack/include/ember-types.h"

#ifdef SL_COMPONENT_CATALOG_PRESENT
#include "sl_component_catalog.h"
#endif // SL_COMPONENT_CATALOG_PRESENT

#ifdef SL_CATALOG_ZIGBEE_CORE_CLI_PRESENT
#include "zigbee_core_cli_config.h"
#endif // SL_CATALOG_ZIGBEE_CORE_CLI_PRESENT

/**
 * @defgroup zigbee_event Event System
 * @ingroup af
 * @brief Zigbee event system API
 *
 * See zigbee_app_framework_event.h for source code.
 *
 */

/**
 * @addtogroup zigbee_event
 *
 * Following a brief usage example that demonstrate how to create an event,
 * initialize it and use it. In this example, the event is initialized and set
 * to pending, then it gets rescheduled every second.
 *
 * @code
 * // Declare event as global
 * sl_zigbee_event_t my_event;
 *
 * void my_event_hendler(sl_zigbee_event_t *event)
 * {
 *    // Event expired, do something
 *
 *    // Reschedule the event to expire again in 1 second
 *    sl_zigbee_event_set_delay_ms(&my_event, 1000);
 * }
 *
 * void app_init(void)
 * {
 *    // Initialize event
 *    sl_zigbee_event_init(&my_event, my_event_handler);
 *
 *    // Set the event to expire immediately (that is, in the next iteration of the main loop)
 *    sl_zigbee_event_set_active(&my_event);
 * }
 * @endcode
 *
 * @{
 */

/**
 * @brief  ZigBee application event.
 */
typedef EmberEvent sl_zigbee_event_t;

/** @name API */
// @{

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Application event initialization routine. Every application event
 * must be initialized.
 *
 * @param[in] event  A pointer to the \ref sl_zigbee_event_t object to be
 *                   initalized. Event objects must be global.
 *
 * @param[in] handler Handler function that shall be called when the event runs.
 */
void sl_zigbee_event_init(sl_zigbee_event_t *event,
                          void (*handler)(sl_zigbee_event_t *));
#else
#if (SL_ZIGBEE_EVENT_DEBUG_ENABLED)
  #define sl_zigbee_event_init(event, handler)                     \
  do {                                                             \
    sli_zigbee_event_init((event), (void *)(handler), 0xFF, 0xFF); \
    ((event)->actions).name = (#event);                            \
  } while (0)
#else
  #define sl_zigbee_event_init(event, handler) \
  sli_zigbee_event_init((event), (void *)(handler), 0xFF, 0xFF);
#endif // SL_ZIGBEE_EVENT_DEBUG_ENABLED
#endif // DOXYGEN_SHOULD_SKIP_THIS

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Schedule an event to run after a delay expressed in milliseconds.
 *
 * @param[in] event  A pointer to the \ref sl_zigbee_event_t object to be
 *                   scheduled to run after a delay.
 *
 * @param[in] delay  The delay in milliseconds after which the event shall run.
 */
void sl_zigbee_event_set_delay_ms(sl_zigbee_event_t *event, uint32_t delay);
#else
#define sl_zigbee_event_set_delay_ms(event, delay) \
  emberEventSetDelayMs(sli_zigbee_get_event_ptr((event), 0xFF), (delay))
#endif // DOXYGEN_SHOULD_SKIP_THIS

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Schedule an event to run immediately.
 *
 * @param[in] event  A pointer to the \ref sl_zigbee_event_t object to be
 *                   scheduled to run immediately.
 */
void sl_zigbee_event_set_active(sl_zigbee_event_t *event);
#else
#define sl_zigbee_event_set_active(event) \
  emberEventSetActive(sli_zigbee_get_event_ptr((event), 0xFF))
#endif // DOXYGEN_SHOULD_SKIP_THIS

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Cancel an event.
 *
 * @param[in] event  A pointer to the \ref sl_zigbee_event_t object to be
 *                   cancelled.
 */
void sl_zigbee_event_set_inactive(sl_zigbee_event_t *event);
#else
#define sl_zigbee_event_set_inactive(event) \
  emberEventSetInactive(sli_zigbee_get_event_ptr((event), 0xFF))
#endif // DOXYGEN_SHOULD_SKIP_THIS

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Returns true if the event is scheduled to run, false otherwise.
 *
 * @param[in] event  A pointer to an \ref sl_zigbee_event_t object.
 *
 * @return \b true if the passed event is scheduled to run, \b false otherwise.
 */
bool sl_zigbee_event_is_scheduled(sl_zigbee_event_t *event);
#else
#define sl_zigbee_event_is_scheduled(event) \
  emberEventIsScheduled(sli_zigbee_get_event_ptr((event), 0xFF))
#endif // DOXYGEN_SHOULD_SKIP_THIS

#if defined(DOXYGEN_SHOULD_SKIP_THIS)
/** @brief Returns the number of milliseconds before the event runs, or -1 if
 * the event is not scheduled to run.
 *
 * @param[in] event  A pointer to an \ref sl_zigbee_event_t object.
 *
 * @return The number of milliseconds before the event runs, or -1 if the event
 *         is not scheduled to run.
 */
uint32_t sl_zigbee_event_get_remaining_ms(sl_zigbee_event_t *event);
#else
#define sl_zigbee_event_get_remaining_ms(event) \
  emberEventGetRemainingMs(sli_zigbee_get_event_ptr((event), 0xFF))
#endif // DOXYGEN_SHOULD_SKIP_THIS

/** @} */ // end of name API
/** @} */ // end of zigbee_event

//------------------------------------------------------------------------------
// Internal

#if !defined(DOXYGEN_SHOULD_SKIP_THIS)

// The data field is currently utilized as follows:
// byte 0 is a control field, whereas:
// - bit 0 is a flag indicating whether the event is multiplexed by network
// - bit 1 is a flag indicating whether the event is multiplexed by endpoint
// - bits [2-3] are used to store the network index (if the event is multiplexed
//              by network)
// - bits [4-7] are currently unused
// byte 1 is used to store the endpoint (if the event is multiplexed by endpoint)
// bytes [2-3] are unused
#define SLI_ZIGBEE_EVENT_CONTROL_FIELD_MASK             0x000000FFu
#define SLI_ZIGBEE_EVENT_CONTROL_FIELD_OFFSET           0u
#define SLI_ZIGBEE_EVENT_CONTROL_IS_NETWORK_EVENT_BIT   0x01u
#define SLI_ZIGBEE_EVENT_CONTROL_IS_ENDPOINT_EVENT_BIT  0x02u
#define SLI_ZIGBEE_EVENT_CONTROL_NETWORK_INDEX_MASK     0x0Cu
#define SLI_ZIGBEE_EVENT_CONTROL_NETWORK_INDEX_OFFSET   2u
#define SLI_ZIGBEE_EVENT_CONTROL_UNUSED_MASK            0xF0u
#define SLI_ZIGBEE_EVENT_ENDPOINT_FIELD_MASK            0x0000FF00u
#define SLI_ZIGBEE_EVENT_ENDPOINT_FIELD_OFFSET          8u

#define sli_zigbee_event_get_control_field(event)                  \
  ((uint8_t)(((event)->data & SLI_ZIGBEE_EVENT_CONTROL_FIELD_MASK) \
             >> SLI_ZIGBEE_EVENT_CONTROL_FIELD_OFFSET))

#define sli_zigbee_event_set_control_field(event, control)                            \
  do {                                                                                \
    (event)->data &= ~SLI_ZIGBEE_EVENT_CONTROL_FIELD_MASK;                            \
    (event)->data |= (((uint8_t)(control)) << SLI_ZIGBEE_EVENT_CONTROL_FIELD_OFFSET); \
  } while (0)

#define sli_zigbee_event_is_network_event(event) \
  ((sli_zigbee_event_get_control_field(event)    \
    & SLI_ZIGBEE_EVENT_CONTROL_IS_NETWORK_EVENT_BIT) > 0)

#define sli_zigbee_event_set_network_event(event)                                          \
  do {                                                                                     \
    uint8_t control = sli_zigbee_event_get_control_field(event);                           \
    sli_zigbee_event_set_control_field((event),                                            \
                                       (control                                            \
                                        | SLI_ZIGBEE_EVENT_CONTROL_IS_NETWORK_EVENT_BIT)); \
  } while (0)

#define sli_zigbee_event_is_endpoint_event(event) \
  ((sli_zigbee_event_get_control_field(event)     \
    & SLI_ZIGBEE_EVENT_CONTROL_IS_ENDPOINT_EVENT_BIT) > 0)

#define sli_zigbee_event_set_endpoint_event(event)                                          \
  do {                                                                                      \
    uint8_t control = sli_zigbee_event_get_control_field(event);                            \
    sli_zigbee_event_set_control_field((event),                                             \
                                       (control                                             \
                                        | SLI_ZIGBEE_EVENT_CONTROL_IS_ENDPOINT_EVENT_BIT)); \
  } while (0)

#define sli_zigbee_event_get_network_index(event)            \
  ((uint8_t)((sli_zigbee_event_get_control_field(event)      \
              & SLI_ZIGBEE_EVENT_CONTROL_NETWORK_INDEX_MASK) \
             >> SLI_ZIGBEE_EVENT_CONTROL_NETWORK_INDEX_OFFSET))

#define sli_zigbee_event_set_network_index(event, network_index)                                \
  do {                                                                                          \
    uint8_t control = sli_zigbee_event_get_control_field(event);                                \
    sli_zigbee_event_set_control_field((event),                                                 \
                                       (control                                                 \
                                        | ((network_index)                                      \
                                           << SLI_ZIGBEE_EVENT_CONTROL_NETWORK_INDEX_OFFSET))); \
  } while (0)

#define sli_zigbee_event_get_endpoint(event)                        \
  ((uint8_t)(((event)->data & SLI_ZIGBEE_EVENT_ENDPOINT_FIELD_MASK) \
             >> SLI_ZIGBEE_EVENT_ENDPOINT_FIELD_OFFSET))

#define sli_zigbee_event_set_endpoint(event, endpoint)                                  \
  do {                                                                                  \
    (event)->data &= ~SLI_ZIGBEE_EVENT_ENDPOINT_FIELD_MASK;                             \
    (event)->data |= (((uint8_t)(endpoint)) << SLI_ZIGBEE_EVENT_ENDPOINT_FIELD_OFFSET); \
  } while (0)

void sli_zigbee_event_init(sl_zigbee_event_t *event,
                           void *handler,
                           uint8_t network_index,
                           uint8_t endpoint);

sl_zigbee_event_t* sli_zigbee_get_event_ptr(sl_zigbee_event_t *event,
                                            uint8_t endpoint);

extern EmberEventQueue emAppEventQueue;
#define sli_zigbee_ms_to_next_app_framework_event() \
  emberMsToNextQueueEvent(&emAppEventQueue)

#define sli_zigbee_run_events() \
  emberRunEventQueue(&emAppEventQueue)

#if (SL_ZIGBEE_EVENT_DEBUG_ENABLED)
  #define sl_zigbee_endpoint_event_init(event, handler, endpoint)        \
  do {                                                                   \
    sli_zigbee_event_init((event), (void *)(handler), 0xFF, (endpoint)); \
    ((event)->actions).name = (#event);                                  \
  } while (0)
#else
  #define sl_zigbee_endpoint_event_init(event, handler, endpoint) \
  sli_zigbee_event_init((event), (void *)(handler), 0xFF, (endpoint));
#endif // SL_ZIGBEE_EVENT_DEBUG_ENABLED

void sli_zigbee_network_event_init(sl_zigbee_event_t *event,
                                   void (*handler)(sl_zigbee_event_t *));

#if (SL_ZIGBEE_EVENT_DEBUG_ENABLED)
  #define sl_zigbee_network_event_init(event, handler) \
  do {                                                 \
    sli_zigbee_network_event_init((event), (handler)); \
    ((event)->actions).name = (#event);                \
  } while (0)
#else
  #define sl_zigbee_network_event_init(event, handler) \
  sli_zigbee_network_event_init((event), (handler));
#endif // SL_ZIGBEE_EVENT_DEBUG_ENABLED

#define sl_zigbee_event_set_delay_qs(event, delay) \
  emberEventSetDelayMs(sli_zigbee_get_event_ptr((event), 0xFF), ((delay) * 250u))

// For conversion from minutes to ms, multiply by 60,000 instead of << 16
// This leads to a 10% difference between the time requested vs the actual time
// leading to a large discrepancy for longer delays
#define sl_zigbee_event_set_delay_minutes(event, delay) \
  emberEventSetDelayMs(sli_zigbee_get_event_ptr((event), 0xFF), ((delay) * 60000u))

#define sl_zigbee_endpoint_event_set_delay_ms(event, endpoint, delay) \
  emberEventSetDelayMs(sli_zigbee_get_event_ptr((event), (endpoint)), (delay))

#define sl_zigbee_endpoint_event_set_delay_qs(event, endpoint, delay) \
  emberEventSetDelayMs(sli_zigbee_get_event_ptr((event), (endpoint)), ((delay) * 250u))

// For conversion from minutes to ms, multiply by 60,000 instead of << 16
// This leads to a 10% difference between the time requested vs the actual time
// leading to a large discrepancy for longer delays
#define sl_zigbee_endpoint_event_set_delay_minutes(event, endpoint, delay) \
  emberEventSetDelayMs(sli_zigbee_get_event_ptr((event), (endpoint)), ((delay) * 60000u))

#define sl_zigbee_endpoint_event_set_active(event, endpoint) \
  emberEventSetActive(sli_zigbee_get_event_ptr((event), (endpoint)))

#define sl_zigbee_endpoint_event_set_inactive(event, endpoint) \
  emberEventSetInactive(sli_zigbee_get_event_ptr((event), (endpoint)))

#define sl_zigbee_endpoint_event_is_scheduled(event, endpoint) \
  emberEventIsScheduled(sli_zigbee_get_event_ptr((event), (endpoint)))

#define sl_zigbee_endpoint_event_get_remaining_ms(event, endpoint) \
  emberEventGetRemainingMs(sli_zigbee_get_event_ptr((event), (endpoint)))

#endif // !DOXYGEN_SHOULD_SKIP_THIS

#endif // ZIGBEE_APP_FRAMEWORK_EVENT_H
