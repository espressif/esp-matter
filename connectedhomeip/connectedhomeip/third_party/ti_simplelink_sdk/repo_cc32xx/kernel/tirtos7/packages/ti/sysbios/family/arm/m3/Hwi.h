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
 * @file ti/sysbios/family/arm/m3/Hwi.h
 * @brief Cortex M3/M4 Hardware Interrupt Manager
 *
 * The Hardware Interrupt Manager provides APIs for managing hardware interrupts.
 *
 * The user can dynamically assign routines that run when specific
 * hardware interrupts occur.
 *
 * Dynamic assignment of Hwi routines to interrupts at run-time is done using
 * the Hwi_create() or Hwi_construct() functions.
 *
 * Interrupt routines can be written completely in C, completely in assembly, or
 * in a mix of C and assembly. 
 *
 * In order to support interrupt routines written
 * completely in C, an interrupt dispatcher is provided that performs the
 * requisite prolog and epilog for an interrupt routine so the interrupt
 * function can safely inter-operate with the BIOS Task and Swi schedulers.
 *
 * Some routines are assigned to interrupts by the other SYS/BIOS modules. For
 * example, the Clock module configures its own timer interrupt handler. See the
 * Clock Module for more details.
 *
 * <h3>Runtime Hwi Creation</h3>
 *
 * Below is an example of configuring an interrupt at runtime. Usually this code
 * would be placed in main().
 *
 * @code
 *  #include <ti/sysbios/family/arm/m3/Hwi.h>
 *  #include <ti/sysbios/runtime/Error.h>
 *
 *  Hwi_Handle myHwi;
 *
 *  int main(int argc, char* argv[])
 *  {
 *      Hwi_Params hwiParams;
 *
 *      // set the argument you want passed to your ISR function
 *      hwiParams.arg = 1;
 *
 *      //
 *      // Configure interrupt 16 to invoke "myIsr".
 *      // Automatically enables interrupt 16 by default
 *      // set params.enableInt = false if you want to control
 *      // when the interrupt is enabled using Hwi_enableInterrupt()
 *      //
 *
 *      myHwi = Hwi_create(16, myIsr, &hwiParams, Error_IGNORE);
 *
 *      if (myHwi == NULL) {
 *          // handle Hwi_create failure
 *      }
 *  }
 *
 *  void myIsr(uintptr_t arg)
 *  {
 *      // here when interrupt #16 goes off
 *  }
 * @endcode
 *
 * Below is an example of configuring the same interrupt without requiring the use of a
 * heap.
 *
 * @code
 *  #include <ti/sysbios/family/arm/m3/Hwi.h>
 *  #include <ti/sysbios/runtime/Error.h>
 *
 *  Hwi_Struct myHwiStruct;
 *  Hwi_Handle myHwi;
 *
 *  int main(int argc, char* argv[])
 *  {
 *      Hwi_Params hwiParams;
 *
 *      // set the argument you want passed to your ISR function
 *      hwiParams.arg = 1;
 *
 *      //
 *      // Configure interrupt 16 to invoke "myIsr".
 *      // Automatically enables interrupt 16 by default
 *      // set params.enableInt = false if you want to control
 *      // when the interrupt is enabled using Hwi_enableInterrupt()
 *      //
 *
 *      myHwi = Hwi_construct(&myHwiStruct, 16, myIsr, &hwiParams, Error_IGNORE);
 *
 *      if (myHwi == NULL) {
 *          // handle Hwi_create failure
 *      }
 *  }
 *
 *  void myIsr(uintptr_t arg)
 *  {
 *      // here when interrupt #16 goes off
 *  }
 * @endcode
 *
 * The Cortex-M devices' Nested Vectored Interrupt Controller (NVIC) supports up
 * to 256 interrupts/exceptions. In practice, most devices support much fewer
 * (ie the SimpleLink CC13XX/CC26XX family of devices have around 50 total
 * interrupts defined).
 *
 * SYS/BIOS Interrupt IDs or interrupt numbers correspond to an interrupt's
 * position in the interrupt vector table.
 *
 * ID 0 corresponds to vector 0 which is used by the NVIC to hold the initial
 * (reset) stack pointer value.
 *
 * ID 1 corresponds to vector 1 which is the reset vector which is usually
 * initialized to point to an application's entry point (ie for the TI compiler
 * tool chain, the entry point is "_c_int00")
 *
 * IDs 2-13 are, by default, hard wired to the internal exception handler which
 * will save important context information that can be viewed using the ROV tool
 * within either the Code Composer Studio debugger or the IAR Workbench
 * debugger.
 *
 * ID 14 is the "pendSV" handler which is used exclusively by the shared
 * interrupt dispatcher to orchestrate the execution of 
 * "Swis" posted from within interrupts, as well as to
 * manage asynchronous task pre-emption upon returning from interrupts which
 * have readied a task of higher priority than the task that was interrupted.
 *
 * ID 15 is the SysTick timer interrupt.
 *
 * ID's 16-255 are mapped to the NVIC's "User" interrupts 0-239 which are tied
 * to platform specific interrupt sources.
 *
 * <h3>Zero Latency Interrupts</h3>
 * The M3/M4 Hwi module supports "zero latency" interrupts. Interrupts
 * configured with priority greater (in actual hardware priority, but lower in
 * number) than the configured @link Hwi_disablePriority @endlink
 * are NOT disabled by Hwi_disable(), and they are not
 * managed by the internal interrupt dispatcher.
 *
 * Zero Latency interrupts fall into the commonly used category of "Unmanaged
 * Interrupts". However they are somewhat distinct from that definition in that
 * in addition to being unmanaged, they are also almost never disabled by
 * SYS/BIOS code, thus gaining the "Zero Latency" title.
 *
 * Zero latency interrupts are distinguished from regular dispatched interrupts
 * at create time solely by their interrupt priority being set greater than the
 * configured @link Hwi_disablePriority @endlink.
 *
 * Note that since zero latency interrupts don't use the dispatcher,
 * the "arg" parameter is not functional. Also note
 * that due to the Cortex-M's native automatic stacking of saved-by-caller C
 * context on the way to an ISR, zero latency interrupt handlers are implemented
 * using regular C functions (ie no 'interrupt' keyword is required).
 *
 * @warning
 * Zero latency interrupts are NOT HANDLED by the SYS/BIOS interrupt dispatcher!
 * Instead, they are vectored to directly. As such, and because they are NOT
 * DISABLED BY Hwi_disable(), these interrupt handlers are SEVERELY RESTRICTED
 * in terms of the SYS/BIOS APIs they can invoke and THREAD SAFETY MUST BE
 * CAREFULLY CONSIDERED! See the descriptions of Hwi_disable
 * and the configuration parameter "Hwi.disablePriority" for
 * more details.
 *
 * <h3>Interrupt Masking Options</h3>
 *
 * The NVIC interrupt controller is designed for priority based interrupts.
 *
 * In this Hwi module, the Hwi_maskSetting instance configuration
 * parameter is ignored. Effectively, only the Hwi_MaskingOption_LOWER
 * is supported.
 *
 * <h3>Interrupt Priorities</h3>
 *
 * In general, the NVIC supports priority values of 0 thru 255.
 *
 * In practice, the number of priorities and their values are device dependent,
 * and their nesting behaviors depend on the @link Hwi_priGroup @endlink setting.
 *
 * For most TI MCU devices, 8 priorities are supported. A peculiarity of ARM's
 * NVIC is that, although the priority field is an 8 bit value, the range of
 * supported priority values are left-justified within this 8 bit field.
 * Consequently, the 8 priority values are not 0 thru 7 as one might expect, but
 * rather:
 *
 * @code
 *      0x00    // highest priority, non dispatched, Zero Latency priority
 *      0x20    // highest dispatched interrupt priority
 *      0x40
 *      0x60
 *      0x80
 *      0xa0
 *      0xc0
 *      0xe0    // lowest dispatched interrupt priority, (default)
 * @endcode
 *
 * Priority 0 is the highest priority and by default is reserved for zero
 * latency interrupts (see Hwi.disablePriority configuration parameter).
 *
 * See the "Cortex M4 Devices Generic User Guide" for details on the behavior of
 * interrupt priorities and their relationship to the @ref Hwi_priGroup
 * "Hwi.priGroup" setting.
 *
 * <h3>Interrupt Vector Tables</h3>
 * SimpleLink CC13XX/CC26XX devices:
 *
 * By default, two vector tables are created for SimpleLink devices:
 *
 * A 15 entry boot vector table is placed at address 0x00000000 in FLASH.
 *
 * A vector table of length @link Hwi_NUM_INTERRUPTS @endlink is placed at address 0x20000000 in RAM.
 *
 * The FLASH boot vector table contains the reset vector and exception handler
 * vectors used until the RAM based vector table is initialized.
 *
 * The RAM vector table contains those same first 15 vectors plus the
 * SysTick interrupt vector and the remainder of the user interrupt vectors.
 *
 * During system startup, the NVIC Vector Table Offset Registor is intialized to
 * point to the RAM vector table after it has been initialized.
 *
 * <h3>Hook Functions</h3>
 *
 * Sets of hook functions can be specified for the Hwi module using the
 * configuration tool.  Each set contains these hook functions:
 *
 *  <b>Register:</b>
 *  A function called before any statically-created Hwis
 *      are initialized at runtime.  The register hook is called at boot time
 *      before main() and before interrupts are enabled.
 *
 *  <b>Create:</b>
 *  A function that is called when a Hwi is created.
 *      This includes hwis that are created statically and those
 *      created dynamically using Hwi_create.
 *
 *  <b>Begin:</b>
 *  A function that is called just prior to running a Hwi.
 *
 *  <b>End:</b>
 *  A function that is called just after a Hwi finishes.
 *
 *  <b>Delete:</b>
 *  A function that is called when a Hwi is deleted at
 *      run-time with Hwi_delete.
 *
 * <b>Register Function</b>
 *
 * The Register function is provided to allow a hook set to store its hookset
 * ID.  This id can be passed to "Hwi_setHookContext"
 * and "Hwi_getHookContext" to set or get hookset-
 * specific context.  The Register function must be specified if the hook
 * implementation needs to use "Hwi_setHookContext" or
 * "Hwi_getHookContext". The registerFxn hook function
 * is called during system initialization before interrupts have been enabled.
 *
 * @code
 *  void myRegisterFxn(int id);
 * @endcode
 *
 * <b>Create and Delete Functions</b>
 *
 * The create and delete functions are called whenever a Hwi is created or
 * deleted.  They are called with interrupts enabled (unless called at boot time
 * or from main()).
 *
 * @code
 *  void myCreateFxn(Hwi_Handle hwi, Error_Block *eb);
 * @endcode
 *
 * @code
 *  void myDeleteFxn(Hwi_Handle hwi);
 * @endcode
 *
 * <b>Begin and End Functions</b>
 *
 * The beginFxn and endFxn function hooks are called with interrupts globally
 * disabled, therefore any hook processing function will contribute to the
 * overall system interrupt response latency.  In order to minimize this impact,
 * carefully consider the processing time spent in an Hwi beginFxn or endFxn
 * function hook.
 *
 * @code
 *  void myBeginFxn(Hwi_Handle hwi);
 * @endcode
 *
 * @code
 *  void myEndFxn(Hwi_Handle hwi);
 * @endcode
 *
 * <b>Hook functions can only be configured statically.</b>
 *
 * To add a Hwi hook or set of Hwi hooks, the following syntax is used in the
 * app.syscfg file:
 *
 * @code
 * const Hwi = scripting.addModule("/family/arm/m3/Hwi");
 * Hwi.hwiHooks.create(1);
 * Hwi.hwiHooks[0].registerFxn = "myRegisterFxn";
 * Hwi.hwiHooks[0].createFxn   = "myCreateFxn";
 * Hwi.hwiHooks[0].beginFxn    = "myBeginFxn";
 * Hwi.hwiHooks[0].endFxn      = "myEndFxn";
 * Hwi.hwiHooks[0].deleteFxn   = "myDeleteFxn";
 * @endcode
 *
 * Leaving a subset of the hook functions undefined is ok.
 *
 * @htmlonly
 *  <h3> Calling Context </h3>
 *  <table border="1" cellpadding="3">
 *    <colgroup span="1"></colgroup> <colgroup span="5" align="center"></colgroup>
 *
 *    <tr><th> Function                 </th><th>  Hwi   </th><th>  Swi   </th><th>  Task  </th><th>  Main  </th><th>  Startup  </th></tr>
 *    <!--                                                                                                                 -->
 *    <tr><td> Hwi_clearInterrupt   </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Hwi_create           </td><td>   N    </td><td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_disable          </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Hwi_disableInterrupt </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_enable           </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_enableInterrupt  </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_Params_init      </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Hwi_restore          </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Hwi_restoreInterrupt </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td></tr>
 *    <tr><td> Hwi_construct        </td><td>   N    </td><td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_delete           </td><td>   N    </td><td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_destruct         </td><td>   N    </td><td>   N    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_getHookContext   </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_setFunc          </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td> Hwi_setHookContext   </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   Y    </td><td>   N    </td></tr>
 *    <tr><td colspan="6"> Definitions: <br />
 *       <ul>
 *         <li> <b>Hwi</b>: API is callable from a Hwi thread. </li>
 *         <li> <b>Swi</b>: API is callable from a Swi thread. </li>
 *         <li> <b>Task</b>: API is callable from a Task thread. </li>
 *         <li> <b>Main</b>: API is callable during any of these phases: </li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.lastFxns. </li>
 *             <li> During main().</li>
 *             <li> During BIOS.startupFxns.</li>
 *           </ul>
 *         <li> <b>Startup</b>: API is callable during any of these phases:</li>
 *           <ul>
 *             <li> During xdc.runtime.Startup.firstFxns.</li>
 *           </ul>
 *       </ul>
 *    </td></tr>
 *
 *  </table>
 * @endhtmlonly
 */

/*
 * ======== Hwi.h ========
 */

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/runtime/Error.h>

/*
 * First time Hwi.h is included within a compilation unit:
 *  -undefine any previous Hwi_ short names
 *  -do short-to-long name conversions.
 *  -include all short-named Hwi API definitions.
 *
 * All subsequent includes:
 *  -undefine any previous Hwi_ short names
 *  -perform short-to-long name converions
 *  -Do not include short-named Hwi API definitins
 *   because the long name Hwi API definitions are
 *   already known to the compiler.
 */

/*! @cond NODOC */
#define do_hwi_undef_short_names
#define do_hwi_short_to_long_name_conversion
#include "Hwi_defs.h"
/*! @endcond */

#ifndef ti_sysbios_family_arm_m3_Hwi__include
#define ti_sysbios_family_arm_m3_Hwi__include
/*! @cond NODOC */
#include <ti/sysbios/hal/Hwi.h>
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Error and Assert Ids
 */
/*!
 * @brief Error raised Error raised if an attempt is made to create a Hwi
 * with an interrupt number greater than Hwi_NUM_INTERRUPTS - 1.
 */
#define Hwi_E_badIntNum        "intnum: %d is out of range"

/*!
 * @brief Error raised when a Hwi is already defined.
 */
#define Hwi_E_alreadyDefined   "Hwi already defined: intr# %d"

/*!
 * @brief Error raised when the number of interrupts being created
 * exceeds the number supported.
 */
#define Hwi_E_hwiLimitExceeded "Too many interrupts defined"

/*!
 * @brief Error raised when an exception occurs
 */
#define Hwi_E_exception        "hardware exception id = %d, pc = 0x%08x"

/*!
 * @brief Error raised when an uninitialized interrupt occurs
 */
#define Hwi_E_noIsr            "no ISR, id = %d, pc = 0x%08x"

/*!
 * @brief Error raised when NMI exception occurs
 */
#define Hwi_E_NMI              "NMI: %s"

/*!
 * @brief Error raised when hard fault exception occurs
 */
#define Hwi_E_hardFault        "Hard-fault: %s"

/*!
 * @brief Error raised when memory fault exception occurs
 */
#define Hwi_E_memFault         "Mem-fault: %s, addresss: 0x%08x"

/*!
 * @brief Error raised when bus fault exception occurs
 */
#define Hwi_E_busFault         "Bus-fault: %s, address: 0x%08x"

/*!
 * @brief Error raised when usage fault exception occurs
 */
#define Hwi_E_usageFault       "Usage-fault: %s"

/*!
 * @brief Error raised when svCall exception occurs
 */
#define Hwi_E_svCall           "SvCall: svNum = %d"

/*!
 * @brief Error raised when debugMon exception occurs
 */
#define Hwi_E_debugMon         "Debug-monitor: %s"

/*!
 * @brief Error raised when reserved exceptions occur
 */
#define Hwi_E_reserved         "reserved %s %d"

/*!
 * @brief Error raised when the ISR stack has overflowed
 */
#define Hwi_E_stackOverflow    "ISR stack overflow"

typedef struct Hwi_Struct Hwi_Struct;
typedef struct Hwi_Struct Hwi_Object;
typedef Hwi_Struct* Hwi_Handle;
typedef Hwi_Struct* Hwi_Instance;

/*! @cond NODOC */
/* included for backward compatibility with DPL but not required otherwise */
typedef struct Hwi_Struct Hwi_Struct2;
typedef struct Hwi_Struct2__ Hwi_Struct2__;
/*! @endcond */

/*!
 * @brief Hwi hook set type definition.
 *
 * The functions that make up a hookSet have certain restrictions. They cannot
 * call any Hwi instance functions other than Hwi_getHookContext() and
 * Hwi_setHookContext(). For all practical purposes, they should treat the
 * Hwi_Handle passed to these functions as an opaque handle.
 */
typedef struct Hwi_HookSet Hwi_HookSet;
typedef struct Hwi_Module_State Hwi_Module_State;
typedef struct Hwi_Params Hwi_Params;

/*!
 * @brief Nested Vectored Interrupt Controller.
 */
typedef struct Hwi_NVIC Hwi_NVIC;
/*!
 * @brief Exception Context - Register contents at the time of an exception.
 */
typedef struct Hwi_ExcContext Hwi_ExcContext;
/*!
 * @brief Structure contains Hwi stack usage info
 *
 * Used by getStackInfo() and viewGetStackInfo() functions
 */
typedef struct Hwi_StackInfo Hwi_StackInfo;

/*!
 * @brief Hwi create function type definition.
 */
typedef void (*Hwi_FuncPtr)(uintptr_t);

/*!
 * @brief Hwi vector function type definition.
 */
typedef void (*Hwi_VectorFuncPtr)(void );

/*!
 * @brief Exception hook function type definition.
 */
typedef void (*Hwi_ExceptionHookFuncPtr)(Hwi_ExcContext* arg1);

/* ExcHookFunc */
typedef Hwi_ExceptionHookFuncPtr Hwi_ExcHookFunc;

/*!
 * @brief Hwi hook set type definition.
 *
 * The functions that make up a hookSet have certain restrictions. They cannot
 * call any Hwi instance functions other than Hwi_getHookContext() and
 * Hwi_setHookContext(). For all practical purposes, they should treat the
 * Hwi_Handle passed to these functions as an opaque handle.
 */
struct Hwi_HookSet {
    void (*registerFxn)(int arg1);
    void (*createFxn)(Hwi_Handle arg1, Error_Block* arg2);
    void (*beginFxn)(Hwi_Handle arg1);
    void (*endFxn)(Hwi_Handle arg1);
    void (*deleteFxn)(Hwi_Handle arg1);
};

typedef struct { int length; const Hwi_HookSet *elem; } Hwi_Hook;

/*!
 * @brief Interrupt masking options
 *
 */
enum Hwi_MaskingOption {
    /*! @brief No interrupts are disabled <b>(not support on cortexM devices)</b> */
    Hwi_MaskingOption_NONE,
    /*! @brief All interrupts are disabled <b>(not support on cortexM devices)</b> */
    Hwi_MaskingOption_ALL,
    /*! @brief Only this interrupt is disabled <b>(not support on cortexM devices)</b> */
    Hwi_MaskingOption_SELF,
    /*! @brief User supplies interrupt enable masks <b>(not support on cortexM devices)</b> */
    Hwi_MaskingOption_BITMASK,
    /*! @brief All current and lower priority interrupts are disabled. <b>(default for all cortexM devices)</b> */
    Hwi_MaskingOption_LOWER
};

/*!
 * @brief Shorthand interrupt masking options
 */
typedef enum Hwi_MaskingOption Hwi_MaskingOption;

/*!
 * @brief Structure contains Hwi stack usage info
 *
 * Used by getStackInfo() and viewGetStackInfo() functions
 */
struct Hwi_StackInfo {
    size_t hwiStackPeak;
    size_t hwiStackSize;
    void * hwiStackBase;
};

/*! @cond NODOC */
/*!
 * @brief NVIC Configuration Control Register (CCR).
 */
struct Hwi_CCR {
    /*!
     * @brief Auto stack alignment in exception
     */
    uint8_t STKALIGN;
    uint8_t BFHFNMIGN;
    /*!
     * @brief Trap on divide by zero
     */
    uint8_t DIV_0_TRP;
    uint8_t UNALIGN_TRP;
    /*!
     * @brief Allow user to trigger interrupts
     */
    uint8_t USERSETMPEND;
    uint8_t NONEBASETHRDENA;
};
/*! @endcond */
/*!
 * @brief Nested Vectored Interrupt Controller.
 */
struct Hwi_NVIC {
    uint32_t RES_00;       /*! 0xE000E000 reserved */
    uint32_t ICTR;         /*! 0xE000E004 Interrupt Control Type */
    uint32_t RES_08;       /*! 0xE000E008 reserved */
    uint32_t RES_0C;       /*! 0xE000E00C reserved */
    uint32_t STCSR;        /*! 0xE000E010 SysTick Control & Status Register */
    uint32_t STRVR;        /*! 0xE000E014 SysTick Reload Value Register */
    uint32_t STCVR;        /*! 0xE000E018 SysTick Current Value Register */
    uint32_t STCALIB;      /*! 0xE000E01C SysTick Calibration Value Register */
    uint32_t RES_20 [56];  /*! 0xE000E020-0xE000E0FC reserved */
    uint32_t ISER [8];     /*! 0xE000E100-0xE000E11C Interrupt Set Enable Registers */
    uint32_t RES_120 [24]; /*! 0xE000E120-0xE000E17C reserved */
    uint32_t ICER [8];     /*! 0xE000E180-0xE000E19C Interrupt Clear Enable Registers */
    uint32_t RES_1A0 [24]; /*! 0xE000E1A0-0xE000E1FC reserved */
    uint32_t ISPR [8];     /*! 0xE000E200-0xE000E21C Interrupt Set Pending Registers */
    uint32_t RES_220 [24]; /*! 0xE000E220-0xE000E7C reserved */
    uint32_t ICPR [8];     /*! 0xE000E280-0xE000E29C Interrupt Clear Pending Registers */
    uint32_t RES_2A0 [24]; /*! 0xE000E2A0-0xE000E2FC reserved */
    uint32_t IABR [8];     /*! 0xE000E300-0xE000E31C Interrupt Active Bit Registers */
    uint32_t RES_320 [56]; /*! 0xE000E320-0xE000E3FC reserved */
    uint8_t  IPR [240];    /*! 0xE000E400-0xE000E4EF Interrupt Priority Registers */
    uint32_t RES_4F0 [516];/*! 0xE000E4F0-0xE000ECFC reserved */
    uint32_t CPUIDBR;      /*! 0xE000ED00 CPUID Base Register */
    uint32_t ICSR;         /*! 0xE000ED04 Interrupt Control State Register */
    uint32_t VTOR;         /*! 0xE000ED08 Vector Table Offset Register */
    uint32_t AIRCR;        /*! 0xE000ED0C Application Interrupt/Reset Control Register */
    uint32_t SCR;          /*! 0xE000ED10 System Control Register */
    uint32_t CCR;          /*! 0xE000ED14 Configuration Control Register */
    uint8_t  SHPR[12];     /*! 0xE000ED18 System Handlers 4-15 Priority Registers */
    uint32_t SHCSR;        /*! 0xE000ED24 System Handler Control & State Register */
    uint8_t  MMFSR;        /*! 0xE000ED28 Memory Manage Fault Status Register */
    uint8_t  BFSR;         /*! 0xE000ED29 Bus Fault Status Register */
    uint16_t UFSR;         /*! 0xE000ED2A Usage Fault Status Register */
    uint32_t HFSR;         /*! 0xE000ED2C Hard Fault Status Register */
    uint32_t DFSR;         /*! 0xE000ED30 Debug Fault Status Register */
    uint32_t MMAR;         /*! 0xE000ED34 Memory Manager Address Register */
    uint32_t BFAR;         /*! 0xE000ED38 Bus Fault Address Register */
    uint32_t AFSR;         /*! 0xE000ED3C Auxiliary Fault Status Register */
    uint32_t PFR0;         /*! 0xE000ED40 Processor Feature Register */
    uint32_t PFR1;         /*! 0xE000ED44 Processor Feature Register */
    uint32_t DFR0;         /*! 0xE000ED48 Debug Feature Register */
    uint32_t AFR0;         /*! 0xE000ED4C Auxiliary Feature Register */
    uint32_t MMFR0;        /*! 0xE000ED50 Memory Model Fault Register0 */
    uint32_t MMFR1;        /*! 0xE000ED54 Memory Model Fault Register1 */
    uint32_t MMFR2;        /*! 0xE000ED58 Memory Model Fault Register2 */
    uint32_t MMFR3;        /*! 0xE000ED5C Memory Model Fault Register3 */
    uint32_t ISAR0;        /*! 0xE000ED60 ISA Feature Register0 */
    uint32_t ISAR1;        /*! 0xE000ED64 ISA Feature Register1 */
    uint32_t ISAR2;        /*! 0xE000ED68 ISA Feature Register2 */
    uint32_t ISAR3;        /*! 0xE000ED6C ISA Feature Register3 */
    uint32_t ISAR4;        /*! 0xE000ED70 ISA Feature Register4 */
    uint32_t RES_D74[5];   /*! 0xE000ED74-0xE000ED84 reserved */
    uint32_t CPACR;        /*! 0xE000ED88 Coprocessor Access Control Register */
    uint32_t RES_D8C[93];  /*! 0xE000ED8C-0xE000EEFC reserved */
    uint32_t STI;          /*! 0xE000EF00 Software Trigger Interrupt Register */
    uint32_t RES_F04[12];  /*! 0xE000EF04-0xE000EF30 reserved */
    uint32_t FPCCR;        /*! 0xE000EF34 FP Context Control Register */
    uint32_t FPCAR;        /*! 0xE000EF38 FP Context Address Register */
    uint32_t FPDSCR;       /*! 0xE000EF3C FP Default Status Control Register */
    uint32_t MVFR0;        /*! 0xE000EF40 Media & FP Feature Register0 */
    uint32_t MVFR1;        /*! 0xE000EF44 Media & FP Feature Register1 */
    uint32_t RES_F48[34];  /*! 0xE000EF48-0xE000EFCC reserved */
    uint32_t PID4;         /*! 0xE000EFD0 Peripheral ID Register4 */
    uint32_t PID5;         /*! 0xE000EFD4 Peripheral ID Register5 */
    uint32_t PID6;         /*! 0xE000EFD8 Peripheral ID Register6 */
    uint32_t PID7;         /*! 0xE000EFDC Peripheral ID Register7 */
    uint32_t PID0;         /*! 0xE000EFE0 Peripheral ID Register0 */
    uint32_t PID1;         /*! 0xE000EFE4 Peripheral ID Register1 */
    uint32_t PID2;         /*! 0xE000EFE8 Peripheral ID Register2 */
    uint32_t PID3;         /*! 0xE000EFEC Peripheral ID Register3 */
    uint32_t CID0;         /*! 0xE000EFF0 Component ID Register0 */
    uint32_t CID1;         /*! 0xE000EFF4 Component ID Register1 */
    uint32_t CID2;         /*! 0xE000EFF8 Component ID Register2 */
    uint32_t CID3;         /*! 0xE000EFFC Component ID Register3 */
};

/*!
 * @brief Physical Nested Vectored Interrupt Controller Device. short name is
 * "Hwi_nvic" long name is "ti_sysbios_family_arm_m3_Hwi_nvic"
 */
extern volatile Hwi_NVIC Hwi_nvic;

/*!
 * @brief Exception Context - Register contents at the time of an exception.
 */
struct Hwi_ExcContext {
    BIOS_ThreadType threadType;
    void * threadHandle;
    void * threadStack;
    size_t threadStackSize;
    void * r0;
    void * r1;
    void * r2;
    void * r3;
    void * r4;
    void * r5;
    void * r6;
    void * r7;
    void * r8;
    void * r9;
    void * r10;
    void * r11;
    void * r12;
    void * sp;
    void * lr;
    void * pc;
    void * psr;
    void * ICSR;
    void * MMFSR;
    void * BFSR;
    void * UFSR;
    void * HFSR;
    void * DFSR;
    void * MMAR;
    void * BFAR;
    void * AFSR;
};

/* Module_State */
struct Hwi_Module_State {
    char *taskSP;           /* 0x0 */
    bool excActive;         /* 0x4 */
    void * excContext;         /* 0x8 */
    void * excStack;           /* 0xc */
    void * isrStack;           /* 0x10 */
    void * isrStackBase;       /* 0x14 */
    unsigned int swiTaskKeys;       /* 0x18 */
    void * isrStackSize;      
    void * vectorTableBase;
    void * dispatchTable;
    volatile bool vnvicFlushRequired;
    uint8_t *intAffinity;
    uint32_t *intAffinityMasks;
    Hwi_VectorFuncPtr *vectorTable;
    bool initDone;
};

/* Object */
struct Hwi_Struct {
    /*!
     * @brief ISR function argument. Default is 0.
     */
    uintptr_t arg;
    Hwi_FuncPtr fxn;
    uintptr_t irp;
    /*!
     * @brief Interrupt priority.
     *
     * The default value of -1 is used as a flag to indicate the lowest
     * (logical) device-specific priority value.
     *
     * Not all targets/devices support this instance parameter. On those that
     * don't, this parameter is ignored.
     */
    uint8_t priority;
    int16_t intNum;
    void * *hookEnv;
};

/*!
 * @brief Hwi exception handler function type definition.
 */
typedef void (*Hwi_ExcHandlerFuncPtr)(unsigned int* arg1, unsigned int arg2);

/*! @cond NODOC */
/*!
 * @brief Include interrupt nesting logic in interrupt dispatcher?
 *
 * Default is true.
 *
 * This option provides the user with the ability to optimize interrupt
 * dispatcher performance when support for interrupt nesting is not required.
 *
 * Setting this parameter to false will disable the logic in the interrupt
 * dispatcher that manipulates interrupt mask registers and enables and disables
 * interrupts before and after invoking the user's Hwi function.
 *
 * Set this parameter to false if you don't need interrupts enabled during the
 * execution of your Hwi functions.
 */
extern const bool Hwi_dispatcherAutoNestingSupport;

/*!
 * @brief Include Swi scheduling logic in interrupt dispatcher?
 *
 * Default is inherited from @ref ti.sysbios.BIOS#swiEnabled "BIOS.swiEnabled",
 * which is true by default.
 *
 * This option provides the user with the ability to optimize interrupt
 * dispatcher performance when it is known that Swis will not be posted from any
 * of their Hwi threads.
 *
 * @warning
 * Setting this parameter to false will disable the logic in the interrupt
 * dispatcher that invokes the Swi scheduler prior to returning from an
 * interrupt. With this setting, Swis MUST NOT be posted from Hwi functions!
 */
extern const bool Hwi_dispatcherSwiSupport;

/*!
 * @brief Include Task scheduling logic in interrupt dispatcher?
 *
 * Default is inherited from @ref ti.sysbios.BIOS#taskEnabled
 * "BIOS.taskEnabled", which is true by default.
 *
 * This option provides the user with the ability to optimize interrupt
 * dispatcher performance when it is known that no Task scheduling APIs (ie @ref
 * ti.sysbios.knl.Semaphore#post "Semaphore_post()") will be executed from any
 * of their Hwi threads.
 *
 * Setting this parameter to false will disable the logic in the interrupt
 * dispatcher that invokes the Task scheduler prior to returning from an
 * interrupt.
 */
extern const bool Hwi_dispatcherTaskSupport;

/*!
 * @brief Controls whether the dispatcher retains the interrupted thread's
 * return address.
 *
 * This option is enabled by default.
 *
 * Setting this parameter to false will disable the logic in the interrupt
 * dispatcher that keeps track of the interrupt's return address and provide a
 * small savings in interrupt latency.
 *
 * The application can get an interrupt's most recent return address using the
 * Hwi_getIrp API.
 */
extern const bool Hwi_dispatcherIrpTrackingSupport;
/*! @endcond */

/*!
 * @brief The Cortex M3/M4 NVIC supports up to 256 interrupts/exceptions.
 *
 * The actual number supported is device specific and provided by the catalog
 * device specification.
 */
extern const int Hwi_NUM_INTERRUPTS;

/*!
 * @brief The Cortex M3/M4 NVIC supports up to 256 interrupt priorities.
 *
 * The actual number supported is device specific and provided by the catalog
 * device specification. For all TI SimpleLink devices, 8 priorities are
 * supported.
 */
extern const int Hwi_NUM_PRIORITIES;

/*! @cond NODOC */
/*!
 * @brief const array to hold all HookSet objects.
 */
extern const Hwi_Hook Hwi_hooks;

/*!
 * @brief Uninitialized ISR Handler. Default is set to an internal exception
 * handler
 */
extern const Hwi_VectorFuncPtr Hwi_nullIsrFunc;

/*!
 * @brief Exception handler function pointer.
 *
 * The default is determined by the value of Hwi.enableException.
 *
 * If the user does NOT set this parameter, then the following default behavior
 * is followed:
 *
 * If Hwi.enableException is true, then the internal 'Hwi_excHandlerMax'
 * function is used. This exception handler saves the exception context then
 * does a complete exception decode and dump to the console, then raises an
 * Error. The exception context can be viewed within CCS in the ROV Hwi module's
 * Exception view.
 *
 * If Hwi.enableException is false, then the internal 'Hwi_excHandlerMin'
 * function is used. This exception handler saves the exception context then
 * raises an Error. The exception context can be viewed within CCS in the ROV
 * Hwi module's Exception view.
 *
 * If the user sets this parameter to their own function, then the user's
 * function will be invoked with the following arguments:
 *
 * void myExceptionHandler(unsigned int *excStack, unsigned int lr);
 *
 * Where 'excStack' is the address of the stack containing the register context
 * at the time of the exception, and 'lr' is the link register value when the
 * low-level-assembly-coded exception handler was vectored to.
 *
 * If this parameter is set to 'null', then an infinite while loop is entered
 * when an exception occurs. This setting minimizes code and data footprint but
 * provides no automatic exception decoding.
 */
extern const Hwi_ExcHandlerFuncPtr Hwi_excHandlerFunc;
/*! @endcond */

/*!
 * @brief User Exception hook function.
 *
 * Called just after the exception context has been initialized.
 *
 * This function will be run on the ISR stack.
 *
 * This function must run to completion.
 *
 * It is called without any Task or Swi scheduling protection and therefore can
 * not call any functions that may cause a Swi or Task scheduling operation
 * (Swi_post(), Semaphore_post(), Event_post(), etc).
 */
extern const Hwi_ExcHookFunc Hwi_excHookFunc;

/*!
 * @brief The priority that BASEPRI is set to by Hwi_disable().
 *
 * All interrupts configured with equal or less priority (equal or higher
 * number) than disablePriority are disabled by Hwi_disable.
 * Interrupts configured with higher priority (smaller number) than
 * Hwi.disablePriority are non-maskable (ie zero-latency).
 *
 * The default setting is the second highest interrupt priority defined for the
 * device (typically '0x20' for devices which support 8 priority values). This
 * results in priority 0 (and all other values in the same priority group, ie
 * 0x00 thru 0x1f) being the zero-latency, non-maskable interrupt priority. All
 * other priorities are disabled with Hwi_disable().
 *
 * The constant Hwi_disablePriority is configured within the app.syscfg file
 * using the following syntax:
 *
 * @code
 * const Hwi = scripting.addModule("/family/arm/m3/Hwi");
 * Hwi.disablePriority = 0x20;
 * @endcode
 */
extern const unsigned int Hwi_disablePriority;

/*!
 * @brief The PRIGROUP setting. Default is 0.
 *
 * This value will be written to the PRIGROUP field within the NVIC's
 * Application Interrupt and Reset Control Register (Hwi_nvic.AIRCR). It defines
 * how the 8 bit priority values are interpreted by the hardware.
 *
 * Valid settings are 0-7.
 *
 * The default setting of 0 causes bits 7-1 of an interrupt's priority value to
 * be used as pre-emption priority, while bit 0 is used to determine which of
 * two simultaneous interrupts with the same pre-emption priority will be
 * serviced first.
 *
 * For most TI MCU devices, this means that each of the 8 supported priority
 * values are unique pre-emption priorities and are not subdivided into priority
 * groups.
 *
 * See the "AIRCR" register description in the Arm "Cortex M4 Devices Generic
 * User Guide" for more details regarding priority groups.
 *
 * The constant Hwi_priGroup is configured within the app.syscfg file
 * using the following syntax:
 *
 * @code
 * const Hwi = scripting.addModule("/family/arm/m3/Hwi");
 * Hwi.priGroup = 0;
 * @endcode
 */
extern const unsigned int Hwi_priGroup;

/*!
 * @brief If Hwi.dispatchTableSize is initialized by the user then
 * Hwi.numSparseInterrupts is set to the value of Hwi.dispatchTableSize
 *
 * If Hwi.dispatchTableSize is NOT set by the user, the normal intNum-indexed
 * Hwi dispatchTable mechanism is used by the dispatcher to find the
 * corresponding Hwi object.
 *
 * If Hwi.dispatchTableSize is set by the user, then a RAM-based fixed sized
 * interrupt jump table is generated that contains a repeating pattern of the
 * following 3 word assembly code snippets:
 *
 *   hwiX:        ldr r3, hwiObjectX
 *                ldr pc, ti_sysbios_family_arm_m3_Hwi_dispatch__I
 *   hwiObjectX: .word 0
 *   hwiY:        ldr r3, hwiObjectY
 *                ldr pc, ti_sysbios_family_arm_m3_Hwi_dispatch__I
 *   hwiObjectY: .word 0
 *
 * Each dispatched interrupt vector is then initialized to point to one of these
 * tuples, and the address of the corresponding Hwi object is written into the
 * hwiObjectX field.
 *
 * The low level assembly code in Hwi_dispatch__I preserves the value of r3 when
 * it calls Hwi_dispatchC(), which results in the Hwi object being passed as the
 * arg3.
 *
 * Depending on the boolean value of Hwi_numSparseInterrupts, the dispatcher
 * either uses the value passed in arg3 as the Hwi object, or uses intNum to
 * index into the standard dispatchTable to fetch the Hwi object.
 */
extern const unsigned int Hwi_numSparseInterrupts;

/*! @cond NODOC */
/*!
 * @brief The initial Hwi_nvic.CCR value.
 *
 * This configuration parameter is 
 */
extern const uint32_t Hwi_ccr;
/*! @endcond */

struct Hwi_Params {
    /*!
     * @brief maskSetting. Default is Hwi_MaskingOption
     * "Hwi_MaskingOption_SELF"
     */
    Hwi_MaskingOption maskSetting;
    /*!
     * @brief ISR function argument. Default is 0.
     */
    uintptr_t arg;
    /*!
     * @brief Enable this interrupt when object is created? Default is true.
     */
    bool enableInt;
    /*!
     * @brief Interrupt event ID (Interrupt Selection Number)
     *
     * Default is -1. Not all targets/devices support this instance parameter.
     * On those that don't, this parameter is ignored.
     */
    int eventId;
    /*!
     * @brief Interrupt priority.
     *
     * The default value of -1 is used as a flag to indicate the lowest
     * (logical) device-specific priority value.
     *
     * Not all targets/devices support this instance parameter. On those that
     * don't, this parameter is ignored.
     */
    int priority;
    /*!
     * @brief Use the interrupt dispatcher with this interrupt. Default is true.
     *
     * If set to false, the interrupt dispatcher is NOT used. Instead, the
     * configured Hwi function address is placed directly in the vector table,
     * which results in the dispatcher being bypassed.
     *
     * @warning
     * Interrupts configured to bupass the dispatcher are not allowed to call
     * ANY SYS/BIOS APIs that effect thread scheduling. Examples of API that
     * should no be invoked are:
     *
     * - Swi_post(), - Semaphore_post(), - Event_post(), - Task_yield()
     *
     * Additionally, although the signature for a non-dispatched interrupt
     * function is the same as that for a dispatched interrupt (see
     * Hwi_FuncPtr), no argument is actually passed to the non-
     * dispatched ISR handler.
     */
    bool useDispatcher;
};

/*!
 * @brief Globally disable interrupts.
 *
 * Hwi_disable globally disables hardware interrupts and returns an opaque key
 * indicating whether interrupts were globally enabled or disabled on entry to
 * Hwi_disable(). The actual value of the key is target/device specific and is
 * meant to be passed to Hwi_restore().
 *
 * Call Hwi_disable before a portion of a function that needs to run without
 * interruption. When critical processing is complete, call Hwi_restore or
 * Hwi_enable to reenable hardware interrupts.
 *
 * Servicing of interrupts that occur while interrupts are disabled is postponed
 * until interrupts are reenabled. However, if the same type of interrupt occurs
 * several times while interrupts are disabled, the interrupt's function is
 * executed only once when interrupts are reenabled.
 *
 * A context switch can occur when calling Hwi_enable or Hwi_restore if an
 * enabled interrupt occurred while interrupts are disabled.
 *
 * Hwi_disable may be called from main(). However, since Hwi interrupts are
 * already disabled in main(), such a call has no effect.
 *
 * @warning
 * The cortexM implementation of Hwi_disable() sets the basepri register to the
 * value of @link Hwi_disablePriority @endlink.
 *
 * @warning
 * If a Task switching API such as Semaphore_pend(),
 * Semaphore_post(), Task_sleep(), Task_yield(), is invoked which results in a
 * context switch while interrupts are disabled, an embedded call to
 * Hwi_enable() occurs on the way to the new thread context which
 * unconditionally re-enables interrupts. Interrupts will remain enabled until a
 * subsequent Hwi_disable() invocation.
 * 
 * @retval opaque uint32_t key for use by Hwi_restore()
 */
extern unsigned int Hwi_Disable();

/*!
 * @brief Globally enable interrupts
 *
 * <b>Hwi_enable()</b>
 *
 * Hwi_enable globally enables hardware interrupts and returns an
 * opaque key indicating whether interrupts were globally enabled or
 * disabled on entry to Hwi_enable().
 * The actual value of the key is target/device specific and is meant
 * to be passed to Hwi_restore().
 *
 * This function is called within BIOS_start() to enable interrupts after
 * "main()" has been executed. Prior to BIOS_start(), interrupts are globally
 * disabled.
 *
 * Hardware interrupts are enabled unless a call to Hwi_disable() disables
 * them.
 *
 * Servicing of interrupts that occur while interrupts are disabled is
 * postponed until interrupts are reenabled. However, if the same type
 * of interrupt occurs several times while interrupts are disabled,
 * the interrupt's function is executed only once when interrupts are
 * reenabled.
 *
 * A context switch can occur when calling Hwi_enable or Hwi_restore if
 * an enabled interrupt occurred while interrupts are disabled.
 *
 * Any call to Hwi_enable enables interrupts, even if Hwi_disable has
 * been called several times.
 *
 * Hwi_enable must not be called from within "main()".
 */
extern unsigned int Hwi_Enable();

/*!
 * @brief restore global interrupts to the state they were in prior Hwi_disable().
 *
 * <b>Hwi_restore(key)</b>
 *
 * Globally restore interrupts.
 *
 * Hwi_restore() globally restores interrupts to the state determined
 * by the key argument provided by a previous invocation of Hwi_disable().
 *
 * A context switch may occur when calling Hwi_restore if Hwi_restore()
 * reenables interrupts and another Hwi occurred while interrupts were
 * disabled.
 *
 * The Hwi_disable() / Hwi_restore() pair may be called from within main().
 * However, since interrupts are globally disabled prior to main(),
 * the call to Hwi_restore() has no effect.
 *
 * @arg key = enable/disable state to restore
 *
 */
 
extern void Hwi_Restore(uint32_t key);
 
 /*! @cond NODOC */
extern void Hwi_init(void);

/* Instance_init */
extern int Hwi_Instance_init(Hwi_Object *obj, int intNum, Hwi_FuncPtr hwiFxn, const Hwi_Params *prms, Error_Block *eb);

/* Instance_finalize */
extern void Hwi_Instance_finalize(Hwi_Object *obj, int ec);
/*! @endcond */

extern Hwi_Handle Hwi_create(int intNum, Hwi_FuncPtr hwiFxn, const Hwi_Params *prms, Error_Block *eb);

extern Hwi_Handle Hwi_construct(Hwi_Struct *obj, int intNum, Hwi_FuncPtr hwiFxn, const Hwi_Params *prms, Error_Block *eb);

extern void Hwi_delete(Hwi_Handle *handle);

extern void Hwi_destruct(Hwi_Struct *obj);

/*!
 * @brief Get Hwi stack usage Info.
 *
 * getStackInfo returns the Hwi stack usage info to its calling function by
 * filling stack base address, stack size and stack peak fields in the
 * Hwi_StackInfo structure.
 *
 * getStackInfo accepts two arguments, a pointer to a structure of type
 * Hwi_StackInfo and a boolean. If the boolean is set to true, the
 * function computes the stack depth and fills the stack peak field in the
 * StackInfo structure. If a stack overflow is detected, the stack depth is not
 * computed. If the boolean is set to false, the function only checks for a
 * stack overflow.
 *
 * The isr stack is always checked for an overflow and a boolean is returned to
 * indicate whether an overflow occured.
 *
 * Below is an example of calling getStackInfo() API:
 *
 * @code
 *  #include <ti/sysbios/BIOS.h>
 *  #include <ti/sysbios/family/arm/m3/Hwi.h>
 *  #include <ti/sysbios/knl/Swi.h>
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  Swi_Handle swi0;
 *  volatile bool swiStackOverflow = false;
 *
 *  void swi0Fxn(uintptr_t arg1, uintptr_t arg2)
 *  {
 *      Hwi_StackInfo stkInfo;
 *
 *      // Request stack depth
 *      swiStackOverflow = Hwi_getStackInfo(&stkInfo, true);
 *
 *      // Alternately, we can omit the request for stack depth and
 *      // request only the stack base and stack size (the check for
 *      // stack overflow is always performed):
 *      //
 *      swiStackOverflow = Hwi_getStackInfo(&stkInfo, false);
 *
 *      if (swiStackOverflow) {
 *          // isr Stack Overflow detected
 *      }
 *  }
 *
 *  void idleTask()
 *  {
 *      Swi_post(swi0);
 *  }
 *
 *  int main(int argc, char* argv[])
 *  {
 *      swi0 = Swi_create(swi0Fxn, NULL, NULL);
 *
 *      BIOS_start();
 *      return (0);
 *  }
 * @endcode
 *
 * @param stkInfo pointer to stack info structure
 * @param computeStackDepth decides whether to compute stack depth
 * 
 * @retval boolean to indicate a stack overflow
 */
extern bool Hwi_getStackInfo(Hwi_StackInfo *stkInfo, bool computeStackDepth);

/*! @cond NODOC */
/*!
 * @brief Get Hwi stack usage Info for the specified coreId.
 *
 * getCoreStackInfo returns the Hwi stack usage info for the specified coreId to
 * its calling function by filling stack base address, stack size and stack peak
 * fields in the Hwi_StackInfo structure.
 *
 * This function should be used only in applications built with
 * BIOS.smpEnabled set to true.
 *
 * getCoreStackInfo accepts three arguments, a pointer to a structure of type
 * Hwi_StackInfo, a boolean and a coreId. If the boolean is set
 * to true, the function computes the stack depth and fills the stack peak field
 * in the StackInfo structure. If a stack overflow is detected, the stack depth
 * is not computed. If the boolean is set to false, the function only checks for
 * a stack overflow.
 *
 * The isr stack is always checked for an overflow and a boolean is returned to
 * indicate whether an overflow occured.
 *
 * Below is an example of calling getCoreStackInfo() API:
 *
 * @code
 *  #include <ti/sysbios/BIOS.h>
 *  #include <ti/sysbios/family/arm/m3/Hwi.h>
 *  #include <ti/sysbios/hal/Core.h>
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  ...
 *
 *  void idleTask()
 *  {
 *      unsigned int idx;
 *      Hwi_StackInfo stkInfo;
 *      bool stackOverflow = false;
 *
 *      // Request stack depth for each core's Hwi stack and check for
 *      // overflow
 *      for (idx = 0; idx < Core_numCores; idx++) {
 *          stackOverflow = Hwi_getCoreStackInfo(&stkInfo, true, idx);
 *
 *          // Alternately, we can omit the request for stack depth and
 *          // request only the stack base and stack size (the check for
 *          // stack overflow is always performed):
 *          //
 *          stackOverflow = Hwi_getCoreStackInfo(&stkInfo, false, idx);
 *
 *          if (stackOverflow) {
 *              // isr Stack Overflow detected
 *          }
 *      }
 *  }
 *
 *  int main(int argc, char* argv[])
 *  {
 *      ...
 *      BIOS_start();
 *      return (0);
 *  }
 * @endcode
 *
 * @param stkInfo pointer to stack info structure
 *
 * @param computeStackDepth decides whether to compute stack depth
 *
 * @param coreId core whose stack info needs to be retrieved
 * 
 *@retval boolean to indicate a stack overflow
 */
extern bool Hwi_getCoreStackInfo(Hwi_StackInfo *stkInfo, bool computeStackDepth, unsigned int coreId);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Initially enable interrupts
 *
 * Called within BIOS_start
 */
extern void Hwi_startup(void);

/*!
 * @brief Indicate that we are leaving the boot stack and are about to switch to
 * a task stack. Used by Task_startup()
 */
extern void Hwi_switchFromBootStack(void);
/*! @endcond */

/*!
 * @brief Generate an interrupt for test purposes.
 *
 * @param intNum ID of interrupt to generate
 */
extern void Hwi_post(unsigned int intNum);

/*! @cond NODOC */
/*!
 * @brief retrieve interrupted task's SP
 *
 * Used for benchmarking the SYS/BIOS Hwi dispatcher's task stack utilization.
 * 
 * @retval interrupted task's SP
 */
extern char *Hwi_getTaskSP(void);
/*! @endcond */

/*!
 * @brief Disable a specific interrupt.
 *
 * Disable a specific interrupt identified by an interrupt number.
 *
 * @param intNum interrupt number to disable
 * 
 * @retval key to restore previous enable/disable state
 */
extern unsigned int Hwi_disableInterrupt(unsigned int intNum);

/*!
 * @brief Enable a specific interrupt.
 *
 * Enables a specific interrupt identified by an interrupt number.
 *
 * @param intNum interrupt number to enable
 * 
 * @retval key to restore previous enable/disable state
 */
extern unsigned int Hwi_enableInterrupt(unsigned int intNum);

/*!
 * @brief Restore a specific interrupt's enabled/disabled state.
 *
 * Restores a specific interrupt identified by an interrupt number.
 * restoreInterrupt is generally used to restore an interrupt to its state
 * before Hwi_disableInterrupt or Hwi_enableInterrupt was invoked.
 *
 * @param intNum interrupt number to restore
 * @param key key returned from enableInt or disableInt
 */
extern void Hwi_restoreInterrupt(unsigned int intNum, unsigned int key);

/*!
 * @brief Clear a specific interrupt.
 *
 * Clears a specific interrupt's pending status. The implementation is family-
 * specific.
 *
 * @param intNum interrupt number to clear
 */
extern void Hwi_clearInterrupt(unsigned int intNum);

/*!
 * @brief Get Hwi function and arg
 *
 * @param hwi Hwi handle
 * @param arg pointer for returning hwi's ISR function argument
 *
 * @retval hwi's ISR function
 */
extern Hwi_FuncPtr Hwi_getFunc(Hwi_Handle hwi, uintptr_t *arg);

/*!
 * @brief Overwrite Hwi function and arg
 *
 * Replaces a Hwi object's hwiFxn function originally provided in
 * Hwi_create.
 *
 * @pre
 * Hwi_setFunc() is not thread safe. This means that the new value for for 'fxn'
 * may be temporarily paired with the previous value for 'arg' if pre-emption
 * occurs within the execution of Hwi_setFunc().
 *
 * To guard against this condition, surround the Hwi_setFunc() call with calls
 * to Hwi_disable() and Hwi_restore():
 *
 * @code
 *  key = Hwi_disable();
 *
 *  Hwi_setFunc(newFunc, newArg);
 *
 *  Hwi_restore(key);
 * @endcode
 *
 * @param hwi Hwi handle
 * @param fxn pointer to ISR function
 * @param arg argument to ISR function
 */
extern void Hwi_setFunc(Hwi_Handle hwi, Hwi_FuncPtr fxn, uintptr_t arg);

/*!
 * @brief Get hook instance's context for a Hwi.
 * 
 * @param hwi Hwi handle
 * @param id hook id
 *
 * @retval hook instance's context for hwi
 */
extern void * Hwi_getHookContext(Hwi_Handle hwi, int id);

/*!
 * @brief Set hook instance's context for a Hwi.
 *
 * @param hwi Hwi handle
 * @param id hook id
 * @param hookContext value to write to context
 */
extern void Hwi_setHookContext(Hwi_Handle hwi, int id, void * hookContext);

/*!
 * @brief Get address of interrupted instruction.
 * 
 * @param hwi Hwi handle
 *
 * @retval most current IRP of a Hwi
 */
extern uintptr_t Hwi_getIrp(Hwi_Handle hwi);

/*!
 * @brief Plug a non dispatched interrupt vector with an ISR address.
 *
 * Used internally by Hwi_create() and Hwi_construct().
 *
 * This API is provided for external use primarily to allow users to plug the
 * NMI vector (interrupt #2) at runtime.
 *
 * @note
 * Interrupt vectors plugged using Hwi_plug() are NOT managed by the Hwi
 * interrupt dispatcher. Consequently, it is not safe to call SYS/BIOS APIs from
 * within these ISRs.
 *
 * @param intNum interrupt number
 * @param fxn pointer to ISR function
 */
extern void Hwi_plug(unsigned int intNum, void *fxn);

/*!
 * @brief Returns Hwi_handle associated with intNum
 *
 * @param intNum interrupt number
 *
 * @retval Hwi handle
 */
extern Hwi_Handle Hwi_getHandle(unsigned int intNum);

/*!
 * @brief Set an interrupt's relative priority.
 *
 * Valid priorities are 0 - 255. 0 is highest priority.
 *
 * @warning
 * Setting the priority of a dispatched Hwi to a value higher than
 * Hwi_disablePriority will make it become non-maskable by
 * Hwi_disable. The behavior of your application after that
 * will be unpredictable and will likely yield catastrophic results!
 *
 * @param intNum ID of interrupt
 * @param priority priority
 */
extern void Hwi_setPriority(unsigned int intNum, unsigned int priority);

/*!
 * @brief Set the exception context and stack buffer pointers
 *
 * @param excContextBuffer Address to place ExcContext
 * @param excStackBuffer Address to place ExcStack
 */
extern void Hwi_excSetBuffers(void * excContextBuffer, void * excStackBuffer);

/*!
 * @brief Reconfigure a dispatched interrupt.
 *
 * @param hwi Hwi handle
 * @param fxn Hwi function
 * @param params pointer to Hwi params structure
 */
extern void Hwi_reconfig(Hwi_Handle hwi, Hwi_FuncPtr fxn, const Hwi_Params *params);

/*! @cond NODOC */
extern Hwi_Handle Hwi_construct2(Hwi_Struct2 *hwi, int intNum, Hwi_FuncPtr hwiFxn, const Hwi_Params *prms);
extern void Hwi_initNVIC(void);
extern void Hwi_initStacks(void * hwiStack);
extern int Hwi_postInit(Hwi_Object *hwi, Error_Block *eb);
extern void Hwi_excHandlerAsm(void);
extern void Hwi_excHandler(unsigned int *excStack, unsigned int lr);
extern void Hwi_excHandlerMin(unsigned int *excStack, unsigned int lr);
extern void Hwi_excHandlerMax(unsigned int *excStack, unsigned int lr);
extern void Hwi_excFillContext(unsigned int *excStack);
extern void Hwi_excNmi(unsigned int *excStack);
extern void Hwi_excHardFault(unsigned int *excStack);
extern void Hwi_excMemFault(unsigned int *excStack);
extern void Hwi_excBusFault(unsigned int *excStack);
extern void Hwi_excUsageFault(unsigned int *excStack);
extern void Hwi_excSvCall(unsigned int *excStack);
extern void Hwi_excDebugMon(unsigned int *excStack);
extern void Hwi_excReserved(unsigned int *excStack, unsigned int excNum);
extern void Hwi_excNoIsr(unsigned int *excStack, unsigned int excNum);
extern void Hwi_excDumpRegs(unsigned int lr);
extern void Hwi_pendSV(void);
extern void Hwi_setStackLimit(void * stackBase);
extern unsigned int Hwi_swiDisableNull(void);
extern void Hwi_swiRestoreNull(unsigned int key);
extern void Hwi_dispatch(void);
extern unsigned int Hwi_dispatchC(uintptr_t irp, uint32_t dummy1, uint32_t dummy2, Hwi_Object *hwi);
extern void Hwi_doSwiRestore(unsigned int tskKey);
extern void Hwi_doTaskRestore(unsigned int tskKey);
extern unsigned int Hwi_swiDisable();
extern void Hwi_swiRestore(unsigned int key);
extern void Hwi_swiRestoreHwi(unsigned int key);
extern unsigned int Hwi_taskDisable();
extern void Hwi_taskRestoreHwi(unsigned int key);
/*! @endcond */

/*! @cond NODOC */
/*!
 * @brief Indicate that we are leaving the boot stack and are about to switch to
 * a task stack. Used by Task_startup()
 */
extern void Hwi_switchFromBootStack(void);

/* checkStack */
extern void Hwi_checkStack(void);

/* initStack */
extern void Hwi_initStack(void);

/* initStackMin */
extern void Hwi_initStackMin(void);

/* switchAndRunFunc */
extern void Hwi_switchAndRunFunc(void (*func)(void));
/*! @endcond */

/*
 * ======== SYSTEM FUNCTIONS ========
 */

/* Params_init */
extern void Hwi_Params_init(Hwi_Params *prms);

/*! @cond NODOC */
/* Params_copy */
extern void Hwi_Params_copy(Hwi_Params *dst, Hwi_Params *src);
/*! @endcond */

/* Object_first */
extern Hwi_Handle Hwi_Object_first(void);

/* Object_next */
extern Hwi_Handle Hwi_Object_next(Hwi_Object *obj);

/*
 * ======== STATE STRUCTURES ========
 */

/*! @cond NODOC */
extern Hwi_Module_State Hwi_Module_state;

#define Hwi_module ((Hwi_Module_State *) &(Hwi_Module_state))

#define Hwi_Object_heap() NULL

static inline Hwi_Handle Hwi_handle(Hwi_Struct *str)
{  
    return ((Hwi_Handle)str);
}

static inline Hwi_Struct * Hwi_struct(Hwi_Handle h)
{
    return ((Hwi_Struct *)h);
}
/*! @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_family_arm_m3__Hwi__include */

/*! @cond NODOC */
#if defined(ti_sysbios_family_arm_m3_Hwi__nolocalnames)
#define do_hwi_undef_short_names
#include "Hwi_defs.h"
#endif
/*! @endcond */
