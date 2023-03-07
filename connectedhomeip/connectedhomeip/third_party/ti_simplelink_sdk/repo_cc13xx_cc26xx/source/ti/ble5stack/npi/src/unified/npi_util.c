/******************************************************************************

 @file  npi_util.c

 @brief NPI Utilities

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include "inc/npi_util.h"

// -----------------------------------------------------------------------------
// defines
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// typedefs
// -----------------------------------------------------------------------------

// RTOS queue for profile/app messages.
typedef struct _queueRec_
{
  Queue_Elem _elem;          // queue element
  uint8_t *pData;            // pointer to app data
} queueRec_t;

/// -----------------------------------------------------------------------------
// globals
/// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// PUBLIC FUNCTIONS
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
//! \brief    Memory allocation for NPI
//!
//! \param    size - number of bytes to be allocated.
//!
//! \return   pointer to the allocated memory.
// -----------------------------------------------------------------------------
uint8_t *NPIUtil_malloc(uint16_t size)
{
    uint8_t *pMsg;
#ifdef USE_ICALL
    pMsg = ICall_malloc(size);
#else
    volatile uint32_t keyHwi;
    keyHwi = Hwi_disable();
    pMsg = malloc(size);
    Hwi_restore(keyHwi);
#endif
    return pMsg;
}

// -----------------------------------------------------------------------------
//! \brief    Memory free for NPI
//!
//! \param    pMsg - pointer to the message to be freed.
//!
//! \return   None
// -----------------------------------------------------------------------------
void NPIUtil_free(uint8_t *pMsg)
{
#ifdef USE_ICALL
    ICall_free(pMsg);
#else
    volatile uint32_t keyHwi;
    keyHwi = Hwi_disable();
    free(pMsg);
    Hwi_restore(keyHwi);
#endif
}

// -----------------------------------------------------------------------------
//! \brief      Critical section entrance. Disables Tasks and HWI
//!
//! \return     _npiCSKey_t   CS Key used to later exit CS
// -----------------------------------------------------------------------------
_npiCSKey_t NPIUtil_EnterCS(void)
{
    _npiCSKey_t key;
    key.taskkey = (uint_least16_t) Task_disable();
    key.hwikey = (uint_least16_t) Hwi_disable();
    return key;
}

// -----------------------------------------------------------------------------
//! \brief      Critical section exit. Enables Tasks and HWI
//!
//! \param    key   key obtained with corresponding call to EnterCS()
//!
//! \return   void
// -----------------------------------------------------------------------------
void NPIUtil_ExitCS(_npiCSKey_t key)
{
    Hwi_restore((UInt) key.hwikey);
    Task_restore((UInt) key.taskkey);
}


// -----------------------------------------------------------------------------
//! \brief   Initialize an RTOS queue to hold messages to be processed.
//!
//! \param   pQueue - pointer to queue instance structure.
//!
//! \return  A queue handle.
// -----------------------------------------------------------------------------
Queue_Handle NPIUtil_constructQueue(Queue_Struct *pQueue)
{
  // Construct a Queue instance.
  Queue_construct(pQueue, NULL);

  return Queue_handle(pQueue);
}

// -----------------------------------------------------------------------------
//! \brief   Creates a queue node and puts the node in RTOS queue.
//!
//! \param   msgQueue - queue handle.
//! \param   event - thread's event processing synchronization object that
//!                  queue is associated with.
//! \param   eventFlag - events to signal with synchronization object associated
//!                      with this pMsg.
//! \param   sem - thread's event processing semaphore that queue is
//!                associated with.
//! \param   pMsg - pointer to message to be queued
//!
//! \return  TRUE if message was queued, FALSE otherwise.
// -----------------------------------------------------------------------------
uint8_t NPIUtil_enqueueMsg(Queue_Handle msgQueue,
#ifdef ICALL_EVENTS
                           Event_Handle event,
                           uint32_t eventFlags,
#else //!ICALL_EVENTS
                           Semaphore_Handle sem,
#endif //ICALL_EVENTS
                           uint8_t *pMsg)
{
  queueRec_t *pRec;

  // Allocated space for queue node.
  pRec = (queueRec_t *)NPIUtil_malloc(sizeof(queueRec_t));
  if (pRec)
  {
    pRec->pData = pMsg;

    Queue_put(msgQueue, &pRec->_elem);

    // Wake up the application thread event handler.
#ifdef ICALL_EVENTS
    if (event)
    {
      Event_post(event, eventFlags);
    }
#else //!ICALL_EVENTS
    if (sem)
    {
      Semaphore_post(sem);
    }
#endif //ICALL_EVENTS

    return TRUE;
  }

  // Free the message.
  NPIUtil_free((uint8_t *)pMsg);

  return FALSE;
}

// -----------------------------------------------------------------------------
//! \brief   Dequeues the message from the RTOS queue.
//!
//! \param   msgQueue - queue handle.
//!
//! \return  pointer to dequeued message, NULL otherwise.
// -----------------------------------------------------------------------------
uint8_t *NPIUtil_dequeueMsg(Queue_Handle msgQueue)
{
  queueRec_t *pRec = Queue_get(msgQueue);
  if (pRec != (queueRec_t *)msgQueue)
  {
    // Queue not empty
    uint8_t *pData = pRec->pData;

    // Free the queue node
    // Note:  this does not free space allocated by data within the node.
    NPIUtil_free((uint8_t *)pRec);

    return pData;
  }

  return NULL;
}
