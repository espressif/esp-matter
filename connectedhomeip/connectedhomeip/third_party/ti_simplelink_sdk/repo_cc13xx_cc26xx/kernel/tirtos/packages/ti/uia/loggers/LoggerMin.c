/*
 * Copyright (c) 2013-2014, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
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
 *  ======== LoggerMin.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Log.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/uia/runtime/EventHdr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/loggers/LoggerMin.h>

#include <string.h>  /* for memcpy */

#include <package/internal/LoggerMin.xdc.h>

#define ENABLE 1

#define HDR_OFFSET_IN_WORDS     1

#define BYTES_IN_EVENTWORD 4
#define MAU_TO_BITS32(mau)      ((mau) / sizeof(Bits32))
#define BYTES_PER_MAU           (4 / sizeof(Bits32))
/*
 *  Don't include the timestamp size in here in these defines.  The timestamp
 *  size will be added on in LoggerMin_write().  This reduces the code size.
 */
#define NUM_BYTES_WRITE0        (BYTES_IN_EVENTWORD * 2)
#define NUM_BYTES_WRITE1        (NUM_BYTES_WRITE0 + BYTES_IN_EVENTWORD)
#define NUM_BYTES_WRITE2        (NUM_BYTES_WRITE1 + BYTES_IN_EVENTWORD)
#define NUM_BYTES_WRITE4        (NUM_BYTES_WRITE2 + 2 * BYTES_IN_EVENTWORD)
#define NUM_BYTES_WRITE8        (NUM_BYTES_WRITE4 + 4 * BYTES_IN_EVENTWORD)

/*
 *  ======== LoggerMin_Instance_init =========
 *  Currently not implemented since we cannot dynamically
 *  plug in loggers.
 */
Void LoggerMin_Instance_init(LoggerMin_Object *obj,
        const LoggerMin_Params *prms)
{
}

/*
 *  ======== LoggerMin_Module_startup ========
 */
Int LoggerMin_Module_startup(Int phase)
{
    LoggerMin_module->droppedEvents = 0;

    /* JTAG Stop Mode case: iniitalize the packetBuffer array*/
    LoggerMin_initBuffer((Ptr)LoggerMin_module->packetBuffer, 0);

    /* Note: LoggerMin_module pointers have been iniitalized in .xs file */

    return (Startup_DONE);
}

/*
 *  ======== LoggerMin_write0 =========
 */
Void LoggerMin_write0(LoggerMin_Object *obj, Log_Event evt, Types_ModuleId mid)
{
    LoggerMin_write(evt, mid, NUM_BYTES_WRITE0, 0, 0, 0, 0, 0, 0, 0, 0);
}

/*
 *  ======== LoggerMin_write1 =========
 */
Void LoggerMin_write1(LoggerMin_Object *obj, Log_Event evt, Types_ModuleId mid,
        IArg a1)
{
    LoggerMin_write(evt, mid, NUM_BYTES_WRITE1, a1, 0, 0, 0, 0, 0, 0, 0);
}

/*
 *  ======== LoggerMin_write2 =========
 */
Void LoggerMin_write2(LoggerMin_Object *obj, Log_Event evt, Types_ModuleId mid,
        IArg a1, IArg a2)
{
    LoggerMin_write(evt, mid, NUM_BYTES_WRITE2, a1, a2, 0, 0, 0, 0, 0, 0);
}

/*
 *  ======== LoggerMin_write4 =========
 */
Void LoggerMin_write4(LoggerMin_Object *obj, Log_Event evt, Types_ModuleId mid,
        IArg a1, IArg a2, IArg a3, IArg a4)
{
    LoggerMin_write(evt, mid, NUM_BYTES_WRITE4, a1, a2, a3, a4, 0, 0, 0, 0);
}


/*
 *  ======== LoggerMin_write8 =========
 */
Void LoggerMin_write8(LoggerMin_Object *obj, Log_Event evt, Types_ModuleId mid,
        IArg a1, IArg a2, IArg a3, IArg a4, IArg a5, IArg a6, IArg a7, IArg a8)
{
    LoggerMin_write(evt, mid, NUM_BYTES_WRITE8, a1, a2, a3, a4,
            a5, a6, a7, a8);
}

#if ENABLE
/*
 *  LoggerMin_enable and LoggerMin_disable will always be pulled into
 *  the executable, since they are referenced in the logger's function
 *  table, so we may want to leave these in the .xdt file.
 */

/*
 *  ======== LoggerMin_disable ========
 */
Bool LoggerMin_disable(LoggerMin_Object *obj)
{
    UInt key;
    Bool prev;

    key = Hwi_disable();

    prev = LoggerMin_module->enabled;
    LoggerMin_module->enabled = FALSE;

    Hwi_restore(key);

    return (prev);
}


/*
 *  ======== LoggerMin_enable ========
 */
Bool LoggerMin_enable(LoggerMin_Object *obj)
{
    UInt key;
    Bool prev;

    key = Hwi_disable();

    prev = LoggerMin_module->enabled;
    LoggerMin_module->enabled = TRUE;

    Hwi_restore(key);

    return (prev);
}

#endif

/*
 *  ======== LoggerMin_getContents =========
 *  Fills buffer that is passed in with unread data, up to size MAU's in
 *  length.  Used for testing.
 */
Bool LoggerMin_getContents(LoggerMin_Object *obj, Ptr pMemBlock,
        SizeT maxSize, SizeT *cpSize)
{
    UIAPacket_Hdr *pPktHdr = NULL;
    Char *start;
    Char *writePtr;
    Char *readPtr;
    Char *nextReadPtr;
    Char *end;
    UInt32 len, len2;
    UInt32 prevLen;
    SizeT packetLength = 0;

    *cpSize = 0;

    if (LoggerMin_isEmpty(obj)) {
        return (FALSE);
    }

    pPktHdr = (UIAPacket_Hdr *)LoggerMin_module->start;

    /* Packet length in MAUs */
    packetLength = UIAPacket_getMinEventLength(pPktHdr) / BYTES_PER_MAU;

    start = (Char *)LoggerMin_module->start + sizeof(UInt32);

    /*
     *  'end' points to the dummy event header at the end of the packet in the
     *  case where the buffer has wrapped.  Otherwise 'end' points to the start
     *  of the data, since packetLength is 0 if the buffer has not wrapped.
     */
    end = (Char *)LoggerMin_module->start + packetLength;
    writePtr = (Char *)LoggerMin_module->write;

    len = writePtr - start;

    if ((packetLength > maxSize) || (len > maxSize)) {
        /* Buffer is not big enough to hold the data */
        return (FALSE);
    }

    if (writePtr >= end) {
        /*
         *  Buffer has not wrapped yet, or it has wrapped up to or beyond the
         *  end of the previous packet.  Only copy up to the write pointer.
         */
        memcpy(pMemBlock, start, len);
        *cpSize = len;
    }
    else {
        /*
         *  Find the oldest record not overwritten yet. 'readPtr' will
         *  be set to the beginning of this record.
         */

        /* Points to dummy Event header at end of packet */
        nextReadPtr = readPtr = end;

        while (nextReadPtr > writePtr) {
            readPtr = nextReadPtr;
            prevLen = EventHdr_getPrevLength(*((UInt32 *)readPtr));
            nextReadPtr -= (prevLen / BYTES_PER_MAU);

            if ((EventHdr_getLength(*((UInt32 *)(nextReadPtr))) != prevLen) ||
                (prevLen == 0)) {
                /* nextReadPtr may be overwritten so length is invalid */
                break;
            }
        }

        len = writePtr - start;
        if (readPtr < end) {
            len2 = end - readPtr;
            memcpy(pMemBlock, readPtr, len2);
            memcpy((Char *)pMemBlock + len2, start, len);
            *cpSize = len + len2;
        }
        else {
            memcpy(pMemBlock, start, len);
            *cpSize = len;
        }
    }

    return (TRUE);
}

/*
 * ======== LoggerMin_isEmpty ========
 * Returns TRUE if the buffer has no data
 */
Bool LoggerMin_isEmpty(LoggerMin_Object *obj)
{
    UIAPacket_Hdr *pPktHdr = NULL;
    SizeT packetLength = 0;
    Bool result;

    pPktHdr = (UIAPacket_Hdr *)LoggerMin_module->start;
    packetLength = UIAPacket_getMinEventLength(pPktHdr);

    if ((LoggerMin_module->write == (LoggerMin_module->start + 1)) &&
            packetLength == 0) {
        result = TRUE;
    }
    else {
        result = FALSE;
    }

    return (result);
}
