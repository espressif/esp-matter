/*
 * Copyright (c) 2020, Texas Instruments Incorporated - http://www.ti.com
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
 *
 */
/*!
 * @file ti/sysbios/knl/Clock.h
 * @brief System Clock Manager
 *
 * The System Clock Manager is responsible for all timing services in SYS/BIOS.
 * It generates the periodic system tick. The tick period is configurable. The
 * timeout and period for all Clock Instances and timeout values in other
 * SYS/BIOS modules are specified in terms of Clock ticks.
 *
 * The Clock Manager supports two tick "modes": a periodic mode with an
 * interrupt on each tick (TickMode_PERIODIC), and a tick suppression mode
 * (TickMode_DYNAMIC), which reduces the number of timer interrupts to the
 * minimum required to support the scheduled timeouts.  For devices that support
 * it (e.g., CC13xx/CC26xx devices), TickMode_DYNAMIC may be the default mode if
 * one is not specified in the application configuration; otherwise, the default
 * mode will be TickMode_PERIODIC.  The following example shows how the tick
 * mode  can be specified in the application configuration:
 *
 * @code
 *  const Clock = scripting.addModule("ti/sysbios/knl/Clock");
 *
 *  // Tell the Clock module to use TickMode_PERIODIC
 *  Clock.tickMode = "Clock_TickMode_PERIODIC";
 * @endcode
 *
 * Clock objects contain functions that can be scheduled to run after a certain
 * number of Clock ticks. Clock objects are either one-shot or periodic.
 * Instances are started when created or they are started later using the
 * Clock_start() function. Instances can be stopped using the Clock_stop()
 * function. All Clock Instances are executed when they expire in the context of
 * a software interrupt.
 *
 * Clock objects are placed in the Clock object service list when
 * created/constructed and remain there until deleted/destructed. To minimize
 * processing overhead, unused or expired Clock objects should be deleted or
 * destructed.
 *
 * By default, all Clock functions run in the context of a Swi. That is, the
 * Clock module automatically creates a Swi for its use and runs the Clock
 * functions within that Swi. The priority of the Swi used by Clock can be
 * changed the Clock.swiPriority configuration parameter.
 *
 * If Swis are disabled in an application (if config parameter 'BIOS.swiEnabled'
 * is set to false), then all Clock functions are executed within the
 * context of a Timer Hwi.
 *
 * @note
 *
 *  As Clock functions execute in either a Swi or Hwi context, they
 *  are not permitted to call blocking APIs.
 *
 * The getTicks() function returns number of clock ticks since startup.
 *
 * If you want to use a custom configured timer for the Clock module's tick
 * source, use the following example configuration as a guide:
 *
 * @code
 *  const Clock = scripting.addModule("ti/sysbios/knl/Clock");
 *
 *  // Tell the Clock module that YOU are providing the periodic interrupt
 *  Clock.tickSource = "Clock_TickSource_USER";
 *
 *  // this example uses the ti.sysbios.timers.dmtimer.Timer module
 *  const Timer = scripting.addModule("/ti/sysbios/family/arm/m3/Timer");
 * @endcode
 *
 * In your 'C' code, construct your timer, and add your timer interrupt
 * handler and have it call Clock_tick(), which will perform all of the Clock module tick duties:
 *
 * @code
 *  #include <ti/sysbios/knl/Clock.h>
 *  #include <ti/sysbios/family/arm/m3/Timer.h>
 *
 *  Timer_Struct timer0Struct;
 *  Timer_Handle timer0;
 *
 *  main() {
 *
 *      Timer_Params timerParams0;
 *      Timer_Params_init(&timerParams0);
 *      timerParams0.period = 1000;
 *
 *      timer0 = Timer_construct(&timer0Struct, Timer_ANY, myTimerTick, &timerParams0, NULL);
 *  }
 *
 *  void myTimerTick(uintptr_t arg)
 *  {
 *       Clock_tick();
 *       ...
 *  }
 * @endcode
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *    </colgroup>
 *
 *    <tr><th> Function                </th><th>  Hwi   </th><th>  Swi   </th>
 *    <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                -->
 *    <tr><td> Clock_construct      </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_create         </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_delete         </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_destruct       </td><td>   N    </td><td>   N    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_getTicks       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Clock_getTimerHandle </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Clock_Params_init    </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Clock_tick           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Clock_tickReconfig   </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Clock_tickStart      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Clock_tickStop       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *
 *    <tr><td> Clock_getTimeout     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_isActive       </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_setFunc        </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_setPeriod      </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_setTimeout     </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_start          </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Clock_stop           </td><td>   Y    </td><td>   Y    </td>
 *    <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module has been initialized
 *    (e.g. after Clock_init() has been called). </li>
 *             <li> During
 *    Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During
 *    BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During
 *    Startup.firstFxns.</li>
 *             <li> In your module startup before this module has been initialized
 *    (e.g. before Clock_init() has been called). </li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== Clock.h ========
 */

#ifndef Clock__include
#define Clock__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Swi.h>

#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
#define ti_sysbios_knl_Clock_long_names
#include "Clock_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error and Assert Ids
 */

/*!
 * @brief Asserted in Clock_create
 */
#define Clock_A_clockDisabled "Cannot create a clock instance when BIOS.clockEnabled is false."

/*!
 * @brief Asserted in Clock_create and Clock_delete
 */
#define Clock_A_badThreadType "Cannot create/delete a Clock from a Hwi or Swi thread."

/*!
 * @brief Asserted in Clock_create and Clock_delete
 */
#define Clock_A_badTimeout    "Bad timeout value."

typedef struct Clock_Module_State Clock_Module_State;
typedef struct Clock_Params Clock_Params;
typedef struct Clock_Struct Clock_Struct;
typedef struct Clock_Struct Clock_Object;
typedef Clock_Object* Clock_Handle;
typedef Clock_Object* Clock_Instance;

/*!
 * @brief Clock tick source
 *
 * Like most other module configuration parameters, the Clock.tickPeriod config
 * parameter value is accessible in runtime C code as "Clock_tickPeriod".
 *
 * @sa Clock_tickPeriod
 */
enum Clock_TickSource {
    /*!
     * @brief The Clock module automatically configures a a
     * Timer instance to drive the Clock tick. The specific timer and its
     * period can be controlled via @link Clock_timerId @endlink and
     * @link Clock_tickPeriod @endlink.
     */
    Clock_TickSource_TIMER,
    /*!
     * @brief The Application is responsible for calling @link
     * Clock_tick @endlink periodically. Make sure @link Clock_tickPeriod
     * @endlink is set to the period that Clock_tick() is called.
     */
    Clock_TickSource_USER,
    /*!
     * @brief The Clock module is disabled. In this case, it is an error
     * for the application to ever call Clock_tick().
     */
    Clock_TickSource_NULL
};
typedef enum Clock_TickSource Clock_TickSource;

/*!
 * @brief Clock Tick Mode
 */
enum Clock_TickMode {
    Clock_TickMode_PERIODIC,
    Clock_TickMode_DYNAMIC
};
/*!
 * @brief Clock Tick Mode
 */
typedef enum Clock_TickMode Clock_TickMode;

/*!
 * @brief Instance function prototype
 */
typedef void (*Clock_FuncPtr)(uintptr_t arg1);

/* Params */
struct Clock_Params {
    /*!
     * @brief Start immediately after instance is created
     *
     * When this flag is set to false, the user will have to call Clock_start()
     * to start the instance.
     *
     * When set to true, both statically created Clock objects and Clock objects
     * created in main() are started at the end of main() when the user calls
     * BIOS_start(). Dynamically created Clock objects created after main() (ie
     * within a task) will be started immediately.
     *
     * The default setting for this parameter is false.
     *
     * The configured Clock function will be called initially after an interval
     * equal to the 'timeout' argument for both one-shot and periodic Clock
     * objects.
     *
     * Periodic Clock objects will subsequently be called at the rate specified
     * by the @link Clock_tickPeriod @endlink parameter.
     */
    bool startFlag;
    /*!
     * @brief Period of this instance (in clock ticks)
     *
     * This parameter is used to set the subsequent timeout interval (in Clock
     * ticks) for periodic instances.
     *
     * The default value of this parameter is 0, which indicates this is a one-
     * shot Clock object.
     *
     * A non zero value for this parameter specifies that the Clock object is to
     * be called periodically, and also specifies the rate (in Clock ticks) that
     * the Clock function will be called AFTER the initial 'timeout' argument
     * period.
     *
     * For one-shot Clock instances, this parameter must be set to zero.
     */
    uint32_t period;
    /*!
     * @brief Uninterpreted argument passed to instance function
     *
     * The default is null.
     */
    uintptr_t arg;
};

/* Module_State */
struct Clock_Module_State {
    Queue_Object clockQ;
    volatile uint32_t ticks;
    unsigned int swiCount;
    Swi_Struct swiStruct;
    Swi_Handle swi;
    volatile unsigned int numTickSkip;
    uint32_t nextScheduledTick;
    uint32_t maxSkippable;
    bool inWorkFunc;
    volatile bool startDuringWorkFunc;
    bool ticking;
    /*!
     * @brief Timer tick mode
     *
     * This parameter specifies the tick mode to be used by the underlying
     * Timer.
     *
     * With TickMode_PERIODIC the timer will interrupt the CPU at a fixed rate,
     * defined by the tickPeriod.
     *
     * With TickMode_DYNAMIC the timer can be dynamically reprogrammed by Clock,
     * to interrupt the CPU when the next tick is actually needed for a
     * scheduled timeout. TickMode_DYNAMIC is not supported on all devices, and
     * may have some application constraints.
     */
    Clock_TickMode tickMode;
    /*!
     * @brief Tick period specified in microseconds
     *
     * Default value is family dependent. For example, Linux systems often only
     * support a minimum period of 10000 us and multiples of 10000 us. TI
     * platforms have a default of 1000 us.
     *
     * Like most other module configuration parameters, the Clock.tickPeriod
     * config parameter value is accessible in runtime C code as
     * "Clock_tickPeriod".
     */
    uint32_t tickPeriod;
    /*!
     * @brief Source of clock ticks
     *
     * If this parameter is not set to TickSource_TIMER, @link Clock_tickStart
     * @endlink, @link Clock_tickStop @endlink, and @link Clock_tickReconfig
     * @endlink, have no effect.
     *
     * The default is TickSource_TIMER.
     */
    Clock_TickSource tickSource;
    /*!
     * @brief Used to insure idem potency during startup
     */
    bool initDone;
};

/* Struct */
struct Clock_Struct {
    Queue_Elem objElem;
    uint32_t timeout;
    uint32_t currTimeout;
    /*!
     * @brief Period of this instance (in clock ticks)
     *
     * This parameter is used to set the subsequent timeout interval (in Clock
     * ticks) for periodic instances.
     *
     * The default value of this parameter is 0, which indicates this is a one-
     * shot Clock object.
     *
     * A non zero value for this parameter specifies that the Clock object is to
     * be called periodically, and also specifies the rate (in Clock ticks) that
     * the Clock function will be called AFTER the initial 'timeout' argument
     * period.
     *
     * For one-shot Clock instances, this parameter must be set to zero.
     */
    uint32_t period;
    volatile bool active;
    Clock_FuncPtr fxn;
    /*!
     * @brief Uninterpreted argument passed to instance function
     *
     * The default is null.
     */
    uintptr_t arg;
    uint32_t timeoutTicks;
};

/*! @cond NODOC */
/*!
 * @brief !!! Do not delete. Required for ROM compatibility !!!
 */
extern const uint32_t Clock_serviceMargin;
/*! @endcond */

/*!
 * @brief Source of clock ticks
 *
 * If this parameter is not set to TickSource_TIMER, @link Clock_tickStart
 * @endlink, @link Clock_tickStop @endlink, and @link
 * Clock_tickReconfig @endlink, have no effect.
 *
 * The default is Clock_TickSource_TIMER.
 */
extern const Clock_TickSource Clock_tickSource;

/*!
 * @brief Timer tick mode
 *
 * This parameter specifies the tick mode to be used by the underlying Timer.
 *
 * With TickMode_PERIODIC the timer will interrupt the CPU at a fixed rate,
 * defined by the tickPeriod.
 *
 * With TickMode_DYNAMIC the timer can be dynamically reprogrammed by Clock, to
 * interrupt the CPU when the next tick is actually needed for a scheduled
 * timeout. TickMode_DYNAMIC is not supported on all devices, and may have some
 * application constraints.
 */
extern const Clock_TickMode Clock_tickMode;

/*!
 * @brief Timer Id used to create a Timer instance
 *
 * If @link Clock_tickSource @endlink is set to TickSource_TIMER, the
 * Clock module internally creates a static Timer instance
 * that automatically calls Clock_tick() on a periodic basis (as specified
 * by @link Clock_tickPeriod @endlink.)
 *
 * This configuration parameter allows you to control which timer is used to
 * drive the Clock module.
 *
 * The default value is Timer_ANY (~0) and the maximum timerId possible is
 * device specific.
 */
extern const unsigned int Clock_timerId;

/*!
 * @brief Tick period specified in microseconds
 *
 * Default value is family dependent. For example, Linux systems often only
 * support a minimum period of 10000 us and multiples of 10000 us. TI platforms
 * have a default of 1000 us.
 *
 * Like most other module configuration parameters, the Clock.tickPeriod config
 * parameter value is accessible in runtime C code as "Clock_tickPeriod".
 */
extern const uint32_t Clock_tickPeriod;

/*! @cond NODOC */
extern const Clock_Handle Clock_triggerClock;

extern void Clock_init(void);

extern void Clock_Instance_init(Clock_Object *obj, Clock_FuncPtr clockFxn, unsigned int timeout, const Clock_Params *prms);

extern void Clock_Instance_finalize(Clock_Object *obj);
/*! @endcond */

/*!
 * @brief Creates a Clock Instance
 *
 * The first argument is the function that gets called when the timeout expires.
 *
 * The 'timeout' argument is used to specify the startup timeout for both one-
 * shot and periodic Clock instances (in Clock ticks).  This timeout is applied
 * when the Clock instance is started.  For periodic instances, the configured
 * Clock function will be called initially after an interval equal to the
 * timeout, and will be subsequently called at the rate specified by the
 * period parameter.  For one-shot instances (where the period parameter is 0),
 * once the Clock instance is started (with @link Clock_start @endlink or
 * automatically if 'startFlag' is true) the configured Clock function will be
 * called once after an interval equal to the timeout.
 *
 * When instances are created they are placed upon a linked list managed by the
 * Clock module.  For this reason, instances cannot be created from either Hwi
 * or Swi context.
 *
 * By default, all Clock functions run in the context of a Swi. That is, the
 * Clock module automatically creates a Swi for its use and runs the Clock
 * functions within that Swi. The priority of the Swi used by Clock can be
 * changed with the Clock.swiPriority configuration parameter.
 *
 * If Swis are disabled in an application (config parmeter BIOS.swiEnabled is
 * set to false), then all Clock functions are executed within the context of
 * a Timer Hwi.
 *
 * <h3>constraint</h3>
 *
 * As Clock functions execute in either a Swi or Hwi context, they
 * are not permitted to call blocking APIs.
 *
 * @param clockFxn Function that runs upon timeout
 * @param timeout One-shot timeout or initial start delay (in clock ticks)
 * @param prms Clock parameters
 * @param eb Error block
 *
 * @retval Clock handle
 */
extern Clock_Handle Clock_create(Clock_FuncPtr clockFxn, unsigned int timeout, const Clock_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a Clock
 *
 * Clock_construct is equivalent to Clock_create except that the Clock_Struct is
 * pre-allocated.  See Clock_create() for a description of this API.
 *
 * @param obj pointer to a Clock object
 * @param clockFxn Clock function
 * @param timeout One-shot timeout or initial start delay (in clock ticks)
 * @param prms Clock parameters
 *
 * @retval Clock handle (NULL on failure)
 */
extern Clock_Handle Clock_construct(Clock_Struct *obj, Clock_FuncPtr clockFxn, unsigned int timeout, const Clock_Params *prms);

/*!
 * @brief Delete a Clock
 *
 * Clock_delete deletes a Clock object. Note that Clock_delete takes a pointer to
 * a Clock_Handle which enables Clock_delete to set the Clock_handle to NULL.
 * 
 * @param clock pointer to Clock handle
 */
extern void Clock_delete(Clock_Handle *clock);

/*!
 * @brief Destruct a Clock
 *
 * Clock_destruct destructs a Clock object.
 *
 * @param obj pointer to Clock object
 */
extern void Clock_destruct(Clock_Struct *obj);

/*!
 * @brief Initialize the Clock_Params structure with default values.
 *
 * Clock_Params_init initializes the Clock_Params structure with default values.
 * Clock_Params_init should always be called before setting individual parameter
 * fields. This allows new fields to be added in the future with compatible
 * defaults -- existing source code does not need to change when new fields
 * are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Clock_Params_init(Clock_Params *prms);

/*!
 * @brief return handle of the first Clock on Clock list 
 *
 * Return the handle of the first Clock on the create/construct list. NULL if no
 * Clocks have been created or constructed.
 *
 * @retval Clock handle
 */
extern Clock_Handle Clock_Object_first(void);

/*!
 * @brief return handle of the next Clock on Clock list 
 *
 * Return the handle of the next Clock on the create/construct list. NULL if no
 * more Clocks are on the list.
 *
 * @param clock Clock handle
 *
 * @retval Clock handle
 */
extern Clock_Handle Clock_Object_next(Clock_Handle clock);

/*!
 * @brief Time in Clock ticks
 *
 * The value returned will wrap back to zero after it reaches the max value that
 * can be stored in 32 bits.
 * 
 * @retval time in clock ticks
 */
extern uint32_t Clock_getTicks(void);

/*! @cond NODOC */
/*!
 * @brief Get timer Handle
 *
 * Used when it is necessary to change family specific options for the timer and
 * its Hwi Object.
 * 
 * @retval Timer Handle
 */
extern void * Clock_getTimerHandle(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Set the internal Clock tick counter
 *
 * Used internally by Power modules. Only applicable for Clock.TickMode_PERIODIC
 */
extern void Clock_setTicks(uint32_t ticks);
/*! @endcond */

/*!
 * @brief Stop clock for reconfiguration
 *
 * This function is used to stop the timer used for generation of clock ticks.
 * It is used along with Clock_tickStart() and Clock_tickReconfig() to allow
 * reconfiguration of timer at runtime.
 *
 * Stopping the timer may not be supported for some types of timers, and is not
 * supported for Clock.TickMode_DYNAMIC; in these cases, this this function call
 * will have no effect.
 *
 * @pre
 * This function is non-reentrant and appropriate locks must be used to protect
 * against  re-entrancy.
 */
extern void Clock_tickStop(void);

/*!
 * @brief Reconfigure clock for new cpu frequency
 *
 * This function uses the new cpu frequency to reconfigure the timer used for
 * generation of clock ticks such that tick period is accurate.  This function
 * is used along with Clock_tickStop() and Clock_tickStart() to allow
 * reconfiguration of timer at runtime.
 *
 * Reconfiguration may not be supported for some types of timers, and is not
 * supported for Clock.TickMode_DYNAMIC; in these cases, this this function call
 * will have no effect, and will return false.
 *
 * When calling Clock_tickReconfig outside of main(), you must also call
 * Clock_tickStop and Clock_tickStart to stop and restart the timer. Use the
 * following call sequence:
 *
 * @code
 *  // disable interrupts if an interrupt could lead to
 *  // another call to Clock_tickReconfig or if interrupt
 *  // processing relies on having a running timer
 *  Hwi_disable() or Swi_disable();
 *  BIOS_setCpuFreq(&freq);
 *  Clock_tickStop();
 *  Clock_tickReconfig();
 *  Clock_tickStart();
 *  Hwi_restore() or Swi_enable()
 * @endcode
 *
 * When calling Clock_tickReconfig from main(), the timer has not yet been
 * started because the timer is started as part of BIOS_start(). As a result,
 * you can use the following simplified call sequence in main():
 *
 * @code
 *  BIOS_setCpuFrequency(Types.FreqHz *freq);
 *  Clock_tickReconfig(void);
 * @endcode
 *
 * The return value is false if the timer cannot support the new frequency
 *
 * @pre
 * This function is non-reentrant and appropriate locks must be used to protect
 * against  re-entrancy.
 * 
 * @retval true if successful
 */
extern bool Clock_tickReconfig(void);

/*!
 * @brief Start clock after reconfiguration
 *
 * This function starts the timer used for generation of clock ticks It is used
 * along with Clock_tickStop() and Clock_tickReconfig() to allow reconfiguration
 * of timer at runtime. The new timer configuration reflects changes caused by a
 * call to reconfig().
 *
 * Reconfiguration and restart of a timer may not be supported for some types of
 * timers, and is not supported for Clock.TickMode_DYNAMIC; in these cases, this
 * function call will have no effect.
 *
 * @pre
 * This function is non-reentrant and appropriate locks must be used to protect
 * against  re-entrancy.
 */
extern void Clock_tickStart(void);

/*!
 * @brief Advance Clock time by one tick
 *
 * After incrementing a global tick counter, this function posts a Swi that
 * processes the clock instances.
 *
 * This function is automatically called by a timer ISR when @link
 * Clock_tickSource @endlink is set to @link Clock_TickSource_TIMER @endlink.
 *
 * When @link Clock_tickSource @endlink is set to @link Clock_TickSource_USER
 * @endlink, Clock_tick() must be called by the application.  Usually, this is
 * done within a user defined ISR.
 *
 * Note that this function is not re-entrant.  The application is responsible
 * for ensuring that invocations of this function are serialized: either only
 * one thread in the system ever calls this function or all calls are "wrapped"
 * by an appropriate mutex.
 */
extern void Clock_tick(void);

/*! @cond NODOC */
/*!
 * @brief Clock Q service routine
 *
 * @param arg0 Unused. required to match Swi.FuncPtr
 *
 * @param arg1 Unused. required to match Swi.FuncPtr
 */
extern void Clock_workFunc(uintptr_t arg0, uintptr_t arg1);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Clock Q service routine for TickMode_DYNAMIC
 *
 * @param arg0 Unused. required to match Swi.FuncPtr
 *
 * @param arg1 Unused. required to match Swi.FuncPtr
 */
extern void Clock_workFuncDynamic(uintptr_t arg0, uintptr_t arg1);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Log the LD_tick from within Clock module scope
 */
extern void Clock_logTick(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Get the number of Clock ticks that have completed
 *
 * Returns the number of ticks completed, to the point where the underlying
 * Timer interrupt has been serviced.
 *
 * Used by some TimestampProviders
 * 
 *@retval time in clock ticks
 */
extern uint32_t Clock_getCompletedTicks(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Get the Clock tick period in timer counts
 *
 * The period is in units returned by the underlying Timer.
 *
 * Used by some TimestampProviders
 * 
 *@retval period in timer counts
 */
extern uint32_t Clock_getTickPeriod(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Get the number of Clock tick periods expected to expire between now
 * and the next interrupt from the timer peripheral
 *
 * Used internally by Power modules.
 * 
 *@retval count in ticks
 */
extern uint32_t Clock_getTicksUntilInterrupt(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Get the number of Clock tick periods between now and the next active
 * Clock object timeout.
 *
 * Used internally by Power modules.
 *
 * @pre
 * Must be called with interrupts disabled.  Only applicable for
 * Clock.TickSource_TIMER.
 * 
 *@retval count in ticks
 */
extern uint32_t Clock_getTicksUntilTimeout(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Walk Clock's work queue for TickMode_DYNAMIC
 */
extern uint32_t Clock_walkQueueDynamic(bool service, uint32_t tick);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Walk Clock's work queue for TickMode_PERIODIC
 */
extern uint32_t Clock_walkQueuePeriodic(void);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Reprogram Clock's Timer for earliest required tick
 */
extern void Clock_scheduleNextTick(uint32_t deltaTicks, uint32_t absTick);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Lightweight One-Shot Clock create for internal SYS/BIOS timeout APIs
 * Does NOT start the timeout (ie requires Clock_startI() to be called) Does NOT
 * assume Hwis are disabled
 */
extern void Clock_addI(Clock_Handle clock, Clock_FuncPtr clockFxn, uint32_t timeout, uintptr_t arg);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Lightweight One-Shot Clock create for internal SYS/BIOS timeout APIs
 * Does NOT start the timeout (ie requires Clock_startI() to be called) Does NOT
 * assume Hwis are disabled Does NOT add Clock object to Clock Q.
 */
extern void Clock_initI(Clock_Handle clock, Clock_FuncPtr clockFxn, uint32_t timeout, uintptr_t arg);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Add Clock object to Clock Q. Assumes Hwis are disabled
 */
extern void Clock_enqueueI(Clock_Handle clock);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Lightweight Clock delete for internal SYS/BIOS timeout APIs Assumes
 * Hwis are disabled
 */
extern void Clock_removeI(Clock_Handle clock);
/*! @endcond */

/*!
 * @brief Start instance
 *
 * The timeout and period values set during create() or by calling
 * Clock_setTimeout() and Clock_setPeriod() are used and the expiry is
 * recomputed. Note that for periodic instances, the first expiry is computed
 * using the timeout specified. All subsequent expiries use the period value.
 *
 * @pre
 * Timeout of instance cannot be zero
 */
extern void Clock_start(Clock_Handle clock);

/*! @cond NODOC */
/*!
 * @brief Internal start function which assumes Hwis disabled
 */
extern void Clock_startI(Clock_Handle clock);
/*! @endcond */

/*!
 * @brief Stop instance
 *
 * @param clock Clock handle
 */
extern void Clock_stop(Clock_Handle clock);

/*!
 * @brief Set periodic interval
 *
 * @pre
 * Cannot change period of instance that has been started.
 *
 * @param clock Clock handle
 * @param period periodic interval in Clock ticks
 */
extern void Clock_setPeriod(Clock_Handle clock, uint32_t period);

/*!
 * @brief Set the initial timeout
 *
 * @pre
 * Cannot change the initial timeout of instance that has been started.
 *
 * @param clock Clock handle
 * @param timeout initial timeout in Clock ticks
 */
extern void Clock_setTimeout(Clock_Handle clock, uint32_t timeout);

/*!
 * @brief Overwrite Clock function and arg
 *
 * Replaces a Clock object's clockFxn function originally provided in @link
 * Clock_create @endlink.
 *
 * @pre
 * Cannot change function and arg of Clock object that has been started.
 *
 * @param clock Clock handle
 * @param fxn Clock function
 * @param arg argument to clock function
 */
extern void Clock_setFunc(Clock_Handle clock, Clock_FuncPtr fxn, uintptr_t arg);

/*!
 * @brief Get period of instance
 *
 * Returns the period of an instance.
 * 
 * @param clock Clock handle
 *
 * @retval returns periodic interval in Clock ticks
 */
extern uint32_t Clock_getPeriod(Clock_Handle clock);

/*!
 * @brief Get timeout of instance
 *
 * Returns the remaining time if the instance is active; if the instance is not
 * active, returns zero.
 * 
 * @param clock Clock handle
 *
 * @retval returns timeout in clock ticks
 */
extern uint32_t Clock_getTimeout(Clock_Handle clock);

/*!
 * @brief Determine if Clock object is currently active (ie running)
 *
 * Returns true if Clock object is currently active
 * 
 * @param clock Clock handle
 *
 * @retval returns active state
 */
extern bool Clock_isActive(Clock_Handle clock);

/* @cond NODOC */
/*!
 * @brief Function called by the timer interrupt handler
 *
 * @param arg Unused. Required to match signature of Hwi.FuncPtr
 */
extern void Clock_doTick(uintptr_t arg);

extern void Clock_triggerFunc(uintptr_t arg);

extern Clock_Module_State Clock_Module_state;

#define Clock_module ((Clock_Module_State *) &(Clock_Module_state))

static inline Clock_Handle Clock_handle(Clock_Struct *str)
{  
    return ((Clock_Handle)str);
}

static inline Clock_Struct * Clock_struct(Clock_Handle h)
{
    return ((Clock_Struct *)h);
}

/* CC32XX DPL calls this internal function, map it for now */
#define ti_sysbios_knl_Clock_doTick(a) Clock_tick()
/* @endcond */

#ifdef __cplusplus
}
#endif
#endif /* Clock__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Clock_long_names
#include "Clock_defs.h"
/*! @endcond */
