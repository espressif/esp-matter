/*
 * Copyright (c) 2016-2018 Texas Instruments Incorporated - http://www.ti.com
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

/*
 * ======== logging.h ========
 */
#ifndef _LOGGING_H_
#define _LOGGING_H_

#if 1
#define IArg void *
#define xdc_IArg void *
#define Diags_ANALYSIS 0

#if 0
#define Log_print0(a, b)                 printf(b "\n")
#define Log_print1(a, b, c)              printf(b "\n", c)
#define Log_print2(a, b, c, d)           printf(b "\n", c, d)
#define Log_print3(a, b, c, d, e)        printf(b "\n", c, d, e)
#define Log_print4(a, b, c, d, e, f)     printf(b "\n", c, d, e, f)
#else
/* disabled for now */
#define Log_print0(a, b)
#define Log_print1(a, b, c)
#define Log_print2(a, b, c, d)
#define Log_print3(a, b, c, d, e)
#define Log_print4(a, b, c, d, e, f)
#endif

#define Log_error0(a)
#define Log_error1(a, b)

#define NOREGISTRY 1

#else
#if defined(xdc_runtime_Log_DISABLE_ALL) && \
    defined(xdc_runtime_Assert_DISABLE_ALL)
#define NOREGISTRY 1
#endif

#ifndef NOREGISTRY
/* This define must precede inclusion of any xdc header files */
#define Registry_CURDESC ti_net_http_HTTPServer_desc
#endif

#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>

#ifndef NOREGISTRY
#include <xdc/runtime/Registry.h>
extern Registry_Desc ti_net_http_HTTPServer_desc;
#endif

#endif

#endif
