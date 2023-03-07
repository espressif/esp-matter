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
 * @file ti/sysbios/knl/Swi.h
 * @brief Software Interrupt Manager
 *
 * The Swi module manages software interrupt service routines, which are
 * patterned after hardware interrupt service routines.
 *
 * SYS/BIOS manages four distinct levels of execution threads: hardware
 * interrupt service routines, software interrupt routines, tasks, and
 * background idle functions. A software interrupt is an object that
 * encapsulates a function to be executed and a priority. Software interrupts
 * are prioritized, preempt tasks, and are preempted by hardware interrupt
 * service routines.
 *
 * Each software interrupt has a priority level. A software interrupt preempts
 * any lower-priority software interrupt currently executing.
 *
 * A target program uses an API call to post a Swi object. This causes the Swi
 * module to schedule execution of the software interrupt's function. When a Swi
 * is posted by an API call, the Swi object's function is not executed
 * immediately. Instead, the function is scheduled for execution. SYS/BIOS uses
 * the Swi's priority to determine whether to preempt the thread currently
 * running. Note that if a Swi is posted several times before it begins running,
 * (because Hwis and higher priority interrupts are running), when the Swi does
 * eventually run, it will run only one time.
 *
 * Software interrupts can be posted for execution with a call to @link Swi_post
 * @endlink or a number of other Swi functions. Each Swi object has a "trigger"
 * which is used either to determine whether to post the Swi or as a value that
 * can be evaluated within the Swi's function. @link Swi_andn @endlink and @link
 * Swi_dec @endlink post the Swi if the trigger value transitions to 0. @link
 * Swi_or @endlink and @link Swi_inc @endlink also modify the trigger value.
 * (@link Swi_or @endlink sets bits, and @link Swi_andn @endlink clears bits.)
 *
 * The @link Swi_disable @endlink and @link Swi_restore @endlink operations
 * allow you to post several Swis and enable them all for execution at the same
 * time. The Swi priorities then determine which Swi runs first.
 *
 * All Swis run to completion; you cannot suspend a Swi while it waits for
 * something (for example, a device) to be ready. So, you can use the trigger to
 * tell the Swi when all the devices and other conditions it relies on are
 * ready. Within a Swi processing function, a call to Swi_getTrigger returns the
 * value of the trigger when the Swi started running. Note that the trigger is
 * automatically reset to its original value when a Swi runs; however, @link
 * Swi_getTrigger @endlink will return the saved trigger value from when the Swi
 * started execution.
 *
 * All Swis run with interrupts globally enabled (ie GIE = 1). Therefore, any
 * Swi module API that results in a Swi being made ready to run (ie @link
 * Swi_post @endlink, @link Swi_inc @endlink, @link Swi_andn @endlink, @link
 * Swi_or @endlink, or @link Swi_restore @endlink)
 * will subsequently also cause interrupts to be enabled while the Swi function
 * executes. Upon return from the Swi function, global interrupts are restored
 * to their previous enabled/disabled state.
 *
 * A Swi preempts any currently running Swi with a lower priority. When multiple
 * Swis of the same priority level have been posted, their respective Swi
 * functions are executed in the order the Swis were posted. Hwis in turn
 * preempt any currently running Swi, allowing the target to respond quickly to
 * hardware peripherals.
 *
 * Swi threads are executed using the ISR (or "Hwi") stack. Thus they share the
 * ISR stack with Hwi threads.
 *
 * To use the Swi module or to set any of the Swi module configuration variables,
 * the following must be added to the app.syscfg file:
 *
 * @code 
 * const Swi = scripting.addModule("/ti/sysbios/knl/Swi");
 * @endcode 
 *
 * @anchor SwiHookFunctions
 *
 * <b>Hook Functions</b>
 *
 * Sets of hook functions can be specified for the Swi module.  Each set
 * contains these hook functions:
 *
 *  - Register:  A function called before all statically-created Swis
 *      are initialized at runtime.
 *  - Create:    A function that is called when a Swi is created.
 *      This includes Swis that are created statically and those
 *      created dynamically using @link Swi_create @endlink.
 *  - Ready:     A function that is called when any Swi becomes ready
 *      to run.
 *  - Begin:     A function that is called just prior to running a Swi.
 *  - End:       A function that is called just after a Swi finishes.
 *  - Delete:    A function that is called when a Swi is deleted at
 *      run-time with Swi_delete.
 *
 *
 * Hook functions can only be configured statically.
 *
 * If you define more than one set of hook functions, all the functions of a
 * particular type will be run when a Swi triggers that type of hook.
 *
 * To add a Swi hook or set of Swi hooks, the following syntax is used in the
 * app.syscfg file:
 *
 * @code
 * const Swi = scripting.addModule("/ti/sysbios/knl/Swi");
 * Swi.swiHooks[0].registerFxn = "myRegisterFxn";
 * Swi.swiHooks[0].createFxn   = "myCreateFxn";
 * Swi.swiHooks[0].readyFxn    = "myrReadyFxn";
 * Swi.swiHooks[0].beginFxn    = "myBeginFxn";
 * Swi.swiHooks[0].endFxn      = "myEndFxn";
 * Swi.swiHooks[0].deleteFxn   = "myDeleteFxn";
 * @endcode
 *
 * Leaving a subset of the hook functions undefined is ok.
 *
 * <b>Register Function</b>
 *
 * The Register function is provided to allow a hook set to store its hookset
 * ID.  This id can be passed to @link Swi_setHookContext @endlink and @link
 * Swi_getHookContext @endlink to set or get hookset-specific context.  The
 * Register function must be specified if the hook implementation needs to use
 * @link Swi_setHookContext @endlink or @link Swi_getHookContext @endlink. The
 * registerFxn hook function is called during system initialization before
 * interrupts have been enabled.
 *
 * @code
 *  void myRegisterFxn(int id);
 * @endcode
 *
 * <b>Create and Delete Functions</b>
 *
 * The create and delete functions are called whenever a Swi is created or
 * deleted.  They are called with interrupts enabled (unless called at boot time
 * or from main()).
 *
 * @code
 *  void myCreateFxn(Swi_Handle swi, Error_Block *eb);
 * @endcode
 *
 * @code
 *  void myDeleteFxn(Swi_Handle swi);
 * @endcode
 *
 * @htmlonly
 *  <b>Ready, Begin, and End Functions</b>
 * @endhtmlonly
 *
 * The ready, begin and end functions are all called with interrupts enabled.
 * The ready function is called when a Swi is posted and made ready to run.  The
 * begin function is called right before the function associated with the given
 * Swi is run.  The end function is called right after this function returns.
 *
 * @code
 *  void myReady(Swi_Handle swi);
 * @endcode
 *
 * @code
 *  void myBegin(Swi_Handle swi);
 * @endcode
 *
 * @code
 *  void myEnd(Swi_Handle swi);
 * @endcode
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center">
 *  </colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th>
 *  <th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                             -->
 *    <tr><td> Swi_create          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_disable         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_getTrigger      </td><td>   Y    </td><td>   Y    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Swi_Params_init     </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Swi_restore         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_self            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   N    </td><td>   N    </td><td>   N    </td></tr>
 *
 *    <tr><td> Swi_andn            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_construct       </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_dec             </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_delete          </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_destruct        </td><td>   N    </td><td>   N    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_getAttrs        </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_getFunc         </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_getHookContext  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_getPri          </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_inc             </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_or              </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_post            </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_setAttrs        </td><td>   Y*   </td><td>   Y*   </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Swi_setHookContext  </td><td>   Y    </td><td>   Y    </td>
 *  <td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> In your module startup after this module is started
 *    (e.g. after Swi_init() has been called). </li>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *             <li> In your module startup before this module is started
 *    (e.g. before Swi_init() has been called). </li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== Swi.h ========
 */

#ifndef ti_sysbios_knl_Swi__include
#define ti_sysbios_knl_Swi__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
#define ti_sysbios_knl_Swi_long_names
#include "Swi_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Assertion raised if Swi_create is called and runtime Swi
 * creation is disabled.
 */
#define Swi_A_swiDisabled "cannot create a SWI when Swi is disabled"

/*!
 * @brief Assertion raised if a Swi's priority is out of range.
 *
 * Swi priorities must be in the range of 0 and Swi_numPriorities-1.
 */
#define Swi_A_badPriority "invalid priority"

typedef struct Swi_Struct Swi_Struct;
typedef struct Swi_Struct Swi_Object;
typedef Swi_Struct* Swi_Handle;
typedef Swi_Struct* Swi_Instance;

/*!
 * @brief Swi hook set type definition
 *
 * This structure defines the set of hook functions that can be specified for
 * the Swi module.
 *
 * See @ref SwiHookFunctions "Hook Functions" for details.
 */
typedef struct Swi_HookSet Swi_HookSet;

typedef struct Swi_Params Swi_Params;

/*! @cond NODOC */
typedef struct Swi_Module_State Swi_Module_State;
typedef Queue_Object Swi_Module_State__readyQ;
/*! @endcond */

/*!
 * @brief Swi function type definition
 *
 * All Swi functions are passed two uninterpreted arguments of type uintptr_t and
 * have no return value.
 */
typedef void (*Swi_FuncPtr)(uintptr_t arg1, uintptr_t arg2);

/*!
 * @brief Swi hook set type definition
 *
 * This structure defines the set of hook functions that can be specified for
 * the Swi module.
 *
 * See @ref SwiHookFunctions "Hook Functions" for details.
 */
struct Swi_HookSet {
    void (*registerFxn)(int arg1);
    void (*createFxn)(Swi_Handle arg1, Error_Block* arg2);
    void (*readyFxn)(Swi_Handle arg1);
    void (*beginFxn)(Swi_Handle arg1);
    void (*endFxn)(Swi_Handle arg1);
    void (*deleteFxn)(Swi_Handle arg1);
};

typedef struct { int length; const Swi_HookSet *elem; } Swi_Hook;

/*! @cond NODOC */
typedef Swi_Struct Swi_Struct2;

struct Swi_Module_State {
    Queue_Struct objQ;
    volatile bool locked;
    unsigned int curSet;
    unsigned int curTrigger;
    Swi_Handle curSwi;
    Queue_Handle curQ;
    Queue_Handle readyQ;
    Queue_Handle constructedSwis;
    bool initDone;
};

struct Swi_Struct {
    Queue_Elem qElem;
    Queue_Elem objElem;
    /*!
     * @brief Optional name for the object. NULL by default.
     */
    char * name;
    Swi_FuncPtr fxn;
    /*!
     * @brief Swi function argument 0
     *
     * The default value of this optional parameter is 0.
     *
     * @see #FuncPtr
     */
    uintptr_t arg0;
    /*!
     * @brief Swi function argument 1
     *
     * The default value of this optional parameter is 0.
     *
     * @see #FuncPtr
     */
    uintptr_t arg1;
    /*!
     * @brief Swi priority
     *
     * Each software interrupt has a priority level, 0 to (@link
     * Swi_numPriorities @endlink - 1). A software interrupt preempts any lower-
     * priority software interrupt currently executing. When multiple Swis of
     * the same priority level have been posted, their respective Swi functions
     * are executed in the order the Swis were posted.
     *
     * The default value of this optional parameter is ~0, which yields a Swi
     * with the highest priority: (@link Swi_numPriorities @endlink - 1).
     */
    unsigned int priority;
    unsigned int mask;
    bool posted;
    unsigned int initTrigger;
    /*!
     * @brief Initial Swi trigger value
     *
     * The default value of this optional parameter is 0.
     *
     * Each Swi object has a "trigger" used either to determine whether to post
     * the Swi or as a value that can be evaluated within the Swi's function.
     *
     * The @link Swi_andn @endlink and @link Swi_dec @endlink functions post the
     * Swi if the trigger value transitions to 0. The @link Swi_or @endlink and
     * @link Swi_inc @endlink functions also modify the trigger value. (@link
     * Swi_or @endlink sets bits, and @link Swi_andn @endlink clears bits.)
     */
    unsigned int trigger;
    Queue_Handle readyQ;
    void * *hookEnv;
};
/*! @endcond */

/*!
 * @brief Number of Swi priorities supported
 *
 * The maximum number of priorities supported is target-specific and depends on
 * the number of bits in a unsigned int data type. For 6x and ARM devices the maximum
 * number of priorities is therefore 32. For the C28x, the maximum number of
 * priorities is 16.
 */
extern const unsigned int Swi_numPriorities;

/*! @cond NODOC */
typedef unsigned int (*CT__Swi_taskDisable)(void);
extern const CT__Swi_taskDisable Swi_taskDisable;

typedef void (*CT__Swi_taskRestore)(unsigned int arg1);
extern const CT__Swi_taskRestore Swi_taskRestore;
/*! @endcond */

/*!
 * @brief const array to hold all HookSet objects.
 */
extern const Swi_Hook Swi_hooks;

struct Swi_Params {
    /*!
     * @brief Optional name for the object. NULL by default.
     */
    char * name;
    /*!
     * @brief Swi function argument 0
     *
     * The default value of this optional parameter is 0.
     *
     * @sa Swi_FuncPtr
     */
    uintptr_t arg0;
    /*!
     * @brief Swi function argument 1
     *
     * The default value of this optional parameter is 0.
     *
     * @sa Swi_FuncPtr
     */
    uintptr_t arg1;
    /*!
     * @brief Swi priority
     *
     * Each software interrupt has a priority level, 0 to (@link
     * Swi_numPriorities @endlink - 1). A software interrupt preempts any lower-
     * priority software interrupt currently executing. When multiple Swis of
     * the same priority level have been posted, their respective Swi functions
     * are executed in the order the Swis were posted.
     *
     * The default value of this optional parameter is ~0, which yields a Swi
     * with the highest priority: (@link Swi_numPriorities @endlink - 1).
     */
    unsigned int priority;
    /*!
     * @brief Initial Swi trigger value
     *
     * The default value of this optional parameter is 0.
     *
     * Each Swi object has a "trigger" used either to determine whether to post
     * the Swi or as a value that can be evaluated within the Swi's function.
     *
     * The @link Swi_andn @endlink and @link Swi_dec @endlink functions post the
     * Swi if the trigger value transitions to 0. The @link Swi_or @endlink and
     * @link Swi_inc @endlink functions also modify the trigger value. (@link
     * Swi_or @endlink sets bits, and @link Swi_andn @endlink clears bits.)
     */
    unsigned int trigger;
};

/*! @cond NODOC */
extern void Swi_init(void);

extern int Swi_Instance_init(Swi_Object *obj, Swi_FuncPtr swiFxn, const Swi_Params *prms, Error_Block *eb);

extern void Swi_Instance_finalize(Swi_Object *obj, int ec);
/*! @endcond */

/*!
 * @brief Create a software interrupt
 *
 * Swi_create creates a new Swi object.
 *
 * The following C code sets Swi parameters and creates two Swi objects:
 *
 * @code
 *  void main()
 *  {
 *      Swi_Params swiParams;
 *
 *      Swi_Params_init(&swiParams);
 *      swiParams.arg0 = 1;
 *      swiParams.arg1 = 0;
 *      swiParams.priority = 2;
 *      swiParams.trigger = 0;
 *
 *      swi0 = Swi_create(swi0Fxn, &swiParams, NULL);
 *
 *      swiParams.arg0 = 2;
 *      swiParams.arg1 = 0;
 *      swiParams.priority = 1;
 *      swiParams.trigger = 3;
 *
 *      swi1 = Swi_create(swi1Fxn, &swiParams, NULL);
 *
 *      BIOS_start();
 *  }
 * @endcode
 *
 * @param swiFxn Swi Function
 * @param prms optional create parameters (NULL for defaults)
 * @param eb error block
 *
 * @retval Swi handle (NULL on failure)
 */
extern Swi_Handle Swi_create(Swi_FuncPtr swiFxn, const Swi_Params *prms, Error_Block *eb);

/*!
 * @brief Construct a software interrupt
 *
 * Swi_construct is equivalent to Swi_create except that the Swi_Struct is
 * pre-allocated.  See Swi_create() for a description of this API.
 *
 * @param obj pointer to a Swi object
 * @param swiFxn Swi Function
 * @param prms optional create parameters (NULL for defaults)
 * @param eb error block
 *
 * @retval Swi handle (NULL on failure)
 */
extern Swi_Handle Swi_construct(Swi_Struct *obj, Swi_FuncPtr swiFxn, const Swi_Params *prms, Error_Block *eb);

/*!
 * @brief Delete a software interrupt
 *
 * Swi_delete deletes a Swi object. Note that Swi_delete takes a pointer to
 * a Swi_Handle which enables Swi_delete to set the Swi_handle to NULL.
 * 
 * @param swi pointer to Swi handle
 */
extern void Swi_delete(Swi_Handle *swi);

/*!
 * @brief Destruct a software interrupt
 *
 * Swi_destruct destructs a Swi object.
 *
 * @param obj pointer to Swi object
 */
extern void Swi_destruct(Swi_Struct *obj);

/*!
 * @brief Initialize the Swi_Params structure with default values.
 *
 * Swi_Params_init initializes the Swi_Params structure with default values.
 * Swi_Params_init should always be called before setting individual parameter
 * fields. This allows new fields to be added in the future with compatible
 * defaults -- existing source code does not need to change when new fields
 * are added.
 *
 * @param prms pointer to uninitialized params structure
 */
extern void Swi_Params_init(Swi_Params *prms);

/*!
 * @brief return handle of the first Swi on Swi list 
 *
 * Return the handle of the first Swi on the create/construct list. NULL if no
 * Swis have been created or constructed.
 *
 * @retval Swi handle
 */
extern Swi_Handle Swi_Object_first(void);

/*!
 * @brief return handle of the next Swi on Swi list 
 *
 * Return the handle of the next Swi on the create/construct list. NULL if no
 * more Swis are on the list.
 *
 * @param swi Swi handle
 *
 * @retval Swi handle
 */
extern Swi_Handle Swi_Object_next(Swi_Handle swi);

/*! @cond NODOC */
extern Swi_Handle Swi_construct2(Swi_Struct2 *swi, Swi_FuncPtr swiFxn, const Swi_Params *prms);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Start the Swi scheduler
 *
 * This function is called in BIOS_start() after Hwi_enable().
 */
extern void Swi_startup(void);

/*!
 * @brief Returns true if the Swi scheduler is enabled
 */
extern bool Swi_enabled(void);

/*!
 * @brief Force a Swi scheduler unlock. Used by Core_atExit() & Core_hwiFunc()
 * to unlock Swi scheduler before exiting.
 *
 * This function should only be called after a Hwi_disable() has entered the
 * Inter-core gate and disabled interrupts locally.
 */
extern void Swi_unlockSched(void);
/*! @endcond */

/*!
 * @brief Disable Swi Scheduling
 *
 * Swi_disable() and @link Swi_restore @endlink control Swi scheduling.
 * Swi_disable() disables all Swi functions from running until Swi_restore() is
 * called. Hardware interrupts can still run.
 *
 * Swi_disable() and Swi_restore() allow you to ensure that statements that must
 * be performed together during critical processing are not preempted by other
 * Swis or Tasks.
 *
 * The value of the key returned by Swi_disable() is opaque to applications and
 * is meant to be passed to Swi_restore().
 *
 * In the following example, the critical section cannot be preempted by any
 * Swis. Nor can it be pre-empted by other Tasks.
 *
 * @code
 *  key = Swi_disable();
 *      `critical section`
 *  Swi_restore(key);
 * @endcode
 *
 * <h3>Side Effects of Disabling the Swi Scheduler</h3>
 *
 * @link Swi_disable @endlink, in addition to disabling Swis from pre-
 * empting the  code which follows its invocation, has the side effect of also
 * disabling the Task scheduler. Consequently, Task pre-emption and blocking is
 * also disabled while the Swi scheduler is disabled. When @ref Swi_restore
 * "Swi_restore()" is subsequently called, it will re-enable and invoke the Task
 * scheduler if the Task scheduler was not already disabled prior to invoking
 * Swi_disable().
 *
 * The following code snippet:
 *
 * @code
 *  key = Swi_disable();
 *  ...
 *  Swi_post(swi);        <-- 'swi' will not run
 *  ...
 *  Swi_restore(key);     <-- 'swi' runs now
 * @endcode
 *
 * Should be thought of as equivalent to this:
 *
 * @code
 *  tasKey = Task_disable();
 *  swiKey = Swi_disable();
 *  ...
 *  Swi_post(swi);        <-- 'swi' will not run
 *  ...
 *  Swi_restore(swiKey);  <-- 'swi' runs now
 *  Task_restore(taskKey);
 * @endcode
 *
 * In the following example, even though the Semaphore_post() call unblocks a
 * task of higher priority, the local task is not pre-empted until after the
 * Swi_restore() call is made:
 *
 * @code
 *  key = Swi_disable();
 *  ...
 *  Swi_post(swi);        <-- 'swi' will not run
 *  Semaphore_post(sem);  <-- readys a task of higher priority than current task
 *  ...
 *  Swi_restore(key);     <-- 'swi' runs now, then current task is pre-empted.
 * @endcode
 *
 * @htmlonly
 *  <B>
 *  A common mistake that users make is to invoke a blocking
 *  API such as Semaphore_pend() after calling Swi_disable().
 *  This results in unrecoverable damage to the Task scheduler's internal
 *  state and will lead to unpredictable and usually catastrophic behavior:
 *  </B>
 * @endhtmlonly
 *
 * @code
 *  key = Swi_disable();
 *  ...
 *  Semaphore_pend(sem, BIOS_WAIT_FOREVER);  <-- !!! DO NOT DO THIS !!!
 *  ...
 *  Swi_restore(key);   <-- !!! System failure guaranteed to follow !!!
 * @endcode
 *
 * @htmlonly
 *  <B>
 *  A more subtle variant of the above problem occurs when an API such
 *  as GateMutex_enter() is invoked directly or indirectly while the
 *  Swi scheduler
 *  is disabled. If the GateMutex has already been entered by another thread,
 *  the current thread should block until the other thread calls
 *  GateMutex_leave().
 *  But because the Task scheduler is disabled, the local thread returns
 *  immediately from GateMutex_enter(), just as though it successfully
 *  entered the GateMutex! This usually leads to catastrophic results.
 *  </B>
 * @endhtmlonly
 * 
 *@retval opaque key for use with Swi_restore()
 */
extern unsigned int Swi_disable(void);

/*! @cond NODOC */
/*!
 * @brief Enable Swi Scheduling
 *
 * Swi_enable unconditionally enables Swis and invokes the Swi scheduler if any
 * Swis are pending.
 *
 * @pre
 * Swi_enable will also invoke the Task scheduler if the Task scheduler is not
 * currently disabled.
 *
 * The @link Swi_post @endlink discussion regarding global interrupts also
 * applies to this API.
 */
extern void Swi_enable(void);
/*! @endcond */

/*!
 * @brief Restore Swi Scheduling state
 *
 * Swi_restore restores the Swi scheduler to the locked/unlocked state it was in
 * when Swi_disable was called. If the scheduler becomes unlocked and Swis of
 * sufficient priority have been made ready to run by any of the posting APIs,
 * then they are run at this time.
 *
 * Swi_disable and Swi_restore control software interrupt processing.
 * Swi_disable disables all other Swi functions from running until Swi_restore
 * is called. Hardware interrupts can still run.
 *
 * Swi_disable and Swi_restore allow you to ensure that statements that must be
 * performed together during critical processing are not pre-empted by other
 * Swis.
 *
 * In the following example, the critical section cannot be preempted by any
 * Swis. Nor can it be pre-empted by other Tasks.
 *
 * @code
 *  key = Swi_disable();
 *      `critical section`
 *  Swi_restore(key);
 * @endcode
 *
 * Read the discussion of the side effects of disabling the Swi scheduler @ref
 * Swi_disable "here".
 *
 * @pre
 * Swi_restore will also re-enable and invoke the Task scheduler if the Task
 * scheduler was not disabled prior to invoking Swi_disable().
 *
 * The @link Swi_post @endlink discussion regarding global interrupts applies to
 * this API.
 *
 * @param key key to restore previous Swi scheduler state
 */
extern void Swi_restore(unsigned int key);

/*! @cond NODOC */
/*!
 * @brief Restore Swi Scheduling state
 *
 * Optimized version used by Hwi dispatcher.
 */
extern void Swi_restoreHwi(unsigned int key);
/*! @endcond */

/*!
 * @brief Return address of currently executing Swi object
 *
 * Swi_self returns the handle of the currently executing Swi.
 *
 * For example, you can call Swi_self as follows if you want a Swi to repost
 * itself:
 *
 * @code
 *  Swi_post(Swi_self());
 * @endcode
 * 
 *@retval handle of currently running Swi
 */
extern Swi_Handle Swi_self(void);

/*!
 * @brief Return the trigger value of the currently executing Swi
 *
 * Swi_getTrigger returns the value that Swi's trigger had when the Swi started
 * running. SYS/BIOS saves the trigger value internally, so that Swi_getTrigger
 * can access it at any point within a Swi object's function, and then
 * automatically resets the trigger to its initial value.
 *
 * Swi_getTrigger should only be called within a function run by a Swi object.
 *
 * When called from within the context of a Swi, the value returned by
 * Swi_getTrigger is zero if the Swi was posted by a call to Swi_andn, or
 * Swi_dec. Therefore, Swi_getTrigger provides relevant information only if the
 * Swi was posted by a call to Swi_inc, Swi_or, or Swi_post.
 *
 * This API is called within a Swi object's function to use the trigger value
 * that caused the function to run. For example, if you use Swi_or or Swi_inc to
 * post a Swi, different trigger values can require different processing.
 *
 * @code
 *  swicount = Swi_getTrigger();
 * @endcode
 * 
 *@retval trigger value
 */
extern unsigned int Swi_getTrigger(void);

/*! @cond NODOC */
/*!
 * @brief Raise a Swi's priority
 *
 * This function is provided for legacy compatibility.
 *
 * Swi_raisePri is used to raise the priority of the currently running Swi to
 * the priority passed in as the argument. Swi_raisePri can be used in
 * conjunction with Swi_restorePri to provide a mutual exclusion mechanism
 * without disabling Swis.
 *
 * Swi_raisePri should be called before a shared resource is accessed, and
 * Swi_restorePri should be called after the access to the shared resource.
 *
 * A call to Swi_raisePri not followed by a Swi_restorePri keeps the Swi's
 * priority for the rest of the processing at the raised level. A Swi_post of
 * the Swi posts the Swi at its original priority level.
 *
 * A Swi object's execution priority must range from 0 to Swi_numPriorities - 1
 *
 * Swi_raisePri never lowers the current Swi priority.
 *
 * Constraints and Calling Context
 *
 *  - Swi_raisePri must only be called from a Swi context.
 *
 * 
 *@retval key for use with restorePri()
 */
extern unsigned int Swi_raisePri(unsigned int priority);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Restore a Swi's priority
 *
 * This function is provided for legacy compatibility.
 *
 * Swi_restorePri restores the priority to the Swi's priority prior to the
 * Swi_raisePri call. Swi_restorePri can be used in conjunction with
 * Swi_raisePri to provide a mutual exclusion mechanism without disabling all
 * Swis.
 *
 * Swi_raisePri should be called right before the shared resource is referenced,
 * and Swi_restorePri should be called after the reference to the shared
 * resource.
 *
 * Constraints and Calling Context
 *
 *  - Swi_restorePri must only be called from a Swi context.
 *
 *
 * @param key key returned from Swi_raisePri
 */
extern void Swi_restorePri(unsigned int key);
/*! @endcond */

/*!
 * @brief Clear bits in Swi's trigger; post if trigger becomes 0
 *
 * Swi_andn is used to conditionally post a software interrupt. Swi_andn clears
 * the bits specified by a mask from Swi's internal trigger. If the Swi's
 * trigger becomes 0, Swi_andn posts the Swi. The bitwise logical operation
 * performed is:
 *
 * @code
 *  trigger = trigger AND (NOT MASK)
 * @endcode
 *
 * If multiple conditions that all be met before a Swi can run, you should use a
 * different bit in the trigger for each condition. When a condition is met,
 * clear the bit for that condition.
 *
 * For example, if two events must happen before a Swi is to be triggered, the
 * initial trigger value of the Swi can be 3 (binary 0011). One call to Swi_andn
 * can have a mask value of 2 (binary 0010), and another call to Swi_andn can
 * have a mask value of 1 (binary 0001). After both calls have been made, the
 * trigger value will be 0.
 *
 * @code
 *  Swi_andn(swi0, 2);  // clear bit 1
 *  Swi_andn(swi0, 1);  // clear bit 0
 * @endcode
 *
 * Swi_andn results in a context switch if the Swi's trigger becomes zero and
 * the Swi has higher priority than the currently executing thread.
 *
 * You specify a Swi's initial trigger value at Swi creation time. The trigger
 * value is automatically reset when the Swi executes.
 *
 * @pre
 * The @link Swi_post @endlink discussion regarding global interrupts applies to
 * this API.
 *
 * @param swi Swi handle
 * @param mask inverse value to be ANDed
 */
extern void Swi_andn(Swi_Handle swi, unsigned int mask);

/*!
 * @brief Decrement Swi's trigger value; post if trigger becomes 0
 *
 * Swi_dec is used to conditionally post a software interrupt. Swi_dec
 * decrements the value in Swi's trigger by 1. If Swi's trigger value becomes 0,
 * Swi_dec posts the Swi. You can increment a trigger value by using Swi_inc,
 * which always posts the Swi.
 *
 * For example, you would use Swi_dec if you wanted to post a Swi after a number
 * of occurrences of an event.
 *
 * @code
 *  // swi0's trigger is configured to start at 3
 *  Swi_dec(swi0);      // trigger = 2
 *  Swi_dec(swi0);      // trigger = 1
 *  Swi_dec(swi0);      // trigger = 0
 * @endcode
 *
 * You specify a Swi's initial trigger value at Swi creation time. The trigger
 * value is automatically reset when the Swi executes.
 *
 * Swi_dec results in a context switch if the Swi's trigger becomes zero and the
 * Swi has higher priority than the currently executing thread.
 *
 * @pre
 * The @link Swi_post @endlink discussion regarding global interrupts applies to
 * this API.
 *
 * @param swi Swi handle
 */
extern void Swi_dec(Swi_Handle swi);

/*!
 * @brief Get hook instance's context pointer for a Swi
 *
 * For example, this C code gets the HookContext, prints it, and sets a new
 * value for the HookContext.
 *
 * @code
 *  void * pEnv;
 *  Swi_Handle mySwi;
 *  int myHookSetId1;
 *
 *  pEnv = Swi_getHookContext(swi, myHookSetId1);
 *
 *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
 *                (ULong)pEnv, (ULong)Timestamp_get32());
 *
 *  Swi_setHookContext(swi, myHookSetId1, (void *)0xc0de1);
 * @endcode
 *
 * See @ref SwiHookFunctions "Hook Functions" for more details.
 *
 * @param swi Swi handle
 * @param id hook id
 * 
 * @retval hook instance's context pointer for Swi
 */
extern void * Swi_getHookContext(Swi_Handle swi, int id);

/*!
 * @brief Set hook instance's context for a swi
 *
 * For example, this C code gets the HookContext, prints it, and sets a new
 * value for the HookContext.
 *
 * @code
 *  void * pEnv;
 *  Swi_Handle mySwi;
 *  int myHookSetId1;
 *
 *  pEnv = Swi_getHookContext(swi, myHookSetId1);
 *
 *  System_printf("myEnd1: pEnv = 0x%lx, time = %ld\n",
 *                (ULong)pEnv, (ULong)Timestamp_get32());
 *
 *  Swi_setHookContext(swi, myHookSetId1, (void *)0xc0de1);
 * @endcode
 *
 * See @ref SwiHookFunctions "Hook Functions" for more details.
 *
 * @param swi Swi handle
 * @param id hook instance's ID
 * @param hookContext value to write to context
 */
extern void Swi_setHookContext(Swi_Handle swi, int id, void * hookContext);

/*!
 * @brief Return a Swi's name
 *
 * Swi_getName returns the name of the Swi passed in as the argument.
 * 
 * @param swi Swi handle
 *
 * @retval name of the Swi
 */
extern char * Swi_getName(Swi_Handle swi);

/*!
 * @brief Return a Swi's priority
 *
 * Swi_getPri returns the priority of the Swi passed in as the argument.
 * 
 * @param swi Swi handle
 *
 * @retval Priority of Swi
 */
extern unsigned int Swi_getPri(Swi_Handle swi);

/*!
 * @brief Get Swi function and arguments
 *
 * If either arg0 or arg1 is NULL, then the corresponding argument is not
 * returned.
 *
 * @sa Swi_getAttrs
 *
 * @param swi Swi handle
 * @param arg0 pointer for returning Swi's first function argument
 * @param arg1 pointer for returning Swi's second function argument
 * 
 * @retval Swi function
 */
extern Swi_FuncPtr Swi_getFunc(Swi_Handle swi, uintptr_t *arg0, uintptr_t *arg1);

/*!
 * @brief Retrieve attributes of an existing Swi object.
 *
 * The 'handle' argument specifies the address of the Swi object whose
 * attributes are to be retrieved.
 *
 * The 'swiFxn' argument is the address of a function pointer where the the Swi
 * function address is to be written to. If NULL is passed for 'swiFxn', no
 * attempt is made to return the Swi function.
 *
 * The 'params' argument is a pointer to a Swi_Params structure that will
 * contain the retrieved Swi attributes.  If 'params' is NULL, no attempt is
 * made to retrieve the Swi_Params.
 *
 * @sa Swi_setAttrs
 *
 * @param swi Swi handle
 * @param swiFxn pointer to a Swi_FuncPtr
 * @param params pointer for returning Swi's Params
 */
extern void Swi_getAttrs(Swi_Handle swi, Swi_FuncPtr *swiFxn, Swi_Params *params);

/*!
 * @brief Set the attributes of an existing Swi object.
 *
 * The 'handle' argument specifies the address of the Swi object whose
 * attributes are to be set.
 *
 * The 'swiFxn' argument is the address of the function to be invoked when the
 * Swi runs. If 'swiFxn' is NULL, no change is made to the Swi function.
 *
 * The 'params' argument, which can be either NULL or a pointer to a Swi_Params
 * structure that contains attributes for the Swi object, facilitates setting
 * the attributes of the Swi object.
 *
 * If 'params' is NULL, the Swi object is assigned a default set of attributes.
 * Otherwise, the Swi object's attributes are set according the values passed
 * within 'params'.
 *
 * @warning Swi_setAttrs() must not be used on a Swi that is preempted or is
 * ready to run.
 *
 * @sa Swi_getAttrs
 *
 * @param swi Swi handle
 * @param swiFxn address of the Swi function
 * @param params pointer to optional Swi_Params structure
 */
extern void Swi_setAttrs(Swi_Handle swi, Swi_FuncPtr swiFxn, Swi_Params *params);

/*!
 * @brief Set a Swi's priority
 *
 * Swi_setPri sets the priority of the Swi passed in as the argument.
 *
 * @pre
 * The priority must be in the range of 0 and numPriorities-1.
 *
 * @pre
 * Swi_setPri() must not be used on a Swi that is preempted or is ready to run.
 *
 * @sa Swi_getPri
 *
 * @param swi Swi handle
 * @param priority priority of Swi
 */
extern void Swi_setPri(Swi_Handle swi, unsigned int priority);

/*!
 * @brief Increment Swi's trigger value and post the Swi
 *
 * Swi_inc increments the value in Swi's trigger by 1 and posts the Swi
 * regardless of the resulting trigger value. You can decrement a trigger value
 * using Swi_dec, which only posts the Swi if the trigger value is 0.
 *
 * If a Swi is posted several times before it has a chance to begin executing
 * (i.e. when Hwis or higher priority Swis are running) the Swi only runs one
 * time. If this situation occurs, you can use Swi_inc to post the Swi. Within
 * the Swi's function, you could then use Swi_getTrigger to find out how many
 * times this Swi has been posted since the last time it was executed.
 *
 * You specify a Swi's initial trigger value at Swi creation time. The trigger
 * value is automatically reset when the Swi executes. To get the trigger value,
 * use Swi_getTrigger.
 *
 * Swi_inc results in a context switch if the Swi is higher priority than the
 * currently executing thread.
 *
 * @pre
 * The @link Swi_post @endlink discussion regarding global interrupts applies to
 * this API.
 *
 * @param swi Swi handle
 */
extern void Swi_inc(Swi_Handle swi);

/*!
 * @brief Or mask with value contained in Swi's trigger and post the Swi.
 *
 * Swi_or is used to post a software interrupt. Swi_or sets the bits specified
 * by a mask in Swi's trigger. Swi_or posts the Swi regardless of the resulting
 * trigger value. The bitwise logical operation performed on the trigger value
 * is:
 *
 * @code
 *  trigger = trigger OR mask
 * @endcode
 *
 * You specify a Swi's initial trigger value at Swi creation time. The trigger
 * value is automatically reset when the Swi executes. To get the trigger value,
 * use Swi_getTrigger.
 *
 * For example, you might use Swi_or to post a Swi if any of three events should
 * cause a Swi to be executed, but you want the Swi's function to be able to
 * tell which event occurred. Each event would correspond to a different bit in
 * the trigger.
 *
 * Swi_or results in a context switch if the Swi is higher priority than the
 * currently executing thread.
 *
 * @pre
 * The @link Swi_post @endlink discussion regarding global interrupts applies to
 * this API.
 *
 * @param swi Swi handle
 * @param mask value to be ORed
 */
extern void Swi_or(Swi_Handle swi, unsigned int mask);

/*!
 * @brief Unconditionally post a software interrupt
 *
 * Swi_post is used to post a software interrupt regardless of the trigger
 * value. No change is made to the Swi object's trigger value.
 *
 * Swi_post results in a context switch if the Swi is higher priority than the
 * currently executing thread.
 *
 * @pre
 * Swis are ALWAYS run with interrupts enabled. If a Swi is made ready to run as
 * a consequence of this API, interrupts will be globally enabled while the Swi
 * function executes, regardless of the prior globally enabled/disabled state of
 * interrupts. Upon return from this API, the global interrupt enabled/disabled
 * state is restored to its previous value.
 *
 * @param swi Swi handle
 */
extern void Swi_post(Swi_Handle swi);

/*! @cond NODOC */
extern void Swi_schedule(void);

extern void Swi_runLoop(void);

extern void Swi_run(Swi_Object *swi);

extern int Swi_postInit(Swi_Object *swi, Error_Block *eb);

extern void Swi_restoreSMP(void);

extern Swi_Module_State Swi_Module_state;

#define Swi_module ((Swi_Module_State *) &(Swi_Module_state))

#define Swi_Object_heap() NULL

static inline char * Swi_Handle_name(Swi_Handle obj)
{
    return (obj->name);
}

static inline Swi_Handle Swi_handle(Swi_Struct *str)
{  
    return ((Swi_Handle)str);
}

static inline Swi_Struct * Swi_struct(Swi_Handle h)
{
    return ((Swi_Struct *)h);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_knl_Swi__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Swi_long_names
#include "Swi_defs.h"
/*! @endcond */
