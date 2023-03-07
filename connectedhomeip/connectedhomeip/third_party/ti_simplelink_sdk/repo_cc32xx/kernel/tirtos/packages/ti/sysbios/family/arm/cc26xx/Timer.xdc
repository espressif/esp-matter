/*
 * Copyright (c) 2014-2017, Texas Instruments Incorporated
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

package ti.sysbios.family.arm.cc26xx;

import xdc.rov.ViewInfo;

import xdc.runtime.Types;
import xdc.runtime.Error;

import ti.sysbios.interfaces.ITimer;
import ti.sysbios.family.arm.m3.Hwi;

/*!
 *  ======== Timer ========
 *  CC26xx Timer Peripheral Manager
 *
 *  This Timer module manages the RTC timer peripheral on
 *  CC26XX/CC13XX devices.  This Timer operates in a dynamic tick mode
 *  (RunMode_DYNAMIC). Rather than interrupting on every fixed tick period,
 *  the Timer is dynamically reprogrammed to interrupt on the next required
 *  tick (as determined by work that has been scheduled with a future timeout).
 *
 *  By default, this Timer module is used by the SYS/BIOS
 *  {@link ti.sysbios.knl.Clock Clock} module for implementing timing services
 *  on CC26XX/CC13XX devices.  Operating in dynamic mode allows the Clock module
 *  to implement dynamic tick suppression, to reduce the number of interrupts
 *  from the timer, to the minimum required for any scheduled work.
 *
 *  The RTC peripheral is implemented as a 64-bit counter, with the upper
 *  32-bits of count representing seconds, and the lower 32-bits representing
 *  sub-seconds.  Three timer "channels" are provided for generating time match
 *  interrupt events.  The match compare value for each channel is a 32-bit
 *  value, spanning the lower 16-bits of the RTC seconds count, and the upper
 *  16-bits of the subsecond count.  There is a single interrupt line from the
 *  RTC to generate a CPU interrupt, for a match event occurring on any
 *  of these three channels.
 *
 *  Channel 0 of the RTC is dedicated to use by the Clock module.  This Timer
 *  module implementation is therefore responsible for overall management of
 *  the RTC, including resetting and starting the RTC during application boot,
 *  and providing the single interrupt service routine (ISR) for the RTC.
 *
 *  Channels 1 and 2 of the RTC are not used by the Clock module.  These
 *  channels may be available for use by some applications, depending upon the
 *  mix of software components being used.  For this purpose, this Timer
 *  module supports sharing of the RTC interrupt, to support usage
 *  of these other channels (in parallel with the usage of Channel 0 by the
 *  Clock module).
 *
 *  To use one of these other channels the application will need to explicitly
 *  configure an interrupt "hook" function for the channel. In this case, when
 *  an RTC interrupt triggers the ISR will check the status of each channel to
 *  see if the corresponding channel hook function should be called.
 *
 *  The time match values for Channel 0 will be automatically programmed by
 *  the Clock module.  To use Channels 1 (and/or Channel 2), the application
 *  will need to explicitly program the match value for the corresponding
 *  channel, for the desired time for the interrupt.  Also, the application
 *  will need to explicitly enable the additional channel(s).  Note that if a
 *  hook function is configured for Channel 1 or Channel 2, the corresponding
 *  events will be configured automatically when Channel 0 is started.  In
 *  other words, there is no need for the application to explicitly configure
 *  events for Channel 1 or Channel 2 by calling AONRTCCombinedEventConfig().
 *
 *  The below snippets show an example of using Channel 1, with Driverlib API
 *  calls to configure an RTC event at 4 seconds after boot.
 *
 *  First, in the application .cfg file a hook function is defined for
 *  Channel 1:
 *
 *  @p(code)
 *    var Timer = xdc.module('ti.sysbios.family.arm.cc26xx.Timer');
 *    Timer.funcHookCH1 = "&myHookCH1";
 *  @p
 *
 *  In main(), Channel 1 is first cleared, a compare (match) value of 4 seconds
 *  is set, the channel is enabled:
 *
 *  @p(code)
 *    AONRTCEventClear(AON_RTC_CH1);
 *    AONRTCCompareValueSet(AON_RTC_CH1, 0x40000);
 *    AONRTCChannelEnable(AON_RTC_CH1);
 *  @p
 *
 *  With the above, myHookCH1() will be called when the RTC reaches a count of
 *  4 seconds.  At that time, a new compare value can be written for the next
 *  interrupt that should occur for Channel 1.
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

    // -------- Module Types --------

    /*! Max value of Timer period for PeriodType_COUNTS */
    const UInt MAX_PERIOD = 0xFFFFFFFF;

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
        String      startMode;
        String      tickFxn[];
        UArg        arg;
        String      hwiHandle;
    };

    /*! @_nodoc */
    metaonly struct DeviceView {
        UInt        id;
        String      device;
        String      devAddr;
        UInt        intNum;
        UInt32      currCount;
        UInt32      nextCompareCount;
        UInt32      remainingCount;
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
     *  ======== funcHookCH1 ========
     *  Optional hook function for processing RTC channel 1 events
     *
     *  This function will be called when there is a timeout event on
     *  RTC Channel 1.  It will be called from hardware interrupt context,
     *  so any API calls from this function must be appropriate for
     *  execution from an ISR.
     *
     *  Function hooks are only supported with RunMode_DYNAMIC.
     */
    config FuncPtr funcHookCH1 = null;

    /*!
     *  ======== funcHookCH2 ========
     *  Optional hook function for processing RTC channel 2 events.
     *
     *  This function will be called when there is a timeout event on
     *  RTC Channel 2.  It will be called from hardware interrupt context,
     *  so any API calls from this function must be appropriate for
     *  execution from an ISR.
     *
     *  Function hooks are only supported with RunMode_DYNAMIC.
     */
    config FuncPtr funcHookCH2 = null;

    /*!
     *  ======== dynamicStub ========
     *  @_nodoc
     *
     *  @param(arg)     Unused.
     */
    Void dynamicStub(UArg arg);

    /*!
     *  ======== dynamicMultiStub ========
     *  @_nodoc
     *
     *  @param(arg)     Unused.
     */
    Void dynamicMultiStub(UArg arg);

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
     *  ======== getExpiredCounts64 ========
     *  Returns expired counts (64-bits) since the last serviced interrupt.
     *
     *  @b(returns)     timer counter value
     */
    UInt64 getExpiredCounts64(Object * timer);

    /*!
     *  ======== getHandle ========
     *  @_nodoc
     *  Used by TimestampProvider module to get hold of timer handle used by
     *  Clock.
     *
     *  @param(id)      timer Id.
     */
    Handle getHandle(UInt id);

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
     *  ======== postInit ========
     *  finish initializing static Timers
     */
    Int postInit(Object *timer, Error.Block *eb);

    /*
     *  ======== setThreshold ========
     *  set the compare threshold in RTC register
     */
    Void setThreshold(Object *timer, UInt32 next, Bool wrap);

    struct Instance_State {
        Bool                    staticInst;
        Int                     id;
        ITimer.StartMode        startMode;
        UInt32                  period;
        UArg                    arg;
        Hwi.FuncPtr             tickFxn;
        Types.FreqHz            frequency;
        Hwi.Handle              hwi;
        UInt64                  period64;
        UInt64                  savedCurrCount;
        UInt64                  prevThreshold;
        UInt64                  nextThreshold;
    }

    struct Module_State {
        UInt            availMask;      /* available peripherals */
        Handle          handle;	    /* array of handles based on id */
    }
}
