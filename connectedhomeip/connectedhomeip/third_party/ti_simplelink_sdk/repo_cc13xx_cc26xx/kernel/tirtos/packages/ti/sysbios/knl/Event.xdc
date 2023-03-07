/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *  ======== Event.xdc ========
 */

package ti.sysbios.knl;

import xdc.rov.ViewInfo;

import xdc.runtime.Assert;
import xdc.runtime.Diags;
import xdc.runtime.Log;

import ti.sysbios.knl.Queue;
import ti.sysbios.knl.Clock;
import ti.sysbios.knl.Task;

/*!
 *  ======== Event ========
 *  Event Manager.
 *
 *  SYS/BIOS events are a means of communication between Tasks and other
 *  threads such as Hwis, Swis, and other Tasks, or between Tasks and
 *  other SYS/BIOS objects. Other SYS/BIOS objects include semaphores,
 *  mailboxes,
 *  message queues, etc. Only tasks can wait for events; whereas tasks,
 *  Hwis, Swis, or SYS/BIOS objects can post them.
 *
 *  In order for a task to be notified of an event from a SYS/BIOS object an
 *  event object must first be registered with that object. Separate APIs
 *  are provided (in their respective modules) for each of the SYS/BIOS object
 *  types that support this feature.
 *
 *  Events are synchronous in nature, meaning that a receiving task will
 *  block or pend while waiting for the events to occur. When the desired
 *  events are received, the pending task continues its execution, as it
 *  would after a call to Semaphore_pend(), for example.
 *
 *  Tasks can also wait on events that are not linked to other SYS/BIOS objects.
 *  These events are explicitly posted from other threads such as tasks,
 *  Swis, or Hwis. A task does not register to receive these events; the
 *  sending thread simply posts its event(s) to the event object the task
 *  is pending on. This scenario is similar to having an ISR post a
 *  semaphore.
 *
 *  A task can wait on events from multiple resources and/or threads; thus,
 *  it can be waiting for a semaphore to be posted and for a message to
 *  arrive in a message queue or an ISR thread to signal that an event has
 *  occurred.
 *
 *  Unlike Semaphores, only a single task can pend on an Event object.
 *
 *  {@link #pend} is used to wait for events. The andMask & orMask
 *  determine which
 *  event(s) must occur before returning from {@link #pend}. The timeout
 *  parameter
 *  allows the task to wait until a timeout, wait indefinitely, or not wait
 *  at all. A return value of zero indicates that a timeout has occurred. A
 *  non-zero return value is the set of events that were active at the time
 *  the task was unblocked.
 *
 *  The andMask defines a set of events that must ALL occur to allow
 *  {@link #pend} to return.
 *
 *  The orMask defines a set of events that will cause {@link #pend} to
 *  return if ANY of them occur.
 *
 *  Events are binary. Events become available (posted) on each Event_post()
 *  of the eventId and become non-available (consumed) on each qualifying
 *  Event_pend() mask. 
 *
 *  All active events present in the orMask are consumed (ie removed from
 *  the event object) upon return from {@link #pend}. Only when all events
 *  present in the andMask are active are they consumed on return from
 *  {@link #pend}.
 *
 *  @a(Caveat)
 *  @p(html)
 *  <BLOCKQUOTE>
 *  When Events are implicitly posted while used in conjunction with
 *  Semaphore or Mailbox objects, then following the call to Event_pend()
 *  which consumes the matching Event_IDs pended on, the Event object will be
 *  updated by the intervening Semaphore_pend() or Mailbox_pend/post()
 *  call so that it reflects the current state of availability of the
 *  corresponding Semaphore or Mailbox object.
 *  </BLOCKQUOTE>
 *  @p
 *
 *  {@link #pend} returns immediately if the andMask OR orMask conditions
 *  are true upon entry.
 *
 *  {@link #post} is used to signal events. If a task is waiting for the event
 *  and all of the event conditions are met, {@link #post} unblocks the task.
 *  If no task is waiting, {@link #post} simply registers the event with the
 *  event object and returns.
 *
 *  The maximum number of eventIds supported is target specific and depends
 *  on the number of bits in a UInt data type.
 *  For 6x and ARM devices the maximum number of eventIds is therefore 32.
 *  For 28x the maximum number of eventIds is 16.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                  -->
 *    <tr><td> {@link #create}          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getPostedEvents} </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #pend}            </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td> {@link #post}            </td><td>   Y    </td><td>   Y    </td>
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
 *    (e.g. Event_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. Event_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
/*
 *  To remove Event.Ids not defined for targets with 16 bit Ints
 */
@CustomHeader
@InstanceInitStatic     /* Construct/Destruct CAN becalled at runtime */

module Event
{
    /*!
     *  Pre-defined Event Ids
     *
     *  Event_Ids are provided to simplify the specification of
     *  andMasks and orMasks arguments to {@link #pend Event_pend()}.
     *
     *  Since each Event_Id is a bitmask composed of only a single bit,
     *  a group of Event_Ids within an andMask or orMask can be indicated
     *  by simply adding them together. For instance, to indicate that a task
     *  is to be awakened only when both Event_Id_00 and Event_Id_01 have
     *  been posted, the andMask for Event_pend() would be constructed
     *  as below:
     *
     *  @p(code)
     *  Event_pend(event, Event_Id_00+EventId_01, Event_Id_NONE,
     *             BIOS_WAIT_FOREVER);
     *  @p
     *
     *  As shown above, Event_Id_NONE is to be used as an empty (NULL)
     *  andMask or orMask.
     *
     *  @a(Note)
     *  For targets where a UInt is 32 bits in length,
     *  Event_Id_00 thru Event_Id_31 can be used.
     *
     *  For targets where a UInt is 16 bits in length,
     *  Event_Id_00 thru Event_Id_15 can be used.
     */

    const UInt Id_00 = 0x1;
    const UInt Id_01 = 0x2;         /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_02 = 0x4;         /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_03 = 0x8;         /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_04 = 0x10;        /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_05 = 0x20;        /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_06 = 0x40;        /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_07 = 0x80;        /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_08 = 0x100;       /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_09 = 0x200;       /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_10 = 0x400;       /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_11 = 0x800;       /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_12 = 0x1000;      /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_13 = 0x2000;      /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_14 = 0x4000;      /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_15 = 0x8000;      /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_16 = 0x10000;     /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_17 = 0x20000;     /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_18 = 0x40000;     /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_19 = 0x80000;     /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_20 = 0x100000;    /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_21 = 0x200000;    /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_22 = 0x400000;    /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_23 = 0x800000;    /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_24 = 0x1000000;   /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_25 = 0x2000000;   /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_26 = 0x4000000;   /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_27 = 0x8000000;   /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_28 = 0x10000000;  /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_29 = 0x20000000;  /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_30 = 0x40000000;  /*! @see #Id_00 Pre-defined Event IDs. */
    const UInt Id_31 = 0x80000000;  /*! @see #Id_00 Pre-defined Event IDs. */

    const UInt Id_NONE = 0;         /*! @see #Id_00 Pre-defined Event IDs. */
    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String          label;
        String          postedEvents;
        String          pendedTask;
        String          andMask;
        String          orMask;
        String          timeout;
    };

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Basic', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitBasic',
                structName: 'BasicView'}]
            ]
        });

    // -------- Module Parameters --------

    // Logs

    /*! Logged on calls to Event_post() */
    config Log.Event LM_post = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_post: event: 0x%x, currEvents: 0x%x, eventId: 0x%x"
    };

    /*! Logged on calls to Event_pend() */
    config Log.Event LM_pend = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_pend: event: 0x%x, currEvents: 0x%x, andMask: 0x%x, orMask: 0x%x, timeout: %d"
    };

    // Asserts

    /*!
     *  Asserted when {@link #pend} is called with andMask and orMask == 0
     */
    config Assert.Id A_nullEventMasks = {
        msg: "A_nullEventMasks: orMask and andMask are null."
    };

    /*!
     *  Asserted when {@link #post} is called with eventId == 0
     */
    config Assert.Id A_nullEventId = {
        msg: "A_nullEventId: posted eventId is null."
    };

    /*!
     *  Asserted when {@link #pend} is called by multiple tasks on the same
     *  Event object.
     */
    config Assert.Id A_eventInUse = {
        msg: "A_eventInUse: Event object already in use."
    };

    /*!
     *  Asserted when {@link #pend} is called with non-zero timeout from other
     *  than a Task context.
     */
    config Assert.Id A_badContext = {
        msg: "A_badContext: bad calling context. Must be called from a Task."
    };

    /*!
     *  ======== A_pendTaskDisabled ========
     *  Asserted in Event_pend()
     *
     *  Assert raised if Event_pend() is called with the Task or
     *  Swi scheduler disabled.
     */
    config Assert.Id A_pendTaskDisabled = {
        msg: "A_pendTaskDisabled: Cannot call Event_pend() while the Task or Swi scheduler is disabled."
    };

instance:

    /*!
     *  ======== create ========
     *  Create an Event object.
     *
     *  This function creates a new Event object.
     */
    create();

    /*!
     *  ======== pend ========
     *  Wait for events defined in 'and' OR 'or' masks.
     *
     *  pend() is used to wait for events. The andMask and orMask determine
     *  which
     *  event(s) must occur before returning from pend(). The timeout parameter
     *  allows the task to wait until a timeout, wait indefinitely, or not wait
     *  at all. A return value of zero indicates that a timeout has occurred. A
     *  non-zero return value is the set of events that were active at the time
     *  the task was unblocked.
     *
     *  The andMask defines a set of events that must
     *  ALL occur to allow pend() to return.
     *
     *  The orMask defines a set of events
     *  that will cause pend() to return if ANY of them occur.
     *
     *  All active events
     *  present in the orMask are consumed (i.e. removed from the event object)
     *  upon return from pend(). Only when all events present in the andMask
     *  are active are they consumed on return from pend().
     *
     *  The pend conditions are satisfied when either ALL of the events
     *  in the andMask occur or ANY of the events in the orMask occur.
     *
     *  A timeout value of
     *  {@link ti.sysbios.BIOS#WAIT_FOREVER, BIOS_WAIT_FOREVER} causes
     *  the task to wait indefinitely for matching events to be posted.
     *
     *  A timeout value of {@link ti.sysbios.BIOS#NO_WAIT, BIOS_NO_WAIT}
     *  causes Event_pend to return immediately.
     *
     *  @param(andMask)         return from pend() when ALL of these
     *                          events have occurred
     *  @param(orMask)          return from pend() when ANY of these
     *                          events have occurred
     *  @param(timeout)         return from pend() after this many system
     *                          time units
     *
     *  @b(returns)             All consumed events or zero if timeout.
     */
    UInt pend(UInt andMask, UInt orMask, UInt32 timeout);

    /*!
     *  ======== post ========
     *  Post event(s) to an event object.
     *
     *  Event_post() is used to signal events. If a task is waiting for the
     *  event and the event conditions are met, post() unblocks the
     *  task. If no tasks are waiting, post() simply registers the event with
     *  the event object and returns.
     *
     *  @param(eventMask)         mask of eventIds to post (must be non-zero)
     */
    Void post(UInt eventMask);

    /*!
     *  ======== getPostedEvents ========
     *  Returns the set of events that have not been consumed by a task
     *  (ie those events that have not fully satisfied any pend() conditions).
     *  No events are consumed!
     *
     *  @b(returns)             All un-consumed events posted to the event.
     */
    UInt getPostedEvents();

    /*!
     *  @_nodoc
     *  ======== sync ========
     *  synchronize an eventId to the value given
     *  must call with interrupts disabled
     *
     *  @param(eventId)         eventId to sync
     *  @param(count)           count
     */
    Void sync(UInt eventId, UInt count);

    /*!
     *  @_nodoc
     *  ======== syncMeta ========
     *  synchronize an eventId to the value given (static version)
     *
     *  @param(eventId)         eventId to sync
     *  @param(count)           count
     */
    metaonly Void syncMeta(UInt eventId, UInt count);

internal:

    /*!
     *  ======== pendTimeout ========
     *  This function is the clock event handler for pend
     */
    Void pendTimeout(UArg arg);

    /*
     *  ======== checkEvents ========
     *  Checks postedEvents for matching event conditions.
     *  Returns matchingEvents if a match and consumes matched events,
     *  else returns 0 and consumes nothing.
     *  Called with ints disabled
     */
    UInt checkEvents (Object *event, UInt andMask, UInt orMask);

    /*
     *  Static instance array used by Semaphore module
     *  to enable Grace to display a list of Event instances
     *  to choose from to configure a Semaphore's 'event' config
     *  parameter.
     */
    metaonly config Any eventInstances[];

    /* pendQ Element PendStates */
    enum PendState {
        PendState_TIMEOUT = 0,          /* Clock timeout */
        PendState_POSTED = 1,           /* posted by post */
        PendState_CLOCK_WAIT = 2,       /* in Clock queue */
        PendState_WAIT_FOREVER = 3      /* not in Clock queue */
    };

    /* Event pendQ Element */
    struct PendElem {
        Task.PendElem           tpElem;
        volatile PendState      pendState;
        UInt                    matchingEvents;
        UInt                    andMask;
        UInt                    orMask;
    };

    struct Instance_State {
        volatile UInt           postedEvents;   /* Current unconsumed events */
        Queue.Object            pendQ;          /* queue of PendElems  */
                                                /* for compatibility with */
                                                /* Semaphore */
    };
}
