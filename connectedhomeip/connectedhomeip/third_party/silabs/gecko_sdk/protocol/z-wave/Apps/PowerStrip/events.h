/**
 * @file
 *
 * Definitions of events for Power Strip Certified App.
 *
 * @copyright 2019 Silicon Laboratories Inc.
 */
#ifndef APPS_POWERSTRIP_EVENTS_H_
#define APPS_POWERSTRIP_EVENTS_H_

#include <ev_man.h>

/**
 * Defines events for the application.
 *
 * These events are not referred to anywhere else than in the application. Hence, they can be
 * altered to suit the application flow.
 *
 * The events are located in a separate file to make it possible to include them in other
 * application files. An example could be a peripheral driver that enqueues an event when something
 * specific happens, e.g. on electric overload.
 */
typedef enum EVENT_APP_POWER_STRIP
{
  EVENT_EMPTY = DEFINE_EVENT_APP_NBR,
  EVENT_APP_INIT,
  EVENT_APP_REFRESH_MMI,
  EVENT_APP_FLUSHMEM_READY,
  EVENT_APP_NEXT_EVENT_JOB,
  EVENT_APP_FINISH_EVENT_JOB,
  EVENT_APP_SEND_OVERLOAD_NOTIFICATION,
  EVENT_APP_SMARTSTART_IN_PROGRESS,
  EVENT_APP_BUTTON_LEARN_RESET_SHORT_PRESS,
  EVENT_APP_BUTTON_LEARN_RESET_LONG_PRESS,
  EVENT_APP_BUTTON_OUTLET1_TOGGLE,
  EVENT_APP_BUTTON_OUTLET2_DIMMER_SHORT_PRESS,
  EVENT_APP_BUTTON_OUTLET2_DIMMER_HOLD,
  EVENT_APP_BUTTON_OUTLET2_DIMMER_RELEASE,
  EVENT_APP_BUTTON_NOTIFICATION_TOGGLE,
}
EVENT_APP;

#endif /* APPS_POWERSTRIP_EVENTS_H_ */
