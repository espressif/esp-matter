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
 * @file ti/sysbios/gates/GateHwi.h
 * @brief Hardware Interrupt Gate.
 *
 * GateHwi uses disabling and enabling of interrupts as the resource locking
 * mechanism. Such a gate guarantees exclusive access to the CPU. This gate can
 * be used whenever the resource is being shared by Tasks, Swis, and/or Hwis.
 *
 * The duration between the enter and leave should be as short as possible to
 * minimize Hwi latency.
 *
 * GateHwi inherits from xdc.runtime.IGateProvider, therefore supports nesting.
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
 *    <tr><td> enter       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   Y**  </td></tr>
 *    <tr><td> leave       </td><td>   Y    </td><td>   Y    </td>
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

#ifndef ti_sysbios_gates_GateHwi__include
#define ti_sysbios_gates_GateHwi__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/runtime/Error.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_gates_GateHwi_long_names
#include "GateHwi_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GateHwi_Params GateHwi_Params;
typedef struct GateHwi_Struct GateHwi_Struct;
typedef struct GateHwi_Struct GateHwi_Object;
typedef GateHwi_Object* GateHwi_Handle;

struct GateHwi_Params {
    uint32_t dummy;     /* to make IAR happy */
};

struct GateHwi_Struct {
    unsigned int dummy;
};

/*!
 * @brief query Gate 'preempt' characteristics
 *
 * GateHwi cannot be preempted. This API always returns `false`.
 *
 * @retval false
 */
extern bool GateHwi_canBePreempted(void);

/*!
 * @brief query Gate 'blocking' characteristics
 *
 * GateHwi enter will never block. This API always returns `false`.
 *
 * @retval false
 */
extern bool GateHwi_canBlock(void);

/*!
 * @brief Create a `GateHwi` gate
 *
 * @param prms optional parameters
 * @param eb error block
 *
 * @retval GateHwi handle (NULL on failure)
 */
extern GateHwi_Handle GateHwi_create(const GateHwi_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a `GateHwi` gate
 *
 * GateHwi_construct is equivalent to GateHwi_create except that the 
 * GateHwi_Struct is pre-allocated.
 *
 * @param obj pointer to a GateHwi object
 * @param prms optional parameters
 *
 * @retval GateHwi handle (NULL on failure)
 */
extern GateHwi_Handle GateHwi_construct(GateHwi_Struct *obj, const GateHwi_Params *prms);

/*!
 * @brief Delete a `GateHwi` gate
 *
 * Note that GateHwi_delete takes a pointer to a GateHwi_Handle which enables
 * GateHwi_delete to set the GateHwi handle to NULL.
 *
 * @param gate pointer to a GateHwi handle
 */
extern void GateHwi_delete(GateHwi_Handle *gate);

/*!
 * @brief Destruct a `GateHwi` gate
 *
 * @param obj pointer to a GateHwi objects
 */
extern void GateHwi_destruct(GateHwi_Struct *obj);

/*!
 * @brief Initialize the GateHwi_Params structure with default values.
 *
 * GateHwi_Params_init initializes the GateHwi_Params structure with
 * default values. GateHwi_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void GateHwi_Params_init(GateHwi_Params *prms);

/*!
 * @brief enter the GateHwi gate
 *
 * GateHwi_enter enters the GateHwi gate and returns a key for use
 * by GateHwi_enter. GateHwi_enter calls can be nested and the key is
 * used to unlock the gate on the final call to GateHwi_leave.
 *
 * @param gate GateHwi handle
 *
 * @retval opaque key to be passed to GateHwi_leave()
 */
extern intptr_t GateHwi_enter(GateHwi_Handle gate);

/*!
 * @brief leave the GateHwi gate
 *
 * GateHwi_leave exits the GateHwi gate if key matches the outermost
 * call to GateHwi_enter. If GateHwi_enter calls are nested, the key
 * will keep the nested calls from leaving the gate.
 *
 * @param gate GateHwi handle
 * @param key opaque key to unlock the gate
 */
extern void GateHwi_leave(GateHwi_Handle gate, intptr_t key);

/*! @cond NODOC */
extern bool GateHwi_query(int qual);

static inline GateHwi_Handle GateHwi_handle(GateHwi_Struct *str)
{  
    return ((GateHwi_Handle)str);
}

static inline GateHwi_Struct * GateHwi_struct(GateHwi_Handle h)
{
    return ((GateHwi_Struct *)h);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_gates_GateHwi__include */

/*! @cond NODOC */
#undef ti_sysbios_gates_GateHwi_long_names
#include "GateHwi_defs.h"
/*! @endcond */
