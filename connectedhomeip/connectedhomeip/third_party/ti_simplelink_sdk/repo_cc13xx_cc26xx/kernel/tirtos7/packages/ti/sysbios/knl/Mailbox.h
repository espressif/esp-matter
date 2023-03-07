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
 * @file ti/sysbios/knl/Mailbox.h
 * @brief Mailbox Manager
 *
 * The Mailbox module makes available a set of functions that manipulate mailbox
 * objects accessed through handles of type Mailbox_Handle.
 *
 * @link Mailbox_pend @endlink is used to wait for a message from a mailbox.
 * The timeout parameter to Mailbox_pend allows the task to wait until a timeout
 * specified in terms of system clock ticks. A timeout value of @link
 * BIOS_WAIT_FOREVER @endlink causes the task to wait
 * indefinitely for a message. A timeout value of @link BIOS_NO_WAIT @endlink
 * causes Mailbox_pend to return immediately. Mailbox_pend's
 * return value indicates whether the mailbox was signaled successfully.
 *
 * When a Mailbox has been configured with a readerEvent
 * Event object and a task has returned from @link Event_pend @endlink with
 * the corresponding readerEventId, then BIOS_NO_WAIT
 * should be passed to Mailbox_pend() to retrieve the message.
 *
 * NOTE: Since only a single reader can pend on a readerEvent
 * Event object, a Mailbox configured with a readerEvent
 * Event object does not support multiple readers.
 *
 * @link Mailbox_post @endlink is used to send a message to a mailbox. The
 * timeout parameter to Mailbox_post specifies the amount of time the calling
 * task waits if the mailbox is full.
 *
 * When a Mailbox has been configured with a writerEvent
 * Event object and a task has returned from @link Event_pend @endlink with
 * the corresponding writerEventId, then BIOS_NO_WAIT
 * should be passed to Mailbox_post() knowing that the message will be
 * successfully posted.
 *
 * To use the Mailbox module,
 * the following must be added to the app.syscfg file:
 *
 * @code 
 * const Mailbox = scripting.addModule("/ti/sysbios/knl/Mailbox");
 * @endcode 
 *
 * NOTE: Since only a single writer can pend on a writerEvent
 * Event object, a Mailbox configured with a writerEvent
 * Event object does not support multiple writers.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function        </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                        -->
 *    <tr><td> Mailbox_Params_init       </td><td>   Y    </td><td>   Y   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Mailbox_construct         </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_create            </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_delete            </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_destruct          </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_getNumFreeMsgs    </td><td>   Y    </td><td>   Y   </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_getNumPendingMsgs </td><td>   Y    </td><td>   Y   </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_pend              </td><td>   N*   </td><td>   N*  </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td> Mailbox_post              </td><td>   N*   </td><td>   N*  </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: (N* means OK to call iff the timeout
 *           parameter is set to '0'.)<br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. after Mailbox_init() has been called). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. before Mailbox_init() has been called). </li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

#ifndef ti_sysbios_knl_Mailbox__include
#define ti_sysbios_knl_Mailbox__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>

#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
#define ti_sysbios_knl_Mailbox_long_names
#include "Mailbox_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Assert raised when Mailbox_Params.bufSize is too small to handle
 *  (size of messages + sizeof(MbxElem)) * number of messages.
 *  See Mailbox_Params.buf for more information on the buf
 *  parameter.
 */
#define Mailbox_A_invalidBufSize "Mailbox_create's bufSize parameter is invalid (too small)"

/*! @cond NODOC */
/*!
 * @brief The header used to save each Mailbox message
 *
 * Mailbox messages are stored in a queue that requires a header in front of
 * each message.  This structure defines that header and its size must be
 * factored into the total data size requirements for a mailbox instance.
 */
struct Mailbox_MbxElem {
    Queue_Elem elem;
};

/*!
 * @brief The header used to save each Mailbox message
 *
 * Mailbox messages are stored in a queue that requires a header in front of
 * each message.  This structure defines that header and its size must be
 * factored into the total data size requirements for a mailbox instance.
 */
typedef struct Mailbox_MbxElem Mailbox_MbxElem;

#define Mailbox_maxTypeAlign (sizeof (long long))
/*! @endcond */

struct Mailbox_Params {
    /*!
     * @brief The IHeap instance used for dynamic creates
     *
     * This heap is used only for dynamic instances is ignored  for static
     * instances.
     */
    IHeap_Handle heap;
    /*!
     * @brief Mailbox not empty event if using Events. Default is null
     *
     * Posted whenever a mailbox is written to. Reader task pends on this event.
     * Note that the configuration parameter Semaphore.supportsEvents
     * has to be set to true for Mailbox to support Events.
     */
    Event_Handle readerEvent;
    /*!
     * @brief Mailbox not empty event Id if using Events. Default is 1
     *
     * Posted whenever a mailbox is written to. Reader task pends on this
     * eventId. Note that the configuration parameter Semaphore.supportsEvents
     * has to be set to true for Mailbox to support Events.
     */
    unsigned int readerEventId;
    /*!
     * @brief Mailbox not full event if using Events. Default is null
     *
     * Posted whenever a mailbox is read from. Writer task pends on this event.
     * Note that the configuration parameter Semaphore.supportsEvents
     * has to be set to true for Mailbox to support * Events.
     */
    Event_Handle writerEvent;
    /*!
     * @brief Mailbox not full event Id if using Events
     *
     * Posted whenever a mailbox is read from. Writer task pends on this
     * eventId. Note that the configuration parameter Semaphore.supportsEvents
     * has to be set to true for Mailbox to support Events.
     */
    unsigned int writerEventId;
    /*!
     * @brief The address of the buffer used for creating messages
     *
     * This property is only used for dynamically created Mailboxes. If set to
     * 'null', the messages will be allocated from the heap during runtime,
     * otherwise the user may set this to a buffer of their creation to be used
     * for allocating the messages.
     *
     * The module will split the buf into "numMsgs"
     * number of blocks (one block per Mailbox message).
     *
     * Please note that if the buffer is user supplied, then it is the user's
     * responsibility to ensure that it is aligned properly and is also large
     * enough to contain "numMsgs" number of
     * blocks.  The size of each block is defined as follows:
     *
     * @code
     *      sizeof(Mailbox_MbxElem) + msgSize
     * @endcode
     *
     * Since the buffer must be a aligned properly, it may be necessary to
     * 'round up' the total size of the buffer to the next multiple of the
     * alignment for odd sized messages.
     */
    void * buf;
    /*!
     * @brief The size of the buffer that 'buf' references
     *
     * This property is only used for dynamically created Mailboxes.
     */
    unsigned int bufSize;
};
typedef struct Mailbox_Params Mailbox_Params;

/* Struct */
struct Mailbox_Struct {
/*! @cond NODOC */
    Queue_Elem objElem;
    /*!
     * @brief The IHeap instance used for dynamic creates
     *
     * This heap is used only for dynamic instances is ignored  for static
     * instances.
     */
    IHeap_Handle heap;
    size_t msgSize;
    unsigned int numMsgs;
    /*!
     * @brief The address of the buffer used for creating messages
     *
     * This property is only used for dynamically created Mailboxes. If set to
     * 'null', the messages will be allocated from the heap during runtime,
     * otherwise the user may set this to a buffer of their creation to be used
     * for allocating the messages.
     *
     * The module will split the buf into @link ti.sysbios.knl.Mailbox#numMsgs
     * @endlink number of blocks (one block per Mailbox message).
     *
     * Please note that if the buffer is user supplied, then it is the user's
     * responsibility to ensure that it is aligned properly and is also large
     * enough to contain @link ti.sysbios.knl.Mailbox#numMsgs @endlink number of
     * blocks.  The size of each block is defined as follows:
     *
     * @code
     *      sizeof(Mailbox_MbxElem) + msgSize
     * @endcode
     *
     * Since the buffer must be a aligned properly, it may be necessary to
     * 'round up' the total size of the buffer to the next multiple of the
     * alignment for odd sized messages.
     *
     * Also note that if @ref ti.sysbios.BIOS#runtimeCreatesEnabled
     * "BIOS.runtimeCreatesEnabled" is set to false, then the user is required
     * to provide this buffer when constructing the Mailbox object. If 'buf' is
     * not set, then Mailbox_construct() will fail.
     *
     * @see #MbxElem
     */
    void * buf;
    unsigned int numFreeMsgs;
    void * allocBuf;
    Queue_Struct dataQue;
    Queue_Struct freeQue;
    Semaphore_Struct dataSem;
    Semaphore_Struct freeSem;
/*! @endcond */
};
typedef struct Mailbox_Struct Mailbox_Object;
typedef struct Mailbox_Struct Mailbox_Struct;
typedef struct Mailbox_Struct *Mailbox_Handle;

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} Mailbox_Module_State;
/*! @endcond */

/*!
 * @brief Create a mailbox
 *
 * Mailbox_create creates a mailbox object which is initialized to contain
 * numMsgs messages of size msgSize.
 *
 * @param msgSize size of message
 * @param numMsgs length of mailbox
 * @param prms mailbox parameters
 * @param eb error block
 *
 * @retval Mailbox handle (NULL on failure)
 */
extern Mailbox_Handle Mailbox_create(size_t msgSize, unsigned int numMsgs, const Mailbox_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a task
 *
 * Mailbox_construct is equivalent to Mailbox_create except that the
 * Mailbox_Struct is pre-allocated.  See Mailbox_create() for a description
 * of this API.
 *
 * @param obj pointer to a Mailbox object
 * @param msgSize size of message
 * @param numMsgs length of mailbox
 * @param prms mailbox parameters
 * @param eb error block
 *
 * @retval Mailbox handle (NULL on failure)
 */
extern Mailbox_Handle Mailbox_construct(Mailbox_Struct *obj, size_t msgSize, unsigned int numMsgs, const Mailbox_Params *prms, Error_Block *eb);

/*!
 * @brief Delete a mailbox
 *
 * Mailbox_delete deletes a Mailbox object. Note that Mailbox_delete takes
 * a pointer to a Mailbox_Handle which enables Mailbox_delete to set the
 * Mailbox_handle to NULL.
 *
 * @param mailbox pointer to Task handle
 */
extern void Mailbox_delete(Mailbox_Handle *mailbox);

/*!
 * @brief Destruct a mailbox
 *
 * Mailbox_destruct destructs a Mailbox object.
 *
 * @param obj pointer to Mailbox object
 */
extern void Mailbox_destruct(Mailbox_Struct *obj);

/*!
 * @brief Get the message size
 *
 * @param mailbox Mailbox handle
 *
 * @retval mailbox message size
 */
extern size_t Mailbox_getMsgSize(Mailbox_Handle mailbox);

/*!
 * @brief Get the number messages available for use
 *
 * @param mailbox Mailbox handle
 *
 * @retval mailbox message size
 */
extern int Mailbox_getNumFreeMsgs(Mailbox_Handle mailbox);

/*!
 * @brief Get the number of messages that are ready to be read
 *
 * @param mailbox Mailbox handle
 *
 * @retval number of pending messages
 */
extern int Mailbox_getNumPendingMsgs(Mailbox_Handle mailbox);

/*!
 * @brief Wait for a message from mailbox
 *
 * If the mailbox is not empty, Mailbox_pend copies the first message into msg
 * and returns true. Otherwise, Mailbox_pend suspends the execution of the
 * current task until Mailbox_post is called or the timeout expires.
 *
 * A timeout value of @link BIOS_WAIT_FOREVER @endlink
 * causes the task to wait indefinitely  for a message.
 *
 * A timeout value of @link BIOS_NO_WAIT @endlink causes
 * Mailbox_pend to return immediately.
 *
 * The timeout value of @link BIOS_NO_WAIT @endlink should be
 * passed to Mailbox_pend to retrieve a message after Event_pend() is called
 * outside of Mailbox_pend to wait on an incoming message.
 *
 * Mailbox_pend's return value indicates whether the mailbox was signaled
 * successfully.
 *
 * <h3>Event Object Note</h3>
 * If the Mailbox object has been configured with an embedded readerEvent Event
 * object, then prior to returnig from this function, the Event object's state
 * is updated to reflect whether messages are available in the Mailbox after the
 * current message is removed. If there are no more messages available, then the
 * readerEventId is cleared in the Event object. If more messages are available,
 * then the readerEventId is set in the Event object.
 *
 * @warning
 * Be careful with the 'msg' parameter!  The size of the buffer that 'msg'
 * points to must match the 'msgSize' that was specified when the mailbox was
 * created.  This function does a blind copy of the message from the mailbox to
 * the destination pointer, so the destination buffer must be big enough to
 * handle this copy.
 *
 * @param mailbox Mailbox handle
 * @param msg message pointer
 * @param timeout maximum duration in system clock ticks
 *
 * @retval true if successful, false if timeout
 */
extern bool Mailbox_pend(Mailbox_Handle mailbox, void * msg, uint32_t timeout);

/*!
 * @brief Post a message to mailbox
 *
 * Mailbox_post checks to see if there are any free message slots before copying
 * msg into the mailbox. Mailbox_post readies the first task (if any) waiting on
 * the mailbox. If the mailbox is full and a timeout is specified  the task
 * remains suspended until Mailbox_pend is called or the timeout expires.
 *
 * A timeout value of @link BIOS_WAIT_FOREVER @endlink
 * causes the task to wait indefinitely for a free slot.
 *
 * A timeout value of @link BIOS_NO_WAIT @endlink causes
 * Mailbox_post to return immediately.
 *
 * The timeout value of @link BIOS_NO_WAIT @endlink should be
 * passed to Mailbox_post() to post a message after Event_pend() is called
 * outside of Mailbox_post to wait on an available message buffer.
 *
 * Mailbox_post's return value indicates whether the msg was copied or not.
 *
 * <h3>Event Object Note</h3>
 * If the Mailbox object has been configured with an embedded writerEvent Event
 * object, then prior to returnig from this function, the Event object's state
 * is updated to reflect whether more messages can be posted to the Mailbox
 * after the current message has been posted. If no more room is available, then
 * the writerEventId is cleared in the Event object. If more room is available,
 * then the writerEventId is set in the Event object.
 *
 * @note
 * The operation of adding a message to the mailbox and signalling the task (if
 * any) waiting on the mailbox is not atomic. This can result in a priority
 * inversion with respect to message delivery. This can for example affect the
 * order of message delivery for 2 tasks with different priorities. The lower
 * priority task's message may be delivered first while the higher priority
 * task's message may not unblock the task waiting on the mailbox until the
 * lower priority task resumes and completes its Mailbox_post() call.
 *
 * @param mailbox Mailbox handle
 * @param msg message pointer
 * @param timeout maximum duration in system clock ticks
 *
 * @retval true if successful, false if timeout
 */
extern bool Mailbox_post(Mailbox_Handle mailbox, void * msg, uint32_t timeout);

/*!
 * @brief Initialize the Mailbox_Params structure with default values.
 *
 * Mailbox_Params_init initializes the Mailbox_Params structure with default
 * values. Mailbox_Params_init should always be called before setting individual
 * parameter fields. This allows new fields to be added in the future with
 * compatible defaults -- existing source code does not need to change when
 * new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Mailbox_Params_init(Mailbox_Params *prms);

/*!
 * @brief return handle of the first Mailbox on Mailbox list
 *
 * Return the handle of the first Mailbox on the create/construct list. NULL if
 * no Mailbox instances have been created or constructed.
 *
 * @retval Mailbox handle
 */
extern Mailbox_Handle Mailbox_Object_first(void);

/*!
 * @brief return handle of the next Mailbox on Mailbox list
 *
 * Return the handle of the next mailbox on the create/construct list. NULL if
 * no more Mailboxs are on the list.
 *
 * @param mbx Mailbox handle
 *
 * @retval Mailbox handle
 */
extern Mailbox_Handle Mailbox_Object_next(Mailbox_Handle mbx);

/*! @cond NODOC */
static inline Mailbox_Handle Mailbox_handle(Mailbox_Struct *str)
{
    return ((Mailbox_Handle)str);
}

static inline Mailbox_Struct * Mailbox_struct(Mailbox_Handle h)
{
    return ((Mailbox_Struct *)h);
}

#define Mailbox_module ((Mailbox_Module_State *) &(Mailbox_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_knl_Mailbox__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Mailbox_long_names
#include "Mailbox_defs.h"
/*! @endcond */
