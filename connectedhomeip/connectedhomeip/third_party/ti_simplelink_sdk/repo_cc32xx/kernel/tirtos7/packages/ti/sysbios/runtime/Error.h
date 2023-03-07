/*
 * Copyright (c) 2020-2021, Texas Instruments Incorporated - http://www.ti.com
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
 * @file ti/sysbios/runtime/Error.h
 * @brief Runtime error manager
 *
 * The `Error` module provides mechanisms for raising, checking, and handling
 * errors in a program. At the configuration time, you can use the parameters
 * Error.policy and Error.raiseHook to specify what happens when an error
 * takes place. You can control how much debugging information is available in
 * that case, while also controlling the memory footprint that the `Error'
 * module adds to the program.
 *
 * The Error.policy configuration parameter defines what happens when an
 * error is raised. If Error.policy is set to "Error_UNWIND", the error will
 * be captured in the error block and control will be returned to the caller.
 * If Error.policy is set to "Error_TERMINATE", System_abort() will be called
 * when an error is raised.
 *
 * The Error.raiseHook configuration parameter allows a
 * configured function to be invoked when an error is raised.
 * This function is passed a pointer to the error's error
 * block and makes it easy to manage all errors from a common point. For
 * example, you can trap any error (fatal or not) by simply setting a breakpoint
 * in this function. You can use Error_getMsg() to extract information
 * from an error block.
 *
 * Example 1: The following C code shows that you may pass a special constant
 * Error_IGNORE in place of an error block to a function requiring an error
 * block. The purpose of this constant is to avoid allocating an error block on
 * stack in the use case where the caller is not checking the error block after
 * the call returns. In this example, the caller only checks the returned value
 * but not the error block. If the function raises an error, the program will
 * return to the caller, assuming Error.policy is set to "Error_UNWIND".
 *
 * @code
 *  #include <ti/sysbios/runtime/Error.h>
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  tsk = Task_create(..., Error_IGNORE);
 *  if (tsk != NULL) {
 *      ...
 *  }
 * @endcode
 *
 * @htmlonly
 *  <hr />
 * @endhtmlonly
 *
 * Example 2: The following C code shows that you may pass a special constant
 * Error_ABORT (NULL) in place of an error block to a function
 * requiring an error block. In this case, if the function raises an error,
 * the program is aborted (via System_abort()), thus execution control will
 * never return to the caller.
 *
 * @code
 *  #include <ti/sysbios/runtime/Error.h>
 *  #include <ti/sysbios/knl/Task.h>
 *
 *  tsk = Task_create(..., Error_ABORT);
 *  ...will never get here if an error was raised in Task_create...
 * @endcode
 *
 * @htmlonly
 *  <hr />
 * @endhtmlonly
 *
 * Example 3: The following C code supplies an error block to a function that
 * requires one and tests the error block to see if the function raised an
 * error. Note that an error block must be initialized before it can be used and
 * same error block may be passed to several functions.
 *
 * @code
 *  #include <ti/sysbios/runtime/Error.h>
 *  #include <ti/sysbios/knl/Task.h>
 *  Error_Block eb;
 *  Task_Handle tsk;
 *
 *  Error_init(&eb);
 *  tsk = Task_create(..., &eb);
 *
 *  if (Error_check(&eb)) {
 *      ...an error has been raised...
 *  }
 * @endcode
 *
 * @htmlonly
 *  <hr />
 * @endhtmlonly
 */

/*
 * ======== Error.h ========
 */

#ifndef ti_sysbios_runtime_Error__include
#define ti_sysbios_runtime_Error__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_Error_long_names
#include "Error_defs.h"
/* @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Error block
 *
 * An opaque structure used to store information about errors once raised. This
 * structure must be initialized via @link Error_init @endlink before being
 * used for the first time.
 */
typedef struct Error_Block {
    const char * id;
    intptr_t a0;
    intptr_t a1;
} Error_Block;

typedef void (*Error_HookFxn)(Error_Block *eb);

/* @cond NODOC */
extern Error_Block Error_ignore;

#define Error_UNWIND 0
#define Error_TERMINATE 1
#define Error_SPIN 2
/* @endcond */

/*!
 * @brief Out of memory error
 *
 * The first parameter must be the heap instance handle. The second parameter is
 * the size of the object for which the allocation failed.
 */
#define Error_E_memory "out of memory: heap=0x%x, size=%u"

/*!
 * @brief Generic error
 *
 * This error takes advantage of the $S specifier to allow for recursive
 * formatting of the error message passed to error raise.
 *
 * For example, the following is possible:
 *
 * @code
 *  Error_raise(eb, Error_E_generic, "Error occurred, code: %d", code);
 * @endcode
 *
 * @see System#extendedFormats @see System#printf
 */
#define Error_E_generic "error %s"

/*!
 * @brief A pointer to a special Error_Block used when the caller does not want
 * to check Error_Block
 *
 * This constant should be used when the caller does not plan to check
 * Error_Block after the call returns, but wants the call to return even in
 * the case when an error is raised.  The configuration parameter, Error.policy,
 * is still in effect and the application will still terminate when an error
 * is raised if Error.policy is not set to "Error_UNWIND".
 */
#define Error_IGNORE (&Error_ignore)

/*!
 * @brief A special Error_Block pointer that terminates the application in case
 * of an error
 *
 * This constant has the same effect as passing NULL in place of an
 * Error_Block. If an error is raised when Error_ABORT is passed, the
 * application terminates regardless of the Error.policy configuration
 * parameter setting.
 */
#define Error_ABORT NULL

/*!
 * @brief Return true if an error was raised
 *
 * If eb is non-NULL and Error.policy is set to Error_UNWIND and an
 * error was raised on eb, this function returns true.  Otherwise, it
 * returns false.
 *
 * @param eb pointer to an Error_Block, Error_ABORT or Error_IGNORE
 * 
 * @retval true if an error was raised, false if no error raised
 */
extern bool Error_check(Error_Block *eb);

/*!
 * @brief Initialize an error block
 *
 * If eb is non-NULL, initialize the fields in the error block to 0.
 *
 * @param eb pointer to an Error_Block
 */
extern void Error_init(Error_Block *eb);

/*!
 * @brief Print error using System.printf()
 *
 * This function prints the error using System_printf().  The output is on a
 * single line terminated with a new line character and has the following form:
 *
 * @code
 *      Error_raised: {err_msg} ({file}:{line})
 * @endcode
 *
 * where "{file}" and "{line} are the file and line at the Error_raise() call site,
 * and "{err_msg}" is the error message rendered with the arguments associated with
 * the error.
 *
 * If "eb" is "Error_ABORT" or "Error_IGNORE", this function simply returns with
 * no output.
 *
 * @warning
 * This function is not protected by a gate and, as a result, if two threads
 * call this method concurrently, the output of the two calls will be
 * intermingled.  To prevent intermingled error output, you can either wrap all
 * calls to this method with an appropriate `Gate_enter`/`Gate_leave` pair or
 * simply ensure that only one thread in the system ever calls this method.
 *
 * @param eb pointer to an "Error_Block", "Error_ABORT" or "Error_IGNORE"
 */
extern void Error_print(Error_Block *eb);

/* @cond NODOC */

extern void Error_raiseX(Error_Block *eb, int prefix, const char * id, intptr_t a0, intptr_t a1);

#if Error_retainStrings_D
#define Error_PLACE_STR_IN_SECTION(name, str) static const char name[] = (str);
#else
#if defined(__IAR_SYSTEMS_ICC__)
#define Error_PLACE_STR_IN_SECTION(name, str) \
    __root static const char name[] @ ".error_str" = str;
#elif defined(__TI_COMPILER_VERSION__) || (defined(__clang__) && defined(__ti_version__)) || defined(__GNUC__)
#define Error_PLACE_STR_IN_SECTION(name, str) \
    static const char name[] __attribute__((used,section(".error_str"))) = str;
#else
#error Incompatible compiler: Error/Assert is currently supported by the following \
compilers: TI ARM Compiler, TI CLANG Compiler, GCC, IAR. Please migrate to a \
a supported compiler.
#endif
#endif

#define Error_STR_NX(l) #l
#define Error_STR(l) Error_STR_NX(l)

#if Error_addFileLine_D
#define Error_raise(eb, id, a0, a1)  do { \
    Error_PLACE_STR_IN_SECTION(err, id " (" __FILE__ ":" Error_STR(__LINE__) ")"); \
    Error_raiseX((eb), 0, err, (intptr_t)(a0), (intptr_t)(a1)); } while (0);
#else
#define Error_raise(eb, id, a0, a1) do { \
    Error_PLACE_STR_IN_SECTION(err, id); \
    Error_raiseX((eb), 0, err, (intptr_t)(a0), (intptr_t)(a1)); } while (0);
#endif

#define Error_assert(id, a0, a1) do { \
    Error_PLACE_STR_IN_SECTION(err, id); \
    Error_raiseX(NULL, 1, err, (intptr_t)(a0), (intptr_t)(a1)); } while (0);

/* @endcond */

#ifdef __cplusplus
}
#endif

#endif /* ti_sysbios_runtime_Error__include */

/* @cond NODOC */
#undef ti_sysbios_runtime_Error_long_names
#include "Error_defs.h"
/* @endcond */
