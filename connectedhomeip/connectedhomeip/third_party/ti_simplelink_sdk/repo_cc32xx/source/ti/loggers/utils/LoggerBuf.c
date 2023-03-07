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
 *  ======== LoggerBuf.c ========
 */

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <ti/utils/osal/Interrupt.h>
#include <ti/utils/osal/Timestamp.h>

#include <ti/utils/runtime/ILogger.h>
#include <ti/loggers/utils/LoggerBuf.h>

#define LoggerBuf_FULL -1
#define LoggerBuf_MAX_ARGS (LoggerBuf_WORDS_PER_RECORD - 1)


static void findNextRecord(LoggerBuf_Handle inst, LoggerBuf_Rec **rec)
{
    /* compute next available record */
    *rec = inst->curEntry;
    if (*rec == inst->endEntry) {
        if (inst->advance == LoggerBuf_Type_CIRCULAR) {
            inst->curEntry = inst->buffer;
        }
        else {
            inst->advance = LoggerBuf_FULL;
        }
    }
    else {
        inst->curEntry = (LoggerBuf_Rec *)((char *)*rec + sizeof(LoggerBuf_Rec));
    }
}

/*
 *  ======== LoggerBuf_event ========
 */
void ti_loggers_utils_LoggerBuf_event(ILogger_Handle handle,
                                      uintptr_t header,
                                      uintptr_t event,
                                      uintptr_t arg0,
                                      uintptr_t arg1,
                                      uintptr_t arg2,
                                      uintptr_t arg3)
{
    uintptr_t key;
    uint32_t serial;
    LoggerBuf_Rec *rec;
    LoggerBuf_Handle inst = (LoggerBuf_Handle)handle;

    /* TODO - test null handle at call site */
    if (handle == NULL) return;

    /* disable interrupts */
    key = Interrupt_disable();

    /* increment serial even when full */
    serial = ++(inst->serial);

    if (inst->advance == LoggerBuf_FULL) {
        Interrupt_restore(key);
        goto leave;
    }

    /* compute next available record */
    findNextRecord(inst, &rec);

    Timestamp_get64((Timestamp_Val64 *)&rec->ts_hi);

    /* enable interrupts */
    Interrupt_restore(key);

    /* write data to record */
    rec->serial = serial;
    rec->type = LoggerBuf_EVENT;
    rec->data[0] = event;
    rec->data[1] = arg0;
    rec->data[2] = arg1;
    rec->data[3] = arg2;
    rec->data[4] = arg3;

leave:
    return;
}

/*
 *  ======== LoggerBuf_printf ========
 */
void  ti_loggers_utils_LoggerBuf_printf(ILogger_Handle handle,
                                        uint32_t header,
                                        uint32_t numArgs,
                                        ...)
{
    uintptr_t key;
    uint32_t serial;
    LoggerBuf_Rec *rec;
    LoggerBuf_Handle inst = (LoggerBuf_Handle)handle;
    va_list argptr;
    uint32_t argsToCopy = numArgs;

    /* Guard against more arguments being passed in than supported */
    if(numArgs > LoggerBuf_MAX_ARGS)
    {
        argsToCopy = LoggerBuf_MAX_ARGS;
    }

    va_start(argptr, numArgs);

    if (handle == NULL) return;

    /* disable interrupts */
    key = Interrupt_disable();

    /* increment serial even when full */
    serial = ++(inst->serial);

    if (inst->advance == LoggerBuf_FULL) {
        Interrupt_restore(key);
        goto leave;
    }

    /* compute next available record */
    findNextRecord(inst, &rec);

    Timestamp_get64((Timestamp_Val64 *)&rec->ts_hi);

    /* enable interrupts */
    Interrupt_restore(key);

    /* write data to record */
    rec->serial = serial;
    rec->type = LoggerBuf_PRINTF;
    va_arg(argptr, uintptr_t);
    rec->data[0] = header;

    uint32_t i;
    for(i = 0; i < argsToCopy; i++)
    {
        rec->data[1+i] = va_arg(argptr, uintptr_t);
    }

    va_end (argptr);

leave:
    return;
}

/*
 *  ======== LoggerBuf_buf ========
 */
void  ti_loggers_utils_LoggerBuf_buf(ILogger_Handle handle,
                                     uint32_t header,
                                     const char* format,
                                     uint8_t *data,
                                     size_t size)
{
    uintptr_t key;
    uint32_t serial;
    LoggerBuf_Rec *rec;
    LoggerBuf_Handle inst = (LoggerBuf_Handle)handle;
    uint32_t numRecords = ((size) / LoggerBuf_SIZEOF_RECORD) +                 \
                          ((size % LoggerBuf_SIZEOF_RECORD) != 0);
    numRecords += 1;
    uint32_t i;

    if (handle == NULL) return;

    /* disable interrupts */
    key = Interrupt_disable();

    /* Here we aquire records as contiguous memory.
     * This approach leads to long critical sections for large buffers
     * and can be improved in the future once all hosts support fragmentation
     * of packets
     */
    for(i = 0; i < numRecords; i++)
    {
        /* increment serial even when full */
        serial = ++(inst->serial);

        if (inst->advance == LoggerBuf_FULL) {
            Interrupt_restore(key);
            goto leave;
        }

        /* compute next available record */
        findNextRecord(inst, &rec);

        Timestamp_get64((Timestamp_Val64 *)&rec->ts_hi);

        /* write data to record */
        rec->serial = serial;
        if(i == 0)
        {
            rec->type = LoggerBuf_BUFFER_START;
            rec->data[0] = header;
            rec->data[1] = size;

        }
        else {
            rec->type = LoggerBuf_BUFFER_CONTINUED;
            memcpy(rec->data,
                   &data[(i-1)*LoggerBuf_SIZEOF_RECORD],
                   LoggerBuf_SIZEOF_RECORD);
        }
    }

    /* enable interrupts */
    Interrupt_restore(key);

leave:
    return;
}
