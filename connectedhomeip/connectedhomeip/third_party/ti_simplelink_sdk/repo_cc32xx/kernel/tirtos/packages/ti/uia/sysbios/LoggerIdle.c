/*
 * Copyright (c) 2013-2019, Texas Instruments Incorporated
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
 * */

/*
 *  ======== LoggerIdle.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/uia/runtime/EventHdr.h>

#include <package/internal/LoggerIdle.xdc.h>

#include <string.h> /* For memcpy */

/*
 *  Number of words written to buffer for each event, without timestamp
 */
#define WRITE0_IN_BITS32 2
#define WRITE1_IN_BITS32 3
#define WRITE2_IN_BITS32 4
#define WRITE4_IN_BITS32 6
#define WRITE8_IN_BITS32 10

#define TIMESTAMP_IN_BITS32 2

#define BITS32_TO_BITS8(bits32) ((bits32) * 4)

/*
 *  ======== LoggerIdle_idleWriteEvent =========
 *  Idle function that sends one log event to the user's transport function.
 */
Void LoggerIdle_idleWriteEvent()
{
    UInt key;
    Int lengthInBytes, mausWritten;
    static Int lengthInMAU = 0;
    static Int remainder = 0;

    if (remainder > 0) {
        mausWritten = LoggerIdle_module->loggerFxn(
            (UChar *)LoggerIdle_module->tempBuffer + (lengthInMAU - remainder),
            remainder);

        remainder -= mausWritten;
    }
    else if (!LoggerIdle_module->empty) {

        key = Hwi_disable();

        lengthInBytes = EventHdr_getLength(*LoggerIdle_module->bufferRead);
        lengthInMAU = lengthInBytes / (4 / sizeof(UInt32));

        memcpy((Char *)LoggerIdle_module->tempBuffer,
                (Char *)LoggerIdle_module->bufferRead, lengthInMAU);

        LoggerIdle_module->bufferRead =
            (UInt32 *)((Char *)LoggerIdle_module->bufferRead + lengthInMAU);

        if (LoggerIdle_module->bufferRead > LoggerIdle_module->bufferPad) {
            LoggerIdle_module->bufferRead = LoggerIdle_module->idleBuffer;
        }

        /* If buffer is empty, set flag to true */
        if (LoggerIdle_module->bufferRead == LoggerIdle_module->bufferWrite) {
            LoggerIdle_module->empty = TRUE;
        }

        Hwi_restore(key);

        /* Send the buffer to the configured transport function */
        remainder = lengthInMAU;

        mausWritten = LoggerIdle_module->loggerFxn(
            (UChar *)LoggerIdle_module->tempBuffer + (lengthInMAU - remainder),
            remainder);

        remainder -= mausWritten;
    }
}


/*
 *  ======== LoggerIdle_Instance_init =========
 */
Void LoggerIdle_Instance_init(LoggerIdle_Object *obj,
    const LoggerIdle_Params *prms)
{
}

/*
 *  ======== LoggerIdle_disable ========
 */
Bool LoggerIdle_disable(LoggerIdle_Object *obj)
{
    UInt key = Hwi_disable();
    Bool prev = LoggerIdle_module->enabled;

    LoggerIdle_module->enabled = FALSE;
    Hwi_restore(key);

    return (prev);
}

/*
 *  ======== LoggerIdle_enable ========
 */
Bool LoggerIdle_enable(LoggerIdle_Object *obj)
{
    UInt key = Hwi_disable();
    Bool prev = LoggerIdle_module->enabled;

    LoggerIdle_module->enabled = TRUE;
    Hwi_restore(key);

    return (prev);
}

/*
 *  ======== LoggerIdle_flush ========
 */
Void LoggerIdle_flush()
{
    while (!LoggerIdle_module->empty) {
        LoggerIdle_idleWriteEvent();
    }
}

/*
 *  ======== write0 =========
 */
Void LoggerIdle_write0(LoggerIdle_Object *obj, Log_Event evt,
                      Types_ModuleId mid)
{
    LoggerIdle_write(evt, mid, WRITE0_IN_BITS32,
            0, 0, 0, 0, 0, 0, 0, 0);
}

/*
 *  ======== write1 =========
 */
Void LoggerIdle_write1(LoggerIdle_Object *obj, Log_Event evt,
                      Types_ModuleId mid, IArg a1)
{
    LoggerIdle_write(evt, mid, WRITE1_IN_BITS32,
            a1, 0, 0, 0, 0, 0, 0, 0);
}

/*
 *  ======== write2 =========
 */
Void LoggerIdle_write2(LoggerIdle_Object *obj, Log_Event evt,
                      Types_ModuleId mid, IArg a1, IArg a2)
{
    LoggerIdle_write(evt, mid, WRITE2_IN_BITS32,
            a1, a2, 0, 0, 0, 0, 0, 0);
}

/*
 *  ======== write4 =========
 */
Void LoggerIdle_write4(LoggerIdle_Object *obj, Log_Event evt,
                      Types_ModuleId mid, IArg a1, IArg a2, IArg a3, IArg a4)
{
    LoggerIdle_write(evt, mid, WRITE4_IN_BITS32,
            a1, a2, a3, a4, 0, 0, 0, 0);
}

/*
 *  ======== write8 =========
 */
Void LoggerIdle_write8(LoggerIdle_Object *obj, Log_Event evt,
                      Types_ModuleId mid, IArg a1, IArg a2, IArg a3, IArg a4,
                      IArg a5, IArg a6, IArg a7, IArg a8)
{
    LoggerIdle_write(evt, mid, WRITE8_IN_BITS32,
            a1, a2, a3, a4, a5, a6, a7, a8);
}
