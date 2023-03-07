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
#include <bl702.h>

#include <bl_sys.h>
#include <bl_chip.h>
#include <bl_irq.h>
#include <bl_sec.h>
#include <bl_rtc.h>
#include <bl_timer.h>
#include <bl_wdt.h>
#include <hal_boot2.h>
#include <hal_board.h>
#include <hal_hwtimer.h>
#include <hal_pds.h>
#include <hal_tcal.h>
#include <hosal_uart.h>
#include <hosal_rng.h>

#include <FreeRTOS.h>
#include <task.h>

#include <libfdt.h>
#include <utils_log.h>
#include <blog.h>

#include <main.h>

#ifdef CONFIG_OTDEMO

static SemaphoreHandle_t    appSemHandle          = NULL;

static void ot_stateChangeCallback(uint32_t flags, void * p_context) 
{
    char states[5][10] = {"disabled", "detached", "child", "router", "leader"};
    otInstance *instance = (otInstance *)p_context;
    uint8_t *p;

    if (flags & OT_CHANGED_THREAD_ROLE)
    {

        uint32_t role = otThreadGetDeviceRole(p_context);
        switch(role)
        {
            case OT_DEVICE_ROLE_CHILD:
                app_gpioLedOn(3);
                app_gpioLedOn(2);
                break;
            case OT_DEVICE_ROLE_ROUTER:
                app_gpioLedOff(3);
                app_gpioLedOn(2);
                break;
            case OT_DEVICE_ROLE_LEADER:
                app_gpioLedOn(3);
                app_gpioLedOff(2);
                break;

            case OT_DEVICE_ROLE_DISABLED:
            case OT_DEVICE_ROLE_DETACHED:
            default:
                break;
        }

        if (role) {
            printf("Current role       : %s\r\n", states[otThreadGetDeviceRole(p_context)]);

            p = (uint8_t *)(otLinkGetExtendedAddress(instance)->m8);
            printf("Extend Address     : %02x%02x-%02x%02x-%02x%02x-%02x%02x\r\n", 
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t *)(otThreadGetMeshLocalPrefix(instance)->m8);
            printf("Local Prefx        : %02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

            p = (uint8_t *)(otThreadGetLinkLocalIp6Address(instance)->mFields.m8);
            printf("IPv6 Address       : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);

            printf("Rloc16             : %x\r\n", otThreadGetRloc16(instance));

            p = (uint8_t *)(otThreadGetRloc(instance)->mFields.m8);
            printf("Rloc               : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
        }
    }
}

#if OPENTHREAD_FTD
static void ot_neighborChangeCallback (otNeighborTableEvent aEvent, const otNeighborTableEntryInfo *aEntryInfo) 
{
    switch (aEvent) {
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_ADDED:
            printf("child added %llx\r\n", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_REMOVED:
            printf("child removed %llx\r\n", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_CHILD_MODE_CHANGED:
            printf("child mode changned %llx\r\n", *(uint64_t *)aEntryInfo->mInfo.mChild.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_ADDED:
            printf("router added %llx\r\n", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
        case OT_NEIGHBOR_TABLE_EVENT_ROUTER_REMOVED:
            printf("router removed %llx\r\n", *(uint64_t *)aEntryInfo->mInfo.mRouter.mExtAddress.m8);
            break;
    }
}
#endif

static void led_handler(uint32_t val) 
{
    if (val & 1) {
        app_gpioLedOn(1);
    }
    else {
        app_gpioLedOff(1);
    }
}

static void keyPress_handler(void *parg)
{
    if (appSemHandle) {
        BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
        xSemaphoreGiveFromISR( appSemHandle, &pxHigherPriorityTaskWoken);
    }
}

void app_task (void) 
{
    uint8_t                 *p, i;
    otMessageInfo           messageInfo;
    otMeshLocalPrefix       *pprefix = NULL;
    otMessage               *pmsg;
    otNeighborInfo          neighborInfo;
    otNeighborInfoIterator  nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
    uint32_t                ledCnt = 0;
#if OPENTHREAD_FTD
    otChildInfo             childInfo;
#else
    otRouterInfo            parentInfo;
#endif

    app_gpioInit(keyPress_handler);

    appSemHandle = xSemaphoreCreateBinary();
    configASSERT(appSemHandle != NULL);

    while (true) {

        if (xSemaphoreTake(appSemHandle, 10000)) {

            if (app_gpioGetKeyValue()) {
                vTaskDelay(300);
                if (app_gpioGetKeyValue()) {
                    OT_THREAD_SAFE(
                        otInstance *instance = otrGetInstance();
                        if (instance) {
#if CONFIG_OTDEMO==1
                            app_udpSend(instance, ledCnt);
#elif CONFIG_OTDEMO==2
                            app_coapReq(instance, ledCnt);
#endif
                        }
                    );

                    ledCnt += 1;
                }
            }

            continue;
        }

        OT_THREAD_SAFE(
            otInstance *instance = otrGetInstance();
            if (instance) {
                pprefix = (otMeshLocalPrefix *)otThreadGetMeshLocalPrefix(instance);

#if OPENTHREAD_FTD
                for (i = 0; i < otThreadGetMaxAllowedChildren(instance); i ++) {
                    if (OT_ERROR_NONE != otThreadGetChildInfoByIndex(instance, i, &childInfo)) {
                        continue;
                    }

                    printf("Child(%02d) Rloc16           : %x\r\n", i, childInfo.mRloc16);
                    p = childInfo.mExtAddress.m8;
                    printf("Child(%02d) Ext Addr         : %02x%02x-%02x%02x-%02x%02x-%02x%02x\r\n",
                            i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

                    memset(&messageInfo, 0, sizeof(messageInfo));

                    memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
                    messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                    messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                    messageInfo.mPeerAddr.mFields.m8[14] = childInfo.mRloc16 >> 8;
                    messageInfo.mPeerAddr.mFields.m8[15] = childInfo.mRloc16 & 0xff;
                    
                    p = messageInfo.mPeerAddr.mFields.m8;
                    printf("Child(%02d) Rloc             : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                           i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                }
#else

                if (OT_ERROR_NONE == otThreadGetParentInfo(instance, &parentInfo)) {

                    memset(&messageInfo, 0, sizeof(messageInfo));

                    memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
                    messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                    messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                    messageInfo.mPeerAddr.mFields.m8[14] = parentInfo.mRloc16 >> 8;
                    messageInfo.mPeerAddr.mFields.m8[15] = parentInfo.mRloc16 & 0xff;
                    
                    p = messageInfo.mPeerAddr.mFields.m8;

                    printf("Parent Rloc16                : %x\r\n", parentInfo.mRloc16);
                    p = parentInfo.mExtAddress.m8;
                    printf("Parent Ext Addr              : %02x%02x-%02x%02x-%02x%02x-%02x%02x\r\n",
                            p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
                    printf("Parent Rloc                  : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                           p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                }
#endif

                i = 0;
                nbrIter = OT_NEIGHBOR_INFO_ITERATOR_INIT;
                while (otThreadGetNextNeighborInfo(instance, &nbrIter, &neighborInfo) == OT_ERROR_NONE){
                    printf("Neighbor(%02d) Rloc16        : %x\r\n", i, neighborInfo.mRloc16);
                    p = neighborInfo.mExtAddress.m8;
                    printf("Neighbor(%02d) Ext Addr      : %02x%02x-%02x%02x-%02x%02x-%02x%02x\r\n",
                            i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

                    memset(&messageInfo, 0, sizeof(messageInfo));

                    memcpy(messageInfo.mPeerAddr.mFields.m8, pprefix->m8, sizeof(pprefix->m8));
                    messageInfo.mPeerAddr.mFields.m8[11] = 0xff;
                    messageInfo.mPeerAddr.mFields.m8[12] = 0xfe;
                    messageInfo.mPeerAddr.mFields.m8[14] = neighborInfo.mRloc16 >> 8;
                    messageInfo.mPeerAddr.mFields.m8[15] = neighborInfo.mRloc16 & 0xff;

                    p = messageInfo.mPeerAddr.mFields.m8;
                    printf("Neighbor(%02d) Rloc          : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\r\n",
                           i, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7], p[8], p[9], p[10], p[11], p[12], p[13], p[14], p[15]);
                    i ++;

                    pmsg = otIp6NewMessage(instance, NULL);
                    if (pmsg) {
                        messageInfo.mHopLimit          = 0;
                        messageInfo.mAllowZeroHopLimit = false;

                        if (OT_ERROR_NONE != otIcmp6SendEchoRequest(instance, pmsg, &messageInfo, 0)) {
                            otMessageFree(pmsg);
                        }
                    }
                }
            }
        );
    }
}

void otrInitUser(otInstance * instance)
{
    char tempStr[OT_NETWORK_KEY_SIZE + 10] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    otLinkModeConfig mode;

    otAppCliInit(instance);

    printf("Thread version     : %s\r\n", otGetVersionString());

    otThreadSetNetworkKey(instance, (const otNetworkKey *)tempStr);
    otLinkSetChannel(instance, THREAD_CHANNEL);

    otLinkSetPanId(instance, THREAD_PANID);
    sprintf(tempStr, "OpenThread-%x", THREAD_PANID);
    otThreadSetNetworkName(instance, (const char *)tempStr);

    memset(&mode, 0, sizeof(mode));

#if OPENTHREAD_FTD==1
    mode.mDeviceType         = 1;
    mode.mRxOnWhenIdle       = 1;
    mode.mNetworkData        = 1;
#endif

#if OPENTHREAD_MTD==1
    mode.mDeviceType         = 0;
    mode.mNetworkData        = 0;
#if CONFIG_PP > 0
    mode.mRxOnWhenIdle       = 0;
    otLinkSetPollPeriod(instance, CONFIG_PP);
#endif
#endif
    otThreadSetLinkMode(instance, mode);

    otIp6SetEnabled(instance, true);
    otThreadSetEnabled(instance, true);

    printf("Link Mode           %d, %d, %d\r\n", 
        otThreadGetLinkMode(instance).mRxOnWhenIdle, 
        otThreadGetLinkMode(instance).mDeviceType, 
        otThreadGetLinkMode(instance).mNetworkData);
    printf("Link Mode           %d, %d, %d\r\n", 
        mode.mRxOnWhenIdle, mode.mDeviceType, mode.mNetworkData);
    printf("Network name        : %s\r\n", otThreadGetNetworkName(instance));
    printf("PAN ID              : %x\r\n", otLinkGetPanId(instance));

    printf("channel             : %d\r\n", otLinkGetChannel(instance));

    otSetStateChangedCallback(instance, ot_stateChangeCallback, instance);
#if OPENTHREAD_FTD
    otThreadRegisterNeighborTableCallback(instance, ot_neighborChangeCallback);
#endif

#if CONFIG_OTDEMO==1
    app_sockInit(instance, led_handler);
#elif CONFIG_OTDEMO==2
    app_coapInit(instance, led_handler);
#endif
}

#else

void otrInitUser(otInstance * instance)
{
#ifdef CONFIG_NCP
    otAppNcpInit((otInstance * )instance);
#else
    otAppCliInit((otInstance * )instance);
#endif
}
#endif
