/*
 * Copyright (c) 2019-2020 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LoggerBuf.h ========
 */

#ifndef ti_loggers_utils_LoggerBuf__include
#define ti_loggers_utils_LoggerBuf__include

#include <stdint.h>

#include <ti/utils/runtime/ILogger.h>

#if defined (__cplusplus)
extern "C" {
#endif

#define LoggerBuf_Type_LINEAR       (1)
#define LoggerBuf_Type_CIRCULAR     (2)
#define LoggerBuf_WORDS_PER_RECORD  (5)
#define LoggerBuf_BUF_HEADER_SIZE sizeof(LoggerBuf_RecordType) + sizeof(uint32_t)
#define LoggerBuf_SIZEOF_RECORD (sizeof(uint32_t)*LoggerBuf_WORDS_PER_RECORD)

typedef enum{
    LoggerBuf_EVENT = 0,
    LoggerBuf_PRINTF = 1,
    LoggerBuf_BUFFER_START = 2,
    LoggerBuf_BUFFER_CONTINUED = 3
} LoggerBuf_RecordType;


/*
 *  ======== LoggerBuf_Rec ========
 */
typedef struct LoggerBuf_Rec {
    uint32_t                serial;
    uint32_t                ts_hi;
    uint32_t                ts_lo;
    LoggerBuf_RecordType    type;
    uint32_t                data[LoggerBuf_WORDS_PER_RECORD];
} LoggerBuf_Rec;

/*
 *  ======== LoggerBuf_Instance ========
 */
typedef struct LoggerBuf_Instance {
    uint8_t             bufType;
    int8_t              advance;
    uint16_t            numEntries;
    uint32_t            serial;
    LoggerBuf_Rec      *buffer;
    LoggerBuf_Rec      *curEntry;               /* next record to write */
    LoggerBuf_Rec      *endEntry;
} LoggerBuf_Instance;

/*
 *  ======== LoggerBuf_Handle ========
 */
typedef LoggerBuf_Instance *LoggerBuf_Handle;

/*
 *  ======== LoggerBuf_event ========
 */
extern void ti_loggers_utils_LoggerBuf_event(ILogger_Handle handle,
                                             uintptr_t header,
                                             uintptr_t event,
                                             uintptr_t arg0,
                                             uintptr_t arg1,
                                             uintptr_t arg2,
                                             uintptr_t arg3);

/*
 *  ======== LoggerBuf_printf ========
 */
extern void  ti_loggers_utils_LoggerBuf_printf(ILogger_Handle handle,
                                               uint32_t header,
                                               uint32_t numArgs,
                                               ...);

/*
 *  ======== LoggerBuf_buf ========
 */
extern void  ti_loggers_utils_LoggerBuf_buf(ILogger_Handle handle,
                                            uint32_t header,
                                            const char* format,
                                            uint8_t *data,
                                            size_t size);

#if defined (__cplusplus)
}
#endif

#endif /* ti_loggers_utils_LoggerBuf__include */
