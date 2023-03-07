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
 * @file ti/sysbios/runtime/Startup.h
 * @brief The startup bootstrap
 *
 * This module manages the very early startup initialization that occurs before
 * C's `main()` function is invoked.  This initialization typically consists of
 * setting hardware specific registers that control watchdog timers, access to
 * memory, cache settings, clock speeds, etc.
 *
 * In addition to configuration parameters that allow the user to add custom
 * startup functions, this module also provides services that allow other modules to
 * add initialiazation functions to the startup sequence.
 *
 * This module adds two functions to the generated ti_sysbios_config.c file:
 * `Startup_reset()` and `Startup_exec()`. You can review the ti_sysbios_config.c
 * to see how these functions are generated.
 *
 * <h3>Startup Sequence</h3>
 * The following list defines the startup sequence and, in particular, when user
 * provided startup functions are invoked:
 *
 *  - CPU is initialized and initial C stack setup is performed.
 *  - The functions configured as "Reset Function" are called.
 *  - C runtime initialization is performed.
 *  - The functions configured as "First Functions" are called.
 *  - The functions configured as "Module Init Functions" are called.
 *  - The functions configured as "Last Functions" are called.
 *  - The function `main()` is called.
 *
 * <h3>Examples</h3>
 * The following code shows how to add a custom reset functions to this module.
 *
 * @code
 *    const Startup = scripting.addModule("/ti/sysbios/runtime/Startup", {}, false);
 *    const myStartup = Startup.addInstance();
 *    myStartup.startupFxn = "myResetFxn";
 *    myStartup.functionType = "Reset Function";
 * @endcode
 */

/*
 * ======== Startup.h ========
 */

#ifndef ti_sysbios_runtime_Startup__include
#define ti_sysbios_runtime_Startup__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_Startup_long_names
#include "Startup_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Startup_done is set to true when startup is complete.
 */
extern volatile bool Startup_done;

/*!
 * @brief Execute the startup functions
 *
 * This function executes the configured Startup functions in the
 * following order:
 *  - The functions configured as "First Functions" are called.
 *  - The functions configured as "Module Init Functions" are called.
 *  - The functions configured as "Last Functions" are called.
 */
extern void Startup_exec(void);

/*!
 * @brief Application-specific reset functions
 *
 * This function executes the configured Startup functions that
 * are configured as "Reset Functions".
 */
extern void Startup_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_runtime_Startup__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_Startup_long_names
#include "Startup_defs.h"
/*! @endcond */
