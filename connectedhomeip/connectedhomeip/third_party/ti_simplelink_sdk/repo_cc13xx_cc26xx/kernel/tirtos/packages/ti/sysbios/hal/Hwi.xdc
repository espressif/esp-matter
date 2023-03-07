/*
 * Copyright (c) 2015-2017, Texas Instruments Incorporated
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
 *  ======== Hwi.xdc ========
 *
 */
package ti.sysbios.hal;

import xdc.runtime.Error;

/*!
 *  ======== Hwi ========
 *  Hardware Interrupt Manager Proxy.
 *
 *  This module provides APIs for managing hardware interrupts.
 *  These APIs are generic across all supported targets and devices
 *  and should provide sufficient functionality for most applications.
 *
 *  The actual implementations of the Hwi module APIs are
 *  provided by the Hwi module delegates.
 *  Additional, family-specific Hwi module APIs may also be provided by
 *  the Hwi module delegates.
 *  See the list of
 *  {@link ./../family/doc-files/delegates.html Delegate Mappings}
 *  to determine which Hwi delegate is used
 *  for your target/device.
 *
 *  You can statically or dynamically assign functions that run when
 *  specific hardware interrupts occur. Dynamic assignment of Hwi
 *  functions to interrupts at run-time is done
 *  using the {@link #create Hwi_create} function.
 *
 *  Interrupt routines can be written completely in C, completely in
 *  assembly, or in a mix of C and assembly. In order to support interrupt
 *  routines
 *  written completely in C, an interrupt dispatcher is provided that performs
 *  the requisite prolog and epilog for an interrupt routine.
 *
 *  Some routines are assigned to interrupts by the other SYS/BIOS
 *  modules. For example, the
 *  {@link ti.sysbios.knl.Clock} module configures its own timer interrupt
 *  handler.
 *
 *  @a(constraints)
 *  Since the hal Hwi module has no knowledge of the delegate Hwi
 *  module's instance definition, Hwi_construct() can NOT be properly
 *  supported.
 *
 *  If {@link ti.sysbios.BIOS#runtimeCreatesEnabled BIOS.runtimeCreatesEnabled}
 *  is set to true, both Hwi_create() and Hwi_construct()
 *  will attempt to dynamically create (ie NOT construct) a delegate Hwi
 *  object.
 *
 *  If {@link ti.sysbios.BIOS#runtimeCreatesEnabled BIOS.runtimeCreatesEnabled}
 *  is set to false, both Hwi_create() and Hwi_construct() will fail.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                         -->
 *    <tr><td> {@link #clearInterrupt}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #create}           </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #disable}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #disableInterrupt} </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #enable}           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #enableInterrupt}  </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #restore}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #restoreInterrupt} </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}        </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}           </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}         </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getHookContext}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setFunc}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setHookContext}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. Hwi_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. Hwi_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 *
 *  @a(Runtime Hwi Creation)
 *
 *  Below is an example that configures an interrupt at runtime.
 *  Typically such code would be placed in main().
 *
 *  @p(code)
 *  #include <xdc/runtime/Error.h>
 *  #include <ti/sysbios/hal/Hwi.h>
 *
 *  Hwi_Handle myHwi;
 *
 *  Int main(Int argc, char* argv[])
 *  {
 *      Hwi_Params hwiParams;
 *      Error_Block eb;
 *
 *      Hwi_Params_init(&hwiParams);
 *      Error_init(&eb);
 *
 *      // set the argument you want passed to your ISR function
 *      hwiParams.arg = 1;
 *
 *      // set the event id of the peripheral assigned to this interrupt
 *      hwiParams.eventId = 10;
 *
 *      // don't allow this interrupt to nest itself
 *      hwiParams.maskSetting = Hwi_MaskingOption_SELF;
 *
 *      //
 *      // Configure interrupt 5 to invoke "myIsr".
 *      // Automatically enables interrupt 5 by default
 *      // set params.enableInt = FALSE if you want to control
 *      // when the interrupt is enabled using Hwi_enableInterrupt()
 *      //
 *
 *      myHwi = Hwi_create(5, myIsr, &hwiParams, &eb);
 *
 *      if (Error_check(&eb)) {
 *          // handle the error
 *      }
 *  }
 *
 *  Void myIsr(UArg arg)
 *  {
 *      // this runs when interrupt #5 goes off
 *  }
 *  @p
 *
 *  @a(Hook Functions)
 *
 *  Sets of hook functions can be specified for the Hwi module
 *  using the configuration tool.  Each set contains these hook
 *  functions:
 *  @p(blist)
 *  -Register:  A function called before any statically-created Hwis
 *      are initialized at runtime.  The register hook is called at boot time
 *      before main() and before interrupts are enabled.
 *  -Create:    A function that is called when a Hwi is created.
 *      This includes hwis that are created statically and those
 *      created dynamically using {@link #create Hwi_create}.
 *  -Begin:     A function that is called just prior to running a Hwi.
 *  -End:       A function that is called just after a Hwi finishes.
 *  -Delete:    A function that is called when a Hwi is deleted at
 *      run-time with {@link #delete Hwi_delete}.
 *  @p
 *
 *  Register Function
 *
 *  The Register function is provided to allow a hook set to store its
 *  hookset ID.  This id can be passed to
 *  {@link #setHookContext Hwi_setHookContext} and
 *  {@link #getHookContext Hwi_getHookContext} to set or get
 *  hookset-specific context.  The
 *  Register function must be specified if the hook implementation
 *  needs to use {@link #setHookContext  Hwi_setHookContext} or
 *  {@link #getHookContext  Hwi_getHookContext}.
 *  The registerFxn hook function is called during system initialization
 *  before interrupts have been enabled.
 *
 *  @p(code)
 *  Void myRegisterFxn(Int id);
 *  @p
 *
 *  Create and Delete Functions
 *
 *  The create and delete functions are called whenever a Hwi is created
 *  or deleted.  They are called with interrupts enabled (unless called
 *  at boot time or from main()).
 *
 *  @p(code)
 *  Void myCreateFxn(Hwi_Handle hwi, Error_Block *eb);
 *  @p
 *
 *  @p(code)
 *  Void myDeleteFxn(Hwi_Handle hwi);
 *  @p
 *
 *  Begin and End Functions
 *
 *  The beginFxn and endFxn function hooks are called with interrupts
 *  globally disabled, therefore any hook processing function will contribute
 *  to the overall system interrupt response latency.  In order to minimize
 *  this impact, carefully consider the processing time spent in an Hwi
 *  beginFxn or endFxn function hook.
 *
 *  @p(code)
 *  Void myBeginFxn(Hwi_Handle hwi);
 *  @p
 *
 *  @p(code)
 *  Void myEndFxn(Hwi_Handle hwi);
 *  @p
 *
 *  Hook functions can only be configured statically.
 *
 */

@ModuleStartup      /* generate a call to startup function */

module Hwi inherits ti.sysbios.interfaces.IHwi
{
    /*!
     *  Error raised when a stack overflow (or corruption) is detected.
     *
     *  This error is raised by kernel's stack checking function.  This
     *  function checks the stacks before every task switch to make sure
     *  that reserved word at top of stack has not been modified.
     *
     *  The stack checking logic is enabled by the {@link #checkStackFlag}
     *  configuration parameter. If this flag is set to true, the kernel will
     *  validate the stacks.
     */
    config Error.Id E_stackOverflow  = {
        msg: "E_stackOverflow: ISR stack overflow."
    };

    /*!
     *  Initialize ISR stack with known value for stack checking at runtime
     *
     *  This is also useful for inspection of stack in debugger or core
     *  dump utilities for stack overflow and depth.
     *
     *  Default is true.
     *  (see {@link #checkStackFlag}).
     */
    metaonly config Bool initStackFlag = true;

    /*!
     *  Check for Hwi stack overrun during Idle loop.
     *
     *  If true, then an idle function is added to the idle loop
     *  that checks for a Hwi stack overrun condition and raises
     *  an Error if one is detected.
     *
     *  The check consists of testing the top of stack value against
     *  its initial value (see {@link #initStackFlag}). If it is no
     *  longer at this value, the assumption is that the ISR stack
     *  has been overrun. If the test fails, then the
     *  {@link #E_stackOverflow} error is raised.
     *
     *  Runtime stack depth computation is only performed if {@link #initStackFlag} is
     *  also true.
     *
     *  Default is true.
     *  (see {@link #initStackFlag}).
     *
     *  To enable or disable full stack checking, you should set both this
     *  flag and the {@link ti.sysbios.knl.Task#checkStackFlag}.
     */
    metaonly config Bool checkStackFlag = true;

    /*!
     *  ======== disable ========
     *  Globally disable interrupts.
     *
     *  Hwi_disable globally disables hardware interrupts and returns an
     *  opaque key indicating whether interrupts were globally enabled or
     *  disabled on entry to Hwi_disable().
     *  The actual value of the key is target/device specific and is meant
     *  to be passed to Hwi_restore().
     *
     *  Call Hwi_disable before a portion of a function that needs
     *  to run without interruption. When critical processing is complete, call
     *  Hwi_restore or Hwi_enable to reenable hardware interrupts.
     *
     *  Servicing of interrupts that occur while interrupts are disabled is
     *  postponed until interrupts are reenabled. However, if the same type
     *  of interrupt occurs several times while interrupts are disabled,
     *  the interrupt's function is executed only once when interrupts are
     *  reenabled.
     *
     *  A context switch can occur when calling Hwi_enable or Hwi_restore if
     *  an enabled interrupt occurred while interrupts are disabled.
     *
     *  Hwi_disable may be called from main(). However, since Hwi interrupts
     *  are already disabled in main(), such a call has no effect.
     *
     *  @a(constraints)
     *  If a Task switching API such as
     *  {@link ti.sysbios.knl.Semaphore#pend Semaphore_pend()},
     *  {@link ti.sysbios.knl.Semaphore#post Semaphore_post()},
     *  {@link ti.sysbios.knl.Task#sleep Task_sleep()}, or
     *  {@link ti.sysbios.knl.Task#yield Task_yield()}
     *  is invoked which results in a context switch while
     *  interrupts are disabled, an embedded call to
     *  {@link #enable Hwi_enable} occurs
     *  on the way to the new thread context which unconditionally re-enables
     *  interrupts. Interrupts will remain enabled until a subsequent
     *  {@link #disable Hwi_disable}
     *  invocation.
     *
     *  Swis always run with interrupts enabled.
     *  See {@link ti.sysbios.knl.Swi#post Swi_post()} for a discussion Swis and
     *  interrupts.
     *
     *  @b(returns)     opaque key for use by Hwi_restore()
     */
    @Macro
    override UInt disable();

    /*!
     *  ======== enable ========
     */
    @Macro
    override UInt enable();

    /*!
     *  ======== restore ========
     */
    @Macro
    override Void restore(UInt key);

    /*!
     *  ======== viewGetHandle ========
     *  @_nodoc
     *  Returns the corresponding hal Hwi handle for a delegate Hwi handle
     *
     *  @b(returns)     hal Hwi handle
     */
    metaonly Handle viewGetHandle(Ptr pi);

    /*!
     *  ======== viewGetLabel ========
     *  @_nodoc
     *  Returns the corresponding hal Hwi label for a delegate Hwi handle
     *
     *  @b(returns)     hal Hwi Label
     */
    metaonly String viewGetLabel(Ptr pi);

instance:

    /*!
     *  ======== create ========
     *  Create a dispatched interrupt.
     *
     *  To cause a C function to run in response to a particular system
     *  interrupt, you create a Hwi object that encapsulates information
     *  regarding the interrupt required by the Hwi module.
     *
     *  The standard static and dynamic forms of the "create" function are
     *  supported by the ti.sysbios.hal.Hwi module.
     *  The following C code configures interrupt 5 with the "myIsr"
     *  C function.
     *
     *  @p(code)
     *  #include <ti/sysbios/hal/Hwi>
     *
     *  Hwi_create(5, myIsr, NULL, NULL);
     *  @p
     *
     *  The NULL, NULL arguments are used when the default instance
     *  parameters and generic error handling is satisfactory for creating
     *  a Hwi object.
     *
     *  A Hwi dispatcher table entry is created and filled with the
     *  function specified by the fxn parameter and the attributes
     *  specified by the params parameter.
     *
     *  If params is NULL, the Hwi's dispatcher properties are assigned a
     *  default set of values. Otherwise, the following properties
     *  are specified by a structure of type Hwi_Params.
     *
     *  @p(blist)
     *  - The arg element is a generic argument that is passed to the plugged
     *  function as its only parameter. The default value is 0.
     *  - The enableInt element determines whether the interrupt should be
     *  enabled in the IER by create.
     *  - The maskSetting element defines the dispatcherAutoNestingSupport
     *  behavior of the interrupt.
     *  @p
     *
     *  Hwi_create returns a pointer to the created Hwi object.
     *
     *  @param(intNum)  interrupt number
     *  @param(hwiFxn)  pointer to ISR function
     */
    override create(Int intNum, FuncPtr hwiFxn);

    /*!
     *  ======== getHookContext ========
     *  Get hook instance's context for a Hwi.
     *
     *  The Handle passed to this API must be the handle passed
     *  to any of the Hook functions, not the one returned by
     *  {@link #create Hwi_create}.
     *
     *  @b(returns)     hook instance's context for hwi
     */
    override Ptr getHookContext(Int id);

    /*!
     *  ======== setHookContext ========
     *  Set hook instance's context for a Hwi.
     *
     *  The Handle passed to this API must be the handle passed
     *  to any of the Hook functions, not the one returned by
     *  {@link #create Hwi_create}.
     *
     *  @param(id)            hook instance's ID
     *  @param(hookContext)   value to write to context
     */
    override Void setHookContext(Int id, Ptr hookContext);

internal:   /* not for client use */

    /* keep track of the number of hooks defined */
    metaonly config UInt numHooks = 0;

    /*! target/device-specific Hwi implementation. */
    proxy HwiProxy inherits ti.sysbios.interfaces.IHwi;

    struct Instance_State {
        HwiProxy.Handle pi;
    };
}
