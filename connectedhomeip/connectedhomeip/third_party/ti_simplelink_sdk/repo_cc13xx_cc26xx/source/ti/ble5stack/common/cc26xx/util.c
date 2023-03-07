/******************************************************************************

 @file  util.c

 @brief This file contains utility functions commonly used by
        BLE applications for CC26xx with TIRTOS.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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
#ifdef FREERTOS
#include <pthread.h> // Only for timer init - NOT FOR thread creation
#include <mqueue.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <mqueue.h>
#else
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/hal/Hwi.h>
#endif


#ifdef USE_ICALL
#include <icall.h>
#else
#include <stdlib.h>
#endif

#include "bcomdef.h"
#include "util.h"

#ifdef FREERTOS
#ifndef ICALL_TIMER_TASK_STACK_SIZE
/**
 * @internal
 * Timer thread stack size
 */
#define ICALL_TIMER_TASK_STACK_SIZE (512)
#endif //ICALL_TIMER_TASK_STACK_SIZE
#endif//FREERTOS

#ifdef FREERTOS
extern mqd_t g_EventsQueueID;
#endif
/*********************************************************************
 * TYPEDEFS
 */

#ifndef FREERTOS
// RTOS queue for profile/app messages.
typedef struct _queueRec_
{
  Queue_Elem _elem;          // queue element
  uint8_t *pData;            // pointer to app data
} queueRec_t;
#endif
/*********************************************************************
 * LOCAL FUNCTIONS
 */

#ifdef FREERTOS
typedef void (*UtilTimerCback)(void *arg);
void Util_stopClock(Clock_Struct *pClock);

/**
 * @internal
 * Clock event handler function.
 * This function is used to implement the wakeup scheduler.
 *
 * @param arg  an @ref ICall_ScheduleEntry
 */
static void UtilclockFunc(union sigval sv)
{
    Clock_Struct *entry = (Clock_Struct *) (sv.sival_ptr);

    /* this means that this timer is not periodic so we set the isActive to 0 */
    if((entry->timeVal.it_interval.tv_nsec == 0) && (entry->timeVal.it_interval.tv_sec == 0))
    {
        entry->isActive = 0;
    }

    ((UtilTimerCback)(entry->cback))(entry->arg);
}
#endif


/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Util_constructClock
 *
 * @brief   Initialize a TIRTOS Clock instance.
 *
 * @param   pClock        - pointer to clock instance structure.
 * @param   clockCB       - callback function upon clock expiration.
 * @param   clockDuration - longevity of clock timer in milliseconds
 * @param   clockPeriod   - if set to a value other than 0, the first
 *                          expiry is determined by clockDuration.  All
 *                          subsequent expiries use the clockPeriod value.
 * @param   startFlag     - TRUE to start immediately, FALSE to wait.
 * @param   arg           - argument passed to callback function.
 *
 * @return  Clock_Handle  - a handle to the clock instance.
 */

#ifdef FREERTOS
void* Util_constructClock(Clock_Struct *entry, void *clockCB,
                          uint32_t clockDuration, uint32_t clockPeriod,
                          uint8_t startFlag, void *arg)
{

    int ret;
   // entry = ICall_malloc(sizeof(Clock_Struct));
    if (entry == NULL)
    {
        return (void *)(ICALL_ERRNO_NO_RESOURCE);
    }

    memset(entry, 0, sizeof(Clock_Struct));

    //Clock_Params params
    pthread_attr_init(&(entry->timerThrdAttr));

    entry->timerThrdAttr.stacksize = ICALL_TIMER_TASK_STACK_SIZE;

    entry->evnt.sigev_notify = SIGEV_THREAD;
    entry->evnt.sigev_notify_function = &UtilclockFunc;
    entry->evnt.sigev_notify_attributes = &(entry->timerThrdAttr);

    entry->arg = arg;
    entry->cback = clockCB;

    entry->evnt.sigev_value.sival_ptr = entry;

    ret = timer_create(CLOCK_MONOTONIC, &(entry->evnt), &(entry->clock));

    if (ret < 0)
    {
        return (void*)-1;
    }

    if (clockDuration == 0)
    {
        return (void *)(ICALL_ERRNO_INVALID_PARAMETER);
    }

    entry->timeVal.it_value.tv_sec = clockDuration / 1000;
    entry->timeVal.it_value.tv_nsec = 1000000 * (clockDuration % 1000);
    entry->timeVal.it_interval.tv_sec = clockPeriod / 1000;
    entry->timeVal.it_interval.tv_nsec = 1000000 * (clockPeriod % 1000);

    if (startFlag)
    {
        entry->isActive = 1;
        ret = timer_settime(entry->clock, 0, &(entry->timeVal), NULL);
        if (ret < 0)
        {
            return (void *)(ICALL_ERRNO_NO_RESOURCE);
        }
    }

    return (&(entry->clock));
}

void Clock_destruct(Clock_Struct *structP)
{
    Util_stopClock(structP);
    timer_delete(structP->clock);

    structP->clock = 0;
}

#else
Clock_Handle Util_constructClock(Clock_Struct *pClock,
                                 Clock_FuncPtr clockCB,
                                 uint32_t clockDuration,
                                 uint32_t clockPeriod,
                                 uint8_t startFlag,
                                 UArg arg)
{
  Clock_Params clockParams;

  // Convert clockDuration in milliseconds to ticks.
  uint32_t clockTicks = clockDuration * (1000 / Clock_tickPeriod);

  // Setup parameters.
  Clock_Params_init(&clockParams);

  // Setup argument.
  clockParams.arg = arg;

  // If period is 0, this is a one-shot timer.
  clockParams.period = clockPeriod * (1000 / Clock_tickPeriod);

  // Starts immediately after construction if true, otherwise wait for a call
  // to start.
  clockParams.startFlag = startFlag;

  // Initialize clock instance.
  Clock_construct(pClock, clockCB, clockTicks, &clockParams);

  return Clock_handle(pClock);
}


#endif // FREERTOS

/*********************************************************************
 * @fn      Util_startClock
 *
 * @brief   Start a clock.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  none
 */
void Util_startClock(Clock_Struct *pClock)
{
#ifdef FREERTOS

    pClock->isActive = 1;
    timer_settime(pClock->clock, 0, &(pClock->timeVal), NULL);

#else
    Clock_Handle handle = Clock_handle(pClock);

    // Start clock instance
    Clock_start(handle);
#endif
}

/*********************************************************************
 * @fn      Util_restartClock
 *
 * @brief   Restart a clock by changing the timeout.
 *
 * @param   pClock - pointer to clock struct
 * @param   clockTimeout - longevity of clock timer in milliseconds
 *
 * @return  none
 */
#ifndef FREERTOS
void Util_restartClock(Clock_Struct *pClock, uint32_t clockTimeout)
{
  uint32_t clockTicks;
  Clock_Handle handle;

  handle = Clock_handle(pClock);

  if (Clock_isActive(handle))
  {
    // Stop clock first
    Clock_stop(handle);
  }

  // Convert timeout in milliseconds to ticks.
  clockTicks = clockTimeout * (1000 / Clock_tickPeriod);

  // Set the initial timeout
  Clock_setTimeout(handle, clockTicks);

  // Start clock instance
  Clock_start(handle);
}
#endif
/*********************************************************************
 * @fn      Util_isActive
 *
 * @brief   Determine if a clock is currently active.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  TRUE if Clock is currently active
            FALSE otherwise
 */
bool Util_isActive(Clock_Struct *pClock)
{
#ifdef FREERTOS
    if(pClock->isActive == 1)
    {
        return TRUE;
    }

    // Start clock instance
    return FALSE;
#else
    Clock_Handle handle = Clock_handle(pClock);

  // Start clock instance
  return Clock_isActive(handle);
#endif
}

/*********************************************************************
 * @fn      Util_stopClock
 *
 * @brief   Stop a clock.
 *
 * @param   pClock - pointer to clock struct
 *
 * @return  none
 */
void Util_stopClock(Clock_Struct *pClock)
{
#ifdef FREERTOS
    struct itimerspec timeVal;

    memset(&timeVal,0,sizeof(struct itimerspec));

    /* stop the timer */
    timer_settime(pClock->clock, 0, &(timeVal), NULL);

    pClock->isActive = 0;

#else
    Clock_Handle handle = Clock_handle(pClock);

    // Stop clock instance
    Clock_stop(handle);
#endif
}

/*********************************************************************
 * @fn      Util_rescheduleClock
 *
 * @brief   Reschedule a clock by changing the timeout and period values.
 *
 * @param   pClock - pointer to clock struct
 * @param   clockPeriod - longevity of clock timer in milliseconds
 * @return  none
 */
void Util_rescheduleClock(Clock_Struct *pClock, uint32_t clockPeriod)
{
  bool running;

#ifdef FREERTOS
  running = Util_isActive(pClock);

  if (running)
  {
      Util_stopClock(pClock);
  }

  pClock->timeVal.it_interval.tv_sec = clockPeriod / 1000;
  pClock->timeVal.it_interval.tv_nsec = 1000000 * (clockPeriod % 1000);
  pClock->timeVal.it_value.tv_sec = clockPeriod / 1000;
  pClock->timeVal.it_value.tv_nsec = 1000000 * (clockPeriod % 1000);

  timer_settime(pClock->clock, 0, &(pClock->timeVal), NULL);

#else
  uint32_t clockTicks;
  Clock_Handle handle;

  handle = Clock_handle(pClock);
  running = Clock_isActive(handle);

  if (running)
  {
    Clock_stop(handle);
  }

  // Convert period in milliseconds to ticks.
  clockTicks = clockPeriod * (1000 / Clock_tickPeriod);

  Clock_setTimeout(handle, clockTicks);
  Clock_setPeriod(handle, clockTicks);

  if (running)
  {
    Clock_start(handle);
  }
#endif
}

/*********************************************************************
 * @fn      Util_constructQueue
 *
 * @brief   Initialize an RTOS queue to hold messages to be processed.
 *
 * @param   pQueue - pointer to queue instance structure.
 *
 * @return  A queue handle.
 */
#ifdef FREERTOS

void Util_constructQueue(mqd_t *pQueue)
{
    struct mq_attr MRattr;

    MRattr.mq_flags = O_NONBLOCK; //Blocking
    MRattr.mq_curmsgs = 0;
    MRattr.mq_maxmsg = 32;
    MRattr.mq_msgsize = sizeof(uint8_t*);
    /* Open the reply message queue */
    *pQueue = mq_open("/AppQueue", O_CREAT | O_NONBLOCK , 0, &MRattr);
    if (pQueue == (mqd_t) -1)
    {
        //handle_error("mq_open");

        while(1);
    }
}
#else // FREERTOS
Queue_Handle Util_constructQueue(Queue_Struct *pQueue)
{
  // Construct a Queue instance.
  Queue_construct(pQueue, NULL);

  return Queue_handle(pQueue);
}
#endif // FREERTOS

/*********************************************************************
 * @fn      Util_enqueueMsg
 *
 * @brief   Creates a queue node and puts the node in RTOS queue.
 *
 * @param   msgQueue - queue handle.
 * @param   event - thread's event processing handle that queue is
 *                associated with.
 * @param   pMsg - pointer to message to be queued
 *
 * @return  TRUE if message was queued, FALSE otherwise.
 */
#ifdef FREERTOS

typedef struct {
    uint8_t * pData;
}queueMSG;

uint8_t Util_enqueueMsg(mqd_t msgQueue,
                        Event_Handle event,
                        uint8_t *pMsg)
{
    queueMSG myMsg;

    myMsg.pData = pMsg;
    mq_send(msgQueue, (char*)&myMsg, sizeof(queueMSG), 1);

    // Wake up the application thread event handler.
    if (event)
    {

        uint32_t msg_ptr = UTIL_QUEUE_EVENT_ID;
        mq_send(event, (char*)&msg_ptr, sizeof(msg_ptr), 1);
    }

    return TRUE;

}
#else


uint8_t Util_enqueueMsg(Queue_Handle msgQueue,
                        Event_Handle event,
                        uint8_t *pMsg)
{
  queueRec_t *pRec;

  // Allocated space for queue node.
#ifdef USE_ICALL
  if ((pRec = ICall_malloc(sizeof(queueRec_t))))
#else
  if ((pRec = (queueRec_t *)malloc(sizeof(queueRec_t))))
#endif
  {
    pRec->pData = pMsg;

    // This is an atomic operation
    Queue_put(msgQueue, &pRec->_elem);

    // Wake up the application thread event handler.
    if (event)
    {

        Event_post(event, UTIL_QUEUE_EVENT_ID);
    }
    return TRUE;
  }

  // Free the message.
#ifdef USE_ICALL
  ICall_free(pMsg);
#else
  free(pMsg);
#endif

  return FALSE;
}
#endif

/*********************************************************************
 * @fn      Util_dequeueMsg
 *
 * @brief   Dequeues the message from the RTOS queue.
 *
 * @param   msgQueue - queue handle.
 *
 * @return  pointer to dequeued message, NULL otherwise.
 */
#ifdef FREERTOS
extern ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len,
                          unsigned int *msg_prio);
uint8_t *Util_dequeueMsg(mqd_t msgQueue)
{
    uint32_t msg_prio;
    queueMSG myMsg;
    uint8_t *pData = NULL;
    //* Non blocking queue */
    int msgSize = mq_receive(msgQueue,(char *)&myMsg, sizeof(queueMSG),(void*) &msg_prio);

    if(msgSize == (-1))
    {
        /*  The queue is empty which it's Ok*/
        return NULL;
    }
    pData = myMsg.pData;
    return pData;
}

#else


uint8_t *Util_dequeueMsg(Queue_Handle msgQueue)
{
  queueRec_t *pRec = Queue_get(msgQueue);

  if (pRec != (queueRec_t *)msgQueue)
  {
    uint8_t *pData = pRec->pData;

    // Free the queue node
    // Note:  this does not free space allocated by data within the node.
#ifdef USE_ICALL
    ICall_free(pRec);
#else
    free(pRec);
#endif

    return pData;
  }

  return NULL;
}
#endif

/*********************************************************************
 * @fn      Util_convertBdAddr2Str
 *
 * @brief   Convert Bluetooth address to string. Only needed when
 *          LCD display is used.
 *
 * @param   pAddr - BD address
 *
 * @return  BD address as a string
 */
char *Util_convertBdAddr2Str(uint8_t *pAddr)
{
  uint8_t     charCnt;
  char        hex[] = "0123456789ABCDEF";
  static char str[(2*B_ADDR_LEN)+3];
  char        *pStr = str;

  *pStr++ = '0';
  *pStr++ = 'x';

  // Start from end of addr
  pAddr += B_ADDR_LEN;

  for (charCnt = B_ADDR_LEN; charCnt > 0; charCnt--)
  {
    *pStr++ = hex[*--pAddr >> 4];
    *pStr++ = hex[*pAddr & 0x0F];
  }
#ifdef FREERTOS
  pStr = NULL;
#else
  *pStr = 0;
#endif
  return str;
}

/*********************************************************************
 * @fn      Util_isBufSet
 *
 * @brief   Check if contents of buffer matches byte pattern.
 *
 * @param   pBuf    - buffer to check
 * @param   pattern - pattern to match
 * @param   len     - len of buffer (in bytes) to iterate over
 *
 * @return  TRUE if buffer matches the pattern, FALSE otherwise.
 */
uint8_t Util_isBufSet(uint8_t *pBuf, uint8_t pattern, uint16_t len)
{
  uint8_t result = FALSE;
  uint16_t i = 0;

  if (pBuf)
  {
    result = TRUE;

    for(i = 0; i < len; i++)
    {
      if (pBuf[i] != pattern)
      {
        // Buffer does not match pattern.
        result = FALSE;
        break;
      }
    }
  }

  return (result);
}


/*********************************************************************
*********************************************************************/
