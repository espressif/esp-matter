/*
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
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
 *  ======== Mailbox.c ========
 *  Implementation of functions specified in Mailbox.xdc.
 */
#include <xdc/std.h>

#include <string.h> /* for memcpy() */

#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/Startup.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>

#include "package/internal/Mailbox.xdc.h"

/*
 *  ======== Mailbox_Module_startup ========
 *  Calls postInit on all of the statically created instances to split
 *  their buffers into blocks.
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
Int Mailbox_Module_startup(Int phase)
{
    Int i;
    SizeT blockSize;
    Mailbox_Object *obj;

    for (i = (Int)0; i < (Int)Mailbox_Object_count(); i++) {
        obj = Mailbox_Object_get(NULL, i);

        if (obj->buf == NULL) {
            blockSize = (sizeof(Mailbox_MbxElem) + obj->msgSize
                + (Mailbox_maxTypeAlign - 1U)) &
                ~(Mailbox_maxTypeAlign - 1U);
        }
        else {
            blockSize = (sizeof(Mailbox_MbxElem) + obj->msgSize);
        }

        /* call postInit for static instance */
        (Void)Mailbox_postInit(obj, blockSize);
    }

    return Startup_DONE;
}

/*
 *  ======== Mailbox_Instance_init ========
 */
Int Mailbox_Instance_init(Mailbox_Object *obj, SizeT msgSize,
    UInt numMsgs, const Mailbox_Params* params,  Error_Block *eb)
{
    Semaphore_Params semParams;
    Queue_Handle dataQue;
    Queue_Handle freeQue;
    Semaphore_Handle dataSem, freeSem;
    SizeT bufSize;
    SizeT blockSize;

    dataQue = Mailbox_Instance_State_dataQue(obj);
    freeQue = Mailbox_Instance_State_freeQue(obj);
    dataSem = Mailbox_Instance_State_dataSem(obj);
    freeSem = Mailbox_Instance_State_freeSem(obj);

    obj->buf = params->buf;
    obj->heap = params->heap;
    obj->msgSize = msgSize;
    obj->numMsgs = numMsgs;
    obj->numFreeMsgs = numMsgs;
    obj->allocBuf = NULL;

    blockSize = sizeof(Mailbox_MbxElem) + msgSize;

    /* if user didn't specify a buf, alloc one; else use the one specified. */
    if (obj->buf == NULL) {
        if (BIOS_runtimeCreatesEnabled == TRUE) {
            /*
             * Ensure blockSize is properly aligned.
             *
             * 'round up' blockSize so malloc'ed buf will respect HeapBuf's rules.
             * Use worst case alignment to match the HeapBuf default).
             */
            blockSize = (blockSize + (Mailbox_maxTypeAlign - 1U)) &
                    ~(Mailbox_maxTypeAlign - 1U);

            bufSize = blockSize * numMsgs;

            /* alloc buf from configured heap */
            obj->allocBuf = (Char *)Memory_alloc(obj->heap, bufSize, 0, eb);

            if (obj->allocBuf == NULL) {
                /* clean-up in finalize */
                return (1);
            }
        }
        else {
            return (1); /* fail to construct if buf is not provided in construct-only model */
        }
    }
    else {
        /* Assert that bufSize is not too small. */
        Assert_isTrue((params->bufSize >= (numMsgs * blockSize)),
            Mailbox_A_invalidBufSize);
    }

    /* construct queues */
    Queue_construct(Queue_struct(freeQue), NULL);
    Queue_construct(Queue_struct(dataQue), NULL);

    (Void)Mailbox_postInit(obj, blockSize);

    /* construct semaphores */
    Semaphore_Params_init(&semParams);
    semParams.event   = params->readerEvent;
    semParams.eventId = params->readerEventId;
    Semaphore_construct(Semaphore_struct(dataSem), 0, &semParams);

    semParams.event   = params->writerEvent;
    semParams.eventId = params->writerEventId;
    Semaphore_construct(Semaphore_struct(freeSem), (Int)obj->numMsgs, &semParams);

    return 0;
}

/*
 *  ======== Mailbox_Instance_finalize ========
 */
Void Mailbox_Instance_finalize(Mailbox_Object *obj, Int status)
{
    Queue_Handle freeQue;
    Queue_Handle dataQue;
    Semaphore_Handle dataSem, freeSem;
    SizeT blockSize;

    /* Return if create failed while allocating the buffer for the HeapBuf. */
    if (status == 1) {
        return;
    }

    freeQue = Mailbox_Instance_State_freeQue(obj);
    dataQue = Mailbox_Instance_State_dataQue(obj);
    dataSem = Mailbox_Instance_State_dataSem(obj);
    freeSem = Mailbox_Instance_State_freeSem(obj);

    if (BIOS_runtimeCreatesEnabled) {
        /* if buf is alloc'ed then free it */
        if (obj->allocBuf != NULL) {
            blockSize = (sizeof(Mailbox_MbxElem) + obj->msgSize +
                        (Mailbox_maxTypeAlign - 1U)) &
                        ~(Mailbox_maxTypeAlign - 1U);

            Memory_free(obj->heap, obj->allocBuf, blockSize * obj->numMsgs);
        }
    }

    Queue_destruct(Queue_struct(freeQue));
    Queue_destruct(Queue_struct(dataQue));
    Semaphore_destruct(Semaphore_struct(freeSem));
    Semaphore_destruct(Semaphore_struct(dataSem));
}

/*
 *  ======== Mailbox_getMsgSize ========
 */
SizeT Mailbox_getMsgSize(Mailbox_Object *obj)
{
    /* return the message size */
    return (obj->msgSize);
}

/*
 *  ======== Mailbox_getNumFreeMsgs ========
 */
Int Mailbox_getNumFreeMsgs(Mailbox_Object *obj)
{
    /* return the number of free msgs */
    return ((Int)obj->numFreeMsgs);
}

/*
 *  ======== Mailbox_getNumPendingMsgs ========
 */
Int Mailbox_getNumPendingMsgs(Mailbox_Object *obj)
{
    /* return the number of unread msgs */
    return ((Int)obj->numMsgs - (Int)obj->numFreeMsgs);
}

/*
 *  ======== Mailbox_pend ========
 */
Bool Mailbox_pend(Mailbox_Object *obj, Ptr msg, UInt32 timeout)
{
    Mailbox_MbxElem *elem;
    Queue_Handle dataQue;
    Queue_Handle freeQue;
    Semaphore_Handle dataSem, freeSem;
    UInt key;

    dataQue = Mailbox_Instance_State_dataQue(obj);
    freeQue = Mailbox_Instance_State_freeQue(obj);
    dataSem = Mailbox_Instance_State_dataSem(obj);
    freeSem = Mailbox_Instance_State_freeSem(obj);

    if (Semaphore_pend(dataSem, timeout)) {
        /* get message from dataQue */
        elem = Queue_get(dataQue);

        /* copy message to user supplied pointer */
        /* SV.BANNED.REQUIRED.COPY */
        (Void)memcpy(msg, elem + 1, obj->msgSize);

        /* perform the enqueue and increment numFreeMsgs atomically */
        key = Hwi_disable();

        /* put message on freeQue */
        Queue_enqueue(freeQue, &(elem->elem));

        /* increment numFreeMsgs */
        obj->numFreeMsgs++;

        /* re-enable ints */
        Hwi_restore(key);

        /* post the semaphore */
        Semaphore_post(freeSem);

        return (TRUE);
    }
    else {
        return (FALSE);
    }
}

/*
 *  ======== Mailbox_post ========
 */
Bool Mailbox_post(Mailbox_Object *obj, Ptr msg, UInt32 timeout)
{
    Mailbox_MbxElem *elem;
    Queue_Handle dataQue;
    Queue_Handle freeQue;
    Semaphore_Handle dataSem, freeSem;
    UInt key;

    dataQue = Mailbox_Instance_State_dataQue(obj);
    freeQue = Mailbox_Instance_State_freeQue(obj);
    dataSem = Mailbox_Instance_State_dataSem(obj);
    freeSem = Mailbox_Instance_State_freeSem(obj);

    if (Semaphore_pend(freeSem, timeout)) {
        /* perform the dequeue and decrement numFreeMsgs atomically */
        key = Hwi_disable();

        /* get a message from the free queue */
        elem = Queue_dequeue(freeQue);

        /* Make sure that a valid pointer was returned. */
        if (elem == (Mailbox_MbxElem *)(freeQue)) {
            Hwi_restore(key);
            return (FALSE);
        }

        /* decrement the numFreeMsgs */
        obj->numFreeMsgs--;

        /* re-enable ints */
        Hwi_restore(key);

        /* copy msg to elem */
        /* SV.BANNED.REQUIRED.COPY */
        (Void)memcpy(elem + 1, msg, obj->msgSize);

        /* put message on dataQueue */
        Queue_put(dataQue, &(elem->elem));

        /* post the semaphore */
        Semaphore_post(dataSem);

        return (TRUE);          /* success */
    }
    else {
        return (FALSE);         /* error */
    }
}

/*
 *  ======== Mailbox_postInit ========
 */
Int Mailbox_postInit(Mailbox_Object *obj, SizeT blockSize)
{
    UInt i;
    Queue_Handle freeQue;
    Char *buf;

    freeQue = Mailbox_Instance_State_freeQue(obj);

    if (obj->buf) {
        buf = obj->buf;
    }
    else {
        buf = obj->allocBuf;
    }

    /*
     * Split the buffer into blocks that are length "blockSize" and
     * add into the free Queue.
     */
    for (i = 0; i < obj->numMsgs; i++) {
        /* MISRA.CAST.OBJ_PTR_TO_OBJ_PTR.2012 */
        Queue_put(freeQue, (Queue_Elem *)buf);
        buf += blockSize;
    }

    return (0);
}
