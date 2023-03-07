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
 * @file Load.h
 * @brief The Load module reports execution times and load information for
 * threads in a system.
 *
 * SYS/BIOS manages four distinct levels of execution threads: hardware
 * interrupt service routines, software interrupt routines, tasks, and
 * background idle functions. This module reports execution time and load on a
 * per-task basis, and also provides information globally for hardware interrupt
 * service routines, software interrupt routines and idle functions (in the form
 * of the idle task). It can also report an estimate of the global CPU load.
 *
 * Execution time is reported in units of
 * Timestamp counts, and load is reported in percentages.
 *
 * By default, load data is gathered for Task threads. Load_hwiEnabled,
 * Load_swiEnabled and Load_taskEnabled
 * can be used to select which type(s) of threads are monitored.
 * Users can also choose to call @link Load_getTaskLoad @endlink,
 * @link Load_getGlobalSwiLoad @endlink, @link Load_getGlobalHwiLoad @endlink
 * and @link Load_getCPULoad @endlink at any time to obtain the statistics at
 * runtime.
 *
 * The module relies on @link Load_update @endlink to be called to compute load
 * and execution times from the time when @link Load_update @endlink was last
 * called. This is automatically done for every period specified by
 * Load_windowInMs in an Idle
 * function when Load_updateInIdle is set to true. The time
 * between two calls to @link Load_update @endlink is called the benchmark time
 * window.
 *
 * By passing in a function pointer of type void(*Load_postUpdate)(void)
 * through the Load_postUpdate config
 * parameter, one can specify a Load_postUpdate function that
 * is automatically called by @link Load_update @endlink immediately after the
 * statistics have been computed at the end of a benchmark time window.  Setting
 * this function allows the user to optionally post-process the thread
 * statistics once and only once per benchmark window.
 *
 * Advanced users could optionally omit calling @link Load_update @endlink in
 * the idle loop by setting Load_updateInIdle to false. They
 * can then call @link Load_update @endlink somewhere else (e.g. in a periodic
 * higher priority Task) instead to ensure statistics are computed even when the
 * system is never idle.
 *
 * <h3>CPU Load Calculation Methods</h3>
 *
 * The CPU load is computed in three different ways, depending on what threads
 * are monitored, and whether or not Power management is used to idle the CPU
 * when no threads are running.
 *
 * @htmlonly
 *  <B>Task Load Disabled and No Power Management</B>
 * @endhtmlonly
 *
 * The first method of calculating CPU load is used when Task load monitoring is
 * disabled, ie, Load_taskEnabled is false, and Power management
 * is not used. The CPU load is computed as the percentage of time in the
 * benchmark window which was NOT spent in the idle loop. More specifically, the
 * load is computed as follows:
 *
 * global CPU load = 100 * (1 - (min. time for a trip around idle loop * # times
 * in idle loop)/(benchmark time window) )
 *
 * Any work done in the idle loop is included in the CPU load - in other words,
 * any time spent in the loop beyond the shortest trip around the idle loop is
 * counted as non-idle time.
 *
 * This method works fairly well if the timestamp frequency is sufficiently high
 * (for example, if it's equal to the CPU frequency). The CPU load accuracy can
 * also be affected by caching and user idle functions.
 *
 * @htmlonly
 *  <B>Task Load Enabled and No Power Management</B>
 * @endhtmlonly
 *
 * The second method of calculating CPU load is used when Task load monitoring
 * is enabled (Load_taskEnabled = true) and Power management is not
 * used. In this case the CPU load is calculted as
 *
 * global CPU load = 100 - (Idle task load)
 *
 * This prevents any discrepancy between the calculated CPU load had we used the
 * first method, and 100 - the Idle task load.  If Swi and Hwi load monitoring
 * are not enabled, however, time spent in a Swi or Hwi will be charged to the
 * task in which it ran.  This will affect the accuracy of the CPU (and Task)
 * load, but the trade off is more overhead to do the Hwi and Swi load
 * monitoring.
 *
 * @htmlonly
 *  <B>Power Management Enabled</B>
 * @endhtmlonly
 *
 * The Load module is dependent on the timestamp timer, thus the latter must
 * continue to run during idle, in order to accurately measure idle and non-idle
 * time. On platforms where the timestamp timer is halted during sleep
 * (e.g. CC32XX), the Load module can only report correct numbers when power
 * management is disabled. On other platforms where the timer continues to run
 * during sleep, the best way to get CPU load is to make sure that
 * Load_taskEnabled is set to true.  Then the CPU load will be calculated as
 * 100 - the idle task load.  However, for BIOS in ROM builds, this method will
 * not work, as Task hooks are not allowed. So to use Load for any devices that
 * support BIOS in ROM builds, make sure the ROM build is disabled.
 *
 * <h3>Caveats</h3>
 *
 * - For the module to return accurate load values, the @link Load_update
 * @endlink function must be run at least once before the Timestamp count gets
 * to wrap around. This means on a platform with a 32-bit
 * Timestamp frequency that runs at 200 MHz, the
 * function must be called at least once every 21 sec. With faster
 * Timestamp frequencies it would have to be called
 * even more frequently.
 *
 * - If the Load_updateInIdle option is enabled, the module
 * will call @link Load_update @endlink periodically according to
 * Load_windowInMs. The latter must be set to a time interval that
 * is below (2^32) * 1000 / (Timestamp Frequency) milliseconds
 *
 * - If the Load_updateInIdle option is disabled, the user
 * is then responsible for calling @link Load_update @endlink more often than
 * the above time interval.
 *
 * - Load values might not add up to 100%. Because the module minimizes its
 * interrupt latency and avoids locking up the system to compute all thread
 * loads at once, each thread's execution time is evaluated separately over its
 * own time window, which could be slightly different to that of another thread.
 *
 * - When disabling monitoring of a thread type of higher priority, the time
 * spent in that thread type will be counted towards time spent in threads of
 * lower priority. E.g. if Load_hwiEnabled and
 * Load_swiEnabled are both false, then time spent in Hwi's and
 * Swi's will be counted towards the Tasks in which the interrupts happened.
 * Thus, for better accuracy, it is best to leave monitoring on for threads of a
 * higher priority relative to the thread type of interest.
 *
 * - When Task load monitoring is not enabled and Power management is not used,
 * the implementation of @link Load_getCPULoad() @endlink self-calibrates the
 * shortest path through the idle loop. It does this by keeping track of the
 * shortest time between invocations of an idle function automatically inserted
 * by the Load module, and assumes that to be the time it takes for one
 * iteration through the idle loop. Because of this, the CPU load value is only
 * an estimate since the idle loop might occasionally take longer to run (e.g.
 * due to caching effects, stalls). The reported CPU load tends to be slightly
 * higher than reality, especially when the load is low.
 *
 * - Currently does not support Timestamp frequencies over 4 GHz.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                          -->
 *    <tr><td> getCPULoad      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> getGlobalHwiLoad</td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> getGlobalSwiLoad</td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> getTaskLoad     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> reset           </td><td>   Y*   </td><td>   Y*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> update          </td><td>   Y*   </td><td>   Y*   </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. Load_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. Load_Module_startupDone() returns false).</li>
 *           </ul>
 *       <li> <b>*</b>: Indicates only when taskEnabled
 *    is set to false. </li>
 *       </ul>
 *    </td></tr>
 *  </table>
 * @endhtmlonly
 */

#ifndef ti_sysbios_utils_Load__include
#define ti_sysbios_utils_Load__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/runtime/Error.h>

#define ti_sysbios_utils_Load_long_names
#include "Load_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * ======== INTERNAL DEFINITIONS ========
 */

/*!
 * @brief Callback function type
 */
typedef void (*Load_FuncPtr)(void);

/*!
 * @brief Load statistics info
 */
typedef struct {
    uint32_t threadTime; /*! time spent in thread */
    uint32_t totalTime;  /*! amount of time elapsed */
} Load_Stat;

/*! @cond NODOC */
/*!
 * @brief Hook Context
 */
typedef struct {
    Queue_Elem qElem;        /*! Queue element */
    uint32_t totalTimeElapsed; /*! Total amount of time elapsed */
    uint32_t totalTime;        /*! time spent in thread */ 
    uint32_t nextTotalTime;    /*! working counter of time spent in thread */
    uint32_t timeOfLastUpdate; /*! time when update was last called */
    void * threadHandle;        /*! handle to thread whose context this is */
} Load_HookContext;
/*! @endcond */

typedef struct {
    Queue_Object taskList;   /* List to hold registered task instances */
    int taskHId;             /* Task Hook Context Id for this module */
    uint32_t taskStartTime;    /* The start time of the current task */
    Task_Handle runningTask; /* Currently running task */
    uint32_t timeElapsed;      /* Working count of time elapsed */
    bool firstSwitchDone;    /* Flag for first task switch */
    uint32_t swiStartTime;     /* Start time of the current Swi */
    Load_HookContext swiEnv; /* Singleton hook context for swi's */
    uint32_t swiCnt;           /* number of Swi's currently executing */
    uint32_t hwiStartTime;     /* Start time of the current Hwi */
    Load_HookContext hwiEnv; /* Singleton hook context for hwi's */
    uint32_t hwiCnt;           /* number of Hwi's currently executing */
    uint32_t timeSlotCnt;      /* count of number of time windows printed */
    uint32_t minLoop;          /* shortest time thru the idle loop */
    uint32_t minIdle;          /* minLoop is never set below this value */
    uint32_t t0;               /* start time of previous call to idle fxn */
    uint32_t idleCnt;          /* number of times through idle loop */
    uint32_t cpuLoad;          /* CPU load in previous time window */
} Load_Module_State;

/*
 * ======== FUNCTION DECLARATIONS ========
 */
extern void Load_init(void);
/*!
 * @brief Return the load and time spent in a specific task
 *
 * This function returns the load and time spent in a specific task along with
 * the duration over which the measurement was done. Numbers are reported in
 * Timestamp counts.
 *
 * Task handle must be valid and have been registered with Load.
 *
 * @param task Handle of the Task which time we are interested in.
 *
 * @param stat Load and time statistics info
 * 
 *@retval true if success, false if failure
 */
extern bool Load_getTaskLoad(Task_Handle task, Load_Stat *stat);
/*!
 * @brief Record thread statistics and reset all counters
 *
 * If Load_taskEnabled is set to true, this function can only
 * be called in task context.
 */
extern void Load_update();
/*! @cond NODOC */
/*!
 * @brief Record CPU load and thread loads if Task, Swi, or Hwi load monitoring
 * is enabled. This function is called by Load_update() if Power management is
 * not used.
 */
extern void Load_updateLoads();
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief This function is called by Load_update() when Load.enableCPULoadCalc
 * is false. Updates the current thread's time and hook contexts for all
 * threads. Call the postUpdate() function.  It is up to the application to
 * calculate the loads.
 */
extern void Load_updateContextsAndPost();
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Update the total time for the currently running thread.  This function
 * will be called if Task, Swi, or Hwi Load monitoring is enabled.
 */
extern void Load_updateCurrentThreadTime();
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Update hook contexts for all threads.  This function will be called if
 * Task, Swi, or Hwi Load monitoring is enabled.
 */
extern void Load_updateThreadContexts();
/*! @endcond */
/*!
 * @brief Reset all internal load counters
 *
 * If Load_taskEnabled is set to true, this function can only
 * be called in task context.
 */
extern void Load_reset();
/*!
 * @brief Return the load and time spent in Swi's
 *
 * This function returns the load and time spent in Swi's along with the time
 * duration over which the measurement was done. Numbers are reported in
 * Timestamp counts.
 *
 * @param stat Load and time statistics info
 * 
 *@retval true if success, false if failure
 */
extern bool Load_getGlobalSwiLoad(Load_Stat *stat);
/*!
 * @brief Return the load and time spent in hwi's
 *
 * This function computes the load and time spent in Hwi's along with the time
 * duration over which the measurement was done. Numbers are reported in
 * Timestamp counts.
 *
 * @param stat Load and time statistics info
 * 
 *@retval true if success, false if failure
 */
extern bool Load_getGlobalHwiLoad(Load_Stat *stat);
/*!
 * @brief Return an estimate of the global CPU load
 *
 * This function returns an estimate of CPU load (% utilization of the CPU),
 * with the idle time determined based on number of trips through the idle loop
 * multiplied by the shortest amount of time through the loop.
 *
 * This function requires the idle loop to be run during a benchmark time
 * window.
 *
 * Note: Time spent in kernel while switching to a Hwi/Swi/Task is considered
 * non-idle time.
 * 
 *@retval CPU load in %
 */
extern uint32_t Load_getCPULoad();
/*!
 * @brief Compute total CPU load from a Load_Stat structure
 *
 * This function computes percent load from the values in a Load_Stat structure.
 * 
 *@retval Load value of a Load_Stat structure in %.
 */
extern uint32_t Load_calculateLoad(Load_Stat *stat);
/*!
 * @brief Set lower bound on idle loop time used to compute CPU load
 *
 * @see Load_minIdle
 */
extern uint32_t Load_setMinIdle(uint32_t newMinIdleTime);
/*!
 * @brief Add a task to the list for benchmarking
 *
 * If Load_taskEnabled is set to true, this function can only be
 * called in task context.
 *
 * @param task Handle of the Task to be added to the list.
 *
 * @param env Handle of context structure to be used by the Task
 */
extern void Load_addTask(Task_Handle task, Load_HookContext *env);
/*!
 * @brief Remove a task from the list for benchmarking
 *
 * If Load_taskEnabled is set to true, this funciton can only be
 * called in task context.
 *
 * @param taskHandle Handle of the Task to be removed from the list.
 */
extern bool Load_removeTask(Task_Handle taskHandle);
/*! @cond NODOC */
/*!
 * @brief Idle function used to periodically update the Task time values
 */
extern void Load_idleFxn();
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Initialize thread load update times.
 */
extern void Load_startup();
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Create hook function used to initialize all task's hook context to
 * NULL during creation time. Also adds the task's hook context when @link
 * Load_autoAddTasks @endlink is set to true.
 *
 * @param task Handle of the Task to initialize.
 *
 * @param eb Error block.
 */
extern void Load_taskCreateHook(Task_Handle task, Error_Block *eb);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Delete hook function used to remove the task's hook context when @link
 * Load_autoAddTasks @endlink is set to true.
 *
 * @param task Handle of the Task to delete.
 */
extern void Load_taskDeleteHook(Task_Handle task);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Switch hook function used to perform benchmarks
 *
 * @param curTask Handle of currently executing Task.
 *
 * @param nextTask Handle of the next Task to run
 */
extern void Load_taskSwitchHook(Task_Handle curTask, Task_Handle nextTask);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Swi begin hook function used to perform benchmarks
 *
 * @param swi Handle of Swi to begin execution.
 */
extern void Load_swiBeginHook(Swi_Handle swi);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Swi end hook function used to perform benchmarks
 *
 * @param swi Handle of Swi to end execution.
 */
extern void Load_swiEndHook(Swi_Handle swi);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Hwi begin hook function used to perform benchmarks
 *
 * @param hwi Handle of Hwi to begin execution.
 */
extern void Load_hwiBeginHook(Hwi_Handle hwi);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Hwi end hook function used to perform benchmarks
 *
 * @param hwi Handle of Hwi to end execution.
 */
extern void Load_hwiEndHook(Hwi_Handle hwi);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Registration function for the module's hook
 *
 * Moved out of the internal section for ROM purposes. This function is not
 * referenced directly, so it must be a "public" function so that the linker
 * does not drop it when creating a ROM image.
 *
 * @param id The id of the hook for use in load.
 */
extern void Load_taskRegHook(int id);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Logs load values for all monitored threads. Statistics messages from
 * the kernel must be enabled (via Load.common$.diags_USER4) in order to see the
 * output.
 *
 * If @link Load_taskEnabled @endlink is set to true, this function can only be
 * called in task context.
 */
extern void Load_logLoads(void);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Log CPU load only.
 */
extern void logCPULoad(void);
/*! @endcond */

#define Load_module ((Load_Module_State *) &(Load_Module_state))

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_utils_Load__include */

#undef ti_sysbios_utils_Load_long_names
#include "Load_defs.h"
