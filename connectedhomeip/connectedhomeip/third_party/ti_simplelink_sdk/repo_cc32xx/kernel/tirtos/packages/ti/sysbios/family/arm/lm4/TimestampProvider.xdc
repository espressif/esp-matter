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
 *  ======== TimestampProvider.xdc ========
 *
 *
 */

package ti.sysbios.family.arm.lm4;

/*!
 *  ======== TimestampProvider ========
 *  Stellaris LM4 TimestampProvider
 *
 *  Stellaris LM4 TimestampProvider delegate for use
 *  with {@link xdc.runtime.Timestamp}.
 *
 *  Implements {@link ti.sysbios.interfaces.ITimestampProvider}
 *
 *  A timer managed by the {@link Timer Stellaris LM4 Timer} module
 *  is used as the timestamp source.
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center"></colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th><th>  Main**  </th><th>  Startup***  </th></tr>
 *    <!--                                                                                                                 -->
 *    <tr><td> {@link #get32}           </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #get64}           </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getFreq}         </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started (e.g. TimestampProvider_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started (e.g. TimestampProvider_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *  </table>
 *  @p
 */

@ModuleStartup

module TimestampProvider inherits ti.sysbios.interfaces.ITimestamp
{
    /*!
     *  Set this parameter when you want TimestampProvider to use
     *  the same timer as the Clock module.
     *
     *  This parameter overrides the {@link #timerId} setting.
     */
    config Bool useClockTimer = false;

    /*!
     *  TimestampProvider timer id.
     *
     *  When not using the Clock timer, TimestampProvider will
     *  create an {@link Timer} using this timer id.
     */
    config UInt timerId = Timer.ANY;

    /*!@_nodoc
     *  ======== startTimer ========
     *  Internal use. Function that starts timer before main
     */
    Void startTimer();

    /*!@_nodoc
     *  ======== rolloverFunc ========
     *  Internal use. Timer rollover interrupt handler.
     */
    Void rolloverFunc(UArg unused);

internal:   /* not for client use */

    /*!@_nodoc
     *  ======== initTimerHandle ========
     *  firstFxn used to ensure that Module->timer is initialized
     *  before Timestamp_get32() is invoked
     */
    Void initTimerHandle();

    struct Module_State {
        Timer.Handle    timer;          /* timer instance used */
        volatile UInt32 hi;             /* upper 32 bits */
    }
}
