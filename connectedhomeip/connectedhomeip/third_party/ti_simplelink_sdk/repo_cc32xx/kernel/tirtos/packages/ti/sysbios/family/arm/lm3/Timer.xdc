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
 *  ======== Timer.xdc ========
 *
 */

package ti.sysbios.family.arm.lm3;

import xdc.rov.ViewInfo;

import xdc.runtime.Types;
import xdc.runtime.Error;

import ti.sysbios.interfaces.ITimer;
import ti.sysbios.family.arm.m3.Hwi;

/*!
 *  ======== Timer ========
 *  Stellaris LM3 Timer Peripheral Driver
 *
 *  Stellaris LM3 Timer Peripheral Driver for standalone use and
 *  as the {@link ti.sysbios.hal.Timer} delegate.
 *
 *  Implements {@link ti.sysbios.interfaces.ITimer}
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center"></colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                                                                 -->
 *    <tr><td> {@link #getNumTimers}            </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #getStatus}               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #Params_init}             </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #construct}               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #create}                  </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #delete}                  </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #destruct}                </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #getCount}                </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #getFreq}                 </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #getPeriod}               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #reconfig}                </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #setPeriod}               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #setPeriodMicroSecs}      </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #start}                   </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> {@link #stop}                    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started (e.g. Cache_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started (e.g. Cache_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */
@ModuleStartup          /* to configure static timers */
@InstanceInitStatic

module Timer inherits ti.sysbios.interfaces.ITimer
{
    /*! Max value of Timer period for PeriodType_COUNTS*/
    const UInt MAX_PERIOD = 0xFFFFFFFF;

    /*! Timer clock divider wrt cpu clock */
    const Int TIMER_CLOCK_DIVIDER = 1;

    /*! @_nodoc
     *  Min instructions to use in trigger().
     */
    const Int MIN_SWEEP_PERIOD = 1;

    /*! @_nodoc */
    @XmlDtd
    metaonly struct BasicView {
        Ptr         halTimerHandle;
        String      label;
        UInt        id;
        String      device;
        String      startMode;
        String      runMode;
        UInt        period;
        String      periodType;
        UInt        intNum;
        String      tickFxn[];
        UArg        arg;
        String      extFreq;
        String      hwiHandle;
    };


    /*! @_nodoc */
    metaonly struct ModuleView {
        String      availMask;      /* available 32-bit timer halves */
    }

    /*! @_nodoc */
    metaonly struct DeviceView {
        UInt        id;
        String      device;
        String      devAddr;
        UInt        intNum;
        String      runMode;
        UInt        period;
        UInt        currCount;
        UInt        remainingCount;
        String      state;
    };

    /*! @_nodoc */
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
            [
                'Device',
                {
                    type: ViewInfo.INSTANCE,
                    viewInitFxn: 'viewInitDevice',
                    structName: 'DeviceView'
                }
            ],
            [
                'Module',
                {
                    type: ViewInfo.MODULE,
                    viewInitFxn: 'viewInitModule',
                    structName: 'ModuleView'
                }
            ],
            ]
        });

    /*! Timer enable function type definition. */
    typedef Void (*TimerEnableFuncPtr)(Int);

    /*! Timer disable function type definition. */
    typedef Void (*TimerDisableFuncPtr)(Int);

    /*!
     *  Error raised when timer id specified is not supported.
     */
    config Error.Id E_invalidTimer  = {msg: "E_invalidTimer: Invalid Timer Id %d"};

    /*!
     *  Error raised when timer requested is in use
     */
    config Error.Id E_notAvailable  =
        {msg: "E_notAvailable: Timer not available %d"};

    /*!
     *  Error raised when period requested is not supported
     */
    config Error.Id E_cannotSupport  =
        {msg: "E_cannotSupport: Timer cannot support requested period %d"};

    /*!
     *  ======== anyMask ========
     *  Available mask to be used when select = Timer_ANY
     */
    config UInt anyMask = 0x3;

    /*!
     *  ======== enableFunc ========
     *  Pointer to Timer enable callback function
     *
     *  Timer enable callback function enables the timer clock and resets
     *  the timer.
     */
    config TimerEnableFuncPtr enableFunc = null;

    /*!
     *  ======== disableFunc ========
     *  Pointer to Timer disable callback function
     *
     *  Timer disable callback function disables the timer clock.
     */
    config TimerDisableFuncPtr disableFunc = null;

    /*!
     *  ======== getHandle ========
     *  Get Handle associated with a timer id.
     *
     *  @param(id)      timer Id.
     *  @b(returns)     timer Handle
     */
    Handle getHandle(UInt id);

    /*!
     *  ======== isrStub ========
     *  @_nodoc
     *  Timer interrupts must be acknowledged
     *
     *  @param(arg)     Timer object.
     */
    Void isrStub(UArg arg);

instance:

    /*! Hwi Params for Hwi Object. Default is null. */
    config Hwi.Params *hwiParams = null;

    /*!
     *  ======== reconfig ========
     *  Used to modify static timer instances at runtime.
     *
     *  @param(timerParams)     timer Params
     *  @param(tickFxn)         function that runs when timer expires
     */
    Void reconfig(FuncPtr tickFxn, const Params *timerParams, Error.Block *eb);

    /*!
     *  ======== getExpiredCounts64 ========
     *  Get current timer counter
     *
     *  @_nodoc
     *  Reads timer counter and adds period if IFR was set 
     *  before counter read. Used exclusively by TimestampProvider.
     *
     *  Must be called with interrupts disabled.
     *
     *  @b(returns)     expired counts.
     */
    UInt64 getExpiredCounts64();

internal:   /* not for client use */

    /*
     *  ======== noStartupNeeded ========
     *  Flag used to prevent misc code from being brought in
     *  un-necessarily
     */
    config UInt startupNeeded = false;

    /*! Information about timer */
    struct TimerDevice {
        UInt intNum;
        Ptr  baseAddr;
    };

    /*!
     *  ======== numTimerDevices ========
     *  The number of logical timers on a device.
     */
    config Int numTimerDevices;

    /*!
     *  ======== WRALLOW ========
     *  Write protection register for critical registers;
     */
    config Ptr WRALLOW = null;

    /*!
     *  ======== disableStellaris ========
     */
    Void disableStellaris(Int id);

    /*!
     *  ======== enableStellaris ========
     */
    Void enableStellaris(Int id);

    /*
     *  ======== initDevice ========
     *  reset timer to its resting state
     */
    Void initDevice(Object *timer);

    /*
     *  ======== postInit ========
     *  finish initializing static and dynamic Timers
     */
    Int postInit(Object *timer, Error.Block *eb);

    /*
     *  ======== enableTimers ========
     *  enable timer register access
     *  called as a Startup.firstFxnw
     */
    Void enableTimers();

    struct Instance_State {
        Bool            staticInst;
        Int             id;
        RunMode         runMode;
        StartMode       startMode;
        UInt            period;
        PeriodType      periodType;
        UInt            intNum;
        UArg            arg;
        Hwi.FuncPtr     tickFxn;
        Types.FreqHz    extFreq;        /* external frequency in Hz */
        Hwi.Handle      hwi;
    }

    struct Module_State {
        UInt            availMask;      /* available peripherals */
        TimerDevice     device[];       /* timer device information */
        Handle          handles[];      /* array of handles based on id */
    }
}

