/******************************************************************************

 @file  osal_timers.c

 @brief OSAL Timer definition and manipulation functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2004-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 * INCLUDES
 */

#include "comdef.h"
#include "onboard.h"
#include "osal.h"
#include "osal_timers.h"
#include "hal_timer.h"
#include "osal_clock.h"

#ifdef USE_ICALL
#ifdef ICALL_JT
  #include "icall_jt.h"
#else
  #include <icall.h>
#endif /* ICALL_JT */
#endif /* USE_ICALL */
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */
#define MILLS_IN_MICROS                   1000

/*********************************************************************
 * TYPEDEFS
 */

typedef union {
  uint32 time32;
  uint16 time16[2];
  uint8 time8[4];
} osalTime_t;

typedef struct
{
  void   *next;
  osalTime_t timeout;
  uint16 event_flag;
  uint8  task_id;
  uint32 reloadTimeout;
} osalTimerRec_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */

osalTimerRec_t *timerHead;

// Last read tick count value reflected into the OSAL timer
uint_least32_t osal_last_timestamp;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Milliseconds since last reboot
static uint32 osal_systemClock;

/*********************************************************************
 * LOCAL FUNCTION PROTOTYPES
 */
osalTimerRec_t  *osalAddTimer( uint8 task_id, uint16 event_flag, uint32 timeout );
osalTimerRec_t *osalFindTimer( uint8 task_id, uint16 event_flag );
void osalDeleteTimer( osalTimerRec_t *rmTimer );

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      osalTimerInit
 *
 * @brief   Initialization for the OSAL Timer System.
 *
 * @param   none
 *
 * @return
 */
void osalTimerInit( void )
{
  osal_systemClock = 0;

#ifdef USE_ICALL
  // Initialize variables used to track timing and provide OSAL timer service
  osal_last_timestamp = (uint_least32_t) ICall_getTicks();
#endif /* USE_ICALL */
}

/*********************************************************************
 * @fn      osalAddTimer
 *
 * @brief   Add a timer to the timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 * @param   timeout
 *
 * @return  osalTimerRec_t * - pointer to newly created timer
 */
osalTimerRec_t * osalAddTimer( uint8 task_id, uint16 event_flag, uint32 timeout )
{
  osalTimerRec_t *newTimer;
  osalTimerRec_t *srchTimer;

  // Look for an existing timer first
  newTimer = osalFindTimer( task_id, event_flag );
  if ( newTimer )
  {
    // Timer is found - update it.
    newTimer->timeout.time32 = timeout;

    return ( newTimer );
  }
  else
  {
    // New Timer
    newTimer = osal_mem_alloc( sizeof( osalTimerRec_t ) );

    if ( newTimer )
    {
      // Fill in new timer
      newTimer->task_id = task_id;
      newTimer->event_flag = event_flag;
      newTimer->timeout.time32 = timeout;
      newTimer->next = (void *)NULL;
      newTimer->reloadTimeout = 0;

      // Does the timer list already exist
      if ( timerHead == NULL )
      {
        // Start task list
        timerHead = newTimer;
      }
      else
      {
        // Add it to the end of the timer list
        srchTimer = timerHead;

        // Stop at the last record
        while ( srchTimer->next )
          srchTimer = srchTimer->next;

        // Add to the list
        srchTimer->next = newTimer;
      }

      return ( newTimer );
    }
    else
    {
      return ( (osalTimerRec_t *)NULL );
    }
  }
}

/*********************************************************************
 * @fn      osalFindTimer
 *
 * @brief   Find a timer in a timer list.
 *          Ints must be disabled.
 *
 * @param   task_id
 * @param   event_flag
 *
 * @return  osalTimerRec_t *
 */
osalTimerRec_t *osalFindTimer( uint8 task_id, uint16 event_flag )
{
  osalTimerRec_t *srchTimer;

  // Head of the timer list
  srchTimer = timerHead;

  // Stop when found or at the end
  while ( srchTimer )
  {
    if ( srchTimer->event_flag == event_flag &&
         srchTimer->task_id == task_id )
    {
      break;
    }

    // Not this one, check another
    srchTimer = srchTimer->next;
  }

  return ( srchTimer );
}

/*********************************************************************
 * @fn      osalDeleteTimer
 *
 * @brief   Delete a timer from a timer list.
 *
 * @param   table
 * @param   rmTimer
 *
 * @return  none
 */
void osalDeleteTimer( osalTimerRec_t *rmTimer )
{
  // Does the timer list really exist
  if ( rmTimer )
  {
    // Clear the event flag and osalTimerUpdate() will delete
    // the timer from the list.
    rmTimer->event_flag = 0;
  }
}

#ifdef USE_ICALL
/*********************************************************************
 * @fn      osal_timer_refTimeUpdate
 *
 * @brief
 *
 *   Update the reference time for the rtos tick. Since osal timer are relative
 *   to their start, it is important to do this in the following condition:
 *     - if the very first timer is started
 *     - if the RTOS systick has wrap around while no timer has been running
 *     and a new timer is started.
 *
 *    Once the reference time is found, all timers are updated with the
 *    time elapsed since last reference time
 * @param   None
 *
 * @return  None
 */
void osal_timer_refTimeUpdate (void)
{
    unsigned long newtimestamp = ICall_getTicks();
    uint32 milliseconds;

    if (osal_tickperiod == MILLS_IN_MICROS)
    {
      milliseconds = newtimestamp - osal_last_timestamp;
      osal_last_timestamp = newtimestamp;
    }
    else
    {
      // This code has been moved from osal.c and has ben keep as is on purpose.
      unsigned long long delta = (unsigned long long)
        ((newtimestamp - osal_last_timestamp) & 0xfffffffful);
      delta *= osal_tickperiod;
      delta /= MILLS_IN_MICROS;
      milliseconds = (uint32) delta;
      osal_last_timestamp += (uint32) (delta * MILLS_IN_MICROS / osal_tickperiod);
    }
    osalAdjustTimer(milliseconds);
}
#endif /* USE_ICALL */

/*********************************************************************
 * @fn      osal_start_timerEx
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 * @param   uint32 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
uint8 osal_start_timerEx( uint8 taskID, uint16 event_id, uint32 timeout_value )
{
  halIntState_t intState;
  osalTimerRec_t *newTimer;

#ifdef USE_ICALL
  if ( timerHead == NULL )
  {
    osal_timer_refTimeUpdate();
  }
#endif /* USE_ICALL */

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Add timer
  newTimer = osalAddTimer( taskID, event_id, timeout_value );

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      osal_start_reload_timer
 *
 * @brief
 *
 *   This function is called to start a timer to expire in n mSecs.
 *   When the timer expires, the calling task will get the specified event
 *   and the timer will be reloaded with the timeout value.
 *
 * @param   uint8 taskID - task id to set timer for
 * @param   uint16 event_id - event to be notified with
 * @param   UNINT16 timeout_value - in milliseconds.
 *
 * @return  SUCCESS, or NO_TIMER_AVAIL.
 */
uint8 osal_start_reload_timer( uint8 taskID, uint16 event_id, uint32 timeout_value )
{
  halIntState_t intState;
  osalTimerRec_t *newTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Add timer
  newTimer = osalAddTimer( taskID, event_id, timeout_value );
  if ( newTimer )
  {
    // Load the reload timeout value
    newTimer->reloadTimeout = timeout_value;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (newTimer != NULL) ? SUCCESS : NO_TIMER_AVAIL );
}

/*********************************************************************
 * @fn      osal_stop_timerEx
 *
 * @brief
 *
 *   This function is called to stop a timer that has already been started.
 *   If ZSUCCESS, the function will cancel the timer and prevent the event
 *   associated with the timer from being set for the calling task.
 *
 * @param   uint8 task_id - task id of timer to stop
 * @param   uint16 event_id - identifier of the timer that is to be stopped
 *
 * @return  SUCCESS or INVALID_EVENT_ID
 */
uint8 osal_stop_timerEx( uint8 task_id, uint16 event_id )
{
  halIntState_t intState;
  osalTimerRec_t *foundTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Find the timer to stop
  foundTimer = osalFindTimer( task_id, event_id );
  if ( foundTimer )
  {
    osalDeleteTimer( foundTimer );
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return ( (foundTimer != NULL) ? SUCCESS : INVALID_EVENT_ID );
}

/*********************************************************************
 * @fn      osal_get_timeoutEx
 *
 * @brief
 *
 * @param   uint8 task_id - task id of timer to check
 * @param   uint16 event_id - identifier of timer to be checked
 *
 * @return  Return the timer's tick count if found, zero otherwise.
 */
uint32 osal_get_timeoutEx( uint8 task_id, uint16 event_id )
{
  halIntState_t intState;
  uint32 rtrn = 0;
  osalTimerRec_t *tmr;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  tmr = osalFindTimer( task_id, event_id );

  if ( tmr )
  {
    rtrn = tmr->timeout.time32;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return rtrn;
}

/*********************************************************************
 * @fn      osal_timer_num_active
 *
 * @brief
 *
 *   This function counts the number of active timers.
 *
 * @return  uint8 - number of timers
 */
uint8 osal_timer_num_active( void )
{
  halIntState_t intState;
  uint8 num_timers = 0;
  osalTimerRec_t *srchTimer;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

  // Head of the timer list
  srchTimer = timerHead;

  // Count timers in the list
  while ( srchTimer != NULL )
  {
    num_timers++;
    srchTimer = srchTimer->next;
  }

  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  return num_timers;
}

/*********************************************************************
 * @fn      osalTimerUpdate
 *
 * @brief   Update the timer structures for a timer tick.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osalTimerUpdate( uint32 updateTime )
{
  halIntState_t intState;
  osalTimerRec_t *srchTimer;
  osalTimerRec_t *prevTimer;

  osalTime_t timeUnion;
  timeUnion.time32 = updateTime;

  HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.
  // Update the system time
  osal_systemClock += updateTime;
  HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

  // Look for open timer slot
  if ( timerHead != NULL )
  {
    // Add it to the end of the timer list
    srchTimer = timerHead;
    prevTimer = (void *)NULL;

    // Look for open timer slot
    while ( srchTimer )
    {
      osalTimerRec_t *freeTimer = NULL;

      HAL_ENTER_CRITICAL_SECTION( intState );  // Hold off interrupts.

      // To minimize time in this critical section, avoid 32-bit math
      if ((timeUnion.time16[1] == 0) && (timeUnion.time8[1] == 0))
      {
        // If upper 24 bits are zero, check lower 8 bits for roll over
        if (srchTimer->timeout.time8[0] >= timeUnion.time8[0])
        {
          // 8-bit math
          srchTimer->timeout.time8[0] -= timeUnion.time8[0];
        }
        else
        {
          // 32-bit math
          if (srchTimer->timeout.time32 > timeUnion.time32)
          {
            srchTimer->timeout.time32 -= timeUnion.time32;
          }
          else
          {
            srchTimer->timeout.time32 = 0;
          }
        }
      }
      else
      {
          // 32-bit math
        if (srchTimer->timeout.time32 > timeUnion.time32)
        {
          srchTimer->timeout.time32 -= timeUnion.time32;
        }
        else
        {
          srchTimer->timeout.time32 = 0;
        }
      }

      // Check for reloading
      if ( (srchTimer->timeout.time16[0] == 0) && (srchTimer->timeout.time16[1] == 0) &&
           (srchTimer->reloadTimeout) && (srchTimer->event_flag) )
      {
        // Notify the task of a timeout
        osal_set_event( srchTimer->task_id, srchTimer->event_flag );

        // Reload the timer timeout value
        srchTimer->timeout.time32 = srchTimer->reloadTimeout;
      }

      // When timeout or delete (event_flag == 0)
      if ( ((srchTimer->timeout.time16[0] == 0) && (srchTimer->timeout.time16[1] == 0)) ||
            (srchTimer->event_flag == 0) )
      {
        // Take out of list
        if ( prevTimer == NULL )
        {
          timerHead = srchTimer->next;
        }
        else
        {
          prevTimer->next = srchTimer->next;
        }

        // Setup to free memory
        freeTimer = srchTimer;

        // Next
        srchTimer = srchTimer->next;
      }
      else
      {
        // Get next
        prevTimer = srchTimer;
        srchTimer = srchTimer->next;
      }

      HAL_EXIT_CRITICAL_SECTION( intState );   // Re-enable interrupts.

      if ( freeTimer )
      {
        if ( (freeTimer->timeout.time16[0] == 0) && (freeTimer->timeout.time16[1] == 0) )
        {
          osal_set_event( freeTimer->task_id, freeTimer->event_flag );
        }
        osal_mem_free( freeTimer );
      }
    }
  }
}

#ifdef POWER_SAVING
/*********************************************************************
 * @fn      osal_adjust_timers
 *
 * @brief   Update the timer structures for elapsed ticks.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
void osal_adjust_timers( void )
{
  uint32 eTime;

  if ( timerHead != NULL )
  {
    // Compute elapsed time (msec)
    eTime = TimerElapsed() / TICK_COUNT;

    if ( eTime )
    {
      osalTimerUpdate( eTime );
    }
  }
}
#endif /* POWER_SAVING */

#if defined POWER_SAVING || defined USE_ICALL
/*********************************************************************
 * @fn      osal_next_timeout
 *
 * @brief
 *
 *   Search timer table to return the lowest timeout value. If the
 *   timer list is empty, then the returned timeout will be zero.
 *
 * @param   none
 *
 * @return  none
 *********************************************************************/
uint32 osal_next_timeout( void )
{
  uint32 nextTimeout;
  osalTimerRec_t *srchTimer;

  if ( timerHead != NULL )
  {
    // Head of the timer list
    srchTimer = timerHead;
    nextTimeout = OSAL_TIMERS_MAX_TIMEOUT;

    // Look for the next timeout timer
    while ( srchTimer != NULL )
    {
      if (srchTimer->timeout.time32 < nextTimeout)
      {
        nextTimeout = srchTimer->timeout.time32;
      }
      // Check next timer
      srchTimer = srchTimer->next;
    }
  }
  else
  {
    // No timers
    nextTimeout = 0;
  }

  return ( nextTimeout );
}
#endif // POWER_SAVING || USE_ICALL

/*********************************************************************
 * @fn      osal_GetSystemClock()
 *
 * @brief   Read the local system clock.
 *
 * @param   none
 *
 * @return  local clock in milliseconds
 */
uint32 osal_GetSystemClock( void )
{
  return ( osal_systemClock );
}

/*********************************************************************
*********************************************************************/
