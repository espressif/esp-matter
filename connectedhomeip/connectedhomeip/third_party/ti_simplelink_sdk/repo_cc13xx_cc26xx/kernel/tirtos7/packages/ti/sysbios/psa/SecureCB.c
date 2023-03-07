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
 *  ======== SecureCB. ========
 */

#include <ti/sysbios/family/arm/v8m/Hwi.h>

#include "SecureCB.h"

typedef struct {
    SecureCB_Object *volatile next;
    SecureCB_Object *volatile prev;
} SecureCB_Head;

/* queue of callbacks */
static SecureCB_Head SecureCB_qHead;

static Hwi_Struct SecureCB_HwiS;
static Hwi_Handle SecureCB_HwiH;

/*
 *  ======== SecureCB_isr ========
 */
static void SecureCB_isr(uintptr_t arg)
{
    SecureCB_Object *scb = (SecureCB_Object *)&SecureCB_qHead;

    while (scb->next != (SecureCB_Object *)&SecureCB_qHead) {
        scb = scb->next;
        if (scb->crstat == (SecureCB_CRSTAT_DEFINED|SecureCB_CRSTAT_ENABLED)) {
            /* clear ENABLED bit */
            scb->crstat &= ~SecureCB_CRSTAT_ENABLED;
            scb->fxn(scb->arg);
        }
    }
}

/*
 *  ======== SecureCB_init ========
 */
void SecureCB_init()
{
    Hwi_Params params;

    SecureCB_qHead.next = (SecureCB_Object *)&SecureCB_qHead;
    SecureCB_qHead.prev = (SecureCB_Object *)&SecureCB_qHead;

    Hwi_Params_init(&params);

    /*
     * Any Hwi construct failure will result in a terminating
     * Exception being raised.
     */
    Hwi_construct(&SecureCB_HwiS, SecureCB_INT_NUM,
                   SecureCB_isr, &params, NULL);

    SecureCB_HwiH = Hwi_handle(&SecureCB_HwiS);
}

/*
 *  ======== SecureCB_add ========
 */
uint32_t SecureCB_add(SecureCB_Object *scb)
{
    uint32_t key;

    key = Hwi_disable();

    SecureCB_qHead.prev->next = scb;
    scb->prev = SecureCB_qHead.prev;
    SecureCB_qHead.prev = scb;
    scb->next = (SecureCB_Object *)&SecureCB_qHead;

    Hwi_restore(key);

    return (SecureCB_STATUS_OK);
}

/*
 *  ======== SecureCB_remove ========
 */
uint32_t SecureCB_remove(SecureCB_Handle scb)
{
    uint32_t key;

    if (!(scb->crstat & SecureCB_CRSTAT_DEFINED)) {
        return (SecureCB_STATUS_ERROR);
    }

    key = Hwi_disable();

    scb->prev->next = scb->next;
    scb->next->prev = scb->prev;

    /*
     * Implementation note:
     * In order to allow a callback function to destruct its
     * associated callback object, preserve this callback
     * objects's 'next' pointer so that object traversal in
     * the CB dispatcher is not affected.
     */

    Hwi_restore(key);

    return (SecureCB_STATUS_OK);
}

/*
 *  ======== SecureCB_construct ========
 */
SecureCB_Handle SecureCB_construct(SecureCB_Object *scb, SecureCB_FuncPtr fxn,
        SecureCB_Arg arg)
{
    scb->next = scb;
    scb->prev = scb;

    scb->fxn = fxn;
    scb->arg = arg;
    scb->intNum = SecureCB_INT_NUM;
    scb->crstat = SecureCB_CRSTAT_DEFINED;

    SecureCB_add(scb);

    return (scb);
}

/*
 *  ======== SecureCB_destruct ========
 */
uint32_t SecureCB_destruct(SecureCB_Object *scb)
{
    uint32_t key;
    uint32_t ret = SecureCB_STATUS_ERROR;

    key = Hwi_disable();

    if (scb->crstat & SecureCB_CRSTAT_DEFINED) {

        SecureCB_remove(scb);

        /* mark as undefined */
        scb->crstat &= ~SecureCB_CRSTAT_DEFINED;

        ret = SecureCB_STATUS_OK;
    }

    Hwi_restore(key);

    return (ret);
}
