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
 * @file ti/sysbios/gates/GateMutex.h
 * @brief Mutex Gate.
 *
 * GateMutex uses a Semaphore as the resource locking mechanism. Each GateMutex
 * instance has its own unique Semaphore. This gate can only be used by a Task
 * as a gate can potentially block. This gate cannot be used by a Hwi or Swi.
 *
 * The task that uses a gate can call enter() any number of times without risk
 * of being blocked, although relinquishing ownership of the gate requires a
 * balanced number of calls to leave().
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th>
 *    <th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                        -->
 *    <tr><td> Params_init </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> query       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> construct   </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> create      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> delete      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> destruct    </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> enter       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> leave       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. GateMutex_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. GateMutex_Module_startupDone() returns false).</li>
 *           </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Must be used in enter/leave pairs. </li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

#ifndef ti_sysbios_gates_GateMutex__include
#define ti_sysbios_gates_GateMutex__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Error.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_gates_GateMutex_long_names
#include "GateMutex_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Assert when GateMutex_enter() is not called from correct context.
 * GateMutex_enter() can only be called from main() or Task context (not Hwi or
 * Swi).
 *
 * Common causes and workarounds for hitting this Assert:
 * - Calling printf() from a Hwi or Swi thread. Use System_printf (with SysMin)
 * instead.
 * - Calling Memory_alloc() from a Hwi or Swi thread. Use a different Heap manager
 * or change the memory manager's gate type.
 */
#define GateMutex_A_badContext "bad calling context"

typedef struct GateMutex_Params GateMutex_Params;
typedef struct GateMutex_Struct GateMutex_Struct;
typedef struct GateMutex_Struct GateMutex_Object;
typedef GateMutex_Object* GateMutex_Handle;

struct GateMutex_Params {
    uint32_t dummy;     /* to make IAR happy */
};

struct GateMutex_Struct {
    Queue_Elem objElem;
    Task_Handle owner;
    Semaphore_Struct sem;
};

/*! @cond NODOC */
typedef struct {
    Queue_Struct objQ;
} GateMutex_Module_State;
/*! @endcond */

/*!
 * @brief query Gate 'preempt' characteristics
 *
 * GateMutex can be preempted. This API always returns `true`.
 *
 * @retval true
 */
extern bool GateMutex_canBePreempted(void);

/*!
 * @brief query Gate 'blocking' characteristics
 *
 * GateMutex enter can block if another task owns the gate. This API always
 * returns `true`.
 *
 * @retval true
 */
extern bool GateMutex_canBlock(void);

/*!
 * @brief Create a `GateMutex` gate
 *
 * @param prms optional parameters
 * @param eb error block
 *
 * @retval GateMutex handle (NULL on failure)
 */
extern GateMutex_Handle GateMutex_create(const GateMutex_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a `GateMutex` gate
 *
 * GateMutex_construct is equivalent to GateMutex_create except that the 
 * GateMutex_Struct is pre-allocated.
 *
 * @param obj pointer to a GateMutex object
 * @param prms optional parameters
 *
 * @retval GateMutex handle (NULL on failure)
 */
extern GateMutex_Handle GateMutex_construct(GateMutex_Struct *obj, const GateMutex_Params *prms);

/*!
 * @brief Delete a `GateMutex` gate
 *
 * Note that GateMutex_delete takes a pointer to a GateMutex_Handle which enables
 * GateMutex_delete to set the GateMutex handle to NULL.
 *
 * @param gate pointer to a GateMutex handle
 */
extern void GateMutex_delete(GateMutex_Handle *gate);

/*!
 * @brief Destruct a `GateMutex` gate
 *
 * @param obj pointer to a GateMutex objects
 */
extern void GateMutex_destruct(GateMutex_Struct *obj);

/*!
 * @brief Initialize the GateMutex_Params structure with default values.
 *
 * GateMutex_Params_init initializes the GateMutex_Params structure with
 * default values. GateMutex_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void GateMutex_Params_init(GateMutex_Params *prms);

/*!
 * @brief enter the GateMutex gate
 *
 * GateMutex_enter enters the GateMutex gate and returns a key for use
 * by GateMutex_enter. GateMutex_enter calls can be nested and the key is
 * used to unlock the gate on the final call to GateMutex_leave.
 *
 * @param gate GateMutex handle
 *
 * @retval opaque key to be passed to GateMutex_leave()
 */
extern intptr_t GateMutex_enter(GateMutex_Handle gate);

/*!
 * @brief leave the GateMutex gate
 *
 * GateMutex_leave exits the GateMutex gate if key matches the outermost
 * call to GateMutex_enter. If GateMutex_enter calls are nested, the key
 * will keep the nested calls from leaving the gate.
 *
 * @param gate GateMutex handle
 * @param key opaque key to unlock the gate
 */
extern void GateMutex_leave(GateMutex_Handle gate, intptr_t key);

/*!
 * @brief return handle of the first GateMutex on GateMutex list
 *
 * Return the handle of the first GateMutex on the create/construct list. NULL if
 * no GateMutex instances have been created or constructed.
 *
 * @retval GateMutex handle
 */
extern GateMutex_Handle GateMutex_Object_first(void);

/*!
 * @brief return handle of the next GateMutex on GateMutex list
 *
 * Return the handle of the next GateMutex on the create/construct list. NULL if
 * no more GateMutexs are on the list.
 *
 * @param gate GateMutex handle
 *
 * @retval GateMutex handle
 */
extern GateMutex_Handle GateMutex_Object_next(GateMutex_Handle gate);

/*! @cond NODOC */
extern bool GateMutex_query(int qual);

static inline GateMutex_Handle GateMutex_handle(GateMutex_Struct *str)
{
    return ((GateMutex_Handle)str);
}

static inline GateMutex_Struct * GateMutex_struct(GateMutex_Handle h)
{
    return ((GateMutex_Struct *)h);
}

extern void GateMutex_Instance_init(GateMutex_Object *obj, const GateMutex_Params *prms);
extern void GateMutex_Instance_finalize(GateMutex_Object *obj);

#define GateMutex_module ((GateMutex_Module_State *) &(GateMutex_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_gates_GateMutex__include */

/*! @cond NODOC */
#undef ti_sysbios_gates_GateMutex_long_names
#include "GateMutex_defs.h"
/*! @endcond */
