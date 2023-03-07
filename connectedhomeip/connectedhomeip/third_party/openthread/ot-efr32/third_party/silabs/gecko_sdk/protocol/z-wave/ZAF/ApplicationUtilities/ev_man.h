/**
 * @file ev_man.h
 * @brief Framework event manager, handling all event types for the framework
 * @copyright 2018 Silicon Laboratories Inc.
 */

#ifndef _ev_man_H_
#define _ev_man_H_

/****************************************************************************/
/*                              INCLUDE FILES                               */
/****************************************************************************/
#include <ZW_basis_api.h>

/****************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                       */
/****************************************************************************/

/**
 * Event groups for application state event machine
 */
#define DEFINE_EVENT_WAKEUP_NBR 0x01
#define DEFINE_EVENT_KEYPAD_NBR 0x10
#define DEFINE_EVENT_IR_NBR 0x20
#define DEFINE_EVENT_SYSTEM_NBR 0x30

/**
 * Event group for keys 0x40 - 0x6F
 */
#define DEFINE_EVENT_KEY_NBR 0x40
#define DEFINE_EVENT_APP_NBR 0x70


/**
 * System events
 */
typedef enum _EVENT_SYSTEM_
{
  EVENT_SYSTEM_RESET = DEFINE_EVENT_SYSTEM_NBR,
  EVENT_SYSTEM_LEARNMODE_START,      //Enqueue to manually start learn mode.
  EVENT_SYSTEM_LEARNMODE_FINISHED,   //Enqueued after learn process finished. Both on success and timeout.
  EVENT_SYSTEM_LEARNMODE_STOP,       //Enqueue to manually stop learn mode.
  EVENT_SYSTEM_WATCHDOG_RESET,
  EVENT_SYSTEM_OTA_START,
  EVENT_SYSTEM_LEARNMODE_DSK_START
} EVENT_SYSTEM;

/**
 * Button events
 *
 * NB: For every button there MUST be the following events in this order:
 *     DOWN, UP, SHORT_PRESS, HOLD, LONG_PRESS
 *     The macros BTN_EVENT_xxx in board.h depends on it.
 */
typedef enum _BUTTON_EVENT_
{
  EVENT_PB1_DOWN = DEFINE_EVENT_KEY_NBR, // 64
  EVENT_PB1_UP,
  EVENT_PB1_SHORT_PRESS,
  EVENT_PB1_HOLD,
  EVENT_PB1_LONG_PRESS,
  EVENT_PB2_DOWN,
  EVENT_PB2_UP,
  EVENT_PB2_SHORT_PRESS,
  EVENT_PB2_HOLD,
  EVENT_PB2_LONG_PRESS,
  EVENT_PB3_DOWN,
  EVENT_PB3_UP,
  EVENT_PB3_SHORT_PRESS,
  EVENT_PB3_HOLD,
  EVENT_PB3_LONG_PRESS,
  EVENT_PB4_DOWN,
  EVENT_PB4_UP,
  EVENT_PB4_SHORT_PRESS,
  EVENT_PB4_HOLD,
  EVENT_PB4_LONG_PRESS,
  EVENT_PB5_DOWN,
  EVENT_PB5_UP,
  EVENT_PB5_SHORT_PRESS,
  EVENT_PB5_HOLD,
  EVENT_PB5_LONG_PRESS,
  EVENT_PB6_DOWN,
  EVENT_PB6_UP,
  EVENT_PB6_SHORT_PRESS,
  EVENT_PB6_HOLD,
  EVENT_PB6_LONG_PRESS,
  EVENT_SLIDER1_DOWN,
  EVENT_SLIDER1_UP,
  EVENT_SLIDER1_SHORT_PRESS, // Not used, but don't delete because of BTN_EVENT_xxx macros
  EVENT_SLIDER1_HOLD,        // Not used, but don't delete because of BTN_EVENT_xxx macros
  EVENT_SLIDER1_LONG_PRESS,  // Not used, but don't delete because of BTN_EVENT_xxx macros
  EVENT_BTN_MAX /**< EVENT_BTN_MAX define the last enum type*/
} BUTTON_EVENT;

#define BTN_EVENT_FILTER(btnEvent)          ((btnEvent >= DEFINE_EVENT_KEY_NBR) && (btnEvent < EVENT_BTN_MAX))

/****************************************************************************/
/*                              EXPORTED DATA                               */
/****************************************************************************/


/****************************************************************************/
/*                           EXPORTED FUNCTIONS                             */
/****************************************************************************/

#define EventSchedulerInit ZAF_eventSchedulerInit
/**
 * @brief Initializes event scheduler.
 */
void ZAF_eventSchedulerInit(VOID_CALLBACKFUNC(pApplicationStateMachine)(uint8_t));


#define ZCB_EventSchedulerEventAdd ZCB_eventSchedulerEventAdd
/**
 * @brief Adds a given event to the event queue.
 * @param event A given event.
 * @return true if given event is added to queue, false if queue full.
 */
bool ZCB_eventSchedulerEventAdd(uint8_t event);


#define ZCB_EventScheduler ZCB_eventScheduler
/**
 * @brief Processes events.
 */
bool ZCB_eventScheduler(void);


#define ZCB_EventEnqueue ZAF_jobEnqueue
/**
 * @brief Adds a given event to the job queue.
 * @param event A given event.
 * @return true if given event is added to queue, false if queue full.
 */
bool ZAF_jobEnqueue(uint8_t event);


#define ZCB_EventDequeue ZAF_jobDequeue
/**
 * @brief Deque job queue
 * @param[out] pEvent return event from the queue.
 * @return false if queue is empty else true.
 */
bool ZAF_jobDequeue(uint8_t* pEvent);


/**
 * @brief Get number of events on queue
 * @return number of events on queue
 */
uint8_t ZAF_jobQueueCount(void);

#endif /* _ev_man_H_ */


