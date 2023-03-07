/*
 * Copyright (c) 2014, Texas Instruments Incorporated
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
 *  ======== Alarm.xdc ========
 *
 */
package ti.sysbios.family.arm.cc26xx;

import xdc.rov.ViewInfo;
import ti.sysbios.knl.Clock;

/*!
 *  ======== Alarm ========
 *  CC26xx Alarm Manager
 *
 *  This module manages Alarms objects.  Alarm objects internally create
 *  and schedule Clock objects, to expire at specified 64-bit RTC count values.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *  </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *  <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                          -->
 *    <tr><td> {@link #getCount}            </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}             </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #construct}               </td><td>   N    </td>
 *  <td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}                  </td><td>   N    </td>
 *  <td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}                  </td><td>   N    </td>
 *  <td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}                </td><td>   N    </td>
 *  <td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #set}                </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setFunc}                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #stop}                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during xdc.runtime.Startup functions.</li>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
@InstanceFinalize

module Alarm
{
    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String label;
        UInt32 timeout;      /* alarm Clock tick */
        UInt64 rtcCount;     /* alarm RTC count */
        UInt64 rtcInterrupt; /* RTC count of scheduled Clock interrupt */
        Bool active;         /* instance is activated? */
    }

    /*!
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
            [
                'Basic',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitBasic',
                    structName: 'BasicView'
                }
            ],
            ]
        });

    /*!
     *  ======== FuncPtr ========
     * Instance function prototype
     */
    typedef Void (*FuncPtr)(UArg);

    /*!
     *  ======== getCount ========
     *  Get the current RTC count
     */
    UInt64 getCount();

instance:

    /*!
     *  ======== create ========
     *  Create an Alarm object
     *
     *  @param(alarmFxn)        alarm function, of type
     *                          {@link #FuncPtr Alarm_FuncPtr}
     *  @param(arg)             argument to the alarm function
     */
    create(FuncPtr alarmFxn, UArg arg);

    /*!
     *  ======== set ========
     *  Set and start an alarm, to expire upon a specified RTC count value
     *
     *  The RTC count will be converted to the closest
     *  {@link ti.sysbios.knl.Clock Clock} tick that expires before the
     *  specified RTC count.
     *
     *  When the alarm expires, the alarm's function will be called, with the
     *  specified argument, from {@link ti.sysbios.knl.Swi Swi} context.  The
     *  alarm function could complete its processing and return, or it might
     *  defer processing by signaling another thread.
     *
     *  The alarm function can be unique to the application, or it might be
     *  a common kernel function.  For example, to create an alarm that wakes
     *  a task that is pended upon a semaphore, the alarm function could be
     *  Semaphore_post(), and the argument the handle of a semaphore that
     *  the task is pended upon:
     *
     *  @p(code)
     *  Void yourFunc() {
     *      ...
     *      sem = Semaphore_create(0, NULL, NULL);
     *      ...
     *      alm = Alarm_create((Alarm_FuncPtr)Semaphore_post, (UArg)sem, NULL,
     *          NULL);
     *      ...
     *      Alarm_set(alm, 0x200000000);
     *      ...
     *  }
     *  @p
     *
     *  @param(countRTC)        64-bit target RTC count
     *
     *  @b(returns)     TRUE if alarm is set, FALSE if countRTC is in the past,
     *                  or is too soon to trigger the corresponding Clock
     *                  interrupt
     */
    Bool set(UInt64 countRTC);

   /*!
     *  ======== setFunc ========
     *  Change an alarm's function and argument
     *
     *  Replaces an alarm's function and argument, with a new function and
     *  argument.
     *
     *  @param(fxn)             alarm function, of type
     *                          {@link #FuncPtr Alarm_FuncPtr}
     *  @param(arg)             argument to the Alarm function
     *
     *  @a(constraints)
     *  Cannot change the function and argument of Alarm that has already been
     *  started with {@link #set Alarm_set} or {@link #setTick Alarm_setTick}.
     */
    Void setFunc(FuncPtr fxn, UArg arg);

    /*!
     *  ======== stop ========
     *  Stop an alarm that has been started with {@link #set Alarm_set} or
     *  {@link #setTick Alarm_setTick}
     */
    Void stop();

internal:

    /* -------- Internal Structures -------- */
    struct Instance_State {
        Clock.Object  clockObj;
        UInt64        rtcCount;
    };
}
