/***************************************************************************//**
 * @file
 * @brief schedulable events
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

#ifndef SILABS_EVENT_QUEUE_H
#define SILABS_EVENT_QUEUE_H

// Forward declarations to make up for C's one-pass type checking.
struct Event_s;
struct EventQueue_s;

// We use this instead of NULL at the end of a list so that unscheduled
// events can be marked by having a 'next' field of NULL.  This makes them
// easier to initialize.
#define EVENT_QUEUE_LIST_END ((EmberEvent *) 1)

/** @brief The static part of an event.  Each event can be used with only one
 * event queue.
 */

typedef struct {
  struct EventQueue_s *queue;           // the queue this event goes on
  void (*handler)(struct Event_s *);    // called when the event fires
  void (*marker)(struct Event_s *);     // marking function, can be NULL
  const char *name;                     // event name for debugging purposes
} EmberEventActions;

typedef struct Event_s {
  EmberEventActions actions;            // static data

  // For internal use only, but the 'next' field must be initialized
  // to NULL.
  struct Event_s *next;
  uint32_t timeToExecute;

  // Owned by the end user
  uint32_t data;                        // a generic 4-byte data field
  void *dataPtr;                        // a generic pointer
} EmberEvent;

/** @brief An event queue is currently just a list of events ordered by
 * execution time.
 */
typedef struct EventQueue_s {
  EmberEvent *isrEvents;
  EmberEvent *events;
  uint32_t runTime;
  bool running;
} EmberEventQueue;

/**
 * @addtogroup event
 *
 * @{

 * An event queue is a queue of events that will be run at some future time(s).
 * The Event type is declared in ember-types.h and has three fields:
 *
   @verbatim
   EmberEventActions *actions;      // see below
   EmberEvent *next;                // internal use only, but must be
                                    //  initialized to NULL
   uint32_t timeToExecute;       // internal use only
   @endverbatim
 *
 * The EmberEventAction struct also has three fields:
 *
   @verbatim
   struct EventQueue_s *queue; // the queue this event goes on
   void (*handler)(Event *);   // called when the event fires
   void (*marker)(Event *);    // marking fuction, can be NULL
   @endverbatim
 *
 * The marker function is needed if an event refers to a buffer or is
 * itself a buffer; see the dynamic event example below.
 *
 *
 * Sample static event declaration:
 *
   @verbatim
   static void fooEventHandler(Event *event);
   static EmberEventActions fooEventActions = {
   &emStackEventQueue,
   fooEventHandler,
   NULL          // no marking function is needed
   };

   Initialize the event as unscheduled.
   static EmberEvent fooEvent = { &fooEventActions, NULL };

   static void fooEventHandler(Event *event)
   {
   ...
   }
   @endverbatim
 *
 * Sample dynamic event:
   @verbatim

   typedef struct {
   EmberEvent event;
   PacketHeader someBuffer;
   // plus other values as needed
   } BarEvent;

   static void barEventHandler(BarEvent *event);
   static void barEventMarker(BarEvent *event);

   static EmberEventActions barEventActions = {
   &emStackEventQueue,
   (void (*)(Event *)) barEventHandler,
   (void (*)(Event *)) barEventMarker // required for all dynamic events
   };

   static void barEventHandler(BarEvent *event)
   {
   ...
   };

   static void barEventMarker(BarEvent *event)
   {
   // The Buffer that holds BarEvent itself is not marked, but any Buffers
   // within the BarEvent must be marked.  If BarEvent doesn't contain any
   // Buffers, the marking function is still required, but will be empty.
   emMarkBuffer(&event->someBuffer);
   }

   static void addBarEvent(Buffer someBuffer, uint32_t delayMs)
   {
   Buffer eventBuffer = emAllocateBuffer(sizeof(BarEvent));
   if (eventBuffer == NULL_BUFFER) {
     return;
   }
   BarEvent *barEvent = (BarEvent *) emGetBufferPointer(eventBuffer);
   barEvent->event.actions = &barEventActions;
   barEvent->event.next = NULL;       // initialize as unscheduled
   barEvent->someBuffer = someBuffer;
   emberEventSetDelayMs((Event *) barEvent, delayMs);
   }
   @endverbatim

 * Cancelling a dynamic event:
   @verbatim

   static bool barEventPredicate(EmberEvent *event,
                                 void *castBuffer)
   {
   return (((BarEvent *) event)->someBuffer) == (Buffer) castBuffer;
   }

   static void cancelBarEvent(Buffer someBuffer)
   {
   emberFindEvent(&emStackEventQueue,
                  &barEventActions,
                  barEventPredicate,
                  (void *) someBuffer);
   }
   @endverbatim
 * Delays greater than this will cause the scheduled execution time to
 * wrap around into the past and the event will be executed immediately.
 *
 * Event Queue operations
 */

#define EMBER_MAX_EVENT_DELAY_MS (HALF_MAX_INT32U_VALUE - 1)

/**
 * Marker function for ISR event types.  Events that are scheduled from
 * ISR context must use this as their marker function (and must not
 * required an actual marker function.
 *
 * Events with this in their type may only be scheduled with a zero delay.
 */
void emIsrEventMarker(struct Event_s *event);

/**
 * Initialize 'queue'.  This must be called before any use of 'queue'
 * or any of its events.
 */
void emInitializeEventQueue(EmberEventQueue *queue);

/**
 * Unschedule all the events.
 */
void emberCancelAllEvents(EmberEventQueue *queue);

/**
 * Return the number of milliseconds until the next event fires, or -1 if
 * there are no scheduled events.
 */
uint32_t emberMsToNextQueueEvent(EmberEventQueue *queue);

/**
 * Run any and all events which have fired since the last time this queue's
 * events were run.
 */
void emberRunEventQueue(EmberEventQueue *queue);

/**
 * If any scheduled events have mark functions, mark those events, if
 * they are buffers, and call the mark functions.
 */
void emberMarkEventQueue(EmberEventQueue *queue);

/**
 * Returns the first scheduled event that has 'actions' and for which
 * 'predicate' returns true.  The returned event has been cancelled.
 */
#define emberFindEvent(queue, actions, predicate, data) \
  (emFindEvents((queue), &(actions), (predicate), (data), false))

/**
 * Returns the scheduled events that have 'actions' and for which
 * 'predicate' returns true.  If 'predicate' is NULL then all events
 * that have 'actions' are returned.  The returned events are linked
 * through the 'next' field and have been cancelled.
 *
 * WARNING: the 'next' field of the returned events must be set to
 * NULL before the event is passed to any event operation.
 */

#define emberFindAllEvents(queue, actions, predicate, data) \
  (emFindEvents((queue), &(actions), (predicate), (data), true))

typedef bool (*EmberEventPredicate)(EmberEvent *, void *);

EmberEvent *emFindEvents(EmberEventQueue *queue,
                         EmberEventActions *actions,
                         EmberEventPredicate predicate,
                         void *data,
                         bool all);

/**
 * Returns the first scheduled event that has 'actions' and for which
 * 'predicate' returns true.  The returned event is not cancelled.
 */
EmberEvent *emFindEventWithoutRemoving(EmberEventQueue *queue,
                                       EmberEventActions *actions,
                                       EmberEventPredicate predicate,
                                       void *data);

/**
 * Event operations
 */

/**
 * Schedule 'event' to run after 'delay' milliseconds.  Delays greater than
 * EMBER_MAX_EVENT_DELAY_MS will be reduced to EMBER_MAX_EVENT_DELAY_MS.
 */
void emberEventSetDelayMs(EmberEvent *event, uint32_t delay);

/**
 * Schedule 'event' to run immediately.
 */
#define emberEventSetActive(event) emberEventSetDelayMs((event), 0)

/**
 * Cancel 'event'.
 */
void emberEventSetInactive(EmberEvent *event);

/**
 * Return true if the event is scheduled to run.
 */
bool emberEventIsScheduled(EmberEvent *event);

/**
 * Return true if the event queue is empty.
 */
bool emberEventQueueIsEmpty(EmberEventQueue *queue);

/**
 * Return the number of milliseconds before 'event' runs, or -1 if 'event'
 * is not scheduled to run.
 */
uint32_t emberEventGetRemainingMs(EmberEvent *event);

/**
 * @}
 */

#endif // SILABS_EVENT_QUEUE_H
