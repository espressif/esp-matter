/******************************************************************************

 @file shade.c

 @brief shade example application

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 *  Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 *  Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 *  Neither the name of Texas Instruments Incorporated nor the names of
    its contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/******************************************************************************
 Includes
 *****************************************************************************/
#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* OpenThread public API Header files */
#include <openthread/coap.h>
#include <openthread/link.h>
#include <openthread/platform/uart.h>
#include <openthread/thread.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <mqueue.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include "ti_drivers_config.h"

#include "images.h"
#include "shade.h"
#include "utils/code_utils.h"
#include "tiop_ui.h"

#include "disp_utils.h"
#include "otstack.h"

/* Private configuration Header files */
#include "task_config.h"
#include "tiop_config.h"

#if BOARD_DISPLAY_USE_UART
/* CUI Header file */
#include "cui.h"
#endif /* BOARD_DISPLAY_USE_UART */

#if (OPENTHREAD_CONFIG_COAP_API_ENABLE == 0)
#error "OPENTHREAD_CONFIG_COAP_API_ENABLE needs to be defined and set to 1"
#endif

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/* read attribute */
#define ATTR_READ     0x01
/* write attribute */
#define ATTR_WRITE    0x02
/* report attribute */
#define ATTR_REPORT   0x04

/**
 * @brief Size of the message queue for `Shade_procQueue`
 *
 * There are 9/12 events that can be raised, it is unlikely that they will all be
 * raised at the same time. Add one buffer queue element.
 */
#if TIOP_CUI
#define SHADE_PROC_QUEUE_MAX_MSG        (13)
#else
#define SHADE_PROC_QUEUE_MAX_MSG        (10)
#endif /* TIOP_CUI */

/* coap attribute descriptor */
typedef struct
{
    const char*    uriPath; /* attribute URI */
    uint16_t       type;    /* type of resource: read only or read write */
    uint8_t*       pValue;  /* pointer to value of attribute state */

} attrDesc_t;

struct Shade_procQueueMsg
{
    Shade_evt_t evt;
};

/******************************************************************************
 Local variables
 *****************************************************************************/

/* POSIX message queue for passing events to the application processing loop */
const  char  Shade_procQueueName[] = "sh_process";
static mqd_t Shade_procQueueDesc;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_SHADE_TASK_STACK_SIZE];

/* coap resource for the application */
static otCoapResource coapResource;

/* coap attribute state of the application */
static uint8_t attrState[15] = SHADE_STATE_OPEN;

/* coap attribute discriptor for the application */
const attrDesc_t coapAttr = {
    SHADE_STATE_URI,
    (ATTR_READ|ATTR_WRITE),
    attrState,
};

static Button_Handle rightButtonHandle;

/* Holds the server setup state: True indicates CoAP server has been setup */
static bool serverSetup;

/* String variable for copying over app lines to CUI */
static char statusBuf[MAX_STATUS_LINE_VALUE_LEN];

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Shade processing thread. */
void *Shade_task(void *arg0);

/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleServer(void *aContext, otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage = NULL;
    otCoapCode messageCode = otCoapMessageGetCode(aMessage);
    otCoapType messageType = otCoapMessageGetType(aMessage);

    if (OT_COAP_CODE_GET == messageCode)
    {
        responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
        otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

        otCoapMessageInitResponse(responseMessage, aMessage,
                                  OT_COAP_TYPE_ACKNOWLEDGMENT,
                                  OT_COAP_CODE_CHANGED);
        otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                              otCoapMessageGetTokenLength(aMessage));
        otCoapMessageSetPayloadMarker(responseMessage);

        error = otMessageAppend(responseMessage, attrState,strlen((const char*)attrState));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }
    else if(OT_COAP_CODE_POST == messageCode)
    {
        char data[32];
        uint16_t offset = otMessageGetOffset(aMessage);
        uint16_t read = otMessageRead(aMessage, offset, data, sizeof(data) - 1);
        data[read] = '\0';

        /* process message */
        if(strcmp(SHADE_STATE_OPEN, data) == 0)
        {
            /* update global state */
            strcpy((char *)attrState, SHADE_STATE_OPEN);
            /* send open event */
            Shade_postEvt(Shade_evtOpen);
        }
        else if(strcmp(SHADE_STATE_CLOSED, data) == 0)
        {
            /* update global state */
            strcpy((char *)attrState, SHADE_STATE_CLOSED);
            /* send close event */
            Shade_postEvt(Shade_evtClosed);
        }
        else if(strcmp(SHADE_STATE_DRAWN, data) == 0)
        {
            /* update global state */
            strcpy((char *)attrState, SHADE_STATE_DRAWN);
            /* send drawn event */
            Shade_postEvt(Shade_evtDrawn);
        }
        else
        {
            /* no valid body, fail without response */
            otEXPECT_ACTION(false, error = OT_ERROR_NO_BUFS);
        }

        if (OT_COAP_TYPE_CONFIRMABLE == messageType)
        {
            responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
            otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

            otCoapMessageInitResponse(responseMessage, aMessage,
                                      OT_COAP_TYPE_ACKNOWLEDGMENT,
                                      OT_COAP_CODE_CHANGED);
            otCoapMessageSetToken(responseMessage,
                                  otCoapMessageGetToken(aMessage),
                                  otCoapMessageGetTokenLength(aMessage));
            otCoapMessageSetPayloadMarker(responseMessage);

            error = otMessageAppend(responseMessage, attrState, strlen((const char*)attrState));
            otEXPECT(OT_ERROR_NONE == error);

            error = otCoapSendResponse((otInstance*)aContext,
                                       responseMessage, aMessageInfo);
            otEXPECT(OT_ERROR_NONE == error);
        }
    }

exit:

    if (error != OT_ERROR_NONE && responseMessage != NULL)
    {
        otMessageFree(responseMessage);
    }
}

/**
 * @brief sets up the application coap server.
 *
 * @param aInstance A pointer to the context information.
 * @param attr      Attribute data
 *
 * @return OT_ERROR_NONE if successful, else error code
 */
static otError setupCoapServer(otInstance *aInstance, const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        coapResource.mHandler = &coapHandleServer;
        coapResource.mUriPath = (const char*)attr->uriPath;
        coapResource.mContext = aInstance;

        OtRtosApi_lock();
        otCoapAddResource(aInstance, &(coapResource));
        OtRtosApi_unlock();
    }

exit:
    return error;
}

/**
 * @brief Handles the key press events.
 *
 * @param _buttonHandle identifies which keys were pressed
 * @param _buttonEvents identifies the event that occurred on the key
 * @return None
 */
void processKeyChangeCB(Button_Handle _buttonHandle, Button_EventMask _buttonEvents)
{
    if (_buttonHandle == rightButtonHandle && _buttonEvents & Button_EV_CLICKED)
    {
        Shade_postEvt(Shade_evtKeyRight);
    }
}

/**
 * @brief Processes the OT stack events
 *
 * @param event             event identifier
 * @param aContext          context pointer for the event
 *
 * @return None
 */
static void processOtStackEvents(uint8_t event, void *aContext)
{
    (void) aContext;

    switch (event)
    {
        case OT_STACK_EVENT_NWK_JOINED:
        {
            Shade_postEvt(Shade_evtNwkJoined);
            break;
        }

        case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        {
            Shade_postEvt(Shade_evtNwkJoinFailure);
            break;
        }

        case OT_STACK_EVENT_NWK_DATA_CHANGED:
        {
            Shade_postEvt(Shade_evtNwkSetup);
            break;
        }

        case OT_STACK_EVENT_DEV_ROLE_CHANGED:
        {
            Shade_postEvt(Shade_evtDevRoleChanged);
            break;
        }

        default:
        {
            /* do nothing */
            break;
        }
    }
}

/**
 * @brief Processes the events.
 *
 * @param event             event to process in the switch
 *
 * @return None
 */
static void processEvent(Shade_evt_t event)
{
    switch (event)
    {
        case Shade_evtOpen:
        {
            /* perform activity related to the shade open event. */
#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_CYAN "Shade State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET, (char*)attrState);
            tiopCUIUpdateApp((char*)statusBuf);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF( 0, 0, "Shade Open Event received");
            DispUtils_lcdDraw(&Images_shadeOpen);
            break;
        }

        case Shade_evtClosed:
        {
            /* perform activity related to the shade closed event */
#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_CYAN "Shade State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET, (char*)attrState);
            tiopCUIUpdateApp((char*)statusBuf);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF( 0, 0, "Shade close Event received");
            DispUtils_lcdDraw(&Images_shadeClosed);
            break;
        }

        case Shade_evtDrawn:
        {
           /* perform activity related to the shade drawn event */
#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_CYAN "Shade State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET, (char*)attrState);
            tiopCUIUpdateApp((char*)statusBuf);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF(0, 0, "Shade drawn Event received");
            DispUtils_lcdDraw(&Images_shadeDrawn);
            break;
        }

        case Shade_evtNwkSetup:
        {
            if (false == serverSetup)
            {
                serverSetup = true;
                (void)setupCoapServer(OtInstance_get(), &coapAttr);

                /* display unlock image on LCD */
                DISPUTILS_SERIALPRINTF(1, 0, "CoAP server setup done");
                DispUtils_lcdDraw(&Images_shadeOpen);
            }
            break;
        }

        case Shade_evtKeyRight:
        {
            if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
            {
#if TIOP_CUI
                tiopCUIUpdateConnStatus(CUI_conn_joining);
#endif /* TIOP_CUI */
                DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
                DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");

                OtStack_joinConfiguredNetwork();
            }
            break;
        }

        case Shade_evtKeyLeft:
        {
            /* do nothing */
            break;
        }

        case Shade_evtNwkJoined:
        {
            DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");
            DISPUTILS_LCDPRINTF(1, 0, "Joined Nwk");

            (void)OtStack_setupNetwork();
#if TIOP_CUI
            OtRtosApi_lock();
            tiopCUIUpdatePANID(otLinkGetPanId(OtInstance_get()));
            tiopCUIUpdateChannel(otLinkGetChannel(OtInstance_get()));
            tiopCUIUpdateShortAddr(otLinkGetShortAddress(OtInstance_get()));
            tiopCUIUpdateNwkName(otThreadGetNetworkName(OtInstance_get()));
            tiopCUIUpdateMasterkey(*(otThreadGetMasterKey(OtInstance_get())));
            tiopCUIUpdateExtPANID(*(otThreadGetExtendedPanId(OtInstance_get())));
            OtRtosApi_unlock();
#endif /* TIOP_CUI */
            break;
        }

        case Shade_evtNwkJoinFailure:
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_join_fail);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
            DISPUTILS_LCDPRINTF(1, 0, "Join Failure");
            break;
        }
#if TIOP_CUI
        case Shade_evtProcessMenuUpdate:
        {
            CUI_processMenuUpdate();
            break;
        }

        case Shade_evtNwkAttach:
        {
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
            tiopCUIUpdateConnStatus(CUI_conn_joining);
            (void)OtStack_setupInterfaceAndNetwork();
            break;
        }

        case Shade_evtNwkJoin:
        {
            if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
            {
                DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
                tiopCUIUpdateConnStatus(CUI_conn_joining);
                OtStack_joinConfiguredNetwork();
                break;
            }
        }
#endif /* TIOP_CUI */

        case Shade_evtDevRoleChanged:
        {
            OtRtosApi_lock();
            otDeviceRole role = otThreadGetDeviceRole(OtInstance_get());
            OtRtosApi_unlock();
#ifndef TIOP_POWER_MEASUREMENT

            /* This function handles LEDs as well as CUI updates. In future
             * it's role may be reduced and need to be folded into the
             * following `#if TIOP_CUI`.
             */
            tiopCUIUpdateRole(role);
#if TIOP_CUI
            switch (role)
            {
                case OT_DEVICE_ROLE_DISABLED:
                case OT_DEVICE_ROLE_DETACHED:
                    break;

                case OT_DEVICE_ROLE_CHILD:
                case OT_DEVICE_ROLE_ROUTER:
                case OT_DEVICE_ROLE_LEADER:
                {
                    tiopCUIUpdateConnStatus(CUI_conn_joined);

                    OtRtosApi_lock();
                    tiopCUIUpdatePANID(otLinkGetPanId(OtInstance_get()));
                    tiopCUIUpdateChannel(otLinkGetChannel(OtInstance_get()));
                    tiopCUIUpdateShortAddr(otLinkGetShortAddress(OtInstance_get()));
                    tiopCUIUpdateNwkName(otThreadGetNetworkName(OtInstance_get()));
                    tiopCUIUpdateMasterkey(*(otThreadGetMasterKey(OtInstance_get())));
                    tiopCUIUpdateExtPANID(*(otThreadGetExtendedPanId(OtInstance_get())));
                    OtRtosApi_unlock();
                    break;
                }

                default:
                {
                    break;
                }
            }
#endif /* TIOP_CUI */
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }
    }
}

/******************************************************************************
 External Functions
 *****************************************************************************/

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartReceived(const uint8_t *aBuf, uint16_t aBufLength)
{
    (void)aBuf;
    (void)aBufLength;
    /* Do nothing. */
}

/**
 * Documented in openthread/platform/uart.h.
 */
void otPlatUartSendDone(void)
{
    /* Do nothing. */
}

/* Documented in shade.h */
void Shade_postEvt(Shade_evt_t event)
{
    struct Shade_procQueueMsg msg;
    int                       ret;
    msg.evt = event;
    ret = mq_send(Shade_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);
    (void)ret;
}

#if TIOP_CUI
/**
 * documented in tiop_ui.h
 */
void processMenuUpdateFn(void)
{
    Shade_postEvt(Shade_evtProcessMenuUpdate);
}

/**
 * documented in tiop_ui.h
 */
void shade_toggleOpen(const int32_t _itemEntry)
{
    /* send open event */
    strcpy((char *)attrState, SHADE_STATE_OPEN);
    Shade_postEvt(Shade_evtOpen);
}

/**
 * documented in tiop_ui.h
 */
void shade_toggleClose(const int32_t _itemEntry)
{
    /* send close event */
    strcpy((char *)attrState, SHADE_STATE_CLOSED);
    Shade_postEvt(Shade_evtClosed);
}

/**
 * documented in tiop_ui.h
 */
void shade_toggleDraw(const int32_t _itemEntry)
{
    /* send drawn event */
    strcpy((char *)attrState, SHADE_STATE_DRAWN);
    Shade_postEvt(Shade_evtDrawn);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUINwkReset(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    if (CUI_ITEM_INTERCEPT_START == _input)
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
        strncpy(_pLines[2], "Resetting, Please wait...", MAX_MENU_LINE_LEN);
    }
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIReset(const char _input, char* _pLines[3], CUI_cursorInfo_t* _pCurInfo)
{
    if (CUI_ITEM_INTERCEPT_START == _input)
    {
        OtRtosApi_lock();
        otInstanceReset(OtInstance_get());
        OtRtosApi_unlock();
        strncpy(_pLines[2], "Resetting, Please wait...", MAX_MENU_LINE_LEN);
    }
}

/**
 * documented in tiop_ui.h
 */
void uiActionAttach(const int32_t _itemEntry)
{
    Shade_postEvt(Shade_evtNwkAttach);
}

/**
 * documented in tiop_ui.h
 */
void uiActionJoin(const int32_t _itemEntry)
{
    Shade_postEvt(Shade_evtNwkJoin);
}
#endif /* TIOP_CUI */

/**
 * Documented in task_config.h.
 */
void Shade_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_SHADE_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)stack,
                                 TASK_CONFIG_SHADE_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, Shade_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void)retc;
}

/**
 *  Shade processing thread.
 */
void *Shade_task(void *arg0)
{
    struct mq_attr attr;
#if !TIOP_CONFIG_SET_NW_ID
    bool           commissioned;
#endif /* !TIOP_CONFIG_SET_NW_ID */
    mqd_t          procQueueLoopDesc;

    attr.mq_curmsgs = 0;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = SHADE_PROC_QUEUE_MAX_MSG;
    attr.mq_msgsize = sizeof(struct Shade_procQueueMsg);

    /* Open the processing queue in non-blocking mode for the notify
     * callback functions
     */
    Shade_procQueueDesc = mq_open(Shade_procQueueName,
                                  (O_WRONLY | O_NONBLOCK | O_CREAT),
                                  0, &attr);

    /* Open the processing queue in blocking read mode for the process loop */
    procQueueLoopDesc = mq_open(Shade_procQueueName, O_RDONLY, 0, NULL);

    DispUtils_open();

    OtStack_taskCreate();

    OtStack_registerCallback(processOtStackEvents);

    snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_CYAN "Shade State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET, (char*)attrState);
    tiopCUIInit((char*)statusBuf, &rightButtonHandle);

    DISPUTILS_SERIALPRINTF(0, 0, "Shade init!");

#if !TIOP_CONFIG_SET_NW_ID
    OtRtosApi_lock();
    commissioned = otDatasetIsCommissioned(OtInstance_get());
    OtRtosApi_unlock();

    if (false == commissioned)
    {
        otExtAddress extAddress;

        OtRtosApi_lock();
        otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
        OtRtosApi_unlock();

        DISPUTILS_SERIALPRINTF(2, 0, "pskd: %s", TIOP_CONFIG_PSKD);
        DISPUTILS_SERIALPRINTF(3, 0, "EUI64: 0x%02x%02x%02x%02x%02x%02x%02x%02x",
                               extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                               extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                               extAddress.m8[6], extAddress.m8[7]);

        DISPUTILS_LCDPRINTF(2, 0, "pskd:");
        DISPUTILS_LCDPRINTF(3, 0, "%s", TIOP_CONFIG_PSKD);
        DISPUTILS_LCDPRINTF(4, 0, "EUI64:");
        DISPUTILS_LCDPRINTF(5, 0, "%02x%02x%02x%02x%02x%02x%02x%02x",
                            extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
                            extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
                            extAddress.m8[6], extAddress.m8[7]);

    }
    else
    {
        OtStack_setupInterfaceAndNetwork();
    }
#else
    OtStack_setupInterfaceAndNetwork();
#endif /* !TIOP_CONFIG_SET_NW_ID */

    /* process events */
    while (1)
    {
        struct Shade_procQueueMsg msg;
        ssize_t                   ret;

        ret = mq_receive(procQueueLoopDesc, (char *)&msg, sizeof(msg), NULL);
        /* priorities are ignored */
        if (ret < 0 || ret != sizeof(msg))
        {
            /* there was an error on receive or we did not receive a full message */
            continue;
        }

        processEvent(msg.evt);
    }
}
