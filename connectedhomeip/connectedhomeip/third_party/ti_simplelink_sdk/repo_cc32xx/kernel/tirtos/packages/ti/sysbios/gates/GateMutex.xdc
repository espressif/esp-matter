/*
 * Copyright (c) 2013, Texas Instruments Incorporated
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
 *  ======== GateMutex.xdc ========
 *
 */
package ti.sysbios.gates;

import xdc.rov.ViewInfo;
import xdc.runtime.Assert;

/*!
 *  ======== GateMutex ========
 *  Mutex Gate.
 *
 *  GateMutex uses a Semaphore as the resource locking mechanism. Each
 *  GateMutex instance has its own unique Semaphore. This gate can only be
 *  used by a Task as a gate can potentially block. This gate cannot be used
 *  by a Hwi or Swi. 
 *
 *  The task that uses a gate can call enter() any number of times without risk
 *  of being blocked, although relinquishing ownership of the gate requires a 
 *  balanced number of calls to leave().
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th>
 *    <th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                        -->
 *    <tr><td> {@link #Params_init} </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #query}       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}   </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}      </td><td>   N*   </td><td>   N*   </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}    </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #enter}       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #leave}       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started 
 *    (e.g. GateMutex_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started 
 *    (e.g. GateMutex_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       <li> <b>*</b>:  Assuming blocking Heap is used for creation. </li>
 *       <li> <b>**</b>: Must be used in enter/leave pairs. </li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p 
 */

@InstanceFinalize       /* destruct semaphore */

module GateMutex inherits xdc.runtime.IGateProvider
{

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String status;
        String owner;
        String pendedTasks[];
    }
    
    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo = 
        ViewInfo.create({
            viewMap: [
                ['Basic', {type: ViewInfo.INSTANCE, 
                   viewInitFxn: 'viewInitBasic', 
                   structName: 'BasicView'}]
            ]
        });
    
    /*!
     *  Assert when GateMutex_enter() is not called from correct context.
     *  GateMutex_enter() can only be called from main() or Task context (not
     *  Hwi or Swi).
     *
     *  Common causes and workarounds for hitting this Assert:
     *
     *  - Calling printf() from a Hwi or Swi thread.
     *  @p(blist)
     *          - Use xdc.runtime.System_printf (with SysMin) instead.
     *  @p
     *  - Calling System_printf() from a Hwi or Swi thread when using SysStd.
     *  @p(blist)
     *          - Use xdc.runtime.SysMin instead of xdc.runtume.SysStd.
     *          - Use a different type of Gate for 
     *            {@link ti.sysbios.BIOS#rtsGateType BIOS.rtsGateType} 
     *            (ie {@link ti.sysbios.BIOS#GateHwi BIOS.GateHwi} 
     *            or {@link ti.sysbios.BIOS#GateSwi BIOS.GateSwi})
     *  @p
     *  - Calling Memory_alloc() from a Hwi or Swi thread.
     *  @p(blist)
     *          - Use a different Heap manager
     *  @p
     */
    config Assert.Id A_badContext = {
        msg: "A_badContext: bad calling context. See GateMutex API doc for details."
    };

instance:

    override IArg enter();

    override Void leave(IArg key);

internal:

    /* -------- Internal Structures -------- */
    struct Instance_State {
        ti.sysbios.knl.Task.Handle         owner;
        ti.sysbios.knl.Semaphore.Object    sem;
    };

}
