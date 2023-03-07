/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== GateMutexPri.xdc ========
 *
 */
package ti.sysbios.gates;

import xdc.rov.ViewInfo;
import xdc.runtime.Assert;
import ti.sysbios.knl.Queue;
import ti.sysbios.knl.Task;

/*!
 *  ======== GateMutexPri ========
 *  Mutex Gate with priority inheritance.
 *
 *  GateMutexPri is a mutex gate (it can only be held by one thread at a time)
 *  which implements priority inheritance in order to prevent priority
 *  inversion. Priority inversion occurs when a high priority task has its
 *  priority effectively 'inverted' because it is waiting on a gate held by a
 *  low priority task.
 *
 *  When multiple tasks wait on this gate, they will receive the gate in order
 *  of priority (higher priority tasks will receive the gate first). This is
 *  because the queue of tasks waiting on a GateMutexPri is sorted by priority,
 *  not FIFO.
 *
 *  @p(html)
 *  <h3> Problem: Priority Inversion </h3>
 *  The following example demonstrates priority inversion.
 *  A system has three tasks, Low, Med, and High, each with the priority
 *  suggested by its name. Task Low runs first and acquires the gate. Task High
 *  is scheduled and preempts Low. Task High tries to acquire the gate, and
 *  waits on it. Next, task Med is scheduled and preempts task Low. Now task
 *  High must wait for both task Med and task Low to finish before it can
 *  continue. In this situation, task Low has in effect lowered task High's
 *  priority to that of Low.
 *
 *  <h3> Solution: Priority Inheritance </h3>
 *  To guard against priority inversion, GateMutexPri implements priority
 *  inheritance: when task High tries to acquire a gate that is owned by task
 *  Low, task Low's priority will be temporarily raised to that of High, as
 *  long as High is waiting on the gate. Task High will "donate" its priority
 *  to task Low.
 *
 *  When multiple tasks wait on the gate, the gate owner will receive the
 *  highest priority of any of the tasks waiting on the gate.
 *
 *  <h3> Caveats </h3>
 *  Priority inheritance is not a complete guard against priority inversion.
 *  Tasks only donate priority on the call to gate, so if a task has its
 *  priority raised while waiting on a gate, that priority will not carry
 *  through to the gate owner.
 *  This can occur in situations involving multiple gates. A system has four
 *  tasks: VeryLow, Low, Med, and High, each with the priority suggested by its
 *  name. Task VeryLow runs first and acquires gate A. Task Low runs next and
 *  acquires gate B, then waits on gate A. Task High runs and waits on gate B.
 *  Task High has donated its priority to task Low, but Low is blocked on
 *  VeryLow, so priority inversion occurs despite the use of the gate.
 *  The solution to this problem is to design around it. If gate A may be
 *  needed by a high-priority, time-critical task, then it should be a design
 *  rule that no task holds this gate for a long time, or blocks while holding
 *  this gate.
 *
 *  <h3> Miscellaneous </h3>
 *  Calls to enter() may block, so this gate can only be used in the task
 *  context.
 *
 *  GateMutexPri is non-deterministic on calls to gate because it keeps the
 *  queue of waiting tasks sorted by priority.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *  </colgroup>
 *
 *    <tr><th> Function </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th>
 *  <th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                          -->
 *    <tr><td> {@link #Params_init} </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #query}       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}    </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #enter}       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   N    </td></tr>
 *    <tr><td> {@link #leave}       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. GateMutexPri_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. GateMutexPri_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Must be used in enter/leave pairs. </li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@InstanceFinalize       /* To deconstruct the Queue */

module GateMutexPri inherits xdc.runtime.IGateProvider
{

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String     status;
        String     label;
        UInt       mutexCnt;
        String     owner;
        Int        ownerOrigPri;
        Int        ownerCurrPri;
    }

    /*!
     *  ======== DetailedView ========
     *  @_nodoc
     */
    metaonly struct DetailedView {
        String     status;
        String     label;
        UInt       mutexCnt;
        String     owner;
        Int        ownerOrigPri;
        Int        ownerCurrPri;
        String     pendedTasks[];
    }

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Basic', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitBasic', structName: 'BasicView'}],
                ['Detailed', {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitDetailed', structName: 'DetailedView'}],
            ]
        });

    /*!
     *  Assert when GateMutexPri_enter() is not called from correct context.
     *  GateMutexPri_enter() can only be called from main() or Task context (not
     *  Hwi or Swi).
     *
     *  Common causes and workarounds for hitting this Assert:
     *
     *  - Calling printf() from a Hwi or Swi thread.
     *  @p(blist)
     *          - Use xdc.runtime.System_printf (with SysMin) instead.
     *  @p
     *  - Calling System_printf() from a Hwi or Swi thread when using SysStd.
     *  @p(blist)
     *          - Use xdc.runtime.SysMin instead of xdc.runtume.SysStd.
     *          - Use a different type of Gate for
     *            {@link ti.sysbios.BIOS#rtsGateType BIOS.rtsGateType}
     *            (ie {@link ti.sysbios.BIOS#GateHwi BIOS.GateHwi}
     *            or {@link ti.sysbios.BIOS#GateSwi BIOS.GateSwi})
     *  @p
     *  - Calling Memory_alloc() from a Hwi or Swi thread.
     *  @p(blist)
     *          - Use a different Heap manager
     *  @p
     */
    config Assert.Id A_badContext = {
        msg: "A_badContext: bad calling context. See GateMutexPri API doc for details."
    };

    /*!
     *  ======== A_enterTaskDisabled ========
     *  Asserted in GateMutexPri_enter()
     *
     *  Assert raised if GateMutexPri_enter() is called with the Task or
     *  Swi scheduler disabled.
     */
    config Assert.Id A_enterTaskDisabled = {
        msg: "A_enterTaskDisabled: Cannot call GateMutexPri_enter() while the Task or Swi scheduler is disabled."
    };

instance:

    override IArg enter();

    override Void leave(IArg key);

internal:

    Void insertPri(ti.sysbios.knl.Queue.Object *queue,
                   ti.sysbios.knl.Queue.Elem *newElem,
                   Int newPri);

    /* instance object */
    struct Instance_State {
        volatile UInt           mutexCnt;       /* The mutex, 0 or 1 */
        volatile Int            ownerOrigPri;   /* Owner's original pri */
        volatile Task.Handle    owner;          /* Gate owner */
        Queue.Object            pendQ;          /* Q of pending tasks */
    };
}
