/*
 * Copyright (c) 2015-2021, Texas Instruments Incorporated
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
 *  ======== BIOS.xdc ========
 */

package ti.sysbios;

import xdc.rov.ViewInfo;

import xdc.runtime.Error;
import xdc.runtime.Types;

/*! ======== BIOS ========
 *  SYS/BIOS Top-Level Manager
 *
 *  This module is responsible for setting up global parameters
 *  pertaining to SYS/BIOS and for performing the SYS/BIOS startup
 *  sequence.
 *
 *  SYS/BIOS configures the
 *  {@link xdc.runtime.Memory#defaultHeapInstance Memory.defaultHeapInstance}
 *  using a {@link ti.sysbios.heaps.HeapMem HeapMem} instance of size
 *  {@link #heapSize}.
 *
 *  The SYS/BIOS startup sequence is logically divided into two phases: those
 *  operations that occur prior to the application's "main()" function being
 *  called, and those operations that are performed after the application's
 *  "main()" function is invoked.
 *
 *  The "before main()" startup sequence is governed completely by the RTSC
 *  runtime package's {@link xdc.runtime.Startup Startup} module.
 *
 *  The "after main()" startup sequence is governed by SYS/BIOS and is
 *  initiated by an explicit call to the {@link #start BIOS_start()} function
 *  at the end of the application's main() function.
 *
 *  Control points are provided at various places in each of the two startup
 *  sequences for user startup operations to be inserted.
 *
 *  The RTSC runtime startup sequence is as follows:
 *
 *  @p(nlist)
 *  - Immediately after CPU reset, perform target-specific CPU
 *  initialization (beginning at c_int00).
 *  - Prior to cinit(), run the user-supplied "reset functions"
 *  (see {@link xdc.runtime.Reset#fxns Reset.fxns}).
 *  - Run cinit() to initialize C runtime environment.
 *  - Run the user-supplied "first functions"
 *  (see {@link xdc.runtime.Startup#firstFxns Startup.firstFxns}).
 *  - Run all the module initialization functions.
 *  - Run pinit().
 *  - Run the user-supplied "last functions"
 *  (see {@link xdc.runtime.Startup#lastFxns Startup.lastFxns}).
 *  - Run main().
 *  @p
 *
 *  The SYS/BIOS startup sequence begins at the end of main() when
 *  BIOS_start() is called:
 *
 *  @p(nlist)
 *  - Run the user-supplied "startup functions"
 *  (see {@link #startupFxns BIOS.startupFxns}).
 *  - Enable Hardware Interrupts.
 *  - Enable Software Interrupts. If the system supports Software Interrupts
 *  (Swis) (see {@link #swiEnabled BIOS.swiEnabled}), then the SYS/BIOS
 *  startup sequence enables Swis at this point.
 *  - Timer Startup. If the system supports Timers, then at this point all
 *  statically configured timers are initialized per their
 *  user-configuration.
 *  If a timer was configured to start "automatically", it is started here.
 *  - Task Startup. If the system supports Tasks
 *  (see {@link #taskEnabled BIOS.taskEnabled}),
 *  then task scheduling begins here. If there are no statically or
 *  dynamically created Tasks in the system, then execution proceeds
 *  directly to the Idle loop.
 *  @p
 *
 *  @a(Note)
 *  Local variables defined in main() no longer exist once BIOS_start() is
 *  called. The RAM where main's local variables reside is reassigned for
 *  use as the interrupt stack during the execution of BIOS_start().
 *
 *  Below is a configuration script excerpt that installs a user-supplied
 *  startup function at every possible control point in the RTSC and
 *  SYS/BIOS startup sequence:
 *
 *  @p(code)
 *  // get handle to xdc Startup module
 *  var Startup = xdc.useModule('xdc.runtime.Startup');
 *
 *  // install "reset function"
 *  Startup.resetFxn = '&myReset';
 *
 *  // install a "first function"
 *  var len = Startup.firstFxns.length
 *  Startup.firstFxns.length++;
 *  Startup.firstFxns[len] = '&myFirst';
 *
 *  // install a "last function"
 *  var len = Startup.lastFxns.length
 *  Startup.lastFxns.length++;
 *  Startup.lastFxns[len] = '&myLast';
 *
 *  // get handle to SYS/BIOS module
 *  var BIOS = xdc.useModule('ti.sysbios.BIOS');
 *
 *  // install a SYS/BIOS startup function
 *  BIOS.addUserStartupFunction('&myBiosStartup');
 *  @p
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                        -->
 *    <tr><td> {@link #getCpuFreq}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #getThreadType}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setCpuFreq}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #start}      </td><td>   N    </td><td>   N    </td>
 *    <td>   N    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *                  (e.g. BIOS_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *                  (e.g. BIOS_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@CustomHeader   /* to check for codegen compatibility */
@Template("./BIOS.xdt")

@DirectCall
module BIOS
{
    /*!
     *  ======== ThreadType ========
     *  Current thread type definitions
     *
     *  These values are returned by {@link #getThreadType BIOS_getThreadType}.
     *
     *  @see #getThreadType
     */
    enum ThreadType {
        ThreadType_Hwi,         /*! Current thread is a Hwi */
        ThreadType_Swi,         /*! Current thread is a Swi */
        ThreadType_Task,        /*! Current thread is a Task */
        ThreadType_Main         /*! Current thread is Boot/Main */
    };

    /*!
     *  ======== RtsLockType ========
     *  Type of Gate to use in the TI RTS library
     *
     *  @field(NoLocking) no gate is added to the RTS library.  In this case,
     *  the application needs to be careful to always serialize access to the
     *  inherently  non-reentrant ANSI C functions (such as `malloc()`,
     *  `printf()`, etc.).
     *
     *  @field(GateHwi) Interrupts are disabled and restored to maintain
     *  re-entrancy.  This is a very efficient lock but will also result in
     *  unbounded interrupt latency times.  If real-time response to interrupts
     *  is important, you should not use this gate to lock the RTS library.
     *
     *  @field(GateSwi) Swis are disabled and restored to maintain
     *  re-entrancy.
     *
     *  @field(GateMutex) A single mutex is used to maintain re-entrancy.
     *
     *  @field(GateMutexPri) A single priority inheriting mutex is used to
     *  maintain re-entrancy.
     *
     *  @see #rtsGateType
     */
    enum RtsLockType {
        NoLocking,
        GateHwi,
        GateSwi,
        GateMutex,
        GateMutexPri
    };

    /*!
     *  ======== LibType ========
     *  SYS/BIOS library selection options
     *
     *  This enumeration defines all the SYS/BIOS library types
     *  supported by the product.  You can select the library type by setting
     *  the {@link #libType BIOS.libType} configuration parameter.
     *
     *  @field(LibType_Instrumented) The library is built with logging and
     *  assertions enabled.
     *
     *  @field(LibType_NonInstrumented) The library is built with logging and
     *  assertions disabled.
     *
     *  @field(LibType_Custom) The library is built using the options
     *  specified by {@link #customCCOpts}. Program optimization is performed
     *  to reduce the size of the executable and improve its performance.
     *  Enough debug information is retained to allow you to step through the
     *  application code in CCS and locate global variables.
     *
     *  @field(LibType_Debug) This setting is similar to the LibType_Custom
     *  setting, however, no program optimization is performed. The resulting
     *  executable is fully debuggable, and you can step into SYS/BIOS code.
     *  The tradeoff is that the executable is larger and runs slower than
     *  builds that use the LibType_Custom option.
     *
     *  @see #libType
     */
    enum LibType {
        LibType_Instrumented,           /*! Instrumented (Asserts and Logs enabled) */
        LibType_NonInstrumented,        /*! Non-instrumented (Asserts and Logs disabled) */
        LibType_Custom,                 /*! Custom (Fully configurable) */
        LibType_Debug                   /*! Debug (Fully configurable) */
    };

    /*! Used in APIs that take a timeout to specify wait forever */
    const UInt WAIT_FOREVER = ~(0U);

    /*! Used in APIs that take a timeout to specify no waiting */
    const UInt NO_WAIT = 0U;

    /*! User startup function type definition. */
    typedef Void (*StartupFuncPtr)(Void);

    /*!
     *  ======== ModuleView ========
     *  @_nodoc
     */
    metaonly struct ModuleView {
        String       currentThreadType[];
        String       rtsGateType;
        Int          cpuFreqLow;
        Int          cpuFreqHigh;
        Bool         clockEnabled;
        Bool         swiEnabled;
        Bool         taskEnabled;
        String       startFunc;
    }

    /*!
     *  ======== ErrorView ========
     *  @_nodoc
     */
    metaonly struct ErrorView {
        String mod;
        String tab;
        String inst;
        String field;
        String message;
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
                'Module',
                {
                    type: ViewInfo.MODULE,
                    viewInitFxn: 'viewInitModule',
                    structName: 'ModuleView'
                }
            ],
            [
                'Scan for errors...',
                {
                    type: ViewInfo.MODULE_DATA,
                    viewInitFxn: 'viewInitErrorScan',
                    structName: 'ErrorView'
                }
            ],
            ]
        });

    /*!
     *  ======== libType ========
     *  SYS/BIOS Library type
     *
     *  The SYS/BIOS runtime is built in the form of a library that is
     *  linked with your application.  Several forms of this library are
     *  supported by the SYS/BIOS product.  This configuration parameter
     *  allows you to select the form of the SYS/BIOS library to use.
     *
     *  The default value of libType is
     *  {@link #LibType_Instrumented BIOS_LibType_Instrumented}.  For a
     *  complete list of options and what they offer see {@link #LibType}.
     */
    metaonly config LibType libType = LibType_Instrumented;

    /*!
     *  ======== customCCOpts ========
     *  Compiler options used when building a custom SYS/BIOS library
     *
     *  When {@link #libType BIOS.libType} is set to
     *  {@link #LibType_Custom BIOS_LibType_Custom} or
     *  {@link #LibType_Debug BIOS_LibType_Debug},
     *  this string contains the options passed to the compiler during any
     *  build of the SYS/BIOS sources.
     *
     *  In addition to the options specified by `BIOS.customCCOpts`, several
     *  `-D` and `-I` options are also passed to the compiler.  The options
     *  specified by `BIOS.customCCOpts` are, however, the first options passed
     *  to the compiler on the command line.
     *
     *  To view the custom compiler options, add the following line to your
     *  config script:
     *
     *  @p(code)
     *  print(BIOS.customCCOpts);
     *  @p
     *
     *  When {@link #libType BIOS.libType} is set to
     *  {@link #LibType_Custom BIOS_LibType_Custom},
     *  `BIOS.customCCOpts` is initialized to settings that create a highly
     *  optimized SYS/BIOS library.
     *
     *  When {@link #libType BIOS.libType} is set to
     *  {@link #LibType_Debug BIOS_LibType_Debug},
     *  `BIOS.customCCOpts` is initialized to settings that create a
     *  non-optimized SYS/BIOS library that can be used to single-step through
     *  the APIs with the CCS debugger.
     *
     *  More information about using `BIOS.customCCOpts` is provided in the
     *  {@link http://processors.wiki.ti.com/index.php/SYS/BIOS_FAQs SYS/BIOS FAQs}.
     *
     *  @a(Warning)
     *  The default value of `BIOS.customCCOpts`, which is derived from the
     *  target specified by your configuration, includes runtime model options
     *  (such as endianess) that must be the same for all sources built and
     *  linked into your application.  You must not change or add any options
     *  that can alter the runtime model specified by the default value of
     *  `BIOS.customCCOpts`.
     *
     *  @a(Warning)
     *  Setting `BIOS.libType` overwrites `BIOS.customCCOpts`. Therefore, if an
     *  application's *.cfg file sets both these config params, the libType must
     *  be set before customCCOpts so the changes to customCCOpts persist.
     */
    metaonly config String customCCOpts;

    /*!
     *  ======== includeXdcRuntime ========
     *  Include xdc.runtime sources in custom built library 
     *
     *  By default, the xdc.runtime library sources are not included in the
     *  custom SYS/BIOS library created for the application. Instead,
     *  the pre-built xdc.runtime library is provided by the respective target
     *  used to build the application.
     *
     *  Setting this parameter to true will cause the xdc.runtime library
     *  sources to be included in the custom SYS/BIOS library. This setting
     *  yields the most efficient library in both code size and runtime
     *  performance.
     */
    metaonly config Bool includeXdcRuntime = false;

    /*!
     *  ======== smpEnabled ========
     *  Enables multi core SMP task scheduling
     *
     *  This functionality is available on only select multi-core devices.
     *
     *  More information about SMP/BIOS is provided here:
     *  {@link http://processors.wiki.ti.com/index.php/SMP/BIOS SMP/BIOS}.
     */
    config Bool smpEnabled = false;

    /*!
     *  ======== psaEnabled ========
     *  Enables ARM's Platform Security Architecture (PSA) extensions
     *
     *  This functionality is available on only select devices.
     */
    metaonly config Bool psaEnabled = false;

    /*!
     *  ======== cpuFreq ========
     *  CPU frequency in Hz
     *
     *  This configuration parameter allow SYS/BIOS to convert various
     *  periods between timer ticks (or instruction cycles) and real-time
     *  units.  For example, timer periods expressed in micro-seconds need
     *  to be converted into timer ticks in order to properly program the
     *  timers.
     *
     *  The default value of this parameter is obtained from the platform
     *  (the clockRate property of {@link xdc.cfg.Program#cpu Program.cpu})
     *  which is the CPU clock rate when the processor is reset.
     *
     *  @a(Example)
     *  If CPU frequency is 720MHz, the following configuration script
     *  configures SYS/BIOS with the proper clock frequency:
     *  @p(code)
     *     var BIOS = xdc.useModule('ti.sysbios.BIOS');
     *     BIOS.cpuFreq.hi = 0;
     *     BIOS.cpuFreq.lo = 720000000;
     *  @p
     */
    config Types.FreqHz cpuFreq;

    /*!
     *  ======== runtimeCreatesEnabled ========
     *  Runtime instance creation enable flag.
     *
     *  true = Mod_create() & Mod_delete() callable at runtime
     *  false = Mod_create() & Mod_delete() not callable at runtime
     */
    config Bool runtimeCreatesEnabled = true;

    /*!
     *  ======== taskEnabled ========
     *  SYS/BIOS Task services enable flag
     *
     *  The following behaviors occur when {@link #taskEnabled} is
     *  set to false:
     *
     *  @p(blist)
     *  - Static {@link ti.sysbios.knl.Task Task} creation will
     *    result in a fatal build error.
     *  - The Idle task object is not created.
     *    (The Idle functions are invoked within the {@link #start()}
     *    thread.)
     *  - Runtime calls to Task_create will trigger an assertion violation
     *    via {@link xdc.runtime.Assert#isTrue}.
     *  @p
     */
    config Bool taskEnabled = true;

    /*!
     *  ======== swiEnabled ========
     *  SYS/BIOS Swi services enable flag
     *
     *  The following behaviors occur when {@link #swiEnabled} is
     *  set to false:
     *
     *  @p(blist)
     *  - Static {@link ti.sysbios.knl.Swi Swi} creation will
     *    result in a fatal build error.
     *  - See other effects as noted for {@link #clockEnabled} = false;
     *  - Runtime calls to Swi_create will trigger an assertion violation
     *    via {@link xdc.runtime.Assert#isTrue}.
     *  @p
     */
    config Bool swiEnabled = true;

    /*!
     *  ======== clockEnabled ========
     *  SYS/BIOS Clock services enable flag
     *
     *  The following behaviors occur when {@link #clockEnabled} is
     *  set to false:
     *
     *  @p(blist)
     *  - Static Clock creation will result in a fatal build error.
     *  - No Clock Swi is created.
     *  - The {@link ti.sysbios.knl.Clock#tickSource Clock_tickSource}
     *    is set to
     *    {@link ti.sysbios.knl.Clock#TickSource_NULL Clock_TickSource_NULL}
     *    to prevent a Timer object from being created.
     *  - For APIs that take a timeout, values other than {@link #NO_WAIT}
     *    will be equivalent to {@link #WAIT_FOREVER}.
     *  @p
     */
    config Bool clockEnabled = true;

    /*!
     *  ======== assertsEnabled ========
     *  SYS/BIOS Assert checking in Custom SYS/BIOS library enable flag
     *
     *  When set to true, Assert checking code is compiled into
     *  the custom library created when {@link #libType BIOS.libType}
     *  is set to {@link #LibType_Custom BIOS_LibType_Custom} or
     *  {@link #LibType_Debug BIOS_LibType_Debug}.
     *
     *  When set to false, Assert checking code is removed from the custom
     *  library created when BIOS.libType is set to BIOS.LibType_Custom
     *  or BIOS.LibType_Debug.
     *  This option can considerably improve runtime performance as well
     *  significantly reduce the application's code size.
     *
     *  see {@link #libType BIOS.libType}.
     */
    metaonly config Bool assertsEnabled = true;

    /*!
     *  ======== logsEnabled ========
     *  SYS/BIOS Log support in Custom SYS/BIOS library enable flag
     *
     *  When set to true, SYS/BIOS execution Log code is compiled into
     *  the custom library created when {@link #libType BIOS.libType}
     *  is set to {@link #LibType_Custom BIOS_LibType_Custom} or
     *  {@link #LibType_Debug BIOS_LibType_Debug}. 
     *
     *  When set to false, all Log code is removed from
     *  the custom library created when BIOS.libType = BIOS.LibType_Custom
     *  or BIOS.LibType_Debug.
     *  This option can considerably improve runtime performance as well
     *  significantly reduce the application's code size.
     *
     *  see {@link #libType BIOS.libType}.
     *
     *  @a(Warning) Since interrupts
     *  are enabled when logs are generated, this setting will have the
     *  side effect of requiring task stacks to be sized large enough
     *  to absorb two interrupt contexts rather than one. 
     *  See the discussion on task stacks in {@link ti.sysbios.knl.Task
     *  Task} for more information.
     */
    metaonly config Bool logsEnabled = true;

    /*!
     *  ======== heapSize ========
     *  Size of system heap, units are in MAUs
     *
     *  The system heap is, by default, used to allocate instance object
     *  state structures, such as {@link ti.sysbios.knl.Task Task} objects
     *  and their stacks, {@link ti.sysbios.knl.Semaphore Semaphore} objects,
     *  etc.
     *
     *  If the application configuration does not set
     *  Memory.defaultHeapInstance, then SYS/BIOS will create a
     *  {@link ti.sysbios.heaps.HeapMem HeapMem} heap of this size.  This
     *  heap will be assigned to
     *  {@link xdc.runtime.Memory#defaultHeapInstance Memory.defaultHeapInstance}
     *  and will therefore be used as the default system heap.  This heap
     *  will also be used by the SYS/BIOS version of the standard C library
     *  functions malloc(), calloc() and free().
     */
    config SizeT heapSize = 0x1000;

    /*!
     *  ======== heapSection ========
     *  Section to place the system heap
     *
     *  This configuration parameter allows you to specify a named output
     *  section that will contain the SYS/BIOS system heap.  The system heap
     *  is, by default, used to allocate {@link ti.sysbios.knl.Task Task}
     *  stacks and instance object state structures.  So, giving this section
     *  a name and explicitly placing it via a linker command file can
     *  significantly improve system performance.
     *
     *  If heapSection is `null` (or `undefined`) the system heap is placed
     *  in the target's default data section.
     */
    config String heapSection = null;

    /*!
     *  ======== heapTrackEnabled ========
     *  Use HeapTrack with system default heap
     *
     *  This configuration parameter will add a HeapTrack instance on top of
     *  the system heap. HeapTrack adds a tracker packet to every allocated
     *  buffer and displays the information in RTOS Object Viewer (ROV).
     *  An assert will be raised on a free if there was a buffer overflow.
     */
    config Bool heapTrackEnabled = false;

    /*!
     *  ======== setupSecureContext ========
     *  @_nodoc
     *  Sets up a secure context when using secure version of BIOS
     *
     *  This is available for some C66 secure devices only.
     *  This parameter take effect only when 'useSK' is set to true.
     *  If set to true, a call to Hwi_setupSC() is done in a last function.
     */
    config Bool setupSecureContext = false;

    /*!
     *  ======== useSK ========
     *  @_nodoc
     *  use the secure version of BIOS
     *
     *  This is available for some C66 secure devices only.
     *  This parameter can only be used with the custom build.
     */
    config Bool useSK = false;

    /*!
     *  ======== rtsGateType ========
     *  Gate to make sure TI RTS library APIs are re-entrant
     *
     *  The application gets to determine the type of gate (lock) that is used
     *  in the TI RTS library. The gate will be used to guarantee re-entrancy
     *  of the RTS APIs.
     *
     *  The type of gate depends on the type of threads that are going to
     *  be calling into the RTS library.  For example, if both Swi and Task
     *  threads are going to be calling the RTS library's printf, GateSwi
     *  should be used. In this case, Hwi threads are not impacted (i.e.
     *  disabled) during the printf calls from the Swi or Task threads.
     *
     *  If NoLocking is used, the RTS lock is not plugged and re-entrancy for
     *  the TI RTS library calls are not guaranteed. The application can plug
     *  the RTS locks directly if it wants.
     *
     *  Numerous gate types are provided by SYS/BIOS.  Each has its advantages
     *  and disadvantages.  The following list summarizes when each type is
     *  appropriate for protecting an underlying non-reentrant RTS library.
     *  @p(dlist)
     *      - {@link #GateHwi}:
     *        Interrupts are disabled and restored to maintain re-entrancy.
     *        Use if only making RTS calls from a Hwi, Swi and/or Task.
     *
     *      - {@link #GateSwi}:
     *        Swis are disabled and restored to maintain re-entrancy. Use if
     *        only making RTS calls from a Swi and/or Task.
     *
     *      - {@link #GateMutex}:
     *        A single mutex is used to maintain re-entrancy.  Use if only
     *        making RTS calls from a Task.  Blocks only Tasks that are
     *        also trying to execute critical regions of RTS library.
     *
     *      - {@link #GateMutexPri}:
     *        A priority inheriting mutex is used to maintain re-entrancy.
     *        Blocks only Tasks that are also trying to execute critical
     *        regions of RTS library.  Raises the priority of the Task that
     *        is executing the critical region in the RTS library to the
     *        level of the highest priority Task that is block by the mutex.
     *  @p
     *
     *  The default value of rtsGateType depends on the type of threading
     *  model enabled by other configuration parameters.
     *  If {@link #taskEnabled} is true, {@link #GateMutex} is used.
     *  If {@link #swiEnabled} is true and {@link #taskEnabled} is false:
     *  {@link #GateSwi} is used.
     *  If both {@link #swiEnabled} and {@link #taskEnabled} are false:
     *  {@link xdc.runtime#GateNull} is used.
     *
     *  If {@link #taskEnabled} is false, the user should not select
     *  {@link #GateMutex} (or other Task level gates). Similarly, if
     *  {@link #taskEnabled} and {@link #swiEnabled}are false, the user
     *  should not select {@link #GateSwi} or the Task level gates.
     */
    metaonly config RtsLockType rtsGateType;

    /*!
     *  ======== startupFxns ========
     *  Functions to be executed at the beginning of BIOS_start()
     *
     *  These user (or middleware) functions are executed before Hwis,
     *  Swis, and Tasks are started.
     */
    metaonly config StartupFuncPtr startupFxns[] = [];

    /*!
     *  ======== version ========
     *  SYS/BIOS version number macro
     *
     *  This macro has a hex value that represents the SYS/BIOS version
     *  number. The hex value has the version format 0xMmmpp, where
     *  M is a single digit Major number, mm is a 2 digit minor number
     *  and pp is a 2 digit patch number.
     *
     *  Example: A macro hex value of 0x64501 implies that the SYS/BIOS
     *  product version number is 6.45.01
     */
    const UInt32 version = 0x68104;

    /*!
     *  ======== addUserStartupFunction ========
     *  @_nodoc
     *  Statically add a function to the startupFxns table.
     */
    metaonly Void addUserStartupFunction(StartupFuncPtr func);

    /*!
     *  ======== linkedWithIncorrectBootLibrary ========
     *  Application was linked with incorrect Boot library
     *
     *  This function has a loop that spins forever. If execution
     *  reaches this function, it indicates that the application
     *  was linked with an incorrect boot library and the XDC
     *  runtime startup functions did not get run. This can happen
     *  if the code gen tool's RTS library was before SYS/BIOS's
     *  generated linker cmd file on the link line.
     */
    Void linkedWithIncorrectBootLibrary();

    /*!
     *  ======== start ========
     *  Start SYS/BIOS
     *
     *  The user's main() function is required to call this function
     *  after all other user initializations have been performed.
     *
     *  This function does not return.
     *
     *  This function performs any remaining SYS/BIOS initializations
     *  and then transfers control to the highest priority ready
     *  task if {@link #taskEnabled} is true. If {@link #taskEnabled}
     *  is false, control is transferred directly to the Idle Loop.
     *
     *  The SYS/BIOS start sequence is as follows:
     *  @p(blist)
     *  - Invoke all the functions in the {@link #startupFxns} array.
     *  - call {@link ti.sysbios.hal.Hwi#enable Hwi_startup()}
     *    to enable interrupts.
     *  - if {@link #swiEnabled} is true, call
     *    {@link ti.sysbios.knl.Swi#enable Swi_startup()} to enable
     *    the Swi scheduler.
     *  - Start any statically created or constructed Timers
     *    in the {@link ti.sysbios.hal.Timer#StartMode Timer_StartMode_AUTO}
     *    mode.
     *  - if {@link #taskEnabled} is true, enable the Task scheduler
     *    and transfer the execution thread to the highest priority
     *    task in the {@link ti.sysbios.knl.Task#Mode Task_Mode_READY}
     *    mode.
     *  - Otherwise, fall directly into the Idle Loop.
     *  @p
     *
     */
    Void start();

    /*!
     *  ======== exit ========
     *  Exit currently running SYS/BIOS executable
     *
     *  This function is called when a SYS/BIOS executable needs to terminate
     *  normally.  This function sets the internal SYS/BIOS threadType to
     *  {@link #ThreadType_Main} and then calls
     *  {@link xdc.runtime.System#exit System_exit}(stat), passing along
     *  the 'stat' argument.
     *
     *  All functions bound via
     * `{@link xdc.runtime.System#atexit System_atexit}` or the ANSI C
     *  Standard Library `atexit` function are then executed.
     *
     *  @param(stat)    exit status to return to calling environment.
     */
    Void exit(Int stat);

    /*!
     *  ======== getThreadType ========
     *  Get the current thread type
     *
     *  @b(returns)     Current thread type
     */
    ThreadType getThreadType();

    /*!
     *  @_nodoc
     *  ======== setThreadType ========
     *  Set the current thread type
     *
     *  Called by the various threadType owners.
     *
     *  @param(ttype)   New thread type value
     *  @b(returns)     Previous thread type
     */
    ThreadType setThreadType(ThreadType ttype);

    /*!
     *  ======== setCpuFreq ========
     *  Set CPU Frequency in Hz
     *
     *  This API is not thread safe. Please use appropriate locks.
     */
    Void setCpuFreq(Types.FreqHz *freq);

    /*!
     *  ======== getCpuFreq ========
     *  Get CPU frequency in Hz
     *
     *  This API is not thread safe. Please use appropriate locks.
     */
    Void getCpuFreq(Types.FreqHz *freq);

    /*!
     *  @_nodoc
     *  ======== getCpuFrequency ========
     *  Get CPU frequency in Hz.
     *
     *  This function is currently used by UIA and is called in the
     *  UIAMetaData validate() function.
     *  NOTE: Javascript does not support UInt64, so this only works
     *  if the frequency is less than 4GHz.  Keep this function for
     *  backwards compatibility (for awhile).
     */
    metaonly UInt64 getCpuFrequency();

    /*!
     *  @_nodoc
     *  ======== getCpuFreqMeta ========
     *  Get CPU frequency in Hz.
     *
     *  This function is currently used by UIA and is called in the
     *  UIAMetaData validate() function.
     */
    metaonly Types.FreqHz getCpuFreqMeta();

    /*!
     *  @_nodoc
     *  ======== getTimestampFrequency ========
     *  Get timestamp frequency in Hz.  If we don't know the timestamp
     *  frequency of the device, return 0.
     *
     *  This function is currently used by UIA and is called in the
     *  UIAMetaData validate() function.
     *  NOTE: Javascript does not support UInt64, so this only works
     *  if the frequency is less than 4GHz.  Keep this function for
     *  backwards compatability (for awhile).
     */
    metaonly UInt64 getTimestampFrequency();

    /*!
     *  @_nodoc
     *  ======== getTimestampFreqMeta ========
     *  Get timestamp frequency in Hz.  If we don't know the timestamp
     *  frequency of the device, return 0.
     *
     *  This function is currently used by UIA and is called in the
     *  UIAMetaData validate() function.
     */
    metaonly Types.FreqHz getTimestampFreqMeta();

    /*!
     *  @_nodoc
     *  ======== getDefaultTimestampProvider ========
     *  Returns the name of the TimestampProvider module BIOS will set
     *  xdc.runtime.Timestamp.SupportProxy to if it hasn't been configured
     *  in the user's config script.
     *
     *  This function is meant to be used by modules that have their own
     *  TimestampProvider proxies if they want to initialize them to the
     *  default xdc.runtime.Timestamp.SupportProxy binding selected by BIOS:
     *
     *  if (!this.$written("TimestampProxy")) {
     *      if (xdc.runtime.$written("Timestamp.SupportProxy") {
     *          this.TimestampProxy = xdc.runtime.Timestamp.SupportProxy;
     *      }
     *      else {
     *          this.TimestampProxy = xdc.module(BIOS.getDefaultTimestampProvider());
     *      }
     *  }
     */
    metaonly String getDefaultTimestampProvider();

internal:

    /*
     *  ======== buildingAppLib ========
     *  Enable custom build of SYS/BIOS from source
     *
     *  true = building application-specific custom lib
     *  false = building internal instrumented/nonInstrumented lib
     */
    metaonly config Bool buildingAppLib = true;

    /*
     *  ======== libDir ========
     *  Specify output library directory
     */
    metaonly config String libDir = null;

    /*
     *  @_nodoc
     *  ======== codeCoverageEnabled ========
     *  Setting this to 'true' will tweak other settings for
     *  code coverage.
     *
     *  For example, by setting this to true, the code that melts
     *  away when hook.length is 0 does not melt away.
     */
    metaonly config Bool codeCoverageEnabled = false;

    /*
     *  ======== getCCOpts ========
     *  Get the compiler options necessary to build
     */
    metaonly String getCCOpts(String target);

    /*
     *  ======== intSize ========
     *  Used to determine number of bits in an Int
     */
    struct intSize {
        Int intSize;
    }

    /*
     *  ======== bitsPerInt ========
     *  Number of bits in an integer
     *
     *  Used for error checking
     */
    metaonly config Char bitsPerInt;

    /*
     *  ======== installedErrorHook ========
     *  User/default Error.raiseHook
     *
     *  BIOS_errorRaiseHook() calls this after setting threadType
     *  to Main so that GateMutex's threadType check will
     *  pass.
     */
    config Void (*installedErrorHook)(Error.Block *);

    /*
     *  ======== errorRaiseHook ========
     *  Error.raiseHook that sets threadType to Main so
     *  threadType checking Asserts will pass.
     */
    Void errorRaiseHook(Error.Block *eb);

    /*
     *  ======== startFunc ========
     *  Generated BIOS_start function
     */
    Void startFunc();

    /*
     *  ======== atExitFunc ========
     *  Generated BIOS_atExitFunc function
     */
    Void atExitFunc(Int stat);

    /*
     *  ======== exitFunc ========
     *  Generated BIOS_exitFunc function
     */
    Void exitFunc(Int stat);

    /*
     *  ======== registerRTSLock ========
     *  Register the RTS lock
     *
     *  Added as a startup function in BIOS.xs.
     */
    Void registerRTSLock();

    /*
     *  ======== removeRTSLock ========
     *  Remove the RTS locks
     *
     *  This function is called by BIOS_exit().
     */
    Void removeRTSLock();

    /*
     *  ======== rtsLock ========
     *  Called by rts _lock() function
     */
    Void rtsLock();

    /*
     *  ======== rtsUnLock ========
     *  Called by rts _unlock() function
     */
    Void rtsUnlock();

    /*
     *  ======== nullFunc ========
     */
    Void nullFunc();

    /*
     *  ======== fireFrequencyUpdate ========
     */
    function fireFrequencyUpdate(newFreq);

    /*
     *  ======== RtsGateProxy ========
     *  Gate proxy to be used for the rts gate
     */
    proxy RtsGateProxy inherits xdc.runtime.IGateProvider;

    /*
     *  ======== StartFuncPtr ========
     *  Function prototype for the generated BIOS_start
     */
    typedef Void (*StartFuncPtr)(void);

    /*
     *  ======== ExitFuncPtr ========
     *  Function prototype for the generated BIOS_exit
     */
    typedef Void (*ExitFuncPtr)(Int);

    /*
     *  ======== Module_State ========
     */
    struct Module_State {
        Types.FreqHz        cpuFreq;            /* in KHz */
        UInt                rtsGateCount;       /* count for nesting */
        IArg                rtsGateKey;         /* key for unlocking */
        RtsGateProxy.Handle rtsGate;            /* gate for RTS calls */
        ThreadType          threadType;         /* Curr Thread Type */
                                                /* (Hwi, Swi, Task) */
        ThreadType          smpThreadType[];    /* SMP Core specific */
                                                /* Thread Type */
        volatile StartFuncPtr startFunc;
        volatile ExitFuncPtr  exitFunc;
    };
}
