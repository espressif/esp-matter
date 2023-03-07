/*
 * Copyright (c) 2013-2020, Texas Instruments Incorporated
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
 *  ======== Clock.xdc ========
 *
 *
 */

package ti.sysbios.knl;

import xdc.rov.ViewInfo;

import xdc.runtime.Assert;
import xdc.runtime.Diags;
import xdc.runtime.Log;

/*!
 *  ======== Clock ========
 *  System Clock Manager
 *
 *  The System Clock Manager is responsible for all timing services in
 *  SYS/BIOS.
 *  It generates the periodic system tick. The tick period is configurable.
 *  The timeout and period for all Clock Instances and timeout values in
 *  other SYS/BIOS modules are specified in terms of Clock ticks.
 *
 *  The Clock Manager supports two tick "modes": a periodic mode with an
 *  interrupt on each tick (TickMode_PERIODIC), and a tick suppression
 *  mode (TickMode_DYNAMIC), which reduces the number of timer interrupts to
 *  the minimum required to support the scheduled timeouts.  For devices that
 *  support it (e.g., CC13xx/CC26xx devices), TickMode_DYNAMIC may be the default
 *  mode if one is not specified in the application configuration; otherwise,
 *  the default mode will be TickMode_PERIODIC.  The following example shows
 *  how the tick mode  can be specified in the application configuration:
 *
 *  @p(code)
 *  var Clock = xdc.useModule('ti.sysbios.knl.Clock');
 *
 *  // Tell the Clock module to use TickMode_PERIODIC
 *  Clock.tickMode = Clock.TickMode_PERIODIC;
 *  @p
 *
 *  Clock objects contain functions that can be scheduled to run after a
 *  certain number of Clock ticks.
 *  Clock objects are either one-shot or periodic. Instances are started
 *  when created or they are started later using the Clock_start() function.
 *  Instances can be stopped using the Clock_stop() function. All Clock
 *  Instances are executed when they expire in the context of a software
 *  interrupt.
 *
 *  Clock objects are placed in the Clock object service list when
 *  created/constructed and remain there until deleted/destructed.
 *  To minimize processing overhead, unused or expired Clock objects
 *  should be deleted or destructed.
 *
 *  By default, all Clock functions run in the context of a Swi.
 *  That is, the Clock module automatically creates a Swi for
 *  its use and runs the Clock functions within that Swi.
 *  The priority of the Swi used by Clock can be changed
 *  by configuring {@link #swiPriority Clock.swiPriority}.
 *
 *  If Swis are disabled in an application
 *  (ie {@link ti.sysbios.BIOS#swiEnabled BIOS.swiEnabled} = false),
 *  then all Clock functions are executed within the context of
 *  a Timer Hwi.
 *
 *  @a(Note)
 *  @p(blist)
 *  As Clock functions execute in either a Swi or Hwi context, they
 *  are not permitted to call blocking APIs.
 *  @p
 *  @a
 *
 *  The getTicks() function returns number of clock ticks since startup.
 *
 *  By default, the Timer module defined by {@link #TimerProxy} is used to
 *  statically create a timer instance that provides a periodic 1 ms
 *  tick interrupt.
 *
 *  If you want to use a custom configured timer for the Clock module's
 *  tick source, use the following example configuration as a guide:
 *
 *  @p(code)
 *  var Clock = xdc.useModule('ti.sysbios.knl.Clock');
 *
 *  // Tell the Clock module that YOU are providing the periodic interrupt
 *  Clock.tickSource = Clock.TickSource_USER;
 *
 *  // this example uses the ti.sysbios.timers.dmtimer.Timer module
 *  var Timer = xdc.useModule('ti.sysbios.timers.dmtimer.Timer');
 *
 *  // Change Timer 3 frequency to 24 Mhz from default if necessary
 *  Timer.intFreqs[3] = { hi:0, lo:24000000 };
 *
 *  // create a dmtimer config parameter object
 *  var timerParams = new Timer.Params();
 *
 *  // make sure you set the period to 1000 us (1ms)
 *  timerParams.period = 1000;
 *
 *  // custom dmtimer config parameters here...
 *  timerParams.twer.ovf_wup_ena = 1;
 *
 *  // Create the timer.
 *  // This example uses timer id 3.
 *  // Provide your own timer interrupt handler function.
 *  Timer.create(3, '&myTimerTick', timerParams);
 *  @p
 *
 *  In your 'C' code, add your timer interrupt handler and have it
 *  call Clock_tick(), which will perform all of the Clock module
 *  tick duties:
 *
 *  @p(code)
 *  #include <ti/sysbios/knl/Clock.h>
 *
 *  Void myTimerTick(UArg arg)
 *  {
 *       Clock_tick();
 *       ...
 *  }
 *  @p
 *
 *  @p(html)
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                -->
 *    <tr><td> {@link #construct}      </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #create}         </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #delete}         </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #destruct}       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getTicks}       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #getTimerHandle} </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #Params_init}    </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> {@link #tick}           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #tickReconfig}   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #tickStart}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> {@link #tickStop}       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *
 *    <tr><td> {@link #getTimeout}     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #isActive}       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setFunc}        </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setPeriod}      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #setTimeout}     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #start}          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> {@link #stop}           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. Clock_Module_startupDone() returns TRUE). </li>
 *             <li> During
 *    {@link xdc.runtime.Startup#lastFxns Startup.lastFxns}. </li>
 *             <li> During main().</li>
 *             <li> During
 *    {@link ti.sysbios.BIOS#startupFxns BIOS.startupFxns}.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During
 *    {@link xdc.runtime.Startup#firstFxns Startup.firstFxns}.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. Clock_Module_startupDone() returns FALSE).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 *  @p
 */

@DirectCall
@ModuleStartup
@InstanceInitStatic /* Construct/Destruct CAN becalled at runtime */
@InstanceFinalize   /* generate call to Clock_Instance_finalize on delete */
@Template("./Clock.xdt")

module Clock
{
    /*!
     *  ======== TickSource ========
     *  Clock tick source
     *
     *  @field(TickSource_TIMER) The Clock module automatically configures a
     *  a Timer instance (see {@link #TimerProxy}) to drive the Clock tick.
     *  The specific timer and its period can be controlled via
     *  {@link #timerId} and {@link #tickPeriod}.
     *
     *  @field(TickSource_USER) The Application is responsible for calling
     *  {@link #tick Clock_tick()} periodically. Make sure {@link #tickPeriod
     *  Clock.tickPeriod} is set to the period that Clock_tick() is called.
     *
     *  Like most other module configuration parameters, the Clock.tickPeriod
     *  config parameter value is accessible in runtime C code as
     *  "Clock_tickPeriod".
     *
     *  @field(TickSource_NULL) The Clock module is disabled.
     *  In this case, it is an error for the application to ever call
     *  Clock_tick().
     *
     *  @see #tickPeriod
     *  @see #timerId
     */
    enum  TickSource {
        TickSource_TIMER,   /*! Internally configure a Timer to periodically call Clock_tick() */
        TickSource_USER,    /*! Application code calls Clock_tick() */
        TickSource_NULL     /*! The Clock module is disabled */
    };

    /*!
     *  ======== TickMode ========
     *  Clock Tick Mode
     */
    enum  TickMode {
        TickMode_PERIODIC,  /*! Timer will interrupt every period */
        TickMode_DYNAMIC    /*! Unnecessary timer ticks will be suppressed */
    };

    /*!
     *  ======== BasicView ========
     *  @_nodoc
     */
    metaonly struct BasicView {
        String          label;
        UInt32          timeout;
        UInt            period;
        String          fxn[];
        UArg            arg;
        Bool            started;        /* Instance running? */
        String          tRemaining;     /* Remaining timeout */
        Bool            periodic;       /* Periodic? (vs. one-shot) */
    }

    /*!
     *  ======== ModuleView ========
     *  @_nodoc
     */
    metaonly struct ModuleView {
        String          ticks;
        String          tickSource;
        String          tickMode;
        String          timerHandle;
        UInt            timerId;
        UInt            swiPriority;
        UInt32          tickPeriod;
        volatile UInt   nSkip;
    }

    /*
     *  ======== rovViewInfo ========
     *  @_nodoc
     */
    @Facet
    metaonly config ViewInfo.Instance rovViewInfo =
        ViewInfo.create({
            viewMap: [
              ['Basic',    {type: ViewInfo.INSTANCE, viewInitFxn: 'viewInitBasic',  structName: 'BasicView'}],
              ['Module',   {type: ViewInfo.MODULE,   viewInitFxn: 'viewInitModule', structName: 'ModuleView'}],
            ]
        });

    /*!
     *  ======== FuncPtr ========
     * Instance function prototype
     */
    typedef Void (*FuncPtr)(UArg);

    /*!
     *  ======== TimerProxy ========
     *  target/device-specific Timer implementation.
     *
     *  The Timer module used by the Clock module to
     *  create a Timer instance when Clock.tickSource_TIMER is configured.
     *
     *  By default, a target specific Timer module is internally selected
     *  for this purpose. If the user wishes to use a different Timer module
     *  then the following configuration script will serve as an example for
     *  achieving that:
     *
     *  @p(code)
     *  var Clock = xdc.useModule('ti.sysbios.knl.Clock');
     *
     *  // Use a dmtimer Timer instance
     *  Clock.TimerProxy = xdc.useModule('ti.sysbios.timers.dmtimer.Timer');
     *  @p
     *
     */
    proxy TimerProxy inherits ti.sysbios.interfaces.ITimer;

    /*!
     *  ======== LW_delayed ========
     *  Logged if Clock Swi delayed by >= 1 tick
     */
    config Log.Event LW_delayed = {
        mask: Diags.USER3,
        msg: "LW_delayed: delay: %d"
    };

    /*!
     *  ======== LM_tick ========
     *  Logged in every Clock tick interrupt
     */
    config Log.Event LM_tick = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_tick: tick: %d"
    };

    /*!
     *  ======== LM_begin ========
     *  Logged just prior to calling each Clock function
     */
    config Log.Event LM_begin = {
        mask: Diags.USER1 | Diags.USER2,
        msg: "LM_begin: clk: 0x%x, func: 0x%x"
    };

    /*!
     *  ======== A_clockDisabled ========
     *  Asserted in Clock_create()
     */
    config Assert.Id A_clockDisabled = {
        msg: "A_clockDisabled: Cannot create a clock instance when BIOS.clockEnabled is false."
    };

    /*!
     *  ======== A_badThreadType ========
     *  Asserted in Clock_create and Clock_delete
     */
    config Assert.Id A_badThreadType = {
        msg: "A_badThreadType: Cannot create/delete a Clock from Hwi or Swi thread."
    };

    /*!
     *  @_nodoc
     *  !!! Do not delete. Required for ROM compatibility !!!
     */
    config UInt32 serviceMargin = 0;

    /*!
     *  ======== tickSource ========
     *  Source of clock ticks
     *
     *  If this parameter is not set to TickSource_TIMER,
     *  {@link #tickStart Clock_tickStart()},
     *  {@link #tickStop Clock_tickStop()}, and
     *  {@link #tickReconfig Clock_tickReconfig()}, have no effect.
     *
     *  The default is TickSource_TIMER.
     */
    config TickSource tickSource = TickSource_TIMER;

    /*!
     *  ======== tickMode ========
     *  Timer tick mode
     *
     *  This parameter specifies the tick mode to be used by the underlying
     *  Timer.
     *
     *  With TickMode_PERIODIC the timer will interrupt the CPU at
     *  a fixed rate, defined by the tickPeriod.
     *
     *  With TickMode_DYNAMIC the timer can be dynamically reprogrammed by
     *  Clock, to interrupt the CPU when the next tick is actually needed for
     *  a scheduled timeout. TickMode_DYNAMIC is not supported on all devices,
     *  and may have some application constraints.
     */
    config TickMode tickMode;

    /*!
     *  ======== timerId ========
     *  Timer Id used to create a Timer instance
     *
     *  If {@link #tickSource Clock.tickSource} is set to TickSource_TIMER,
     *  the Clock module internally creates a
     *  static Timer instance (see {@link #TimerProxy}) that automatically calls
     *  Clock_doTick() on a periodic basis (as specified by
     *  {@link #tickPeriod tickPeriod} and {@link #periodType periodType}.)
     *
     *  This configuration parameter allows you to control which timer is
     *  used to drive the Clock module.
     *
     *  The default value is {@link ti.sysbios.hal.Timer#ANY Timer.ANY} (~0)
     *  and the maximum timerId possible is family and device specific.
     */
    config UInt timerId = ~0;

    /*!
     *  ======== swiPriority ========
     *  The priority of Swi used by Clock to process its instances
     *
     *  All Clock instances are executed in the context of a single
     *  {@link Swi}.  This parameter allows you to control the priority of
     *  that Swi.
     *
     *  The default value of this parameter is Swi.numPriorities - 1; i.e.,
     *  the maximum Swi priority.
     *
     *  @see ti.sysbios.knl.Swi#numPriorities
     */
    metaonly config UInt swiPriority;

    /*!
     *  ======== tickPeriod ========
     *  Tick period specified in microseconds
     *
     *  Default value is family dependent. For example, Linux systems often
     *  only support a minimum period of 10000 us and multiples of 10000 us.
     *  TI platforms have a default of 1000 us.
     *
     *  Like most other module configuration parameters, the Clock.tickPeriod
     *  config parameter value is accessible in runtime C code as
     *  "Clock_tickPeriod".
     */
    config UInt32 tickPeriod;

    /*!
     *  @_nodoc
     *  ======== stopCheckNext ========
     *  Boolean to control whether a check is made upon each Clock_stop() call
     *  to determine if the Clock object being stopped is due to timeout on the
     *  next scheduled tick.  If this feature is enabled, and the timeout
     *  coincides with the next scheduled tick, then a special 'trigger' Clock
     *  will be started to force a reschedule of the next tick, as soon as
     *  possible. This feature is only applicable for Clock.TickMode_DYNAMIC.
     *
     *  For most use cases it is most efficient to simply stop
     *  a Clock object, and then let the next scheduled tick occur naturally.
     *  But some low power application use cases (that routinely stop the next
     *  expiring Clock object) can benefit by scheduling an immediate tick, to
     *  suppress the next scheduled tick.  The default value for most
     *  targets is 'false', for cc26xx/cc13xx it is 'true'.  The default is
     *  established in Clock.xs, if the application has not explicitly
     *  specified a value.
     */
    metaonly config Bool stopCheckNext;

    /*!
     *  ======== getTicks ========
     *  Time in Clock ticks
     *
     *  The value returned will wrap back to zero after it reaches the max
     *  value that can be stored in 32 bits.
     *
     *  @b(returns)     time in clock ticks
     */
    UInt32 getTicks();

    /*!
     *  @_nodoc
     *  ======== getTimerHandle ========
     *  Get timer Handle
     *
     *  Used when it is necessary to change family
     *  specific options for the timer and its Hwi Object.
     *
     *  @b(returns)     Timer Handle
     */
    TimerProxy.Handle getTimerHandle();

    /*!
     *  @_nodoc
     *  ======== setTicks ========
     *  Set the internal Clock tick counter
     *
     *  Used internally by Power modules. Only applicable for
     *  Clock.TickMode_PERIODIC
     */
    Void setTicks(UInt32 ticks);

    /*!
     *  ======== tickStop ========
     *  Stop clock for reconfiguration
     *
     *  This function is used to stop the timer used for generation of
     *  clock ticks. It is used along with Clock_tickStart() and
     *  Clock_tickReconfig() to allow reconfiguration of timer at runtime.
     *
     *  Stopping the timer may not be supported for some types of timers, and
     *  is not supported for Clock.TickMode_DYNAMIC; in these cases, this
     *  this function call will have no effect.
     *
     *  @a(constraints)
     *  This function is non-reentrant and appropriate locks must be used to
     *  protect against  re-entrancy.
     */
    Void tickStop();

    /*!
     *  ======== tickReconfig ========
     *  Reconfigure clock for new cpu frequency
     *
     *  This function uses the new cpu frequency to reconfigure the timer used
     *  for generation of clock ticks such that tick period is
     *  accurate.  This function is used along with Clock_tickStop() and
     *  Clock_tickStart() to allow reconfiguration of timer at runtime.
     *
     *  Reconfiguration may not be supported for some types of timers, and is
     *  not supported for Clock.TickMode_DYNAMIC; in these cases, this
     *  this function call will have no effect, and will return false.
     *
     *  When calling Clock_tickReconfig outside of main(), you must also call
     *  Clock_tickStop and Clock_tickStart to stop and restart the timer.
     *  Use the following call sequence:
     *
     *  @p(code)
     *  // disable interrupts if an interrupt could lead to
     *  // another call to Clock_tickReconfig or if interrupt
     *  // processing relies on having a running timer
     *  Hwi_disable() or Swi_disable();
     *  BIOS_setCpuFreq(&freq);
     *  Clock_tickStop();
     *  Clock_tickReconfig();
     *  Clock_tickStart();
     *  Hwi_restore() or Swi_enable()
     *  @p
     *
     *  When calling Clock_tickReconfig from main(), the timer has not yet
     *  been started because the timer is started as part of BIOS_start().
     *  As a result, you can use the following simplified call sequence
     *  in main():
     *
     *  @p(code)
     *  BIOS_setCpuFrequency(Types.FreqHz *freq);
     *  Clock_tickReconfig(Void);
     *  @p
     *
     *  The return value is false if the timer cannot support the new
     *  frequency
     *
     *  @b(returns)     true if successful
     *
     *  @a(constraints)
     *  This function is non-reentrant and appropriate locks must be used to
     *  protect against  re-entrancy.
     */
    Bool tickReconfig();

    /*!
     *  ======== tickStart ========
     *  Start clock after reconfiguration
     *
     *  This function starts the timer used for generation of clock ticks
     *  It is used along with Clock_tickStop() and Clock_tickReconfig() to
     *  allow reconfiguration of timer at runtime. The new timer configuration
     *  reflects changes caused by a call to reconfig().
     *
     *  Reconfiguration and restart of a timer may not be supported for some
     *  types of timers, and is not supported for Clock.TickMode_DYNAMIC; in
     *  these cases, this function call will have no effect.
     *
     *  @a(constraints)
     *  This function is non-reentrant and appropriate locks must be used to
     *  protect against  re-entrancy.
     */
    Void tickStart();

    /*!
     *  ======== tick ========
     *  Advance Clock time by one tick
     *
     *  After incrementing a global tick counter, this function posts a Swi
     *  that processes the clock instances.
     *
     *  This function is automatically called by a timer ISR when
     *  {@link #tickSource} is set to {@link #TickSource_TIMER}.
     *
     *  When {@link #tickSource} is set to
     *  {@link #TickSource_USER}, Clock_tick() must be called by the
     *  application.  Usually, this is done within a user defined {@link ti.sysbios.hal.Hwi Hwi},
     *  {@link Swi}, or {@link Task}.
     *
     *  Note that this function is not re-entrant.  The application is
     *  responsible for ensuring that invocations of this function are
     *  serialized: either only one thread in the system ever calls this
     *  function or all calls are "wrapped" by an appropriate mutex.
     *
     *  @see #tickSource
     */
    Void tick();

    /*!
     *  @_nodoc
     *  ======== workFunc ========
     *  Clock Q service routine
     *
     *  @param(arg0)    Unused. required to match Swi.FuncPtr
     *  @param(arg1)    Unused. required to match Swi.FuncPtr
     */
    Void workFunc(UArg arg0, UArg arg1);

    /*!
     *  @_nodoc
     *  ======== workFuncDynamic ========
     *  Clock Q service routine for TickMode_DYNAMIC
     *
     *  @param(arg0)    Unused. required to match Swi.FuncPtr
     *  @param(arg1)    Unused. required to match Swi.FuncPtr
     */
    Void workFuncDynamic(UArg arg0, UArg arg1);

    /*!
     *  @_nodoc
     *  ======= logTick ========
     *  Log the LD_tick from within Clock module scope
     */
    Void logTick();

    /*!
     *  @_nodoc
     *  ======== getCompletedTicks ========
     *  Get the number of Clock ticks that have completed
     *
     *  Returns the number of ticks completed, to the point where
     *  the underlying Timer interrupt has been serviced.
     *
     *  Used by some TimestampProviders
     *
     *  @b(returns)     time in clock ticks
     */
    UInt32 getCompletedTicks();

    /*!
     *  @_nodoc
     *  ======== getTickPeriod ========
     *  Get the Clock tick period in timer counts
     *
     *  The period is in units returned by the underlying Timer.
     *
     *  Used by some TimestampProviders
     *
     *  @b(returns)     period in timer counts
     */
    UInt32 getTickPeriod();

    /*!
     *  @_nodoc
     *  ======== getTicksUntilInterrupt ========
     *  Get the number of Clock tick periods expected to expire between now
     *  and the next interrupt from the timer peripheral
     *
     *  Used internally by Power modules.
     *
     *  @b(returns)     count in ticks
     */
    UInt32 getTicksUntilInterrupt();

    /*!
     *  @_nodoc
     *  ======== getTicksUntilTimeout ========
     *  Get the number of Clock tick periods between now and the next
     *  active Clock object timeout.
     *
     *  Used internally by Power modules.
     *
     *  @a(constraints)
     *  Must be called with interrupts disabled.  Only applicable for
     *  Clock.TickSource_TIMER.
     *
     *  @b(returns)     count in ticks
     */
    UInt32 getTicksUntilTimeout();

    /*!
     *  @_nodoc
     *  ======= walkQueueDynamic ========
     *  Walk Clock's work queue for TickMode_DYNAMIC
     */
    UInt32 walkQueueDynamic(Bool service, UInt32 tick);

    /*!
     *  @_nodoc
     *  ======= walkQueuePeriodic ========
     *  Walk Clock's work queue for TickMode_PERIODIC
     */
    UInt32 walkQueuePeriodic();

    /*!
     *  @_nodoc
     *  ======= scheduleNextTick ========
     *  Reprogram Clock's Timer for earliest required tick
     */
    Void scheduleNextTick(UInt32 deltaTicks, UInt32 absTick);

instance:

    /*!
     *  ======== create ========
     *  Creates a Clock Instance
     *
     *  The first argument is the function that gets called when the timeout
     *  expires.
     *
     *  The 'timeout' argument is used to specify the startup timeout for
     *  both one-shot and periodic Clock instances (in Clock ticks).  This
     *  timeout is applied when the Clock instance is started.  For periodic
     *  instances, the configured Clock function will be called initially
     *  after an interval equal to the timeout, and will be subsequently
     *  called at the rate specified by the {@link #period} parameter.  For
     *  one-shot instances (where the {@link #period} parameter is 0), once
     *  the Clock instance is started (with {@link #start Clock_start()} or
     *  automatically if {@link #startFlag} is true) the configured Clock
     *  function will be called once after an interval equal to the timeout.
     *
     *  When instances are created they are placed upon a linked list managed
     *  by the Clock module.  For this reason, instances cannot be created
     *  from either Hwi or Swi context.
     *
     *  By default, all Clock functions run in the context of a Swi.
     *  That is, the Clock module automatically creates a Swi for
     *  its use and runs the Clock functions within that Swi.
     *  The priority of the Swi used by Clock can be changed
     *  by configuring {@link #swiPriority Clock.swiPriority}.
     *
     *  If Swis are disabled in an application
     *  (ie {@link ti.sysbios.BIOS#swiEnabled BIOS.swiEnabled} = false),
     *  then all Clock functions are executed within the context of
     *  a Timer Hwi.
     *
     *  @a(constraint)
     *  @p(blist)
     *  As Clock functions execute in either a Swi or Hwi context, they
     *  are not permitted to call blocking APIs.
     *  @p
     *  @a
     *
     *  @param(clockFxn)  Function that runs upon timeout
     *  @param(timeout)   One-shot timeout or initial start delay (in clock
     *                    ticks)
     */
    create(FuncPtr clockFxn, UInt timeout);

    /*!
     *  ======== startFlag ========
     *  Start immediately after instance is created
     *
     *  When this flag is set to false, the user will have to call
     *  Clock_start() to start the instance.
     *
     *  When set to true, both statically created Clock objects and Clock
     *  objects created in main() are started at the end of main() when the
     *  user calls BIOS_start(). Dynamically created Clock objects created
     *  after main() (ie within a task) will be started immediately.
     *
     *  The default setting for this parameter is false.
     *
     *  The configured Clock function will be called initially after an
     *  interval equal to the 'timeout' argument for both one-shot and
     *  periodic Clock objects.
     *
     *  Periodic Clock objects will subsequently be called at the rate
     *  specified by the {@link #period} parameter.
     *
     */
    config Bool startFlag = false;

    /*!
     *  ======== period ========
     *  Period of this instance (in clock ticks)
     *
     *  This parameter is used to set the subsequent timeout interval (in
     *  Clock ticks) for periodic instances.
     *
     *  The default value of this parameter is 0, which indicates this is
     *  a one-shot Clock object.
     *
     *  A non zero value for this parameter specifies that the Clock
     *  object is to be called periodically, and also specifies the
     *  rate (in Clock ticks) that the Clock function will be called
     *  AFTER the initial 'timeout' argument period.
     *
     *  For one-shot Clock instances, this parameter must be set to zero.
     */
    config UInt32 period = 0;

    /*!
     *  ======== arg ========
     *  Uninterpreted argument passed to instance function
     *
     *  The default is null.
     */
    config UArg arg = null;

    /*!
     *  @_nodoc
     *  ======== addI ========
     *  Lightweight One-Shot Clock create for internal SYS/BIOS timeout APIs
     *  Does NOT start the timeout (ie requires Clock_startI() to be called)
     *  Does NOT assume Hwis are disabled
     */
    Void addI(FuncPtr clockFxn, UInt32 timeout, UArg arg);

    /*!
     *  @_nodoc
     *  ======== removeI ========
     *  Lightweight Clock delete for internal SYS/BIOS timeout APIs
     *  Assumes Hwis are disabled
     */
    Void removeI();

    /*!
     *  ======== start ========
     *  Start instance
     *
     *  The {@link #timeout} and {@link #period} values set during create()
     *  or by calling Clock_setTimeout() and Clock_setPeriod() are used and
     *  the expiry is recomputed.
     *  Note that for periodic instances, the first expiry is
     *  computed using the timeout specified. All subsequent expiries use the
     *  period value.
     *
     *  @a(constraints)
     *  Timeout of instance cannot be zero
     */
    Void start();

    /*!
     *  @_nodoc
     *  ======== startI ========
     *  Internal start function which assumes Hwis disabled
     */
    Void startI();

    /*!
     *  ======== stop ========
     *  Stop instance
     */
    Void stop();

    /*!
     *  ======== setPeriod ========
     *  Set periodic interval
     *
     *  @param(period)          periodic interval in Clock ticks
     *
     *  @a(constraints)
     *  Cannot change period of instance that has been started.
     */
    Void setPeriod(UInt32 period);

    /*!
     *  ======== setTimeout ========
     *  Set the initial timeout
     *
     *  @param(timeout)         initial timeout in Clock ticks
     *
     *  @a(constraints)
     *  Cannot change the initial timeout of instance that has been started.
     */
    Void setTimeout(UInt32 timeout);

    /*!
     *  ======== setFunc ========
     *  Overwrite Clock function and arg
     *
     *  Replaces a Clock object's clockFxn function originally
     *  provided in {@link #create}.
     *
     *  @param(clockFxn)        function of type FuncPtr
     *  @param(arg)             argument to clockFxn
     *
     *  @a(constraints)
     *  Cannot change function and arg of Clock object that has been started.
     */
    Void setFunc(FuncPtr fxn, UArg arg);

    /*!
     *  ======== getPeriod ========
     *  Get period of instance
     *
     *  Returns the period of an instance.
     *
     *  @b(returns)             returns periodic interval in Clock ticks
     */
    UInt32 getPeriod();

    /*!
     *  ======== getTimeout ========
     *  Get timeout of instance
     *
     *  Returns the remaining time if the instance is active; if the instance
     *  is not active, returns zero.
     *
     *  There are certain edge cases to consider when calling this function
     *  for a periodic Clock instance.  Let the Clock instance's current
     *  timeout tick refer to the Clock system tick value when the Clock
     *  instance's function should be run. For a one-shot Clock, the
     *  current timeout tick is the number of Clock system ticks at the time
     *  the Clock instance was started, plus the Clock instance's timeout
     *  value. This is also the current timeout tick for a periodic Clock on
     *  its initial timeout.  A periodic Clock's current timeout tick is
     *  advanced by the period ticks, each time the periodic function is
     *  run.
     *  The remaining number of ticks for the Clock's timeout or period to
     *  expire is computed by subtracting the current Clock system tick
     *  value from the Clock instance's current timeout tick. Normally,
     *  this value should be less than or equal to the Clock instance's
     *  timeout (for a one-shot Clock or periodic clock on its initial
     *  timeout), or less than or equal to the Clock instance's period (for
     *  a periodic Clock that has already run its initial timeout).
     *  However, if the Clock module Swi is held off long enough for the system
     *  tick count to advance beyond the Clock instance's current timeout
     *  tick, this subtraction will result in a very large number (close to
     *  0xFFFFFFFF).  Clock_getTimeout() takes this into consideration.
     *  If the resulting value of the subtraction is greater than the Clock
     *  instance's period (always true for a one-shot clock) and greater than
     *  the Clock instance's timeout, it is assumed that the Clock system
     *  ticks has surpassed the Clock instance's current timeout tick, and
     *  0 is returned. This calculation of the remaining timeout ticks always
     *  works for one-shot clocks. The calculation may fail if the all of
     *  the following conditions are met:
     *  @p(blist)
     *      - The Clock instance is periodic
     *      - Clock_getTimeout() is called at a moment in time where
     *        the system Clock ticks has surpassed the Clock instance's
     *        current timeout (due to the Clock Swi being held off),
     *        resulting in a very large value for
     *  @p(code)
     *            remaining ticks = Clock instance's current timeout tick -
     *                              system Clock tick count
     *  @p
     *  @p(blist)
     *      - Either
     *  @p(code)
     *            remaining ticks > Clock instance's timeout, or
     *            remaining ticks > Clock instance's period
     *  @p
     *        This condition can only happen if the Clock instance's timeout
     *        or period is close to 0xFFFFFFFF.
     *  @p
     *
     *  An example of a failure case is the following:
     *  @p(blist)
     *      - A Clock instance has an initial timeout of 0xFFFFFFFF and
     *        period of 100.  Clock_getTimeout() is called after the initial
     *        timeout has expired and the Clock is now running periodically.
     *        In additiion, when Clock_getTimeout() is called, the system
     *        tick count has surpassed the Clock instance's current timeout
     *        by 2.  The remaining ticks will be 0xFFFFFFFE, which is still
     *        less than the timeout of 0xFFFFFFFF, so 0xFFFFFFFE is returned
     *        when actually 0 should be returned. Calling Clock_setTimeout()
     *        after the initial timeout expires, passing a small value for
     *        the new timeout (which will not be used since the Clock instance
     *        is now using the period value), will prevent this case from
     *        occurring.  An analogous failure can occur if the period is
     *        close to 0xFFFFFFFF and Clock_getTimeout() is called when the
     *        initial timeout has just expired and the Clock system tick
     *        count has surpassed the Clock's current timeout.
     *  @p
     *
     *  @b(returns)             returns timeout in clock ticks
     */
    UInt32 getTimeout();

    /*!
     *  ======== isActive ========
     *  Determine if Clock object is currently active (ie running)
     *
     *  Returns TRUE if Clock object is currently active
     *
     *  @b(returns)             returns active state
     */
    Bool isActive();

internal:

    /*
     * ======== timerSupportsDynamic ========
     * used in Clock.xml to enable/disable tickMode setting
     */
    metaonly config Bool timerSupportsDynamic = false;

    /*
     *  ======== doTickFunc =======
     *  access doTick through a func ptr so that
     *  ROM'd BIOS code doesn't reference a generated function.
     */
    config Void (*doTickFunc)(UArg);

    /*!
     *  ======== doTick ========
     *  Function called by the timer interrupt handler
     *
     *  @param(arg)     Unused. Required to match signature of Hwi.FuncPtr
     */
    Void doTick(UArg arg);

    /*
     *  ======== triggerClock ========
     *  Special Clock object created when Clock.stopCheckNext is 'true'.
     */
    config Clock.Handle triggerClock;

    /*
     *  ======== triggerFunc ========
     *  Empty function used by Clock.triggerClock
     */
    Void triggerFunc(UArg arg);

    /*
     *  ======== Instance_State ========
     */
    struct Instance_State {
        Queue.Elem      elem;           // required for clock queue
        UInt32          timeout;        // in clock ticks
        UInt32          currTimeout;    // working timeout
        UInt32          period;         // periodic instance if > 0
        volatile Bool   active;         // active/idle flag
        FuncPtr         fxn;            // instance function
        UArg            arg;            // function arg
    };

    /*
     *  ======== Module_State ========
     */
    struct Module_State {
        volatile UInt32     ticks;          // last tick serviced
        UInt                swiCount;       // num of Swi posts before Swi runs
        TimerProxy.Handle   timer;          // timer used
                                            // points to generated Clock_doTick()
        Queue.Object        clockQ;         // clock que
        Swi.Handle          swi;            // clock swi
        volatile UInt       numTickSkip;    // number of ticks being suppressed
        UInt32              nextScheduledTick;
        UInt32              maxSkippable;   // timer dependent (in tickPeriods)
        Bool                inWorkFunc;     // true if in Clock Swi servicing Q
        volatile Bool       startDuringWorkFunc; // Clock_start during workFunc?
        Bool                ticking;        // set true during first Clock tick
    };
}
