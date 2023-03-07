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
 * @file ti/sysbios/runtime/System.h
 * @brief Basic system services
 *
 * This module provides basic low-level "system" services; e.g., character
 * output, `printf`-like output, and exit handling.
 */

#ifndef ti_sysbios_runtime_System__include
#define ti_sysbios_runtime_System__include

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_System_long_names
#include "System_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Assert that the target's `float` type fits in an `intptr_t`
 *
 * This assertion is triggered when the `%f` format specifier is used, the
 * argument treated as an `intptr_t`, but for the current target `sizeof(Float)` >
 * `sizeof(intptr_t)`.
 */
#define System_A_cannotFitIntoArg "sizeof(float) > sizeof(intptr_t)"

/*! @cond NODOC */
typedef struct System_ParseData System_ParseData;
typedef struct System_Module_State System_Module_State;
typedef int (*System_ExtendFxn)(char** arg1, const char ** arg2, va_list* arg3, System_ParseData* arg4);
/*! @endcond */

/*!
 * @brief `System`'s atexit function prototype.
 *
 * Functions of this type can be added to the list of functions that are
 * executed during application termination.
 */
typedef void (*System_AtexitHandler)(int arg1);

/*! @cond NODOC */
struct System_ParseData {
    int width;
    bool lFlag;
    bool lJust;
    int precis;
    unsigned int len;
    int zpad;
    char *end;
    bool aFlag;
    char *ptr;
};

struct System_Module_State {
    System_AtexitHandler *atexitHandlers;
    int numAtexitHandlers;
    bool initDone;
};
/*! @endcond */

/*!
 * @brief System abort function prototype.
 *
 * Functions of this type can be plugged into `System`'s abort function that
 * will be executed during abnormal application termination.
 */
typedef void (*System_AbortFxn)(void);

/*!
 * @brief System exit function prototype.
 *
 * Functions of this type can be plugged into `System`'s exit function that will
 * be executed during normal application termination.
 */
typedef void (*System_ExitFxn)(int arg1);

/*!
 * @brief Maximum number of dynamic atexit handlers allowed in the system.
 *
 * Maximum number of `System` `atexit` handlers set during runtime via the @link
 * System_atexit @endlink function.
 */
extern const int System_maxAtexitHandlers;

/*!
 * @brief Abort handler function
 *
 * This configuration parameter allows user to plug in their own abort function.
 * By default @link System_abortStd @endlink which calls ANSI C Standard
 * `abort()` is plugged in. Alternatively @link System_abortSpin @endlink can be
 * plugged which loops infinitely.
 */
extern const System_AbortFxn System_abortFxn;

/*!
 * @brief Exit handler function
 *
 * This configuration parameter allows user to plug in their own exit function.
 * By default @link System_exitStd @endlink which calls ANSI C Standard `exit()`
 * is plugged in. Alternatively @link System_exitSpin @endlink can be plugged
 * which loops infinitely.
 */
extern const System_ExitFxn System_exitFxn;

/*! @cond NODOC */
extern const System_ExtendFxn System_extendFxn;

extern void System_init(void);
/*! @endcond */

/*!
 * @brief Print a message and abort currently running executable.
 *
 * This is called when an executable abnormally terminates. 
 * No exit functions bound via `System_atexit()` or the ANSI C Standard Library
 * `atexit()` functions are executed.
 *
 * @param str abort message (not a format string)
 */
extern void System_abort(const char * str);

/*!
 * @brief ANSI C Standard implementation of abortFxn function
 *
 * This function calls ANSI C Standard `abort()` to terminate currently running
 * executable. This function is used by default in @link System_abortFxn
 * @endlink.
 */
extern void System_abortStd(void);

/*!
 * @brief Lightweight implementation of abortFxn function
 *
 * This functions loops indefinitely. This can used as an alternative @link
 * System_abortFxn @endlink when a lightweight implementation is required
 * instead of the ANSI C Standard `abort()`.
 */
extern void System_abortSpin(void);

/*!
 * @brief Add an exit handler
 *
 * `System_atexit` pushes `handler` onto an internal stack of functions to be
 * executed when system is exiting (e.g. `System_exit` is called). Up to @link
 * System_maxAtexitHandlers @endlink functions can be specified in this manner.
 * During the exit processing, the functions are popped off the internal stack
 * and called until the stack is empty.
 *
 * The `System` gate is entered before the `System_atexit` functions are called.
 *
 * Returns `true` on success. If `false` is returned, the exit handler was not
 * added and it will not be called during an exit.
 *
 * @param handler the `AtexitHandler` to invoke during system
 * 
 * @retval true if handler was registered successfully, false otherwise
 */
extern bool System_atexit(System_AtexitHandler handler);

/*!
 * @brief Exit currently running executable.
 *
 * This function is called when an executable needs to terminate normally.  This
 * function processes all functions bound via `System_atexit` and then calls
 * @link System_exitFxn @endlink.
 *
 * @param stat exit status to return to calling environment.
 */
extern void System_exit(int stat);

/*!
 * @brief Implements an `exitFxn` function
 *
 * This function calls ANSI C Standard `exit()` to terminate currently running
 * executable normally. This function is used by default in @link System_exitFxn
 * @endlink.
 *
 * @param stat exit status to return to calling environment.
 */
extern void System_exitStd(int stat);

/*!
 * @brief Implements an `exitFxn` function
 *
 * This functions loops indefinitely. This can used as an alternative @link
 * System_exitFxn @endlink when a light weight implementation is required
 * instead of the ANSI C Standard `exit()`.
 *
 * @param stat exit status to return to calling environment.
 */
extern void System_exitSpin(int stat);

/*!
 * @brief Output a single character
 *
 * The SystemSupport module's `putch` function is called by this
 * function.
 *
 * @param ch character to be output.
 */
extern void System_putch(char ch);

/*!
 * @brief Flush standard System I/O
 *
 * This function causes any buffered output characters are "written" to the
 * output device.
 *
 * The SystemSupport module's `flush` function is called by this
 * function.
 */
extern void System_flush (void);

/*!
 * @brief A smaller faster printf
 *
 * This function behaves much like the ANSI C Standard `printf` but does not
 * support the full range of format strings specified by the C Standard.  In
 * addition, several non-standard format specifiers are recognized.
 *
 * <h3>Format Strings</h3>
 * The format string is a character string composed of zero or more directives:
 * ordinary characters (not %), which are copied unchanged to the output stream;
 * and conversion specifications, each of which results in fetching zero or more
 * subsequent arguments.  Each conversion specification is introduced by the
 * character %, and ends with a conversion specifier.  In between there may be
 * (in this order) zero or more flags, an optional minimum field width, an
 * optional precision and an optional length modifier.
 *
 * <h3>Flags</h3>
 * The following flags are supported:
 *
 * - `-` The converted value is to be left adjusted on the field boundary (the
 * default is right justification.) - `0` The value should be zero padded. For
 * d, i, o, u, and x conversions, the converted value is padded on the left with
 * zeros rather than blanks.
 *
 * <h3>Field Width</h3>
 * The optional field width specifier is a decimal digit string (with nonzero
 * first digit) specifying a minimum field width. If the converted value has
 * fewer characters than the field width, it will be padded with spaces on the
 * left (or right, if the left-adjustment flag has been given).  Instead of a
 * decimal digit string one may write `*` to specify that the field width is
 * given in the next argument.  A negative field width is taken as a '-' flag
 * followed by a positive field width.
 *
 * <h3>Precision</h3>
 * The optional precision specifier is a period ('.') followed by an optional
 * decimal digit string.  Instead of a decimal digit string one may write `*` to
 * specify that the precision is given in the next argument which must be of
 * type int.
 *
 * If the precision is given as just '.', or the precision is negative, the
 * precision is taken to be zero.  This gives the minimum number of digits to
 * appear for d, i, o, u, and x conversions, or the maximum number of characters
 * to be printed from a string for s conversions.
 *
 * <h3>Length Modifiers</h3>
 * The optional length modifier is a single character from the following list.
 *
 * - `l` A  following integer conversion corresponds to a long int or unsigned
 * long int argument
 *
 * <h3>Conversion Specifiers</h3>
 * The following conversion specifiers are supported.
 *
 * - `d`, `i` signed integer - `u` unsigned decimal - `x` unsigned hex - `o`
 * unsigned octal - `p` pointer (@ + hex num) - `c` character - `s` string
 *
 * <h3>Extended Conversion Specifiers</h3>
 * The following conversion specifiers are optionally supported.  See the
 * System_extendedFormats configuration parameter for more information
 * about how to enable these conversion specifiers.
 *
 * @param fmt a 'printf-style' format string
 * 
 * @retval number of characters written
 */
extern int System_printf(const char * fmt, ...);

/*!
 * @brief Write formated output to a character buffer
 *
 * This function is identical to @link System_printf @endlink except that the
 * output is copied to the specified character buffer `buf` followed by a
 * terminating '\0' character.
 *
 * `System_sprintf` returns the number of characters output not including the '\0'
 * termination character.
 *
 * @param buf a character output buffer
 * @param fmt a 'printf-style' format string
 * 
 * @retval number of characters written
 */
extern int System_sprintf(char buf[], const char * fmt, ...);

/*!
 * @brief A va_list printf
 *
 * This function is identical to @link System_printf @endlink except that its
 * arguments are passed via a va_list (a "varargs list").
 *
 * by the fmt string
 *
 * `System_vprintf` returns the number of characters output.
 *
 * @param fmt a standard 'printf-style' format string.
 * @param va an args list that points to the arguments referenced

 * @retval number of characters written
 */
extern int System_vprintf(const char * fmt, va_list va);

/*!
 * @brief A `va_list` sprintf
 *
 * This function is identical to @link System_sprintf @endlink except that its
 * arguments are passed via a `va_list` (a "varargs list").
 *
 * by the `fmt` string
 *
 * `vsprintf` returns the number of characters output.
 *
 * @param buf a character output buffer
 * @param fmt a standard '`printf`-style' format string.
 * @param va an arguments list that points to the arguments referenced
 * 
 * @retval number of characters written
 */
extern int System_vsprintf(char buf[], const char * fmt, va_list va);

/*!
 * @brief Write formated output to a character buffer
 *
 * This function is identical to @link System_sprintf @endlink except that at
 * most `n` characters are copied to the specified character buffer `buf`. If n
 * is zero, nothing is written to character buffer. Otherwise, output characters
 * beyond the `n` - 1 are discarded rather than being written to the character
 * buf, and a null character is written at the end of the characters written
 * into the buffer.
 *
 * the output buffer `buf`
 *
 * `snprintf` returns the number of characters that would have been written had
 * `n` been sufficiently large, not counting the terminating '\0' character.
 *
 * @param buf a character output buffer
 * @param n the maximum number of characters, including '\0', written to
 * @param fmt a 'printf-style' format string
 * 
 * @retval number of characters written
 */
extern int System_snprintf(char buf[], size_t n, const char * fmt, ...);

/*!
 * @brief A `va_list` snprintf
 *
 * This function is identical to @link System_snprintf @endlink except that its
 * arguments are passed via a `va_list` (a "varargs list").
 *
 * output buffer
 *
 * by the `fmt` string
 *
 * `vsnprintf` returns the number of characters that would have been written had
 * `n` been sufficiently large, not counting the terminating '\0' character.
 *
 * @param buf a character output buffer
 * @param n at most number of characters including '\0' written to
 * @param fmt a standard '`printf`-style' format string.
 * @param va an arguments list that points to the arguments referenced
 * 
 * @retval number of characters written
 */
extern int System_vsnprintf(char buf[], size_t n, const char * fmt, va_list va);

/*! @cond NODOC */
extern void System_processAtExit(int stat);
extern int System_printf_va(const char * fmt, va_list _va);
extern int System_aprintf_va(const char * fmt, va_list _va);
extern int System_sprintf_va(char buf[], const char * fmt, va_list _va);
extern int System_asprintf_va(char buf[], const char * fmt, va_list _va);
extern int System_snprintf_va(char buf[], size_t n, const char * fmt, va_list _va);
extern int System_aprintf(const char * fmt, ...);
extern int System_asprintf(char buf[], const char * fmt, ...);
extern int System_avprintf(const char * fmt, va_list va);
extern int System_avsprintf(char buf[], const char * fmt, va_list va);
extern int System_printfExtend(char **bufp, const char * *fmt, va_list *va, System_ParseData *parse);
extern int System_doPrint(char buf[], size_t n, const char * fmt, va_list *pva, bool aFlag);
extern void System_lastFxn(void);
extern void System_putchar(char **bufp, char ch, size_t *n);
extern void System_rtsExit(void);
extern bool System_atexitDone( void);
extern System_Module_State System_Module_state;

#define System_module ((System_Module_State *) &(System_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_runtime_System__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_System_long_names
#include "System_defs.h"
/*! @endcond */
