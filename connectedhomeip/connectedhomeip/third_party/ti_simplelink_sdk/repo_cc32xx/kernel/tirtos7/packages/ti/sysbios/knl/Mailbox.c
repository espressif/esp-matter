/*
 * Copyright (c) 2015-2020, Texas Instruments Incorporated
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
 */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h> /* for memcpy() */

#include <ti/sysbios/BIOS.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/runtime/Assert.h>
#include <ti/sysbios/runtime/Error.h>
#include <ti/sysbios/runtime/Memory.h>
#include <ti/sysbios/runtime/Startup.h>

int Mailbox_postInit(Mailbox_Object *obj, size_t blockSize);

Mailbox_Module_State Mailbox_Module_state = {
    .objQ.next = &Mailbox_Module_state.objQ,
    .objQ.prev = &Mailbox_Module_state.objQ
};

static const Mailbox_Params Mailbox_Params_default = {
    .heap = NULL,
    .readerEvent = NULL,
    .readerEventId = 0,
    .writerEvent = NULL,
    .writerEventId = 0,
    .buf = NULL,
    .bufSize = 0
};

/*
 *  ======== Mailbox_Instance_init ========
 */
int Mailbox_Instance_init(Mailbox_Object *obj, size_t msgSize,
    unsigned int numMsgs, const Mailbox_Params* params,  Error_Block *eb)
{
    Semaphore_Params semParams;
    Queue_Handle dataQue;
    Queue_Handle freeQue;
    Semaphore_Handle dataSem, freeSem;
    size_t bufSize;
    size_t blockSize;

    dataQue = &obj->dataQue;
    freeQue = &obj->freeQue;
    dataSem = &obj->dataSem;
    freeSem = &obj->freeSem;

    obj->buf = params->buf;
    obj->heap = params->heap;
    obj->msgSize = msgSize;
    obj->numMsgs = numMsgs;
    obj->numFreeMsgs = numMsgs;
    obj->allocBuf = NULL;

    blockSize = sizeof(Mailbox_MbxElem) + msgSize;

    /* if user didn't specify a buf, alloc one; else use the one specified. */
    if (obj->buf == NULL) {
        if (BIOS_runtimeCreatesEnabled == true) {
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
            obj->allocBuf = (char *)Memory_alloc(obj->heap, bufSize, 0, eb);

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

    (void)Mailbox_postInit(obj, blockSize);

    /* construct semaphores */
    Semaphore_Params_init(&semParams);
    semParams.event   = params->readerEvent;
    semParams.eventId = params->readerEventId;
    Semaphore_construct(Semaphore_struct(dataSem), 0, &semParams);

    semParams.event   = params->writerEvent;
    semParams.eventId = params->writerEventId;
    Semaphore_construct(Semaphore_struct(freeSem), (int)obj->numMsgs, &semParams);

    /* put Mailbox object on global Mailbox Object list (Queue_put is atomic) */
    Queue_put(&Mailbox_module->objQ, &obj->objElem);

    return 0;
}

/*
 *  ======== Mailbox_Instance_finalize ========
 */
void Mailbox_Instance_finalize(Mailbox_Object *obj, int status)
{
    Queue_Handle freeQue;
    Queue_Handle dataQue;
    Semaphore_Handle dataSem, freeSem;
    size_t blockSize;

    /* Return if create failed while allocating the buffer for the HeapBuf. */
    if (status == 1) {
        return;
    }

    dataQue = &obj->dataQue;
    freeQue = &obj->freeQue;
    dataSem = &obj->dataSem;
    freeSem = &obj->freeSem;

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
 *  ======== Mailbox_getMailboxFromObjElem ========
 */
Mailbox_Handle Mailbox_getMailboxFromObjElem(Queue_Elem *mbxQelem)
{
    if (mbxQelem == (Queue_Elem *)&Mailbox_module->objQ) {
        return (NULL);
    }

    return ((Mailbox_Handle)((char *)mbxQelem -
               offsetof(Mailbox_Struct, objElem)));
}

/*
 *  ======== Mailbox_getMsgSize ========
 */
size_t Mailbox_getMsgSize(Mailbox_Object *obj)
{
    /* return the message size */
    return (obj->msgSize);
}

/*
 *  ======== Mailbox_getNumFreeMsgs ========
 */
int Mailbox_getNumFreeMsgs(Mailbox_Object *obj)
{
    /* return the number of free msgs */
    return ((int)obj->numFreeMsgs);
}

/*
 *  ======== Mailbox_getNumPendingMsgs ========
 */
int Mailbox_getNumPendingMsgs(Mailbox_Object *obj)
{
    /* return the number of unread msgs */
    return ((int)obj->numMsgs - (int)obj->numFreeMsgs);
}

/*
 *  ======== Mailbox_pend ========
 */
bool Mailbox_pend(Mailbox_Object *obj, void * msg, uint32_t timeout)
{
    Mailbox_MbxElem *elem;
    Queue_Handle dataQue;
    Queue_Handle freeQue;
    Semaphore_Handle dataSem, freeSem;
    unsigned int key;

    dataQue = &obj->dataQue;
    freeQue = &obj->freeQue;
    dataSem = &obj->dataSem;
    freeSem = &obj->freeSem;

    if (Semaphore_pend(dataSem, timeout)) {
        /* get message from dataQue */
        elem = Queue_get(dataQue);

        /* copy message to user supplied pointer */
        /* SV.BANNED.REQUIRED.COPY */
        (void)memcpy(msg, elem + 1, obj->msgSize);

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

        return (true);
    }
    else {
        return (false);
    }
}

/*
 *  ======== Mailbox_post ========
 */
bool Mailbox_post(Mailbox_Object *obj, void * msg, uint32_t timeout)
{
    Mailbox_MbxElem *elem;
    Queue_Handle dataQue;
    Queue_Handle freeQue;
    Semaphore_Handle dataSem, freeSem;
    unsigned int hwiKey, taskKey;

    dataQue = &obj->dataQue;
    freeQue = &obj->freeQue;
    dataSem = &obj->dataSem;
    freeSem = &obj->freeSem;

    if (Semaphore_pend(freeSem, timeout)) {
        /* perform the dequeue and decrement numFreeMsgs atomically */
        hwiKey = Hwi_disable();

        /* get a message from the free queue */
        elem = Queue_dequeue(freeQue);

        /* Make sure that a valid pointer was returned. */
        if (elem == (Mailbox_MbxElem *)(freeQue)) {
            Hwi_restore(hwiKey);
            return (false);
        }

        /* decrement the numFreeMsgs */
        obj->numFreeMsgs--;

        /* re-enable ints */
        Hwi_restore(hwiKey);

        /* copy msg to elem */
        /* SV.BANNED.REQUIRED.COPY */
        (void)memcpy(elem + 1, msg, obj->msgSize);

        /* Make Queue_enqueue and Semaphore_post atomic */
        taskKey = Task_disable();
        hwiKey = Hwi_disable();

        /* put message on dataQueue */
        Queue_enqueue(dataQue, &(elem->elem));

        /* post the semaphore */
        Semaphore_post(dataSem);

        Hwi_restore(hwiKey);
        Task_restore(taskKey);

        return (true);          /* success */
    }
    else {
        return (false);         /* error */
    }
}

/*
 *  ======== Mailbox_postInit ========
 */
int Mailbox_postInit(Mailbox_Object *obj, size_t blockSize)
{
    unsigned int i;
    Queue_Handle freeQue;
    char *buf;

    freeQue = &obj->freeQue;

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

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Mailbox_Object_first ========
 */
Mailbox_Handle Mailbox_Object_first()
{
    return (Mailbox_getMailboxFromObjElem(Queue_head(&(Mailbox_module->objQ))));
}

/*
 *  ======== Mailbox_Object_next ========
 */
Mailbox_Handle Mailbox_Object_next(Mailbox_Handle mbx)
{
    return (Mailbox_getMailboxFromObjElem(Queue_next(&mbx->objElem)));
}

/*
 *  ======== Mailbox_Params_init ========
 */
void Mailbox_Params_init(Mailbox_Params *params)
{
    *params = Mailbox_Params_default;
}

/*
 *  ======== Mailbox_construct ========
 */
Mailbox_Handle Mailbox_construct(Mailbox_Struct *mailbox,
    size_t msgSize, unsigned int numMsgs, const Mailbox_Params *params, Error_Block *eb)
{
    int status;

    if (params == NULL) {
        params = &Mailbox_Params_default;
    }

    status = Mailbox_Instance_init(mailbox, msgSize, numMsgs, params, eb);

    if (status != 0) {
        Mailbox_Instance_finalize(mailbox, status);
        mailbox = NULL;
    }

    return (mailbox);
}

/*
 *  ======== Mailbox_create ========
 */
Mailbox_Handle Mailbox_create(
    size_t msgSize, unsigned int numMsgs, const Mailbox_Params *params, Error_Block *eb)
{
    Mailbox_Handle mailbox, status;

    mailbox = Memory_alloc(NULL, sizeof(Mailbox_Object), 0, eb);

    if (mailbox != NULL) {
        status = Mailbox_construct(mailbox, msgSize, numMsgs, params, eb);
        if (status == NULL) {
            Memory_free(NULL, mailbox, sizeof(Mailbox_Object));
            mailbox = NULL;
        }
    }

    return (mailbox);
}

/*
 *  ======== Mailbox_delete ========
 */
void Mailbox_delete(Mailbox_Handle *mailbox)
{
    Mailbox_destruct(*mailbox);

    Memory_free(NULL, *mailbox, sizeof(Mailbox_Object));

    *mailbox = NULL;
}

/*
 *  ======== Mailbox_destruct ========
 */
void Mailbox_destruct(Mailbox_Handle mailbox)
{
    unsigned int hwiKey;

    Mailbox_Instance_finalize(mailbox, 0);

    /* remove Mailbox object from global Mailbox object list (Queue_remove is not atomic) */
    hwiKey = Hwi_disable();
    Queue_remove(&mailbox->objElem);
    Hwi_restore(hwiKey);
}
