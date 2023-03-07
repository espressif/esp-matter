/*
 * Copyright (c) 2014-2018, Texas Instruments Incorporated
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
 *  ======== Swi.xdc ========
 *
 */

package ti.sysbios.knl;

import xdc.rov.ViewInfo;

import xdc.runtime.Error;
import xdc.runtime.Assert;
import xdc.runtime.Diags;
import xdc.runtime.Log;
import xdc.runtime.Types;

import ti.sysbios.knl.Queue;

/*!
 *  ======== Swi ========
 *  Software Interrupt Manager
 *
 *  The Swi module manages software interrupt service routines, which are
 *  patterned after hardware interrupt service routines.

 *  SYS/BIOS manages four distinct levels of execution threads: hardware
 *  interrupt service routines, software interrupt routines, tasks, and
 *  background idle functions. A software interrupt is an object that
 *  encapsulates a function to be executed and a priority.
 *  Software interrupts are prioritized, preempt tasks, and are preempted
 *  by hardware interrupt service routines.
 *
 *  Each software interrupt has a priority level. A software interrupt
 *  preempts any lower-priority software interrupt currently executing.
 *
 *  A target program uses an API call to post a Swi object. This causes the
 *  Swi module to schedule execution of the software interrupt's function.
 *  When a Swi is posted by an API call, the Swi object's function is not
 *  executed immediately. Instead, the function is scheduled for execution.
 *  SYS/BIOS uses the Swi's priority to determine whether to preempt the
 *  thread currently running. Note that if a Swi is posted several times
 *  before it begins running, (because Hwis and higher priority interrupts
 *  are running,) when the Swi does eventually run, it will run only one time.
 *
 *  Software interrupts can be posted for execution with a call to
 *  {@link #post} or a number of other Swi functions. Each Swi object has a
 *  "trigger" which is used either to determine whether to post the Swi or as
 *  a value that can be evaluated within the Swi's function. {@link #andn} and
 *  {@link #dec} post the Swi if the trigger value transitions to 0.
 *  {@link #or} and {@link #inc} also modify the trigger value. ({@link #or}
 *  sets bits, and {@link #andn} clears bits.)
 *
 *  The {@link #disable} and {@link #restore} operations allow you to
 *  post several
 *  Swis and enable them all for execution at the same time. The Swi
 *  priorities then determine which Swi runs first.
 *
 *  All Swis run to completion; you cannot suspend a Swi while it waits for
 *  something (for example, a device) to be ready. So, you can use the
 *  trigger to tell the Swi when all the devices and other conditions it
 *  relies on are ready. Within a Swi processing function, a call to
 *  Swi_getTrigger returns the value of the trigger when the Swi started
 *  running.
 *  Note that the trigger is automatically reset to its original value
 *  when a Swi runs; however, {@link #getTrigger} will return the saved
 *  trigger
 *  value from when the Swi started execution.
 *
 *  All Swis run with interrupts globally enabled (ie GIE = 1).
 *  Therefore, any Swi module API that results in a
 *  Swi being made ready to run (ie {@link #post}, {@link #inc},
 *  {@link #andn}, {@link #or}, {@link #restore}, or {@link #enable})
 *  will subsequently also cause interrupts to be enabled while the
 *  Swi function executes. Upon return from the Swi function,
 *  global interrupts are restored to their previous enabled/disabled
 *  state.
 *
 *  A Swi preempts any currently running Swi with a lower priority.
 *  When multiple Swis of the same priority level have been posted,
 *  their respective Swi functions are executed in the order the Swis
 *  were posted.
 *  Hwis in turn preempt any currently running Swi,
 *  allowing the target to respond quickly to hardware peripherals.
 *
 *  Swi threads are executed using the ISR (or "Hwi") stack. Thus
 *  they share the ISR stack with Hwi threads.
 *
 *  @p(html)
 *  <a name="hookfunc"></a>
 *  @p
 *
 *  @a(Hook Functions)
 *
 *  Sets of hook functions can be specified for the Swi module.  Each set
 *  contains these hook functions:
 *  @p(blist)
 *  -Register:  A function called before all statically-created Swis
 *      are initialized at runtime.
 *  -Create:    A function that is called when a Swi is created.
 *      This includes Swis that are created statically and those
 *      created dynamically using {@link #create}.
 *  -Ready:     A function that is called when any Swi becomes ready
 *      to run.
 *  -Begin:     A function that is called just prior to running a Swi.
 *  -End:       A function that is called just after a Swi finishes.
 *  -Delete:    A function that is called when a Swi is deleted at
 *      run-time with {@link #delete}.
 *  @p
 *  Hook functions can only be configured statically.
 *
 *  If you define more than one set of hook functions, all the functions
 *  of a particular type will be run when a Swi triggers that type of
 *  hook.
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
 *  The create and delete functions are called whenever a Swi is created
 *  or deleted.  They are called with interrupts enabled (unless called
 *  at boot time or from main()).
 *
 *  @p(code)
 *  Void myCreateFxn(Swi_Handle swi, Error_Block *eb);
 *  @p
 *
 *  @p(code)
 *  Void myDeleteFxn(Swi_Handle swi);
 *  @p
 *
 *  @p(html)
 *  <B>Ready, Begin, and End Functions</B>
 *  @p
 *
 *  The ready, begin and end functions are all called with interrupts
 *  enabled.  The ready function is called when a Swi is posted and made
 *  ready to run.  The begin function is called right before the function
 *  associated with the given Swi is run.  The end function is called
 *  right after this function returns.
 *
 *  @p(code)
 *  Void myReady(Swi_Handle swi);
 *  @p
 *
 *  @p(code)
 *  Void myBegin(Swi_Handle swi);
 *  @p
 *
 *  @p(code)
 *  Void myEnd(Swi_Handle swi);
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
 *    <!--                                             -->
 *    <tr><td> {@link #create}          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #disable}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getTrigger}      </td><td>   Y    </td><td>   Y    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}     </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #restore}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #self}            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *
 *    <tr><td> {@link #andn}            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #construct}       </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #dec}             </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}        </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getAttrs}        </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getFunc}         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getHookContext}  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getPri}          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #inc}             </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #or}              </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #post}            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setAttrs}        </td><td>   Y*   </td><td>   Y*   </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setHookContext}  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *  (e.g. Swi_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *  (e.g. Swi_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
@ModuleStartup      /* generate a call to Swi_Module_startup */
@InstanceFinalize   /* generate call to Swi_Instance_finalize on delete */
@InstanceInitError  /* instance init can fail, call finalize if so */
@InstanceInitStatic /* Construct/Destruct CAN becalled at runtime */

module Swi
{
    // -------- Module Constants --------

    // -------- Module Types --------

    /*!
     *  ======== FuncPtr ========
     *  Swi function type definition
     *
     *  All Swi functions are passed two uninterpreted arguments of type
     *  UArg and have no return value.
     */
    typedef Void (*FuncPtr)(UArg, UArg);

    /*!
     *  ======== HookSet ========
     *  Swi hook set type definition
     *
     *  This structure defines the set of hook functions that can be
     *  specified for the Swi module.
     *
     *  See {@link #hookfunc Hook Functions} for details.
     */
    struct HookSet {
        Void (*registerFxn)(Int);
        Void (*createFxn)(Handle, Error.Block *);
        Void (*readyFxn)(Handle);
        Void (*beginFxn)(Handle);
        Void (*endFxn)(Handle);
        Void (*deleteFxn)(Handle);
    };

    /*!
     *  ======== Struct2 ========
     */
    struct Struct2__ {
        Queue.Elem      qElem;
        FuncPtr         fxn;
        UArg            arg0;
        UArg            arg1;
        UInt            priority;
        UInt            mask;
        Bool            posted;
        UInt            initTrigger;
        UInt            trigger;
        Queue.Handle    readyQ;
        Ptr             hookEnv[];
        Types.CordAddr  name;
    };

    typedef Struct2__ Struct2;

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String  label;
        String  state;
        UInt    priority;
        String  fxn[];
        UArg    arg0;
        UArg    arg1;
        UInt    initTrigger;
        UInt    curTrigger;
        //Ptr   hookEnv[];
    };

    /*!
     *  ======== ModuleView ========
     *  @_nodoc
     */
    metaonly struct ModuleView {
        String  schedulerState;
        String  readyQMask;
        Ptr     currentSwi;
        String  currentFxn[];
    };

    /*!
     *  ======== ReadyQView ========
     *  @_nodoc
     */
    metaonly struct ReadyQView {
        Ptr         swi;
        Ptr         next;
        Ptr         prev;
        Ptr         readyQ;
        String      label;
        String      state;
        Int         priority;
        String      fxn[];
        UArg        arg0;
        UArg        arg1;
    }

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        xdc.rov.ViewInfo.create({
            viewMap: [
                ['Basic',    {type: ViewInfo.INSTANCE,     viewInitFxn: 'viewInitBasic',    structName: 'BasicView'}],
                ['Module',   {type: ViewInfo.MODULE,       viewInitFxn: 'viewInitModule',   structName: 'ModuleView'}],
                ['ReadyQs',  {type: ViewInfo.TREE_TABLE,   viewInitFxn: 'viewInitReadyQs',  structName: 'ReadyQView'}],
            ]
         });

    // -------- Module Proxies --------

    // -------- Module Parameters --------

    /*!
     *  ======== LM_begin ========
     *  The event logged just prior to invoking a Swi's function
     */
    config Log.Event LM_begin = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_begin: swi: 0x%x, func: 0x%x, preThread: %d"
    };

    /*!
     *  ======== LD_end ========
     *  The event logged just after returning from a Swi's function
     */
    config Log.Event LD_end = {
        mask: Diags.USER2,
        msg: "LD_end: swi: 0x%x"
    };

    /*!
     *  ======== LM_post ========
     *  The event logged when Swi_post() is called
     */
    config Log.Event LM_post = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_post: swi: 0x%x, func: 0x%x, pri: %d"
    };

    /*!
     *  ======== A_swiDisabled ========
     *  Assertion raised if Swi_create is called and runtime Swi creation is
     *  disabled
     *
     *  see {@link ti.sysbios.BIOS#swiEnabled}
     */
    config Assert.Id A_swiDisabled = {
        msg: "A_swiDisabled: Cannot create a Swi when Swi is disabled."
    };

    /*!
     *  ======== A_badPriority ========
     *  Assertion raised if a Swi's priority is out of range
     *
     *  Swi priorities must be in the range of 0 and numPriorities-1.
     */
    config Assert.Id A_badPriority = {
        msg: "A_badPriority: An invalid Swi priority was used."
    };

    /*!
     *  ======== numPriorities ========
     *  Number of Swi priorities supported
     *
     *  The maximum number of priorities supported is
     *  target-specific and depends on the number of
     *  bits in a UInt data type. For 6x and ARM devices
     *  the maximum number of priorities is therefore 32.
     *  For the C28x, the maximum number of
     *  priorities is 16.
     */
    config UInt numPriorities = 16;

    /*!
     *  ======== hooks ========
     *  const array to hold all HookSet objects
     *  @_nodoc
     */
    config HookSet hooks[length] = [];


    // -------- Module Functions --------

    /*!
     *  ======== construct2 ========
     *  Construct a software interrupt
     *
     *  Swi_construct2 constructs a Swi object.  This function is identical
     *  to Swi_construct(), but does not take an Error_Block parameter, and
     *  returns a Swi_Handle.
     *
     *  The following C code sets Swi parameters and
     *  constructs a Swi object:
     *
     *  @p(code)
     *
     *  Swi_Struct2 swiStruct2;
     *  Swi_Handle  swi;
     *
     *  Void main()
     *  {
     *      Swi_Params swiParams;
     *
     *      Swi_Params_init(&swiParams);
     *      swiParams.arg0 = 1;
     *      swiParams.arg1 = 0;
     *      swiParams.priority = 2;
     *      swiParams.trigger = 0;
     *
     *      swi = Swi_construct2(&swiStruct2,  swiFxn, &swiParams);
     *      if (swi == NULL) {
     *          // Failure
     *      }
     *
     *      BIOS_start();
     *  }
     *  @p
     *
     *  @param(swi)        Pointer to Swi_Struct2 object.
     *  @param(swiFxn)     Swi Function
     *  @param(prms)       Pointer to Swi_Params structure
     *
     *  @b(returns)        A Swi handle
     */
    Handle construct2(Struct2 *swi, FuncPtr swiFxn, const Params *prms);

    /*!
     *  ======== addHookSet ========
     *  Add hook functions to be called by the Swi scheduler
     *
     *  This function is used in a config file to add a set of functions
     *  that are called before or after significant points within the Swi
     *  scheduler.
     *
     *  Configures a set of hook functions for the
     *  Swi module. Each set contains these hook functions:
     *
     *  @p(blist)
     *  -Register:  A function called before all statically-created Swis
     *      are initialized at runtime.
     *  -Create:    A function that is called when a Swi is created.
     *      This includes Swis that are created statically and those
     *      created dynamically using {@link #create}.
     *  -Ready:     A function that is called when any Swi becomes ready
     *      to run.
     *  -Begin:     A function that is called just prior to running a Swi.
     *  -End:       A function that is called just after a Swi finishes.
     *  -Delete:    A function that is called when a Swi is deleted at
     *  run-time with {@link #delete}.
     *  @p
     *
     *  See {@link #hookfunc Hook Functions} for more details.
     *
     *  HookSet structure elements may be omitted, in which case those
     *  elements will not exist.
     *
     *  For example, the following configuration code defines a
     *  HookSet:
     *
     *  @p(code)
     *  // Hook Set 1
     *  Swi.addHookSet({
     *     registerFxn: '&myRegister1',
     *     createFxn:   '&myCreate1',
     *     readyFxn:    '&myReady1',
     *     beginFxn:    '&myBegin1',
     *     endFxn:      '&myEnd1',
     *     deleteFxn:   '&myDelete1'
     *  });
     *  @p
     *
     *  @param(hookSet)         structure of type HookSet
     */
    metaonly Void addHookSet(HookSet hookSet);

    /*!
     *  ======== Swi_startup ========
     *  Start the Swi scheduler
     *
     *  @_nodoc
     *  This function is called in BIOS_start() after Hwi_enable().
     */
    Void startup();

    /*!
     *  ======== Swi_enabled ========
     *  Returns TRUE if the Swi scheduler is enabled
     *
     *  @_nodoc
     */
    Bool enabled();

    /*!
     *  @_nodoc
     *  ======== unlockSched ========
     *  Force a Swi scheduler unlock. Used by Core_atExit() & Core_hwiFunc()
     *  to unlock Swi scheduler before exiting.
     *
     *  This function should only be called after a Hwi_disable() has entered
     *  the Inter-core gate and disabled interrupts locally.
     */
    Void unlockSched();

    /*!
     *  ======== disable ========
     *  Disable Swi Scheduling
     *
     *  Swi_disable() and {@link #restore Swi_restore()} control Swi
     *  scheduling.
     *  Swi_disable() disables all Swi functions from running until
     *  Swi_restore() is called. Hardware interrupts can still run.
     *
     *  Swi_disable() and Swi_restore() allow you to ensure that
     *  statements that must be performed together during critical
     *  processing are not preempted by other Swis or Tasks.
     *
     *  The value of the key returned by Swi_disable() is opaque to
     *  applications and is meant to be passed to Swi_restore().
     *
     *  In the following example, the critical section cannot be preempted
     *  by any Swis. Nor can it be pre-empted by other Tasks.
     *
     *  @p(code)
     *  key = Swi_disable();
     *      `critical section`
     *  Swi_restore(key);
     *  @p
     *
     *  @a(Side Effects of Disabling the Swi Scheduler)
     *
     *  {@link #disable Swi_disable()}, in addition to disabling Swis from
     *  pre-empting the  code which follows its invocation, has
     *  the side effect of also disabling the Task scheduler.
     *  Consequently, Task pre-emption and blocking is also disabled while
     *  the Swi scheduler is disabled.
     *  When {@link #restore Swi_restore()} is subsequently called, it will
     *  re-enable and invoke the Task scheduler if the Task scheduler was not
     *  already disabled prior to invoking Swi_disable().
     *
     *  The following code snippet:
     *  @p(code)
     *  key = Swi_disable();
     *  ...
     *  Swi_post(swi);        <-- 'swi' will not run
     *  ...
     *  Swi_restore(key);     <-- 'swi' runs now
     *  @p
     *  Should be thought of as equivalent to this:
     *  @p(code)
     *  tasKey = Task_disable();
     *  swiKey = Swi_disable();
     *  ...
     *  Swi_post(swi);        <-- 'swi' will not run
     *  ...
     *  Swi_restore(swiKey);  <-- 'swi' runs now
     *  Task_restore(taskKey);
     *  @p
     *
     *  In the following example, even though the Semaphore_post() call
     *  unblocks a task of higher priority, the local task is not pre-empted
     *  until after the Swi_restore() call is made:
     *
     *  @p(code)
     *  key = Swi_disable();
     *  ...
     *  Swi_post(swi);        <-- 'swi' will not run
     *  Semaphore_post(sem);  <-- readys a task of higher priority than current task
     *  ...
     *  Swi_restore(key);     <-- 'swi' runs now, then current task is pre-empted.
     *  @p
     *
     *  @p(html)
     *  <B>
     *  A common mistake that users make is to invoke a blocking
     *  API such as Semaphore_pend() after calling Swi_disable().
     *  This results in unrecoverable damage to the Task scheduler's internal
     *  state and will lead to unpredictable and usually catastrophic behavior:
     *  </B>
     *  @p
     *
     *  @p(code)
     *  key = Swi_disable();
     *  ...
     *  Semaphore_pend(sem, BIOS_WAIT_FOREVER);  <-- !!! DO NOT DO THIS !!!
     *  ...
     *  Swi_restore(key);   <-- !!! System failure guaranteed to follow !!!
     *  @p
     *
     *  @p(html)
     *  <B>
     *  A more subtle variant of the above problem occurs when an API such
     *  as GateMutex_enter() is invoked directly or indirectly while the
     *  Swi scheduler
     *  is disabled. If the GateMutex has already been entered by another thread,
     *  the current thread should block until the other thread calls
     *  GateMutex_leave().
     *  But because the Task scheduler is disabled, the local thread returns
     *  immediately from GateMutex_enter(), just as though it successfully
     *  entered the GateMutex! This usually leads to catastrophic results.
     *  </B>
     *  @p
     *
     *  @b(returns)     opaque key for use with Swi_restore()
     */
    UInt disable();

    /*!
     *  ======== enable ========
     *  Enable Swi Scheduling
     *
     *  @_nodoc
     *  Swi_enable unconditionally enables Swis and invokes the Swi scheduler
     *  if any Swis are pending.
     *
     *  @a(constraints)
     *  Swi_enable will also invoke the Task scheduler if the
     *  Task scheduler is not currently disabled.
     *
     *  The {@link #post} discussion regarding global interrupts also applies
     *  to this API.
     *
     */
    Void enable();

    /*!
     *  ======== restore ========
     *  Restore Swi Scheduling state
     *
     *  Swi_restore restores the Swi scheduler to the locked/unlocked state
     *  it was in when Swi_disable was called. If the scheduler becomes
     *  unlocked and Swis of sufficient priority have been made ready to
     *  run by any of the posting APIs, then they are run at this time.
     *
     *  Swi_disable and Swi_restore control software interrupt processing.
     *  Swi_disable disables all other Swi functions from running until
     *  Swi_restore is called. Hardware interrupts can still run.
     *
     *  Swi_disable and Swi_restore allow you to ensure that statements that
     *  must be performed together during critical processing are not
     *  pre-empted by other Swis.
     *
     *  In the following example, the critical section cannot be preempted
     *  by any Swis. Nor can it be pre-empted by other Tasks.
     *
     *  @p(code)
     *  key = Swi_disable();
     *      `critical section`
     *  Swi_restore(key);
     *  @p
     *
     *  Read the discussion of the side effects of disabling the Swi
     *  scheduler {@link #disable here}.
     *
     *  @a(constraints)
     *  Swi_restore will also re-enable and invoke the Task
     *  scheduler if the Task scheduler was not disabled prior to
     *  invoking Swi_disable().
     *
     *  The {@link #post} discussion regarding global interrupts applies
     *  to this API.
     *
     *  @param(key)     key to restore previous Swi scheduler state
     */
    Void restore(UInt key);

    /*!
     *  ======== restoreHwi ========
     *  Restore Swi Scheduling state
     *
     *  @_nodoc
     *  Optimized version used by Hwi dispatcher.
     */
    Void restoreHwi(UInt key);

    /*!
     *  ======== self ========
     *  Return address of currently executing Swi object
     *
     *  Swi_self returns the handle of the currently executing Swi.
     *
     *  For example, you can call Swi_self as follows if you want
     *  a Swi to repost itself:
     *
     *  @p(code)
     *  Swi_post( Swi_self() );
     *  @p
     *
     *  @b(returns)     handle of currently running Swi
     */
    Handle self();

    /*!
     *  ======== getTrigger ========
     *  Return the trigger value of the currently executing Swi
     *
     *  Swi_getTrigger returns the value that Swi's trigger had when the Swi
     *  started running. SYS/BIOS saves the trigger value internally, so that
     *  Swi_getTrigger can access it at any point within a Swi object's
     *  function, and then automatically resets the trigger to its initial
     *  value.
     *
     *  Swi_getTrigger should only be called within a function run by a Swi
     *  object.
     *
     *  When called from within the context of a Swi, the value returned by
     *  Swi_getTrigger is zero if the Swi was posted by a call to Swi_andn,
     *  or Swi_dec. Therefore, Swi_getTrigger provides relevant information
     *  only if the Swi was posted by a call to Swi_inc, Swi_or, or Swi_post.
     *
     *  This API is called within a Swi object's function to use the trigger
     *  value that caused the function to run. For example, if you use
     *  Swi_or or Swi_inc to post a Swi, different trigger values can require
     *  different processing.
     *
     *  @p(code)
     *  swicount = Swi_getTrigger();
     *  @p
     *
     *  @b(returns)         trigger value
     */
    UInt getTrigger();

    /*!
     *  ======== raisePri ========
     *  Raise a Swi's priority
     *
     *  @_nodoc
     *  This function is provided for legacy compatibility.
     *
     *  Swi_raisePri is used to raise the priority of the currently running
     *  Swi to the priority passed in as the argument. Swi_raisePri can be
     *  used in conjunction with Swi_restorePri to provide a mutual exclusion
     *  mechanism without disabling Swis.
     *
     *  Swi_raisePri should be called before a shared resource is accessed,
     *  and Swi_restorePri should be called after the access to the shared
     *  resource.
     *
     *  A call to Swi_raisePri not followed by a Swi_restorePri keeps the
     *  Swi's priority for the rest of the processing at the raised level. A
     *  Swi_post of the Swi posts the Swi at its original priority level.
     *
     *  A Swi object's execution priority must range from 0 to
     *  Swi_numPriorities - 1
     *
     *  Swi_raisePri never lowers the current Swi priority.
     *
     *  Constraints and Calling Context
     *  @p(blist)
     *  - Swi_raisePri must only be called from a Swi context.
     *  @p
     *
     *  @b(returns)         key for use with restorePri()
     */
    UInt raisePri(UInt priority);

    /*!
     *  ======== restorePri ========
     *  Restore a Swi's priority
     *
     *  @_nodoc
     *  This function is provided for legacy compatibility.
     *
     *  Swi_restorePri restores the priority to the Swi's priority prior to the
     *  Swi_raisePri call. Swi_restorePri can be used in
     *  conjunction with Swi_raisePri to provide a mutual exclusion mechanism
     *  without disabling all Swis.
     *
     *  Swi_raisePri should be called right before the shared resource is
     *  referenced, and Swi_restorePri should be called after the reference to
     *  the shared resource.
     *
     *  Constraints and Calling Context
     *
     *  @p(blist)
     *  - Swi_restorePri must only be called from a Swi context.
     *  @p
     *
     *  @param(key)     key returned from Swi_raisePri
     */
    Void restorePri(UInt key);

instance:

    /*!
     *  ======== create ========
     *  Create a software interrupt
     *
     *  Swi_create creates a new Swi object.
     *
     *  The following C code sets Swi parameters and
     *  creates two Swi objects:
     *
     *  @p(code)
     *  Void main()
     *  {
     *      Swi_Params swiParams;
     *
     *      Swi_Params_init(&swiParams);
     *      swiParams.arg0 = 1;
     *      swiParams.arg1 = 0;
     *      swiParams.priority = 2;
     *      swiParams.trigger = 0;
     *
     *      swi0 = Swi_create(swi0Fxn, &swiParams, NULL);
     *
     *      swiParams.arg0 = 2;
     *      swiParams.arg1 = 0;
     *      swiParams.priority = 1;
     *      swiParams.trigger = 3;
     *
     *      swi1 = Swi_create(swi1Fxn, &swiParams, NULL);
     *
     *      BIOS_start();
     *  }
     *  @p
     *
     *  The following XDCscript statements set Swi parameters and
     *  create two Swi objects:
     *
     *  @p(code)
     *  var Swi = xdc.useModule('ti.sysbios.knl.Swi');
     *
     *  var swiParams = new Swi.Params();
     *  swiParams.arg0 = 1;
     *  swiParams.arg1 = 0;
     *  swiParams.priority = 2;
     *  swiParams.trigger = 0;
     *  Program.global.swi0 = Swi.create('&swi0Fxn', swiParams);
     *
     *  swiParams.arg0 = 2;
     *  swiParams.priority = 1;
     *  swiParams.trigger = 3;
     *  Program.global.swi1 = Swi.create('&swi1Fxn', swiParams);
     *  @p
     *
     *  @param(swiFxn)     Swi Function
     */
    create(FuncPtr swiFxn);

    // -------- Handle Parameters --------

    /*!
     *  ======== arg0 ========
     *  Swi function argument 0
     *
     *  The default value of this optional parameter is 0.
     *
     *  @see #FuncPtr
     */
    config UArg arg0 = 0;

    /*!
     *  ======== arg1 ========
     *  Swi function argument 1
     *
     *  The default value of this optional parameter is 0.
     *
     *  @see #FuncPtr
     */
    config UArg arg1 = 0;

    /*!
     *  ======== priority ========
     *  Swi priority
     *
     *  Each software interrupt has a priority level, 0 to
     *  ({@link #numPriorities} - 1). A software interrupt
     *  preempts any lower-priority software interrupt currently executing.
     *  When multiple Swis of the same priority level have been posted,
     *  their respective Swi functions are executed in the order the Swis
     *  were posted.
     *
     *  The default value of this optional parameter is ~0, which yields a
     *  Swi with the highest priority: ({@link #numPriorities} - 1).
     */
    config UInt priority = ~0;

    /*!
     *  ======== trigger ========
     *  Initial Swi trigger value
     *
     *  The default value of this optional parameter is 0.
     *
     *  Each Swi object has a "trigger" used either to determine whether to
     *  post the Swi or as a value that can be evaluated within the Swi's
     *  function.
     *
     *  The {@link #andn} and {@link #dec} functions post the Swi
     *  if the trigger value transitions to 0. The {@link #or} and
     *  {@link #inc} functions also modify the trigger value. ({@link #or}
     *  sets bits, and {@link #andn} clears bits.)
     */
    config UInt trigger = 0;

    // -------- Handle Functions --------

    /*!
     *  ======== andn ========
     *  Clear bits in Swi's trigger; post if trigger becomes 0
     *
     *  Swi_andn is used to conditionally post a software interrupt.
     *  Swi_andn clears the bits specified by a mask from Swi's internal
     *  trigger. If the Swi's trigger becomes 0, Swi_andn posts the Swi.
     *  The bitwise logical operation performed is:
     *
     *  @p(code)
     *  trigger = trigger AND (NOT MASK)
     *  @p
     *
     *  If multiple conditions that all be met before a
     *  Swi can run, you should use a different bit in the trigger for
     *  each condition. When a condition is met, clear the bit for that
     *  condition.
     *
     *  For example, if two events must happen before a Swi is to be
     *  triggered, the initial trigger value of the Swi can be 3 (binary 0011).
     *  One call to Swi_andn can have a mask value of 2 (binary 0010), and
     *  another call to Swi_andn can have a mask value of 1 (binary 0001).
     *  After both calls have been made, the trigger value will be 0.
     *
     *  @p(code)
     *  Swi_andn(swi0, 2);  // clear bit 1
     *  Swi_andn(swi0, 1);  // clear bit 0
     *  @p
     *
     *  Swi_andn results in a context switch if the Swi's trigger becomes
     *  zero and the Swi has higher priority than the currently executing
     *  thread.
     *
     *  You specify a Swi's initial trigger value at Swi creation time.
     *  The trigger value is automatically reset when the Swi executes.
     *
     *  @a(constraints)
     *  The {@link #post} discussion regarding global interrupts applies
     *  to this API.
     *
     *  @param(mask)    inverse value to be ANDed
     */
    Void andn(UInt mask);

    /*!
     *  ======== dec ========
     *  Decrement Swi's trigger value; post if trigger becomes 0
     *
     *  Swi_dec is used to conditionally post a software interrupt. Swi_dec
     *  decrements the value in Swi's trigger by 1. If Swi's trigger value
     *  becomes 0, Swi_dec posts the Swi. You can increment a trigger value
     *  by using Swi_inc, which always posts the Swi.
     *
     *  For example, you would use Swi_dec if you wanted to post a Swi after
     *  a number of occurrences of an event.
     *
     *  @p(code)
     *  // swi0's trigger is configured to start at 3
     *  Swi_dec(swi0);      // trigger = 2
     *  Swi_dec(swi0);      // trigger = 1
     *  Swi_dec(swi0);      // trigger = 0
     *  @p
     *
     *  You specify a Swi's initial trigger value at Swi creation time. The
     *  trigger value is automatically reset when the Swi executes.
     *
     *  Swi_dec results in a context switch if the Swi's trigger becomes
     *  zero and the Swi has higher priority than the currently executing
     *  thread.
     *
     *  @a(constraints)
     *  The {@link #post} discussion regarding global interrupts applies
     *  to this API.
     */
    Void dec();

    /*!
     *  ======== getHookContext ========
     *  Get hook instance's context pointer for a Swi
     *
     *  For example, this C code gets the HookContext, prints it,
     *  and sets a new value for the HookContext.
     *
     *  @p(code)
     *  Ptr pEnv;
     *  Swi_Handle mySwi;
     *  Int myHookSetId1;
     *
     *  pEnv = Swi_getHookContext(swi, myHookSetId1);
     *
     *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
     *                (ULong)pEnv, (ULong)Timestamp_get32());
     *
     *  Swi_setHookContext(swi, myHookSetId1, (Ptr)0xc0de1);
     *  @p
     *
     *  See {@link #hookfunc Hook Functions} for more details.
     *
     *  @b(returns)     hook instance's context pointer for Swi
     */
    Ptr getHookContext(Int id);

    /*!
     *  ======== setHookContext ========
     *  Set hook instance's context for a swi
     *
     *  For example, this C code gets the HookContext, prints it,
     *  and sets a new value for the HookContext.
     *
     *  @p(code)
     *  Ptr pEnv;
     *  Swi_Handle mySwi;
     *  Int myHookSetId1;
     *
     *  pEnv = Swi_getHookContext(swi, myHookSetId1);
     *
     *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
     *                (ULong)pEnv, (ULong)Timestamp_get32());
     *
     *  Swi_setHookContext(swi, myHookSetId1, (Ptr)0xc0de1);
     *  @p
     *
     *  See {@link #hookfunc Hook Functions} for more details.
     *
     *  @param(id)              hook instance's ID
     *  @param(hookContext)     value to write to context
     */
    Void setHookContext(Int id, Ptr hookContext);

    /*!
     *  ======== getPri ========
     *  Return a Swi's priority
     *
     *  Swi_getPri returns the priority of the Swi passed in as the
     *  argument.
     *
     *  @b(returns)     Priority of Swi
     */
    UInt getPri();

    /*!
     *  ======== getFunc ========
     *  Get Swi function and arguments
     *
     *  If either arg0 or arg1 is NULL, then the corresponding argument is not
     *  returned.
     *
     *  @related {@link #getAttrs Swi_getAttrs()}
     *
     *  @param(arg0)     pointer for returning Swi's first function argument
     *  @param(arg1)     pointer for returning Swi's second function argument
     *
     *  @b(returns)     Swi function
     */
    FuncPtr getFunc(UArg *arg0, UArg *arg1);

    /*!
     *  ======== getAttrs ========
     *  Retrieve attributes of an existing Swi object.
     *
     *  The 'handle' argument specifies the address of the Swi object whose
     *  attributes are to be retrieved.
     *
     *  The 'swiFxn' argument is the address of a function pointer where the
     *  the Swi function address is to be written to. If NULL is passed for
     *  'swiFxn', no attempt is made to return the Swi function.
     *
     *  The 'params' argument is a pointer to a Swi_Params structure that will
     *  contain the retrieved Swi attributes.  If 'params' is NULL, no attempt
     *  is made to retrieve the Swi_Params.
     *
     *  @related {@link #setAttrs Swi_setAttrs()}
     *
     *  @param(swiFxn)     pointer to a Swi_FuncPtr
     *  @param(params)     pointer for returning Swi's Params
     */
    Void getAttrs(FuncPtr *swiFxn, Params *params);

    /*!
     *  ======== setAttrs ========
     *  Set the attributes of an existing Swi object.
     *
     *  The 'handle' argument specifies the address of the Swi object whose
     *  attributes are to be set.
     *
     *  The 'swiFxn' argument is the address of the function to be invoked
     *  when the Swi runs. If 'swiFxn' is NULL, no change is made to the Swi
     *  function.
     *
     *  The 'params' argument, which can be either NULL or a pointer to
     *  a Swi_Params structure that contains attributes for the
     *  Swi object, facilitates setting the attributes of the Swi object.
     *
     *  If 'params' is NULL, the Swi object is assigned a default set of
     *  attributes.
     *  Otherwise, the Swi object's attributes are set according the values
     *  passed within 'params'.
     *
     *  @Constraints
     *  Swi_setAttrs() must not be used on a Swi that is preempted
     *  or is ready to run.
     *
     *  @related {@link #getAttrs Swi_getAttrs()}
     *
     *  @param(swiFxn)     address of the Swi function
     *  @param(params)     pointer to optional Swi_Params structure
     */
    Void setAttrs(FuncPtr swiFxn, Params *params);

    /*!
     *  ======== setPri ========
     *  Set a Swi's priority
     *
     *  Swi_setPri sets the priority of the Swi passed in as the
     *  argument.
     *
     *  @a(constraints)
     *  The priority must be in the range of 0 and numPriorities-1.
     *
     *  @a(constraints)
     *  Swi_setPri() must not be used on a Swi that is preempted
     *  or is ready to run.
     *
     *  @related {@link #getPri Swi_setAttrs()}
     *
     *  @param(priority)     priority of Swi
     */
    Void setPri(UInt priority);

    /*!
     *  ======== inc ========
     *  Increment Swi's trigger value and post the Swi
     *
     *  Swi_inc increments the value in Swi's trigger by 1 and posts the Swi
     *  regardless of the resulting trigger value. You can decrement a
     *  trigger value using Swi_dec, which only posts the Swi if the
     *  trigger value is 0.
     *
     *  If a Swi is posted several times before it has a chance to begin
     *  executing (i.e. when Hwis or higher priority Swis are running) the Swi
     *  only runs one time. If this situation occurs, you can use Swi_inc to
     *  post the Swi. Within the Swi's function, you could then use
     *  Swi_getTrigger to find out how many times this Swi has been posted
     *  since the last time it was executed.
     *
     *  You specify a Swi's initial trigger value at Swi creation time.
     *  The trigger value is automatically reset when the Swi executes.
     *  To get the trigger value, use Swi_getTrigger.
     *
     *  Swi_inc results in a context switch if the Swi is higher priority
     *  than the currently executing thread.
     *
     *  @a(constraints)
     *  The {@link #post} discussion regarding global interrupts applies
     *  to this API.
     */
    Void inc();

    /*!
     *  ======== or ========
     *  Or mask with value contained in Swi's trigger and post the
     *  Swi.
     *
     *  Swi_or is used to post a software interrupt. Swi_or sets the bits
     *  specified by a mask in Swi's trigger. Swi_or posts the Swi
     *  regardless of the resulting trigger value. The bitwise logical
     *  operation performed on the trigger value is:
     *
     *  @p(code)
     *  trigger = trigger OR mask
     *  @p
     *
     *  You specify a Swi's initial trigger value at Swi creation time.
     *  The trigger value is automatically reset when the Swi executes.
     *  To get the trigger value, use Swi_getTrigger.
     *
     *  For example, you might use Swi_or to post a Swi if any of three
     *  events should cause a Swi to be executed, but you want the Swi's
     *  function to be able to tell which event occurred. Each event
     *  would correspond to a different bit in the trigger.
     *
     *  Swi_or results in a context switch if the Swi is higher priority
     *  than the currently executing thread.
     *
     *  @a(constraints)
     *  The {@link #post} discussion regarding global interrupts applies
     *  to this API.
     *
     *  @param(mask)    value to be ORed
     */
    Void or(UInt mask);

    /*!
     *  ======== post ========
     *  Unconditionally post a software interrupt
     *
     *  Swi_post is used to post a software interrupt regardless of the
     *  trigger value. No change is made to the Swi object's trigger value.
     *
     *  Swi_post results in a context switch if the Swi is higher priority
     *  than the currently executing thread.
     *
     *  @a(constraints)
     *  Swis are ALWAYS run with interrupts enabled.
     *  If a Swi is made ready to run as a consequence of this
     *  API, interrupts will be globally enabled while the Swi function
     *  executes, regardless of the prior globally enabled/disabled
     *  state of interrupts.
     *  Upon return from this API, the global interrupt enabled/disabled state
     *  is restored to its previous value.
     */
    Void post();

internal:   /* not for client use */

    /*
     *  ======== taskDisable ========
     *  Swi and Task module function pointers
     *
     *  Used to decouple Hwi from Swi and Task when
     *  dispatcherSwiSupport or dispatcherTaskSupport is false.
     */
    config UInt (*taskDisable)();
    config Void (*taskRestore)(UInt);

    /*
     *  ======== schedule ========
     *  Run the highest priority Swi
     *
     *  Called by Swi_restore() which is used
     *  in a task context.
     *
     *  Must be called with interrupts disabled.
     */
    Void schedule();

    /*
     *  ======== runLoop ========
     *  Calls Swi_run in a loop
     *
     *  Called by Swi_schedule() via a local volatile function pointer.
     */
    Void runLoop();

    /*
     *  ======== run ========
     *  Set up and run Swi
     *
     *  Enter with Hwi's disabled.
     *  Exits with Hwi's enabled
     */
    Void run(Object *swi);

    /*
     *  ======== postInit ========
     *  Finish initializing static and dynamic Swis
     */
    Int postInit(Object *swi, Error.Block *eb);

    /*
     *  ======== restoreSMP ========
     *  Swi restore invoked when core != 0 and swiKey == false.
     */
    Void restoreSMP();

    /*!
     *  ======== numConstructedSwis ========
     *  Number of statically constructed Swi objects
     *
     *  @_nodoc
     *  Shouldn't be set directly by the user's
     *  config (it gets set by instance$static$init).
     */
    config UInt numConstructedSwis = 0;

    /*!
     *  ======== Instance_State ========
     *  @_nodoc
     */
    struct Instance_State {
        Queue.Elem      qElem;      // Link within readyQ
        FuncPtr         fxn;        // Swi function
        UArg            arg0;       // Swi function 1st arg
        UArg            arg1;       // Swi function 2nd arg
        UInt            priority;   // Swi priority
        UInt            mask;       // handy curSet orMask (= 1 << priority)
        Bool            posted;     // TRUE = Swi already posted.
        UInt            initTrigger;// Initial Trigger value
        UInt            trigger;    // Swi Trigger
        Queue.Handle    readyQ;     // This Swi's readyQ
        Ptr             hookEnv[];
    };

    /*!
     *  ======== Module_State ========
     *  @_nodoc
     */
    struct Module_State {
        volatile Bool   locked;     // TRUE = Swi scheduler locked
        UInt            curSet;     // Bitmask reflects readyQ states
        UInt            curTrigger; // current Swi's on-entry trigger
        Handle          curSwi;     // current Swi instance
        Queue.Handle    curQ;       // current Swi's readyQ,
                                    // when all posted Swis have run
        Queue.Object    readyQ[];   // Swi ready queues
        Handle          constructedSwis[]; // array of statically
                                    // constructed Swis
    };
}
