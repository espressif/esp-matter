/*
 * Copyright (c) 2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== ILogger.h ========
 */

#ifndef ti_utils_runtime_ILogger__include
#define ti_utils_runtime_ILogger__include

#include <stdint.h>

#if defined (__cplusplus)
extern "C" {
#endif

/*
 *  ======== ILogger_Handle ========
 */
typedef void *ILogger_Handle;

/*
 *  ======== ILogger_event ========
 */
extern void ILogger_event(ILogger_Handle handle,
                          uint32_t header,
                          uintptr_t event,
                          uintptr_t arg0,
                          uintptr_t arg1,
                          uintptr_t arg2,
                          uintptr_t arg3);

/*
 *  ======== ILogger_printf ========
 */
extern void ILogger_printf(ILogger_Handle handle,
                           uint32_t header,
                           uint32_t numArgs,
                           ...);

/*
 *  ======== ILogger_buf ========
 */
extern void ILogger_buf(ILogger_Handle handle,
                        uint32_t header,
                        const char* format,
                        uint8_t *data,
                        size_t size);

#if defined (__cplusplus)
}
#endif

#endif /* ti_utils_runtime_ILogger__include */
