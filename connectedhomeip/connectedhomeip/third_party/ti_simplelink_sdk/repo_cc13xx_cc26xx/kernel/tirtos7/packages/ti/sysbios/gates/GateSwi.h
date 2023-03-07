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
 * @file ti/sysbios/gates/GateSwi.h
 * @brief Software Interrupt Gate.
 *
 * GateSwi uses Swi_disable() and Swi_restore() as the resource locking
 * mechanism. This gate can be used whenever the resource is being shared by
 * Tasks or Swis.
 *
 * The duration between the enter and leave should be as short as possible to
 * minimize Swi latency.
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
 *    <tr><td> enter       </td><td>   N    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   Y**  </td></tr>
 *    <tr><td> leave       </td><td>   N    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y**  </td><td>   Y**  </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. GateSwi_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. GateSwi_Module_startupDone() returns false).</li>
 *           </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Must be used in enter/leave pairs. </li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

#ifndef ti_sysbios_gates_GateSwi__include
#define ti_sysbios_gates_GateSwi__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Swi.h>

#include <ti/sysbios/runtime/Error.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_gates_GateSwi_long_names
#include "GateSwi_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct GateSwi_Params GateSwi_Params;
typedef struct GateSwi_Struct GateSwi_Struct;
typedef struct GateSwi_Struct GateSwi_Object;
typedef GateSwi_Object* GateSwi_Handle;

struct GateSwi_Params {
    uint32_t dummy;     /* to make IAR happy */
};

struct GateSwi_Struct {
    unsigned int dummy;
};

/*!
 * @brief query Gate 'preempt' characteristics
 *
 * GateSwi can be preempted by an hardware interrupt. Always returns `true`.
 *
 * @retval true
 */
extern bool GateSwi_canBePreempted(void);

/*!
 * @brief query Gate 'blocking' characteristics
 *
 * GateSwi enter will never block. This API always returns `false`.
 *
 * @retval false
 */
extern bool GateSwi_canBlock(void);

/*!
 * @brief Create a `GateSwi` gate
 *
 * @param prms optional parameters
 * @param eb error block
 *
 * @retval GateSwi handle (NULL on failure)
 */
extern GateSwi_Handle GateSwi_create(const GateSwi_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a `GateSwi` gate
 *
 * GateSwi_construct is equivalent to GateHwi_create except that the 
 * GateSwi_Struct is pre-allocated.
 *
 * @param obj pointer to a GateSwi object
 * @param prms optional parameters
 *
 * @retval GateSwi handle (NULL on failure)
 */
extern GateSwi_Handle GateSwi_construct(GateSwi_Struct *obj, const GateSwi_Params *prms);

/*!
 * @brief Delete a `GateSwi` gate
 *
 * Note that GateSwi_delete takes a pointer to a GateHwi_Handle which enables
 * GateSwi_delete to set the GateHwi handle to NULL.
 *
 * @param gate pointer to a GateSwi handle
 */
extern void GateSwi_delete(GateSwi_Handle *gate);

/*!
 * @brief Destruct a `GateSwi` gate
 *
 * @param obj pointer to a GateSwi objects
 */
extern void GateSwi_destruct(GateSwi_Struct *obj);

/*!
 * @brief Initialize the GateSwi_Params structure with default values.
 *
 * GateSwi_Params_init initializes the GateHwi_Params structure with
 * default values. GateSwi_Params_init should always be called before
 * setting individual parameter fields. This allows new fields to be
 * added in the future with compatible defaults -- existing source code
 * does not need to change when new fields are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void GateSwi_Params_init(GateSwi_Params *prms);

/*!
 * @brief enter the GateSwi gate
 *
 * GateSwi_enter enters the GateHwi gate and returns a key for use
 * by GateSwi_enter. GateHwi_enter calls can be nested and the key is
 * used to unlock the gate on the final call to GateSwi_leave.
 *
 * @param gate GateSwi handle
 *
 * @retval opaque key to be passed to GateSwi_leave()
 */
extern intptr_t GateSwi_enter(GateSwi_Handle gate);

/*!
 * @brief leave the GateSwi gate
 *
 * GateSwi_leave exits the GateHwi gate if key matches the outermost
 * call to GateSwi_enter. If GateHwi_enter calls are nested, the key
 * will keep the nested calls from leaving the gate.
 *
 * @param gate GateSwi handle
 * @param key opaque key to unlock the gate
 */
extern void GateSwi_leave(GateSwi_Handle gate, intptr_t key);

/*! @cond NODOC */
extern bool GateSwi_query(int qual);

static inline GateSwi_Handle GateSwi_handle(GateSwi_Struct *str)
{  
    return ((GateSwi_Handle)str);
}

static inline GateSwi_Struct * GateSwi_struct(GateSwi_Handle h)
{
    return ((GateSwi_Struct *)h);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_gates_GateSwi__include */

/*! @cond n NODOC*/
#undef ti_sysbios_gates_GateSwi_long_names
#include "GateSwi_defs.h"
/*! @endcond */
