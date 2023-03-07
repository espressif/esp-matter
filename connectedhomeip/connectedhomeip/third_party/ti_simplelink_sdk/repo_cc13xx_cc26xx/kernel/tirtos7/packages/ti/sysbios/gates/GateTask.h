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
 * @file ti/sysbios/gates/GateTask.h
 * @brief Software Interrupt Gate.
 *
 * GateTask uses Task_disable() and Task_restore() as the resource locking
 * mechanism. This gate can be used whenever the resource is being shared by
 * Tasks.
 *
 * The duration between the enter and leave should be as short as possible to
 * minimize Task scheduling latency.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th>
 *    <th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                               -->
 *    <tr><td> Params_init </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> query       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> construct   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> create      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> delete      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> destruct    </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> enter       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   Y**  </td></tr>
 *    <tr><td> leave       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   Y**  </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. GateHwi_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. GateHwi_Module_startupDone() returns false).</li>
 *           </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Must be used in enter/leave pairs. </li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

#ifndef ti_sysbios_gates_GateTask__include
#define ti_sysbios_gates_GateTask__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Error.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_gates_GateTask_long_names
#include "GateTask_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GateTask_Params GateTask_Params;
typedef struct GateTask_Struct GateTask_Struct;
typedef struct GateTask_Struct GateTask_Object;
typedef GateTask_Object* GateTask_Handle;

struct GateTask_Params {
    uint32_t dummy;     /* to make IAR happy */
};

struct GateTask_Struct {
    unsigned int dummy;
};

/*!
 * @brief query Gate 'preempt' characteristics
 *
 * GateTask can be preempted by an Hwi or Swi interrupt. Always returns `true`.
 *
 * @retval true
 */
extern bool GateTask_canBePreempted(void);

/*!
 * @brief query Gate 'blocking' characteristics
 *
 * GateTask enter will never block. This API always returns `false`.
 *
 * @retval false
 */
extern bool GateTask_canBlock(void);

/*!
 * @brief Create a `GateTask` gate
 *
 * @param prms optional parameters
 * @param eb error block
 *
 * @retval GateTask handle (NULL on failure)
 */
extern GateTask_Handle GateTask_create(const GateTask_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a `GateTask` gate
 *
 * GateTask_construct is equivalent to GateHwi_create except that the 
 * GateTask_Struct is pre-allocated.
 *
 * @param obj pointer to a GateTask object
 * @param prms optional parameters
 *
 * @retval GateTask handle (NULL on failure)
 */
extern GateTask_Handle GateTask_construct(GateTask_Struct *obj, const GateTask_Params *prms);

/*!
 * @brief Delete a `GateTask` gate
 *
 * Note that GateTask_delete takes a pointer to a GateHwi_Handle which enables
 * GateTask_delete to set the GateHwi handle to NULL.
 *
 * @param gate pointer to a GateTask handle
 */
extern void GateTask_delete(GateTask_Handle *gate);

/*!
 * @brief Destruct a `GateTask` gate
 *
 * @param obj pointer to a GateTask objects
 */
extern void GateTask_destruct(GateTask_Struct *obj);

/*!
 * @brief Initialize the GateTask_Params structure with default values.
 *
 * GateTask_Params_init initializes the GateHwi_Params structure with
 * default values. GateTask_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void GateTask_Params_init(GateTask_Params *prms);

/*!
 * @brief enter the GateTask gate
 *
 * GateTask_enter enters the GateHwi gate and returns a key for use
 * by GateTask_enter. GateHwi_enter calls can be nested and the key is
 * used to unlock the gate on the final call to GateTask_leave.
 *
 * @param gate GateTask handle
 *
 * @retval opaque key to be passed to GateTask_leave()
 */
extern intptr_t GateTask_enter(GateTask_Handle gate);

/*!
 * @brief leave the GateTask gate
 *
 * GateTask_leave exits the GateHwi gate if key matches the outermost
 * call to GateTask_enter. If GateHwi_enter calls are nested, the key
 * will keep the nested calls from leaving the gate.
 *
 * @param gate GateTask handle
 * @param key opaque key to unlock the gate
 */
extern void GateTask_leave(GateTask_Handle gate, intptr_t key);

/*! @cond NODOC */
static inline GateTask_Handle GateTask_handle(GateTask_Struct *str)
{  
    return ((GateTask_Handle)str);
}

static inline GateTask_Struct * GateTask_struct(GateTask_Handle h)
{
    return ((GateTask_Struct *)h);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_gates_GateTask__include */

/*! @cond NODOC */
#undef ti_sysbios_gates_GateTask_long_names
#include "GateTask_defs.h"
/*! @endcond */
