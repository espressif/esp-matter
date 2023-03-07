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
 *  ======== Timer.xdc ========
 *
 */
package ti.sysbios.hal;

import xdc.runtime.Error;

/*!
 *  ======== Timer ========
 *  Timer Manager Proxy.
 *
 *  The ti.sysbios.hal.Timer module presents a standard interface for using 
 *  the timer peripherals. This module is part of the Hardware Abstraction  
 *  Layer (HAL) package, which hides target/device-specific characteristics  
 *  of the hardware. 
 *   
 *  You can use this module to create a timer (that is, to mark a timer for  
 *  use) and configure it to call a tickFxn when the timer expires. Use this 
 *  module only if you do not need to do any custom configuration of the  
 *  timer peripheral. 
 *   
 *  The timer can be configured as a one-shot or a continuous mode timer. 
 *  The period can be specified in timer counts or microseconds. 
 *   
 *  The timer interrupt always uses the Hwi dispatcher. The Timer tickFxn 
 *  runs in the context of a Hwi thread. The Timer module automatically 
 *  creates a Hwi instance for the timer interrupt.
 *  
 *  The actual implementations of the Timer module functions are
 *  provided by device/family-specific Timer modules. 
 *  Additional, family-specific Timer module APIs may also be provided by
 *  your Timer module implementation.
 *  Follow the link below to determine which Timer implementation is used
 *  for your target:
 *
 *      {@link ./../family/doc-files/delegates.html Delegate Mappings}
 *
 *  @a(constraints)
 *  Since the hal Timer module has no knowledge of the delegate Timer
 *  module's instance definition, Timer_construct() can NOT be properly
 *  supported.
 *
 *  If {@link ti.sysbios.BIOS#runtimeCreatesEnabled BIOS.runtimeCreatesEnabled}
 *  is set to true, both Timer_create() and Timer_construct() 
 *  will attempt to dynamically create (ie NOT construct) a delegate Timer
 *  object.
 *
 *  If {@link ti.sysbios.BIOS#runtimeCreatesEnabled BIOS.runtimeCreatesEnabled}
 *  is set to false, both Timer_create() and Timer_construct() will fail.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                  -->
 *    <tr><td> {@link #getNumTimers}            </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getStatus}               </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}             </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #construct}               </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}                  </td><td>   N    </td>
 *    <td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}                  </td><td>   N    </td>
 *    <td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}                </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getFreq}                 </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getPeriod}               </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setPeriod}               </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setPeriodMicroSecs}      </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #start}                   </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #stop}                    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started 
 *    (e.g. Timer_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started 
 *    (e.g. Timer_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@ModuleStartup

module Timer inherits ti.sysbios.interfaces.ITimer 
{
    /*!
     *  ======== viewGetHandle ========
     *  @_nodoc
     *  Returns the corresponding hal Timer handle for a delegate Timer handle
     *
     *  @b(returns)     hal Timer handle
     */
    metaonly Handle viewGetHandle(Ptr pi);

instance:

    /*! 
     *  ======== create ========
     *  Create a timer.
     *
     *  Create could fail if timer peripheral is unavailable. To
     *  request any available timer use {@link #ANY} as the id.
     *  TimerId's are logical ids. The family-specific implementations 
     *  map the ids to physical peripherals.
     *
     *  @param(id)      Timer id ranging from 0 to a platform specific value
     *  @param(tickFxn) function that runs upon timer expiry.
     */
    override create(Int id, FuncPtr tickFxn);

internal:   /* not for client use */

    /*! target/device-specific Timer implementation. */
    proxy TimerProxy inherits ti.sysbios.interfaces.ITimer;

    struct Instance_State {
        TimerProxy.Handle pi;
    };
}
