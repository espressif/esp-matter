/*
 * Copyright (c) 2015-2019, Texas Instruments Incorporated
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
 *  ======== Task.xdc ========
 */

package ti.sysbios.knl;

import xdc.rov.ViewInfo;

import xdc.runtime.Error;
import xdc.runtime.Assert;
import xdc.runtime.Diags;
import xdc.runtime.Log;
import xdc.runtime.IHeap;

import ti.sysbios.knl.Queue;

/*!
 *  ======== Task ========
 *  Task Manager.
 *
 *  The Task module makes available a set of functions that manipulate task
 *  objects accessed through pointers of type {@link #Handle}. Tasks represent
 *  independent threads of control that conceptually execute functions in
 *  parallel within a single C program; in reality, concurrency is achieved
 *  by switching the processor from one task to another.
 *
 *  All tasks executing within a single program share a common set of
 *  global variables, accessed according to the standard rules of scope
 *  defined for C functions.
 *
 *  Each task is in one of five modes of execution at any point in time:
 *  running, ready, blocked, terminated, or inactive. By design, there is
 *  always one
 *  (and only one) task currently running, even if it is only the idle task
 *  managed internally by Task. The current task can be suspended from
 *  execution by calling certain Task functions, as well as functions
 *  provided by other modules like the Semaphore or Event Modules.
 *  The current task
 *  can also terminate its own execution. In either case, the processor
 *  is switched to the highest priority task that is ready to run.
 *
 *  You can assign numeric priorities to tasks. Tasks are
 *  readied for execution in strict priority order; tasks of the same
 *  priority are scheduled on a first-come, first-served basis.
 *  The priority of the currently running task is never lower
 *  than the priority of any ready task. Conversely, the running task
 *  is preempted and re-scheduled for execution whenever there exists
 *  some ready task of higher priority.
 *
 *  @a(Task Stacks)
 *
 *  When you create a task, it is provided with its own run-time stack,
 *  used for storing local variables as well as for further nesting of
 *  function calls. Each stack must be large enough to handle normal
 *  subroutine calls and one task preemption context.
 *  A task preemption context is the context that gets saved when one task
 *  preempts another as a result of an interrupt thread readying
 *  a higher-priority task.
 *
 *  See sections 3.5.3 and 7.5 of the BIOS User's Guide for further
 *  discussions regarding task stack sizing.
 *
 *  Certain system configuration settings will result in
 *  task stacks needing to be large enough to absorb two interrupt
 *  contexts rather than just one.
 *  Setting {@link ti.sysbios.BIOS#logsEnabled BIOS.logsEnabled} to 'true'
 *  or installing any Task hooks will have the side effect of allowing
 *  up to two interrupt contexts to be placed on a task stack. Also
 *  see {@link #minimizeLatency Task.minimizeLatency}.
 *
 *  @a(Task Deletion)
 *
 *  Any dynamically created task that is not in the Task_Mode_RUNNING
 *  state (ie not the currently running task) can be deleted using the
 *  {@link #delete} API.
 *
 *  Task_delete() removes the task from all internal queues and calls
 *  Memory_free() is used to free the task object and its stack.
 *  Memory_free() must acquire a lock to the memory before proceeding.
 *  If another task already holds a lock to the memory, then the thread
 *  performing the delete will be blocked until the memory is unlocked.
 *
 *  Note:
 *  Task_delete() should be called with extreme care.
 *  As mentioned above, the scope of Task_delete() is limited to
 *  freeing the Task object itself, freeing the task's stack memory
 *  if it was allocated at create time, and removing the task from
 *  any SYS/BIOS-internal state structures.
 *
 *  SYS/BIOS does not keep track of any resources the task may have
 *  acquired or used during its lifetime.
 *
 *  It is the application's responsibility to guarantee the integrity
 *  of a task's partnerships prior to deleting that task.
 *
 *  For example, if a task has obtained exclusive access to a resource,
 *  deleting that task will make the resource forever unavailable.
 *
 *  Task_delete() sets the referenced task handle to NULL. Any subsequent
 *  call to a Task instance API using that null task handle will behave
 *  unpredictably and will usually result in an application crash.
 *
 *  Assuming a task completely cleans up after itself prior to calling
 *  Task_exit() (or falling through the the bottom of the task
 *  function), it is then safest to use Task_delete() only when a task
 *  is in the 'Task_Mode_TERMINATED' state.
 *
 *  Delete hooks:
 *  You can specify application-wide Delete hook functions that
 *  run whenever a task is deleted. See the discussion of Hook Functions
 *  below for details.
 *
 *  Task_delete() constraints:
 *  @p(blist)
 *  -The task cannot be the currently executing task (Task_self()).
 *  -Task_delete cannot be called from a Swi or Hwi.
 *  -No check is performed to prevent Task_delete from being used on a
 *  statically-created object. If a program attempts to delete a task object
 *  that was created statically, the Memory_free() call will result in an
 *  assertion failure in its corresponding Heap manager, causing the
 *  application to exit.
 *  @p
 *
 *  @a(Stack Alignment)
 *
 *  Stack size parameters for both static and dynamic tasks are rounded
 *  up to the nearest integer multiple of a target-specific alignment
 *  requirement.
 *
 *  In the case of Task's which are created with a user-provided stack,
 *  both the base address and the stackSize are aligned. The base address
 *  is increased to the nearest aligned address. The stack size is decreased
 *  accordingly and then rounded down to the nearest integer multiple of the
 *  target-specific required alignment.
 *
 *  @p(html)
 *  <a name="hookfunc"></a>
 *  @p
 *
 *  @a(Hook Functions)
 *
 *  Sets of hook functions can be specified for the Task module.  Each
 *  set can contain these hook functions:
 *  @p(blist)
 *  -Register: A function called before any statically created tasks
 *      are initialized at runtime.  The register hook is called at boot time
 *      before main() and before interrupts are enabled.
 *  -Create: A function that is called when a task is created.
 *      This includes tasks that are created statically and those
 *      created dynamically using {@link #create} or {@link #construct}.
 *      For statically created tasks, create hook is called before main()
 *      and before interrupts are enabled. For dynamically created or
 *      constructed tasks, create hook is called in the same context the
 *      task is created or constructed in i.e. if a task is created in
 *      main(), the create hook is called in main context and if the task
 *      is created within another task, it is called in task context. The
 *      create hook is called outside of a Task_disable/enable block and
 *      before the task has been added to the ready list.
 *  -Ready: A function that is called when a task becomes ready to run.
 *      The ready hook is called in the context of the thread unblocking
 *      a task and therefore it can be called in Hwi, Swi or Task context.
 *      If a Swi or Hwi posts a semaphore that unblocks a task, the ready
 *      hook would be called in the Swi or Hwi's context. The ready hook is
 *      called from within a Task_disable/enable block with interrupts enabled.
 *  -Switch: A function that is called just before a task switch
 *      occurs. The 'prev' and 'next' task handles are passed to the switch
 *      hook. 'prev' is set to NULL for the initial task switch that occurs
 *      during SYS/BIOS startup.  The switch hook is called from within a
 *      Task_disable/enable block with interrupts enabled, in the
 *      context of the task being switched from (ie: the `prev` task).
 *  -Exit: A function that is called when a task exits using {@link #exit}.
 *      It is called in the exiting task's context. The exit hook is passed
 *      the handle of the exiting task. The exit hook is called outside of a
 *      Task_disable/enable block and before the task has been removed from
 *      the kernel lists.
 *  -Delete: A function that is called when any task is deleted at
 *      run-time with {@link #delete}. The delete hook is called in idle task
 *      context if {@link #deleteTerminatedTasks} is set to true. Otherwise,
 *      it is called in the context of the task that is deleting another task.
 *      The delete hook is called outside of a Task_disable/enable block.
 *  @p
 *  Hook functions can only be configured statically.
 *
 *  If you define more than one set of hook functions, all the functions
 *  of a particular type will be run when a Task triggers that type of
 *  hook.
 *
 *  @a(Warning)
 *  Configuring ANY Task hook function will have the side effect of allowing
 *  up to two interrupt contexts beings saved on a task stack. Be careful
 *  to size your task stacks accordingly.
 *
 *  @p(html)
 *  <B>Register Function</B>
 *  @p
 *
 *  The Register function is provided to allow a hook set to store its
 *  hookset ID.  This id can be passed to {@link #setHookContext} and
 *  {@link #getHookContext} to set or get hookset-specific context.  The
 *  Register function must be specified if the hook implementation
 *  needs to use {@link #setHookContext} or {@link #getHookContext}.
 *  The registerFxn hook function is called during system initialization
 *  before interrupts have been enabled.
 *
 *  @p(code)
 *  Void myRegisterFxn(Int id);
 *  @p
 *
 *  @p(html)
 *  <B>Create and Delete Functions</B>
 *  @p
 *
 *  The create and delete functions are called whenever a Task is created
 *  or deleted.  They are called with interrupts enabled (unless called
 *  at boot time or from main()).
 *
 *  @p(code)
 *  Void myCreateFxn(Task_Handle task, Error_Block *eb);
 *  @p
 *
 *  @p(code)
 *  Void myDeleteFxn(Task_Handle task);
 *  @p
 *
 *  @p(html)
 *  <B>Switch Function</B>
 *  @p
 *
 *  If a switch function is specified, it is invoked just before the new task
 *  is switched to.  The switch function is called with interrupts enabled.
 *
 *  This function can be used to save/restore additional task context (for
 *  example, external hardware registers), to check for task stack overflow,
 *  to monitor the time used by each task, etc.
 *
 *  @p(code)
 *  Void mySwitchFxn(Task_Handle prev, Task_Handle next);
 *  @p
 *
 *  To properly handle the switch to the first task your switchFxn should
 *  check for "prev == NULL" before using prev:
 *
 *  @p(code)
 *  Void mySwitchFxn(Task_Handle prev, Task_Handle next)
 *  {
 *      if (prev != NULL) {
 *          ...
 *      }
 *      ...
 *  }
 *  @p
 *
 *  @p(html)
 *  <B>Ready Function</B>
 *  @p
 *
 *  If a ready function is specified, it is invoked whenever a task is made
 *  ready to run.   The ready function is called  with interrupts enabled
 *  (unless called at boot time or from main()).
 *
 *  @p(code)
 *  Void myReadyFxn(Task_Handle task);
 *  @p
 *
 *  @p(html)
 *  <B>Exit Function</B>
 *  @p
 *
 *  If an exit function is specified, it is invoked when a task exits (via
 *  call to Task_exit() or when a task returns from its' main function).
 *  The Exit Function is called with interrupts enabled.
 *
 *  @p(code)
 *  Void myExitFxn(Task_Handle task);
 *  @p
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *  </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *  <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                       -->
 *    <tr><td> {@link #create}          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #disable}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #exit}            </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getIdleTask}     </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}     </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #restore}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #self}            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #sleep}           </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #yield}           </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #construct}       </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}        </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getEnv}          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getHookContext}  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getMode}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getPri}          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getFunc}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setEnv}          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setHookContext}  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setPri}          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #stat}            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *  (e.g. Task_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *  (e.g. Task_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
/* REQ_TAG(SYSBIOS-464) */
@ModuleStartup      /* generate a call to Task_Module_startup at startup */
@InstanceInitStatic /* Construct/Destruct CAN becalled at runtime */
@InstanceFinalize   /* generate call to Task_Instance_finalize on delete */
@InstanceInitError  /* instance init can fail */
@Template ("./Task.xdt") /* generate function to create a SMP specific
                            module state structure and initialize it */

module Task
{

    // -------- Module Constants --------

    // -------- Module Types --------

    /*! Task function type definition. */
    typedef Void (*FuncPtr)(UArg, UArg);

    /*! "All Task Blocked" function type definition. */
    typedef Void (*AllBlockedFuncPtr)(Void);

    /*! Check value computation function type definition. */
    typedef UInt32 (*ModStateCheckValueFuncPtr)(Task.Module_State *);

    /*! Data Integrity Check function type definition */
    typedef Int (*ModStateCheckFuncPtr)(Task.Module_State *, UInt32);

    /*! Check value computation function type definition. */
    typedef UInt32 (*ObjectCheckValueFuncPtr)(Task.Handle);

    /*! Task object data integrity check function type definition */
    typedef Int (*ObjectCheckFuncPtr)(Task.Handle, UInt32);

    /*!
     *  Task execution modes.
     *
     *  These enumerations are the range of modes or states that
     *  a task can be in. A task's current mode can be gotten using
     *  {@link #stat}.
     */
    enum Mode {
        Mode_RUNNING,           /*! Task is currently executing. */
        Mode_READY,             /*! Task is scheduled for execution. */
        Mode_BLOCKED,           /*! Task is suspended from execution. */
        Mode_TERMINATED,        /*! Task is terminated from execution. */
        Mode_INACTIVE           /*! Task is on inactive task list */
    };

    /*!
     *  Task Status Buffer.
     *
     *  Passed to and filled in by {@link #stat};
     */
    struct Stat {
        Int     priority;       /*! Task priority. */
        Ptr     stack;          /*! Task stack. */
        SizeT   stackSize;      /*! Task stack size. */
        IHeap.Handle stackHeap; /*! Heap used to alloc stack. */
        Ptr     env;            /*! Global environment struct. */
        Mode    mode;           /*! Task's current mode. */
        Ptr     sp;             /*! Task's current stack pointer. */
        SizeT   used;           /*! Maximum number of bytes used on stack. */
    };

    /*!
     *  Task hook set type definition.
     *
     *  Sets of hook functions can be specified for the Task module.
     *  See {@link #hookfunc Hook Functions} for details.
     */
    /* REQ_TAG(SYSBIOS-454) */
    struct HookSet {
        Void (*registerFxn)(Int);
        Void (*createFxn)(Handle, Error.Block *);
        Void (*readyFxn)(Handle);
        Void (*switchFxn)(Handle, Handle);
        Void (*exitFxn)(Handle);
        Void (*deleteFxn)(Handle);
    };

    /*! "Don't care" task affinity */
    const UInt AFFINITY_NONE = ~(0);

    /*!  @_nodoc */
    metaonly struct BasicView {
        String      label;
        Int         priority;
        String      mode;
        String      fxn[];
        UArg        arg0;
        UArg        arg1;
        SizeT       stackSize;
        Ptr         stackBase;
        String      curCoreId;
        String      affinity;
    }

    /*!  @_nodoc */
    metaonly struct DetailedView {
        String      label;
        Int         priority;
        String      mode;
        String      fxn[];
        UArg        arg0;
        UArg        arg1;
        String      stackPeak;
        SizeT       stackSize;
        Ptr         stackBase;
        String      curCoreId;
        String      affinity;
        String      blockedOn;
    }

    /*!  @_nodoc */
    metaonly struct ModuleView {
        String      schedulerState;
        String      readyQMask[];
        Bool        workPending;
        UInt        numVitalTasks;
        Ptr         currentTask[];
        String      hwiStackPeak;
        SizeT       hwiStackSize;
        Ptr         hwiStackBase;
    }

    /*!  @_nodoc (not used by view) */
    metaonly struct CallStackView {
        Int         depth;
        String      decode;
    }

    /*!  @_nodoc */
    metaonly struct ReadyQView {
        Ptr         task;
        Ptr         next;
        Ptr         prev;
        Ptr         readyQ;
        String      label;
        Int         priority;
        String      mode;
        String      fxn[];
        String      curCoreId;
        String      affinity;
    }

    /*! @_nodoc */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
                ['Basic',    {type: ViewInfo.INSTANCE,     viewInitFxn: 'viewInitBasic',    structName: 'BasicView'}],
                ['Detailed', {type: ViewInfo.INSTANCE,     viewInitFxn: 'viewInitDetailed', structName: 'DetailedView'}],
                ['CallStacks',  {type: ViewInfo.TREE,         viewInitFxn: 'viewInitCallStack', structName: 'CallStackView'}],
                ['ReadyQs',     {type: ViewInfo.TREE_TABLE,   viewInitFxn: 'viewInitReadyQs',   structName: 'ReadyQView'}],
                ['Module',      {type: ViewInfo.MODULE,       viewInitFxn: 'viewInitModule',    structName: 'ModuleView'}],
            ]
        });

    // -------- Module Parameters --------

    // Logs

    /*! Logged on every task switch */
    config Log.Event LM_switch = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_switch: oldtsk: 0x%x, oldfunc: 0x%x, newtsk: 0x%x, newfunc: 0x%x"
    };

    /*! Logged on calls to Task_sleep */
    config Log.Event LM_sleep = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_sleep: tsk: 0x%x, func: 0x%x, timeout: %d"
    };

    /*! Logged when a task is made ready to run (ie Semaphore_post()) */
    config Log.Event LD_ready = {
        mask: Diags.USER2,
        msg: "LD_ready: tsk: 0x%x, func: 0x%x, pri: %d"
    };

    /*! Logged when a task is blocked (ie Semaphore_pend()) */
    config Log.Event LD_block = {
        mask: Diags.USER2,
        msg: "LD_block: tsk: 0x%x, func: 0x%x"
    };

    /*! Logged on calls to Task_yield */
    config Log.Event LM_yield = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_yield: tsk: 0x%x, func: 0x%x, currThread: %d"
    };

    /*! Logged on calls to Task_setPri */
    config Log.Event LM_setPri = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_setPri: tsk: 0x%x, func: 0x%x, oldPri: %d, newPri %d"
    };

    /*!
     *  Logged when Task functions fall thru the bottom
     *  or when Task_exit() is explicitly called.
     */
    config Log.Event LD_exit = {
        mask: Diags.USER2,
        msg: "LD_exit: tsk: 0x%x, func: 0x%x"
    };

    /*! Logged on calls to Task_setAffinity */
    config Log.Event LM_setAffinity = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_setAffinity: tsk: 0x%x, func: 0x%x, oldCore: %d, oldAffinity %d, newAffinity %d"
    };

    /*! Logged on every task schedule entry */
    config Log.Event LM_schedule = {
        mask: Diags.USER3,
        msg: "LD_schedule: coreId: %d, workFlag: %d, curSetLocal: %d, curSetX: %d, curMaskLocal: %d"
    };

    /*! Logged when no scheduling work was found */
    config Log.Event LM_noWork = {
        mask: Diags.USER3,
        msg: "LD_noWork: coreId: %d, curSetLocal: %d, curSetX: %d, curMaskLocal: %d"
    };

    // Errors

    /*!
     *  Error raised when a stack overflow (or corruption) is detected.
     *
     *  This error is raised by kernel's stack checking function.  This
     *  function checks the stacks before every task switch to make sure
     *  that reserved word at top of stack has not been modified.
     *
     *  The stack checking logic is enabled by the {@link #initStackFlag} and
     *  {@link #checkStackFlag} configuration parameters.  If both of these
     *  flags are set to true, the kernel will validate the stacks.
     */
    config Error.Id E_stackOverflow  = {
        msg: "E_stackOverflow: Task 0x%x stack overflow."
    };

    /*!
     *  Error raised when a task's stack pointer (SP) does not point
     *  somewhere within the task's stack.
     *
     *  This error is raised by kernel's stack checking function.  This
     *  function checks the SPs before every task switch to make sure
     *  they point within the task's stack.
     *
     *  The stack checking logic is enabled by the {@link #initStackFlag} and
     *  {@link #checkStackFlag} configuration parameters.  If both of these
     *  flags are set to true, the kernel will validate the stack pointers.
     */
    config Error.Id E_spOutOfBounds  = {
        msg: "E_spOutOfBounds: Task 0x%x stack error, SP = 0x%x."
    };

    config Error.Id E_deleteNotAllowed = {
        msg: "E_deleteNotAllowed: Task 0x%x."
    };

    /*!
     *  Error raised when the check value of the Task module state does not
     *  match the stored check value (computed during startup). This indicates
     *  that the Task module state was corrupted.
     */
    config Error.Id E_moduleStateCheckFailed = {
        msg: "E_moduleStateCheckFailed: Task module state data integrity check failed."
    };

    /*!
     *  Error raised when the check value of the Task object does not match
     *  the stored check value (computed during startup). This indicates
     *  that the Task object was corrupted.
     */
    config Error.Id E_objectCheckFailed = {
        msg: "E_objectCheckFailed: Task 0x%x object data integrity check failed."
    };

    // Asserts

    /*! Asserted in Task_create and Task_delete */
    config Assert.Id A_badThreadType = {
        msg: "A_badThreadType: Cannot create/delete a task from Hwi or Swi thread."
    };

    /*! Asserted in Task_delete */
    config Assert.Id A_badTaskState = {
        msg: "A_badTaskState: Can't delete a task in RUNNING state."
    };

    /*! Asserted in Task_delete */
    config Assert.Id A_noPendElem = {
        msg: "A_noPendElem: Not enough info to delete BLOCKED task."
    };

    /*! Asserted in Task_create */
    config Assert.Id A_taskDisabled = {
        msg: "A_taskDisabled: Cannot create a task when tasking is disabled."
    };

    /*! Asserted in Task_create */
    config Assert.Id A_badPriority = {
        msg: "A_badPriority: An invalid task priority was used."
    };

    /*! Asserted in Task_sleep */
    config Assert.Id A_badTimeout = {
        msg: "A_badTimeout: Can't sleep FOREVER."
    };

    /*! Asserted in Task_setAffinity */
    config Assert.Id A_badAffinity = {
        msg: "A_badAffinity: Invalid affinity."
    };

    /*! Asserted in Task_sleep */
    config Assert.Id A_sleepTaskDisabled = {
        msg: "A_sleepTaskDisabled: Cannot call Task_sleep() while the Task scheduler is disabled."
    };

    /*! Asserted in Task_getIdleTaskHandle */
    config Assert.Id A_invalidCoreId = {
        msg: "A_invalidCoreId: Cannot pass a non-zero CoreId in a non-SMP application."
    };

    /*!
     *  Number of Task priorities supported. Default is 16.
     *
     *  The maximum number of priorities supported is
     *  target specific and depends on the number of
     *  bits in a UInt data type. For 6x and ARM devices
     *  the maximum number of priorities is therefore 32.
     *  For C28x devices, the maximum number of
     *  priorities is 16.
     */
    config UInt numPriorities = 16;

    /*!
     *  Default stack size (in MAUs) used for all tasks.
     *
     *  Default is obtained from the family-specific TaskSupport module
      *  (e.g. {@link ti.sysbios.family.arm.m3.TaskSupport},
      *  {@link ti.sysbios.family.c62.TaskSupport}).
     */
    config SizeT defaultStackSize;

    /*!
     *  Default memory section used for all statically created task stacks.
     *
     *  The default stack section name is target/device specific.
     *  For C6x targets it is ".far:taskStackSection".
     *  For C28x targets it is ".taskStackSection".
     *  For GNU targets it is ".bss".
     *  For all other targets it is ".bss:taskStackSection".
     *
     *  By default, all statically created task stacks are grouped together
     *  into the defaultStackSection and placed where ever
     *  the target specific defaultStackSection base section name
     *  (ie .bss, .far, .ebss) is placed.
     *
     *  To place all task stacks into a different memory segment,
     *  add the following to your config script:
     *
     *  @p(code)
     *  Program.sectMap[Task.defaultStackSection] = new Program.SectionSpec();
     *  Program.sectMap[Task.defaultStackSection].loadSegment =
     *                   "yourMemorySegment";
     *  @p
     *
     *  To group all task stacks into a different section AND place that
     *  section into a specific memory segment, add the following to your
     *  config script:
     *
     *  @p(code)
     *  Task.defaultStackSection = ".yourSectionName";
     *  Program.sectMap[Task.defaultStackSection] = new Program.SectionSpec();
     *  Program.sectMap[Task.defaultStackSection].loadSegment =
     *                   "yourMemorySegment";
     *  @p
     *
     *  Where "yourSectionName" can be just about anything, and
     *                   "yourMemorySegment"
     *  must be a memory segment defined for your board.
     */
    metaonly config String defaultStackSection;

    /*!
     *  Default Mem heap used for all dynamically created task stacks.
     *
     *  Default is null.
     */
    config IHeap.Handle defaultStackHeap;

    /*!
     *  Default core affinity for newly created tasks.
     *
     *  Default is Task_AFFINITY_NONE, meaning don't care.
     */
    metaonly config UInt defaultAffinity = AFFINITY_NONE;

    /*!
     *  Create a task (of priority 0) to run the Idle functions in.
     *
     *  When set to true, a task is created that continuously calls the
     *  {@link Idle#run Idle_run()} function, which, in turn calls each of
     *  the configured Idle functions.
     *
     *  When set to false, no Idle Task is created and it is up to the
     *  user to call the Idle_run() function if the configured Idle
     *  functions need to be run. Or, by adding the following lines to
     *  the config script, the Idle functions will run whenever all
     *  tasks are blocked ({@link #allBlockedFunc Task.allBlockedFunc}):
     *
     *  @p(code)
     *  Task.enableIdleTask = false;
     *  Task.allBlockedFunc = Idle.run;
     *  @p
     *
     *  Default is true.
     *
     *  @see #idleTaskStackSize
     *  @see #idleTaskStackSection
     *  @see #idleTaskVitalTaskFlag
     *  @see #allBlockedFunc
     */
    metaonly config Bool enableIdleTask = true;

    /*!
     *  Reduce interrupt latency by enabling interrupts
     *  within the Task scheduler.
     *
     *  By default, interrupts are disabled within certain critical
     *  sections of the task scheduler when switching to a different
     *  task thread. This default behavior guarantees that a task stack
     *  will only ever absorb ONE ISR context. Nested interrupts all run
     *  on the shared Hwi stack.
     *
     *  While most users find this behavior desirable, the resulting
     *  impact on interrupt latency is too great for certain applications.
     *
     *  By setting this parameter to 'true', the worst case interrupt latency
     *  imposed by the kernel will be reduced but will result in task stacks
     *  needing to be sized to accommodate one additional interrupt context.
     *
     *  See sections 3.5.3 and 7.5 of the BIOS User's Guide for further
     *  discussions regarding task stack sizing.
     *
     *  Also see {@link ti.sysbios.BIOS#logsEnabled BIOS.logsEnabled}
     *  and the discussion on Task hooks.
     */
    metaonly config Bool minimizeLatency = false;

    /*!
     *  Idle task stack size in MAUs.
     *
     *  Default is inherited from module config defaultStackSize.
     */
    metaonly config SizeT idleTaskStackSize;

    /*!
     *  Idle task stack section
     *
     *  Default is inherited from module config defaultStackSection;
     */
    metaonly config String idleTaskStackSection;

    /*!
     *  Idle task's vitalTaskFlag.
     *  (see {@link #vitalTaskFlag}).
     *
     *  Default is true.
     */
    metaonly config Bool idleTaskVitalTaskFlag = true;

    /*!
     *  Function to call while all tasks are blocked.
     *
     *  This function will be called repeatedly while no tasks are
     *  ready to run.
     *
     *  Ordinarily (in applications that have tasks ready to run at startup),
     *  the function will run in the context of the last task to block.
     *
     *  In an application where there are no tasks ready to run
     *  when BIOS_start() is called, the allBlockedFunc function is
     *  called within the BIOS_start() thread which runs on the system/ISR
     *  stack.
     *
     *  By default, allBlockedFunc is initialized to point to an internal
     *  function that simply returns.
     *
     *  By adding the following lines to the config script, the Idle
     *  functions will run whenever all tasks are blocked:
     *
     *  @p(code)
     *  Task.enableIdleTask = false;
     *  Task.allBlockedFunc = Idle.run;
     *  @p
     *
     *  @see #enableIdleTask
     *
     *  @a(constraints)
     *  The configured allBlockedFunc is designed to be called repeatedly.
     *  It must return in order for the task scheduler to check if all
     *  tasks are STILL blocked and if not, run the highest priority task
     *  currently ready to run.
     *
     *  The configured allBlockedFunc function is called with interrupts
     *  disabled. If your function must run with interrupts enabled,
     *  surround the body of your code with  Hwi_enable()/Hwi_restore()
     *  function calls per the following example:
     *
     *  @p(code)
     *  Void yourFunc() {
     *      UInt hwiKey;
     *
     *      hwiKey = Hwi_enable();
     *
     *      ...         // your code here
     *
     *      Hwi_restore(hwiKey);
     *  }
     *  @p
     */
    config AllBlockedFuncPtr allBlockedFunc = null;

    /*!
     *  Initialize stack with known value for stack checking at runtime
     *  (see {@link #checkStackFlag}).
     *  If this flag is set to false, while the
     *  {@link ti.sysbios.hal.Hwi#checkStackFlag} is set to true, only the
     *  first byte of the stack is initialized.
     *
     *  This is also useful for inspection of stack in debugger or core
     *  dump utilities.
     *  Default is true.
     */
    config Bool initStackFlag = true;

    /*!
     *  Check 'from' and 'to' task stacks before task context switch.
     *
     *  The check consists of testing the top of stack value against
     *  its initial value (see {@link #initStackFlag}). If it is no
     *  longer at this value, the assumption is that the task has
     *  overrun its stack. If the test fails, then the
     *  {@link #E_stackOverflow} error is raised.
     *
     *  Default is true.
     *
     *  To enable or disable full stack checking, you should set both this
     *  flag and the {@link ti.sysbios.hal.Hwi#checkStackFlag}.
     *
     *  @a(Note)
     *  Enabling stack checking will add some interrupt latency because the
     *  checks are made within the Task scheduler while interrupts are
     *  disabled.
     */
    config Bool checkStackFlag = true;

    /*!
     *  Automatically delete terminated tasks.
     *
     *  If this feature is enabled, an Idle function is installed that
     *  deletes dynamically created Tasks that have terminated either
     *  by falling through their task function or by explicitly calling
     *  Task_exit().
     *
     *  A list of terminated Tasks that were created dynmically is
     *  maintained internally. Each invocation of the installed Idle function
     *  deletes the first Task on this list. This one-at-a-time process
     *  continues until the list is empty.
     *
     *  @a(Note)
     *  This feature is disabled by default.
     *
     *  @a(WARNING)
     *  When this feature is enabled, an error will be raised if the user's
     *  application attempts to delete a terminated task. If a terminated task
     *  has already been automatically deleted and THEN the user's application
     *  attempts to delete it (ie: using a stale Task handle), the results are
     *  undefined and probably catastrophic!
     *
     */
    config Bool deleteTerminatedTasks = false;

    /*!
     *  Const array that holds the HookSet objects.
     *
     *  See {@link #hookfunc Hook Functions} for details about HookSets.
     */
    config HookSet hooks[length] = [];

    /*!
     *  ======== moduleStateCheckFxn ========
     *  Function called to perform module state data integrity check
     *
     *  If {@link #moduleStateCheckFlag} is set to true, SYS/BIOS kernel
     *  will call this function each time Task_disable() function is called.
     *  SYS/BIOS provides a default implementation of this function that
     *  computes the check value for the static module state fields and
     *  compares the resulting check value against the stored value. In
     *  addition, the check function validates some of the pointers used
     *  by the Task scheduler. The application can install its own
     *  implementation of the module state check function.
     *
     *  Here's an example module state check function:
     *
     *  *.cfg
     *  @p(code)
     *  var Task = xdc.useModule('ti.sysbios.knl.Task');
     *
     *  // Enable Task module state data integrity check
     *  Task.moduleStateCheckFlag = true;
     *
     *  // Install custom module state check function
     *  Task.moduleStateCheckFxn = "&myCheckFunc";
     *  @p
     *
     *  *.c
     *  @p(code)
     *  #define ti_sysbios_knl_Task__internalaccess
     *  #include <ti/sysbios/knl/Task.h>
     *
     *  Int myCheckFunc(Task_Module_State *moduleState, UInt32 checkValue)
     *  {
     *      UInt32 newCheckValue;
     *
     *      newCheckValue = Task_moduleStateCheckValueFxn(moduleState);
     *      if (newCheckValue != checkValue) {
     *          // Return '-1' to indicate data corruption. SYS/BIOS kernel
     *          // will raise an error.
     *          return (-1);
     *      }
     *
     *      return (0);
     *  }
     *  @p
     */
    config ModStateCheckFuncPtr moduleStateCheckFxn = Task.moduleStateCheck;

    /*!
     *  ======== moduleStateCheckValueFxn ========
     *  Function called to compute module state check value
     *
     *  If {@link #moduleStateCheckFlag} is set to true, SYS/BIOS kernel
     *  will call this function during startup to compute the Task module
     *  state's check value.
     *
     *  SYS/BIOS provides a default implementation of this function that
     *  computes a 32-bit checksum for the static module state fields (i.e.
     *  module state fields that do not change during the lifetime of the
     *  application). The application can install its own implementation
     *  of this function.
     *
     *  Here's an example module state check value computation function:
     *
     *  *.cfg
     *  @p(code)
     *  var Task = xdc.useModule('ti.sysbios.knl.Task');
     *
     *  // Enable Task module state data integrity check
     *  Task.moduleStateCheckFlag = true;
     *
     *  // Install custom module state check value function
     *  Task.moduleStateCheckValueFxn = "&myCheckValueFunc";
     *  @p
     *
     *  *.c
     *  @p(code)
     *  #define ti_sysbios_knl_Task__internalaccess
     *  #include <ti/sysbios/knl/Task.h>
     *
     *  UInt32 myCheckValueFunc(Task_Module_State *moduleState)
     *  {
     *      UInt64 checksum;
     *
     *      checksum = (uintptr_t)moduleState->readyQ +
     *                 (uintptr_t)moduleState->smpCurSet +
     *                 (uintptr_t)moduleState->smpCurMask +
     *                 (uintptr_t)moduleState->smpCurTask +
     *                 (uintptr_t)moduleState->smpReadyQ +
     *                 (uintptr_t)moduleState->idleTask +
     *                 (uintptr_t)moduleState->constructedTasks;
     *      checksum = (checksum >> 32) + (checksum & 0xFFFFFFFF);
     *      checksum = checksum + (checksum >> 32);
     *
     *      return ((UInt32)(~checksum));
     *  }
     *  @p
     */
    config ModStateCheckValueFuncPtr moduleStateCheckValueFxn =
        Task.getModuleStateCheckValue;

    /*!
     *  ======== moduleStateCheckFlag ========
     *  Perform a runtime data integrity check on the Task module state
     *
     *  This configuration parameter determines whether a data integrity
     *  check is performed on the Task module state in order to detect
     *  data corruption.
     *
     *  If this field is set to true, a check value of the static fields in
     *  the Task module state (i.e. fields that do not change during the
     *  lifetime of the application) is computed during startup. The
     *  computed check value is stored for use by the Task module state check
     *  function. The application can implement its own check value
     *  computation function (see {@link #moduleStateCheckValueFxn}).
     *  By default, SYS/BIOS installs a check value computation function that
     *  computes a 32-bit checksum of the static fields in the Task module
     *  state.
     *
     *  The module state check function (see {@link #moduleStateCheckFxn})
     *  is called from within the Task_disable() function. The application
     *  can provide its own implementation of this function. By default,
     *  SYS/BIOS installs a check function that computes the check value
     *  for select module state fields and compares the resulting check
     *  value against the stored value.
     *
     *  If the module state check function returns a '-1' (i.e. check failed),
     *  then the SYS/BIOS kernel will raise an error.
     */
    config Bool moduleStateCheckFlag = false;

    /*!
     *  ======== objectCheckFxn ========
     *  Function called to perform Task object data integrity check
     *
     *  If {@link #objectCheckFlag} is set to true, SYS/BIOS kernel
     *  will call this function from within a Task switch hook and
     *  each time a Task blocks or unblocks. SYS/BIOS provides a default
     *  implementation of this function that computes the check value
     *  for the static Task object fields and compares the resulting
     *  check value against the stored value. The application can install
     *  its own implementation of the object check function.
     *
     *  Here's an example Task object check function:
     *
     *  *.cfg
     *  @p(code)
     *  var Task = xdc.useModule('ti.sysbios.knl.Task');
     *
     *  // Enable Task object data integrity check
     *  Task.objectCheckFlag = true;
     *
     *  // Install custom Task object check function
     *  Task.objectCheckFxn = "&myCheckFunc";
     *  @p
     *
     *  *.c
     *  @p(code)
     *  #define ti_sysbios_knl_Task__internalaccess
     *  #include <ti/sysbios/knl/Task.h>
     *
     *  Int myCheckFunc(Task_Handle handle, UInt32 checkValue)
     *  {
     *      UInt32 newCheckValue;
     *
     *      newCheckValue = Task_objectCheckValueFxn(handle);
     *      if (newCheckValue != checkValue) {
     *          // Return '-1' to indicate data corruption. SYS/BIOS kernel
     *          // will raise an error.
     *          return (-1);
     *      }
     *
     *      return (0);
     *  }
     *  @p
     */
    config ObjectCheckFuncPtr objectCheckFxn = Task.objectCheck;

    /*!
     *  ======== objectCheckValueFxn ========
     *  Function called to compute Task object check value
     *
     *  If {@link #objectCheckFlag} is set to true, SYS/BIOS kernel
     *  will call this function to compute the Task object's check value
     *  each time a Task is created.
     *
     *  SYS/BIOS provides a default implementation of this function that
     *  computes a 32-bit checksum for the static Task object fields (i.e.
     *  Task object fields that do not change during the lifetime of the
     *  Task). The application can install its own implementation of this
     *  function.
     *
     *  Here's an example Task object check value computation function:
     *
     *  *.cfg
     *  @p(code)
     *  var Task = xdc.useModule('ti.sysbios.knl.Task');
     *
     *  // Enable Task object data integrity check
     *  Task.objectCheckFlag = true;
     *
     *  // Install custom Task object check value function
     *  Task.objectCheckValueFxn = "&myCheckValueFunc";
     *  @p
     *
     *  *.c
     *  @p(code)
     *  #define ti_sysbios_knl_Task__internalaccess
     *  #include <ti/sysbios/knl/Task.h>
     *
     *  UInt32 myCheckValueFunc(Task_Handle taskHandle)
     *  {
     *      UInt64 checksum;
     *
     *      checksum = taskHandle->stackSize +
     *                 (uintptr_t)taskHandle->stack +
     *                 (uintptr_t)taskHandle->stackHeap +
     *  #if defined(__IAR_SYSTEMS_ICC__)
     *                 (UInt64)taskHandle->fxn +
     *  #else
     *                 (uintptr_t)taskHandle->fxn +
     *  #endif
     *                 taskHandle->arg0 +
     *                 taskHandle->arg1 +
     *                 (uintptr_t)taskHandle->hookEnv +
     *                 taskHandle->vitalTaskFlag;
     *      checksum = (checksum >> 32) + (checksum & 0xFFFFFFFF);
     *      checksum = checksum + (checksum >> 32);
     *
     *      return ((UInt32)(~checksum));
     *  }
     *  @p
     */
    config ObjectCheckValueFuncPtr objectCheckValueFxn =
        Task.getObjectCheckValue;

    /*!
     *  ======== objectCheckFlag ========
     *  Perform a runtime data integrity check on each Task object
     *
     *  This configuration parameter determines whether a data integrity
     *  check is performed on each Task object in the system in order to detect
     *  data corruption.
     *
     *  If this field is set to true, a check value of the static fields in
     *  the Task object (i.e. fields that do not change during the lifetime
     *  of the Task) is computed when the Task is created. The computed check
     *  value is stored for use by the Task object check function. The
     *  application can implement its own check value computation function
     *  (see {@link #objectCheckValueFxn}). By default, SYS/BIOS installs a
     *  check value computation function that computes a 32-bit checksum of
     *  the static fields in the Task object.
     *
     *  The Task object check function (see {@link #objectCheckFxn})
     *  is called from within a Task switch hook if stack checking
     *  (see {@link #checkStackFlag}) is enabled. It is also called when
     *  a task blocks or unblocks. The application can provide its own
     *  implementation of this function. By default, SYS/BIOS installs a
     *  check function that computes the check value for select Task
     *  object fields and compares the resulting check value against the
     *  stored value.
     *
     *  If the Task object check function returns a '-1' (i.e. check failed),
     *  then the SYS/BIOS kernel will raise an error.
     */
    config Bool objectCheckFlag = false;

    // -------- Module Functions --------

    /*!
     *  ======== addHookSet ========
     *  addHookSet is used in a config file to add a hook set.
     *
     *  Configures a set of hook functions for the
     *  Task module. Each set contains these hook functions:
     *
     *  @p(blist)
     *  -Register: A function called before any statically created tasks
     *  are initialized at runtime.  The register hook is called at boot time
     *  before main() and before interrupts are enabled.
     *  -Create: A function that is called when a task is created.
     *  This includes tasks that are created statically and those
     *  created dynamically using {@link #create} or {@link #construct}.
     *  The create hook is called outside of a Task_disable/enable block and
     *   before the task has been added to the ready list.
     *  -Ready: A function that is called when a task becomes ready to run.
     *   The ready hook is called from within a Task_disable/enable block with
     *   interrupts enabled.
     *  -Switch: A function that is called just before a task switch
     *  occurs. The 'prev' and 'next' task handles are passed to the Switch
     *  hook. 'prev' is set to NULL for the initial task switch that occurs
     *  during SYS/BIOS startup.  The Switch hook is called from within a
     *  Task_disable/enable block with interrupts enabled.
     *  -Exit:  A function that is called when a task exits using
     *  {@link #exit}.  The exit hook is passed the handle of the exiting
     *  task.  The exit hook is called outside of a Task_disable/enable block
     *  and before the task has been removed from the kernel lists.
     *  -Delete: A function that is called when any task is deleted at
     *  run-time with {@link #delete}.  The delete hook is called outside
     *  of a Task_disable/enable block.
     *  @p
     *  Hook functions can only be configured statically.
     *
     *  See {@link #hookfunc Hook Functions} for more details.
     *
     *  HookSet structure elements may be omitted, in which case those
     *  elements will not exist.
     *
     *  For example, the following configuration code defines a HookSet:
     *
     *  @p(code)
     *  // Hook Set 1
     *  Task.addHookSet({
     *     registerFxn: '&myRegister1',
     *     createFxn:   '&myCreate1',
     *     readyFxn:    '&myReady1',
     *     switchFxn:   '&mySwitch1',
     *     exitFxn:     '&myExit1',
     *     deleteFxn:   '&myDelete1'
     *  });
     *  @p
     *
     *  @param(hook)    structure of type HookSet
     */
    metaonly Void addHookSet(HookSet hook);

    /*!
     *  @_nodoc
     *  ======== Task_startup ========
     *  Start the task scheduler.
     *
     *  Task_startup signals the end of boot operations, enables
     *  the Task scheduler and schedules the highest priority ready
     *  task for execution.
     *
     *  Task_startup is called by BIOS_start() after Hwi_enable()
     *  and Swi_enable(). There is no return from this function as the
     *  execution thread is handed to the highest priority ready task.
     */
    Void startup();

    /*!
     *  ======== Task_enabled ========
     *  Returns TRUE if the Task scheduler is enabled
     *
     *  @_nodoc
     */
    Bool enabled();

    /*!
     *  @_nodoc
     *  ======== unlockSched ========
     *  Force a Task scheduler unlock. Used by Core_atExit() & Core_hwiFunc()
     *  to unlock Task scheduler before exiting.
     *
     *  This function should only be called after a Hwi_disable() has entered
     *  the Inter-core gate and disabled interrupts locally.
     */
    Void unlockSched();

    /*!
     *  ======== Task_disable ========
     *  Disable the task scheduler.
     *
     *  {@link #disable} and {@link #restore} control Task scheduling.
     *  {@link #disable} disables all other Tasks from running until
     *  {@link #restore} is called. Hardware and Software interrupts
     *  can still run.
     *
     *  {@link #disable} and {@link #restore} allow you to ensure that
     *  statements
     *  that must be performed together during critical processing are not
     *  preempted by other Tasks.
     *
     *  The value of the key returned is opaque to applications and is meant
     *  to be passed to Task_restore().
     *
     *  In the following example, the critical section is
     *  not preempted by any Tasks.
     *
     *  @p(code)
     *  key = Task_disable();
     *      `critical section`
     *  Task_restore(key);
     *  @p
     *
     *  You can also use {@link #disable} and {@link #restore} to
     *  create several Tasks and allow them to be invoked in
     *  priority order.
     *
     *  {@link #disable} calls can be nested.
     *
     *  @b(returns)     key for use with {@link #restore}
     *
     *  @a(constraints)
     *  Do not call any function that can cause the current task to block
     *  within a {@link #disable}/{@link #restore} block. For example,
     *  {@link ti.sysbios.knl.Semaphore#pend Semaphore_pend}
     *  (if timeout is non-zero),
     *  {@link #sleep}, {@link #yield}, and Memory_alloc can all
     *  cause blocking.
     */
    UInt disable();

    /*!
     *  @_nodoc
     *  ======== enable ========
     *  Enable the task scheduler.
     *
     *  {@link #enable} unconditionally enables the Task scheduler and
     *  schedules the highest priority ready task for execution.
     *
     *  This function is called by {@link #startup} (which is called by
     *  {@link ti.sysbios.BIOS#start BIOS_start}) to begin multi-tasking
     *  operations.
     */
    Void enable();

    /*!
     *  ======== restore ========
     *  Restore Task scheduling state.
     *
     *  {@link #disable} and {@link #restore} control Task scheduling
     *  {@link #disable} disables all other Tasks from running until
     *  {@link #restore} is called. Hardware and Software interrupts
     *  can still run.
     *
     *  {@link #disable} and {@link #restore} allow you to ensure that
     *  statements
     *  that must be performed together during critical processing are not
     *  preempted.

     *  In the following example, the critical section is not preempted
     *  by any Tasks.
     *
     *  @p(code)
     *  key = Task_disable();
     *      `critical section`
     *  Task_restore(key);
     *  @p
     *
     *  You can also use {@link #disable} and {@link #restore} to create
     *  several Tasks and allow them to be performed in priority order.
     *
     *  {@link #disable} calls can be nested.
     *
     *  {@link #restore} returns with interrupts enabled if the key unlocks
     *  the scheduler
     *
     *  @param(key)     key to restore previous Task scheduler state
     *
     *  @a(constraints)
     *  Do not call any function that can cause the current task to block
     *  within a {@link #disable}/{@link #restore} block. For example,
     *  {@link ti.sysbios.knl.Semaphore#pend Semaphore_pend()}
     *  (if timeout is non-zero),
     *  {@link #sleep}, {@link #yield}, and Memory_alloc can all
     *  cause blocking.
     *
     *  {@link #restore} internally calls Hwi_enable() if the key passed
     *  to it results in the unlocking of the Task scheduler (ie if this
     *  is root Task_disable/Task_restore pair).
     */
    Void restore(UInt key);

    /*!
     *  @_nodoc
     *  ======== restoreHwi ========
     *  Restore Task scheduling state.
     *  Used by dispatcher. Does not re-enable Ints.
     */
    Void restoreHwi(UInt key);

    /*!
     *  ======== self ========
     *  Returns a handle to the currently executing Task object.
     *
     *  Task_self returns the object handle for the currently executing task.
     *  This function is useful when inspecting the object or when the current
     *  task changes its own priority through {@link #setPri}.
     *
     *  No task switch occurs when calling Task_self.
     *
     *  Task_self will return NULL until Tasking is initiated at the end of
     *  BIOS_start().
     *
     *  @b(returns)     address of currently executing task object
     */
    /* REQ_TAG(SYSBIOS-511) */
    Handle self();

    /*!
     *  ======== selfMacro ========
     *  Returns a handle to the currently executing Task object.
     *
     *  Task_selfMacro is identical to {@link #self} but is implemented as
     *  and inline macro.
     *
     *  @b(returns)     address of currently executing task object
     */
    @Macro
    Handle selfMacro();

    /*!
     *  @_nodoc
     *  ======== checkStacks ========
     *  Check for stack overflow.
     *
     *  This function is usually called by the {@link #HookSet} switchFxn to
     *  make sure task stacks are valid before performing the context
     *  switch.
     *
     *  If a stack overflow is detected on either the oldTask or the
     *  newTask, a {@link #E_stackOverflow} Error is raised and the system
     *  exited.
     *
     *  In order to work properly, {@link #checkStacks} requires that the
     *  {@link #initStackFlag} set to true, which it is by default.
     *
     *  You can call {@link #checkStacks} directly from your application.
     *  For example, you can check the current task's stack integrity
     *  at any time with a call like the following:
     *
     *  @p(code)
     *  Task_checkStacks(Task_self(), Task_self());
     *  @p
     *
     *  @param(oldTask)  leaving Task Object Ptr
     *  @param(newTask)  entering Task Object Ptr
     */
    Void checkStacks(Handle oldTask, Handle newTask);

    /*!
     *  ======== exit ========
     *  Terminate execution of the current task.
     *
     *  Task_exit terminates execution of the current task, changing its mode
     *  from {@link #Mode_RUNNING} to {@link #Mode_TERMINATED}. If all tasks
     *  have been terminated, or if all remaining tasks have their
     *  vitalTaskFlag attribute set to FALSE, then SYS/BIOS terminates the
     *  program as a whole by calling the function System_exit with a status
     *  code of 0.
     *
     *  Task_exit is automatically called whenever a task returns from its
     *  top-level function.
     *
     *  Exit Hooks (see exitFxn in {@link #HookSet}) can be used to provide
     *  functions that run whenever a task is terminated. The exitFxn Hooks
     *  are called before the task has been blocked and marked
     *  {@link #Mode_TERMINATED}.
     *  See {@link #hookfunc Hook Functions} for more information.
     *
     *  Any SYS/BIOS function can be called from an Exit Hook function.
     *
     *  Calling {@link #self} within an Exit function returns the task
     *  being exited. Your Exit function declaration should be similar to
     *  the following:
     *  @p(code)
     *  Void myExitFxn(Void);
     *  @p
     *
     *  A task switch occurs when calling Task_exit unless the program as a
     *  whole is terminated
     *
     *  @a(constraints)
     *  Task_exit cannot be called from a Swi or Hwi.
     *
     *  Task_exit cannot be called from the program's main() function.
     */
    Void exit();

    /*!
     *  ======== sleep ========
     *  Delay execution of the current task.
     *
     *  Task_sleep changes the current task's mode from {@link #Mode_RUNNING}
     *  to {@link #Mode_BLOCKED}, and delays its execution for nticks
     *  increments of the {@link Clock system clock}. The actual time
     *  delayed can be up to 1 system clock tick less than nticks due to
     *  granularity in system timekeeping and the time elapsed per
     *  tick is determined by {@link Clock#tickPeriod Clock_tickPeriod}.
     *
     *  After the specified period of time has elapsed, the task reverts to
     *  the {@link #Mode_READY} mode and is scheduled for execution.
     *
     *  A task switch always occurs when calling Task_sleep if nticks > 0.
     *
     *  @param(nticks)  number of system clock ticks to sleep
     *
     *  @a(constraints)
     *  Task_sleep cannot be called from a Swi or Hwi, or within a
     *  {@link #disable} / {@link #restore} block.
     *
     *  Task_sleep cannot be called from the program's main() function.
     *
     *  Task_sleep should not be called from within an Idle function. Doing
     *  so prevents analysis tools from gathering run-time information.
     *
     *  nticks cannot be {@link ti.sysbios.BIOS#WAIT_FOREVER BIOS_WAIT_FOREVER}.
     */
    /* REQ_TAG(SYSBIOS-518) */
    Void sleep(UInt32 nticks);

    /*!
     *  ======== yield ========
     *  Yield processor to equal priority task.
     *
     *  Task_yield yields the processor to another task of equal priority.
     *
     *  A task switch occurs when you call Task_yield if there is an equal
     *  priority task ready to run.
     *
     *  Tasks of higher priority preempt the currently running task without
     *  the need for a call to Task_yield. If only lower-priority tasks are
     *  ready to run when you call Task_yield, the current task continues to
     *  run. Control does not pass to a lower-priority task.
     *
     *  @a(constraints)
     *  When called within an Hwi, the code sequence calling Task_yield
     *  must be invoked by the Hwi dispatcher.
     *
     *  Task_yield cannot be called from the program's main() function.
     */
    Void yield();

    /*!
     *  ======== getIdleTask ========
     *  returns a handle to the idle task object (for core 0)
     */
    Handle getIdleTask();

    /*!
     *  ======== getIdleTaskHandle ========
     *  returns a handle to the idle task object for the specified coreId
     *  (should be used only in applications built with
     *  {@link ti.sysbios.BIOS#smpEnabled} set to true)
     *
     *  @a(Note)
     *  If this function is called in a non-SMP application, coreId should
     *  always be 0.
     */
    Handle getIdleTaskHandle(UInt coreId);

    /*!
     *  @_nodoc
     *  ======== startCore ========
     *  begin tasking on a core
     */
    Void startCore(UInt coreId);

    /*!
     *  ======== getNickName ========
     *
     */
    metaonly String getNickName(Any tskView);

instance:

    /*!
     *  ======== create ========
     *  Create a Task.
     *
     *  Task_create creates a new task object. If successful, Task_create
     *  returns the handle of the new task object. If unsuccessful,
     *  Task_create returns NULL unless it aborts.
     *
     *  The fxn parameter uses the {@link #FuncPtr} type to pass a pointer to
     *  the function the Task object should run. For example, if myFxn is a
     *  function in your program, your C code can create a Task object
     *  to call that
     *  function as follows:
     *
     *  @p(code)
     *  Task_Params taskParams;
     *
     *  // Create task with priority 15
     *  Task_Params_init(&taskParams);
     *  taskParams.stackSize = 512;
     *  taskParams.priority = 15;
     *  Task_create((Task_FuncPtr)myFxn, &taskParams, &eb);
     *  @p
     *
     *  The following statements statically create a task in the
     *  configuration file:
     *
     *  @p(code)
     *  var params = new Task.Params;
     *  params.instance.name = "tsk0";
     *  params.arg0 = 1;
     *  params.arg1 = 2;
     *  params.priority = 1;
     *  Task.create('&tsk0_func', params);
     *  @p
     *
     *  If NULL is passed instead of a pointer to an actual Task_Params
     *  struct, a
     *  default set of parameters is used. The "eb" is an error block that
     *  you can use
     *  to handle errors that may occur during Task object creation.
     *
     *  The newly created task is placed in {@link #Mode_READY} mode, and is
     *  scheduled to begin concurrent execution of the following function
     *  call:
     *
     *  @p(code)
     *  (*fxn)(arg1, arg2);
     *  @p
     *
     *  As a result of being made ready to run, the task runs any
     *  application-wide Ready functions that have been specified.
     *
     *  Task_exit is automatically called if and when the task returns
     *  from fxn.
     *
     *  @p(html)
     *  <B>Create Hook Functions</B>
     *  @p
     *
     *  You can specify application-wide Create hook functions in your config
     *  file that run whenever a task is created. This includes tasks that
     *  are created statically and those created dynamically using
     *  Task_create.
     *
     *  For Task objects created statically, Create functions are called
     *  during the Task module initialization phase of the program startup
     *  process prior to main().
     *
     *  For Task objects created dynamically, Create functions
     *  are called after the task handle has been initialized but before the
     *  task has been placed on its ready queue.
     *
     *  Any SYS/BIOS function can be called from Create functions.
     *  SYS/BIOS passes the task handle of the task being created to each of
     *  the Create functions.
     *
     *  All Create function declarations should be similar to this:
     *  @p(code)
     *  Void myCreateFxn(Task_Handle task);
     *  @p
     *
     *  @param(fxn)     Task Function
     *
     *  @a(constraints)
     *  @p(blist)
     *  - The fxn parameter and the name attribute cannot be NULL.
     *  - The priority attribute must be less than or equal to
     *  ({@link #numPriorities} - 1) and greater than or equal to one (1)
     *  (priority 0 is owned by the Idle task).
     *  - The priority can be set to -1 for tasks that will not execute
     *  until another task changes the priority to a positive value.
     *  - The stackHeap attribute must identify a valid memory Heap.
     *  @p
     */
    create(FuncPtr fxn);

    // -------- Handle Parameters --------
    /* REQ_TAG(SYSBIOS-463) */

    /*! Task function argument. Default is 0 */
    config UArg arg0 = 0;

    /*! Task function argument. Default is 0 */
    config UArg arg1 = 0;

    /*!
     *  Task priority (0 to Task.numPriorities-1, or -1).
     *  Default is 1.
     */
    config Int priority = 1;

    /*!
     *  Task stack pointer. Default = null.
     *
     *  Null indicates that the stack is to be allocated by create().
     *
     *  @a(Static Configuration Usage Warning)
     *  This parameter can only be assigned a non-null value
     *  during runtime Task creates or constructs.
     *
     *  Static configuration of the 'stack' parameter is not supported.
     *
     *  Note that if {@link ti.sysbios.BIOS#runtimeCreatesEnabled
     *  BIOS.runtimeCreatesEnabled} is set to false, then the user is required
     *  to provide the stack buffer when constructing the Task object.
     *  If 'stack' is not provided, then Task_construct() will fail.
     */
    config Ptr stack = null;

    /*!
     *  Task stack size in MAUs.
     *
     *  The default value of 0 means that the module config
     *  {@link #defaultStackSize} is used.
     */
    config SizeT stackSize = 0;

    /*!
     *  Mem section used for statically created task stacks.
     *
     *  Default is inherited from module config defaultStackSection.
     */
    metaonly config String stackSection;

    /*!
     *  Mem heap used for dynamically created task stack.
     *
     *  The default value of NULL means that the module config
     *  {@link #defaultStackHeap} is used.
     */
    config IHeap.Handle stackHeap = null;

    /*! Environment data struct. */
    config Ptr env = null;

    /*!
     *  Exit system immediately when the last task with this
     *  flag set to TRUE has terminated.
     *
     *  Default is true.
     */
    config Bool vitalTaskFlag = true;

    /*!
     *  The core which this task is to run on. Default is Task_AFFINITY_NONE
     *
     *  If there is a compelling reason for a task to be pinned to a
     *  particular core, then setting 'affinity' to the corresponding core
     *  id will force the task to only be run on that core.
     *
     *  The default affinity is inherited from {@link #defaultAffinity
     *  Task.defaultAffinity}
     *  which in turn defaults to {@link #AFFINITY_NONE Task_AFFINITY_NONE},
     *  which means the task can be run on either core.
     *
     *  Furthermore,  Task_AFFINITY_NONE implies that the task can be moved
     *  from core to core as deemed necessary by the Task scheduler in order
     *  to keep the two highest priority ready tasks running simultaneously.
     */
    config UInt affinity;

    // -------- Handle Functions --------

    /*!
     *  @_nodoc
     *  ======== getArg0 ========
     *  Returns arg0 passed via params to create.
     *
     *  @b(returns)     task's arg0
     */
    UArg getArg0();

    /*!
     *  @_nodoc
     *  ======== getArg1 ========
     *  Returns arg1 passed via params to create.
     *
     *  @b(returns)     task's arg1
     */
    UArg getArg1();

    /*!
     *  ======== getEnv ========
     *  Get task environment pointer.
     *
     *  Task_getEnv returns the environment pointer of the specified task. The
     *  environment pointer references an arbitrary application-defined data
     *  structure.
     *
     *  If your program uses multiple hook sets, {@link #getHookContext}
     *  allows you to get environment pointers you have set for a particular
     *  hook set and Task object combination.
     *
     *  @b(returns)     task environment pointer
     */
    Ptr getEnv();

    /*!
     *  ======== getFunc ========
     *  Get Task function and arguments
     *
     *  If either arg0 or arg1 is NULL, then the corresponding argument is not
     *  returned.
     *
     *  @param(arg0)    pointer for returning Task's first function argument
     *  @param(arg1)    pointer for returning Task's second function argument
     *
     *  @b(returns)     Task function
     */
    /* REQ_TAG(SYSBIOS-455) */
    FuncPtr getFunc(UArg *arg0, UArg *arg1);

    /*!
     *  ======== getHookContext ========
     *  Get hook set's context for a task.
     *
     *  For example, this C code gets the HookContext, prints it,
     *  and sets a new value for the HookContext.
     *
     *  @p(code)
     *  Ptr pEnv;
     *  Task_Handle myTask;
     *  Int myHookSetId1;
     *
     *  pEnv = Task_getHookContext(task, myHookSetId1);
     *
     *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
     *                (ULong)pEnv, (ULong)Timestamp_get32());
     *
     *  Task_setHookContext(task, myHookSetId1, (Ptr)0xc0de1);
     *  @p
     *
     *  See {@link #hookfunc Hook Functions} for more details.
     *
     *  @param(id)      hook set ID
     *  @b(returns)     hook set context for task
     */
    /* REQ_TAG(SYSBIOS-454) */
    Ptr getHookContext(Int id);

    /*!
     *  ======== getPri ========
     *  Get task priority.
     *
     *  Task_getPri returns the priority of the referenced task.
     *
     *  @b(returns)     task priority
     */
    /* REQ_TAG(SYSBIOS-510) */
    Int getPri();

    /*!
     *  @_nodoc
     *  ======== setArg0 ========
     *  Set arg0 (used primarily for legacy support)
     */
    Void setArg0(UArg arg);

    /*!
     *  @_nodoc
     *  ======== setArg1 ========
     *  Set arg1 (used primarily for legacy support)
     */
    Void setArg1(UArg arg);

    /*!
     *  ======== setEnv ========
     *  Set task environment.
     *
     *  Task_setEnv sets the task environment pointer to env. The
     *  environment pointer references an arbitrary application-defined
     *  data structure.
     *
     *  If your program uses multiple hook sets, {@link #setHookContext}
     *  allows you to set environment pointers for any
     *  hook set and Task object combination.
     *
     *  @param(env)     task environment pointer
     */
    Void setEnv(Ptr env);

    /*!
     *  ======== setHookContext ========
     *  Set hook instance's context for a task.
     *
     *  For example, this C code gets the HookContext, prints it,
     *  and sets a new value for the HookContext.
     *
     *  @p(code)
     *  Ptr pEnv;
     *  Task_Handle myTask;
     *  Int myHookSetId1;
     *
     *  pEnv = Task_getHookContext(task, myHookSetId1);
     *
     *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
     *                (ULong)pEnv, (ULong)Timestamp_get32());
     *
     *  Task_setHookContext(task, myHookSetId1, (Ptr)0xc0de1);
     *  @p
     *
     *  See {@link #hookfunc Hook Functions} for more details.
     *
     *  @param(id)              hook set ID
     *  @param(hookContext)     value to write to context
     */
    /* REQ_TAG(SYSBIOS-454) */
    Void setHookContext(Int id, Ptr hookContext);

    /*!
     *  ======== setPri ========
     *  Set a task's priority
     *
     *  Task_setpri sets the execution priority of task to newpri, and returns
     *  that task's old priority value. Raising or lowering a task's priority
     *  does not necessarily force preemption and re-scheduling of the caller:
     *  tasks in the {@link #Mode_BLOCKED} mode remain suspended despite a
     *  change in priority; and tasks in the {@link #Mode_READY} mode gain
     *  control only if their new priority is greater than that of the
     *  currently executing task.
     *
     *  newpri should be set to a value greater than or equal to 1 and
     *  less than or equal to ({@link #numPriorities} - 1).  newpri can also
     *  be set to -1 which puts the the task into the INACTIVE state and the
     *  task will not run until its priority is raised at a later time by
     *  another task.  Priority 0 is reserved for the idle task.
     *  If newpri equals ({@link #numPriorities} - 1), execution of the task
     *  effectively locks out all other program activity, except for the
     *  handling of interrupts.
     *
     *  The current task can change its own priority (and possibly preempt its
     *  execution) by passing the output of {@link #self} as the value of the
     *  task parameter.
     *
     *  A context switch occurs when calling Task_setpri if a currently
     *  running task priority is set lower than the priority of another
     *  currently ready task, or if another ready task is made to have a
     *  higher priority than the currently running task.
     *
     *  Task_setpri can be used for mutual exclusion.
     *
     *  If a task's new priority is different than its previous priority,
     *  then its relative placement in its new ready task priority
     *  queue can be different than the one it was removed from. This can
     *  effect the relative order in which it becomes the running task.
     *
     *  The effected task is placed at the head of its new priority queue
     *  if it is the currently running task. Otherwise it is placed at
     *  at the end of its new task priority queue.
     *
     *  @param(newpri) task's new priority
     *  @b(returns)     task's old priority
     *
     *  @a(constraints)
     *  newpri must be a value between 1 and ({@link #numPriorities} - 1) or -1.
     *
     *  The task cannot be in the {@link #Mode_TERMINATED} mode.
     *
     *  The new priority should not be zero (0). This priority level is
     *  reserved for the Idle task.
     */
    /* REQ_TAG(SYSBIOS-510) */
    Int setPri(Int newpri);

    /*!
     *  ======== stat ========
     *  Retrieve the status of a task.
     *
     *  Task_stat retrieves attribute values and status information about a
     *  task.
     *
     *  Status information is returned through statbuf, which references a
     *  structure of type {@link #Stat}.
     *
     *  When a task is preempted by a software or hardware interrupt, the task
     *  execution mode returned for that task by Task_stat is still
     *  {@link #Mode_RUNNING}  because the task runs when the preemption ends.
     *
     *  The current task can inquire about itself by passing the output of
     *  {@link #self} as the first argument to Task_stat. However, the task
     *  stack pointer (sp) in the {@link #Stat} structure is the value from
     *  the previous context switch.
     *
     *  Task_stat has a non-deterministic execution time. As such, it is not
     *  recommended to call this API from Swis or Hwis.
     *
     *  @param(statbuf) pointer to task status structure
     *
     *  @a(constraints)
     *  statbuf cannot be NULL;
     */
    Void stat(Stat *statbuf);

    /*!
     *  ======== getMode ========
     *  Retrieve the {@link #Mode} of a task.
     */
    Mode getMode();

    /*!
     *  ======== setAffinity ========
     *  Set task's core affinity (should be used only in applications built
     *  with {@link ti.sysbios.BIOS#smpEnabled BIOS.smpEnabled} set to true)
     *
     *  If the new core ID is different than the current core affinity
     *  a reschedule will be performed immediately.
     *
     *  @a(constraints)
     *  Must NOT be called with interrupts disabled
     *  (ie within a Hwi_disable()/Hwi_restore() block).
     *
     *  Must NOT be called with tasking disabled
     *  (ie within a Task_disable()/Task_restore() block).
     *
     *  @b(returns)     task's previous core affinity
     */
    UInt setAffinity(UInt coreId);

    /*!
     *  ======== getAffinity ========
     *  Return task's core affinity (should be used only in applications built
     *  with {@link ti.sysbios.BIOS#smpEnabled} set to true)
     *
     *  @b(returns)     task's current core affinity
     */
    UInt getAffinity();

    /*!
     *  @_nodoc
     *  ======== block ========
     *  Block a task.
     *
     *  Remove a task from its ready list.
     *  The effect of this API is manifest the next time the internal
     *  Task scheduler is invoked.
     *  This can be done directly by embedding the call within a
     *  {@link #disable}/{@link #restore} block.
     *  Otherwise, the effect will be manifest as a result of processing
     *  the next dispatched interrupt, or by posting a Swi, or by falling
     *  through the task function.
     *
     *  @a(constraints)
     *  If called from within a Hwi or a Swi, or main(), there is no need
     *  to embed the call within a {@link #disable}/{@link #restore} block.
     */
    Void block();

    /*!
     *  @_nodoc
     *  ======== unblock ========
     *  Unblock a task.
     *
     *  Place task in its ready list.
     *  The effect of this API is manifest the next time the internal
     *  Task scheduler is invoked.
     *  This can be done directly by embedding the call within a
     *  {@link #disable}/{@link #restore} block.
     *  Otherwise, the effect will be manifest as a result of processing
     *  the next dispatched interrupt, or by posting a Swi, or by falling
     *  through the task function.
     *
     *  @a(constraints)
     *  If called from within a Hwi or a Swi, or main(), there is no need
     *  to embed the call within a {@link #disable}/{@link #restore} block.
     */
    Void unblock();

    /*!
     *  @_nodoc
     *  ======== blockI ========
     *  Block a task.
     *
     *  Remove a task from its ready list.
     *  Must be called within Task_disable/Task_restore block
     *  with interrupts disabled.
     *  This API is meant to be used internally.
     */
    Void blockI();

    /*!
     *  @_nodoc
     *  ======== unblockI ========
     *  Unblock a task.
     *
     *  Place task in its ready list.
     *  Must be called within Task_disable/Task_restore block
     *  with interrupts disabled.
     *  This API is meant to be used internally.
     *
     *  @param(hwiKey) key returned from Hwi_disable()
     */
    Void unblockI(UInt hwiKey);

internal:   /* not for client use */

    /*! Target-specific support functions. */
    proxy SupportProxy inherits ti.sysbios.interfaces.ITaskSupport;

    /*
     *  ======== schedule ========
     *  Find highest priority ready task and invoke it.
     *
     *  Must be called with interrupts disabled.
     */
    /* REQ_TAG(SYSBIOS-456) */
    Void schedule();

    /*
     *  ======== enter ========
     *  Task's initial entry point before entering task function.
     */
    Void enter();

    /*
     *  ======== sleepTimeout ========
     *  This function is the clock event handler for sleep.
     */
    Void sleepTimeout(UArg arg);

    /*
     *  ======== postInit ========
     *  finish initializing static and dynamic Tasks
     */
    Int postInit(Object *task, Error.Block *eb);

    /*
     *  Number of statically constructed Task objects.
     *  Shouldn't be set directly by the user's
     *  config (it gets set by instance$static$init).
     */
    config UInt numConstructedTasks = 0;

    /*
     *  ======== allBlockedFunction ========
     *  default function to be called
     */
    Void allBlockedFunction();

    /*
     *  ======== deleteTerminatedTasksFunc ========
     *  Idle func that deletes the first terminated task it finds
     *  in the queue of dynamically created tasks
     */
    Void deleteTerminatedTasksFunc();

    /*
     *  ======== Task_processVitalTasks ========
     *  Call BIOS_exit() when last vitalTask exits or is
     *  deleted.
     */
    Void processVitalTaskFlag(Object *task);

    /*
     *  ======== moduleStateCheck ========
     */
    Int moduleStateCheck(Task.Module_State *moduleState, UInt32 checkValue);

    /*
     *  ======== getModuleStateCheckValue ========
     */
    UInt32 getModuleStateCheckValue(Task.Module_State *moduleState);

    /*
     *  ======== objectCheck ========
     */
    Int objectCheck(Task.Handle handle, UInt32 checkValue);

    /*
     *  ======== getObjectCheckValue ========
     */
    UInt32 getObjectCheckValue(Task.Handle handle);

    /*
     *  ======== startupHookFunc ========
     *  Called by core 0 just before switch to first task
     */
    config Void (*startupHookFunc)(Void) = null;

    /*
     *  Common object used by all blocked tasks to enable Task_delete()
     *  to remove a task from any pend Q it is placed on while blocked.
     */
    struct PendElem {
        Queue.Elem      qElem;
        Task.Handle     task;
        Clock.Handle    clock;
    };

    struct Instance_State {
        Queue.Elem      qElem;          // Task's readyQ element
        volatile Int    priority;       // Task priority
        UInt            mask;           // curSet mask = 1 << priority
        Ptr             context;        // ptr to Task's saved context
                                        // while not in RUNNING mode.
        Mode            mode;           // READY, BLOCKED, RUNNING, etc
        PendElem        *pendElem;      // ptr to Task, Semaphore, Event,
                                        // or GateMutexPri PendElem
        SizeT           stackSize;      // Task's stack buffer size
        Char            stack[];        // buffer used for Task's stack
        IHeap.Handle    stackHeap;      // Heap to allocate stack from
        FuncPtr         fxn;            // Task function
        UArg            arg0;           // Task function 1st arg
        UArg            arg1;           // Task function 2nd arg
        Ptr             env;            // Task environment pointer
        Ptr             hookEnv[];      // ptr to Task's hook env array
        Bool            vitalTaskFlag;  // TRUE = shutdown system if
                                        // last task like this exits
        Queue.Handle    readyQ;         // This Task's readyQ
        UInt            curCoreId;      // Core this task is currently running on.
        UInt            affinity;       // Core this task must run on
                                        // Task_AFFINITY_NONE = don't care
    };

    struct Module_State {
        volatile Bool   locked;         // Task scheduler locked flag
        volatile UInt   curSet;         // Bitmask reflects readyQ states
        volatile Bool   workFlag;       // Scheduler work is pending.
                                        // Optimization. Must be set
                                        // whenever readyQs are modified.
        UInt            vitalTasks;     // number of tasks with
                                        // vitalTaskFlag = true
        Handle          curTask;        // current Task instance
        Queue.Handle    curQ;           // current Task's readyQ
        Queue.Object    readyQ[];       // Task ready queues

        volatile UInt   smpCurSet[];    // Bitmask reflects readyQ states
                                        // curSet[n] = core n
                                        // curSet[Core.numCores] = don't care
        volatile UInt   smpCurMask[];   // mask of currently running tasks
        Handle          smpCurTask[];   // current Task instance ([0] = core 0, etc)
        Queue.Handle    smpReadyQ[];    // core ready queues
                                        // [0] = core0 readyQs
                                        // [1] = core1 readyQs
                                        // [numCores] = don't care readyQs
        Queue.Object    inactiveQ;      // Task's with -1 priority
        Queue.Object    terminatedQ;    // terminated dynamically created Tasks

        Handle          idleTask[];             // Idle Task handles
        Handle          constructedTasks[];     // array of statically
                                                // constructed Tasks
    };

    struct RunQEntry {
        Queue.Elem      elem;
        UInt            coreId;
        Int             priority;
    };

    struct Module_StateSmp {
        Queue.Object        *sortedRunQ;         // A queue of RunQEntry elems
                                                 // that is  sorted by priority
        volatile RunQEntry  smpRunQ[];           // Run queue entry handles for
                                                 // each core
    };
}
