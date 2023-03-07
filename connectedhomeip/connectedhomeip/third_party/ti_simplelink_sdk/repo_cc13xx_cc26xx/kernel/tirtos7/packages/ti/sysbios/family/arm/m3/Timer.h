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
 * @file ti/sysbios/family/arm/m3/Timer.h
 * @brief Cortex M3 Timer Peripherals Manager.
 *
 * The Cortex M3 Timer Manager utilizes the M3's internal NVIC SysTick Timer.
 * The SysTick timer is clocked at the CPU clock rate, nominally 200MHz. The
 * SysTick timer has only 24 bits of period resolution and is therefore limited
 * to a maximum period of 83ms.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center"></colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                                                                 -->
 *    <tr><td> getNumTimers            </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> getStatus               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> Params_init             </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> construct               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> create                  </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> delete                  </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> destruct                </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> getCount                </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> getFreq                 </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> getPeriod               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> reconfig                </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> setPeriod               </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> setPeriodMicroSecs      </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> start                   </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td> stop                    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td><td>   -    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started (e.g. Cache_Module_startupDone() returns true). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started (e.g. Cache_Module_startupDone() returns false).</li>
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

#include <ti/sysbios/family/arm/m3/Hwi.h>
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

#ifndef ti_sysbios_family_arm_m3_Timer__include
#define ti_sysbios_family_arm_m3_Timer__include
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
    volatile unsigned int tickCount;    /* SysTick Rollover counter */
    unsigned int availMask;             /* available peripherals */
    Timer_Handle handle;
};

struct Timer_Struct {
    Queue_Elem objElem;
    bool staticInst;
    int id;
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
     * @brief Period of a tick
     *
     * The period can be specified in timer counts or microseconds and its
     * default value is 0.
     *
     * The implementation of Timer will support a period of uint32_t timer counts
     * and use pre-scalars if necessary.
     */
    unsigned int period;
    /*!
     * @brief Period type
     *
     * Default is PeriodType_MICROSECS
     */
    Timer_PeriodType periodType;
    unsigned int intNum;
    /*!
     * @brief Argument for tick function
     *
     * Default is null.
     */
    uintptr_t arg;
    Hwi_FuncPtr tickFxn;
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
    Types_FreqHz extFreq;        /* external frequency in Hz */
    Hwi_Handle hwi;
};

/*!
 * @brief Available mask to be used when select = Timer_ANY
 */
extern const unsigned int Timer_anyMask;
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
 * @brief Startup function to be called during BIOS_start
 *
 * This function starts statically created timers with startMode =
 * StartMode_AUTO.
 */
extern void Timer_startup(void);
/*! @endcond */
extern int Timer_Instance_init(Timer_Object *obj, int id, Timer_FuncPtr tickFxn, const Timer_Params *prms, Error_Block *eb);
extern void Timer_Instance_finalize(Timer_Object *obj, int ec);
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
/*!
 * @brief SysTick timer does not support one shot mode. This stub stops timer
 * and clears the pending timer interrupt.
 *
 * @param arg Timer object.
 */
extern void Timer_oneShotStub(uintptr_t arg);
/*!
 * @brief SysTick timer interrupt must be acknowledged. This stub acknowledges
 * timer
 *
 * @param arg Timer object.
 */
extern void Timer_periodicStub(uintptr_t arg);

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

/*! @cond NODOC */
/*!
 * @brief Used by TimestampProvider module to get hold of timer handle.
 *
 * @param id timer Id.
 */
extern Timer_Handle Timer_getHandle(unsigned int id);
/*! @endcond */
extern void Timer_initDevice(Timer_Object *timer);
extern int Timer_postInit(Timer_Object *obj, Error_Block *eb);

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

#endif /* ti_sysbios_family_arm_m3_Timer__include */

#if defined(ti_sysbios_family_arm_m3_Timer__nolocalnames)
#define do_timer_undef_short_names
#include "Timer_defs.h"
#endif

