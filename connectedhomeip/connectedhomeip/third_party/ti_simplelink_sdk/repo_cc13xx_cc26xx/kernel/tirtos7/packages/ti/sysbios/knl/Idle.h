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
 * @file ti/sysbios/knl/Idle.h
 * @brief Idle Thread Manager.
 *
 * The Idle module is used to specify a list of functions to be called when no
 * other tasks are running in the system.
 *
 * If tasking is enabled (ie @link BIOS_taskEnabled @endlink
 * = true), then the Task module will create an "Idle task" with the lowest
 * possible priority. When no other tasks are running, this idle task runs in an
 * infinite loop, calling the list of functions specified by the Idle module.
 *
 * If tasking is disabled (ie @link BIOS_taskEnabled @endlink
 * = false), then the idle functions are called in an
 * infinite loop within the @link BIOS_start @endlink function
 * called within main().
 *
 * The list of idle functions is only statically configurable; it cannot be
 * modified at runtime.
 *
 * To add an Idle function,
 * the following must be added to the app.syscfg file:
 *
 * @code 
 * const Idle    = scripting.addModule("/ti/sysbios/knl/Idle");
 * const Idle1   = Idle.addInstance();
 * Idle1.idleFxn = "idle_func";
 * @endcode 
 */

/*
 * ======== Idle.h ========
 */

#ifndef ti_sysbios_include_Idle__include
#define ti_sysbios_include_Idle__include

/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
/*! @cond NODOC */
#include <xdc/std.h>
/*! @endcond */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* @cond NODOC */
#define ti_sysbios_knl_Idle_long_names
#include "Idle_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Idle function type definition.
 */
typedef void (*Idle_FuncPtr)(void);

extern bool Idle_initDone;

/*! @cond NODOC */

/* For ROV, make Idle_funcList volatile so it can't be optimized away */
extern volatile const Idle_FuncPtr Idle_funcList[];

extern void Idle_init(void);

/*!
 * @brief Idle loop which calls the idle functions in an infinite loop.
 *
 * This function is called internally and is not normally intended to be called
 * by the client.
 *
 * When tasking is enabled, the Task module creates an Idle task which simply
 * calls this function. If tasking is disabled, then this function is called
 * after main and any module startup functions.
 *
 * The body of this function is an infinite loop that calls the "run" function.
 */
extern void Idle_loop(uintptr_t arg1, uintptr_t arg2);
/*! @endcond */

/*!
 * @brief Make one pass through idle functions
 *
 * This function is called repeatedly by the Idle task when the Idle task has
 * been enabled in the Task module (see Task.enableIdleTask config parameter).
 *
 * This function makes one pass through an internal static array of functions
 * which are defined in the configuration file.
 *
 * This function returns after all functions have been executed one time.
 */
extern void Idle_run(void);

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_knl_Idle__include */

/* @cond NODOC */
#undef ti_sysbios_knl_Idle_long_names
#include "Idle_defs.h"
/*! @endcond */
