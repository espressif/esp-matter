/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
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
 *  ======== Mailbox.xdc ========
 *
 */
package ti.sysbios.knl;

import xdc.rov.ViewInfo;

import xdc.runtime.IHeap;

/*!
 *  ======== Mailbox ========
 *  Mailbox Manager
 *
 *  The Mailbox module makes available a set of functions that manipulate
 *  mailbox objects accessed through handles of type Mailbox_Handle.
 *
 *  {@link #pend()} is used to wait for a message from a mailbox. 
 *  The timeout parameter to Mailbox_pend allows the task to wait until a 
 *  timeout specified in terms of system clock ticks. 
 *  A timeout value of {@link ti.sysbios.BIOS#WAIT_FOREVER BIOS_WAIT_FOREVER}
 *  causes the task to wait indefinitely  for a message. 
 *  A timeout value of {@link ti.sysbios.BIOS#NO_WAIT BIOS_NO_WAIT} 
 *  causes Mailbox_pend to return immediately. 
 *  Mailbox_pend's return value indicates whether the mailbox was signaled 
 *  successfully.
 *
 *  When a Mailbox has been configured with a {@link #readerEvent} Event
 *  object and a task has returned from {@link Event#pend()} with the 
 *  corresponding {@link #readerEventId}, then BIOS_NO_WAIT
 *  should be passed to Mailbox_pend() to retrieve the message.
 *
 *  NOTE: Since only a single reader can pend on a {@link #readerEvent} 
 *  Event object,
 *  a Mailbox configured with a {@link #readerEvent} Event object does not
 *  support multiple readers. 
 *
 *  {@link #post()} is used to send a message to a mailbox. 
 *  The timeout parameter to Mailbox_post specifies the amount of time the 
 *  calling task waits if the mailbox is full. 
 *
 *  When a Mailbox has been configured with a {@link #writerEvent} Event
 *  object and a task has returned from {@link Event#pend()} with the 
 *  corresponding {@link #writerEventId}, then BIOS_NO_WAIT
 *  should be passed to Mailbox_post() knowing that the message
 *  will be successfully posted.
 *
 *  NOTE: Since only a single writer can pend on a {@link #writerEvent} 
 *  Event object,
 *  a Mailbox configured with a {@link #writerEvent} Event object does not
 *  support multiple writers.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function        </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                        -->
 *    <tr><td> {@link #Params_init}       </td><td>   Y    </td><td>   Y   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}         </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}            </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}            </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}          </td><td>   N    </td><td>   N   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getNumFreeMsgs}    </td><td>   Y    </td><td>   Y   </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getNumPendingMsgs} </td><td>   Y    </td><td>   Y   </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #pend}              </td><td>   N*   </td><td>   N*  </td>
 *    <td>   Y    </td><td>   N*   </td><td>   N    </td></tr>
 *    <tr><td> {@link #post}              </td><td>   N*   </td><td>   N*  </td>
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
 *    (e.g. Mailbox_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started 
 *    (e.g. Mailbox_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p 
 */

@DirectCall
@ModuleStartup     /* Instances require more initialization at startup */
@InstanceFinalize
@InstanceInitError
@InstanceInitStatic     /* Construct/Destruct CAN becalled at runtime */

module Mailbox 
{
    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String label;
        SizeT  msgSize;
        UInt   numMsgs;
    }
    
    /*!
     *  ======== DetailedView ========
     *  @_nodoc
     */
    metaonly struct DetailedView {
        String label;
        SizeT  msgSize;
        UInt   numMsgs;
        UInt   curNumMsgs;
        UInt   freeSlots;
        String pendQueue[];
        String postQueue[];
    }

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
            [
                'Basic',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitBasic',
                    structName: 'BasicView'
                }
            ],
            [
                'Detailed',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitDetailed',
                    structName: 'DetailedView'
                }
            ]
            ]
        });

    /*!
     *  ======== MbxElem ========
     *  The header used to save each Mailbox message
     *
     *  Mailbox messages are stored in a queue that requires a header in
     *  front of each message.  This structure defines that header and its
     *  size must be factored into the total data size requirements for a
     *  mailbox instance.
     */
    struct MbxElem {
        Queue.Elem elem;
    };

    /*!
     *  ======== A_invalidBufSize ========
     *  Assert raised when the bufSize parameter is too small
     *
     *  This assert is raised when bufSize is too small to handle
     *  (size of messages + sizeof(MbxElem)) * number of messages.
     *  See {@link ti.sysbios.knl.MailBox#buf} for more information on the buf
     *  parameter.
     */
    config xdc.runtime.Assert.Id A_invalidBufSize =
        {msg: "Mailbox_create's bufSize parameter is invalid (too small)"};

instance:

    /*!
     *  ======== create ========
     *  Create a mailbox
     *
     *  Mailbox_create creates a mailbox object which is initialized to contain
     *  numMsgs messages of size msgSize.
     *
     *  @param(msgSize)         size of message
     *  @param(numMsgs)         length of mailbox
     */
    create(SizeT msgSize, UInt numMsgs);

    /*!
     *  ======== heap ========
     *  The IHeap instance used for dynamic creates
     *
     *  This heap is used only for dynamic instances is ignored  for static 
     *  instances.
     */
    config xdc.runtime.IHeap.Handle heap = null;

    /*!
     *  ======== sectionName ========
     *  Section name for the buffer managed by the instance
     *
     *  The default section is the 'dataSection' in the platform.
     */
    metaonly config String sectionName = null;
    
    /*!
     *  ======== readerEvent ========
     *  Mailbox not empty event if using Events. Default is null
     *
     *  Posted whenever a mailbox is written to.
     *  Reader task pends on this event. 
     *  Note that {@link ti.sysbios.knl.Semaphore#supportsEvents 
     *  Semaphore.supportsEvents} has to be 
     *  set to true for Mailbox to support Events.
     */
    config Event.Handle readerEvent = null;

    /*!
     *  ======== readerEventId ========
     *  Mailbox not empty event Id if using Events. Default is 1
     *
     *  Posted whenever a mailbox is written to.
     *  Reader task pends on this eventId.
     *  Note that {@link ti.sysbios.knl.Semaphore#supportsEvents
     *  Semaphore.supportsEvents} has to be 
     *  set to true for Mailbox to support Events.
     */
    config UInt readerEventId = 1;

    /*!
     *  ======== writerEvent ========
     *  Mailbox not full event if using Events. Default is null
     *
     *  Posted whenever a mailbox is read from.
     *  Writer task pends on this event.
     *  Note that {@link ti.sysbios.knl.Semaphore#supportsEvents
     *  Semaphore.supportsEvents} has to be 
     *  set to true for Mailbox to support Events.
     */
    config Event.Handle writerEvent = null;

    /*!
     *  ======== writerEventId ========
     *  Mailbox not full event Id if using Events
     *
     *  Posted whenever a mailbox is read from.
     *  Writer task pends on this eventId.
     *  Note that {@link ti.sysbios.knl.Semaphore#supportsEvents 
     *  Semaphore.supportsEvents} has to be 
     *  set to true for Mailbox to support Events.
     */
    config UInt writerEventId = 1;

    /*!
     *  ======== buf ========
     *  The address of the buffer used for creating messages
     *
     *  This property is only used for dynamically created Mailboxes.
     *  If set to 'null', the messages will be allocated from the heap
     *  during runtime, otherwise the user may set this to a buffer of their
     *  creation to be used for allocating the messages.
     *  
     *  The module will split the buf into
     *  {@link ti.sysbios.knl.Mailbox#numMsgs} number of blocks (one block per
     *  Mailbox message).
     *
     *  Please note that if the buffer is user supplied, then it is the user's
     *  responsibility to ensure that it is aligned properly and is also large
     *  enough to contain {@link ti.sysbios.knl.Mailbox#numMsgs} number of
     *  blocks.  The size of each block is defined as follows:
     *  @p(code)
     *      sizeof(Mailbox_MbxElem) + msgSize
     *  @p
     *
     *  Since the buffer must be a aligned properly, it may be necessary to
     *  'round up' the total size of the buffer to the next multiple of the
     *  alignment for odd sized messages.
     *
     *  Also note that if {@link ti.sysbios.BIOS#runtimeCreatesEnabled
     *  BIOS.runtimeCreatesEnabled} is set to false, then the user is required
     *  to provide this buffer when constructing the Mailbox object. If 'buf'
     *  is not set, then Mailbox_construct() will fail.
     *
     *  @see #MbxElem
     */
    config Ptr buf = null;

    /*!
     *  ======== bufSize ========
     *  The size of the buffer that 'buf' references
     *
     *  This property is only used for dynamically created Mailboxes.
     */
    config UInt bufSize = 0;

    /*!
     *  ======== getMsgSize ========
     *  Get the message size
     */
    SizeT getMsgSize();

    /*!
     *  ======== getNumFreeMsgs ========
     *  Get the number messages available for use
     */
    Int getNumFreeMsgs();

    /*!
     *  ======== getNumPendingMsgs ========
     *  Get the number of messages that are ready to be read
     */
    Int getNumPendingMsgs();

    /*!
     *  ======== pend ========
     *  Wait for a message from mailbox
     *
     *  If the mailbox is not empty, Mailbox_pend copies the first message into
     *  msg and returns TRUE. Otherwise, Mailbox_pend suspends the execution of
     *  the current task until Mailbox_post is called or the timeout expires. 
     *
     *  A timeout value of 
     *  {@link ti.sysbios.BIOS#WAIT_FOREVER, BIOS_WAIT_FOREVER} causes 
     *  the task to wait indefinitely  for a message. 
     *
     *  A timeout value of {@link ti.sysbios.BIOS#NO_WAIT, BIOS_NO_WAIT} 
     *  causes Mailbox_pend to return immediately. 
     *
     *  The timeout value of {@link ti.sysbios.BIOS#NO_WAIT, BIOS_NO_WAIT} 
     *  should be passed to Mailbox_pend() to retrieve a message after
     *  Event_pend() is called outside of Mailbox_pend to wait on an incoming
     *  message.
     *
     *  Mailbox_pend's return value indicates whether the mailbox was signaled
     *  successfully.
     *
     *  @a(Event Object Note)
     *  If the Mailbox object has been configured with an embedded readerEvent
     *  Event object, then prior to returnig from this function, the Event
     *  object's state is updated to reflect whether messages are available
     *  in the Mailbox after the current message is removed.
     *  If there are no more messages available, then the readerEventId is
     *  cleared in the Event object. If more messages are available,
     *  then the readerEventId is set in the Event object.
     *
     *  @param(msg)     message pointer
     *  @param(timeout) maximum duration in system clock ticks
     *  @b(returns)     TRUE if successful, FALSE if timeout
     *
     *  @a(WARNING)
     *  Be careful with the 'msg' parameter!  The size of the buffer that 'msg'
     *  points to must match the 'msgSize' that was specified
     *  when the mailbox was created.  This function does a blind copy of the
     *  message from the mailbox to the destination pointer, so the destination
     *  buffer must be big enough to handle this copy.
     */
    Bool pend(Ptr msg, UInt32 timeout);

    /*!
     *  ======== post ========
     *  Post a message to mailbox
     *
     *  Mailbox_post checks to see if there are any free message slots before
     *  copying msg into the mailbox. Mailbox_post readies the first task 
     *  (if any) waiting on the mailbox. If the mailbox is full and a timeout 
     *  is specified  the task remains suspended until Mailbox_pend is called 
     *  or the timeout expires.
     *
     *  A timeout value of 
     *  {@link ti.sysbios.BIOS#WAIT_FOREVER, BIOS_WAIT_FOREVER} causes 
     *  the task to wait indefinitely for a free slot.
     *
     *  A timeout value of {@link ti.sysbios.BIOS#NO_WAIT, BIOS_NO_WAIT} 
     *  causes  Mailbox_post to return immediately.
     *
     *  The timeout value of {@link ti.sysbios.BIOS#NO_WAIT, BIOS_NO_WAIT} 
     *  should be passed to Mailbox_post() to post a message after
     *  Event_pend() is called outside of Mailbox_post to wait on an 
     *  available message buffer.
     *
     *  Mailbox_post's return value indicates whether the msg was 
     *  copied or not.
     *
     *  @a(Event Object Note)
     *  If the Mailbox object has been configured with an embedded writerEvent
     *  Event object, then prior to returnig from this function, the Event
     *  object's state is updated to reflect whether more messages can be
     *  posted to the Mailbox after the current message has been posted.
     *  If no more room is available, then the writerEventId is
     *  cleared in the Event object. If more room is available,
     *  then the writerEventId is set in the Event object.
     *
     *  @param(msg)     message pointer
     *  @param(timeout) maximum duration in system clock ticks
     *  @b(returns)     TRUE if successful, FALSE if timeout
     *
     *  @a(NOTE)
     *  The operation of adding a message to the mailbox and signalling
     *  the task (if any) waiting on the mailbox is not atomic. This can
     *  result in a priority inversion with respect to message delivery.
     *  This can for example affect the order of message delivery for 2
     *  tasks with different priorities. The lower priority task's message
     *  may be delivered first while the higher priority task's message
     *  may not unblock the task waiting on the mailbox until the lower
     *  priority task resumes and completes its Mailbox_post() call.
     */
    Bool post(Ptr msg, UInt32 timeout);

internal:

    Void cleanQue(Queue.Handle obj);
    
    /*
     *  ======== postInit ========
     *  finish initializing static and dynamic instances
     */
    Int postInit(Object *obj, SizeT blockSize);
    
    config UInt maxTypeAlign;

    /* -------- Internal Structures -------- */
    struct Instance_State {
        xdc.runtime.IHeap.Handle    heap;
        SizeT                       msgSize;
        UInt                        numMsgs;
        Ptr                         buf;
        Queue.Object                dataQue;
        Queue.Object                freeQue;
        Semaphore.Object            dataSem;
        Semaphore.Object            freeSem;
        UInt                        numFreeMsgs;
        Char                        allocBuf[];
    };
}
