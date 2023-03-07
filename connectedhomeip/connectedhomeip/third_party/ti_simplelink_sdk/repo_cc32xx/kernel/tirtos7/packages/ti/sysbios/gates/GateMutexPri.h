/*
 * Copyright (c) 2020, Texas Instruments Incorporated
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
/*!
 * @file ti/sysbios/gates/GateMutexPri.h
 * @brief Mutex Gate with priority inheritance.
 *
 * GateMutexPri is a mutex gate (it can only be held by one thread at a time)
 * which implements priority inheritance in order to prevent priority inversion.
 * Priority inversion occurs when a high priority task has its priority
 * effectively 'inverted' because it is waiting on a gate held by a low priority
 * task.
 *
 * When multiple tasks wait on this gate, they will receive the gate in order of
 * priority (higher priority tasks will receive the gate first). This is because
 * the queue of tasks waiting on a GateMutexPri is sorted by priority, not FIFO.
 *
 * @htmlonly
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
 * <h3> Calling Context </h3>
 * <table border="1" cellpadding="3">
 *   <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 * </colgroup>
 *
 * <tr><th> Function </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th>
 * <th>  Main  </th><th>  Startup  </th></tr> <!--
 * --> <tr><td> @link GateMutexPri_Params_init @endlink </td><td>   Y
 * </td><td>   Y    </td> <td>   Y    </td><td>   Y    </td><td>   Y
 * </td></tr> <tr><td> @link GateMutexPri_query @endlink       </td><td>   Y
 * </td><td>   Y    </td> <td>   Y    </td><td>   Y    </td><td>   Y
 * </td></tr> <tr><td> @link GateMutexPri_construct @endlink   </td><td>   Y
 * </td><td>   Y    </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> @link GateMutexPri_create @endlink      </td><td>   N*
 * </td><td>   N*   </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> @link GateMutexPri_delete @endlink      </td><td>   N*
 * </td><td>   N*   </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> @link GateMutexPri_destruct @endlink    </td><td>   Y
 * </td><td>   Y    </td> <td>   Y    </td><td>   Y    </td><td>   N
 * </td></tr> <tr><td> @link GateMutexPri_enter @endlink       </td><td>   N
 * </td><td>   N    </td> <td>   Y    </td><td>   Y**  </td><td>   N
 * </td></tr> <tr><td> @link GateMutexPri_leave @endlink       </td><td>   N
 * </td><td>   N    </td> <td>   Y    </td><td>   Y**  </td><td>   N
 * </td></tr> <tr><td colspan="6"> Definitions: <br /> <ul> <li> <b>Hwi</b>: API
 * is callable from a Hwi thread. </li> <li> <b>Swi</b>: API is callable from a
 * Swi thread. </li> <li> <b>Task</b>: API is callable from a Task thread. </li>
 * <li> <b>Main</b>: API is callable during any of these phases: </li> <ul> <li>
 * In your module startup after this module is started (e.g.
 * GateMutexPri_Module_startupDone() returns true). </li> <li> During
 * xdc.runtime.Startup.lastFxns. </li> <li> During main().</li> <li> During
 * BIOS.startupFxns.</li> </ul> <li> <b>Startup</b>: API is callable during any
 * of these phases:</li> <ul> <li> During xdc.runtime.Startup.firstFxns.</li>
 * <li> In your module startup before this module is started (e.g.
 * GateMutexPri_Module_startupDone() returns false).</li> </ul> <li> <b>*</b>:
 * Assuming blocking Heap is used for creation. </li> <li> <b>**</b>: Must be
 * used in enter/leave pairs. </li> </ul> </td></tr>
 *
 * </table>
 *
 * @endhtmlonly
 */

#ifndef ti_sysbios_gates_GateMutexPri__include
#define ti_sysbios_gates_GateMutexPri__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Error.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_gates_GateMutexPri_long_names
#include "GateMutexPri_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GateMutexPri_Params GateMutexPri_Params;
typedef struct GateMutexPri_Struct GateMutexPri_Struct;
typedef struct GateMutexPri_Struct GateMutexPri_Object;
typedef GateMutexPri_Object* GateMutexPri_Handle;

struct GateMutexPri_Params {
    uint32_t dummy;     /* to make IAR happy */
};

struct GateMutexPri_Struct {
    Queue_Elem objElem;
    volatile unsigned int mutexCnt;
    volatile int ownerOrigPri;
    volatile Task_Handle owner;
    Queue_Object pendQ;
};

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} GateMutexPri_Module_State;
/*! @endcond */

/*!
 * @brief query Gate 'preempt' characteristics
 *
 * GateMutexPri can be preempted. This API always returns `true`.
 *
 * @retval true
 */
extern bool GateMutexPri_canBePreempted(void);

/*!
 * @brief query Gate 'blocking' characteristics
 *
 * GateMutexPri enter can block if another task owns the gate. This API always
 * returns `true`.
 *
 * @retval true
 */
extern bool GateMutexPri_canBlock(void);

/*!
 * @brief Create a `GateMutexPri` gate
 *
 * @param prms optional parameters
 * @param eb error block
 *
 * @retval GateMutexPri handle (NULL on failure)
 */
extern GateMutexPri_Handle GateMutexPri_create(const GateMutexPri_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a `GateMutexPri` gate
 *
 * GateMutexPri_construct is equivalent to GateMutexPri_create except that the 
 * GateMutexPri_Struct is pre-allocated.
 *
 * @param obj pointer to a GateMutexPri object
 * @param prms optional parameters
 *
 * @retval GateMutexPri handle (NULL on failure)
 */
extern GateMutexPri_Handle GateMutexPri_construct(GateMutexPri_Struct *obj, const GateMutexPri_Params *prms);

/*!
 * @brief Delete a `GateMutexPri` gate
 *
 * Note that GateMutexPri_delete takes a pointer to a GateMutexPri_Handle which enables
 * GateMutexPri_delete to set the GateMutexPri handle to NULL.
 *
 * @param gate pointer to a GateMutexPri handle
 */
extern void GateMutexPri_delete(GateMutexPri_Handle *gate);

/*!
 * @brief Destruct a `GateMutexPri` gate
 *
 * @param obj pointer to a GateMutexPri objects
 */
extern void GateMutexPri_destruct(GateMutexPri_Struct *obj);

/*!
 * @brief Initialize the GateMutexPri_Params structure with default values.
 *
 * GateMutexPri_Params_init initializes the GateMutexPri_Params structure with
 * default values. GateMutexPri_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void GateMutexPri_Params_init(GateMutexPri_Params *prms);

/*!
 * @brief enter the GateMutexPri gate
 *
 * GateMutexPri_enter enters the GateMutexPri gate and returns a key for use
 * by GateMutexPri_enter. GateMutexPri_enter calls can be nested and the key is
 * used to unlock the gate on the final call to GateMutexPri_leave.
 *
 * @param gate GateMutexPri handle
 *
 * @retval opaque key to be passed to GateMutexPri_leave()
 */
extern intptr_t GateMutexPri_enter(GateMutexPri_Handle gate);

/*!
 * @brief leave the GateMutexPri gate
 *
 * GateMutexPri_leave exits the GateMutexPri gate if key matches the outermost
 * call to GateMutexPri_enter. If GateMutexPri_enter calls are nested, the key
 * will keep the nested calls from leaving the gate.
 *
 * @param gate GateMutexPri handle
 * @param key opaque key to unlock the gate
 */
extern void GateMutexPri_leave(GateMutexPri_Handle gate, intptr_t key);

/*!
 * @brief return handle of the first GateMutexPri on GateMutexPri list
 *
 * Return the handle of the first GateMutexPri on the create/construct list.
 * NULL if no GateMutexPris have been created or constructed.
 *
 * @retval GateMutexPri handle
 */
extern GateMutexPri_Handle GateMutexPri_Object_first(void);

/*!
 * @brief return handle of the next GateMutexPri on GateMutexPri list
 *
 * Return the handle of the next GateMutexPri on the create/construct list.
 * NULL if no more GateMutexPris are on the list.
 *
 * @param gate GateMutexPri handle
 *
 * @retval GateMutexPri handle
 */
extern GateMutexPri_Handle GateMutexPri_Object_next(GateMutexPri_Handle gate);

/*! @cond NODOC */
extern bool GateMutexPri_query(int qual);
static inline GateMutexPri_Handle GateMutexPri_handle(GateMutexPri_Struct *str)
{
    return ((GateMutexPri_Handle)str);
}

static inline GateMutexPri_Struct * GateMutexPri_struct(GateMutexPri_Handle h)
{
    return ((GateMutexPri_Struct *)h);
}

#define GateMutexPri_module ((GateMutexPri_Module_State *) &(GateMutexPri_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_gates_GateMutexPri__include */

/*! @cond NODOC */
#undef ti_sysbios_gates_GateMutexPri_long_names
#include "GateMutexPri_defs.h"
/*! @endcond */
