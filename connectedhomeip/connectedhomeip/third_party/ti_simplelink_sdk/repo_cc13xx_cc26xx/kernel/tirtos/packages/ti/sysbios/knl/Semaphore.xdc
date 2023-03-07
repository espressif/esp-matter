/*
 * Copyright (c) 2014-2016, Texas Instruments Incorporated
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
 */
/*
 *  ======== Semaphore.xdc ========
 *
 */

package ti.sysbios.knl;

import xdc.rov.ViewInfo;

import xdc.runtime.Diags;
import xdc.runtime.Log;
import xdc.runtime.Assert;

import ti.sysbios.knl.Queue;
import ti.sysbios.knl.Task;
import ti.sysbios.knl.Clock;

/*!
 *  ======== Semaphore ========
 *  Semaphore Manager
 *
 *  The Semaphore manager makes available a set of functions that manipulate
 *  semaphore objects. Semaphores can be used for task synchronization and
 *  mutual exclusion.
 *
 *  Semaphores can be counting semaphores or binary semaphores. Counting
 *  semaphores keep track of the number of times the semaphore has been posted
 *  with post(). This is useful, for example, if you have a group of resources
 *  that are shared between tasks. Such tasks might call pend() to see if a
 *  resource is available before using one.
 *
 *  Binary semaphores can have only two states: available (count = 1) and
 *  unavailable (count = 0). They can be used to share a single resource
 *  between tasks. They can also be used for a basic signaling mechanism,
 *  where the semaphore can be posted multiple times. Binary semaphores do
 *  not keep track of the count; they simply track whether the semaphore has
 *  been posted or not.
 *
 *  See {@link #getCount Semaphore_getCount()} for more details of the 'count'
 *  behavior.
 *
 *  The Mailbox module uses a counting semaphore internally to manage the
 *  count of free (or full) mailbox elements. Another example of a counting
 *  semaphore is an ISR that might fill multiple buffers of data for
 *  consumption by a task. After filling each buffer, the ISR puts the buffer on
 *  a queue and calls post(). The task waiting for the data calls pend(), which
 *  simply decrements the semaphore count and returns or blocks if the count is
 *  0. The semaphore count thus tracks the number of full buffers available for
 *  the task.
 *
 *  pend() is used to wait for a semaphore. The timeout parameter allows the
 *  task to wait until a timeout, wait indefinitely, or not wait at all. The
 *  return value is used to indicate if the semaphore was signaled successfully.
 *
 *  post() is used to signal a semaphore. If a task is waiting for the
 *  semaphore, post() removes the task from the semaphore queue and puts it on
 *  the ready queue. If no tasks are waiting, post() simply increments the
 *  semaphore count and returns.  For a binary semaphore the count is always
 *  set to one.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function               </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                  -->
 *    <tr><td> {@link #Params_init}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}     </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}      </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getCount}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #pend}          </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td> {@link #post}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #registerEvent} </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #reset}         </td><td>   N    </td><td>   N    </td>
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
 *    (e.g. Semaphore_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. Semaphore_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
@InstanceFinalize       /* to destruct queue */
@InstanceInitStatic     /* Construct/Destruct CAN becalled at runtime */

module Semaphore
{
    /*!
     *  Semaphore types.
     *
     *  These enumerations specify the type of semaphore.
     *
     *  Tasks wait for the semaphore in FIFO order unless the PRIORITY
     *  option is chosen.
     *
     *  For PRIORITY semaphores, the pending task will be
     *  inserted in the waiting list before the first task that has
     *  lower priority.  This ensures that tasks of equal priority will
     *  pend in FIFO order.
     *
     *  @a(WARNING)
     *  PRIORITY semaphores have a potential to increase the interrupt
     *  latency in the system since interrupts are disabled while the list of
     *  tasks waiting on the  semaphore is scanned for the proper insertion
     *  point.  This is typically about a dozen instructions per waiting task.
     *  For example, if you have 10 tasks of higher priority waiting, then all
     *  10 will be checked with interrupts disabled before the new task is
     *  entered onto the list.
     */
    enum Mode {
        Mode_COUNTING = 0x0,           /*! Counting (FIFO) */
        Mode_BINARY = 0x1,             /*! Binary (FIFO) */
        Mode_COUNTING_PRIORITY = 0x2,  /*! Counting (priority-based) */
        Mode_BINARY_PRIORITY = 0x3     /*! Binary (priority-based) */
    };

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String          label;
        String          event;
        String          eventId;
        String          mode;
        Int             count;
        String          pendedTasks[];
    };

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Basic', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitBasic', structName: 'BasicView'}]
            ]
        });

    // -------- Module Parameters --------

    /*!
     *  ======== LM_post ========
     *  Logged on calls to Semaphore_post()
     */
    config Log.Event LM_post = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_post: sem: 0x%x, count: %d"
    };

    /*!
     *  ======== LM_pend ========
     *  Logged on calls to Semaphore_pend()
     */
    config Log.Event LM_pend = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_pend: sem: 0x%x, count: %d, timeout: %d"
    };

    /*!
     *  ======== A_noEvents ========
     *  Assert raised if application uses Event but it's not supported
     *
     *  This assertion is triggered by {@link #create} if
     *  {@link #supportsEvents} is false and an {@link ti.sysbios.knl.Event}
     *  object is passed to {@link #create}.
     */
    config Assert.Id A_noEvents = {
        msg: "A_noEvents: The Event.supportsEvents flag is disabled."
    };

    /*!
     *  ======== A_invTimeout ========
     *  @_nodoc
     *  This assertion is no longer used
     */
    config Assert.Id A_invTimeout = {
        msg: "A_invTimeout: Can't use BIOS_EVENT_ACQUIRED with this Semaphore."
    };

    /*!
     *  ======== A_badContext ========
     *  Assert raised if an operation is invalid in the current calling context
     *
     *  Asserted when {@link #pend} is called with non-zero timeout from
     *  other than a Task context.
     */
    config Assert.Id A_badContext = {
        msg: "A_badContext: bad calling context. Must be called from a Task."
    };

    /*!
     *  ======== A_overflow ========
     *  Assert raised if the semaphore count is incremented past 65535
     *
     *  Asserted when Semaphore_post() has been called when the 16 bit
     *  semaphore count is at its maximum value of 65535.
     */
    config Assert.Id A_overflow = {
        msg: "A_overflow: Count has exceeded 65535 and rolled over."
    };

    /*!
     *  ======== A_pendTaskDisabled ========
     *  Asserted in Sempahore_pend()
     *
     *  Assert raised if Semaphore_pend() is called with the Task or
     *  Swi scheduler disabled.
     */
    config Assert.Id A_pendTaskDisabled = {
        msg: "A_pendTaskDisabled: Cannot call Semaphore_pend() while the Task or Swi scheduler is disabled."
    };

    /*!
     *  ======== supportsEvents ========
     *  Support Semaphores with Events?
     *
     *  The default for this parameter is false.
     */
    config Bool supportsEvents = false;

    /*!
     *  ======== supportsPriority ========
     *  Support Task priority pend queuing?
     *
     *  The default for this parameter is true.
     */
    config Bool supportsPriority = true;

instance:

    /*!
     *  ======== create ========
     *  Create a Semaphore object
     *
     *  This function creates a new Semaphore object which is initialized to
     *  count.
     *
     *  @param(count)   initial semaphore count
     *
     *  @a(NOTE)
     *  The "count" argument should not be a negative number as the Semaphore
     *  count is stored as a 16-bit unsigned integer inside the Semaphore
     *  object.
     */
    create(Int count);

    /*!
     *  ======== event ========
     *  Event instance to use if non-NULL
     *
     *  The default value of this parameter is null.  If event is non-null:
     *  @p(blist)
     *      - Event_post(sem->event, sem->eventId) will be invoked when
     *        Semaphore_post() is called.
     *
     *      - Event_pend(sem->event, 0, sem->eventId, timeout) will be
     *        invoked when Semaphore_pend() is called.
     *  @p
     */
    config Event.Handle event = null;

    /*!
     *  ======== eventId ========
     *  eventId if using Events
     *
     *  The default for this parameters is 1.
     */
    config UInt eventId = 1;

    /*!
     *  ======== mode ========
     *  Semaphore mode
     *
     *  When mode is BINARY, the semaphore has only two states, available
     *  and unavailable. When mode is COUNTING, the semaphore keeps track of
     *  number of times a semaphore is posted.
     *
     *  The default for this parameter is COUNTING.
     */
    config Mode mode = Mode_COUNTING;

    /*!
     *  ======== getCount ========
     *  Get current semaphore count
     *
     *  This function returns the current value of the semaphore specified by
     *  the handle.
     *
     *  A semaphore's count is incremented when Semaphore_post() is called.
     *  If configured as a binary semaphore, the count will not increment past
     *  1. If configured as a counting semaphore, the count will continue
     *  incrementing and will rollover to zero after reaching a count of
     *  65,535. Care must be taken in applications to avoid the rollover
     *  situation as a count of zero will always be interpreted as an empty
     *  semaphore.
     *
     *  A semaphore's count is decremented, if non-zero, when Semaphore_pend()
     *  is called. A task will block on a semaphore if the count is zero when
     *  Semaphore_pend() is called. An empty semaphore will always have a
     *  count of zero regardless of the number of tasks that are blocked on
     *  it.
     *
     *  @b(returns)             current semaphore count
     */
    Int getCount();

    /*!
     *  ======== pend ========
     *  Wait for a semaphore
     *
     *  If the semaphore count is greater than zero (available), this function
     *  decrements the count and returns TRUE. If the semaphore count is zero
     *  (unavailable), this function suspends execution of the current task
     *  (leaving the count equal to zero) until post() is called or the
     *  timeout expires.
     *
     *  A timeout value of
     *  {@link ti.sysbios.BIOS#WAIT_FOREVER BIOS_WAIT_FOREVER} causes
     *  the task to wait indefinitely for its semaphore to be posted.
     *
     *  A timeout value of {@link ti.sysbios.BIOS#NO_WAIT BIOS_NO_WAIT}
     *  causes Semaphore_pend to return immediately.
     *
     *  @a(Event Object Note)
     *  If the Semaphore object has been configured with an embedded Event
     *  object, then prior to returning from this function, the Event object's
     *  state is updated to reflect the new value of 'count'. 
     *  If 'count' is zero, then the configured Event_Id is cleared in the
     *  Event object. If 'count' is non-zero, then the configured Event_Id
     *  is set in the Event object.
     *
     *  @param(timeout)     return after this many system time units
     *
     *  @b(returns)         TRUE if successful, FALSE if timeout
     */
    Bool pend(UInt32 timeout);

    /*!
     *  ======== post ========
     *  Signal a semaphore.
     *
     *  If any tasks are waiting on the semaphore, this function readies
     *  the first task waiting for the semaphore without incrementing
     *  the count. If no task is waiting, this function simply increments
     *  the semaphore count and returns. In the case of a binary semaphore,
     *  the count has a maximum value of one.
     */
    Void post();

    /*!
     *  ======== registerEvent ========
     *  Register an Event Object with a semaphore
     *
     *  Ordinarily, an Event object and eventId are configured at
     *  Semaphore create time.
     *
     *  This API is provided so that Semaphore-using middleware
     *  can support implicit Event posting without having to be
     *  retrofitted.
     *
     *  After the Event object and eventId are registered with the
     *  Semaphore:

     *  Event_post(event, eventId) will be invoked when
     *  Semaphore_post(sem) is called.
     *
     *  Event_pend(event, eventId, 0, timeout) will be invoked when
     *  Semaphore_pend(sem, timeout) is called.
     *
     *  @param(event)                     Ptr to Event Object
     *  @param(eventId)                   Event ID
     *
     */
    Void registerEvent(Event.Handle event, UInt eventId);

    /*!
     *  ======== reset ========
     *  Reset semaphore count
     *
     *  Resets the semaphore count to count.
     *  No task switch occurs when calling SEM_reset.
     *
     *  @a(constraints)
     *  count must be greater than or equal to 0.
     *
     *  No tasks should be waiting on the semaphore when
     *  Semaphore_reset is called.
     *
     *  Semaphore_reset cannot be called by a Hwi or a Swi.
     *
     *  @param(count)                   semaphore count
     *
     */
    Void reset(Int count);

internal:

    /*
     * Event module function pointers. Used to decouple
     * Semaphore from Event when supportsEvents is false
     */
    config Void (*eventPost)(Event.Handle, UInt);

    config Void (*eventSync)(Event.Handle, UInt, UInt);

    /*!
     *  ======== pendTimeout ========
     *  This function is the clock event handler for pend
     */
    Void pendTimeout(UArg arg);

    /* pendQ Element PendStates */
    enum PendState {
        PendState_TIMEOUT = 0,          /* Clock timeout */
        PendState_POSTED = 1,           /* posted by post */
        PendState_CLOCK_WAIT = 2,       /* in Clock queue */
        PendState_WAIT_FOREVER = 3      /* not in Clock queue */
    };

    /* Semaphore pendQ Element */
    struct PendElem {
        Task.PendElem           tpElem;
        PendState               pendState;
    };

    struct Instance_State {
        Event.Handle            event;
        UInt                    eventId;
        Mode                    mode;   /* binary or counting */
        volatile UInt16         count;  /* curr semaphore count */
        Queue.Object            pendQ;  /* queue of PendElems */
    };
}
