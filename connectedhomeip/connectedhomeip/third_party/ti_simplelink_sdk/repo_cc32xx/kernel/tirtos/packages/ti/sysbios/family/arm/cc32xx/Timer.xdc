/*
 * Copyright (c) 2014-2015, Texas Instruments Incorporated
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

package ti.sysbios.family.arm.cc32xx;

import xdc.rov.ViewInfo;

import xdc.runtime.Types;
import xdc.runtime.Error;

import ti.sysbios.interfaces.ITimer;
import ti.sysbios.family.arm.m3.Hwi;
import ti.sysbios.knl.Clock;

/*!
 *  ======== Timer ========
 *  CC3200 Timer Peripheral Manager
 *
 *  This module manages the 32Khz RTC timer available on CC3200 devices.
 *  The RTC timer uses 2 32-bit words, with the upper 16 bits of the MSW
 *  reserved, resulting in a 48-bit counter.
 *
 *  The Timer module supports the timer in 'one shot', 'continuous', and
 *  'dynamic' modes.
 *
 *  In 'one shot' mode, a timer function will "fire" (run) when the timer
 *  period expires. In 'one shot' mode this will only happen once.
 *
 *  In 'continuous' mode, the specified timer function will "fire" every
 *  time the period expires, throughout the lifetime of the program.
 *
 *  In 'dynamic' mode, the specified timer function will "fire" every
 *  time the period expires.  But the period of the timer can be changed
 *  dynamically, to correspond to the next tick interrupt needed from the
 *  timer.  This mode is used by the SYS/BIOS
 *  {@link ti.sysbios.knl.Clock Clock} module for implementing
 *  dynamic tick suppression, to reduce the number of interrupts from the
 *  timer to the minimum required for currently scheduled timeouts.
 *
 *  @a(Warning)
 *  Some of the RTC timer registers are accessed from the 32KHz clock
 *  domain, making reads/writes to these registers on the order of 100
 *  microseconds.
 *  If using the RTC timer for the {@link ti.sysbios.knl.Clock Clock}
 *  module's tick source, the interrupt latency will be as high as 200
 *  microseconds, as the {@link ti.sysbios.knl.Clock Clock} ISR makes two
 *  RTC register accesses on the 32KHz clock domain.
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
 *    <tr><td> {@link #getNumTimers}            </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getStatus}               </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #Params_init}             </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #construct}               </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}                  </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}                  </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}                </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getCount}                </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getFreq}                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getFunc}                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getPeriod}               </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setFunc}                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setPeriod}               </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setPeriodMicroSecs}      </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #start}                   </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #stop}                    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *  (e.g. Timer_Module_startupDone() returns TRUE). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *  (e.g. Timer_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */
@ModuleStartup          /* To configure static timers */
@InstanceInitStatic

module Timer inherits ti.sysbios.interfaces.ITimer
{
    /*! override supportsDynamic - this Timer does support RunMode_DYNAMIC */
    override metaonly config Bool supportsDynamic = true;

    /*! override defaultMode - use RunMode_PERIODIC by default */
    override metaonly config Bool defaultDynamic = true;

    /*! Max value of Timer period for PeriodType_COUNTS */
    const UInt64 MAX_PERIOD = 0xFFFFFFFFFFFF;


    /*! @_nodoc */
    @XmlDtd
    metaonly struct BasicView {
        Ptr         halTimerHandle;
        String      label;
        UInt        id;
        String      startMode;
        UInt64      period;
        String      tickFxn[];
        UArg        arg;
        String      hwiHandle;
    };

    /*! @_nodoc */
    metaonly struct DeviceView {
        UInt        id;
        String      device;
        String      devAddr;
	UInt	    intNum;
        UInt64      period64;
        UInt        currCount;
        UInt        remainingCount;
        UInt64      nextThreshold;
        String      state;
    };

    /*! @_nodoc */
    metaonly struct ModuleView {
        String      availMask;
    }

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

    /*!
     *  ======== E_invalidTimer ========
     *  Error raised when specified timer id is not supported
     */
    config Error.Id E_invalidTimer = {
        msg: "E_invalidTimer: Invalid Timer Id %d"
    };

    /*!
     *  ======== E_notAvailable ========
     *  Error raised when requested timer is in use
     */
    config Error.Id E_notAvailable = {
        msg: "E_notAvailable: Timer not available %d"
    };

    /*!
     *  ======== E_cannotSupport ========
     *  Error raised when requested period is not supported
     */
    config Error.Id E_cannotSupport = {
        msg: "E_cannotSupport: Timer cannot support requested period %d"
    };

    /*!
     *  ======== anyMask ========
     *  Mask of available timers
     *
     *  This mask is used to identify the timers that can be used when
     *  Timer_create() is called with an id equal to
     *  {@link Timer#ANY Timer_ANY}.
     */
    config UInt anyMask = 0x1;


    /*!
     *  Period of a tick
     *
     *  The RTC timer can take up to a 48-bit period.
     *  (The implementation of ITimer supports a period of UInt32
     *  timer counts).
     */
    config UInt64 period64 = 0;

    /*!
     *  ======== dynamicStub ========
     *  @_nodoc
     *
     *  @param(arg)     Unused.
     */
    Void dynamicStub(UArg arg);

    /*!
     *  ======== periodicStub ========
     *  @_nodoc
     *
     *  @param(arg)     Unused.
     */
    Void periodicStub(UArg arg);

   /*!
     *  ======== getCount64 ========
     *  Read the 64-bit timer counter register
     *
     *  @b(returns)     timer counter value
     */
    UInt64 getCount64(Object * timer);

   /*!
     *  ======== getMatchShadowReg ========
     *  Get the 64-bit value last written to the match register
     *
     *  @b(returns)     match register value
     */
    UInt64 getMatchShadowReg();

    /*!
     *  ======== getAvailMask ========
     *  Returns the availMask.
     *
     *  @b(returns)     Mask of available timers
     */
    UInt getAvailMask();

    /*!
     *  ======== setAvailMask ========
     *  Set the availMask to given mask.
     *
     *  This function validates the given mask to ensure it does not mark
     *  any currently used timer as available. If validation is successful,
     *  the mask overwrites the current availMask and the function returns
     *  TRUE. Otherwise, the mask is discarded and the function returns
     *  FALSE.
     *
     *  @param(mask)    Mask used to write to availMask
     */
    Bool setAvailMask(UInt mask);

    /*!
     *  ======== getHandle ========
     *  @_nodoc
     *  Used by TimestampProvider module to get hold of timer handle used by
     *  Clock.
     *
     *  @param(id)      timer Id.
     */
    Handle getHandle(UInt id);

    /*!
     *  ======== setNextMaxTick ========
     *  @_nodoc
     *  Instead of using maxSkippable, this timer sets the next
     *  scheduled tick to the last tick just before the next rollover
     *  of the lower 32-bits of the counter.
     */
    Void setNextMaxTick(UArg arg);

instance:

    /*! Hwi Params for Hwi Object. Default is null. */
    config Hwi.Params *hwiParams = null;

internal:   /* not for client use */

    /*!
     *  ======== noStartupNeeded ========
     *  Flag used to prevent misc code from being brought in
     *  un-necessarily
     */
    config UInt startupNeeded = false;

    /*
     *  ======== initDevice ========
     *  reset timer to its resting state
     */
    Void initDevice(Object *timer);

    /*
     *  ======== setThreshold ========
     *  set the compare threshold in RTC register
     */
    Void setThreshold(Object *timer, UInt64 next);

    struct Instance_State {
        Bool                    staticInst;
        Int                     id;
        ITimer.StartMode        startMode;
        UArg                    arg;
        Hwi.FuncPtr             tickFxn;
        Types.FreqHz            frequency;
        Hwi.Handle              hwi;
        UInt64                  period64;
        UInt64                  savedCurrCount;
    }

    struct Module_State {
        UInt            availMask;  /* available peripherals */
        Handle          handle;	    /* array of handles based on id */

        /*
         *  Clock object for setting the next interrupt to, when nothing else
         *  is on the clock queue.
         */
        Clock.Handle    clock;

        /*
         *  The value of the upper 32 bits of timer counter register
         *  at, or a few Clock ticks after the time the Clock object was
         *  last started.
         */
        UInt32          timeUpper;

        /*
         *  We need the period for the Clock function.  There is only one
         *  timer, and this period will be the same as the Timer object's
         *  period.
         */
        UInt64          period64;

        /*
         *  Shadow the match register.  This is part of the module
         *  state, so it can be accessed from an API without having to
         *  specify a Timer object.
         */
        UInt64          nextThreshold;
    }
}
