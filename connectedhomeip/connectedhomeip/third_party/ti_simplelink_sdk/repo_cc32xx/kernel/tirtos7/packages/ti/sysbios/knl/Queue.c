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
 */
/*
 *  ======== Queue.c ========
 *  Implementation of functions specified in Queue.xdc.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <ti/sysbios/hal/Hwi.h>

#include <ti/sysbios/knl/Queue.h>

static const Queue_Params Queue_Params_default = {
    .dummy = 0
};

/*
 *  ======== Instance_init ========
 */
/* MISRA.FUNC.UNUSEDPAR.2012 */
/* REQ_TAG(SYSBIOS-485), REQ_TAG(SYSBIOS-486), REQ_TAG(SYSBIOS-487) */
void Queue_Instance_init(Queue_Object *obj, const Queue_Params *params)
{
    obj->prev = obj;
    obj->next = obj;
}

/*
 *  ======== dequeue ========
 */
/* REQ_TAG(SYSBIOS-488) */
void * Queue_dequeue(Queue_Object *obj)
{
    Queue_Elem *elem;
    Queue_Elem *next;

    elem = obj->next;
    next = elem->next;
    obj->next = next;
    next->prev = obj;

    return (elem);
}

/*
 *  ======== empty ========
 */
/* REQ_TAG(SYSBIOS-494) */
bool Queue_empty(Queue_Object *obj)
{
    return ((bool)(obj->next == obj));
}

/*
 *  ======== enqueue ========
 */
/* REQ_TAG(SYSBIOS-492) */
void Queue_enqueue(Queue_Object *obj, Queue_Elem *elem)
{
    Queue_Elem *prev;

    prev = obj->prev;

    elem->next = obj;
    elem->prev = prev;
    prev->next = elem;
    obj->prev = elem;
}

/*
 *  ======== get ========
 */
/* REQ_TAG(SYSBIOS-489) */
void * Queue_get(Queue_Object *obj)
{
    Queue_Elem *elem;
    unsigned int key;

    key = Hwi_disable();

    elem = obj->next;

    obj->next = elem->next;
    elem->next->prev = obj;

    Hwi_restore(key);

    return (elem);

}

/*
 *  ======== getTail ========
 */
/* REQ_TAG(SYSBIOS-496) */
void * Queue_getTail(Queue_Object *obj)
{
    Queue_Elem *elem;
    unsigned int key;

    key = Hwi_disable();

    elem = obj->prev;

    obj->prev = elem->prev;
    elem->prev->next = obj;

    Hwi_restore(key);

    return (elem);

}

/*
 *  ======== head ========
 */
void * Queue_head(Queue_Object *obj)
{
    return (obj->next);
}

/*
 *  ======== elemClear ========
 */
void Queue_elemClear(Queue_Elem *qelem)
{
    qelem->prev = qelem;
    qelem->next = qelem;
}

/*
 *  ======== insert ========
 */
/* REQ_TAG(SYSBIOS-497) */
void Queue_insert(Queue_Elem *qelem, Queue_Elem *elem)
{
    Queue_Elem *prev;

    prev = qelem->prev;

    elem->next = qelem;
    elem->prev = prev;
    prev->next = elem;
    qelem->prev = elem;
}

/*
 *  ======== next ========
 */
/* REQ_TAG(SYSBIOS-498) */
void * Queue_next(Queue_Elem *qelem)
{
    return (qelem->next);
}

/*
 *  ======== prev ========
 */
/* REQ_TAG(SYSBIOS-499) */
void * Queue_prev(Queue_Elem *qelem)
{
    return (qelem->prev);
}

/*
 *  ======== put ========
 */
/* REQ_TAG(SYSBIOS-493) */
void Queue_put(Queue_Object *obj, Queue_Elem *elem)
{
    unsigned int key;

    key = Hwi_disable();

    elem->next = obj;
    elem->prev = obj->prev;
    obj->prev->next = elem;
    obj->prev = elem;

    Hwi_restore(key);
}

/*
 *  ======== putHead ========
 */
/* REQ_TAG(SYSBIOS-491) */
void Queue_putHead(Queue_Object *obj, Queue_Elem *elem)
{
    unsigned int key;

    key = Hwi_disable();

    elem->prev = obj;
    elem->next = obj->next;
    obj->next->prev = elem;
    obj->next = elem;

    Hwi_restore(key);
}

/*
 *  ======== remove ========
 */
void Queue_remove(Queue_Elem *qelem) 
{
#if defined(__IAR_SYSTEMS_ICC__)
    void * temp;
    temp = qelem->next;
    qelem->prev->next = temp;
    temp = qelem->prev;
    qelem->next->prev = temp;
#else
    qelem->prev->next = qelem->next;
    qelem->next->prev = qelem->prev;
#endif
}

/*
 *  ======== isQueued ========
 */
bool Queue_isQueued(Queue_Elem *qelem) 
{
    bool rv;

    if ((qelem->prev == qelem) && (qelem->next == qelem)) {
        rv = (bool)false;
    }
    else {
        rv = (bool)true;
    }

    return rv;
}

/* -------- The following functions were generated in BIOS 6.x -------- */

/*
 *  ======== Queue_Params_init ========
 */
void Queue_Params_init(Queue_Params *params)
{
    *params = Queue_Params_default;
}

/*
 *  ======== Queue_construct ========
 */
Queue_Handle Queue_construct(Queue_Object *obj, const Queue_Params *params)
{
    Queue_Instance_init(obj, params);

    return (obj);
}

/*
 *  ======== Queue_create ========
 */
Queue_Handle Queue_create(const Queue_Params *params, Error_Block *eb)
{
    Queue_Handle queue;

    queue = Memory_alloc(NULL, sizeof(Queue_Object), 0, eb);

    if (queue != NULL) {
        queue = Queue_construct(queue, params);
    }

    return (queue);
}

/*
 *  ======== Queue_destruct ========
 */
void Queue_destruct(Queue_Object *obj)
{
    Queue_elemClear(obj);
}

/*
 *  ======== Queue_delete ========
 */
void Queue_delete(Queue_Handle *obj)
{
    Memory_free(NULL, *obj, sizeof(Queue_Object));
}
