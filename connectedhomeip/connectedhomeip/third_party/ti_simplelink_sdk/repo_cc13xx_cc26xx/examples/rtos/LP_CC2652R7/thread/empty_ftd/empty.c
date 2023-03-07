/******************************************************************************

 @file empty.c

 @brief Main application file for the Empty TIRTOS example

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

#include <openthread/config.h>
#include <openthread-core-config.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <unistd.h>
#include <mqueue.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread public API Header files */
#include <openthread/dataset.h>
#include <openthread/platform/logging.h>
#include <openthread/platform/uart.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Example/Board Header files */
#include "ti_drivers_config.h"

#include "empty.h"
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

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/**
 * @brief Size of the message queue for `Empty_procQueue`
 *
 * There are 5/8 events that can be raised, it is unlikely that they will all be
 * raised at the same time. Add one buffer queue element.
 */
#if TIOP_CUI
#define EMPTY_PROC_QUEUE_MAX_MSG     (9)
#else
#define EMPTY_PROC_QUEUE_MAX_MSG     (6)
#endif /* TIOP_CUI */

struct Empty_procQueueMsg {
    Empty_evt evt;
};

/******************************************************************************
 Local variables
 *****************************************************************************/

/* POSIX message queue for passing events to the application processing loop. */
const  char  Empty_procQueueName[] = "/em_process";
static mqd_t Empty_procQueueDesc;

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_EMPTY_TASK_STACK_SIZE];

#if !TIOP_OAD
/* String variable for copying over app lines to CUI */
static char statusBuf[MAX_STATUS_LINE_VALUE_LEN];
#endif /* TIOP_OAD */

static Button_Handle rightButtonHandle;

/******************************************************************************
 Function Prototype
 *****************************************************************************/


/* Application thread */
void *empty_task(void *arg0);


/******************************************************************************
 Local Functions
 *****************************************************************************/

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
            Empty_postEvt(Empty_evtNwkJoined);
            break;
        }

        case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        {
            Empty_postEvt(Empty_evtNwkJoinFailure);
            break;
        }

        case OT_STACK_EVENT_NWK_DATA_CHANGED:
        {
            Empty_postEvt(Empty_evtNwkSetup);
            break;
        }

        case OT_STACK_EVENT_DEV_ROLE_CHANGED:
        {
            Empty_postEvt(Empty_evtDevRoleChanged);
            break;
        }

        default:
        {
            // do nothing
            break;
        }
    }
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
        Empty_postEvt(Empty_evtKeyRight);
    }
}


/**
 * @brief Processes the events.
 *
 * @return None
 */
static void processEvent(Empty_evt event)
{
    switch (event)
    {
        case Empty_evtNwkSetup:
        {
            //CoAP set up here
            break;
        }

        case Empty_evtKeyRight:
        {
            if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
            {
#if TIOP_CUI
                tiopCUIUpdateConnStatus(CUI_conn_joining);
#endif /* TIOP_CUI */
                DISPUTILS_SERIALPRINTF(1, 0, "Joining Nwk ...");

                OtStack_joinConfiguredNetwork();
            }
            break;
        }

        case Empty_evtNwkJoined:
        {
            DISPUTILS_SERIALPRINTF( 1, 0, "Joined Nwk");

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

        case Empty_evtNwkJoinFailure:
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_join_fail);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
            break;
        }

#if TIOP_CUI
        case Empty_evtProcessMenuUpdate:
        {
            CUI_processMenuUpdate();
            break;
        }

        case Empty_evtNwkAttach:
        {
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
            tiopCUIUpdateConnStatus(CUI_conn_joining);
            (void)OtStack_setupInterfaceAndNetwork();
            break;
        }

        case Empty_evtNwkJoin:
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

        case Empty_evtDevRoleChanged:
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

#if (OPENTHREAD_CONFIG_LOG_OUTPUT == OPENTHREAD_CONFIG_LOG_OUTPUT_APP)
/**
 * Documented in openthread/platform/logging.h.
 */
void otPlatLog(otLogLevel aLogLevel, otLogRegion aLogRegion, const char *aFormat, ...)
{
    (void)aLogLevel;
    (void)aLogRegion;
    (void)aFormat;
    /* Do nothing. */
}
#endif

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

/* Documented in empty.h */
void Empty_postEvt(Empty_evt event)
{
    struct Empty_procQueueMsg msg;
    int                            ret;
    msg.evt = event;
    ret = mq_send(Empty_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);
    (void)ret;
}

#if TIOP_CUI
/**
 * documented in tiop_ui.h
 */
void processMenuUpdateFn(void)
{
    Empty_postEvt(Empty_evtProcessMenuUpdate);
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
    Empty_postEvt(Empty_evtNwkAttach);
}

/**
 * documented in tiop_ui.h
 */
void uiActionJoin(const int32_t _itemEntry)
{
    Empty_postEvt(Empty_evtNwkJoin);
}
#endif /* TIOP_CUI */

/**
 * Documented in task_config.h.
 */
void empty_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_EMPTY_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)stack,
                                 TASK_CONFIG_EMPTY_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, empty_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void) retc;
}

/**
 * Main thread starting the Empty example within OpenThread.
 */
void *empty_task(void *arg0)
{
    struct mq_attr attr;
#if !TIOP_CONFIG_SET_NW_ID
    bool           commissioned;
#endif /* !TIOP_CONFIG_SET_NW_ID */
    mqd_t          procQueueLoopDesc;

    attr.mq_curmsgs = 0;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = EMPTY_PROC_QUEUE_MAX_MSG;
    attr.mq_msgsize = sizeof(struct Empty_procQueueMsg);

    /* Open the processing queue in non-blocking mode for the notify
     * callback functions
     */
    Empty_procQueueDesc = mq_open(Empty_procQueueName,
                                       (O_WRONLY | O_NONBLOCK | O_CREAT),
                                       0, &attr);

    /* Open the processing queue in blocking read mode for the process loop */
    procQueueLoopDesc = mq_open(Empty_procQueueName, O_RDONLY, 0, NULL);

    DispUtils_open();

    OtStack_taskCreate();

    OtStack_registerCallback(processOtStackEvents);

    snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_CYAN "Empty State" CUI_COLOR_RESET "] ");
    tiopCUIInit((char*)statusBuf, &rightButtonHandle);

    DISPUTILS_SERIALPRINTF(0, 0, "Empty Example initialized! Have fun!");

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

    }
    else
    {
        OtStack_setupInterfaceAndNetwork();
    }
#else
    OtStack_setupInterfaceAndNetwork();
#endif /* !TIOP_CONFIG_SET_NW_ID */
    while (1)
    {
        struct Empty_procQueueMsg msg;
        ssize_t                   ret;

        ret = mq_receive(procQueueLoopDesc, (char *)&msg, sizeof(msg), NULL);
        /* priorities are ignored */
        if (ret < 0 || ret != sizeof(msg))
        {
            /* something has failed */
            continue;
        }
        processEvent(msg.evt);
    }
}

