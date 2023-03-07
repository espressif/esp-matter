/******************************************************************************

 @file  npi_util.h

 @brief NPI Utilities

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2021-2022, Texas Instruments Incorporated
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

#ifndef NPI_UTIL_H
#define NPI_UTIL_H

#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
// Includes
//*****************************************************************************

#ifdef USE_ICALL
#include "icall.h"
#else
#include <stdlib.h>
#include "hal_assert.h"
#endif //USE_ICALL

#include <ti/sysbios/knl/Queue.h>
#ifdef ICALL_EVENTS
#include <ti/sysbios/knl/Event.h>
#else //!ICALL_EVENTS
#include <ti/sysbios/knl/Semaphore.h>
#endif //ICALL_EVENTS

//*****************************************************************************
// Defines
//*****************************************************************************

//*****************************************************************************
// Typedefs
//*****************************************************************************

//! \brief Keys used to enter and exit critical sections
typedef struct _npiCSKey_t
{
    uint_least16_t hwikey;
    uint_least16_t taskkey;
} _npiCSKey_t;

//*****************************************************************************
// Globals
//*****************************************************************************

//*****************************************************************************
// Function Prototypes
//*****************************************************************************

// -----------------------------------------------------------------------------
//! \brief    Memory allocation for NPI
//!
//! \param    size - number of bytes to be allocated.
//!
//! \return   pointer to the allocated memeory.
// -----------------------------------------------------------------------------
extern uint8_t *NPIUtil_malloc(uint16_t size);

// -----------------------------------------------------------------------------
//! \brief    Memory free for NPI
//!
//! \param    pMsg - pointer to the message to be freed.
//!
//! \return   None
// -----------------------------------------------------------------------------
extern void NPIUtil_free(uint8_t *pMsg);

// -----------------------------------------------------------------------------
//! \brief      Critical section entrance. Disables Tasks and HWI
//!
//! \return     _npiCSKey_t 	CS Key used to later exit CS
// -----------------------------------------------------------------------------
extern _npiCSKey_t NPIUtil_EnterCS(void);

// -----------------------------------------------------------------------------
//! \brief      Critical section exit. Enables Tasks and HWI
//!
//! \param		key 	key obtained with corresponding call to EnterCS()
//!
//! \return     void
// -----------------------------------------------------------------------------
extern void NPIUtil_ExitCS(_npiCSKey_t key);

// -----------------------------------------------------------------------------
//! \brief   Initialize an RTOS queue to hold messages to be processed.
//!
//! \param   pQueue - pointer to queue instance structure.
//!
//! \return  A queue handle.
// -----------------------------------------------------------------------------
extern Queue_Handle NPIUtil_constructQueue(Queue_Struct *pQueue);

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
extern uint8_t NPIUtil_enqueueMsg(Queue_Handle msgQueue,
#ifdef ICALL_EVENTS
                                  Event_Handle event,
                                  uint32_t eventFlags,
#else //!ICALL_EVENTS
                                  Semaphore_Handle sem,
#endif //ICALL_EVENTS
                                  uint8_t *pMsg);

// -----------------------------------------------------------------------------
//! \brief   Dequeues the message from the RTOS queue.
//!
//! \param   msgQueue - queue handle.
//!
//! \return  pointer to dequeued message, NULL otherwise.
// -----------------------------------------------------------------------------
extern uint8_t * NPIUtil_dequeueMsg(Queue_Handle msgQueue);

#ifdef __cplusplus
}
#endif

#endif /* NPI_UTIL_H */
