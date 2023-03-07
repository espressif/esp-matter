/******************************************************************************

 @file  osal_cbtimer.c

 @brief This file contains the Callback Timer task(s). This module provides
        'callback' timers using the existing 'event' timers. In other words,
        the registered callback function is called instead of an
        OSAL event being sent to the owner of the timer when it expires.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2008-2022, Texas Instruments Incorporated
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

#include "osal.h"
#include "osal_tasks.h"

#include "hal_mcu.h"
#include "osal_cbtimer.h"

/*********************************************************************
 * MACROS
 */
// Macros to convert one-dimensional index 'timer id' to two-dimensional indices
// 'task id' and 'event id'.

// Find out event id using timer id
#define EVENT_ID( timerId )            ( 0x0001 << ( ( timerId ) % NUM_CBTIMERS_PER_TASK ) )

// Find out task id using timer id
#define TASK_ID( timerId )             ( ( ( timerId ) / NUM_CBTIMERS_PER_TASK ) + baseTaskID )

// Find out bank task id using task id
#define BANK_TASK_ID( taskId )         ( ( baseTaskID - ( taskId ) ) * NUM_CBTIMERS )

/*********************************************************************
 * CONSTANTS
 */
// Number of callback timers supported per task (limited by the number of OSAL event timers)
#define NUM_CBTIMERS_PER_TASK          15

// Total number of callback timers
#define NUM_CBTIMERS                   ( OSAL_CBTIMER_NUM_TASKS * NUM_CBTIMERS_PER_TASK )

/*********************************************************************
 * TYPEDEFS
 */
// Callback Timer structure
typedef struct
{
  pfnCbTimer_t  pfnCbTimer; // callback function to be called when timer expires
  uint8        *pData;      // data to be passed in to callback function
} cbTimer_t;

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Callback Timer base task id
uint16 baseTaskID = TASK_NO_TASK;

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
// Callback Timers table.
#if ( NUM_CBTIMERS > 0 )
  cbTimer_t cbTimers[NUM_CBTIMERS];
#endif

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static Status_t cbTimerSetup( pfnCbTimer_t  pfnCbTimer,
                              uint8        *pData,
                              uint32        timeout,
                              uint8        *pTimerId,
                              uint8         reload );

/*********************************************************************
 * API FUNCTIONS
 */


/*********************************************************************
 * @fn          osal_CbTimerInit
 *
 * @brief       Callback Timer task initialization function. This function
 *              can be called more than once (OSAL_CBTIMER_NUM_TASKS times).
 *
 * @param       taskId - Message Timer task ID.
 *
 * @return      void
 */
void osal_CbTimerInit( uint8 taskId )
{
  if ( baseTaskID == TASK_NO_TASK )
  {
    // Only initialize the base task id
    baseTaskID = taskId;

    // Initialize all timer structures
    osal_memset( cbTimers, 0, sizeof( cbTimers ) );
  }
}


/*********************************************************************
 * @fn          osal_CbTimerProcessEvent
 *
 * @brief       Callback Timer task event processing function.
 *
 * @param       taskId - task ID.
 * @param       events - events.
 *
 * @return      events not processed
 */
uint16 osal_CbTimerProcessEvent( uint8 taskId, uint16 events )
{
  if ( events & SYS_EVENT_MSG )
  {
    // Process OSAL messages

    // return unprocessed events
    return ( events ^ SYS_EVENT_MSG );
  }

  if ( events )
  {
    uint8 i;
    uint16 event = 0;
    halIntState_t cs;

    HAL_ENTER_CRITICAL_SECTION(cs);

    // Process event timers
    for ( i = 0; i < NUM_CBTIMERS_PER_TASK; i++ )
    {
      if ( ( events >> i ) & 0x0001 )
      {
        cbTimer_t *pTimer = &cbTimers[BANK_TASK_ID( taskId )+i];

        // Found the first event
        event =  0x0001 << i;

        // check there is a callback function to call
        if ( pTimer->pfnCbTimer != NULL )
        {
          // Timer expired, call the registered callback function
          pTimer->pfnCbTimer( pTimer->pData );
        }

        // check if the timer is still active
        // Note: An active timer means the timer was started with reload!
        if ( osal_get_timeoutEx( taskId, event ) == 0 )
        {
          // Mark entry as free
          pTimer->pfnCbTimer = NULL;

          // Null out data pointer
          pTimer->pData = NULL;
        }

        // We only process one event at a time
        break;
      }
    }

    HAL_EXIT_CRITICAL_SECTION(cs);

    // return unprocessed events
    return ( events ^ event );
  }

  // If reach here, the events are unknown
  // Discard or make more handlers
  return 0;
}


/*********************************************************************
 * @fn      osal_CbTimerStart
 *
 * @brief   This function is called to start a callback timer to expire
 *          in n mSecs. When the timer expires, the registered callback
 *          function will be called.
 *
 * input parameters
 *
 * @param   pfnCbTimer - Callback function to be called when timer expires.
 * @param   pData      - Data to be passed in to callback function.
 * @param   timeout    - In milliseconds.
 * @param   pTimerId   - Pointer to new timer Id or NULL.
 *
 * output parameters
 *
 * @param   pTimerId   - Pointer to new timer Id or NULL.
 *
 * @return  Success, or Failure.
 */
Status_t osal_CbTimerStart( pfnCbTimer_t  pfnCbTimer,
                            uint8        *pData,
                            uint32        timeout,
                            uint8        *pTimerId )
{
  return ( cbTimerSetup( pfnCbTimer,
                         pData,
                         timeout,
                         pTimerId,
                         FALSE ) );
}


/*********************************************************************
 * @fn      osal_CbTimerReload
 *
 * @brief   This function is called to start a callback timer to expire
 *          in n mSecs. When the timer expires, it will automatically
 *          reload the timer, and the registered callback function will
 *          be called.
 *
 * input parameters
 *
 * @param   pfnCbTimer - Callback function to be called when timer expires.
 * @param   pData      - Data to be passed in to callback function.
 * @param   timeout    - In milliseconds.
 * @param   pTimerId   - Pointer to new timer Id or NULL.
 *
 * output parameters
 *
 * @param   pTimerId   - Pointer to new timer Id or NULL.
 *
 * @return  Success, or Failure.
 */
Status_t osal_CbTimerStartReload( pfnCbTimer_t  pfnCbTimer,
                                  uint8        *pData,
                                  uint32        timeout,
                                  uint8        *pTimerId )
{
  return ( cbTimerSetup( pfnCbTimer,
                         pData,
                         timeout,
                         pTimerId,
                         TRUE ) );
}


/*********************************************************************
 * @fn      osal_CbTimerUpdate
 *
 * @brief   This function is called to update a message timer that has
 *          already been started. If SUCCESS, the function will update
 *          the timer's timeout value. If INVALIDPARAMETER, the timer
 *          either doesn't exit.
 *
 * @param   timerId - identifier of the timer that is to be updated
 * @param   timeout - new timeout in milliseconds.
 *
 * @return  SUCCESS or INVALIDPARAMETER if timer not found
 */
Status_t osal_CbTimerUpdate( uint8 timerId, uint32 timeout )
{
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION(cs);

  // Look for the existing timer
  if ( timerId < NUM_CBTIMERS )
  {
    if ( cbTimers[timerId].pfnCbTimer != NULL )
    {
      // Make sure the corresponding OSAL event timer is still running
      if ( osal_get_timeoutEx( TASK_ID( timerId ), EVENT_ID( timerId ) ) != 0 )
      {
        // Timer exists; update it
        osal_start_timerEx( TASK_ID( timerId ), EVENT_ID( timerId ), timeout );

        HAL_EXIT_CRITICAL_SECTION(cs);

        return (  SUCCESS );
      }
    }
  }

  HAL_EXIT_CRITICAL_SECTION(cs);

  // Timer not found
  return ( INVALIDPARAMETER );
}


/*********************************************************************
 * @fn      osal_CbTimerStop
 *
 * @brief   This function is called to stop a timer that has already been
 *          started. If SUCCESS, the function will cancel the timer. If
 *          INVALIDPARAMETER, the timer doesn't exit.
 *
 * @param   timerId - identifier of the timer that is to be stopped
 *
 * @return  SUCCESS or INVALIDPARAMETER if timer not found
 */
Status_t osal_CbTimerStop( uint8 timerId )
{
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION(cs);

  // Look for the existing timer
  if ( timerId < NUM_CBTIMERS )
  {
    if ( cbTimers[timerId].pfnCbTimer != NULL )
    {
      // Timer exists; stop the OSAL event timer first
      osal_stop_timerEx( TASK_ID( timerId ), EVENT_ID( timerId ) );

      // Mark entry as free
      cbTimers[timerId].pfnCbTimer = NULL;

      // Null out data pointer
      cbTimers[timerId].pData = NULL;

      HAL_EXIT_CRITICAL_SECTION(cs);

      return ( SUCCESS );
    }
  }

  HAL_EXIT_CRITICAL_SECTION(cs);

  // Timer not found
  return ( INVALIDPARAMETER );
}

/*
** Local Functions
*/

/*********************************************************************
 * @fn      cbTimerSetup
 *
 * @brief   This function is a common routine that can be used to start
 *          or reload a callback timer to expire in n mSecs.
 *
 * input parameters
 *
 * @param   pfnCbTimer - Callback function to be called when timer expires.
 * @param   pData      - Data to be passed in to callback function.
 * @param   timeout    - In milliseconds.
 * @param   pTimerId   - Pointer to new timer Id or NULL.
 * @param   reload     - Indicate start (FALSE) or start reload (TRUE) timer.
 *
 * output parameters
 *
 * @param   pTimerId   - Pointer to new timer Id or NULL.
 *
 * @return  Success, or Failure.
 */
static Status_t cbTimerSetup( pfnCbTimer_t  pfnCbTimer,
                              uint8        *pData,
                              uint32        timeout,
                              uint8        *pTimerId,
                              uint8         reload )
{
  uint8 i;
  halIntState_t cs;

  HAL_ENTER_CRITICAL_SECTION(cs);

  // Validate input parameters
  if ( pfnCbTimer == NULL )
  {
    HAL_EXIT_CRITICAL_SECTION(cs);

    return ( INVALIDPARAMETER );
  }

  // Look for an unused timer first
  for ( i = 0; i < NUM_CBTIMERS; i++ )
  {
    if ( cbTimers[i].pfnCbTimer == NULL )
    {
      // Start the OSAL event timer first
      if ( ( (reload==TRUE)          ?
             osal_start_reload_timer :
             osal_start_timerEx )( TASK_ID( i ), EVENT_ID( i ), timeout ) == SUCCESS )
      {
        // Set up the callback timer
        // Note: An odd pointer will be used to indicate to the process event
        //       handler that the timer was started with reload.
        cbTimers[i].pfnCbTimer = pfnCbTimer; //(pfnCbTimer_t)((uint32)pfnCbTimer | reload);
        cbTimers[i].pData      = pData;

        // Check if the caller wants the timer Id
        if ( pTimerId != NULL )
        {
          // Caller is interested in the timer id
          *pTimerId = i;
        }

        HAL_EXIT_CRITICAL_SECTION(cs);

        return ( SUCCESS );
      }
    }
  }

  HAL_EXIT_CRITICAL_SECTION(cs);

  // No timer available
  return ( NO_TIMER_AVAIL );
}

/****************************************************************************
****************************************************************************/
