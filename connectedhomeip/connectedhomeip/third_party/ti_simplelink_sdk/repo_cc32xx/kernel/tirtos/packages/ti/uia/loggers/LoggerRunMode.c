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
 *  ======== LoggerRunMode.c ========
 */
#include <xdc/std.h>

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/IFilterLogger.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Startup.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/uia/runtime/EventHdr.h>
#include <ti/uia/runtime/UIAPacket.h>
#include <ti/uia/runtime/IUIATransfer.h>
#include <ti/uia/runtime/QueueDescriptor.h>

#include <string.h>  /* for memcpy */

#ifdef xdc_target__isaCompatible_64P
#include <c6x.h>
#else
#define DNUM 0
#endif

#include <package/internal/LoggerRunMode.xdc.h>

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

#define BYTES_IN_INVALID_HDR 4

#define MAU_TO_BITS32(mau)          ((mau) / sizeof(Bits32))

#define bits32ToBits8(len) ((len) * 4)
#define bits32ToMau(len)   ((len) * sizeof(Bits32))

#ifdef xdc_target__bigEndian
#define LoggerRunMode_ENDIANNESS UIAPacket_PayloadEndian_BIG
#else
#define LoggerRunMode_ENDIANNESS UIAPacket_PayloadEndian_LITTLE
#endif

#define LoggerRunMode_DUMMY_LEN 0
#define LoggerRunMode_DUMMY_SEQ 0
#define LoggerRunMode_PRIORITY 0

/*
 *  write8 event size in 32-bit words.  Set by startup at
 *  runtime to 10 if timestamps are disabled.
 */
static Int numWrite8Words;


/*
 *  ======== LoggerRunMode_flush =========
 *  If there is data in the buffer, call the exchange function.
 */
Void LoggerRunMode_flush(LoggerRunMode_Object *obj)
{
    UInt key = Hwi_disable();

    /* If there is data in the buffer */
    if (obj->write > obj->buffer + HDR_OFFSET_IN_PTRS) {
        /* Set UIA packet length and sequence number */
        UIAPacket_setEventLength((UIAPacket_Hdr*)obj->buffer,
                (Bits32)((obj->write - obj->buffer) * BYTES_IN_EVENTWORD));

        /* Send filled buffer to exchange function */
        obj->buffer = (UArg *)LoggerRunMode_exchange(obj, (Ptr)obj->buffer,
                (Ptr)obj->write);

        /* Update ptrs to new buffer */
        obj->write = obj->buffer + HDR_OFFSET_IN_PTRS;
        obj->end = obj->buffer + obj->packetSize / sizeof(UArg) -
                numWrite8Words;
        UIAPacket_setSequenceCounts((UIAPacket_Hdr*)obj->buffer,
                obj->pktSequenceNum, obj->eventSequenceNum);
    }
    Hwi_restore(key);
}

/*
 *  ======== LoggerRunMode_prime ========
 */
Ptr LoggerRunMode_prime(LoggerRunMode_Object *obj)
{
    char *pkt = (char *)obj->buffer;
    int i;

    for (i = 0; i < obj->numPackets; i++) {
        LoggerRunMode_initBuffer(obj, pkt, DNUM);
        pkt += obj->packetSize;
    }
    return ((Ptr)(obj->buffer));
}

/*
 *  ======== LoggerRunMode_exchange ========
 *  Called when a packet is full.
 */
Ptr LoggerRunMode_exchange(LoggerRunMode_Object *obj, Ptr full,
                           Ptr lastWritePtr)
{
    char *start;
    char *end;
    char *packet = ((char *)full) + obj->packetSize;
    UInt32 *pPayload;
    UInt32 coreNum = 0;
    UInt32 len;

#ifdef xdc_target__isaCompatible_64P
    if (LoggerRunMode_numCores > 1) {
        coreNum = (UInt32)DNUM;
    }
#endif

    start = obj->packetArray + obj->bufSize * coreNum;
    end = start + obj->bufSize;

    if (packet >= end) {
        packet = start;
    }
    if ((Bits32 *)packet == ((QueueDescriptor_Header *)obj->hdr)->readPtr) {
        /* Re-use the most recently used packet, overwriting unread data */
        packet = full;
    }
    else {
        /*
         *  Check if there is garbage data at the end of the just-filled
         *  UIA packet.
         */
        if (obj->end + numWrite8Words - (UArg *)lastWritePtr > 0) {
            /*
             *  Overwrite the last event header containing the
             *  size of the previous event with an 'invalid packet header'.
             *  To let the host know it should ignore this, we
             *  add a 32 bit Invalid UIA header with the length of the
             *  empty space.
             */
            len = (UInt32)(obj->end + numWrite8Words - (UArg *)lastWritePtr);
            UIAPacket_setInvalidHdr(lastWritePtr, len);
        }

        /*
         *  Initialize the first word of the about-to-be-used packet payload
         *  to 0 to indicate that the packet contains stale data in case the
         *  target is halted before an event is written into the packet.
         */
        pPayload = (UInt32 *)(packet + sizeof(UIAPacket_Hdr));
        *pPayload = 0;
    }

    /*
     *  Ensure that the partialPacketWritePtr is always 'ahead' of or
     *  equal to the queue descriptor's write pointer in case the target
     *  is halted when the CPU is between the following two instructions
     */
    ((QueueDescriptor_Header *)obj->hdr)->partialPacketWritePtr =
            (Bits32 *)packet;
    ((QueueDescriptor_Header *)obj->hdr)->writePtr = (Bits32 *)packet;
    ((QueueDescriptor_Header *)obj->hdr)->partialPacketWritePtr =
            (Bits32 *)(packet + obj->packetSize);

    return ((Ptr)packet);
}

/*
 *  ======== LoggerRunMode_Module_startup ========
 */
Int LoggerRunMode_Module_startup(Int phase)
{
    LoggerRunMode_Object *obj;
    Int i;
    UInt32 coreNum = 0;

#ifdef xdc_target__isaCompatible_64P
    if (LoggerRunMode_numCores > 1) {
        coreNum = (UInt32)DNUM;
    }
#endif
    /* initialize the lastUploadTstamp to hold off uploading packets upon startup */
    LoggerRunMode_module->lastUploadTstamp = 0;
    LoggerRunMode_isUploadRequired();

    /* Set the maximum event size used to set the end pointer */
    numWrite8Words = (LoggerRunMode_isTimestampEnabled) ?
            NUM_WRITE8_WORDS_TS : NUM_WRITE8_WORDS;

    for (i = 0; i < LoggerRunMode_Object_count(); i++) {
        obj = LoggerRunMode_Object_get(NULL, i);
        /*
         *  For single image executables that run on multiple cores,
         *  set buffer and QueueDescriptor hdr based on core number
         */
        obj->buffer = (UArg *)(obj->packetArray + coreNum * (obj->bufSize));

        obj->hdr = (Ptr)(obj->hdr + coreNum * sizeof(QueueDescriptor_Header));

        LoggerRunMode_initQueueDescriptor(obj, Module__MID);

        obj->buffer = LoggerRunMode_prime(obj);
        LoggerRunMode_reset(obj);
    }
    return (Startup_DONE);
}

/*
 *  ======== LoggerRunMode_Instance_init =========
 *  Currently not implemented since we cannot dynamically
 *  plug in loggers.
 */
Void LoggerRunMode_Instance_init(LoggerRunMode_Object *obj,
        const LoggerRunMode_Params *prms)
{
    obj->bufSize = prms->bufSize;

    obj->buffer = (UArg *)LoggerRunMode_prime(obj);
    LoggerRunMode_reset(obj);
}

/*
 *  ======== LoggerRunMode_initQueueDescriptor ========
 */
Void LoggerRunMode_initQueueDescriptor(LoggerRunMode_Object *obj,
        Types_ModuleId mid)
{
    QueueDescriptor_Header* pHdr;

    Assert_isTrue(obj->hdr != NULL, NULL);

    pHdr = (QueueDescriptor_Header* )obj->hdr;
    pHdr->queueSizeInMAUs = obj->bufSize;
    pHdr->instanceId = obj->instanceId;
    pHdr->next = NULL;
    pHdr->ownerModuleId = mid;
    pHdr->numDroppedCtrAdrs = &(obj->droppedEvents);
    pHdr->priority = LoggerRunMode_getPriority(obj);
    pHdr->queueStartAdrs = (Bits32*)obj->buffer;
    pHdr->readPtr = (Bits32*)obj->buffer;
    pHdr->writePtr = (Bits32*)obj->buffer;
    pHdr->queueType = QueueDescriptor_QueueType_TOHOST_EVENT_UIAPACKET_ARRAY;
    pHdr->structSize = sizeof(QueueDescriptor_Header);
    pHdr->partialPacketWritePtr = (Bits32 *)(obj->buffer +
            obj->packetSize);
    QueueDescriptor_addToList((QueueDescriptor_Header *)obj->hdr);
}

/*
 *  ======== LoggerRunMode_disable ========
 */
Bool LoggerRunMode_disable(LoggerRunMode_Object *obj)
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
 *  ======== LoggerRunMode_enable ========
 */
Bool LoggerRunMode_enable(LoggerRunMode_Object *obj)
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
SizeT LoggerRunMode_getBufSize(LoggerRunMode_Object *obj)
{
    return (obj->bufSize);
}

/*
 * ======== getNumDropped =========
 * returns the number of dropped events for this logger instance
 */
Int LoggerRunMode_getNumDropped(LoggerRunMode_Object *obj)
{
    return (obj->droppedEvents);
}

/*
 *  ======== LoggerRunMode_getInstanceId ========
 */
UInt16 LoggerRunMode_getInstanceId(LoggerRunMode_Object *obj)
{
    return (obj->instanceId);
}

/*
 *  ======== LoggerRunMode_getPriority ========
 *  NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
IUIATransfer_Priority LoggerRunMode_getPriority(LoggerRunMode_Object *obj)
{
    return (IUIATransfer_Priority_STANDARD);
}

/*
 * ======== LoggerRunMode_isEmpty ========
 * Returns TRUE if the transfer buffer has no unread data
 */
Bool LoggerRunMode_isEmpty(LoggerRunMode_Object *obj)
{
    QueueDescriptor_Header *pQDHdr;
    UArg *readPtr;
    Bool result;

    pQDHdr = (QueueDescriptor_Header* )obj->hdr;
    readPtr = (UArg *)((Char *)(pQDHdr->readPtr) + sizeof(UIAPacket_Hdr));
    result = (readPtr == obj->write) ? TRUE : FALSE;

    return (result);
}

/*
 *  ======== LoggerRunMode_setPriority ========
 *  NOT IMPLEMENTED.  Included in order to satisfy Interface requirements
 */
Void LoggerRunMode_setPriority(LoggerRunMode_Object *obj,
        IUIATransfer_Priority priority)
{
}

/*
 *  ======== LoggerRunMode_getMaxLength ========
 */
SizeT LoggerRunMode_getMaxLength(LoggerRunMode_Object *obj)
{
    return (obj->maxEventSize);
}

/*
 *  ======== LoggerRunMode_reset ========
 */
Void LoggerRunMode_reset(LoggerRunMode_Object *obj)
{
    QueueDescriptor_Header *pHdr;
    UInt32 coreNum = 0;

#ifdef xdc_target__isaCompatible_64P
    if (LoggerRunMode_numCores > 1) {
        coreNum = (UInt32)DNUM;
    }
#endif

//    Assert_isTrue(obj->hdr != NULL, NULL);
    pHdr = (QueueDescriptor_Header* )obj->hdr;

    obj->enabled = TRUE;
    obj->pktSequenceNum = 0;
    obj->eventSequenceNum = 0;
    obj->numBytesInPrevEvent = 0;
    obj->buffer = (UArg *)(obj->packetArray + coreNum * (obj->bufSize));
    obj->write = obj->buffer + HDR_OFFSET_IN_PTRS;
    obj->end = obj->buffer + (obj->packetSize / sizeof(UArg)) - numWrite8Words;

    pHdr->readPtr = (Bits32*)obj->buffer;
    pHdr->writePtr = (Bits32*)obj->buffer;
    pHdr->partialPacketWritePtr = (Bits32 *)(obj->buffer + obj->packetSize);
}

/*
 * ======== LoggerRunMode_getTransferType ========
 *  Returns whether the logger events can be sent over lossy transports or
 *  requires reliable transport
 */
IUIATransfer_TransferType LoggerRunMode_getTransferType(
    LoggerRunMode_Object *obj)
{
    return (IUIATransfer_TransferType_LOSSY);
}


/*
 *  ======== LoggerRunMode_initBuffer ========
 */
Void LoggerRunMode_initBuffer(LoggerRunMode_Object *obj, Ptr buffer,
        UInt16 src)
{
    UInt32 *pPayload = (UInt32 *)((Char *)buffer + sizeof(UIAPacket_Hdr));

    UIAPacket_initEventRecHdr((UIAPacket_Hdr *)buffer,
            LoggerRunMode_ENDIANNESS,
            LoggerRunMode_DUMMY_LEN,
            LoggerRunMode_DUMMY_SEQ,
            LoggerRunMode_PRIORITY,
            LoggerRunMode_Module_id(),
            obj->instanceId,
            UIAPacket_HOST, src);

    *pPayload = 0;  /* initialize the first word of the payload to 0 */
}

/*
 *  ======== LoggerRunMode_getContents =========
 *  Fills buffer that is passed in with unread data, up to size MAU's in
 *  length.  Used by RTA service to get events to send to host via NDK or IPC.
 */
Bool LoggerRunMode_getContents(LoggerRunMode_Object *obj, Ptr pMemBlock,
        SizeT maxSize, SizeT *cpSize)
{
    Bool result = FALSE;
    QueueDescriptor_Header *pQDHdr;
    UIAPacket_Hdr *pPktHdr = NULL;
    Ptr myRdPtr = NULL;
    SizeT packetLength = 0;

    *cpSize = 0;

    if (obj->hdr != NULL){
        pQDHdr = (QueueDescriptor_Header* )obj->hdr;
        if (pQDHdr->readPtr != pQDHdr->writePtr) {
            pPktHdr = (UIAPacket_Hdr*)pQDHdr->readPtr;

            /* Check packet header */
            if (UIAPacket_getHdrType(pPktHdr) == UIAPacket_HdrType_EventPkt) {
                packetLength = UIAPacket_getEventLength(pPktHdr);

                /* Only copy data if the entire packet fits in the buffer */
                if ((packetLength <= maxSize) && (packetLength > 0)) {
                    memcpy(pMemBlock, pQDHdr->readPtr, packetLength);
                    *cpSize = packetLength;
                    myRdPtr = (Ptr)((Char *)pPktHdr + obj->packetSize);

                    if ((Char *)myRdPtr >= ((Char *)pQDHdr->queueStartAdrs +
                                pQDHdr->queueSizeInMAUs)) {
                        myRdPtr = (Ptr)pQDHdr->queueStartAdrs;
                    }
                    pQDHdr->readPtr = (Bits32 *)myRdPtr;
                    if (pQDHdr->readPtr != pQDHdr->writePtr) {
                        result = TRUE;
                    }
                }
            }
        }
    }
    return (result);
}

/*
 *  ======== LoggerRunMode_isUploadRequired ========
 */
Bool LoggerRunMode_isUploadRequired()
{
    Bool result = FALSE;
    Types_Timestamp64 tstamp;
    Int32 deltaTime = 0;
    Types_FreqHz freqHz;

    Timestamp_getFreq(&freqHz);

    if ((freqHz.hi > 0) || ((freqHz.lo & 0x80000000) != 0)) {
        Timestamp_get64(&tstamp);
        tstamp.lo = 0x7FFFFFFF & ((tstamp.hi << 1) | (tstamp.lo >> 31));
        freqHz.lo = 0x7FFFFFFF & ((freqHz.hi << 1) | (freqHz.lo >> 31));
    } else {
        tstamp.lo = xdc_runtime_Timestamp_get32() & 0x7FFFFFFF;
    }
    deltaTime = (Int32)tstamp.lo -
            (Int32)LoggerRunMode_module->lastUploadTstamp;
    if ((deltaTime < 0) || (deltaTime > (Int32)freqHz.lo)) {
        LoggerRunMode_module->lastUploadTstamp = tstamp.lo;
        result = TRUE;
    }

    return (result);
}

/*
 *  ======== LoggerRunMode_idleHook ========
 *  Hook function that can be called by SysBios's Idle loop.
 *  This function ensures that events are uploaded in a timely manner even if
 *  they are logged infrequently by checking the isUploadRequired method and
 *  if it returns true, calling the flush() method for each of the logger
 *  instances.
 */
Void LoggerRunMode_idleHook()
{
    int i = 0;
    LoggerRunMode_Object *obj = NULL;

    if (LoggerRunMode_isUploadRequired()) {
        for (i = 0; i < LoggerRunMode_Object_count(); i++) {
            obj = LoggerRunMode_Object_get(NULL, i);
            if (obj != NULL){
                LoggerRunMode_flush(obj);
            }
        }
    }
}
