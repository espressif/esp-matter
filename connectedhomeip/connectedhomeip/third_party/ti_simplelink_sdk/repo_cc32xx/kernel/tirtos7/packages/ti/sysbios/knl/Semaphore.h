/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/*!
 * @file ti/sysbios/knl/Semaphore.h
 * @brief Semaphore Manager
 * 
 * The Semaphore manager makes available a set of functions that manipulate
 * semaphore objects. Semaphores can be used for task synchronization and mutual
 * exclusion.
 * 
 * Semaphores can be counting semaphores or binary semaphores. Counting
 * semaphores keep track of the number of times the semaphore has been posted
 * with Semaphore_post(). This is useful, for example, if you have a group of resources
 * that are shared between tasks. Such tasks might call Semaphore_pend() to see if a
 * resource is available before using one.
 * 
 * Binary semaphores can have only two states: available (count = 1) and
 * unavailable (count = 0). They can be used to share a single resource between
 * tasks. They can also be used for a basic signaling mechanism, where the
 * semaphore can be posted multiple times. Binary semaphores do not keep track
 * of the count; they simply track whether the semaphore has been posted or not.
 * 
 * See @link Semaphore_getCount() @endlink for more details
 * of the 'count' behavior.
 * 
 * The Mailbox module uses a counting semaphore internally to manage the count
 * of free (or full) mailbox elements. Another example of a counting semaphore
 * is an ISR that might fill multiple buffers of data for consumption by a task.
 * After filling each buffer, the ISR puts the buffer on a queue and calls
 * Semaphore_post(). The task waiting for the data calls Semaphore_pend(),
 * which simply decrements
 * the semaphore count and returns or blocks if the count is 0. The semaphore
 * count thus tracks the number of full buffers available for the task.
 * 
 * @link Semaphore_pend() @endlink is used to wait for a semaphore. The timeout parameter allows the task
 * to wait until a timeout, wait indefinitely, or not wait at all. The return
 * value is used to indicate if the semaphore was signaled successfully.
 * 
 * @link Semaphore_post() @endlink is used to signal a semaphore. If a task is waiting for the semaphore,
 * Semaphore_post() removes the task from the semaphore queue and puts it on the ready
 * queue. If no tasks are waiting, Semaphore_post() simply increments the semaphore count
 * and returns.  For a binary semaphore the count is always set to one.
 *
 * To use the Semaphore module or to set any of the Semaphore module configuration variables,
 * the following must be added to the app.syscfg file:
 *
 * @code 
 * const Semaphore = scripting.addModule("/ti/sysbios/knl/Semaphore");
 * @endcode 
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 * 
 *    <tr><th> Function               </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                  -->
 *    <tr><td> Semaphore_Params_init   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Semaphore_construct     </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Semaphore_create        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Semaphore_delete        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Semaphore_destruct      </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Semaphore_getCount      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Semaphore_pend          </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td> Semaphore_post          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Semaphore_registerEvent </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Semaphore_reset         </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: (N* means OK to call iff the timeout
 *           parameter is set to '0'.)<br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. after Semaphore_init() has been called). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. before Semaphore_init() has been called). </li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 * 
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== Semaphore.h ========
 */

#ifndef ti_sysbios_knl_Semaphore__include
#define ti_sysbios_knl_Semaphore__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Event.h>

#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
#define ti_sysbios_knl_Semaphore_long_names
#include "Semaphore_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error and Assert Ids
 */

/*!
 * @brief Asserted when Semaphore_pend() is called with non-zero timeout from
 *  other than a Task context.
 */
#define Semaphore_A_badContext       "bad calling context - must be called from a Task"

/*!
 * @brief Asserted by Semaphore_create() if Semaphore.supportsEvents is false
   and an Event object is passed to Semaphore_create().
 */
#define Semaphore_A_noEvents         "the Semaphore.supportsEvents flag is disabled"

/*!
 * @brief Asserted when Semaphore_post() has been called when the 16 bit
 * semaphore count is at its maximum value of 65535.
 */
#define Semaphore_A_overflow         "count has exceeded 65535 and rolled over"

/*!
 * @brief Assert raised if Semaphore_pend() is called with the Task or
 * Swi scheduler disabled.
 */
#define Semaphore_A_pendTaskDisabled "cannot call Semaphore_pend() while the Task or Swi scheduler is disabled"

/*! @cond NODOC */
typedef struct Semaphore_PendElem Semaphore_PendElem;
/*! @endcond */

typedef struct Semaphore_Params Semaphore_Params;
typedef struct Semaphore_Struct Semaphore_Object;
typedef struct Semaphore_Struct Semaphore_Struct;
typedef Semaphore_Struct* Semaphore_Handle;
typedef Semaphore_Struct* Semaphore_Instance;

/*!
 * @brief Semaphore types.
 * 
 * These enumerations specify the type of semaphore.
 * 
 * Tasks wait for the semaphore in FIFO order unless the PRIORITY option is
 * chosen.
 * 
 * For PRIORITY semaphores, the pending task will be inserted in the waiting
 * list before the first task that has lower priority.  This ensures that tasks
 * of equal priority will pend in FIFO order.
 * 
 * @warning
 * PRIORITY semaphores have a potential to increase the interrupt latency in the
 * system since interrupts are disabled while the list of tasks waiting on the
 * semaphore is scanned for the proper insertion point.  This is typically about
 * a dozen instructions per waiting task. For example, if you have 10 tasks of
 * higher priority waiting, then all 10 will be checked with interrupts disabled
 * before the new task is entered onto the list.
 */
enum Semaphore_Mode {
    /*!
     * @brief Counting (FIFO)
     */
    Semaphore_Mode_COUNTING = 0x0,
    /*!
     * @brief Binary (FIFO)
     */
    Semaphore_Mode_BINARY = 0x1,
    /*!
     * @brief Counting (priority-based)
     */
    Semaphore_Mode_COUNTING_PRIORITY = 0x2,
    /*!
     * @brief Binary (priority-based)
     */
    Semaphore_Mode_BINARY_PRIORITY = 0x3
};

/*!
 * @brief Semaphore types.
 * 
 * These enumerations specify the type of semaphore.
 * 
 * Tasks wait for the semaphore in FIFO order unless the PRIORITY option is
 * chosen.
 * 
 * For PRIORITY semaphores, the pending task will be inserted in the waiting
 * list before the first task that has lower priority.  This ensures that tasks
 * of equal priority will pend in FIFO order.
 * 
 * @warning
 * PRIORITY semaphores have a potential to increase the interrupt latency in the
 * system since interrupts are disabled while the list of tasks waiting on the
 * semaphore is scanned for the proper insertion point.  This is typically about
 * a dozen instructions per waiting task. For example, if you have 10 tasks of
 * higher priority waiting, then all 10 will be checked with interrupts disabled
 * before the new task is entered onto the list.
 */
typedef enum Semaphore_Mode Semaphore_Mode;

/*! @cond NODOC */
enum Semaphore_PendState {
    Semaphore_PendState_TIMEOUT = 0,
    Semaphore_PendState_POSTED = 1,
    Semaphore_PendState_CLOCK_WAIT = 2,
    Semaphore_PendState_WAIT_FOREVER = 3
};
typedef enum Semaphore_PendState Semaphore_PendState;

struct Semaphore_PendElem {
    Task_PendElem tpElem;
    Semaphore_PendState pendState;
};
/*! @endcond */

struct Semaphore_Params {
    /*!
     * @brief Semaphore mode
     * 
     * When mode is BINARY, the semaphore has only two states, available and
     * unavailable. When mode is COUNTING, the semaphore keeps track of number
     * of times a semaphore is posted.
     * 
     * The default for this parameter is COUNTING.
     */
    Semaphore_Mode mode;
    /*!
     * @brief Event instance to use if non-NULL
     * 
     * The default value of this parameter is null.  If event is non-null:
     * - Event_post(sem->event, sem->eventId) will be invoked when
     * Semaphore_post() is called.
     * - Event_pend(sem->event, 0, sem->eventId, timeout) will be invoked when
     * Semaphore_pend() is called.
     */
    Event_Handle event;
    /*!
     * @brief eventId if using Events
     * 
     * The default for this parameters is 1.
     */
    unsigned int eventId;
};

struct Semaphore_Struct {
    /*! @cond NODOC */
    Queue_Elem objElem;
    /*!
     * @brief Event instance to use if non-NULL
     * 
     * The default value of this parameter is null.  If event is non-null:
     * - Event_post(sem->event, sem->eventId) will be invoked when
     * Semaphore_post() is called.
     * - Event_pend(sem->event, 0, sem->eventId, timeout) will be invoked when
     * Semaphore_pend() is called.
     */
    Event_Handle event;
    /*!
     * @brief eventId if using Events
     * 
     * The default for this parameters is 1.
     */
    unsigned int eventId;
    /*!
     * @brief Semaphore mode
     * 
     * When mode is BINARY, the semaphore has only two states, available and
     * unavailable. When mode is COUNTING, the semaphore keeps track of number of
     * times a semaphore is posted.
     * 
     * The default for this parameter is COUNTING.
     */
    Semaphore_Mode mode;
    volatile uint16_t count;
    Queue_Elem pendQ;
    /*! @endcond */
};

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} Semaphore_Module_State;
/*! @endcond */

/*!
 * @brief Support Semaphores with Events?
 * 
 * The default for this parameter is false.
 *
 * This parameter is set in the app.syscfg file:
 * @code
 *   Semaphore.supportsEvents = true/false;
 * @endcode
 */
extern const bool Semaphore_supportsEvents;

/*!
 * @brief Support Task priority pend queuing?
 * 
 * The default for this parameter is true.
 *
 * This parameter is set in the app.syscfg file:
 * @code
 *   Semaphore.supportsPriority = true/false;
 * @endcode
 */
extern const bool Semaphore_supportsPriority;

/*! @cond NODOC */
typedef void (*Semaphore_EventPost)(Event_Handle arg1, unsigned int arg2);
extern const Semaphore_EventPost Semaphore_eventPost;

typedef void (*Semaphore_EventSync)(Event_Handle arg1, unsigned int arg2, unsigned int arg3);
extern const Semaphore_EventSync Semaphore_eventSync;

extern void Semaphore_Instance_init(Semaphore_Object *obj, int count, const Semaphore_Params *prms);

extern void Semaphore_Instance_finalize(Semaphore_Object *obj);
/*! @endcond */

/*!
 * @brief Create a Semaphore object
 * 
 * This function creates a new Semaphore object which is initialized to count.
 * 
 * @note
 * The "count" argument should not be a negative number as the Semaphore count
 * is stored as a 16-bit unsigned integer inside the Semaphore object.
 * 
 * @param count initial semaphore count
 * @param prms Semaphore parameters
 * @param eb error block
 * 
 * @retval Semaphore handle (NULL on failure)
 */
extern Semaphore_Handle Semaphore_create(int count, const Semaphore_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a semaphore
 *
 * Semaphore_construct is equivalent to Semaphore_create except that the
 * Semaphore_Struct is pre-allocated.  See Semaphore_create() for a description
 * of this API.
 *
 * @param obj pointer to a Semaphore object
 * @param count initial semaphore count
 * @param prms Semaphore parameters
 *
 * @retval Semaphore handle (NULL on failure)
 */
extern Semaphore_Handle Semaphore_construct(Semaphore_Struct *obj, int count, const Semaphore_Params *prms);

/*!
 * @brief Delete a semaphore
 *
 * Semaphore_delete deletes a Semaphore object. Note that Semaphore_delete takes
 * a pointer to a Semaphore_Handle which enables Semaphore_delete to set the
 * Semaphore_handle to NULL.
 * 
 * @param semaphore pointer to Semaphore handle
 */
extern void Semaphore_delete(Semaphore_Handle *semaphore);

/*!
 * @brief Destruct a semaphore
 *
 * Semaphore_destruct destructs a Semaphore object.
 *
 * @param obj pointer to Semaphore object
 */
extern void Semaphore_destruct(Semaphore_Struct *obj);

/*!
 * @brief Initialize the Semaphore_Params structure with default values.
 *
 * Semaphore_Params_init initializes the Semaphore_Params structure with default
 * values. Semaphore_Params_init should always be called before setting
 * individual parameter fields. This allows new fields to be added in the
 * future with compatible defaults -- existing source code does not need to
 * change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Semaphore_Params_init(Semaphore_Params *prms);

/*!
 * @brief return handle of the first Semaphore on Semaphore list 
 *
 * Return the handle of the first Semaphore on the create/construct list. NULL if no
 * Semaphore have been created or constructed.
 *
 * @retval Semaphore handle
 */
extern Semaphore_Handle Semaphore_Object_first(void);

/*!
 * @brief return handle of the next Semaphore on Semaphore list 
 *
 * Return the handle of the next Semaphore on the create/construct list. NULL if no
 * more Semaphore are on the list.
 *
 * @param semaphore Semaphore handle
 *
 * @retval Semaphore handle
 */
extern Semaphore_Handle Semaphore_Object_next(Semaphore_Handle semaphore);

/*!
 * @brief Get current semaphore count
 * 
 * This function returns the current value of the semaphore specified by the
 * handle.
 * 
 * A semaphore's count is incremented when Semaphore_post() is called. If
 * configured as a binary semaphore, the count will not increment past 1. If
 * configured as a counting semaphore, the count will continue incrementing and
 * will rollover to zero after reaching a count of 65,535. Care must be taken in
 * applications to avoid the rollover situation as a count of zero will always
 * be interpreted as an empty semaphore.
 * 
 * A semaphore's count is decremented, if non-zero, when Semaphore_pend() is
 * called. A task will block on a semaphore if the count is zero when
 * Semaphore_pend() is called. An empty semaphore will always have a count of
 * zero regardless of the number of tasks that are blocked on it.
 * 
 * @param semaphore Semaphore handle
 *
 * @retval current semaphore count
 */
extern int Semaphore_getCount(Semaphore_Handle semaphore);

/*!
 * @brief Wait for a semaphore
 * 
 * If the semaphore count is greater than zero (available), this function
 * decrements the count and returns true. If the semaphore count is zero
 * (unavailable), this function suspends execution of the current task (leaving
 * the count equal to zero) until post() is called or the timeout expires.
 * 
 * A timeout value of @link BIOS_WAIT_FOREVER @endlink causes the task to
 * wait indefinitely for its semaphore to be posted.
 * 
 * A timeout value of @link BIOS_NO_WAIT @endlink causes Semaphore_pend to
 * return immediately.
 * 
 * <h3>Event Object Note</h3>
 * If the Semaphore object has been configured with an embedded Event object,
 * then prior to returning from this function, the Event object's state is
 * updated to reflect the new value of 'count'. If 'count' is zero, then the
 * configured Event_Id is cleared in the Event object. If 'count' is non-zero,
 * then the configured Event_Id is set in the Event object.
 * 
 * @param semaphore Semaphore handle
 * @param timeout return after this many system time units
 * 
 * @retval true if successful, false if timeout
 */
extern bool Semaphore_pend(Semaphore_Handle semaphore, uint32_t timeout);

/*!
 * @brief Signal a semaphore.
 * 
 * If any tasks are waiting on the semaphore, this function readies the first
 * task waiting for the semaphore without incrementing the count. If no task is
 * waiting, this function simply increments the semaphore count and returns. In
 * the case of a binary semaphore, the count has a maximum value of one.
 *
 * @param semaphore Semaphore handle
 */
extern void Semaphore_post(Semaphore_Handle semaphore);

/*!
 * @brief Register an Event Object with a semaphore
 * 
 * Ordinarily, an Event object and eventId are configured at Semaphore create
 * time.
 * 
 * This API is provided so that Semaphore-using middleware can support implicit
 * Event posting without having to be retrofitted.
 * 
 * After the Event object and eventId are registered with the Semaphore:
 * 
 * Event_post(event, eventId) will be invoked when Semaphore_post(sem) is
 * called.
 * 
 * Event_pend(event, eventId, 0, timeout) will be invoked when
 * Semaphore_pend(sem, timeout) is called.
 * 
 * @param semaphore Semaphore handle
 * @param event void * to Event Object
 * 
 * @param eventId Event ID
 */
extern void Semaphore_registerEvent(Semaphore_Handle semaphore, Event_Handle event, unsigned int eventId);

/*!
 * @brief Reset semaphore count
 * 
 * Resets the semaphore count to count. No task switch occurs when calling
 * SEM_reset.
 * 
 * @pre
 * count must be greater than or equal to 0.
 * 
 * No tasks should be waiting on the semaphore when Semaphore_reset is called.
 * 
 * Semaphore_reset cannot be called by a Hwi or a Swi.
 * 
 * @param semaphore Semaphore handle
 * @param count semaphore count
 */
extern void Semaphore_reset(Semaphore_Handle semaphore, int count);

/*! @cond NODOC */

/*!
 * @brief This function is the clock event handler for pend
 */
extern void Semaphore_pendTimeout(uintptr_t arg);

static inline Semaphore_Handle Semaphore_handle(Semaphore_Struct *str)
{  
    return ((Semaphore_Handle)str);
}

static inline Semaphore_Struct * Semaphore_struct(Semaphore_Handle handle)
{  
    return ((Semaphore_Struct *)handle);
}

/*! @endcond */

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_knl_Semaphore__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Semaphore_long_names
#include "Semaphore_defs.h"
/*! @endcond */
