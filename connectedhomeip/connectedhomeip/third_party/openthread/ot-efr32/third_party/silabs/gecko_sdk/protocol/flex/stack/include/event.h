/***************************************************************************//**
 * @file
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
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

#if defined(EZSP_HOST)
  #define EMBER_NO_IDLE_SUPPORT
#endif

/*
 *
 * This was originally part of the group documentation, but it was removed,
 * since it's not important for Connect platforms. It is kept as it might become
 * important later when support for new platforms is added.
 *
 * The base time units for events are ticks.  Each tick is approximately equal
 * to a millisecond, but the true duration depends on the platform.  The
 * duration of a tick is 1000 / ::MILLISECOND_TICKS_PER_SECOND, where 1000 is
 * the number of milliseconds per second and ::MILLISECOND_TICKS_PER_SECOND is
 * the platform-specific number of ticks per second.  For example,
 * ::MILLISECOND_TICKS_PER_SECOND on the EM357 SoC is 1024, so each tick is
 * 1000 / 1024 = ~0.98 milliseconds.  Calling
 * ::emberEventControlSetDelayMS(someEvent, 100) on the EM357 SoC will schedule
 * the event for 100 ticks * (1000 milliseconds / 1024 ticks) =
 * ~97.7 milliseconds.  Note, however, that the accuracy of the base tick
 * depends on the timer source.  Furthermore, the scheduled delay is the minimum
 * delay. If ::emberRunEvents or ::emberRunTask are not called frequently
 * enough, the actual delay may be longer than the scheduled delay.
 *
 * Additionally, the APIs for quarter second and minute delays
 * (::emberEventControlSetDelayQS and ::emberEventControlSetDelayMinutes) use
 * "binary" units.  One quarter second is 256 ticks and one minute is 65536
 * ticks.  Calling ::emberEventControlSetDelayMinutes(someEvent, 3) on the
 * EM357 SoC will schedule the event for 3 minutes * (65536 ticks / minute) *
 * (1000 milliseconds / 1024 ticks) = ~3.2 minutes.  It is possible to avoid
 * these binary units by using ::emberEventControlSetDelayMS and the various
 * MILLISECOND_TICKS_PER_XXX multipliers.  For example, calling
 * ::emberEventControlSetDelayMS(someEvent, 3 * MILLISECOND_TICKS_PER_MINUTE)
 * will delay for 3 minutes on any platform.  Be aware of
 * ::EMBER_MAX_EVENT_CONTROL_DELAY_MS when using this approach.
 */

/**
 * @addtogroup event
 * @brief Scheduling events for future execution.
 *
 * See @ref event for documentation.
 * These macros implement an event abstraction that allows the application
 * to schedule code to run after a specified time interval.  An event
 * consists of a procedure to be called at some point in the future and
 * a control object that determines which procedure should be called.  Events
 * are also useful when an ISR needs to initiate an action that should
 * run outside the ISR context.
 *
 * See @ref event.h for source code.
 *
 * Note that, while not required, it is recommended that the event-handling
 * procedure explicitly define the recurrence of the next event, either by
 * rescheduling via some kind of @e emberEventControlSetDelayXX() call or by
 * deactivating via a call to ::emberEventControlSetInactive().
 *
 * When the handler does not explicitly reschedule or cancel the
 * event, the default behavior of the event control system is to keep the
 * event immediately active as if the handler function had called
 * ::emberEventControlSetActive(someEvent) or
 * ::emberEventControlSetDelayMS(someEvent, 0).
 *
 * The base time units for events are ticks. A tick equals 1 us on every platform
 * supported by Connect. Note, however, that the accuracy of the base tick
 * depends on the timer source, which by default is the LF RC oscillator on
 * EFR32 platforms.
 *
 * Furthermore, the scheduled delay is the minimum delay. If ::emberRunEvents()
 * or ::emberRunTask() are not called frequently enough, the actual delay may be
 * longer than the scheduled delay.
 *
 * Additionally, the APIs for quarter second and minute delays
 * (::emberEventControlSetDelayQS() and ::emberEventControlSetDelayMinutes())
 * use "binary" units. One quarter second is 256 ticks and one minute is 65536
 * ticks. These APIs are therefore doesn't actually mean a quarter of second or
 * a minute on platforms supported by Connect.
 *
 * However, in the future, Connect support might become available on platforms
 * where one tick is not exactly 1 us. For example,. on the EM357 SoC, 1 ms is 1024 ticks,
 * so each tick is 1000 / 1024 = ~0.98 milliseconds. If you need platform
 * independent accurate delays, use the macros
 * ::MILLISECOND_TICKS_PER_SECOND and ::MILLISECOND_TICKS_PER_MINUTE. For
 * example, calling
 * ::emberEventControlSetDelayMS(someEvent, 3 * MILLISECOND_TICKS_PER_MINUTE)
 * will delay for 3 minutes on any platform.
 *
 * The following are brief usage examples.
 * @code
 * EmberEventControl delayEvent;
 * EmberEventControl signalEvent;
 * EmberEventControl periodicEvent;
 *
 * void delayEventHandler(void)
 * {
 *   // Disable this event until its next use.
 *   emberEventControlSetInactive(delayEvent);
 * }
 *
 * void signalEventHandler(void)
 * {
 *   // Disable this event until its next use.
 *   emberEventControlSetInactive(signalEvent);
 *
 *   // Sometimes an action has to occur 100 ms later.
 *   if (somethingIsExpected)
 *     emberEventControlSetDelayMS(delayEvent, 100);
 * }
 *
 * void periodicEventHandler(void)
 * {
 *   emberEventControlSetDelayQS(periodicEvent, 4);
 * }
 *
 * void someIsr(void)
 * {
 *   // Set the signal event to run at the first opportunity.
 *   emberEventControlSetActive(signalEvent);
 * }
 *
 * // Put the controls and handlers in an array.  They will be run in
 * // this order (this is usually generated)
 * EmberEventData events[] =
 *  {
 *    { &delayEvent,    delayEventHandler },
 *    { &signalEvent,   signalEentHandler },
 *    { &periodicEvent, periodicEventHandler },
 *    { NULL, NULL }                            // terminator
 *  };
 *
 * void main(void)
 * {
 *   // Cause the periodic event to occur once a second.
 *   emberEventControlSetDelayQS(periodicEvent, 4);
 *
 *   while (TRUE) {
 *     emberRunEvents(events);
 *   }
 * }
 * @endcode
 *
 * @{
 */

// Controlling events

// Possible event status values.  Having zero as the 'inactive' value
// causes events to initially be inactive.
//
#ifndef __EVENT_H__
#define __EVENT_H__

/**
 * @brief The number of event tasks that can be used to schedule and run
 * events. Connect stack requires one, while another is used for Application
 * Framework events.
 */
#define EMBER_TASK_COUNT (3)

/**
 * @brief Set ::EmberEventControl as inactive (no pending event).
 *
 * @param[in] control Control of the event to set inactive.
 */
#define emberEventControlSetInactive(control) \
  do { (control).status = EMBER_EVENT_INACTIVE; } while (0)

/**
 * @brief Check whether ::EmberEventControl is currently active. An event
 * is considered active if it is set to run some time in the future (activated
 * by @ref emberEventControlSetActive(), @ref emberEventControlSetDelayMS()
 * or any other emberEventControlSetDelay* functions)
 *
 * @param[in] control Control of the event in question.
 *
 * @return
 *   Returns @b true if the event is active @b false otherwise
 */
#define emberEventControlGetActive(control) \
  ((control).status != EMBER_EVENT_INACTIVE)

/**
 * @brief Set ::EmberEventControl to run at the next available
 * opportunity.
 *
 * @param[in] control Control of the event to set active.
 */
#define emberEventControlSetActive(control) \
  do { emEventControlSetActive(&(control)); } while (0)

/**
 * @copybrief emberEventControlSetActive
 * @param[in] event Pointer to the control of the event to set active
 * @warning Applications should use @ref emberEventControlSetActive() instead.
 */
void emEventControlSetActive(EmberEventControl *event);

/**
 * @brief The maximum delay that may be passed to
 * ::emberEventControlSetDelayMS().
 */
#define EMBER_MAX_EVENT_CONTROL_DELAY_MS (HALF_MAX_INT32U_VALUE - 1)

/**
 * @brief Set ::EmberEventControl to run some milliseconds in the future.
 * @param[in] control Control of the event to run.
 * @param[in] delay
 *   The delay in milliseconds. Must be less than
 *   @ref EMBER_MAX_EVENT_CONTROL_DELAY_MS
 */
#define emberEventControlSetDelayMS(control, delay) \
  do { emEventControlSetDelayMS(&(control), (delay)); } while (0)

/**
 * @copybrief emberEventControlSetDelayMS
 * @param[in] event Pointer to the control of the event to run.
 * @param[in] delay
 *   The delay in milliseconds. Must be less than
 *   @ref EMBER_MAX_EVENT_CONTROL_DELAY_MS
 * @warning Applications should use @ref emberEventControlSetDelayMS() instead.
 */
void emEventControlSetDelayMS(EmberEventControl*event, uint32_t delay);

/** @brief The maximum delay that may be passed to
 * ::emberEventControlSetDelayQS().
 */
#define EMBER_MAX_EVENT_CONTROL_DELAY_QS (EMBER_MAX_EVENT_CONTROL_DELAY_MS >> 8)

/**
 * @brief Set ::EmberEventControl to run some quarter seconds in the
 * future.
 * @param[in] control Control of the event to run.
 * @param[in] delay
 *   The delay in quarter seconds. One quarter second is actually 256 ms. Must be
 *   less than  @ref EMBER_MAX_EVENT_CONTROL_DELAY_QS
 * @warning Applications should use @ref emberEventControlSetDelayQS() instead.
 */
#define emberEventControlSetDelayQS(control, delay) \
  do { emEventControlSetDelayMS(&(control), (delay) << 8); } while (0)

/** @brief The maximum delay that may be passed to
 * ::emberEventControlSetDelayMinutes().
 */
#define EMBER_MAX_EVENT_CONTROL_DELAY_MINUTES (EMBER_MAX_EVENT_CONTROL_DELAY_MS >> 16)

/**
 * @brief Set ::EmberEventControl to run some minutes in the future.
 * @param[in] control Control of the event to run.
 * @param[in] delay
 *   The delay in minute. One minute is actually 65536 ms. Must be
 *   less than  @ref EMBER_MAX_EVENT_CONTROL_DELAY_MINUTES
 */
#define emberEventControlSetDelayMinutes(control, delay) \
  do { emEventControlSetDelayMS(&(control), (delay) << 16); } while (0)

/**
 * @brief Check when the event is scheduled to run.
 * @param[in] control Control of the event in question.
 * @returns
 *   Returns the amount of milliseconds remaining before the event is scheduled
 *   to run. If the event is inactive, @ref MAX_INT32U_VALUE is returned.
 */
#define emberEventControlGetRemainingMS(control) \
  (emEventControlGetRemainingMS(&(control)))

/**
 * @copybrief emberEventControlGetRemainingMS
 * @param[in] event Pointer to the control of the event in question.
 * @returns
 *   Return the amount of milliseconds remaining before the event is scheduled
 *   to run. If the event is inactive, @ref MAX_INT32U_VALUE is returned.
 * @warning Applications should use @ref emberEventControlGetRemainingMS()
 * instead.
 */
uint32_t emEventControlGetRemainingMS(EmberEventControl *event);

// Running events

/**
 * @brief Start an event handler if anything is scheduled when this function is
 * called.
 *
 * An application typically creates an array of events along with their
 * handlers. This function should be called in the main loop to run those events.
 * @param[in] events Pointer to the array of events.
 *
 * @warning This is normally handled by @ref emberRunTask() in the main plugin.
 */
void emberRunEvents(EmberEventData *events);

/**
 * @brief Start an event handler if there is anything scheduled at the moment
 * this function is called.
 *
 * If an application has initialized a task via @ref emberTaskInit(), to
 * run the events associated with that task, it should call @ref emberRunTask()
 * instead of @ref emberRunEvents().
 *
 * @warning This is normally handled by the main plugin.
 *
 */
void emberRunTask(EmberTaskId taskid);

/**
 * @brief Check when the next event is scheduled to run.
 * @param[in] events An array of events to check.
 * @param[in] maxMs
 *   If no event is scheduled before maxMs, maxMs will be returned
 * @return
 *   Returns the number of milliseconds before the next event is scheduled to
 *   expire, or @p maxMs if no event is scheduled to expire within that time.
 * @note
 *   If any events are modified within an interrupt, to guarantee
 *   the accuracy of this API, it must be called with interrupts disabled.
 * @sa emberMsToNextEventExtended()
 */
uint32_t emberMsToNextEvent(EmberEventData *events, uint32_t maxMs);

/**
 * @copybrief emberMsToNextEvent
 * @param[in] events An array of events to check.
 * @param[in] maxMs
 *   If no event is scheduled before maxMs, maxMs will be returned
 * @param[out] returnIndex If not NULL pointer was passed, the index of the next
 *   event will be returned here, or 0xFF if no event is scheduled before
 *   maxMs.
 * @return
 *   Returns the number of milliseconds before the next event is scheduled to
 *   expire, or @p maxMs if no event is scheduled to expire within that time.
 * @note
 *   If any events are modified within an interrupt, to guarantee
 *   the accuracy of this API, it must be called with interrupts disabled.
 * @sa emberMsToNextEvent()
 */
uint32_t emberMsToNextEventExtended(EmberEventData *events, uint32_t maxMs, uint8_t* returnIndex);

/**
 * @brief Check when the next stack event is scheduled to run.
 * @return
 *   Returns the number of milliseconds before the next stack event is
 *   scheduled to run.
 */
uint32_t emberMsToNextStackEvent(void);

/**
 * @brief Initialize a task for managing events and processor idling state.
 * @param[in] events Pointer to the array of events to manage
 * @return
 *   Returns the @ref EmberTaskId which represents the newly created task.
 * @note After the task is created @ref emberRunTask() should be called
 * periodically.
 */
EmberTaskId emberTaskInit(EmberEventData *events);

/**
 * @brief Try to idle the CPU, unless any events in any tasks are pending.
 * @param[in] taskid the task which should handle the idling.
 * @return
 *   Returns @b true if the processor was idled @b false if idling wasn't
 *   permitted because a task has something to do.
 * @note
 *   This API should always be called with interrupts disabled. It will forcibly
 *   re-enable interrupts before returning.
 */
bool emberMarkTaskIdle(EmberTaskId taskid);

#ifndef EMBER_NO_IDLE_SUPPORT
/**
 * @brief Enable or disable idling.
 * @param[in] allow
 *    Setting it to @b true will enable, while setting it to @b false will
 *    disable idling.
 */
#define emberTaskEnableIdling(allow) \
  do { emTaskEnableIdling((allow)); } while (0)

/**
 * @copydoc emberTaskEnableIdling
 * @warning Applications should use @ref emberTaskEnableIdling() instead.
 */
void emTaskEnableIdling(bool allow);

/**
 * @brief Calling it indicates that a task has something to do, so it should
 * prevent the CPU from idling until @ref emberMarkTaskIdle is next called on this
 * task.
 * @param[in] taskid The task to mark active.
 */
#define emberMarkTaskActive(taskid) \
  do { emMarkTaskActive((taskid)); } while (0)

/**
 * @copydoc emberMarkTaskActive
 * @warning Applications should use @ref emberMarkTaskActive() instead.
 */
void emMarkTaskActive(EmberTaskId taskid);
#else
  #define emberTaskEnableIdling(allow)  do {} while (0)
  #define emberMarkTaskActive(taskid)   do {} while (0)
#endif // EMBER_NO_IDLE_SUPPORT

#endif // __EVENT_H__

/** @} END addtogroup */
