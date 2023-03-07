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
 * @file ti/sysbios/runtime/SysMin.h
 * @brief Minimal implementation of SystemSupport.h
 *
 * This implementation provides a fully functional implementation of all methods
 * specified by `SystemSupport.h`.
 *
 * The module maintains an internal buffer (with a configurable size) that
 * stores on the "output". When full, the data is over-written.  When
 * `System_flush()` is called the characters in the internal buffer are "output"
 * using the user configurable SysMin.outputFxn.
 *
 * These functions are invoked by the System module. Application code
 * does not directly call these functions.
 */

#ifndef ti_sysbios_runtime_SysMin__include
#define ti_sysbios_runtime_SysMin__include

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*! @cond NODOC */
/* BIOS 6.x compatibility, use -Dxdc_std__include to disable */
#include <xdc/std.h>

#define ti_sysbios_runtime_SysMin_long_names
#include "SysMin_defs.h"
/*! @endcond */

#ifdef __cplusplus
extern "C" {
#endif

/*! @cond NODOC */
typedef struct SysMin_Module_State SysMin_Module_State;

extern char SysMin_outbuf[];

struct SysMin_Module_State {
    size_t bufsize;
    char *outbuf;
    unsigned int outidx;
    bool wrapped;
};
/*! @endcond */

/*!
 * @brief Output characters in the specified buffer
 *
 * The first parameter is a pointer to a buffer of characters to be output.  The
 * second parameter is the number of characters in the buffer to output.
 *
 * This function may be called with 0 as the second parameter.  In this case,
 * the function should simply return.
 */
typedef void (*SysMin_OutputFxn)(char* arg1, unsigned int arg2);

/*!
 * @brief Size (in MAUs) of the output.
 *
 * An internal buffer of this size is allocated. All output is stored in this
 * internal buffer.
 *
 * If 0 is specified for the size, no buffer is created, all output is dropped,
 * and SysMin_ready always returns `false`.
 */
extern const size_t SysMin_bufSize;

/*!
 * @brief Flush the internal buffer during @link System_exit() @endlink or @link
 * System_abort() @endlink.
 *
 * If the application's target is a TI target, the internal buffer is flushed
 * via the `HOSTwrite` function in the TI C Run Time Support (RTS) library.
 *
 * If the application's target is not a TI target, the internal buffer is
 * flushed to `stdout` via `fwrite(..., stdout)`.
 *
 * Setting this parameter to `false` reduces the footprint of the application at
 * the expense of not getting output when the application ends via a
 * `System_exit()`, `System_abort()`, `exit()` or `abort()`.
 */
extern const bool SysMin_flushAtExit;

/*! @cond NODOC */
extern const SysMin_OutputFxn SysMin_outputFxn;
extern const SysMin_OutputFxn SysMin_outputFunc;
extern void SysMin_output(char *buf, unsigned int size);

extern void SystemSupport_init(void);
extern void SystemSupport_abort(const char * str);
extern void SystemSupport_exit(int stat);
extern void SystemSupport_flush(void);
extern void SystemSupport_putch(char ch);
extern bool SystemSupport_ready(void);
extern void SystemSupport_output(char *buf, unsigned int size);

extern SysMin_Module_State SysMin_Module_state;

#define SysMin_module ((SysMin_Module_State *) &(SysMin_Module_state))
/*! @endcond */

#ifdef __cplusplus
}
#endif
#endif /* ti_sysbios_runtime_System__include */

/*! @cond NODOC */
#undef ti_sysbios_runtime_SysMin_long_names
#include "SysMin_defs.h"
/*! @endcond */
