/*
 * Copyright (c) 2016-2022 Bouffalolab.
 *
 * This file is part of
 *     *** Bouffalolab Software Dev Kit ***
 *      (see www.bouffalolab.com).
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of Bouffalo Lab nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <utils_log.h>

#include <main.h>



static otCoapResource app_coapResLed =
{
    .mUriPath = "led",
    .mHandler = NULL,
    .mContext = NULL,
    .mNext    = NULL
};
static void (*app_coapHandler) (uint32_t);

static void app_coapLedHandler(void *aContext, otMessage *aMessage, const otMessageInfo *aMessageInfo) 
{
    uint32_t val;

    otMessageRead(aMessage, otMessageGetOffset(aMessage), &val, sizeof(val));
    printf ("CoAP Request received, port: %d, len: %d, val: %ld\r\n", aMessageInfo->mPeerPort, otMessageGetLength(aMessage), val);

    if (app_coapHandler) {
        app_coapHandler(val);
    }
}

void app_coapInit (otInstance *instance, void (*handler)(uint32_t)) 
{
    otCoapStart(instance, THREAD_COAP_PORT);

    app_coapResLed.mContext       = instance;
    app_coapResLed.mHandler       = app_coapLedHandler;

    app_coapHandler = handler;
    otCoapAddResource(instance, &app_coapResLed);
}

void app_coapReq(otInstance *instance, uint32_t val) 
{
    otNeighborInfo          neighborInfo;
    otNeighborInfoIterator  nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    otMessage               *pmsg = NULL;
    otMeshLocalPrefix       *pprefix = NULL;
    otMessageInfo           messageInfo;

    pprefix = (otMeshLocalPrefix *)otThreadGetMeshLocalPrefix(instance);

    nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    while (otThreadGetNextNeighborInfo(instance, &nbrIter, &neighborInfo) == OT_ERROR_NONE){

        memset(&messageInfo, 0, sizeof(messageInfo));

        memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
        messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
        messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
        messageInfo.mPeerAddr.mFields.m8[14] = neighborInfo.mRloc16 >> 8;
        messageInfo.mPeerAddr.mFields.m8[15] = neighborInfo.mRloc16 & 0xff;
        messageInfo.mPeerPort = THREAD_COAP_PORT;
        messageInfo.mHopLimit          = 0;
        messageInfo.mAllowZeroHopLimit = false;

        pmsg = otCoapNewMessage(instance, NULL);
        if (pmsg) {
            otCoapMessageInit(pmsg, OT_COAP_TYPE_NON_CONFIRMABLE, OT_COAP_CODE_PUT);
            otCoapMessageGenerateToken(pmsg, 2);

            if (OT_ERROR_NONE != otCoapMessageAppendUriPathOptions(pmsg, "led") ||
                OT_ERROR_NONE != otCoapMessageSetPayloadMarker(pmsg) ||
                OT_ERROR_NONE != otMessageAppend(pmsg, &val, sizeof(val))||
                OT_ERROR_NONE != otCoapSendRequest(instance, pmsg, &messageInfo, NULL, instance) 
                ) {
                break;
            }

            printf ("CoAP request sent, Rloc16: %x, val: %ld\r\n", neighborInfo.mRloc16, val);
        }
        pmsg = NULL;
    }

    if (pmsg) {
        otMessageFree(pmsg);
    }
}
