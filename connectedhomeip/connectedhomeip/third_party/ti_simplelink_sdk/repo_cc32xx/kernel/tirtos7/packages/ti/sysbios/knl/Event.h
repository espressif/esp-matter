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
 * @file ti/sysbios/knl/Event.h
 * @brief Event Manager.
 *
 * SYS/BIOS events are a means of communication between Tasks and other threads
 * such as Hwis, Swis, and other Tasks, or between Tasks and other SYS/BIOS
 * objects. Other SYS/BIOS objects include semaphores, mailboxes, message
 * queues, etc. Only tasks can wait for events; whereas tasks, Hwis, Swis, or
 * SYS/BIOS objects can post them.
 *
 * In order for a task to be notified of an event from a SYS/BIOS object an
 * event object must first be registered with that object. Separate APIs are
 * provided (in their respective modules) for each of the SYS/BIOS object types
 * that support this feature.
 *
 * Events are synchronous in nature, meaning that a receiving task will block or
 * pend while waiting for the events to occur. When the desired events are
 * received, the pending task continues its execution, as it would after a call
 * to Semaphore_pend(), for example.
 *
 * Tasks can also wait on events that are not linked to other SYS/BIOS objects.
 * These events are explicitly posted from other threads such as tasks, Swis, or
 * Hwis. A task does not register to receive these events; the sending thread
 * simply posts its event(s) to the event object the task is pending on. This
 * scenario is similar to having an ISR post a semaphore.
 *
 * A task can wait on events from multiple resources and/or threads; thus, it
 * can be waiting for a semaphore to be posted and for a message to arrive in a
 * message queue or an ISR thread to signal that an event has occurred.
 *
 * Unlike Semaphores, only a single task can pend on an Event object.
 *
 * @link Event_pend @endlink is used to wait for events. The andMask & orMask
 * determine which event(s) must occur before returning from @link Event_pend
 * @endlink. The timeout parameter allows the task to wait until a timeout, wait
 * indefinitely, or not wait at all. A return value of zero indicates that a
 * timeout has occurred. A non-zero return value is the set of events that were
 * active at the time the task was unblocked.
 *
 * The andMask defines a set of events that must ALL occur to allow @link
 * Event_pend @endlink to return.
 *
 * The orMask defines a set of events that will cause @link Event_pend @endlink
 * to return if ANY of them occur.
 *
 * Events are binary. Events become available (posted) on each Event_post() of
 * the eventId and become non-available (consumed) on each qualifying
 * Event_pend() mask.
 *
 * All active events present in the orMask are consumed (ie removed from the
 * event object) upon return from @link Event_pend @endlink. Only when all
 * events present in the andMask are active are they consumed on return from
 * @link Event_pend @endlink.
 *
 *
 * To use the Event module,
 * the following must be added to the app.syscfg file:
 *
 * @code 
 * const Event = scripting.addModule("/ti/sysbios/knl/Event");
 * @endcode 
 *
 * <h3>Caveat</h3>
 *
 * @htmlonly
 *  <BLOCKQUOTE>
 *  When Events are implicitly posted while used in conjunction with
 *  Semaphore or Mailbox objects, then following the call to Event_pend()
 *  which consumes the matching Event_IDs pended on, the Event object will be
 *  updated by the intervening Semaphore_pend() or Mailbox_pend/post()
 *  call so that it reflects the current state of availability of the
 *  corresponding Semaphore or Mailbox object.
 *  </BLOCKQUOTE>
 * @endhtmlonly
 *
 * @link Event_pend @endlink returns immediately if the andMask OR orMask
 * conditions are true upon entry.
 *
 * @link Event_post @endlink is used to signal events. If a task is waiting for
 * the event and all of the event conditions are met, @link Event_post @endlink
 * unblocks the task. If no task is waiting, @link Event_post @endlink simply
 * registers the event with the event object and returns.
 *
 * The maximum number of eventIds supported is target specific and depends on
 * the number of bits in a unsigned int data type. For 6x and ARM devices the maximum
 * number of eventIds is therefore 32. For 28x the maximum number of eventIds is
 * 16.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                  -->
 *    <tr><td> Event_create          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Event_Params_init     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Event_construct       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Event_delete          </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Event_destruct        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Event_getPostedEvents </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Event_pend            </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td> Event_post            </td><td>   Y    </td><td>   Y    </td>
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
 *    (e.g. after Event_init() has been called). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. before Event_init() has been called). </li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

#ifndef ti_sysbios_knl_Event__include
#define ti_sysbios_knl_Event__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
#define ti_sysbios_knl_Event_long_names
#include "Event_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Asserted when Event_pend() is called with andMask and orMask == 0
 */
#define Event_A_nullEventMasks "orMask and andMask are null"

/*!
 * @brief  Asserted when Event_post() is called with eventId == 0
 */
#define Event_A_nullEventId "posted eventId is null"

/*!
 * @brief Asserted when Event_pend() is called by multiple tasks on the same
 * Event object.
 */
#define Event_A_eventInUse "event object already in use"

/*!
 * @brief Asserted when Event_pend() is called with non-zero timeout from other
 * than a Task context.
 */
#define Event_A_badContext "bad calling context - must be called from a Task"

/*!
 * @brief Assert raised if Event_pend() is called with the Task or
 * Swi scheduler disabled.
 */
#define Event_A_pendTaskDisabled "cannot call Event_pend() while the Task or Swi scheduler is disabled"

/*!
 * @brief Pre-defined Event Ids
 *
 * Event_Ids are provided to simplify the specification of andMasks and orMasks
 * arguments to @ref Event_pend "Event_pend()".
 *
 * Since each Event_Id is a bitmask composed of only a single bit, a group of
 * Event_Ids within an andMask or orMask can be indicated by simply adding them
 * together. For instance, to indicate that a task is to be awakened only when
 * both Event_Id_00 and Event_Id_01 have been posted, the andMask for
 * Event_pend() would be constructed as below:
 *
 * @code
 *  Event_pend(event, Event_Id_00+Event_Id_01, Event_Id_NONE,
 *             BIOS_WAIT_FOREVER);
 * @endcode
 *
 * As shown above, Event_Id_NONE is to be used as an empty (NULL) andMask or
 * orMask.
 *
 * @note
 * For targets where a unsigned int is 32 bits in length, Event_Id_00 thru Event_Id_31
 * can be used. For targets where a unsigned int is 16 bits in length, Event_Id_00
 * thru Event_Id_15 can be used.
 */
#define Event_Id_00 (0x1)
#define Event_Id_01 (0x2)
#define Event_Id_02 (0x4)
#define Event_Id_03 (0x8)
#define Event_Id_04 (0x10)
#define Event_Id_05 (0x20)
#define Event_Id_06 (0x40)
#define Event_Id_07 (0x80)
#define Event_Id_08 (0x100)
#define Event_Id_09 (0x200)
#define Event_Id_10 (0x400)
#define Event_Id_11 (0x800)
#define Event_Id_12 (0x1000)
#define Event_Id_13 (0x2000)
#define Event_Id_14 (0x4000)
#define Event_Id_15 (0x8000)
#define Event_Id_16 (0x10000)
#define Event_Id_17 (0x20000)
#define Event_Id_18 (0x40000)
#define Event_Id_19 (0x80000)
#define Event_Id_20 (0x100000)
#define Event_Id_21 (0x200000)
#define Event_Id_22 (0x400000)
#define Event_Id_23 (0x800000)
#define Event_Id_24 (0x1000000)
#define Event_Id_25 (0x2000000)
#define Event_Id_26 (0x4000000)
#define Event_Id_27 (0x8000000)
#define Event_Id_28 (0x10000000)
#define Event_Id_29 (0x20000000)
#define Event_Id_30 (0x40000000)
#define Event_Id_31 (0x80000000)
#define Event_Id_NONE (0)


/*
 * ======== INTERNAL DEFINITIONS ========
 */

typedef enum {
    Event_PendState_TIMEOUT = 0,
    Event_PendState_POSTED = 1,
    Event_PendState_CLOCK_WAIT = 2,
    Event_PendState_WAIT_FOREVER = 3
} Event_PendState;

typedef struct {
    Task_PendElem tpElem;
    Event_PendState pendState;
    unsigned int matchingEvents;
    unsigned int andMask;
    unsigned int orMask;
} Event_PendElem;

struct Event_Params {
    unsigned int dummy;
};

struct Event_Struct {
    Queue_Elem objElem;
    volatile unsigned int postedEvents;
    Queue_Struct pendQ;
};

typedef struct Event_Params Event_Params;
typedef struct Event_Struct Event_Object;
typedef struct Event_Struct Event_Struct;
typedef struct Event_Struct *Event_Handle;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} Event_Module_State;
/*! @endcond */

/*
 * ======== FUNCTION DECLARATIONS ========
 */

/*! @cond NODOC */
extern void Event_Instance_init(Event_Object *obj, const Event_Params *params);
/*! @endcond */

/*!
 * @brief Create an Event object.
 *
 * This function creates a new Event object.
 */
extern Event_Handle Event_create(const Event_Params *params, Error_Block *eb);

/*!
 * @brief Construct an Event object.
 *
 * Event_construct is equivalent to Event_create except that the Event_Struct
 * is pre-allocated. See Event_construct for a description of this API.
 */
extern Event_Handle Event_construct(Event_Struct *obj, const Event_Params *params);

/*!
 * @brief Delete an Event
 *
 * Event_delete deletes a Event object. Note that Event_delete takes a pointer to
 * a Event_Handle which enables Event_delete to set the Event_handle to NULL.
 *
 * @param event pointer to Event handle
 */
void Event_delete(Event_Handle *event);

/*!
 * @brief Destruct an Event
 *
 * Event_destruct destructs an Event object.
 *
 * @param obj pointer to Event object
 */
extern void Event_destruct(Event_Struct *obj);

/*!
 * @brief Wait for events defined in 'and' OR 'or' masks.
 *
 * pend() is used to wait for events. The andMask and orMask determine which
 * event(s) must occur before returning from pend(). The timeout parameter
 * allows the task to wait until a timeout, wait indefinitely, or not wait at
 * all. A return value of zero indicates that a timeout has occurred. A non-zero
 * return value is the set of events that were active at the time the task was
 * unblocked.
 *
 * The andMask defines a set of events that must ALL occur to allow pend() to
 * return.
 *
 * The orMask defines a set of events that will cause pend() to return if ANY of
 * them occur.
 *
 * All active events present in the orMask are consumed (i.e. removed from the
 * event object) upon return from pend(). Only when all events present in the
 * andMask are active are they consumed on return from pend().
 *
 * The pend conditions are satisfied when either ALL of the events in the
 * andMask occur or ANY of the events in the orMask occur.
 *
 * A timeout value of @link BIOS_WAIT_FOREVER @endlink
 * causes the task to wait indefinitely for matching events to be posted.
 *
 * A timeout value of @link BIOS_NO_WAIT @endlink causes
 * Event_pend to return immediately.
 *
 * events have occurred
 *
 * events have occurred
 *
 * time units
 *
 * @param event Event handle
 * @param andMask return from pend() when ALL of these
 * @param orMask return from pend() when ANY of these
 * @param timeout return from pend() after this many system
 *
 * @retval All consumed events or zero if timeout.
 */
extern unsigned int Event_pend(Event_Handle event, unsigned int andMask, unsigned int orMask, uint32_t timeout);

/*!
 * @brief Post event(s) to an event object.
 *
 * Event_post() is used to signal events. If a task is waiting for the event and
 * the event conditions are met, post() unblocks the task. If no tasks are
 * waiting, post() simply registers the event with the event object and returns.
 *
 * @param event Event handle
 * @param eventMask mask of eventIds to post (must be non-zero)
 */
extern void Event_post(Event_Handle event, unsigned int eventMask);

/*!
 * @brief Returns the set of events that have not been consumed by a task (ie
 * those events that have not fully satisfied any pend() conditions). No events
 * are consumed!
 *
 * @param event Event handle
 *
 * @retval All un-consumed events posted to the event.
 */
extern unsigned int Event_getPostedEvents(Event_Handle event);

/*!
 * @brief Initialize the Event_Params structure with default values.
 *
 * Event_Params_init initializes the Event_Params structure with default values.
 * Event_Params_init should always be called before setting individual parameter
 * fields. This allows new fields to be added in the future with compatible
 * defaults -- existing source code does not need to change when new fields
 * are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Event_Params_init(Event_Params *prms);

/*!
 * @brief return handle of the first Event on Event list 
 *
 * Return the handle of the first Event on the create/construct list. NULL if no
 * Events have been created or constructed.
 *
 * @retval Event handle
 */
extern Event_Handle Event_Object_first(void);

/*!
 * @brief return handle of the next Event on Event list 
 *
 * Return the handle of the next Event on the create/construct list. NULL if no
 * more Events are on the list.
 *
 * @param event Event handle
 *
 * @retval Event handle
 */
extern Event_Handle Event_Object_next(Event_Handle event);

/*! @cond NODOC */
/*!
 * @brief synchronize an eventId to the value given must call with interrupts
 * disabled
 *
 * @param eventId eventId to sync
 *
 * @param count count
 */
extern void Event_sync(Event_Handle event, unsigned int eventId, unsigned int count);

extern void Event_pendTimeout__I(uintptr_t arg);
extern unsigned int Event_checkEvents(Event_Object *event, unsigned int andMask, unsigned int orMask);


static inline Event_Handle Event_handle(Event_Struct *str)
{
    return ((Event_Handle)str);
}

static inline Event_Struct * Event_struct(Event_Handle h)
{
    return ((Event_Struct *)h);
}

#define Event_module ((Event_Module_State *) &(Event_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_knl_Event__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Event_long_names
#include "Event_defs.h"
/*! @endcond */
