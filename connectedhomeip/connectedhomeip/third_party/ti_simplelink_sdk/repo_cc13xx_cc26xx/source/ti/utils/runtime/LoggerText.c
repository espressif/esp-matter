/*
 * Copyright (c) 2019 Texas Instruments Incorporated - http://www.ti.com
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
 *  ======== LoggerText.c ========
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>

#include <ti/sysbios/hal/Hwi.h>

#include "LoggerText.h"

#define LoggerText_FULL -1


/*
 *  ======== ti_utils_runtime_config.c ========
 */
extern const int LoggerText_count;
extern LoggerText_Instance LoggerText_config[];


/*
 *  ======== LoggerText_handle ========
 */
LoggerText_Handle LoggerText_handle(int idx)
{
    LoggerText_Handle hndl = NULL;

    if ((0 <= idx) && (idx < LoggerText_count)) {
        hndl = &LoggerText_config[idx];
    }

    return (hndl);
}

/*
 *  ======== LoggerText_write ========
 */
void LoggerText_write(LoggerText_Handle hndl, char *text)
{
    UInt key;
    uint32_t serial;
    LoggerText_Record *rec;

    /* disable interrupts */
    key = Hwi_disable();

    /* increment serial even when full */
    serial = ++(hndl->serial);

    if (hndl->advance == LoggerText_FULL) {
        Hwi_restore(key);
        goto leave;
    }

    /* compute next available record */
    rec = hndl->curEntry;
    if (rec == hndl->endEntry) {
        if (hndl->advance == LoggerText_Type_CIRCULAR) {
            hndl->curEntry = hndl->store;
        }
        else {
            hndl->advance = LoggerText_FULL;
        }
    }
    else {
        hndl->curEntry = (LoggerText_Record *)
            ((char *)rec + sizeof(LoggerText_Header) +
            hndl->textLen * sizeof(char));
    }

    Timestamp_get64((Types_Timestamp64 *)&rec->hdr.ts_hi);

    /* enable interrupts */
    Hwi_restore(key);

    /* write data to record */
    rec->hdr.serial = serial;
    strncpy(rec->text, text, hndl->textLen);
    rec->text[hndl->textLen - 1] = '\0';

leave:
    return;
}
