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
 * @file ti/sysbios/knl/Task.h
 * @brief Task Manager.
 *
 * The Task module makes available a set of functions that manipulate task
 * objects accessed through pointers of type @link Task_Handle @endlink. Tasks
 * represent independent threads of control that conceptually execute functions
 * in parallel within a single C program; in reality, concurrency is achieved by
 * switching the processor from one task to another.
 *
 * All tasks executing within a single program share a common set of global
 * variables, accessed according to the standard rules of scope defined for C
 * functions.
 *
 * Each task is in one of five modes of execution at any point in time: running,
 * ready, blocked, terminated, or inactive. By design, there is always one (and
 * only one) task currently running, even if it is only the idle task managed
 * internally by Task. The current task can be suspended from execution by
 * calling certain Task functions, as well as functions provided by other
 * modules like the Semaphore or Event Modules. The current task can also
 * terminate its own execution. In either case, the processor is switched to the
 * highest priority task that is ready to run.
 *
 * You can assign numeric priorities to tasks. Tasks are readied for execution
 * in strict priority order; tasks of the same priority are scheduled on a
 * first-come, first-served basis. The priority of the currently running task is
 * never lower than the priority of any ready task. Conversely, the running task
 * is preempted and re-scheduled for execution whenever there exists some ready
 * task of higher priority.
 *
 * To use the Task module or to set any of the Task module configuration variables,
 * the following must be added to the app.syscfg file:
 *
 * @code 
 * const Task = scripting.addModule("/ti/sysbios/knl/Task");
 * @endcode 
 *
 * <h3>Task Stacks</h3>
 *
 * When you create a task, it is provided with its own run-time stack, used for
 * storing local variables as well as for further nesting of function calls.
 * Each stack must be large enough to handle normal subroutine calls and one
 * task preemption context. A task preemption context is the context that gets
 * saved when one task preempts another as a result of an interrupt thread
 * readying a higher-priority task.
 *
 * See the BIOS User's Guide for further discussions regarding task stack
 * sizing.
 *
 * Certain system configuration settings will result in task stacks needing to
 * be large enough to absorb two interrupt contexts rather than just one.
 * Setting the BIOS logsEnabled configuration parameter to 'true' or
 * installing any Task hooks will have the side effect of allowing up to two
 * interrupt contexts to be placed on a task stack.
 *
 * <h3>Task Deletion</h3>
 *
 * Any dynamically created task that is not in the Task_Mode_RUNNING state (ie
 * not the currently running task) can be deleted using the @link Task_delete
 * @endlink API.
 *
 * Task_delete() removes the task from all internal queues and calls
 * Memory_free() is used to free the task object and its stack. Memory_free()
 * must acquire a lock to the memory before proceeding. If another task already
 * holds a lock to the memory, then the thread performing the delete will be
 * blocked until the memory is unlocked.
 *
 * Note: Task_delete() should be called with extreme care. As mentioned above,
 * the scope of Task_delete() is limited to freeing the Task object itself,
 * freeing the task's stack memory if it was allocated at create time, and
 * removing the task from any SYS/BIOS-internal state structures.
 *
 * SYS/BIOS does not keep track of any resources the task may have acquired or
 * used during its lifetime.
 *
 * It is the application's responsibility to guarantee the integrity of a task's
 * partnerships prior to deleting that task.
 *
 * For example, if a task has obtained exclusive access to a resource, deleting
 * that task will make the resource forever unavailable.
 *
 * Task_delete() sets the referenced task handle to NULL. Any subsequent call to
 * a Task instance API using that null task handle will behave unpredictably and
 * will usually result in an application crash.
 *
 * Assuming a task completely cleans up after itself prior to calling
 * Task_exit() (or falling through the the bottom of the task function), it is
 * then safest to use Task_delete() only when a task is in the
 * 'Task_Mode_TERMINATED' state.
 *
 * Delete hooks: You can specify application-wide Delete hook functions that run
 * whenever a task is deleted. See the discussion of Hook Functions below for
 * details.
 *
 * Task_delete() constraints:
 *
 *  - The task cannot be the currently executing task (Task_self()).
 *  - Task_delete cannot be called from a Swi or Hwi.
 *  - No check is performed to prevent Task_delete from being used on a
 *  statically-created object. If a program attempts to delete a task object
 *  that was created statically, the Memory_free() call will result in an
 *  assertion failure in its corresponding Heap manager, causing the
 *  application to exit.
 *
 *
 * <h3>Stack Alignment</h3>
 *
 * Stack size parameters for both static and dynamic tasks are rounded up to the
 * nearest integer multiple of a target-specific alignment requirement.
 *
 * In the case of Task's which are created with a user-provided stack, both the
 * base address and the stackSize are aligned. The base address is increased to
 * the nearest aligned address. The stack size is decreased accordingly and then
 * rounded down to the nearest integer multiple of the target-specific required
 * alignment.
 *
 * @anchor taskHookFunctions
 *
 * <b>Hook Functions</b>
 *
 * Sets of hook functions can be specified for the Task module.  Each set can
 * contain these hook functions:
 *
 *  - Register: A function called before any statically created tasks
 *      are initialized at runtime.  The register hook is called at boot time
 *      before main() and before interrupts are enabled.
 *  - Create: A function that is called when a task is created.
 *      This includes tasks that are created statically and those
 *      created dynamically using Task_create or Task_construct.
 *      For statically created tasks, create hook is called before main()
 *      and before interrupts are enabled. For dynamically created or
 *      constructed tasks, create hook is called in the same context the
 *      task is created or constructed in i.e. if a task is created in
 *      main(), the create hook is called in main context and if the task
 *      is created within another task, it is called in task context. The
 *      create hook is called outside of a Task_disable/enable block and
 *      before the task has been added to the ready list.
 *  - Ready: A function that is called when a task becomes ready to run.
 *      The ready hook is called in the context of the thread unblocking
 *      a task and therefore it can be called in Hwi, Swi or Task context.
 *      If a Swi or Hwi posts a semaphore that unblocks a task, the ready
 *      hook would be called in the Swi or Hwi's context. The ready hook is
 *      called from within a Task_disable/enable block with interrupts enabled.
 *  - Switch: A function that is called just before a task switch
 *      occurs. The 'prev' and 'next' task handles are passed to the switch
 *      hook. 'prev' is set to NULL for the initial task switch that occurs
 *      during SYS/BIOS startup.  The switch hook is called from within a
 *      Task_disable/enable block with interrupts enabled, in the
 *      context of the task being switched from (ie: the `prev` task).
 *  - Exit: A function that is called when a task exits using Task_exit.
 *      It is called in the exiting task's context. The exit hook is passed
 *      the handle of the exiting task. The exit hook is called outside of a
 *      Task_disable/enable block and before the task has been removed from
 *      the kernel lists.
 *  - Delete: A function that is called when any task is deleted at
 *      run-time with Task_delete. The delete hook is called in idle task
 *      context if Task_deleteTerminatedTasks is set to true. Otherwise,
 *      it is called in the context of the task that is deleting another task.
 *      The delete hook is called outside of a Task_disable/enable block.
 *
 * Hook functions can only be configured statically.
 *
 * If you define more than one set of hook functions, all the functions of a
 * particular type will be run when a Task triggers that type of hook.
 *
 * To add a Task hook or set of Task hooks, the following syntax is used in the
 * app.syscfg file:
 *
 * @code
 * const Task = scripting.addModule("/ti/sysbios/knl/Task");
 * Task.taskHooks.create(1);
 * Task.taskHooks[0].registerFxn = "myRegisterFxn";
 * Task.taskHooks[0].createFxn   = "myCreateFxn";
 * Task.taskHooks[0].readyFxn    = "myReadyFxn";
 * Task.taskHooks[0].switchFxn   = "mySwitchFxn";
 * Task.taskHooks[0].exitFxn     = "myExitFxn";
 * Task.taskHooks[0].deleteFxn   = "myDeleteFxn";
 * @endcode
 *
 * Leaving a subset of the hook functions undefined is ok.
 *
 * @warning
 * Configuring ANY Task hook function will have the side effect of allowing up
 * to two interrupt contexts beings saved on a task stack. Be careful to size
 * your task stacks accordingly.
 *
 * <b>Register Function</b>
 *
 * The Register function is provided to allow a hook set to store its hookset
 * ID.  This id can be passed to @link Task_setHookContext @endlink and @link
 * Task_getHookContext @endlink to set or get hookset-specific context.  The
 * Register function must be specified if the hook implementation needs to use
 * @link Task_setHookContext @endlink or @link Task_getHookContext @endlink. The
 * registerFxn hook function is called during system initialization before
 * interrupts have been enabled.
 *
 * @code
 *  void myRegisterFxn(int id);
 * @endcode
 *
 * <b>Create and Delete Functions</b>
 *
 * The create and delete functions are called whenever a Task is created or
 * deleted.  They are called with interrupts enabled (unless called at boot time
 * or from main()).
 *
 * @code
 *  void myCreateFxn(Task_Handle task, Error_Block *eb);
 * @endcode
 *
 * @code
 *  void myDeleteFxn(Task_Handle task);
 * @endcode
 *
 * <b>Switch Function</b>
 *
 * If a switch function is specified, it is invoked just before the new task is
 * switched to.  The switch function is called with interrupts enabled.
 *
 * This function can be used to save/restore additional task context (for
 * example, external hardware registers), to check for task stack overflow, to
 * monitor the time used by each task, etc.
 *
 * @code
 *  void mySwitchFxn(Task_Handle prev, Task_Handle next);
 * @endcode
 *
 * To properly handle the switch to the first task your switchFxn should check
 * for "prev == NULL" before using prev:
 *
 * @code
 *  void mySwitchFxn(Task_Handle prev, Task_Handle next)
 *  {
 *      if (prev != NULL) {
 *          ...
 *      }
 *      ...
 *  }
 * @endcode
 *
 * <b>Ready Function</b>
 *
 * If a ready function is specified, it is invoked whenever a task is made ready
 * to run.   The ready function is called  with interrupts enabled (unless
 * called at boot time or from main()).
 *
 * @code
 *  void myReadyFxn(Task_Handle task);
 * @endcode
 *
 * <b>Exit Function</b>
 *
 * If an exit function is specified, it is invoked when a task exits (via call
 * to Task_exit() or when a task returns from its' main function). The Exit
 * Function is called with interrupts enabled.
 *
 * @code
 *  void myExitFxn(Task_Handle task);
 * @endcode
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *  </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *  <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                       -->
 *    <tr><td> create          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> disable         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> exit            </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> getIdleTask     </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Params_init     </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> restore         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> self            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> sleep           </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> yield           </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> construct       </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> delete          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> destruct        </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getEnv          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getHookContext  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getMode         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getPri          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getFunc         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setEnv          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setHookContext  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setPri          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> stat            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *  (e.g. after Task_init() has been called). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *  (e.g. before Task_init() has been called). </li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== Task.h ========
 */

#ifndef ti_sysbios_knl_Task__include
#define ti_sysbios_knl_Task__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Clock.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/IHeap.h>

/*! @cond NODOC */
#define ti_sysbios_knl_Task_long_names
#include "Task_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Asserted in Task_create
 */
#define Task_A_badPriority "invalid priority"

/*!
 * @brief Asserted in Task_delete
 */
#define Task_A_badTaskState "cannot delete a task in RUNNING state"

/*!
 * @brief Asserted in Task_create and Task_delete
 */
#define Task_A_badThreadType "cannot create/delete a task from a Hwi or Swi thread"

/*!
 * @brief Asserted in Task_sleep
 */
#define Task_A_badTimeout "cannot sleep forever"

/*!
 * @brief Asserted in Task_delete
 */
#define Task_A_noPendElem "not enough info to delete BLOCKED task"

/*!
 * @brief Asserted in Task_sleep
 */
#define Task_A_sleepTaskDisabled "cannot call Task_sleep when the scheduler is disabled"

/*!
 * @brief Asserted in Task_create
 */
#define Task_A_taskDisabled "cannot create a task when tasking is disabled"

/*!
 * @brief Error raised when a stack overflow (or corruption) is detected.
 *
 * This error is raised by kernel's stack checking function.  This function
 * checks the stacks before every task switch to make sure that reserved word at
 * top of stack has not been modified.
 *
 * The stack checking logic is enabled by the @link Task_initStackFlag @endlink
 * and @link Task_checkStackFlag @endlink configuration parameters.  If both of
 * these flags are set to true, the kernel will validate the stacks.
 */
#define Task_E_stackOverflow "task 0x%x stack overflow"

/*!
 * @brief Error raised when a task's stack pointer (SP) does not point somewhere
 * within the task's stack.
 *
 * This error is raised by kernel's stack checking function.  This function
 * checks the SPs before every task switch to make sure they point within the
 * task's stack.
 *
 * The stack checking logic is enabled by the @link Task_initStackFlag @endlink
 * and @link Task_checkStackFlag @endlink configuration parameters.  If both of
 * these flags are set to true, the kernel will validate the stack pointers.
 */
#define Task_E_spOutOfBounds "task 0x%x stack error, SP = 0x%x"

/*!
 * @brief Task_delete error.
 *
 * This error raised when a @link Task_delete @endlink is called when
 * the Task.deleteTerminated configuration parameter is set to 'true' and the
 * task is in the terminated state. When Task.deleteTerminated is 'true', the
 * idle task will delete terminated tasks.
 */
#define Task_E_deleteNotAllowed "delete not allowed task 0x%x"

/*! @cond NODOC */
/*!
 * @brief Error raised when the check value of the Task module state does not
 * match the stored check value (computed during startup). This indicates that
 * the Task module state was corrupted.
 */
#define Task_E_moduleStateCheckFailed "invalid module state"

/*!
 * @brief Error raised when the check value of the Task object does not match
 * the stored check value (computed during startup). This indicates that the
 * Task object was corrupted.
 */
#define Task_E_objectCheckFailed "invalid task object 0x%x"
/*! @endcond */

/*! @cond NODOC */
typedef struct Task_PendElem Task_PendElem;
typedef struct Task_RunQEntry Task_RunQEntry;
/*! @endcond */

/*!
 * @brief Task execution modes.
 *
 * These enumerations are the range of modes or states that a task can be in. A
 * task's current mode can be gotten using @link Task_stat @endlink.
 */
enum Task_Mode {
    Task_Mode_RUNNING,
    Task_Mode_READY,
    Task_Mode_BLOCKED,
    Task_Mode_TERMINATED,
    /*!
     * @brief Task is on inactive task list
     */
    Task_Mode_INACTIVE
};

/*!
 * @brief Task execution modes.
 *
 * These enumerations are the range of modes or states that a task can be in. A
 * task's current mode can be gotten using @link Task_stat @endlink.
 */
typedef enum Task_Mode Task_Mode;

/*!
 * @brief Task function type definition.
 */
typedef void (*Task_FuncPtr)(uintptr_t arg1, uintptr_t arg2);

typedef struct Task_Struct Task_Struct;
typedef struct Task_Struct Task_Object;
typedef struct Task_Struct *Task_Handle;
typedef struct Task_Struct *Task_Instance;

/*! @cond NODOC */
struct Task_PendElem {
    Queue_Elem qElem;
    Task_Handle taskHandle;
    Clock_Handle clockHandle;
};
/* @endcond */

/*!
 * @brief Task Status Buffer.
 *
 * Passed to and filled in by @link Task_stat @endlink;
 */
typedef struct Task_Stat Task_Stat;

struct Task_Stat {
    /*!
     * @brief Task priority.
     */
    int priority;
    /*!
     * @brief Task stack pointer. Default = null.
     *
     * Null indicates that the stack is to be allocated by create().
     */
    void * stack;
    /*!
     * @brief Task stack size.
     */
    size_t stackSize;
    /*!
     * @brief Mem heap used for dynamically created task stack.
     *
     * The default value of NULL means that BIOS heap is used.
     */
    IHeap_Handle stackHeap;
    /*!
     * @brief Global environment struct.
     */
    void * env;
    Task_Mode mode;
    /*!
     * @brief Task's current stack pointer.
     */
    void * sp;
    size_t used;
};

/*!
 * @brief Task hook set type definition.
 *
 * Sets of hook functions can be specified for the Task module. See @ref
 * taskHookFunctions "Hook Functions" for details.
 */
typedef struct Task_HookSet Task_HookSet;

struct Task_HookSet {
    void (*registerFxn)(int arg1);
    void (*createFxn)(Task_Handle arg1, Error_Block* arg2);
    void (*readyFxn)(Task_Handle arg1);
    void (*switchFxn)(Task_Handle arg1, Task_Handle arg2);
    void (*exitFxn)(Task_Handle arg1);
    void (*deleteFxn)(Task_Handle arg1);
};

typedef struct { int length; const Task_HookSet *elem; } Task_Hook;

typedef struct Task_Params Task_Params;

struct Task_Params {
    /*!
     * @brief Optional name for the object. NULL by default.
     */
    char * name;
    /*!
     * @brief Task function argument. Default is 0
     */
    uintptr_t arg0;
    /*!
     * @brief Task function argument. Default is 0
     */
    uintptr_t arg1;
    /*!
     * @brief Task priority.
     */
    int priority;
    /*!
     * @brief Task stack pointer. Default = null.
     *
     * NULL indicates that the stack is to be allocated by create().
     */
    void * stack;
    /*!
     * @brief Task stack size.
     */
    size_t stackSize;
    /*!
     * @brief Mem heap used for dynamically created task stack.
     *
     * The default value of NULL means that BIOS heap is used.
     */
    IHeap_Handle stackHeap;
    /*!
     * @brief Global environment struct.
     */
    void * env;
    /*!
     * @brief Exit system immediately when the last task with this flag set to
     * true has terminated.
     *
     * Default is true.
     */
    bool vitalTaskFlag;
};

/*! @cond NODOC */
struct Task_Struct {
    Queue_Elem qElem;
    Queue_Elem objElem;
    char * name;
    volatile int priority;
    unsigned int mask;
    void * context;
    Task_Mode mode;
    Task_PendElem *pendElem;
    size_t stackSize;
    char *stack;
    IHeap_Handle stackHeap;
    Task_FuncPtr fxn;
    uintptr_t arg0;
    uintptr_t arg1;
    void * env;
    void * *hookEnv;
    bool vitalTaskFlag;
    Queue_Handle readyQ;
    uint32_t checkValue;
    void * tls;
};

/* Module_State */
struct Task_Module_State {
    Queue_Struct objQ;
    volatile bool locked;
    volatile unsigned int curSet;
    volatile bool workFlag;
    unsigned int vitalTasks;
    Task_Handle curTask;
    Queue_Handle curQ;
    Queue_Handle readyQ;
    Queue_Object terminatedQ;
    Queue_Object inactiveQ;
    Task_Handle idleTask;
    bool initDone;
    char dummy;
};

typedef struct Task_Module_State Task_Module_State;

/*!
 * @brief "All Task Blocked" function type definition.
 */
typedef void (*Task_AllBlockedFuncPtr)(void);

/* ModStateCheckValueFuncPtr */
typedef uint32_t (*Task_ModuleStateCheckValueFuncPtr)(Task_Module_State* arg1);

/* ModStateCheckFuncPtr */
typedef int (*Task_ModuleStateCheckFuncPtr)(Task_Module_State* arg1, uint32_t arg2);

/*!
 * @brief Check value computation function type definition.
 */
typedef uint32_t (*Task_ObjectCheckValueFuncPtr)(Task_Handle arg1);

/*!
 * @brief Task object data integrity check function type definition
 */
typedef int (*Task_ObjectCheckFuncPtr)(Task_Handle arg1, uint32_t arg2);

extern Queue_Handle Task_terminatedQ;
extern Queue_Handle Task_inactiveQ;
extern Queue_Handle Task_objectQ;

/*! @endcond */

/*!
 * @brief Number of Task priorities supported. Default is 16.
 *
 * The maximum number of priorities supported is target specific and depends on
 * the number of bits in a unsigned int data type. For 6x and ARM devices the maximum
 * number of priorities is therefore 32. For C28x devices, the maximum number of
 * priorities is 16.
 *
 * @code
 *  Task.numPriorities = 16;
 * @endcode
 */
extern const unsigned int Task_numPriorities;

/*!
 * @brief Default stack size (in MAUs) used for all tasks.
 *
 * Default is provided by the ti/sysbios/family/Settings.js file.
 *
 * @code
 *  Task.defaultStackSize = 1024;
 * @endcode
 */
extern const size_t Task_defaultStackSize;

/*! @cond NODOC */
/*!
 * @brief Default Mem heap used for all dynamically created task stacks.
 *
 * Default is null.
 */
extern IHeap_Handle Task_defaultStackHeap;
/*! @endcond */

/*!
 * @brief Function to call while all tasks are blocked.
 *
 * This function will be called repeatedly while no tasks are ready to run.
 *
 * Ordinarily (in applications that have tasks ready to run at startup), the
 * function will run in the context of the last task to block.
 *
 * In an application where there are no tasks ready to run when BIOS_start() is
 * called, the allBlockedFunc function is called within the BIOS_start() thread
 * which runs on the system/ISR stack.
 *
 * By default, allBlockedFunc is initialized to point to an internal function
 * that simply returns.
 *
 * By adding the following lines to the config script, the Idle functions will
 * run whenever all tasks are blocked:
 *
 * @code
 *  Task.enableIdleTask = false;
 *  Task.allBlockedFunc = "ti_sysbios_knl_Idle_run";
 * @endcode
 *
 * @sa Task_enableIdleTask
 *
 * @pre
 * The configured allBlockedFunc is designed to be called repeatedly. It must
 * return in order for the task scheduler to check if all tasks are STILL
 * blocked and if not, run the highest priority task currently ready to run.
 *
 * The configured allBlockedFunc function is called with interrupts disabled. If
 * your function must run with interrupts enabled, surround the body of your
 * code with  Hwi_enable()/Hwi_restore() function calls per the following
 * example:
 *
 * @code
 *  void yourFunc() {
 *      unsigned int hwiKey;
 *
 *      hwiKey = Hwi_enable();
 *
 *      ...         // your code here
 *
 *      Hwi_restore(hwiKey);
 *  }
 * @endcode
 */
extern const Task_AllBlockedFuncPtr Task_allBlockedFunc;

/*!
 * @brief Initialize stack with known value for stack checking at runtime (see
 * @link Task_checkStackFlag @endlink).
 *
 * This is also useful for inspection of stack in debugger or core dump
 * utilities. Default is true.
 *
 * @code
 * Task.initStackFlag = true/false;
 * @endcode
*/
extern const bool Task_initStackFlag;

/*!
 * @brief Check 'from' and 'to' task stacks before task context switch.
 *
 * The check consists of testing the top of stack value against its initial
 * value (see configuration parameter Task.initStackFlag). If it is no longer at this
 * value, the assumption is that the task has overrun its stack. If the test
 * fails, then the Task_E_stackOverflow error is raised.
 *
 * Default is true.
 *
 * To enable or disable full stack checking, you should set both this flag and
 * the Hwi.checkStackFlag configuration parameter.
 *
 * @code
 * Task.checkStackFlag = true/false;
 * @endcode
 *
 * @note
 * Enabling stack checking will add some interrupt latency because the checks
 * are made within the Task scheduler while interrupts are disabled.
 */
extern const bool Task_checkStackFlag;

/*!@cond NODOC */
/*!
 * @brief Perform a runtime data integrity check on each Task object
 *
 * This configuration parameter determines whether a data integrity check is
 * performed on each Task object in the system in order to detect data
 * corruption.
 *
 * If this field is set to true, a check value of the static fields in the Task
 * object (i.e. fields that do not change during the lifetime of the Task) is
 * computed when the Task is created. The computed check value is stored for use
 * by the Task object check function. The application can implement its own
 * check value computation function (see @link Task_objectCheckValueFxn
 * @endlink). By default, SYS/BIOS installs a check value computation function
 * that computes a 32-bit checksum of the static fields in the Task object.
 *
 * The Task object check function (see @link Task_objectCheckFxn @endlink) is
 * called from within a Task switch hook if stack checking (see @link
 * Task_checkStackFlag @endlink) is enabled. It is also called when a task
 * blocks or unblocks. The application can provide its own implementation of
 * this function. By default, SYS/BIOS installs a check function that computes
 * the check value for select Task object fields and compares the resulting
 * check value against the stored value.
 *
 * If the Task object check function returns a '-1' (i.e. check failed), then
 * the SYS/BIOS kernel will raise an error.
 */
extern const bool Task_objectCheckFlag;

/*!
 * @brief Function called to perform Task object data integrity check
 *
 * If @link Task_objectCheckFlag @endlink is set to true, SYS/BIOS kernel will
 * call this function from within a Task switch hook and each time a Task blocks
 * or unblocks. SYS/BIOS provides a default implementation of this function that
 * computes the check value for the static Task object fields and compares the
 * resulting check value against the stored value. The application can install
 * its own implementation of the object check function.
 *
 * Here's an example Task object check function:
 *
 * @code
 *  var Task = xdc.useModule('ti.sysbios.knl.Task');
 *
 *  // Enable Task object data integrity check
 *  Task.objectCheckFlag = true;
 *
 *  // Install custom Task object check function
 *  Task.objectCheckFxn = "&myCheckFunc";
 * @endcode
 *
 * @code
 *  #define ti_sysbios_knl_Task__internalaccess
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  int myCheckFunc(Task_Handle handle, uint32_t checkValue)
 *  {
 *      uint32_t newCheckValue;
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
 * @endcode
 */
extern const Task_ObjectCheckFuncPtr Task_objectCheckFxn;

/*!
 * @brief Function called to compute module state check value
 *
 * If @link Task_moduleStateCheckFlag @endlink is set to true, SYS/BIOS kernel
 * will call this function during startup to compute the Task module state's
 * check value.
 *
 * SYS/BIOS provides a default implementation of this function that computes a
 * 32-bit checksum for the static module state fields (i.e. module state fields
 * that do not change during the lifetime of the application). The application
 * can install its own implementation of this function.
 *
 * Here's an example module state check value computation function:
 *
 * @code
 *  var Task = xdc.useModule('ti.sysbios.knl.Task');
 *
 *  // Enable Task module state data integrity check
 *  Task.moduleStateCheckFlag = true;
 *
 *  // Install custom module state check value function
 *  Task.moduleStateCheckValueFxn = "&myCheckValueFunc";
 * @endcode
 *
 * @code
 *  #define ti_sysbios_knl_Task__internalaccess
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  uint32_t myCheckValueFunc(Task_Module_State *moduleState)
 *  {
 *      uint64_t checksum;
 *
 *      checksum = (uintptr_t)moduleState->readyQ +
 *                 (uintptr_t)moduleState->idleTask +
 *                 (uintptr_t)moduleState->constructedTasks;
 *      checksum = (checksum >> 32) + (checksum & 0xFFFFFFFF);
 *      checksum = checksum + (checksum >> 32);
 *
 *      return ((uint32_t)(~checksum));
 *  }
 * @endcode
 */
extern const Task_ModuleStateCheckValueFuncPtr Task_moduleStateCheckValueFxn;

/* objectCheckValue */
extern uint32_t Task_objectCheckValue(Task_Handle, uint32_t);

/*!
 * @brief Function called to compute Task object check value
 *
 * If @link Task_objectCheckFlag @endlink is set to true, SYS/BIOS kernel will
 * call this function to compute the Task object's check value each time a Task
 * is created.
 *
 * SYS/BIOS provides a default implementation of this function that computes a
 * 32-bit checksum for the static Task object fields (i.e. Task object fields
 * that do not change during the lifetime of the Task). The application can
 * install its own implementation of this function.
 *
 * Here's an example Task object check value computation function:
 *
 * @code
 *  var Task = xdc.useModule('ti.sysbios.knl.Task');
 *
 *  // Enable Task object data integrity check
 *  Task.objectCheckFlag = true;
 *
 *  // Install custom Task object check value function
 *  Task.objectCheckValueFxn = "&myCheckValueFunc";
 * @endcode
 *
 * @code
 *  #define ti_sysbios_knl_Task__internalaccess
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  uint32_t myCheckValueFunc(Task_Handle taskHandle)
 *  {
 *      uint64_t checksum;
 *
 *      checksum = taskHandle->stackSize +
 *                 (uintptr_t)taskHandle->stack +
 *                 (uintptr_t)taskHandle->stackHeap +
 *  #if defined(__IAR_SYSTEMS_ICC__)
 *                 (uint64_t)taskHandle->fxn +
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
 *      return ((uint32_t)(~checksum));
 *  }
 * @endcode
 */
extern const Task_ObjectCheckValueFuncPtr Task_objectCheckValueFxn;

/*!
 * @brief Perform a runtime data integrity check on the Task module state
 *
 * This configuration parameter determines whether a data integrity check is
 * performed on the Task module state in order to detect data corruption.
 *
 * If this field is set to true, a check value of the static fields in the Task
 * module state (i.e. fields that do not change during the lifetime of the
 * application) is computed during startup. The computed check value is stored
 * for use by the Task module state check function. The application can
 * implement its own check value computation function (see @link
 * Task_moduleStateCheckValueFxn @endlink). By default, SYS/BIOS installs a
 * check value computation function that computes a 32-bit checksum of the
 * static fields in the Task module state.
 *
 * The module state check function (see @link Task_moduleStateCheckFxn @endlink)
 * is called from within the Task_disable() function. The application can
 * provide its own implementation of this function. By default, SYS/BIOS
 * installs a check function that computes the check value for select module
 * state fields and compares the resulting check value against the stored value.
 *
 * If the module state check function returns a '-1' (i.e. check failed), then
 * the SYS/BIOS kernel will raise an error.
 */
extern const bool Task_moduleStateCheckFlag;

/*!
 * @brief Function called to perform module state data integrity check
 *
 * If @link Task_moduleStateCheckFlag @endlink is set to true, SYS/BIOS kernel
 * will call this function each time Task_disable() function is called. SYS/BIOS
 * provides a default implementation of this function that computes the check
 * value for the static module state fields and compares the resulting check
 * value against the stored value. In addition, the check function validates
 * some of the pointers used by the Task scheduler. The application can install
 * its own implementation of the module state check function.
 *
 * Here's an example module state check function:
 *
 * @code
 *  var Task = xdc.useModule('ti.sysbios.knl.Task');
 *
 *  // Enable Task module state data integrity check
 *  Task.moduleStateCheckFlag = true;
 *
 *  // Install custom module state check function
 *  Task.moduleStateCheckFxn = "&myCheckFunc";
 * @endcode
 *
 * @code
 *  #define ti_sysbios_knl_Task__internalaccess
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  int myCheckFunc(Task_Module_State *moduleState, uint32_t checkValue)
 *  {
 *      uint32_t newCheckValue;
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
 * @endcode
 */
extern const Task_ModuleStateCheckFuncPtr Task_moduleStateCheckFxn;

/* moduleStateCheckValue */
extern uint32_t Task_moduleStateCheckValue;

/*!
 * @brief Function called to compute module state check value
 *
 * If @link Task_moduleStateCheckFlag @endlink is set to true, SYS/BIOS kernel
 * will call this function during startup to compute the Task module state's
 * check value.
 *
 * SYS/BIOS provides a default implementation of this function that computes a
 * 32-bit checksum for the static module state fields (i.e. module state fields
 * that do not change during the lifetime of the application). The application
 * can install its own implementation of this function.
 *
 * Here's an example module state check value computation function:
 *
 * @code
 *  var Task = xdc.useModule('ti.sysbios.knl.Task');
 *
 *  // Enable Task module state data integrity check
 *  Task.moduleStateCheckFlag = true;
 *
 *  // Install custom module state check value function
 *  Task.moduleStateCheckValueFxn = "&myCheckValueFunc";
 * @endcode
 *
 * @code
 *  #define ti_sysbios_knl_Task__internalaccess
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  uint32_t myCheckValueFunc(Task_Module_State *moduleState)
 *  {
 *      uint64_t checksum;
 *
 *      checksum = (uintptr_t)moduleState->readyQ +
 *                 (uintptr_t)moduleState->idleTask +
 *                 (uintptr_t)moduleState->constructedTasks;
 *      checksum = (checksum >> 32) + (checksum & 0xFFFFFFFF);
 *      checksum = checksum + (checksum >> 32);
 *
 *      return ((uint32_t)(~checksum));
 *  }
 * @endcode
 */
extern const Task_ModuleStateCheckValueFuncPtr Task_moduleStateCheckValueFxn;
/*!@cond NODOC */

/*!
 * @brief Automatically delete terminated tasks.
 *
 * If this feature is enabled, an Idle function is installed that deletes
 * dynamically created Tasks that have terminated either by falling through
 * their task function or by explicitly calling Task_exit().
 *
 * A list of terminated Tasks that were created dynmically is maintained
 * internally. Each invocation of the installed Idle function deletes the first
 * Task on this list. This one-at-a-time process continues until the list is
 * empty.
 *
 * @code
 * Task.deleteTerminatedTasks = true/false;
 * @endcode
 *
 * @note
 * This feature is disabled by default.
 *
 * @warning
 * When this feature is enabled, an error will be raised if the user's
 * application attempts to delete a terminated task. If a terminated task has
 * already been automatically deleted and THEN the user's application attempts
 * to delete it (ie: using a stale Task handle), the results are undefined and
 * probably catastrophic!
 */
extern const bool Task_deleteTerminatedTasks;

/*!
 * @brief Const array that holds the HookSet objects.
 *
 * See @ref taskHookFunctions "Hook Functions" for details about HookSets.
 */
extern const Task_Hook Task_hooks;

/*! @cond NODOC */
extern void Task_init(void);

extern int Task_Instance_init(Task_Object *obj, Task_FuncPtr fxn, const Task_Params *prms, Error_Block *eb);

extern void Task_Instance_finalize(Task_Object *obj, int ec);
/*! @endcond */

/*!
 * @brief Create a Task.
 *
 * Task_create creates a new task object. If successful, Task_create returns the
 * handle of the new task object. If unsuccessful, Task_create returns NULL
 * unless it aborts.
 *
 * The fxn parameter uses the @link Task_FuncPtr @endlink type to pass a pointer
 * to the function the Task object should run. For example, if myFxn is a
 * function in your program, your C code can create a Task object to call that
 * function as follows:
 *
 * @code
 *  Task_Params taskParams;
 *
 *  // Create task with priority 15
 *  Task_Params_init(&taskParams);
 *  taskParams.stackSize = 512;
 *  taskParams.priority = 15;
 *  Task_create((Task_FuncPtr)myFxn, &taskParams, &eb);
 * @endcode
 *
 * The following statements statically create a task in the configuration file:
 *
 * @code
 *  var params = new Task.Params;
 *  params.name = "tsk0";
 *  params.arg0 = 1;
 *  params.arg1 = 2;
 *  params.priority = 1;
 *  Task.create('&tsk0_func', params);
 * @endcode
 *
 * If NULL is passed instead of a pointer to an actual Task_Params struct, a
 * default set of parameters is used. The "eb" is an error block that you can
 * use to handle errors that may occur during Task object creation.
 *
 * The newly created task is placed in @link Task_Mode_READY @endlink mode, and
 * is scheduled to begin concurrent execution of the following function call:
 *
 * @code
 *  (*fxn)(arg1, arg2);
 * @endcode
 *
 * As a result of being made ready to run, the task runs any application-wide
 * Ready functions that have been specified.
 *
 * Task_exit is automatically called if and when the task returns from fxn.
 *
 * <b>Create Hook Functions</b>
 *
 * You can specify application-wide Create hook functions in your config file
 * that run whenever a task is created. This includes tasks that are created
 * statically and those created dynamically using Task_create.
 *
 * For Task objects created statically, Create functions are called during the
 * Task module initialization phase of the program startup process prior to
 * main().
 *
 * For Task objects created dynamically, Create functions are called after the
 * task handle has been initialized but before the task has been placed on its
 * ready queue.
 *
 * Any SYS/BIOS function can be called from Create functions. SYS/BIOS passes
 * the task handle of the task being created to each of the Create functions.
 *
 * All Create function declarations should be similar to this:
 *
 * @code
 *  void myCreateFxn(Task_Handle task);
 * @endcode
 *
 * @pre
 *
 *  - The fxn parameter and the name attribute cannot be NULL.
 *  - The priority attribute must be less than or equal to
 *  (Task_numPriorities - 1) and greater than or equal to one (1)
 *  (priority 0 is owned by the Idle task).
 *  - The priority can be set to -1 for tasks that will not execute
 *  until another task changes the priority to a positive value.
 *  - The stackHeap attribute must identify a valid memory Heap.
 *
 * @param fxn Task function
 * @param prms optional create parameters (NULL for defaults)
 * @param eb error block
 *
 * @retval Task handle (NULL on failure)
 */
extern Task_Handle Task_create(Task_FuncPtr fxn, const Task_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a task
 *
 * Task_construct is equivalent to Task_create except that the Task_Struct is
 * pre-allocated.  See Task_create() for a description of this API.
 *
 * @param obj pointer to a Task object
 * @param fxn Task function
 * @param prms optional create parameters (NULL for defaults)
 * @param eb error block
 *
 * @retval Task handle (NULL on failure)
 */
extern Task_Handle Task_construct(Task_Struct *obj, Task_FuncPtr fxn, const Task_Params *prms, Error_Block *eb);

/*!
 * @brief Delete a task
 *
 * Task_delete deletes a Task object. Note that Task_delete takes a pointer to
 * a Task_Handle which enables Task_delete to set the Task handle to NULL.
 * 
 * @param task pointer to Task handle
 */
extern void Task_delete(Task_Handle *task);

/*!
 * @brief Destruct a task
 *
 * Task_destruct destructs a Task object.
 *
 * @param obj pointer to Task object
 */
extern void Task_destruct(Task_Struct *obj);

/*! @cond NODOC */
/*!
 * @brief Start the task scheduler.
 *
 * Task_startup signals the end of boot operations, enables the Task scheduler
 * and schedules the highest priority ready task for execution.
 *
 * Task_startup is called by BIOS_start() after Hwi_enable() and Swi_enable().
 * There is no return from this function as the execution thread is handed to
 * the highest priority ready task.
 */
extern void Task_startup(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Returns true if the Task scheduler is enabled
 */
extern bool Task_enabled(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Force a Task scheduler unlock. Used by Core_atExit() & Core_hwiFunc()
 * to unlock Task scheduler before exiting.
 *
 * This function should only be called after a Hwi_disable() has entered the
 * Inter-core gate and disabled interrupts locally.
 */
extern void Task_unlockSched(void);
/*! @endcond */

/*!
 * @brief Disable the task scheduler.
 *
 * @link Task_disable @endlink and @link Task_restore @endlink control Task
 * scheduling. @link Task_disable @endlink disables all other Tasks from running
 * until @link Task_restore @endlink is called. Hardware and Software interrupts
 * can still run.
 *
 * @link Task_disable @endlink and @link Task_restore @endlink allow you to
 * ensure that statements that must be performed together during critical
 * processing are not preempted by other Tasks.
 *
 * The value of the key returned is opaque to applications and is meant to be
 * passed to Task_restore().
 *
 * In the following example, the critical section is not preempted by any Tasks.
 *
 * @code
 *  key = Task_disable();
 *      `critical section`
 *  Task_restore(key);
 * @endcode
 *
 * You can also use @link Task_disable @endlink and @link Task_restore @endlink
 * to create several Tasks and allow them to be invoked in priority order.
 *
 * @link Task_disable @endlink calls can be nested.
 *
 * @pre
 * Do not call any function that can cause the current task to block within a
 * @link Task_disable @endlink/@link Task_restore @endlink block. For example,
 * @link Semaphore_pend @endlink (if timeout is non-zero),
 * @link Task_sleep @endlink, @link Task_yield @endlink, and Memory_alloc can
 * all cause blocking.
 * 
 * @retval key for use with @link Task_restore @endlink
 */
extern unsigned int Task_disable(void);

/*! @cond NODOC */
/*!
 * @brief Enable the task scheduler.
 *
 * @link Task_enable @endlink unconditionally enables the Task scheduler and
 * schedules the highest priority ready task for execution.
 *
 * This function is called by @link Task_startup @endlink (which is called by
 * @link BIOS_start @endlink to begin multi-tasking operations.
 */
extern void Task_enable(void);
/*! @endcond */

/*!
 * @brief Restore Task scheduling state.
 *
 * @link Task_disable @endlink and @link Task_restore @endlink control Task
 * scheduling @link Task_disable @endlink disables all other Tasks from running
 * until @link Task_restore @endlink is called. Hardware and Software interrupts
 * can still run.
 *
 * @link Task_disable @endlink and @link Task_restore @endlink allow you to
 * ensure that statements that must be performed together during critical
 * processing are not preempted.
 *
 * In the following example, the critical section is not preempted by any Tasks.
 *
 * @code
 *  key = Task_disable();
 *      `critical section`
 *  Task_restore(key);
 * @endcode
 *
 * You can also use @link Task_disable @endlink and @link Task_restore @endlink
 * to create several Tasks and allow them to be performed in priority order.
 *
 * @link Task_disable @endlink calls can be nested.
 *
 * @link Task_restore @endlink returns with interrupts enabled if the key
 * unlocks the scheduler
 *
 * @pre
 * Do not call any function that can cause the current task to block within a
 * @link Task_disable @endlink/@link Task_restore @endlink block. For example,
 * @link Semaphore_pend @endlink (if timeout is non-zero),
 * @link Task_sleep @endlink, @link Task_yield @endlink, and Memory_alloc
 * can all cause blocking.
 *
 * @link Task_restore @endlink internally calls Hwi_enable() if the key passed
 * to it results in the unlocking of the Task scheduler (ie if this is root
 * Task_disable/Task_restore pair).
 *
 * @param key key to restore previous Task scheduler state
 */
extern void Task_restore(unsigned int key);

/*! @cond NODOC */
/*!
 * @brief Used by Hwi dispatcher. Does not re-enable interrupts.
 */
extern void Task_restoreHwi(unsigned int key);
/*! @endcond */

/*!
 * @brief Returns a handle to the currently executing Task object.
 *
 * Task_self returns the object handle for the currently executing task. This
 * function is useful when inspecting the object or when the current task
 * changes its own priority through @link Task_setPri @endlink.
 *
 * No task switch occurs when calling Task_self.
 *
 * Task_self will return NULL until Tasking is initiated at the end of
 * BIOS_start().
 * 
 *@retval address of currently executing task object
 */
extern Task_Handle Task_self(void);

/*!
 * @brief Check for stack overflow.
 *
 * This function is usually called by the @link Task_HookSet @endlink switchFxn
 * to make sure task stacks are valid before performing the context switch.
 *
 * If a stack overflow is detected on either the oldTask or the newTask, a
 * Task_E_stackOverflow error is raised and the system exited.
 *
 * In order to work properly, @link Task_checkStacks @endlink requires that the
 * @link Task_initStackFlag @endlink set to true, which it is by default.
 *
 * You can call @link Task_checkStacks @endlink directly from your application.
 * For example, you can check the current task's stack integrity at any time
 * with a call like the following:
 *
 * @code
 *  Task_checkStacks(Task_self(), Task_self());
 * @endcode
 *
 * @param oldTask leaving Task Object void *
 * @param newTask entering Task Object void *
 */
extern void Task_checkStacks(Task_Handle oldTask, Task_Handle newTask);

/*!
 * @brief Terminate execution of the current task.
 *
 * Task_exit terminates execution of the current task, changing its mode from
 * @link Task_Mode_RUNNING @endlink to @link Task_Mode_TERMINATED @endlink. If
 * all tasks have been terminated, or if all remaining tasks have their
 * vitalTaskFlag attribute set to false, then SYS/BIOS terminates the program as
 * a whole by calling the function System_exit with a status code of 0.
 *
 * Task_exit is automatically called whenever a task returns from its top-level
 * function.
 *
 * Exit Hooks (see exitFxn in @link Task_HookSet @endlink) can be used to
 * provide functions that run whenever a task is terminated. The exitFxn Hooks
 * are called before the task has been blocked and marked @link
 * Task_Mode_TERMINATED @endlink. See @ref taskHookFunctions "Hook Functions" for
 * more information.
 *
 * Any SYS/BIOS function can be called from an Exit Hook function.
 *
 * Calling @link Task_self @endlink within an Exit function returns the task
 * being exited. Your Exit function declaration should be similar to the
 * following:
 *
 * @code
 *  void myExitFxn(void);
 * @endcode
 *
 * A task switch occurs when calling Task_exit unless the program as a whole is
 * terminated
 *
 * @pre
 * Task_exit cannot be called from a Swi or Hwi.
 *
 * Task_exit cannot be called from the program's main() function.
 */
extern void Task_exit(void);

/*!
 * @brief Delay execution of the current task.
 *
 * Task_sleep changes the current task's mode from @link Task_Mode_RUNNING
 * @endlink to @link Task_Mode_BLOCKED @endlink, and delays its execution for
 * nticks increments of the "system clock". The actual time
 * delayed can be up to 1 system clock tick less than nticks due to granularity
 * in system timekeeping and the time elapsed per tick is determined by @link
 * Clock_tickPeriod @endlink.
 *
 * After the specified period of time has elapsed, the task reverts to the @link
 * Task_Mode_READY @endlink mode and is scheduled for execution.
 *
 * A task switch always occurs when calling Task_sleep if nticks > 0.
 *
 * @pre
 * Task_sleep cannot be called from a Swi or Hwi, or within a @link Task_disable
 * @endlink / @link Task_restore @endlink block.
 *
 * Task_sleep cannot be called from the program's main() function.
 *
 * Task_sleep should not be called from within an Idle function. Doing so
 * prevents analysis tools from gathering run-time information.
 *
 * nticks cannot be @link BIOS_WAIT_FOREVER @endlink.
 *
 * @param nticks number of system clock ticks to sleep
 */
extern void Task_sleep(uint32_t nticks);

/*!
 * @brief Yield processor to equal priority task.
 *
 * Task_yield yields the processor to another task of equal priority.
 *
 * A task switch occurs when you call Task_yield if there is an equal priority
 * task ready to run.
 *
 * Tasks of higher priority preempt the currently running task without the need
 * for a call to Task_yield. If only lower-priority tasks are ready to run when
 * you call Task_yield, the current task continues to run. Control does not pass
 * to a lower-priority task.
 *
 * @pre
 * When called within an Hwi, the code sequence calling Task_yield must be
 * invoked by the Hwi dispatcher.
 *
 * Task_yield cannot be called from the program's main() function.
 */
extern void Task_yield(void);

/*!
 * @brief returns a handle to the idle task object (for core 0)
 */
extern Task_Handle Task_getIdleTask(void);

/*! @cond NODOC */
/*!
 * @brief Returns arg0 passed via params to create.
 * 
 * @param task Task handle
 *
 * @retval task's arg0
 */
extern uintptr_t Task_getArg0(Task_Handle task);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Returns arg1 passed via params to create.
 * 
 * @param task Task handle
 *
 * @retval task's arg1
 */
extern uintptr_t Task_getArg1(Task_Handle task);
/*! @endcond */

/*!
 * @brief Get task environment pointer.
 *
 * Task_getEnv returns the environment pointer of the specified task. The
 * environment pointer references an arbitrary application-defined data
 * structure.
 *
 * If your program uses multiple hook sets, @link Task_getHookContext @endlink
 * allows you to get environment pointers you have set for a particular hook set
 * and Task object combination.
 * 
 * @param task Task handle
 *
 * @retval task environment pointer
 */
extern void * Task_getEnv(Task_Handle task);

/*!
 * @brief Get Task function and arguments
 *
 * If either arg0 or arg1 is NULL, then the corresponding argument is not
 * returned.
 *
 * @param arg0 pointer for returning Task's first function argument
 *
 * @param arg1 pointer for returning Task's second function argument
 * 
 * @param task Task handle
 *
 * @retval Task function
 */
extern Task_FuncPtr Task_getFunc(Task_Handle task, uintptr_t *arg0, uintptr_t *arg1);

/*!
 * @brief Get hook set's context for a task.
 *
 * For example, this C code gets the HookContext, prints it, and sets a new
 * value for the HookContext.
 *
 * @code
 *  void * pEnv;
 *  Task_Handle myTask;
 *  int myHookSetId1;
 *
 *  pEnv = Task_getHookContext(task, myHookSetId1);
 *
 *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
 *                (ULong)pEnv, (ULong)Timestamp_get32());
 *
 *  Task_setHookContext(task, myHookSetId1, (void *)0xc0de1);
 * @endcode
 *
 * See @ref taskHookFunctions "Hook Functions" for more details.
 *
 * @param task Task handle
 * @param id hook set ID
 * 
 * @retval hook set context for task
 */
extern void * Task_getHookContext(Task_Handle task, int id);

/*!
 * @brief Get task name.
 *
 * Task_getName returns the name of the referenced task.
 * 
 * @param task Task handle
 *
 * @retval task name
 */
extern char * Task_getName(Task_Handle task);

/*!
 * @brief Get task priority.
 *
 * Task_getPri returns the priority of the referenced task.
 * 
 * @param task Task handle
 *
 * @retval task priority
 */
extern int Task_getPri(Task_Handle task);

/*! @cond NODOC */
/*!
 * @brief Set arg0 (used primarily for legacy support)
 */
extern void Task_setArg0(Task_Handle task, uintptr_t arg);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Set arg1 (used primarily for legacy support)
 */
extern void Task_setArg1(Task_Handle task, uintptr_t arg);
/*! @endcond */

/*!
 * @brief Set task environment.
 *
 * Task_setEnv sets the task environment pointer to env. The environment pointer
 * references an arbitrary application-defined data structure.
 *
 * If your program uses multiple hook sets, @link Task_setHookContext @endlink
 * allows you to set environment pointers for any hook set and Task object
 * combination.
 *
 * @param task Task handle
 * @param env task environment pointer
 */
extern void Task_setEnv(Task_Handle task, void * env);

/*!
 * @brief Set hook instance's context for a task.
 *
 * For example, this C code gets the HookContext, prints it, and sets a new
 * value for the HookContext.
 *
 * @code
 *  void * pEnv;
 *  Task_Handle myTask;
 *  int myHookSetId1;
 *
 *  pEnv = Task_getHookContext(task, myHookSetId1);
 *
 *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
 *                (ULong)pEnv, (ULong)Timestamp_get32());
 *
 *  Task_setHookContext(task, myHookSetId1, (void *)0xc0de1);
 * @endcode
 *
 * See @ref taskHookFunctions "Hook Functions" for more details.
 *
 * @param task Task handle
 * @param id hook set ID
 * @param hookContext value to write to context
 */
extern void Task_setHookContext(Task_Handle task, int id, void * hookContext);

/*!
 * @brief Set a task's priority
 *
 * Task_setpri sets the execution priority of task to newpri, and returns that
 * task's old priority value. Raising or lowering a task's priority does not
 * necessarily force preemption and re-scheduling of the caller: tasks in the
 * @link Task_Mode_BLOCKED @endlink mode remain suspended despite a change in
 * priority; and tasks in the @link Task_Mode_READY @endlink mode gain control
 * only if their new priority is greater than that of the currently executing
 * task.
 *
 * newpri should be set to a value greater than or equal to 1 and less than or
 * equal to (@link Task_numPriorities @endlink - 1).  newpri can also be set to
 * -1 which puts the the task into the INACTIVE state and the task will not run
 * until its priority is raised at a later time by another task.  Priority 0 is
 * reserved for the idle task. If newpri equals (@link Task_numPriorities
 * @endlink - 1), execution of the task effectively locks out all other program
 * activity, except for the handling of interrupts.
 *
 * The current task can change its own priority (and possibly preempt its
 * execution) by passing the output of @link Task_self @endlink as the value of
 * the task parameter.
 *
 * A context switch occurs when calling Task_setpri if a currently running task
 * priority is set lower than the priority of another currently ready task, or
 * if another ready task is made to have a higher priority than the currently
 * running task.
 *
 * Task_setpri can be used for mutual exclusion.
 *
 * If a task's new priority is different than its previous priority, then its
 * relative placement in its new ready task priority queue can be different than
 * the one it was removed from. This can effect the relative order in which it
 * becomes the running task.
 *
 * The effected task is placed at the head of its new priority queue if it is
 * the currently running task. Otherwise it is placed at at the end of its new
 * task priority queue.
 *
 * @pre
 * newpri must be a value between 1 and (@link Task_numPriorities @endlink - 1)
 * or -1.
 *
 * The task cannot be in the @link Task_Mode_TERMINATED @endlink mode.
 *
 * The new priority should not be zero (0). This priority level is reserved for
 * the Idle task.
 *
 * @param task Task handle
 * @param newpri task's new priority
 * 
 * @retval task's old priority
 */
extern int Task_setPri(Task_Handle task, int newpri);

/*!
 * @brief Retrieve the status of a task.
 *
 * Task_stat retrieves attribute values and status information about a task.
 *
 * Status information is returned through statbuf, which references a structure
 * of type @link Task_Stat @endlink.
 *
 * When a task is preempted by a software or hardware interrupt, the task
 * execution mode returned for that task by Task_stat is still @link
 * Task_Mode_RUNNING @endlink  because the task runs when the preemption ends.
 *
 * The current task can inquire about itself by passing the output of @link
 * Task_self @endlink as the first argument to Task_stat. However, the task
 * stack pointer (sp) in the @link Task_Stat @endlink structure is the value
 * from the previous context switch.
 *
 * Task_stat has a non-deterministic execution time. As such, it is not
 * recommended to call this API from Swis or Hwis.
 *
 * @pre
 * statbuf cannot be NULL;
 *
 * @param task Task handle
 * @param statbuf pointer to task status structure
 */
extern void Task_stat(Task_Handle task, Task_Stat *statbuf);

/*!
 * @brief Retrieve the @link Task_Mode @endlink of a task.
 *
 * @param task Task handle
 */
extern Task_Mode Task_getMode(Task_Handle task);

/*!
 * @brief Block a task.
 *
 * Remove a task from its ready list. The effect of this API is manifest the
 * next time the internal Task scheduler is invoked. This can be done directly
 * by embedding the call within a @link Task_disable @endlink/@link Task_restore
 * @endlink block. Otherwise, the effect will be manifest as a result of
 * processing the next dispatched interrupt, or by posting a Swi, or by falling
 * through the task function.
 *
 * @pre
 * If called from within a Hwi or a Swi, or main(), there is no need to embed
 * the call within a @link Task_disable @endlink/@link Task_restore @endlink
 * block.
 *
 * @param task Task handle
 */
extern void Task_block(Task_Handle task);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Unblock a task.
 *
 * Place task in its ready list. The effect of this API is manifest the next
 * time the internal Task scheduler is invoked. This can be done directly by
 * embedding the call within a @link Task_disable @endlink/@link Task_restore
 * @endlink block. Otherwise, the effect will be manifest as a result of
 * processing the next dispatched interrupt, or by posting a Swi, or by falling
 * through the task function.
 *
 * @pre
 * If called from within a Hwi or a Swi, or main(), there is no need to embed
 * the call within a @link Task_disable @endlink/@link Task_restore @endlink
 * block.
 *
 * @param task Task handle
 */
extern void Task_unblock(Task_Handle task);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Block a task.
 *
 * Remove a task from its ready list. Must be called within
 * Task_disable/Task_restore block with interrupts disabled. This API is meant
 * to be used internally.
 *
 * @param task Task handle
 */
extern void Task_blockI(Task_Handle task);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Unblock a task.
 *
 * Place task in its ready list. Must be called within Task_disable/Task_restore
 * block with interrupts disabled. This API is meant to be used internally.
 *
 * @param task Task handle
 * @param hwiKey key returned from Hwi_disable()
 */
extern void Task_unblockI(Task_Handle task, unsigned int hwiKey);

extern void Task_schedule(void);

extern void Task_enter(void);

extern void Task_enterUnpriv(void);

extern void Task_sleepTimeout(uintptr_t arg);

extern int Task_postInit(Task_Object *task, Error_Block *eb);

extern void Task_allBlockedFunction(void);

extern void Task_deleteTerminatedTasksFunc(void);

extern void Task_processVitalTaskFlag(Task_Object *task);

extern int Task_moduleStateCheck(Task_Module_State *moduleState, uint32_t checkValue);

extern uint32_t Task_getModuleStateCheckValue(Task_Module_State *moduleState);

extern int Task_objectCheck(Task_Handle handle, uint32_t checkValue);

extern uint32_t Task_getObjectCheckValue(Task_Handle handle);

extern void Task_enableOtherCores(void);
/*! @endcond */

/*!
 * @brief Initialize the Task_Params structure with default values.
 *
 * Task_Params_init initializes the Task_Params structure with default values.
 * Task_Params_init should always be called before setting individual parameter
 * fields. This allows new fields to be added in the future with compatible
 * defaults -- existing source code does not need to change when new fields
 * are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Task_Params_init(Task_Params *prms);

/*!
 * @brief return handle of the first task on task list 
 *
 * Return the handle of the first task on the create/construct list. NULL if no
 * Tasks have been created or constructed.
 *
 * @retval Task handle
 */
extern Task_Handle Task_Object_first(void);

/*!
 * @brief return handle of the next task on task list 
 *
 * Return the handle of the next task on the create/construct list. NULL if no
 * more Tasks are on the list.
 *
 * @param task Task handle
 *
 * @retval Task handle
 */
extern Task_Handle Task_Object_next(Task_Handle task);

/* @cond NODOC */
extern Task_Module_State Task_Module_state;

#define Task_module ((Task_Module_State *) &(Task_Module_state))

#define Task_Object_heap() NULL

static inline char * Task_Handle_name(Task_Handle obj)
{
    return (obj->name);
}

static inline Task_Handle Task_handle(Task_Struct *str)
{  
    return ((Task_Handle)str);
}

static inline Task_Struct * Task_struct(Task_Handle h)
{  
    return ((Task_Struct *)h);
}
/* @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_knl_Task__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Task_long_names
#include "Task_defs.h"
/*! @endcond */

