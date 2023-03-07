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
 * @file ti/sysbios/runtime/Assert.h
 * @brief Runtime assertion manager
 *
 * The `Assert` module provides configurable diagnostics to the program. Similar
 * to the standard C `assert()` macro, `Assert` methods are interspersed with
 * other code to add diagnostics to a program. Unlike the standard C assert
 * support, the `Assert` module provides greater flexibility in managing the
 * messages displayed, the message string space overhead, and the runtime
 * handling of failures. In addition, because the `Assert` methods build atop
 * the Diags module, you can precisely control which asserts remain in the final
 * application, if any.
 *
 * `Assert` statements are added to the code using the Assert_isTrue macro.
 *
 * The following C code adds an assert to application code
 *
 * @code
 *  #define BIOS_assertsEnabled_D 1
 *  #include <ti/sysbios/runtime/Assert.h>
 *
 *  Assert_isTrue(count > 0, NULL);
 * @endcode
 *
 * The Assert calls can also be completely removed by defining the symbol
 * BIOS_assertsEnabled_D to 0. This can be done on the compile line, e.g.
 * -DBIOS_assertsEnabled_D=0. This will completely remove the Assert
 * statements from any code compiled with this flag, regardless of the
 * application's configuration or your compiler's optimization settings.
 */

/*
 * ======== Assert.h ========
 */

#ifndef ti_sysbios_runtime_Assert__include
#define ti_sysbios_runtime_Assert__include

#include <ti/sysbios/runtime/Error.h>

/* @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_Assert_long_names
#include "Assert_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define Assert_E_assertFailed "%s"

#if BIOS_assertsEnabled_D
#if Assert_useBkpt_D
#define Assert_isTrue(c, id) do { \
    /* TODO: port the SW bkpt instruction to other ISAs */
    if (!(c)) __asm("bkpt #13"); } while (0);
#else
extern void Assert_failX(const char * id);
#if Assert_addFileLine_D
#define Assert_isTrue(c, id) do { \
    Error_PLACE_STR_IN_SECTION(loc, # id " (" __FILE__ ":" Error_STR(__LINE__) ")"); \
    if (!(c)) Assert_failX(loc); } while (0);
#else
#define Assert_isTrue(c, id) do { \
    Error_PLACE_STR_IN_SECTION(loc, # id); \
    if (!(c)) Assert_failX(loc); } while (0);
#endif
#endif
#else
#define Assert_isTrue(c, id)
#endif

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_runtime_Assert__include */

#undef ti_sysbios_runtime_Assert_long_names
#include "Assert_defs.h"
/* @endcond */
