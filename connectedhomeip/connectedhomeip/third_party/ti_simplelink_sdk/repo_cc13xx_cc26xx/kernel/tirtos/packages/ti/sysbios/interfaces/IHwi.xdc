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
 *  ======== IHwi.xdc ========
 *
 */

import xdc.runtime.Error;


/*!
 *  ======== Hwi ========
 *  Hardware Interrupt Support Module.
 *
 *  The IHwi interface specifies APIs for globally enabling, disabling, and
 *  restoring interrupts.
 *
 *  Additionally, management of individual, device-specific hardware 
 *  interrupts is provided.
 *
 *  The user can statically or dynamically assign routines that run when 
 *  specific hardware interrupts occur. 
 *
 *  Dynamic assignment of Hwi routines to interrupts at run-time is done 
 *  using the Hwi_create function.
 *
 *  Interrupt routines can be written completely in C, completely in 
 *  assembly, or in a mix of C and assembly. In order to support interrupt
 *  routines
 *  written completely in C, an interrupt dispatcher is provided that performs
 *  the requisite prolog and epilog for an interrupt routine.
 *
 *  Some routines are assigned to interrupts by the other SYS/BIOS
 *  modules. For example, the Clock module configures its own timer interrupt
 *  handler. See the Clock Module for more details.
 *
 *  @a(Runtime Hwi Creation)
 *
 *  Below is an example of configuring an interrupt at runtime. 
 *  Usually this code would be placed in main().
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
 *      // here when interrupt #5 goes off
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
 */

@DirectCall
@InstanceFinalize
@InstanceInitError

interface IHwi {

    // -------- Module Types --------

    /*! Hwi create function type definition. */
    typedef Void (*FuncPtr)(UArg);
    
    /*! 
     * Interrupt Return Pointer.
     *
     * This is the address of the interrupted instruction.
     */
    typedef UArg Irp;
   
    /*! 
     *  Hwi hook set type definition. 
     *
     *  The functions that make up a hookSet have certain restrictions. They
     *  cannot call any Hwi instance functions other than Hwi_getHookContext()
     *  and Hwi_setHookContext(). For all practical purposes, they should treat
     *  the Hwi_Handle passed to these functions as an opaque handle.
     */
    struct HookSet {
        Void (*registerFxn)(Int);
        Void (*createFxn)(Handle, Error.Block *);
        Void (*beginFxn)(Handle);
        Void (*endFxn)(Handle);
        Void (*deleteFxn)(Handle);
    };
    
    /*!
     *  ======== MaskingOption ========
     *  Shorthand interrupt masking options
     *
     *  @value(MaskingOption_NONE)      No interrupts are disabled
     *
     *  @value(MaskingOption_ALL)       All interrupts are disabled
     *
     *  @value(MaskingOption_SELF)      Only this interrupt is disabled
     *
     *  @value(MaskingOption_BITMASK)   User supplies interrupt enable masks
     *
     *  @value(MaskingOption_LOWER)     All current and lower priority
     *                                  interrupts are disabled.
     *
     *                                  Only a few targets/devices truly
     *                                  support this masking option. For those
     *                                  that don't, this setting is treated
     *                                  the same as MaskingOption_SELF.
     */
    enum MaskingOption {
        MaskingOption_NONE,
        MaskingOption_ALL, 
        MaskingOption_SELF,
        MaskingOption_BITMASK,
        MaskingOption_LOWER
    };

    /*!
     *  ======== StackInfo ========
     *  Structure contains Hwi stack usage info 
     *
     *  Used by getStackInfo() and viewGetStackInfo() functions
     */
    struct StackInfo {
        SizeT hwiStackPeak;
        SizeT hwiStackSize;
        Ptr hwiStackBase;
    };

    // -------- Module Parameters --------

    /*!
     *  Include interrupt nesting logic in interrupt dispatcher?
     *
     *  Default is true.
     *
     *  This option provides the user with the ability to optimize
     *  interrupt dispatcher performance when support for interrupt
     *  nesting is not required.
     *
     *  Setting this parameter to false will disable the logic in
     *  the interrupt dispatcher that manipulates interrupt mask
     *  registers and enables and disables interrupts before and
     *  after invoking the user's Hwi function. 
     *
     *  Set this parameter to false if you don't need interrupts 
     *  enabled during the execution of your Hwi functions.
     */
    config Bool dispatcherAutoNestingSupport = true;

    /*!
     *  Include Swi scheduling logic in interrupt dispatcher?
     *
     *  Default is inherited from {@link ti.sysbios.BIOS#swiEnabled 
     *  BIOS.swiEnabled}, which is true by default.
     *
     *  This option provides the user with the ability to optimize
     *  interrupt dispatcher performance when it is known that Swis
     *  will not be posted from any of their Hwi threads.
     *
     *  @a(Warning)
     *  Setting this parameter to false will disable the logic in
     *  the interrupt dispatcher that invokes the Swi scheduler
     *  prior to returning from an interrupt.
     *  With this setting, Swis MUST NOT be posted from Hwi functions!
     */
    config Bool dispatcherSwiSupport;

    /*!
     *  Include Task scheduling logic in interrupt dispatcher?
     *
     *  Default is inherited from {@link ti.sysbios.BIOS#taskEnabled 
     *  BIOS.taskEnabled}, which is true by default.
     *
     *  This option provides the user with the ability to optimize
     *  interrupt dispatcher performance when it is known that no
     *  Task scheduling APIs (ie {@link ti.sysbios.knl.Semaphore#post 
     *  Semaphore_post()}) will be executed from any of their Hwi threads.
     *
     *  Setting this parameter to false will disable the logic in
     *  the interrupt dispatcher that invokes the Task scheduler
     *  prior to returning from an interrupt.
     */
    config Bool dispatcherTaskSupport;

    /*!
     *  Controls whether the
     *  dispatcher retains the interrupted thread's return address.
     *
     *  This option is enabled by default.
     *
     *  Setting this parameter to false will disable the logic in
     *  the interrupt dispatcher that keeps track of the interrupt's
     *  return address and provide a small savings in interrupt latency.
     *
     *  The application can get an interrupt's most recent return
     *  address using the {@link #getIrp} API.
     */
    config Bool dispatcherIrpTrackingSupport = true;

    // -------- Module Functions --------

    /*!
     *  ======== addHookSet ========
     *  addHookSet is used in a config file to add a hook set (defined
     *  by struct HookSet).
     *
     *  HookSet structure elements may be omitted, in which case those
     *  elements will not exist.
     *
     *  @param(hook)    structure of type HookSet
     */
    metaonly Void addHookSet(HookSet hook);

    /*!
     *  ======== viewGetStackInfo ========
     *  @_nodoc
     *  Returns the Hwi stack usage info. Used at ROV time.
     *
     *  @b(returns)     Hwi stack base, size, peak
     */
    metaonly StackInfo viewGetStackInfo();

    /*!
     *  ======== getStackInfo ========
     *  Get Hwi stack usage Info.
     *
     *  getStackInfo returns the Hwi stack usage info to its calling 
     *  function by filling stack base address, stack size and stack
     *  peak fields in the {@link #StackInfo} structure.
     *
     *  getStackInfo accepts two arguments, a pointer to a structure
     *  of type {@link #StackInfo} and a boolean. If the boolean is set
     *  to true, the function computes the stack depth and fills the 
     *  stack peak field in the StackInfo structure. If a stack overflow
     *  is detected, the stack depth is not computed. If the boolean is 
     *  set to false, the function only checks for a stack overflow.
     *
     *  The isr stack is always checked for an overflow and a boolean
     *  is returned to indicate whether an overflow occured.
     *
     *  Below is an example of calling getStackInfo() API:
     *
     *  @p(code)
     *  #include <ti/sysbios/BIOS.h>
     *  #include <ti/sysbios/hal/Hwi.h>
     *  #include <ti/sysbios/knl/Swi.h>
     *  #include <ti/sysbios/knl/Task.h>
     *
     *  Swi_Handle swi0;
     *  volatile Bool swiStackOverflow = FALSE;
     *
     *  Void swi0Fxn(UArg arg1, UArg arg2)
     *  {
     *      Hwi_StackInfo stkInfo;
     *
     *      // Request stack depth
     *      swiStackOverflow = Hwi_getStackInfo(&stkInfo, TRUE);
     * 
     *      // Alternately, we can omit the request for stack depth and 
     *      // request only the stack base and stack size (the check for
     *      // stack overflow is always performed):
     *      //
     *      // swiStackOverflow = Hwi_getStackInfo(&stkInfo, FALSE);
     *
     *      if (swiStackOverflow) {
     *          // isr Stack Overflow detected
     *      }
     *  }
     *
     *  Void idleTask()
     *  {
     *      Swi_post(swi0);
     *  }
     *
     *  Int main(Int argc, char* argv[])
     *  {
     *      swi0 = Swi_create(swi0Fxn, NULL, NULL);
     *
     *      BIOS_start();
     *      return (0);
     *  }
     *  @p
     *
     *  @param(stkInfo) pointer to structure of type {@link #StackInfo}
     *  @param(computeStackDepth)       decides whether to compute stack depth
     *
     *  @b(returns)     boolean to indicate a stack overflow
     */
    Bool getStackInfo(StackInfo *stkInfo, Bool computeStackDepth);

    /*!
     *  ======== getCoreStackInfo ========
     *  Get Hwi stack usage Info for the specified coreId.
     *
     *  getCoreStackInfo returns the Hwi stack usage info for the specified
     *  coreId to its calling function by filling stack base address,
     *  stack size and stack peak fields in the {@link #StackInfo} structure.
     *
     *  This function should be used only in applications built with
     *  {@link ti.sysbios.BIOS#smpEnabled} set to true.
     *
     *  getCoreStackInfo accepts three arguments, a pointer to a structure
     *  of type {@link #StackInfo}, a boolean and a coreId. If the boolean
     *  is set to true, the function computes the stack depth and fills the
     *  stack peak field in the StackInfo structure. If a stack overflow
     *  is detected, the stack depth is not computed. If the boolean is
     *  set to false, the function only checks for a stack overflow.
     *
     *  The isr stack is always checked for an overflow and a boolean
     *  is returned to indicate whether an overflow occured.
     *
     *  Below is an example of calling getCoreStackInfo() API:
     *
     *  @p(code)
     *  #include <ti/sysbios/BIOS.h>
     *  #include <ti/sysbios/hal/Hwi.h>
     *  #include <ti/sysbios/hal/Core.h>
     *  #include <ti/sysbios/knl/Task.h>
     *
     *  ...
     *
     *  Void idleTask()
     *  {
     *      UInt idx;
     *      Hwi_StackInfo stkInfo;
     *      Bool stackOverflow = FALSE;
     *
     *      // Request stack depth for each core's Hwi stack and check for
     *      // overflow
     *      for (idx = 0; idx < Core_numCores; idx++) {
     *          stackOverflow = Hwi_getCoreStackInfo(&stkInfo, TRUE, idx);
     *
     *          // Alternately, we can omit the request for stack depth and
     *          // request only the stack base and stack size (the check for
     *          // stack overflow is always performed):
     *          //
     *          // stackOverflow = Hwi_getCoreStackInfo(&stkInfo, FALSE, idx);
     *
     *          if (stackOverflow) {
     *              // isr Stack Overflow detected
     *          }
     *      }
     *  }
     *
     *  Int main(Int argc, char* argv[])
     *  {
     *      ...
     *      BIOS_start();
     *      return (0);
     *  }
     *  @p
     *
     *  @param(stkInfo)     pointer to structure of type {@link #StackInfo}
     *  @param(computeStackDepth)       decides whether to compute stack depth
     *  @param(coreId)      core whose stack info needs to be retrieved
     *
     *  @b(returns)         boolean to indicate a stack overflow
     */
    Bool getCoreStackInfo(StackInfo *stkInfo, Bool computeStackDepth,
        UInt coreId);

    /*!
     *  ======== startup ========
     *  Initially enable interrupts
     *
     *  Called within BIOS_start
     */
    Void startup();

    /*!
     *  ======== disable ========
     */
    UInt disable();

    /*!
     *  ======== enable ========
     *  Globally enable interrupts.
     *
     *  Hwi_enable globally enables hardware interrupts and returns an
     *  opaque key indicating whether interrupts were globally enabled or
     *  disabled on entry to Hwi_enable(). 
     *  The actual value of the key is target/device specific and is meant 
     *  to be passed to Hwi_restore(). 
     *
     *
     *  This function is 
     *  called as part of SYS/BIOS Startup_POST_APP_MAIN phase.
     *
     *  Hardware interrupts are enabled unless a call to Hwi_disable disables
     *  them. 
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
     *  Any call to Hwi_enable enables interrupts, even if Hwi_disable has 
     *  been called several times.
     *
     *  Hwi_enable must not be called from main().
     *
     *  @b(returns)     opaque key for use by Hwi_restore()
     */
    UInt enable();

    /*!
     *  ======== restore ========
     *  Globally restore interrupts.
     *
     *  Hwi_restore globally restores interrupts to the state determined 
     *  by the key argument provided by a previous invocation of Hwi_disable.
     *
     *  A context switch may occur when calling Hwi_restore if Hwi_restore
     *  reenables interrupts and another Hwi occurred while interrupts were 
     *  disabled.
     *
     *  Hwi_restore may be called from main(). However, since Hwi_enable
     *  cannot be called from main(), interrupts are always disabled in 
     *  main(), and a call to Hwi_restore has no effect.
     *
     *  @param(key)     enable/disable state to restore
     */
    Void restore(UInt key);

    /*!
     *  @_nodoc
     *  ======== switchFromBootStack ========
     *  Indicate that we are leaving the boot stack and
     *  are about to switch to a task stack.
     *  Used by Task_startup()
     */
    Void switchFromBootStack();

    /*!
     *  ======== post ========
     *  Generate an interrupt for test purposes.
     *
     *  @param(intNum)      ID of interrupt to generate
     */
    Void post(UInt intNum);

    /*!
     *  @_nodoc
     *  ======== getTaskSP ========
     *  retrieve interrupted task's SP
     *
     *  Used for benchmarking the SYS/BIOS Hwi dispatcher's task 
     *  stack utilization.
     *
     *  @b(returns)     interrupted task's SP
     */
    Char *getTaskSP();

    /*
     *  @_nodoc
     *  The following two target-unique Hwi APIs must be called
     *  directly in order to work properly. Thus they are not
     *  published here in order to bypass the multi-layered indirect function
     *  calls (__E, __F) that would arise if they appeared in this spec file.
     *
     *  These APIs must be implemented by the target Hwi modules and must be
     *  given these EXACT names.
     *
     *  The two functions, switchToIsrStack() and switchToTaskStack() must
     *  work in tandem to insure that only the first order (ie non nested) 
     *  invocation of these APIs result in the switch to the ISR stack and 
     *  the switch back to the task stack. The opaque char * token returned 
     *  by switchToIsrStack() and passed to switchToTaskStack() is provided
     *  purely for implementation efficiency and thus can have implementation
     *  dependent definitions.
     */

    /*
     *  @_nodoc
     *  ======== switchAndRunFunc ========
     *  If not on ISR stack already, switch to it, then call
     *  the function whose address is passed as an argument
     *  and then switch back to Task stack.
     *
     *  Used by the Swi scheduler.
     *
     *  This function must be implemented by all Hwi modules
     *  name) because it can't be _E and _F'd due to its
     *  inherent stack switching behavior.
     *
     *  @a(param)       Function pointer
     */
    /*  Char *ti_bios_family_xxx_Hwi_switchAndRunFunc(); */

    /*
     *  @_nodoc
     *  ======== switchToIsrStack ========
     *  If not on ISR stack already, switch to it.
     *  Used by the Swi scheduler and interrupt dispatcher.
     *
     *  This function must be implemented by all Hwi modules
     *  name) because it can't be _E and _F'd due to its
     *  inherent stack switching behavior.
     *  
     *  @b(returns)     token to use with 
     *                  switchToTaskStack()
     */
    /*  Char *ti_bios_family_xxx_Hwi_switchToIsrStack(); */

    /*
     *  @_nodoc
     *  ======== switchToTaskStack ========
     *  If at bottom of ISR stack, switch to Task stack.
     *  Used by the Swi scheduler and interrupt dispatcher.
     *  
     *  This function must be implemented by all Hwi modules
     *  and be given this exact name (without a target-specific
     *  name) because it can't be _E and _F'd due to its
     *  inherent stack switching behavior.
     *  
     *  @param(key)     token returned by
     *                  switchToIsrStack()
     */
    /*  Void ti_bios_family_xxx_Hwi_switchToTaskStack(Char *key); */

    /*!
     *  ======== disableInterrupt ========
     *  Disable a specific interrupt.
     *
     *  Disable a specific interrupt identified by an interrupt number.
     *
     *  @param(intNum)  interrupt number to disable
     *  @b(returns)     key to restore previous enable/disable state
     */
    UInt disableInterrupt(UInt intNum);

    /*!
     *  ======== enableInterrupt ========
     *  Enable a specific interrupt.
     *
     *  Enables a specific interrupt identified by an interrupt number.
     *
     *  @param(intNum)  interrupt number to enable
     *  @b(returns)     key to restore previous enable/disable state
     */
    UInt enableInterrupt(UInt intNum);

    /*!
     *  ======== restoreInterrupt ========
     *  Restore a specific interrupt's enabled/disabled state.
     *
     *  Restores a specific interrupt identified by an interrupt number.
     *  restoreInterrupt is generally used to restore an interrupt to its state
     *  before {@link #disableInterrupt} or {@link #enableInterrupt} was
     *  invoked
     *
     *  @param(intNum)  interrupt number to restore
     *  @param(key)     key returned from enableInt or disableInt
     */
    Void restoreInterrupt(UInt intNum, UInt key);

    /*!
     *  ======== clearInterrupt ========
     *  Clear a specific interrupt.
     *
     *  Clears a specific interrupt's pending status.
     *  The implementation is family-specific.
     *
     *  @param(intNum)  interrupt number to clear
     */
    Void clearInterrupt(UInt intNum);

instance:

    /*!
     *  Create a dispatched interrupt.
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
     *
     */
    create(Int intNum, FuncPtr hwiFxn);

    /*! maskSetting. Default is {@link #MaskingOption Hwi_MaskingOption_SELF} */
    config MaskingOption maskSetting = MaskingOption_SELF;

    /*! ISR function argument. Default is 0. */
    config UArg arg = 0;

    /*! Enable this interrupt when object is created? Default is true. */
    config Bool enableInt = true;
    
    /*! 
     *  Interrupt event ID (Interrupt Selection Number)
     *
     *  Default is -1. 
     *  Not all targets/devices support this instance parameter. 
     *  On those that don't, this parameter is ignored.
     */
    config Int eventId = -1;

    /*! 
     *  Interrupt priority.
     *
     *  The default value of -1 is used as a flag to indicate 
     *  the lowest (logical) device-specific priority value.
     *
     *  Not all targets/devices support this instance parameter. 
     *  On those that don't, this parameter is ignored.
     */
    config Int priority = -1;

    /*!
     *  ======== getFunc ========
     *  Get Hwi function and arg
     *
     *  @param(arg)     pointer for returning hwi's ISR function argument
     *  @b(returns)     hwi's ISR function
     */
    FuncPtr getFunc(UArg *arg);

    /*!
     *  ======== setFunc ========
     *  Overwrite Hwi function and arg
     *
     *  Replaces a Hwi object's hwiFxn function originally
     *  provided in {@link #create}.
     *
     *  @a(constraints)
     *  Hwi_setFunc() is not thread safe. This means that the new value for
     *  for 'fxn' may be temporarily paired with the previous value for 'arg'
     *  if pre-emption occurs within the execution of Hwi_setFunc().
     *
     *  To guard against this condition, surround the Hwi_setFunc() call with
     *  calls to Hwi_disable() and Hwi_restore():
     *
     *  @p(code)
     *  key = Hwi_disable();
     *
     *  Hwi_setFunc(newFunc, newArg);
     *
     *  Hwi_restore(key);
     *  @p
     *
     *  @param(fxn)     pointer to ISR function
     *  @param(arg)     argument to ISR function
     */
    Void setFunc(FuncPtr fxn, UArg arg);

    /*!
     *  ======== getHookContext ========
     *  Get hook instance's context for a Hwi.
     *
     *  @b(returns)     hook instance's context for hwi
     */
    Ptr getHookContext(Int id);

    /*!
     *  ======== setHookContext ========
     *  Set hook instance's context for a Hwi.
     *
     *  @param(id)            hook instance's ID
     *  @param(hookContext)   value to write to context
     */
    Void setHookContext(Int id, Ptr hookContext);

    /*!
     *  ======== getIrp ========
     *  Get address of interrupted instruction.
     *
     *  @b(returns)     most current IRP of a Hwi
     */
    Irp getIrp();
}
