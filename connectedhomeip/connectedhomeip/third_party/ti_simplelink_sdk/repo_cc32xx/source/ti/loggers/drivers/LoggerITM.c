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
 *  ======== LoggerITM.c ========
 */

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include <ti/utils/runtime/ILogger.h>
#include <ti/loggers/drivers/LoggerITM.h>

#include <ti/drivers/dpl/HwiP.h>
#include <ti/drivers/ITM.h>

#define LoggerITM_RESET_FRAME   (0xBBBBBBBB)

/* Only the first call to LoggerITM_init will result in opening the driver */
static bool isOpen = false;

/*
 *  ======== LoggerITM_init ========
 */
void LoggerITM_init(void)
{
    /* disable interrupts */
    uint32_t key = HwiP_disable();
    /* Setup and enable ITM driver */
    bool itmOpened = ITM_open();

    if(itmOpened)
    {
        /* Send the reset sequence */
        ITM_send32Atomic(LoggerITM_STIM_LOGGER, LoggerITM_RESET_FRAME);
        /* Enable generation of time stamps based on system CPU */
        ITM_enableTimestamps(ITM_TS_DIV_16, false);

        isOpen = true;
    }


    /* enable interrupts */
    HwiP_restore(key);
}

/*
 *  ======== ti_loggers_drivers_LoggerITM_event ========
 */
void ti_loggers_drivers_LoggerITM_event(ILogger_Handle handle,
                                        uintptr_t header,
                                        uintptr_t event,
                                        uintptr_t arg0,
                                        uintptr_t arg1,
                                        uintptr_t arg2,
                                        uintptr_t arg3)
{
    uint32_t key;

    if (handle == NULL)
    {
        return;
    }

    /* Check that the ITM driver has been opened
     * Technically it is safe to write to the ITM port even if the driver has
     * not yet been opened. However, we will avoid the critical section if
     * the user has not yet initialized ITM
     */
    if(!isOpen)
    {
        return;
    }

    /* disable interrupts */
    key = HwiP_disable();

    /* Send the pointer to the meta string that is placed by the callsite */
    ITM_send32Polling(LoggerITM_STIM_HEADER, header);

    /* Send out the user arguments */
    ITM_send32Polling(LoggerITM_STIM_TRACE, arg0);
    ITM_send32Polling(LoggerITM_STIM_TRACE, arg1);
    ITM_send32Polling(LoggerITM_STIM_TRACE, arg2);
    ITM_send32Polling(LoggerITM_STIM_TRACE, arg3);

    /* enable interrupts */
    HwiP_restore(key);

    return;
}

/*
 *  ======== LoggerITM_printf ========
 */
void  ti_loggers_drivers_LoggerITM_printf(ILogger_Handle handle,
                                          uint32_t header,
                                          uint32_t numArgs,
                                          ...)
{
    va_list argptr;
    uint32_t key;
    va_start(argptr, numArgs);

    /* disable interrupts */
    key = HwiP_disable();

    /* Send header */
    ITM_send32Polling(LoggerITM_STIM_HEADER, header);

    /* Here we pop the format string argument off the VA_ARGS
     * it is not used by LoggerITM
     */
    va_arg(argptr, uintptr_t);

    uint32_t i;
    for(i = 0; i < numArgs; ++i)
    {
        uintptr_t arg = va_arg(argptr, uintptr_t);
        ITM_send32Polling(LoggerITM_STIM_TRACE, arg);
    }

    /* enable interrupts */
    HwiP_restore(key);

    va_end (argptr);
}

/*
 *  ======== LoggerITM_buf ========
 */
void ti_loggers_drivers_LoggerITM_buf(ILogger_Handle handle,
                                      uint32_t header,
                                      const char* format,
                                      uint8_t *data,
                                      size_t size)
{
    uint32_t key;
    /* disable interrupts */
    key = HwiP_disable();

    /* Send header */
    ITM_send32Polling(LoggerITM_STIM_HEADER, header);
    /* We always send the size of the expected buffer */
    ITM_send32Polling(LoggerITM_STIM_TRACE, size);
    /* Send out the actual data */
    ITM_sendBufferAtomic(LoggerITM_STIM_TRACE, (const char *)data, size);

    /* enable interrupts */
    HwiP_restore(key);
}


/*
 *  ======== LoggerITM_finalize ========
 */
void LoggerITM_finalize(void)
{
    /* disable interrupts */
    uint32_t key = HwiP_disable();

    ITM_close();
    isOpen = false;

    /* enable interrupts */
    HwiP_restore(key);
}
