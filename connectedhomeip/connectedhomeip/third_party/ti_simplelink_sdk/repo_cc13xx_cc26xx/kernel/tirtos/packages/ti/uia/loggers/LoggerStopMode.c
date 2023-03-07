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
 *  ======== LoggerStopMode.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Types.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/uia/runtime/EventHdr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/IUIATransfer.h>
#include <ti/uia/runtime/QueueDescriptor.h>

#ifdef xdc_target__isaCompatible_64P
#include <c6x.h>
#else
#define DNUM 0
#endif

#include "package/internal/LoggerStopMode.xdc.h"

#ifdef xdc_target__bigEndian
#define LoggerStopMode_ENDIANNESS UIAPacket_PayloadEndian_BIG
#else
#define LoggerStopMode_ENDIANNESS UIAPacket_PayloadEndian_LITTLE
#endif

#define LoggerStopMode_DUMMY_LEN 0
#define LoggerStopMode_DUMMY_SEQ 0
#define LoggerStopMode_PRIORITY 0

#if defined(xdc_target__isaCompatible_v8A)
/* 64-bit target */
#define HDR_OFFSET_IN_WORDS 2        /* Size of the UIA header */
#define BYTES_IN_EVENTWORD  8
#else
#define HDR_OFFSET_IN_WORDS 4        /* Size of the UIA header */
#define BYTES_IN_EVENTWORD  4
#endif

#define TIMESTAMP_WORDS ((LoggerStopMode_isTimestampEnabled) ? 2 : 0)

/* Timestamp is not included in this size */
#define WRITE8_WORDS 10



/*
 *  ======== LoggerStopMode_flush =========
 *  If there is data in the buffer, set the packet length
 *  and reset the write pointer back to the beginning
 *  of the buffer.
 */
Void LoggerStopMode_flush(LoggerStopMode_Object *obj)
{
    /* If there is data in the buffer */
    if (obj->write > obj->buffer + HDR_OFFSET_IN_WORDS) {
        /* Set UIA packet length and sequence number */
        UIAPacket_setEventLength((UIAPacket_Hdr*)obj->buffer,
                (Bits32)((obj->write - obj->buffer) * BYTES_IN_EVENTWORD));

        /* Update write to beginning of buffer */
        obj->write = obj->buffer + HDR_OFFSET_IN_WORDS;

        UIAPacket_setSequenceCounts((UIAPacket_Hdr*)obj->buffer,
                obj->pktSequenceNum, obj->eventSequenceNum);

        ((QueueDescriptor_Header *)obj->hdr)->partialPacketWritePtr =
                (Bits32 *)obj->write;
    }
}

/*
 *  ======== LoggerStopMode_Module_startup ========
 */
Int LoggerStopMode_Module_startup(Int phase)
{
    LoggerStopMode_Object *obj;
    Int i;
    UInt32 coreNum = 0;

#ifdef xdc_target__isaCompatible_64P
    if (LoggerStopMode_numCores > 1) {
        coreNum = (UInt32)DNUM;
    }
#endif

    for (i = 0; i < LoggerStopMode_Object_count(); i++) {
        obj = LoggerStopMode_Object_get(NULL, i);

        /*
	 *  For single image executables that run on multiple cores,
	 *  set buffer and QueueDescriptor hdr based on core number
	 */
        obj->buffer = (UArg *)(obj->packetArray + coreNum * (obj->bufSize));

        obj->hdr = (Ptr)(obj->hdr + coreNum * sizeof(QueueDescriptor_Header));

        LoggerStopMode_initQueueDescriptor(obj, Module__MID);
        LoggerStopMode_initBuffer(obj, (Ptr)obj->buffer, (UInt16)DNUM);

        LoggerStopMode_reset(obj);
    }

    return (Startup_DONE);
}

/*
 *  ======== LoggerStopMode_Instance_init =========
 *  Currently not implemented since we cannot dynamically
 *  plug in loggers.
 */
Void LoggerStopMode_Instance_init(LoggerStopMode_Object *obj,
        const LoggerStopMode_Params *prms)
{
    obj->bufSize = prms->bufSize;

    obj->buffer = (UArg *)(obj->packetArray + DNUM * obj->bufSize);
    LoggerStopMode_initBuffer(obj, (Ptr)obj->buffer, (UInt16)DNUM);

    LoggerStopMode_reset(obj);
}

/*
 *  ======== LoggerStopMode_disable ========
 */
Bool LoggerStopMode_disable(LoggerStopMode_Object *obj)
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
 *  ======== LoggerStopMode_enable ========
 */
Bool LoggerStopMode_enable(LoggerStopMode_Object *obj)
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
SizeT LoggerStopMode_getBufSize(LoggerStopMode_Object *obj)
{
    return (obj->bufSize);
}

/*
 * ======== getNumDropped =========
 * returns the number of dropped events for this logger instance
 */
Int LoggerStopMode_getNumDropped(LoggerStopMode_Object *obj)
{
    return (obj->droppedEvents);
}

/*
 *  ======== LoggerStopMode_getInstanceId ========
 */
UInt16 LoggerStopMode_getInstanceId(LoggerStopMode_Object *obj)
{
    return (obj->instanceId);
}

/*
 *  ======== LoggerStopMode_getPriority ========
 *  NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
IUIATransfer_Priority LoggerStopMode_getPriority(LoggerStopMode_Object *obj)
{
    return (IUIATransfer_Priority_STANDARD);
}

/*
 *  ======== LoggerStopMode_setPriority ========
 *  NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
Void LoggerStopMode_setPriority(LoggerStopMode_Object *obj,
        IUIATransfer_Priority priority)
{
}

/*
 *  ======== LoggerStopMode_getMaxLength ========
 */
SizeT LoggerStopMode_getMaxLength(LoggerStopMode_Object *obj)
{
    return (obj->maxEventSize);
}

/*
 *  ======== LoggerStopMode_reset ========
 */
Void LoggerStopMode_reset(LoggerStopMode_Object *obj)
{
    obj->enabled = TRUE;
    obj->pktSequenceNum = 0;
    obj->eventSequenceNum = 0;
    obj->numBytesInPrevEvent = 0;

    obj->write = (UArg *)((UArg *)obj->buffer + HDR_OFFSET_IN_WORDS);

    /*
     *  Mark the end of the buffer as the point where
     *  a write8 will still fit into the buffer if
     *  writePtr is end - 1.  Note that we need to
     *  subtract one word for the dummy event header
     *  containing the size of the previous event.
     */
    obj->end = obj->buffer + (obj->bufSize / sizeof(UArg)) -
        (WRITE8_WORDS + TIMESTAMP_WORDS) - 1;
}

/*
 * ======== LoggerStopMode_getTransferType ========
 *  Returns whether the logger events can be sent over lossy transports or
 *  requires reliable transport
 */
IUIATransfer_TransferType LoggerStopMode_getTransferType(
    LoggerStopMode_Object *obj)
{
    return (IUIATransfer_TransferType_LOSSY);
}

/*
 *  ======== LoggerStopMode_initBuffer ========
 */
Void LoggerStopMode_initBuffer(LoggerStopMode_Object *obj, Ptr buffer,
        UInt16 src)
{
    UIAPacket_initEventRecHdr((ti_uia_runtime_UIAPacket_Hdr *)buffer,
            LoggerStopMode_ENDIANNESS,
            LoggerStopMode_DUMMY_LEN,
            LoggerStopMode_DUMMY_SEQ,
            LoggerStopMode_PRIORITY,
            LoggerStopMode_Module_id(),
            obj->instanceId,
            UIAPacket_HOST, src);
}

/*
 *  ======== LoggerStopMode_initQueueDescriptor ========
 */
Void LoggerStopMode_initQueueDescriptor(LoggerStopMode_Object *obj,
        Types_ModuleId mid)
{
    QueueDescriptor_Header *pHdr;

    Assert_isTrue(obj->hdr != NULL, NULL);

    pHdr = (QueueDescriptor_Header* )obj->hdr;
    pHdr->queueSizeInMAUs = obj->bufSize;
    pHdr->instanceId = obj->instanceId;
    pHdr->next = NULL;
    pHdr->ownerModuleId = mid;
    pHdr->numDroppedCtrAdrs = &(obj->droppedEvents);
    pHdr->priority = LoggerStopMode_getPriority(obj);
    pHdr->queueStartAdrs = (Bits32*)obj->buffer;
    pHdr->readPtr = (Bits32*)obj->buffer;
    pHdr->writePtr = (Bits32*)obj->buffer;
    pHdr->queueType = QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_STOPMODE;
    pHdr->structSize = sizeof(QueueDescriptor_Header);
    pHdr->partialPacketWritePtr = (Bits32*)obj->buffer;
    QueueDescriptor_addToList((QueueDescriptor_Header *)obj->hdr);
}

/*
 * ======== LoggerStopMode_getContents =========
 * This function is not called when using LoggerStopMode.
 */
Bool LoggerStopMode_getContents(LoggerStopMode_Object *obj, Ptr pMemBlock,
        SizeT maxSize, SizeT *cpSize)
{
    return (FALSE);

}

/*
 * ======== LoggerStopMode_isEmpty ========
 * Returns TRUE if the transfer buffer has no unread data
 */
Bool LoggerStopMode_isEmpty(LoggerStopMode_Object *obj)
{
    Bool result;

    result = (obj->write == (obj->buffer + HDR_OFFSET_IN_WORDS)) ?
        TRUE : FALSE;

    return (result);
}
