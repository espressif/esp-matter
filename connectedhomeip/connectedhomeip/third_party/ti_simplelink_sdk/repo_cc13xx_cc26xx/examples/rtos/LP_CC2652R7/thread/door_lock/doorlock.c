/******************************************************************************

 @file doorlock.c

 @brief Door lock example application

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
#include <openthread/dataset.h>
#include <openthread/platform/uart.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <mqueue.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/apps/Button.h>
#include <ti/drivers/apps/LED.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Board Header files */
#include "ti_drivers_config.h"

#include "images.h"
#include "doorlock.h"
#include "utils/code_utils.h"
#if TIOP_OAD
#include "keys_utils.h"
#else
#include "tiop_ui.h"
#endif /* TIOP_OAD */
#include "disp_utils.h"
#include "otstack.h"

#if TIOP_OAD
/* OAD required Header files */
#include "oad/oad.h"
#include "oad_image_header.h"
/* Low level driverlib files (non-rtos) */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#endif /* TIOP_OAD */

/* Private configuration Header files */
#include "task_config.h"
#include "tiop_config.h"

#if !TIOP_OAD
#if TIOP_CUI
/* CUI Header file */
#include "cui.h"
#endif /* TIOP_CUI */
#endif /* TIOP_OAD */

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
 * @brief Size of the message queue for `Doorlock_procQueue`
 *
 * There are 10/13 events that can be raised, it is unlikely that they will all be
 * raised at the same time. Add one buffer queue element.
 */
#if TIOP_CUI
#define DOORLOCK_PROC_QUEUE_MAX_MSG     (14)
#else
#define DOORLOCK_PROC_QUEUE_MAX_MSG     (11)
#endif /* TIOP_CUI */

/* coap attribute descriptor */
typedef struct
{
    const char*     uriPath; /* attribute URI */
    uint16_t        type;    /* type of resource: read only or read write */
    uint8_t*        pValue;  /* pointer to value of attribute state */
} attrDesc_t;


struct Doorlock_procQueueMsg {
    appEvent_e evt;
};

/******************************************************************************
 Local variables
 *****************************************************************************/

/* POSIX message queue for passing events to the application processing loop. */
const  char  Doorlock_procQueueName[] = "/dl_process";
static mqd_t Doorlock_procQueueDesc;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_DOORLOCK_TASK_STACK_SIZE];

/* coap resource for the application */
static otCoapResource coapResource;

/* coap attribute state of the application */
static uint8_t attrState[15] = DOORLOCK_STATE_UNLOCK;

/* CoAP attribute descriptor for the application */
const attrDesc_t coapAttr = {
    DOORLOCK_STATE_URI,
    (ATTR_READ|ATTR_WRITE),
    attrState,
};

/* Holds the server setup state: 1 indicates CoAP server has been setup */
static bool serverSetup = false;
#if !TIOP_OAD
static Button_Handle rightButtonHandle;
/* String variable for copying over app lines to CUI */
static char statusBuf[MAX_STATUS_LINE_VALUE_LEN];
#endif /* TIOP_OAD */

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Door lock processing thread. */
void *DoorLock_task(void *arg0);


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

        error = otMessageAppend(responseMessage, attrState,
                                strlen((const char*)attrState));
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
        if(strcmp(DOORLOCK_STATE_LOCK, data) == 0)
        {
            /* update the attribute state */
            strcpy((char *)attrState, DOORLOCK_STATE_LOCK);
            /* send lock event */
            app_postEvt(DoorLock_evtLock);
        }
        else if(strcmp(DOORLOCK_STATE_UNLOCK, data) == 0)
        {
            /* update the attribute state */
            strcpy((char *)attrState, DOORLOCK_STATE_UNLOCK);
            /* send unlock event */
            app_postEvt(DoorLock_evtUnlock);
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

            error = otMessageAppend(responseMessage, attrState,
                                    strlen((const char*)attrState));
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
#if TIOP_OAD
/**
 * @brief Handles the key press events.
 *
 * @param keysPressed identifies which keys were pressed
 * @return None
 */
static void processKeyChangeCB(uint8_t keysPressed)
{
    if (keysPressed & KEYS_RIGHT)
    {
        app_postEvt(DoorLock_evtKeyRight);
    }
}
#else
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
        app_postEvt(DoorLock_evtKeyRight);
    }
}
#endif /* TIOP_OAD */

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
            app_postEvt(DoorLock_evtNwkJoined);
            break;
        }

        case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        {
            app_postEvt(DoorLock_evtNwkJoinFailure);
            break;
        }

        case OT_STACK_EVENT_NWK_DATA_CHANGED:
        {
            app_postEvt(DoorLock_evtNwkSetup);
            break;
        }

        case OT_STACK_EVENT_DEV_ROLE_CHANGED:
        {
            app_postEvt(DoorLock_evtDevRoleChanged);
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
static void processEvent(appEvent_e event)
{
    switch (event)
    {
        case DoorLock_evtLock:
        {
            /* perform activity related to the lock event. */
#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf),
                    "[" CUI_COLOR_CYAN "Doorlock State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET,
                    (char*)attrState);
            tiopCUIUpdateApp((char*)statusBuf);
#else
            DISPUTILS_SERIALPRINTF( 0, 0, "Lock Event received");
#endif /* TIOP_CUI */
#if BOARD_DISPLAY_USE_LCD
            DispUtils_lcdDraw(&doorLockImage);
#endif /* BOARD_DISPLAY_USE_UART */
            break;
        }

        case DoorLock_evtUnlock:
        {
            /* perform activity related to the un lock event */
#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf),
                    "[" CUI_COLOR_CYAN "Doorlock State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET,
                    (char*)attrState);
            tiopCUIUpdateApp((char*)statusBuf);
#else
            DISPUTILS_SERIALPRINTF( 0, 0, "Unlock Event received");
#endif /* TIOP_CUI */
#if BOARD_DISPLAY_USE_LCD
            DispUtils_lcdDraw(&doorUnlockImage);
#endif /* BOARD_DISPLAY_USE_UART */
            break;
        }

        case DoorLock_evtNwkSetup:
        {
            if (false == serverSetup)
            {
#if TIOP_OAD
                const otIp6Address *ipAddr_p;
#endif /* TIOP_OAD */

                serverSetup = true;
                (void)setupCoapServer(OtInstance_get(), &coapAttr);
#if TIOP_OAD
                /*setup OAD CoAP resources */
                OAD_open();

                ipAddr_p = otThreadGetMeshLocalEid(OtInstance_get());
#endif /* TIOP_OAD */
#if BOARD_DISPLAY_USE_LCD
                /* display unlock image on LCD */
                DispUtils_lcdDraw(&doorUnlockImage);
#endif /* BOARD_DISPLAY_USE_UART */
            }
            break;
        }

        case DoorLock_evtKeyRight:
        {
            if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
            {
#if TIOP_CUI
                tiopCUIUpdateConnStatus(CUI_conn_joining);
#else
                DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");
#endif /* TIOP_CUI */
                DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
                OtStack_joinConfiguredNetwork();
            }
            break;
        }

        case DoorLock_evtNwkJoined:
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_joined);
#else
            DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");
#endif /* TIOP_CUI */
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

        case DoorLock_evtNwkJoinFailure:
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_join_fail);
#else
            DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
#endif /* TIOP_CUI */
            DISPUTILS_LCDPRINTF(1, 0, "Join Failure");
            break;
        }

#if TIOP_CUI
        case DoorLock_evtProcessMenuUpdate:
        {
            CUI_processMenuUpdate();
            break;
        }

        case DoorLock_evtNwkAttach:
        {
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
            tiopCUIUpdateConnStatus(CUI_conn_joining);
            (void)OtStack_setupInterfaceAndNetwork();
        }

        case DoorLock_evtNwkJoin:
        {
            if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
            {
                DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
                tiopCUIUpdateConnStatus(CUI_conn_joining);
                OtStack_joinConfiguredNetwork();
            }
        }
#endif /* TIOP_CUI */

        case DoorLock_evtDevRoleChanged:
        {
            OtRtosApi_lock();
            otDeviceRole role = otThreadGetDeviceRole(OtInstance_get());
            OtRtosApi_unlock();
#if TIOP_OAD
            switch (role)
            {
                case OT_DEVICE_ROLE_DISABLED:
                {
                    GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
                    GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_OFF);
                    break;
                }

                case OT_DEVICE_ROLE_DETACHED:
                {
                    GPIO_write(CONFIG_GPIO_GLED, CONFIG_GPIO_LED_OFF);
                    GPIO_write(CONFIG_GPIO_RLED, CONFIG_GPIO_LED_ON);
                    DISPUTILS_SERIALPRINTF(1, 0, "Device Detached");
                    OAD_pause();
                    break;
                }

                case OT_DEVICE_ROLE_CHILD:
                case OT_DEVICE_ROLE_ROUTER:
                case OT_DEVICE_ROLE_LEADER:
                {
                    DISPUTILS_SERIALPRINTF(1, 0, "Device Joined");
                    //OAD_resume();
                    break;
                }
            }
#else
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
#endif /* TIOP_OAD */
            break;
        }

        case OAD_queueEvt:
        {
#if TIOP_OAD
            /* perform activity related to the OAD Download. */
            OAD_processQueue();
#endif /* TIOP_OAD */
            break;
        }

        case OAD_CtrlRegEvt:
        {
#if TIOP_OAD
            /* perform activity related to the OAD Download. */
            OAD_processCtrlEvents(event);
#endif /* TIOP_OAD */
            break;
        }

        default:
        {
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

/* Documented in doorlock.h */
void app_postEvt(appEvent_e event)
{
    struct Doorlock_procQueueMsg msg;
    int                          ret;
    msg.evt = event;
    ret = mq_send(Doorlock_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);
    (void)ret;
}

#if TIOP_CUI
/**
 * documented in tiop_ui.h
 */
void processMenuUpdateFn(void)
{
    app_postEvt(DoorLock_evtProcessMenuUpdate);
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
    app_postEvt(DoorLock_evtNwkAttach);
}

/**
 * documented in tiop_ui.h
 */
void uiActionJoin(const int32_t _itemEntry)
{
    app_postEvt(DoorLock_evtNwkJoin);
}

/**
 * documented in tiop_ui.h
 */
void doorlock_toggleLock(const int32_t _itemEntry)
{
    if(strcmp(DOORLOCK_STATE_LOCK, (char*)attrState) == 0)
    {
        /* send lock event */
        strcpy((char *)attrState, DOORLOCK_STATE_UNLOCK);
        app_postEvt(DoorLock_evtUnlock);

    }
    else if(strcmp(DOORLOCK_STATE_UNLOCK, (char*)attrState) == 0)
    {
        /* send unlock event */
        strcpy((char *)attrState, DOORLOCK_STATE_LOCK);
        app_postEvt(DoorLock_evtLock);
    }

}
#endif /* TIOP_CUI */

/**
 * Documented in task_config.h.
 */
void DoorLock_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_DOORLOCK_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)stack,
                                 TASK_CONFIG_DOORLOCK_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, DoorLock_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void)retc;
#if TIOP_OAD
    GPIO_setConfig(CONFIG_GPIO_BTN1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(CONFIG_GPIO_BTN2, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
    GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
    GPIO_setConfig(CONFIG_GPIO_RLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
#endif /* TIOP_OAD */
}

/**
 * @brief Invalidate the OAD IMAGE HEADER.
 *
 * This provides a way to revert to factory image by invalidating the existing
 * stack/application image and doing a system reset. On boot, the BIM
 * on finding the internal invalidated image will restore to factory image.
 *
 * @return None
 */
#if TIOP_OAD
static void TIOP_OAD_invalidate_image_header(void)
{
    /* our data buffer cannot be in flash... so it is on the stack */
    uint8_t zeros[sizeof(oad_image_header.h.imgID)];
    /*
     * We only need to invalidate the IMAGE header
     * We can do this by writing a zero  "zeros" over the signature.
     */

    /* no IRQ chance, we disable here */
    CPUcpsid();

    memset(zeros, 0, sizeof(zeros));
    FlashProgram(&zeros[0],
                  (uint32_t)(&oad_image_header.h.imgID),
                  sizeof(oad_image_header.h.imgID));

    /* press the virtual reset button */
    SysCtrlSystemReset();
}
#endif

/**
 *  Door lock processing thread.
 */
void *DoorLock_task(void *arg0)
{
    struct mq_attr attr;
#if !TIOP_CONFIG_SET_NW_ID
    bool           commissioned;
#endif /* !TIOP_CONFIG_SET_NW_ID */
    mqd_t          procQueueLoopDesc;

    attr.mq_curmsgs = 0;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = DOORLOCK_PROC_QUEUE_MAX_MSG;
    attr.mq_msgsize = sizeof(struct Doorlock_procQueueMsg);

    /* Open The processing queue in non-blocking write mode for the notify
     * callback functions
     */
    Doorlock_procQueueDesc = mq_open(Doorlock_procQueueName,
                                     (O_WRONLY | O_NONBLOCK | O_CREAT),
                                     0, &attr);

    /* Open the processing queue in blocking read mode for the process loop */
    procQueueLoopDesc = mq_open(Doorlock_procQueueName, O_RDONLY, 0, NULL);

#if TIOP_OAD
    KeysUtils_initialize(processKeyChangeCB);
#endif /* TIOP_OAD */

    DispUtils_open();

    OtStack_taskCreate();

    OtStack_registerCallback(processOtStackEvents);
#if TIOP_OAD
#ifndef TIOP_POWER_MEASUREMENT
    /* If button 2 is pressed on boot, reset the OpenThread settings */
    if (!GPIO_read(CONFIG_GPIO_BTN2))
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
    }
#if TIOP_OAD
    /* The BTN, when not pressed reads as 1, when pressed reads as 0 */
    if(!GPIO_read(CONFIG_GPIO_BTN1))
    {
        TIOP_OAD_invalidate_image_header();
    }
#endif
#endif /* !TIOP_POWER_MEASUREMENT */
#else
    snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_CYAN "Doorlock State" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET, (char*)attrState);
    tiopCUIInit((char*)statusBuf, &rightButtonHandle);
#endif /* TIOP_OAD */

#if !TIOP_CUI
    DISPUTILS_SERIALPRINTF(0, 0, "Door Lock init!");
#endif /* !TIOP_CUI */

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
        DISPUTILS_LCDPRINTF(7, 0, "%02x%02x%02x%02x%02x%02x%02x%02x",
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
        struct Doorlock_procQueueMsg msg;
        ssize_t ret;

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
