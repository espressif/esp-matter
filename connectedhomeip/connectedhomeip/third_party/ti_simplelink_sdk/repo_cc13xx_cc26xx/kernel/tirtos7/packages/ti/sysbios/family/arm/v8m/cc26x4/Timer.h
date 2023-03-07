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
 * @file ti/sysbios/family/arm/v8m/cc26x4/Timer.h
 * @brief CC26xx Timer Peripheral Manager
 *
 * This Timer module manages the RTC timer peripheral on CC26XX/CC13XX devices.
 * This Timer operates in a dynamic tick mode (RunMode_DYNAMIC). Rather than
 * interrupting on every fixed tick period, the Timer is dynamically
 * reprogrammed to interrupt on the next required tick (as determined by work
 * that has been scheduled with a future timeout).
 *
 * By default, this Timer module is used by the SYS/BIOS @ref
 * Timer_ti.sysbios.knl.Clock "Clock" module for implementing timing services on
 * CC26XX/CC13XX devices.  Operating in dynamic mode allows the Clock module to
 * implement dynamic tick suppression, to reduce the number of interrupts from
 * the timer, to the minimum required for any scheduled work.
 *
 * The RTC peripheral is implemented as a 64-bit counter, with the upper 32-bits
 * of count representing seconds, and the lower 32-bits representing sub-
 * seconds.  Three timer "channels" are provided for generating time match
 * interrupt events.  The match compare value for each channel is a 32-bit
 * value, spanning the lower 16-bits of the RTC seconds count, and the upper
 * 16-bits of the subsecond count.  There is a single interrupt line from the
 * RTC to generate a CPU interrupt, for a match event occurring on any of these
 * three channels.
 *
 * Channel 0 of the RTC is dedicated to use by the Clock module.  This Timer
 * module implementation is therefore responsible for overall management of the
 * RTC, including resetting and starting the RTC during application boot, and
 * providing the single interrupt service routine (ISR) for the RTC.
 *
 * Channels 1 and 2 of the RTC are not used by the Clock module.  These channels
 * may be available for use by some applications, depending upon the mix of
 * software components being used.  For this purpose, this Timer module supports
 * sharing of the RTC interrupt, to support usage of these other channels (in
 * parallel with the usage of Channel 0 by the Clock module).
 *
 * To use one of these other channels the application will need to explicitly
 * configure an interrupt "hook" function for the channel. In this case, when an
 * RTC interrupt triggers the ISR will check the status of each channel to see
 * if the corresponding channel hook function should be called.
 *
 * The time match values for Channel 0 will be automatically programmed by the
 * Clock module.  To use Channels 1 (and/or Channel 2), the application will
 * need to explicitly program the match value for the corresponding channel, for
 * the desired time for the interrupt.  Also, the application will need to
 * explicitly enable the additional channel(s).  Note that if a hook function is
 * configured for Channel 1 or Channel 2, the corresponding events will be
 * configured automatically when Channel 0 is started.  In other words, there is
 * no need for the application to explicitly configure events for Channel 1 or
 * Channel 2 by calling AONRTCCombinedEventConfig().
 *
 * The below snippets show an example of using Channel 1, with Driverlib API
 * calls to configure an RTC event at 4 seconds after boot.
 *
 * First, in the application .cfg file a hook function is defined for Channel 1:
 *
 * @code
 *    var Timer = xdc.module('ti.sysbios.family.arm.v8m.cc26x4.Timer');
 *    Timer.funcHookCH1 = "&myHookCH1";
 * @endcode
 *
 * In main(), Channel 1 is first cleared, a compare (match) value of 4 seconds
 * is set, the channel is enabled:
 *
 * @code
 *    AONRTCEventClear(AON_RTC_CH1);
 *    AONRTCCompareValueSet(AON_RTC_CH1, 0x40000);
 *    AONRTCChannelEnable(AON_RTC_CH1);
 * @endcode
 *
 * With the above, myHookCH1() will be called when the RTC reaches a count of 4
 * seconds.  At that time, a new compare value can be written for the next
 * interrupt that should occur for Channel 1.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *  </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *  <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                          -->
 *    <tr><td> getNumTimers            </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getStatus               </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Params_init             </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> construct               </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> create                  </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> delete                  </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> destruct                </td><td>   N    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> getCount                </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> getFreq                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getFunc                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> getPeriod               </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setFunc                 </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setPeriod               </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> setPeriodMicroSecs      </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> start                   </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> stop                    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *  (e.g. Timer_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *  (e.g. Timer_Module_startupDone() returns false).</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/family/arm/v8m/Hwi.h>

#include <ti/sysbios/knl/Queue.h>

#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Types.h>

/*
 * First time Timer.h is included within a compilation unit:
 *  -undefine any previous Timer_ short names
 *  -do short-to-long name conversions.
 *  -include all short-named Timer API definitions.
 *
 * All subsequent includes:
 *  -undefine any previous Timer_ short names
 *  -perform short-to-long name converions
 *  -Do not include short-named Timer API definitins
 *   because the long name Timer API definitions are
 *   already known to the compiler.
 */

#define do_timer_undef_short_names
#define do_timer_short_to_long_name_conversion
#include "Timer_defs.h"

#ifndef ti_sysbios_family_arm_v8m_cc26x4_Timer__include
#define ti_sysbios_family_arm_v8m_cc26x4_Timer__include
#include <ti/sysbios/hal/ITimer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Timer_Module_State Timer_Module_State;
typedef struct Timer_Params Timer_Params;
typedef struct Timer_Struct Timer_Struct;
typedef struct Timer_Struct Timer_Object;
typedef Timer_Object* Timer_Handle;
typedef Timer_Object* Timer_Instance;

struct Timer_Params {
    /*!
     * @brief Timer run mode
     *
     * Default is @link Timer_RunMode_CONTINUOUS @endlink.
     */
    Timer_RunMode runMode;
    /*!
     * @brief Timer start mode
     *
     * Default is @link Timer_StartMode_AUTO @endlink.
     */
    Timer_StartMode startMode;
    /*!
     * @brief Argument for tick function
     *
     * Default is null.
     */
    uintptr_t arg;
    /*!
     * @brief Period of a tick
     *
     * The period can be specified in timer counts or microseconds and its
     * default value is 0.
     *
     * The implementation of Timer will support a period of uint32_t timer counts
     * and use pre-scalars if necessary.
     */
    uint32_t period;
    /*!
     * @brief Period type
     *
     * Default is PeriodType_MICROSECS
     */
    Timer_PeriodType periodType;
    /*!
     * @brief Timer frequency
     *
     * This parameter is meaningfull only on platforms where the timer's input
     * clock can be changed. If value is left at zero, then input clock to the
     * timer clock is assumed.
     *
     * This value is used to convert timer ticks to real time units; seconds,
     * milliseconds, etc.
     */
    Types_FreqHz extFreq;
    /*!
     * @brief Hwi Params for Hwi Object. Default is null.
     */
    Hwi_Params *hwiParams;
};

struct Timer_Module_State {
    Queue_Struct objQ;
    Timer_Handle handle;
};

struct Timer_Struct {
    Queue_Elem objElem;
    bool staticInst;
    int id;
    /*!
     * @brief Timer start mode
     *
     * Default is @link Timer_StartMode_AUTO @endlink.
     */
    Timer_StartMode startMode;
    /*!
     * @brief Period of a tick
     *
     * The period can be specified in timer counts or microseconds and its
     * default value is 0.
     *
     * The implementation of Timer will support a period of uint32_t timer counts
     * and use pre-scalars if necessary.
     */
    uint32_t period;
    /*!
     * @brief Argument for tick function
     *
     * Default is null.
     */
    uintptr_t arg;
    Hwi_FuncPtr tickFxn;
    Types_FreqHz frequency;
    Hwi_Struct hwiStruct;
    uint64_t period64;
    uint64_t savedCurrCount;
    uint64_t prevThreshold;
    uint64_t nextThreshold;
};

/*!
 * @brief Mask of available timers
 *
 * This mask is used to identify the timers that can be used when Timer_create()
 * is called with an id equal to @ref Timer#ANY "Timer_ANY".
 */
extern const unsigned int Timer_anyMask;
/*!
 * @brief Optional hook function for processing RTC channel 1 events
 *
 * This function will be called when there is a timeout event on RTC Channel 1.
 * It will be called from hardware interrupt context, so any API calls from this
 * function must be appropriate for execution from an ISR.
 *
 * Function hooks are only supported with RunMode_DYNAMIC.
 */
extern const Timer_FuncPtr Timer_funcHookCH1;
/*!
 * @brief Optional hook function for processing RTC channel 2 events.
 *
 * This function will be called when there is a timeout event on RTC Channel 2.
 * It will be called from hardware interrupt context, so any API calls from this
 * function must be appropriate for execution from an ISR.
 *
 * Function hooks are only supported with RunMode_DYNAMIC.
 */
extern const Timer_FuncPtr Timer_funcHookCH2;
extern const unsigned int Timer_startupNeeded;

/*!
 * @brief Create a timer.
 *
 * Create could fail if timer peripheral is unavailable. To request any
 * available timer use @link Timer_ANY @endlink as the id. TimerId's are logical
 * ids. The family-specific implementations map the ids to physical peripherals.
 *
 * or @link Timer_ANY @endlink
 *
 * @param id Timer id ranging from 0 to a platform specific value,
 *
 * @param tickFxn function that runs upon timer expiry.
 */
extern Timer_Handle Timer_create(int id, Timer_FuncPtr tickFxn, const Timer_Params *prms, Error_Block *eb);
extern Timer_Handle Timer_construct(Timer_Struct *obj, int id, Timer_FuncPtr tickFxn, const Timer_Params *prms, Error_Block *eb);
extern void Timer_delete(Timer_Handle *instp);
extern void Timer_destruct(Timer_Struct *obj);
extern void Timer_Params_init(Timer_Params *);
/*!
 * @brief Returns number of timer peripherals on the platform.
 * 
 *@retval Number of timer peripherals.
 */
extern unsigned int Timer_getNumTimers(void);
/*!
 * @brief Returns timer status (free or in use).
 * 
 *@retval timer status
 */
extern Timer_Status Timer_getStatus(unsigned int id);
/*!
 * @brief Reload and start the timer
 *
 * Thread safety must be observed when using the @link Timer_start @endlink and
 * @link Timer_stop @endlink APIs to avoid possible miss- configuration of the
 * timers and unintended behaviors. To protect against re-entrancy, surround the
 * start/stop invocations with @ref ti.sysbios.hal.Hwi#disable "Hwi_disable()"
 * and @ref ti.sysbios.hal.Hwi#restore "Hwi_restore()" calls:
 *
 * @code
 *  // disable interrupts if an interrupt could lead to
 *  // another call to Timer_start().
 *  key = Hwi_disable();
 *  Timer_stop();
 *  ...
 *  Timer_start();
 *  Hwi_restore(key);
 * @endcode
 *
 * <h3>side effects</h3>
 * Enables the timer's interrupt.
 */
extern void Timer_start(Timer_Handle instp);
/*!
 * @brief Stop the timer
 *
 * Thread safety must be observed when using the @link Timer_start @endlink and
 * @link Timer_stop @endlink APIs to avoid possible miss- configuration of the
 * timers and unintended behaviors. To protect against re-entrancy, surround the
 * start/stop invocations with @ref ti.sysbios.hal.Hwi#disable "Hwi_disable()"
 * and @ref ti.sysbios.hal.Hwi#restore "Hwi_restore()" calls:
 *
 * @code
 *  // disable interrupts if an interrupt could lead to
 *  // another call to Timer_start().
 *  key = Hwi_disable();
 *  Timer_stop();
 *  ...
 *  Timer_start();
 *  Hwi_restore(key);
 * @endcode
 *
 * <h3>side effects</h3>
 * Disables the timer's interrupt.
 */
extern void Timer_stop(Timer_Handle instp);
/*!
 * @brief Set timer period specified in timer counts
 *
 * Timer_setPeriod() invokes Timer_stop() prior to setting the period and leaves
 * the timer in the stopped state.
 *
 * To dynamically change the period of a timer you must protect against re-
 * entrancy by disabling interrupts. Use the following call sequence to
 * guarantee proper results:
 *
 * @code
 *  // disable interrupts if an interrupt could lead to
 *  // another call to Timer_start().
 *  key = Hwi_disable();
 *  Timer_setPeriod(period);
 *  Timer_start();
 *  Hwi_restore(key);
 * @endcode
 *
 * Timer implementation must support uint32_t and use pre-scalars whenever
 * necessary
 *
 * <h3>side effects</h3>
 * Calls Timer_stop(), and disables the timer's interrupt.
 *
 * @param period period in timer counts
 */
extern void Timer_setPeriod(Timer_Handle instp, uint32_t period);
/*!
 * @brief Set timer period specified in microseconds.
 *
 * A best-effort method will be used to set the period register. There might be
 * a slight rounding error based on resolution of timer period register. If the
 * timer frequency cannot support the requested period, i.e. the timer period
 * register cannot support the requested period, then this function returns
 * false.
 *
 * Timer_setPeriodMicroSecs() invokes Timer_stop() prior to setting the period
 * and leaves the timer in the stopped state.
 *
 * To dynamically change the period of a timer you must protect against re-
 * entrancy by disabling interrupts. Use the following call sequence to
 * guarantee proper results:
 *
 * @code
 *  // disable interrupts if an interrupt could lead to
 *  // another call to Timer_start().
 *  key = Hwi_disable();
 *  Timer_setPeriodMicroSecs(period);
 *  Timer_start();
 *  Hwi_restore(key);
 * @endcode
 *
 * @param period period in microseconds
 */
extern bool Timer_setPeriodMicroSecs(Timer_Handle instp, uint32_t microsecs);
/*!
 * @brief Get timer period in timer counts
 * 
 *@retval period in timer counts
 */
extern uint32_t Timer_getPeriod(Timer_Handle instp);
/*!
 * @brief Read timer counter register
 * 
 *@retval timer counter value
 */
extern uint32_t Timer_getCount(Timer_Handle instp);
/*!
 * @brief Return timer frequency in Hz
 *
 * This is the effective frequency of the clock incrementing the timer counter
 * register after all scaling factors are taken into account. (including pre-
 * scalars).
 *
 * @param freq frequency in Hz
 */
extern void Timer_getFreq(Timer_Handle instp, Types_FreqHz *freq);
/*!
 * @brief Get Timer function and arg
 *
 * @param arg pointer for returning Timer's function argument
 * 
 *@retval Timer's function
 */
extern Timer_FuncPtr Timer_getFunc(Timer_Handle instp, uintptr_t *arg);
/*!
 * @brief Overwrite Timer function and arg
 *
 * Replaces a Timer object's tickFxn function originally provided in @link
 * Timer_create @endlink.
 *
 * @param fxn pointer to function
 *
 * @param arg argument to function
 */
extern void Timer_setFunc(Timer_Handle instp, Timer_FuncPtr fxn, uintptr_t arg);

extern int Timer_Module_startup(int state);
extern int Timer_Instance_init(Timer_Object *obj, int id, Timer_FuncPtr tickFxn, const Timer_Params *prms, Error_Block *eb);
extern void Timer_Instance_finalize(Timer_Object *obj, int ec);
extern bool Timer_Module__startupDone(void);
/*! @cond NODOC */
/*!
 * @brief Startup function to be called during BIOS_start
 *
 * This function starts statically created timers with startMode =
 * StartMode_AUTO.
 */
extern void Timer_startup(void);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Gets the maximum number of timer ticks that can be skipped (for Clock
 * tick suppression), given the current timer configuration.
 *
 * This API is used internally by SYS/BIOS for dynamic Clock tick suppression.
 * It is not intended to be used for any other purpose.
 */
extern uint32_t Timer_getMaxTicks(Timer_Handle instp);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Dynamically reprograms the timer with a new period value,
 * corresponding to the tick value saved by the last getCurrentTick() call and
 * the number of ticks passed.
 *
 * The timer is left running after the call, and it does not need to be stopped
 * and restarted by the caller.
 *
 * This API is used internally by SYS/BIOS for dynamic Clock tick suppression.
 * It is not intended to be used for any other purpose.
 *
 * @param ticks the corresponding number of ticks
 */
extern void Timer_setNextTick(Timer_Handle instp, uint32_t ticks);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Trigger timer function
 *
 * Timer runs for specified number of cycles. The runMode must be Mode_ONESHOT.
 *
 * This function should interrupt the cpu after specified number of cpu cycles.
 *
 * The last instruction of trigger will start the timer. Depending on how the
 * code is compiled, there may be one or more instructions in between the timer
 * start and client code. The number of instructions specified is counted from
 * when the timer is started.
 *
 * @param instructions cpu cycles
 */
extern void Timer_trigger(Timer_Handle instp, uint32_t cycles);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Get current timer counter
 *
 * Reads timer counter and adds period if IFR was set before counter read. Used
 * exclusively by TimestampProvider.
 *
 * Must be called with interrupts disabled.
 * 
 *@retval expired counts.
 */
extern uint32_t Timer_getExpiredCounts(Timer_Handle instp);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Get the number of ticks that have elapsed since the last timer
 * interrupt was serviced
 *
 * Reads timer counter and determines the number of virtual ticks that have
 * elapsed given the specified tick period.  This function is intended for use
 * only by the Clock module, when using TickMode_DYNAMIC.
 *
 * Must be called with interrupts disabled.
 * 
 *@retval expired ticks.
 */
extern uint32_t Timer_getExpiredTicks(Timer_Handle instp, uint32_t tickPeriod);
/*! @endcond */
/*! @cond NODOC */
/*!
 * @brief Get the current tick number given a specific tick period.
 *
 * Reads timer counter and divides by the tickPeriod to return a corresponding
 * tick number.  This function is used by the Clock module on some targets when
 * using TickMode_DYNAMIC.
 *
 * for later use by setNextTick();
 *
 * @param save true = save internal representation of currentTick
 * 
 *@retval tick number.
 */
extern uint32_t Timer_getCurrentTick(Timer_Handle instp, bool save);
/*! @endcond */
/*! @cond NODOC */
/*!
 *
 * @param arg Unused.
 */
extern void Timer_dynamicStub(uintptr_t arg);
/*! @endcond */
/*! @cond NODOC */
/*!
 *
 * @param arg Unused.
 */
extern void Timer_dynamicMultiStub(uintptr_t arg);
/*! @endcond */
/*! @cond NODOC */
/*!
 *
 * @param arg Unused.
 */
extern void Timer_periodicStub(uintptr_t arg);
/*! @endcond */
/*!
 * @brief Read the 64-bit timer counter register
 * 
 *@retval timer counter value
 */
extern uint64_t Timer_getCount64(Timer_Object *timer);
/*!
 * @brief Returns expired counts (64-bits) since the last serviced interrupt.
 * 
 *@retval timer counter value
 */
extern uint64_t Timer_getExpiredCounts64(Timer_Object *timer);
/*! @cond NODOC */
/*!
 * @brief Used by TimestampProvider module to get hold of timer handle used by
 * Clock.
 *
 * @param id timer Id.
 */
extern Timer_Handle Timer_getHandle(unsigned int id);
/*! @endcond */
extern int Timer_postInit(Timer_Object *timer);
extern void Timer_setThreshold(Timer_Object *timer, uint32_t next, bool wrap);

/*!
 * @brief return handle of the first Timer on Timer list
 *
 * Return the handle of the first Timer on the create/construct list. NULL if
 * no Timers have been created or constructed.
 *
 * @retval Timer handle
 */
extern Timer_Handle Timer_Object_first(void);

/*!
 * @brief return handle of the next Timer on Timer list
 *
 * Return the handle of the next Timer on the create/construct list. NULL if
 * no more Timers are on the list.
 *
 * @param timer Timer handle
 *
 * @retval Timer handle
 */
extern Timer_Handle Timer_Object_next(Timer_Handle timer);

extern Timer_Module_State Timer_Module_state;

#define Timer_module ((Timer_Module_State *) &(Timer_Module_state))

static inline Timer_Handle Timer_handle(Timer_Struct *str)
{
    return ((Timer_Handle)str);
}

static inline Timer_Struct * Timer_struct(Timer_Handle h)
{
    return ((Timer_Struct *)h);
}

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_family_arm_v8m_cc26x4_Timer__include */

#if defined(ti_sysbios_family_arm_v8m_cc26x4_Timer__nolocalnames)
#define do_timer_undef_short_names
#include "Timer_defs.h"
#endif
