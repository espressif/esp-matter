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

package ti.sysbios.family.arm.m3;

import xdc.rov.ViewInfo;

import xdc.runtime.Types;
import xdc.runtime.Error;

import ti.sysbios.interfaces.ITimer;


/*!
 *  ======== Timer ========
 *  Cortex M3 Timer Peripherals Manager.
 *
 *  The Cortex M3 Timer Manager utilizes the M3's internal NVIC SysTick Timer.
 *  The SysTick timer is clocked at the CPU clock rate, nominally 200MHz.
 *  The SysTick timer has only 24 bits of period resolution and is therefore
 *  limited to a maximum period of 83ms.
 *
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
    /*! Max value of Timer period for PeriodType_COUNTS */
    const UInt MAX_PERIOD = 0x00ffffff;

    /*! Number of timer peripherals on chip */
    const Int NUM_TIMER_DEVICES = 1;

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
        String      availMask;
        UInt        tickCount;
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
    config UInt anyMask = 0x1;

    /*!
     *  ======== getHandle ========
     *  @_nodoc
     *  Used by TimestampProvider module to get hold of timer handle.
     *
     *  @param(id)      timer Id.
     */
    Handle getHandle(UInt id);

    /*!
     *  ======== getTickCount ========
     *  @_nodoc
     *  Used by TimestampProvider.
     */
    UInt32 getTickCount();

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


internal:   /* not for client use */

    metaonly config Bool timerInUse = false;

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

    /*!
     *  ======== oneShotStub ========
     *  SysTick timer does not support one shot mode. This stub stops timer
     *  and clears the pending timer interrupt.
     *
     *  @param(arg)     Timer object.
     */
    Void oneShotStub(UArg arg);

    /*!
     *  ======== periodicStub ========
     *  SysTick timer interrupt must be acknowledged. This stub acknowledges timer
     *
     *  @param(arg)     Timer object.
     */
    Void periodicStub(UArg arg);

    /*!
     *  ======== noStartupNeeded ========
     *  Flag used to prevent misc code from being brought in
     *  un-necessarily
     */
    config UInt startupNeeded = false;

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
        volatile UInt   tickCount;      /* SysTick Rollover counter */
        UInt            availMask;      /* available peripherals */
        Handle          handle;         /* NVIC timer handle */
    }
}

