/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated - http://www.ti.com
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
 * @file ti/sysbios/BIOS.h
 * @brief SYS/BIOS Top-Level Manager
 *
 * This module is responsible for setting up global parameters pertaining to
 * SYS/BIOS and for performing the SYS/BIOS startup sequence.
 *
 * The SYS/BIOS startup sequence is logically divided into two phases: those
 * operations that occur prior to the application's "main()" function being
 * called, and those operations that are performed after the application's
 * "main()" function is invoked.
 *
 * The "before main()" startup sequence is governed completely by the
 * ti/sysbios/runtime/Startup module.
 *
 * The "after main()" startup sequence is governed by SYS/BIOS and is initiated
 * by an explicit call to the @link BIOS_start @endlink function at the end
 * of the application's main() function.
 *
 * Control points are provided at various places in each of the two startup
 * sequences for user startup operations to be inserted.
 *
 * The RTSC runtime startup sequence is as follows:
 *
 * - Immediately after CPU reset, perform target-specific CPU initialization
 * (beginning at c_int00).
 * - Prior to cinit(), run the user-supplied "reset functions"
 * - Run cinit() to initialize C runtime environment.
 * - Run the user-supplied "first functions"
 * - Run all the module initialization functions.
 * - Run pinit().
 * - Run the user-supplied "last functions"
 * - Run * main().
 *
 * The SYS/BIOS startup sequence begins at the end of main() when BIOS_start()
 * is called:
 *
 * - Run the user-supplied "startup functions"
 * - Enable Hardware Interrupts.
 * - Enable Software Interrupts. If the system supports Software Interrupts
 *   (Swis) (via the BIOS.swiEnabled configuration parameter), then the
 *   SYS/BIOS startup sequence enables Swis at this point.
 * - Timer Startup. If the system supports Timers, then at this point all
 *   statically configured timers are initialized per their user-configuration.
 *   If a timer was configured to start "automatically", it is started here.
 * - Task Startup. If the system supports Tasks (via the BIOS.taskEnabled
 *   configuration parameter), then task scheduling begins here. If
 *   there are no statically or dynamically created Tasks in the system, then
 *   execution proceeds directly to the Idle loop.
 *
 * @warning
 * Local variables defined in main() no longer exist once BIOS_start() is
 * called. The RAM where main's local variables reside is reassigned for use as
 * the interrupt stack during the execution of BIOS_start().
 *
 * Below is a configuration script excerpt that installs a user-supplied startup
 * function at every possible control point in the RTSC and SYS/BIOS startup
 * sequence:
 *
 * @code
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
 * @endcode
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                        -->
 *    <tr><td> getCpuFreq      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> getThreadType   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setCpuFreq      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> start      </td><td>   N    </td><td>   N    </td>
 *    <td>   N    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *                  (e.g. BIOS_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *                  (e.g. BIOS_Module_startupDone() returns false).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== BIOS.h ========
 */

#ifndef ti_sysbios_BIOS__include
#define ti_sysbios_BIOS__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Current version number */
#define ti_sysbios_BIOS_version (0x70000)

/*!
 * @brief Current thread type definitions
 *
 * These values are returned by @link BIOS_getThreadType @endlink.
 *
 * @sa BIOS_getThreadType
 */
enum BIOS_ThreadType {
    BIOS_ThreadType_Hwi,
    BIOS_ThreadType_Swi,
    BIOS_ThreadType_Task,
    BIOS_ThreadType_Main
};
typedef enum BIOS_ThreadType BIOS_ThreadType;

/*!
 * @brief Type of Gate to use in the TI RTS library
 *
 * @sa BIOS_rtsGateType
 */
enum BIOS_RtsLockType {
    /*!
     * @brief No gate is added to the RTS library.  In this case, the
     * application needs to be careful to always serialize access to the
     * inherently non-reentrant ANSI C functions (such as `malloc()`,
     * `printf()`, etc.).
     */
    BIOS_NoLocking,
    /*!
     * @brief Interrupts are disabled and restored to maintain re-entrancy.
     * This is a very efficient lock but will also result in unbounded
     * interrupt latency times.  If real-time response to interrupts is
     * important, you should not use this gate to lock the RTS library.
     */
    BIOS_GateHwi,
    /*!
     * @brief Swis are disabled and restored to maintain re-entrancy.
     */
    BIOS_GateSwi,
    /*!
     * @brief A single mutex is used to maintain re-entrancy.
     */
    BIOS_GateMutex,
    /*!
     * @brief A single priority inheriting mutex is used to maintain
     * re-entrancy.
     */
    BIOS_GateMutexPri
};
typedef enum BIOS_RtsLockType BIOS_RtsLockType;

/*!
 * @brief Used in APIs that take a timeout to specify wait forever
 */
#define BIOS_WAIT_FOREVER (~(0U))

/*!
 * @brief Used in APIs that take a timeout to specify no waiting
 */
#define BIOS_NO_WAIT (0U)

/*!
 * @brief User startup function type definition.
 */
typedef void (*BIOS_StartupFuncPtr)(void);

typedef void (*BIOS_StartFuncPtr)(void);

typedef void (*BIOS_ExitFuncPtr)(int arg1);

/* @cond NODOC */
typedef struct BIOS_Module_State BIOS_Module_State;

struct BIOS_Module_State {
    Types_FreqHz cpuFreq;
    unsigned int rtsGateCount;
    intptr_t rtsGateKey;
    void * rtsGate;
    BIOS_ThreadType threadType;
    BIOS_ThreadType smpThreadType[4];
    volatile BIOS_StartFuncPtr startFunc;
    volatile BIOS_ExitFuncPtr exitFunc;
    /*!
     * @brief Gate to make sure TI RTS library APIs are re-entrant
     *
     * The application gets to determine the type of gate (lock) that is used in
     * the TI RTS library. The gate will be used to guarantee re-entrancy of the
     * RTS APIs.
     *
     * The type of gate depends on the type of threads that are going to be
     * calling into the RTS library.  For example, if both Swi and Task threads
     * are going to be calling the RTS library's printf, GateSwi should be used.
     * In this case, Hwi threads are not impacted (i.e. disabled) during the
     * printf calls from the Swi or Task threads.
     *
     * If NoLocking is used, the RTS lock is not plugged and re-entrancy for the
     * TI RTS library calls are not guaranteed. The application can plug the RTS
     * locks directly if it wants.
     *
     * Numerous gate types are provided by SYS/BIOS.  Each has its advantages
     * and disadvantages.  The following list summarizes when each type is
     * appropriate for protecting an underlying non-reentrant RTS library.
     *
     * - @link BIOS_GateHwi @endlink: Interrupts are disabled and restored to
     * maintain re-entrancy. Use if only making RTS calls from a Hwi, Swi and/or
     * Task.
     *
     * - @link BIOS_GateSwi @endlink: Swis are disabled and restored to maintain
     * re-entrancy. Use if only making RTS calls from a Swi and/or Task.
     *
     * - @link BIOS_GateMutex @endlink: A single mutex is used to maintain re-
     * entrancy.  Use if only making RTS calls from a Task.  Blocks only Tasks
     * that are also trying to execute critical regions of RTS library.
     *
     * - @link BIOS_GateMutexPri @endlink: A priority inheriting mutex is used
     * to maintain re-entrancy. Blocks only Tasks that are also trying to
     * execute critical regions of RTS library.  Raises the priority of the Task
     * that is executing the critical region in the RTS library to the level of
     * the highest priority Task that is block by the mutex.
     *
     * The default value of rtsGateType depends on the type of threading model
     * enabled by other configuration parameters. If @link BIOS_taskEnabled
     * @endlink is true, @link BIOS_GateMutex @endlink is used. If @link
     * BIOS_swiEnabled @endlink is true and @link BIOS_taskEnabled @endlink is
     * false: @link BIOS_GateSwi @endlink is used. If both @link BIOS_swiEnabled
     * @endlink and @link BIOS_taskEnabled @endlink are false: @link
     * xdc.runtime#GateNull @endlink is used.
     *
     * If @link BIOS_taskEnabled @endlink is false, the user should not select
     * @link BIOS_GateMutex @endlink (or other Task level gates). Similarly, if
     * @link BIOS_taskEnabled @endlink and @link BIOS_swiEnabled @endlinkare
     * false, the user should not select @link BIOS_GateSwi @endlink or the Task
     * level gates.
     */
    BIOS_RtsLockType rtsGateType;
    uint16_t enables;
    bool initDone;
};
/*! @endcond */


/*! @cond NODOC */
/*!
 * @brief Enables multi core SMP task scheduling
 *
 * This functionality is available on only select multi-core devices.
 *
 * More information about SMP/BIOS is provided here: @ref
 * BIOS_http://processors.wiki.ti.com/index.php/SMP/BIOS "SMP/BIOS".
 */
extern const bool BIOS_smpEnabled;

/*!
 * @brief Enables Memory Protection Extensions (MPE)
 *
 * SYS/BIOS memory protection extensions add the capability to create privileged
 * and unprivileged tasks as well as define access privileges for the
 * unprivileged tasks.
 *
 * This functionality is available on only select devices.
 */
extern const bool BIOS_mpeEnabled;
/*! @endcond */

/*!
 * @brief CPU frequency in Hz
 *
 * This configuration parameter allows SYS/BIOS to convert various periods
 * between timer ticks (or instruction cycles) and real-time units.  For
 * example, timer periods expressed in micro-seconds need to be converted into
 * timer ticks in order to properly program the timers.
 *
 * <h3>Example</h3>
 * If CPU frequency is 100MHz, the following configuration script configures
 * SYS/BIOS with the proper clock frequency:
 *
 * @code
 *     var BIOS = xdc.useModule('ti.sysbios.BIOS');
 *     BIOS.cpuFreq.hi = 0;
 *     BIOS.cpuFreq.lo = 100000000;
 * @endcode
 */
extern const Types_FreqHz BIOS_cpuFreq;

/*!
 * @brief SYS/BIOS Task services enable flag
 *
 * The following behaviors occur when @link BIOS_taskEnabled @endlink is set to
 * false:
 *
 *  - Static Task creation will
 *    result in a fatal build error.
 *  - The Idle task object is not created.
 *    (The Idle functions are invoked within the BIOS_start()
 *    thread.)
 *  - Runtime calls to Task_create will trigger an assertion violation
 *    via xdc.runtime.Assert#isTrue.
 */
extern const bool BIOS_taskEnabled;

/*!
 * @brief SYS/BIOS Swi services enable flag
 *
 * The following behaviors occur when @link BIOS_swiEnabled @endlink is set to
 * false:
 *
 *  - Static Swi creation will
 *    result in a fatal build error.
 *  - See other effects as noted for BIOS_clockEnabled = false;
 *  - Runtime calls to Swi_create will trigger an assertion violation
 */
extern const bool BIOS_swiEnabled;

/*!
 * @brief SYS/BIOS Clock services enable flag
 *
 * The following behaviors occur when @link BIOS_clockEnabled @endlink is set to
 * false:
 *
 *  - Static Clock creation will result in a fatal build error.
 *  - No Clock Swi is created.
 *  - The Clock_tickSource
 *    is set to
 *    Clock_TickSource_NULL
 *    to prevent a Timer object from being created.
 *  - For APIs that take a timeout, values other than BIOS_NO_WAIT
 *    will be equivalent to BIOS_WAIT_FOREVER.
 *
 */
extern const bool BIOS_clockEnabled;

/*!
 * @brief Size of system heap, units are in MAUs
 *
 * The system heap is, by default, used to allocate instance object state
 * structures, such as Task objects and their stacks, Semaphore objects, etc.
 */
extern const size_t BIOS_heapSize;

/*!
 * @brief Use HeapTrack with system default heap
 *
 * This configuration parameter will add a HeapTrack instance on top of the
 * system heap. HeapTrack adds a tracker packet to every allocated buffer and
 * displays the information in RTOS Object Viewer (ROV). An assert will be
 * raised on a free if there was a buffer overflow.
 */
extern const bool BIOS_heapTrackEnabled;

/*! @cond NODOC */
/*!
 * @brief Sets up a secure context when using secure version of BIOS
 *
 * This is available for some C66 secure devices only. This parameter take
 * effect only when 'useSK' is set to true. If set to true, a call to
 * Hwi_setupSC() is done in a last function.
 */
extern const bool BIOS_setupSecureContext;
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Application was linked with incorrect Boot library
 *
 * This function has a loop that spins forever. If execution reaches this
 * function, it indicates that the application was linked with an incorrect boot
 * library and the XDC runtime startup functions did not get run. This can
 * happen if the code gen tool's RTS library was before SYS/BIOS's generated
 * linker cmd file on the link line.
 */
extern void BIOS_linkedWithIncorrectBootLibrary(void);

/* init */
extern void BIOS_init(void);
/* @endcond */

/*!
 * @brief Start SYS/BIOS
 *
 * The user's main() function is required to call this function after all other
 * user initializations have been performed.
 *
 * This function does not return.
 *
 * This function performs any remaining SYS/BIOS initializations and then
 * transfers control to the highest priority ready task if @link
 * BIOS_taskEnabled @endlink is true. If @link BIOS_taskEnabled @endlink is
 * false, control is transferred directly to the Idle Loop.
 *
 * The SYS/BIOS start sequence is as follows:
 *
 *  - Invoke all the functions in the BIOS_startupFxns array.
 *  - call Hwi_startup()
 *    to enable interrupts.
 *  - if BIOS_swiEnabled is true, call
 *    Swi_startup() to enable
 *    the Swi scheduler.
 *  - Start any statically created or constructed Timers
 *    in the Timer_StartMode_AUTO
 *    mode.
 *  - if BIOS_taskEnabled is true, enable the Task scheduler
 *    and transfer the execution thread to the highest priority
 *    task in the Task_Mode_READY
 *    mode.
 *  - Otherwise, fall directly into the Idle Loop.
 *
 */
extern void BIOS_start(void);

/*! @cond NODOC */
extern void BIOS_startFunc(void);

extern void BIOS_exitFunc(int arg1);
/*! @endcond */

/*!
 * @brief Exit currently running SYS/BIOS executable
 *
 * This function is called when a SYS/BIOS executable needs to terminate
 * normally.  This function sets the internal SYS/BIOS threadType to @link
 * BIOS_ThreadType_Main @endlink and then calls @link System_exit @endlink
 * passing along the 'stat' argument.
 *
 * All functions bound via @link System_atexit @endlink or the
 * ANSI C Standard Library atexit function are then executed.
 *
 * @param stat exit status to return to calling environment.
 */
extern void BIOS_exit(int stat);

/*!
 * @brief Get the current thread type
 *
 * @retval Current thread type
 */
extern BIOS_ThreadType BIOS_getThreadType(void);

/*! @cond NODOC */
/*!
 * @brief Set the current thread type
 *
 * Called by the various threadType owners.
 *
 * @param ttype New thread type value
 *
 * @retval Previous thread type
 */
extern BIOS_ThreadType BIOS_setThreadType(BIOS_ThreadType ttype);
/*! @endcond */

/*!
 * @brief Set CPU Frequency in Hz
 *
 * This API is not thread safe. Please use appropriate locks.
 */
extern void BIOS_setCpuFreq(Types_FreqHz *freq);

/*!
 * @brief Get CPU frequency in Hz
 *
 * This API is not thread safe. Please use appropriate locks.
 */
extern void BIOS_getCpuFreq(Types_FreqHz *freq);

/*! @cond NODOC */
extern void BIOS_startFunc(void);
extern void BIOS_atExitFunc(int stat);
extern void BIOS_exitFunc(int stat);
extern void BIOS_registerRTSLock(void);
extern void BIOS_removeRTSLock(void);
extern void BIOS_rtsLock(void);
extern void BIOS_rtsUnlock(void);

extern BIOS_Module_State BIOS_Module_state;
#define BIOS_module (&(BIOS_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_BIOS__include */
