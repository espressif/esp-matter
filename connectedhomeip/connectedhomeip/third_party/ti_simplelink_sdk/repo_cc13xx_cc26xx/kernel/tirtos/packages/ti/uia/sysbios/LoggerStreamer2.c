/*
 * Copyright (c) 2013-2018, Texas Instruments Incorporated
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
 *
 */

/*
 *  ======== LoggerStreamer2.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Startup.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/uia/runtime/EventHdr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/IUIATransfer.h>
#include <xdc/runtime/Log.h>


#ifdef xdc_target__isaCompatible_64P
#include <c6x.h>
#endif

#include <package/internal/LoggerStreamer2.xdc.h>

#if xdc_target__sizeof_Ptr == 8
#define HDR_OFFSET_IN_PTRS   2  /* Size of the UIA header in Ptrs */
#define BYTES_IN_EVENTWORD   8  /* Number of bytes in UArg */
#define NUM_WRITE8_WORDS     9  /* 8 UArg + 2 32-bit event header (no TS) */
#define NUM_WRITE8_WORDS_TS 10  /* 8 UArgs + 4 32-bit event header (with TS) */
#else
#define HDR_OFFSET_IN_PTRS   4  /* Size of the UIA header in Ptrs */
#define BYTES_IN_EVENTWORD   4  /* Number of bytes in UArg */
#define NUM_WRITE8_WORDS    10  /* 8 UArgs + 4 32-bit event headet (no TS) */
#define NUM_WRITE8_WORDS_TS 12  /* 8 UArgs + 4 32-bit event headet (with TS) */
#endif

#define BYTES_IN_WORD 4
#define MAU_TO_BITS32(mau)          ((mau) / sizeof(Bits32))

#define LoggerStreamer2_uiaPacketSequence \
        ti_uia_sysbios_LoggerStreamer2_uiaPacketSequence


#define LoggerStreamer2_ENDIANNESS ti_uia_sysbios_LoggerStreamer2_ENDIANNESS
#define LoggerStreamer2_DUMMY_LEN ti_uia_sysbios_LoggerStreamer2_DUMMY_LEN
#define LoggerStreamer2_DUMMY_SEQ ti_uia_sysbios_LoggerStreamer2_DUMMY_SEQ
#define LoggerStreamer2_PRIORITY ti_uia_sysbios_LoggerStreamer2_PRIORITY
#define LoggerStreamer2_INSTANCE_ID ti_uia_sysbios_LoggerStreamer2_INSTANCE_ID


/*
 *  write8 event size.  Set by prime function at runtime to 10 if timestamps
 *  are disabled.
 */
static Int numWrite8Words = 12;  // TODO: Generate in the .xdt file?

extern UInt16 LoggerStreamer2_uiaPacketSequence;

/*
 *  ======== LoggerStreamer2_flush =========
 *  If there is data in the buffer, call the exchange function.
 */
Void LoggerStreamer2_flush(LoggerStreamer2_Object *obj)
{
    UInt key;

    key = Hwi_disable();

    /* If there is data in the buffer */
    if (obj->write > obj->buffer + HDR_OFFSET_IN_PTRS) {
        /* Set UIA packet length and sequence number */
        UIAPacket_setEventLength((UIAPacket_Hdr*)obj->buffer,
                (Bits32)((obj->write - obj->buffer) * BYTES_IN_EVENTWORD));
        UIAPacket_setSequenceCount((UIAPacket_Hdr*)obj->buffer,
                LoggerStreamer2_uiaPacketSequence);

        /*
         *  When the application calls flush there will always be empty space
         *  at the end of the buffer. To let UIA know it should ignore this we
         *  add a 32 bit Invalid UIA header with the length of the empty space.
         */
        UIAPacket_setInvalidHdr(obj->write,
                (obj->end + numWrite8Words - obj->write) * BYTES_IN_EVENTWORD);

        /* Set the module write ptr to NULL to prevent log calls in exchange */
        if (LoggerStreamer2_testForNullWrPtr) {
            obj->write = NULL;
        }

        /* Send filled buffer to exchange function */
        obj->buffer = (UArg *)obj->exchangeFxn(obj, (Ptr)obj->buffer);

        /* Update ptrs to new buffer */
        obj->write = obj->buffer + HDR_OFFSET_IN_PTRS;
        obj->end = obj->buffer + (obj->bufSize / sizeof(UArg)) -
                numWrite8Words;
        LoggerStreamer2_uiaPacketSequence++;
    }

    Hwi_restore(key);
}

/*
 *  ======== LoggerStreamer2_prime =========
 */
Bool LoggerStreamer2_prime(LoggerStreamer2_Object *obj, Ptr buffer)
{
    if (obj->primeStatus == FALSE) {
        obj->buffer = (UArg *)buffer;

        obj->write = (UArg *)buffer + HDR_OFFSET_IN_PTRS;

        /* Subtract max number of words that can be written to mark the end */
        obj->end = (UArg *)buffer + (obj->bufSize / sizeof(UArg)) -
                numWrite8Words;
        obj->primeStatus = TRUE;
        return (TRUE);
    }
    return (FALSE);
}

/*
 *  ======== LoggerStreamer2_Module_startup ========
 */
Int LoggerStreamer2_Module_startup(Int phase)
{
    LoggerStreamer2_Object *obj;
    Int i;

    /* Set the maximum event size used to set the end pointer */
    numWrite8Words = (LoggerStreamer2_isTimestampEnabled) ?
            NUM_WRITE8_WORDS_TS : NUM_WRITE8_WORDS;

    for (i = 0; i < LoggerStreamer2_Object_count(); i++) {
        obj = LoggerStreamer2_Object_get(NULL, i);

        if (obj->primeFxn != NULL) {
            obj->buffer = obj->primeFxn(obj);
            Assert_isTrue((obj->buffer != NULL),
                    LoggerStreamer2_A_invalidBuffer);
            LoggerStreamer2_reset(obj);
        }
    }
    return (Startup_DONE);
}

/*
 *  ======== LoggerStreamer2_Instance_init =========
 *  Currently not implemented since we cannot dynamically
 *  plug in loggers.
 */
Void LoggerStreamer2_Instance_init(LoggerStreamer2_Object *obj,
        const LoggerStreamer2_Params *prms)
{
    obj->bufSize = prms->bufSize;
    obj->primeFxn = prms->primeFxn;
    obj->exchangeFxn = prms->exchangeFxn;
    obj->context = prms->context;
    obj->instanceId = prms->instanceId;

    if (obj->primeFxn != NULL) {
        obj->buffer = (UArg *)obj->primeFxn(obj);
        Assert_isTrue((obj->buffer != NULL), LoggerStreamer2_A_invalidBuffer);
        LoggerStreamer2_reset(obj);
    }
}

/*
 *  ======== LoggerStreamer2_disable ========
 */
Bool LoggerStreamer2_disable(LoggerStreamer2_Object *obj)
{
    UInt key;
    Bool prev;

    key = Hwi_disable();

    prev = obj->enabled;
    obj->enabled = FALSE;

    Hwi_restore(key);

    return (prev);
}

/*
 *  ======== LoggerStreamer2_enable ========
 */
Bool LoggerStreamer2_enable(LoggerStreamer2_Object *obj)
{
    UInt key;
    Bool prev;

    key = Hwi_disable();

    prev = obj->enabled;
    obj->enabled = TRUE;

    Hwi_restore(key);

    return (prev);
}

/*
 * ======== getBufSize =========
 * Returns the configured buffer size.
 */
SizeT LoggerStreamer2_getBufSize(LoggerStreamer2_Object *obj)
{
    return (obj->bufSize);
}

/*
 * ======== getContext =========
 * Returns the log's context.
 */
UArg LoggerStreamer2_getContext(LoggerStreamer2_Object *obj)
{
    return (obj->context);
}

/*
 * ======== getNumDropped =========
 * returns the number of dropped events for this logger instance
 * TODO getStats instead? Add to IUIATransfer?
 */
Int LoggerStreamer2_getNumDropped(LoggerStreamer2_Object *obj)
{
    return (obj->droppedEvents);
}

/*
 *  ======== LoggerStreamer2_getInstanceId ========
 */
UInt16 LoggerStreamer2_getInstanceId(LoggerStreamer2_Object *obj)
{
    return (obj->instanceId);
}

/*
 *  ======== LoggerStreamer2_getPriority ========
 *  NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
IUIATransfer_Priority LoggerStreamer2_getPriority(LoggerStreamer2_Object *obj)
{
    return (IUIATransfer_Priority_STANDARD);
}

/*
 *  ======== setContext =========
 *  Set the log's context.
 */
Void LoggerStreamer2_setContext(LoggerStreamer2_Object *obj, UArg context)
{
    obj->context = context;
}

/*
 *  ======== LoggerStreamer2_setPriority ========
 *  NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
Void LoggerStreamer2_setPriority(LoggerStreamer2_Object *obj,
        IUIATransfer_Priority priority)
{
}

/*
 *  ======== LoggerStreamer2_getMaxLength ========
 */
SizeT LoggerStreamer2_getMaxLength(LoggerStreamer2_Object *obj)
{
    return (obj->maxEventSize);
}

/*
 *  ======== LoggerStreamer2_reset ========
 */
Void LoggerStreamer2_reset(LoggerStreamer2_Object *obj)
{
    obj->enabled = TRUE;
    obj->seqNumber = 0;

    Assert_isTrue((obj->buffer != NULL), LoggerStreamer2_A_invalidBuffer);

    obj->write = obj->buffer + HDR_OFFSET_IN_PTRS;
    obj->end = obj->buffer + (obj->bufSize / sizeof(UArg)) - numWrite8Words;
}

/*
 * ======== LoggerStreamer2_getTransferType ========
 *  Returns whether the logger events can be sent over lossy transports or
 *  requires reliable transport
 */
IUIATransfer_TransferType LoggerStreamer2_getTransferType(
        LoggerStreamer2_Object *obj)
{
    return (IUIATransfer_TransferType_LOSSY);
}

/*
 * ======== LoggerStreamer2_getContents =========
 * Fills buffer that is passed in with unread data, up to size MAU's in length.
 * NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
Bool LoggerStreamer2_getContents(LoggerStreamer2_Object *obj, Ptr hdrBuf,
        SizeT maxSize, SizeT *cpSize)
{
    return (FALSE);
}

/*
 *  ======== LoggerStreamer2__initBuffer ========
 */
Void LoggerStreamer2_initBuffer(LoggerStreamer2_Object *obj, Ptr buffer,
        UInt16 src)
{
    UIAPacket_initEventRecHdr((ti_uia_runtime_UIAPacket_Hdr *)buffer,
            LoggerStreamer2_ENDIANNESS,
            LoggerStreamer2_DUMMY_LEN,
            LoggerStreamer2_DUMMY_SEQ,
            LoggerStreamer2_PRIORITY,
            LoggerStreamer2_Module_id(),
            obj->instanceId,
            ti_uia_runtime_UIAPacket_HOST, src);
}

/*
 * ======== LoggerStreamer2_isEmpty ========
 * Returns true if the transfer buffer has no unread data
 */
Bool LoggerStreamer2_isEmpty(LoggerStreamer2_Object *obj)
{
    Bool result;

    result = (obj->write == (obj->buffer + HDR_OFFSET_IN_PTRS)) ?
        TRUE : FALSE;

    return (result);
}
