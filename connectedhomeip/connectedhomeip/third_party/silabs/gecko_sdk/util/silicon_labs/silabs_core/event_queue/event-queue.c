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

// Originally I used a heap (complete binary tree stored in an array)
// for the scheduled events.  This took O(log(N)) time for adding an
// event or for removing the next event.  The simple list version here
// takes O(N) time for adding an event O(1) time for removing the next
// one.  It is faster for small numbers of events, because it is much
// simpler, but does poorly as the number of events goes up.
// Measurements showed that the crossover point is somewhere above 100
// scheduled events, which we should never reach.  So the simpler
// version is the way to go.
//
// If more speed is needed, the first step would be to add a pointer
// to the halfway point in the list.  This would cut the lookup time
// almost in half without adding much complexity.
#include PLATFORM_HEADER
#include "hal/hal.h"
#include "event-queue.h"

// We use this instead of NULL at the end of a list so that unscheduled
// events can be marked by having a 'next' field of NULL.  This makes them
// easier to initialize.
#define LIST_END EVENT_QUEUE_LIST_END

// Marker function for ISR event types.  This should never be called.
void emIsrEventMarker(struct Event_s *event)
{
  (void)event;

  assert(false);
}

void emInitializeEventQueue(EmberEventQueue *queue)
{
  queue->isrEvents = NULL;
  queue->events = LIST_END;
}

static void cancelEvents(EmberEvent *events)
{
  while (events != LIST_END) {
    EmberEvent *temp = events->next;
    events->next = NULL;
    events = temp;
  }
}

void emberCancelAllEvents(EmberEventQueue *queue)
{
  while (true) {
    EmberEvent *events = LIST_END;
    ATOMIC(
      if (queue->isrEvents != NULL) {
      events = queue->isrEvents->next;
      queue->isrEvents->next = LIST_END;
      queue->isrEvents = NULL;
    }
      );
    if (events == LIST_END) {
      break;
    }
    cancelEvents(events);
  }
  cancelEvents(queue->events);
  queue->events = LIST_END;
}

#ifdef EMBER_TEST
static UNUSED void printEvents(EmberEvent *events, bool *firstLoc)
{
  for (; events != LIST_END; events = events->next) {
    fprintf(stderr, "%s%s: %d",
            *firstLoc ? "[" : " | ",
            (events->actions.name == NULL
             ? "?"
             : events->actions.name),
            events->timeToExecute);
    *firstLoc = false;
  }
}

// This is not thread safe.

static UNUSED void printEventQueue(EmberEventQueue *queue)
{
  bool first = true;
  EmberEvent *isrEnd = queue->isrEvents;
  if (isrEnd != NULL) {
    EmberEvent *isrStart = isrEnd->next;
    isrEnd->next = LIST_END;
    printEvents(isrStart, &first);
    isrEnd->next = isrStart;
  }
  printEvents(queue->events, &first);
  if (first) {
    fprintf(stderr, "[]\n");
  }
}
#endif // ifdef EMBER_TEST

bool emberEventIsScheduled(EmberEvent *event)
{
  return event->next != NULL;
}

bool emberEventQueueIsEmpty(EmberEventQueue *queue)
{
  return queue->events == LIST_END;
}

uint32_t emberEventGetRemainingMs(EmberEvent *event)
{
  if (event->next == NULL) {
    return -1;
  } else if (event->actions.marker == emIsrEventMarker) {
    return 0;
  } else {
    uint32_t remaining =
      event->timeToExecute - halCommonGetInt32uMillisecondTick();
    if (remaining < HALF_MAX_INT32U_VALUE) {
      return remaining;
    } else {
      return 0;         // we're behind in running the event
    }
  }
}

static void adjustListLocation(EmberEventQueue *queue, EmberEvent *event, bool keep)
{
  EmberEvent *previous = (EmberEvent *) queue;
  EmberEvent *finger = queue->events;
  EmberEvent *newLocation = NULL;
  assert(event->next != event);

  // Find 'event' in the list, noting the new location if we come across it.
  while (finger != event) {
    if (keep
        && newLocation == NULL
        && !timeGTorEqualInt32u(event->timeToExecute, finger->timeToExecute)) {
      newLocation = previous;
    }
    previous = finger;
    finger = finger->next;
  }

  // Remove 'event' from its current location.
  if (previous == (EmberEvent *) queue) {
    queue->events = event->next;
  } else {
    previous->next = event->next;
  }

  if (keep) {
    // If we haven't found the new location it must be after the old
    // location, so we have to keep looking for it.
    if (newLocation == NULL) {
      newLocation = previous;
      finger = event->next;
      assert(event->next != event);
      while (finger != LIST_END
             && timeGTorEqualInt32u(event->timeToExecute,
                                    finger->timeToExecute)) {
        newLocation = finger;
        finger = finger->next;
      }
    }

    // Put 'event' in its new location.
    if (newLocation == (EmberEvent *) queue) {
      event->next = queue->events;
      queue->events = event;
    } else {
      event->next = newLocation->next;
      newLocation->next = event;
    }
  }
}

uint32_t emberMsToNextQueueEvent(EmberEventQueue *queue)
{
  if (queue->isrEvents != NULL) {
    return 0;
  } else if (queue->events == LIST_END) {
    return -1;
  } else {
    return emberEventGetRemainingMs(queue->events);
  }
}

void emberRunEventQueue(EmberEventQueue *queue)
{
  uint32_t now = halCommonGetInt32uMillisecondTick();
  queue->runTime = now;
  queue->running = true;
  while (true) {
    EmberEvent *event = LIST_END;
    ATOMIC(
      if (queue->isrEvents != NULL) {
      event = queue->isrEvents->next;
      if (event->next == event) {
        queue->isrEvents = NULL;
      } else {
        queue->isrEvents->next = event->next;
      }
      event->next = NULL;
    } else if (queue->events != LIST_END
               && timeGTorEqualInt32u(now, queue->events->timeToExecute)) {
      event = queue->events;
      queue->events = event->next;
      event->next = NULL;
    }
      )
    if (event == LIST_END) {
      break;
    } else {
      event->actions.handler(event);
    }
  }
  queue->running = false;
}

#ifdef EVENT_QUEUE_SUPPORTS_BUFFER_MARKING
// Mark any of the events that are also buffers, and call any marker actions.

void emberMarkEventQueue(EmberEventQueue *queue)
{
  EmberEvent **location = &queue->events;
  while (*location != LIST_END) {
    EmberEvent *event = *location;
    if (event->actions.marker == NULL) {
      location = &event->next;
    } else {
      emMarkBufferPointer((void **) location);
      location = &event->next;
      event->actions.marker(event);
    }
  }
}
#endif // EVENT_QUEUE_SUPPORTS_BUFFER_MARKING

// If the event is ready to run, and the new time doesn't change this,
// then just leave the event where it is.  This is done to avoid shuffling
// events when setActive() is called multiple times on the same event.

void emberEventSetDelayMs(EmberEvent *event, uint32_t delay)
{
  EmberEventQueue *queue = event->actions.queue;
  if (event->actions.marker == emIsrEventMarker) {
    assert(delay == 0);
    ATOMIC(
      if (event->next != NULL) {
      // already scheduled, do nothing
    } else if (queue->isrEvents == NULL) {
      event->next = event;
      queue->isrEvents = event;
    } else {
      event->next = queue->isrEvents->next;
      queue->isrEvents->next = event;
      queue->isrEvents = event;
    }
      );
    // Issue a callback to the application so that it knows that an event has
    // happened in ISR context and we should recompute delay times.
    // sToDo: this next call should be replaced
    // emApiEventDelayUpdatedFromIsrHandler(event);
  } else {
    uint32_t now = halCommonGetInt32uMillisecondTick();
    uint32_t timeToExecute;

    if (delay < EMBER_MAX_EVENT_DELAY_MS) {
      timeToExecute = now + delay;
      if (queue->running
          && queue->runTime == timeToExecute) {
        // Avoid infinite loops by allowing events to run only once within
        // any one call to emberRunEventQueue().
        timeToExecute += 1;
      }
    } else {
      timeToExecute = now + EMBER_MAX_EVENT_DELAY_MS;
    }

    if (emberEventIsScheduled(event)) {
      if (timeToExecute == event->timeToExecute
          || (delay == 0
              && timeGTorEqualInt32u(now, event->timeToExecute))) {
        return;           // avoid unnecessary shuffling of events
      }
    } else {
      event->next = queue->events;
      queue->events = event;
    }
    event->timeToExecute = timeToExecute;
    adjustListLocation(queue, event, true);
  }
}

void emberEventSetInactive(EmberEvent *event)
{
  EmberEventQueue *queue = event->actions.queue;
  if (event->actions.marker == emIsrEventMarker) {
    ATOMIC(
      if (event->next == NULL) {
      // do nothing
    } else if (event->next == event) {
      // only event in the queue
      queue->isrEvents = NULL;
      event->next = NULL;
    } else {
      EmberEvent *previous = event->next;
      while (previous->next != event) {
        previous = previous->next;
      }
      if (queue->isrEvents == event) {
        queue->isrEvents = previous;
      }
      previous->next = event->next;
      event->next = NULL;
    }
      );
  } else if (emberEventIsScheduled(event)) {
    EmberEventQueue *queue = event->actions.queue;
    adjustListLocation(queue, event, false);
    event->next = NULL;
  }
}

EmberEvent *emFindEvents(EmberEventQueue *queue,
                         EmberEventActions *actions,
                         EmberEventPredicate predicate,
                         void *data,
                         bool all)
{
  EmberEvent *previous = (EmberEvent *) queue;    // any non-NULL, non-Event value works
  EmberEvent *finger = queue->events;
  EmberEvent *result = NULL;

  while (finger != LIST_END) {
    EmberEvent *next = finger->next;
    if (&(finger->actions) == actions
        && (predicate == NULL
            || predicate(finger, data))) {
      if (previous == (EmberEvent *) queue) {
        queue->events = next;
      } else {
        previous->next = next;
      }
      finger->next = result;
      if (!all) {
        return finger;
      }
      result = finger;
    } else {
      previous = finger;
    }
    finger = next;
  }
  return result;
}

EmberEvent *emFindEventWithoutRemoving(EmberEventQueue *queue,
                                       EmberEventActions *actions,
                                       EmberEventPredicate predicate,
                                       void *data)
{
  EmberEvent *finger = queue->events;

  while (finger != LIST_END) {
    EmberEvent *next = finger->next;
    if (&(finger->actions) == actions
        && (predicate == NULL
            || predicate(finger, data))) {
      return finger;
    }

    finger = next;
  }

  return NULL;
}
