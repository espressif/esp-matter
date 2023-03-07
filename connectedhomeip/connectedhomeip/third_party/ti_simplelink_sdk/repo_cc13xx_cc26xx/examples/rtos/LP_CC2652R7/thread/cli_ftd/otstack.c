/******************************************************************************

 @file otstack.c

 @brief OpenThread stack processing and instantiation and handling of
        application CoAP server.

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
#include OPENTHREAD_PROJECT_CORE_CONFIG_FILE

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>

/* POSIX Header files */
#include <mqueue.h>
#include <pthread.h>
#include <sched.h>

/* OpenThread public API Header files */
#include <openthread/coap.h>
#include <openthread/dataset.h>
#include <openthread/diag.h>
#include <openthread/joiner.h>
#include <openthread/platform/settings.h>
#include <openthread/tasklet.h>
#include <openthread/thread.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otinstance.h"
#include "otsupport/otrtosapi.h"
#include "platform/system.h"

/* Example/Board Header files */
#include "ti_drivers_config.h"
#include "otstack.h"
#include "task_config.h"
#include "utils/code_utils.h"

/* Configuration Header files */
#include "tiop_config.h"
#if TIOP_OAD
#include "oad_image_header.h"
/* Low level driverlib files (non-rtos) */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#endif /* TIOP_OAD */

/* OpenThread Stack thread */
extern void *OtStack_task(void *arg0);

/******************************************************************************
 Constants and definitions
 *****************************************************************************/

/**
 * @brief Size of the message queue for `OtStack_procQueue`
 *
 * Size determined by:
 *  7   main processing loop commands
 *  6   radio process requests
 *  2   UART process requests
 * +1   buffer
 * -----------------------------------
 *  16  queue slots
 */
#define OTSTACK_PROC_QUEUE_MAX_MSG      (16)

enum OtStack_procQueueCmd
{
    OtStack_procQueueCmd_alarm,
    OtStack_procQueueCmd_radio,
    OtStack_procQueueCmd_tasklets,
    OtStack_procQueueCmd_uart,
    OtStack_procQueueCmd_random,
    OtStack_procQueueCmd_alarmu,
    OtStack_procQueueCmd_spi,
};


struct OtStack_procQueueMsg {
    enum OtStack_procQueueCmd cmd;
    uintptr_t arg;
};

/******************************************************************************
 Local variables
 *****************************************************************************/

static otInstance *OtStack_instance = NULL;

/* POSIX message queue for passing state to the processing loop */
const  char  OtStack_procQueueName[] = "/tiop_process";
static mqd_t OtStack_procQueueDesc;

/* OpenThread Stack thread call stack */
static char OtStack_stack[TASK_CONFIG_OT_TASK_STACK_SIZE];

/* Pointer to application callback */
static OtStack_EventsCallback_t appEventHandler = NULL;

/* Holds the stack events related to network */
static volatile uint8_t otStackEvents = OT_STACK_EVENT_NWK_NOT_JOINED;

/******************************************************************************
 Local Functions
 *****************************************************************************/

/**
 * @brief callback function registered with the OpenThread to
 *        get the joining network status.
 *
 * @param aError   error value returned during the joining
 *                 process.
 * @param aContext context if any (ignored).
 * @return None
 */
void joinerCallback(otError aError, void *aContext)
{
    (void)aContext;

    if(aError == OT_ERROR_NONE)
    {
        otStackEvents = OT_STACK_EVENT_NWK_JOINED;
    }
    else
    {
        otStackEvents = OT_STACK_EVENT_NWK_JOINED_FAILURE;
    }

    if (appEventHandler)
    {
        appEventHandler(otStackEvents, NULL);
    }
}

/******************************************************************************
 External Functions
 *****************************************************************************/

/* Documented in otstack.h */
void handleNetifStateChanged(uint32_t aFlags, void *aContext)
{
    if(aFlags & OT_CHANGED_THREAD_NETDATA)
    {
        /* post the network setup done event to the registered app */
        otStackEvents = OT_STACK_EVENT_NWK_DATA_CHANGED;
        appEventHandler(otStackEvents, NULL);
    }

    if(aFlags & OT_CHANGED_THREAD_ROLE)
    {
        /* post the network setup done event to the registered app */
        otStackEvents = OT_STACK_EVENT_DEV_ROLE_CHANGED;
        appEventHandler(otStackEvents, NULL);
    }
    return;
}


/* Documented in otstack.h */
void OtStack_registerCallback(OtStack_EventsCallback_t appCB)
{
    appEventHandler = appCB;
}

/* Documented in otstack.h */
uint8_t OtStack_joinState(void)
{
    return otStackEvents;
}

/* Documented in otstack.h */
void OtStack_joinNetwork(const char* pskd)
{
    OtRtosApi_lock();
    otIp6SetEnabled(OtInstance_get(), true);
    if (OT_ERROR_NONE == otJoinerStart(OtInstance_get(), pskd, NULL, PACKAGE_NAME, OPENTHREAD_CONFIG_PLATFORM_INFO,
                  PACKAGE_VERSION, NULL, joinerCallback, NULL))
    {
        otStackEvents = OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS;
    }
    OtRtosApi_unlock();
}

/* Documented in otstack.h */
void OtStack_joinConfiguredNetwork(void)
{
    OtRtosApi_lock();
    otIp6SetEnabled(OtInstance_get(), true);
    if (OT_ERROR_NONE == TIOP_configuredJoinerStart(OtInstance_get(), NULL, PACKAGE_NAME,
                  OPENTHREAD_CONFIG_PLATFORM_INFO, PACKAGE_VERSION, NULL, joinerCallback, NULL))
    {
        otStackEvents = OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS;
    }
    OtRtosApi_unlock();
}

/* Documented in otstack.h */
bool OtStack_setupNetwork(void)
{
    bool status = false;
    OtRtosApi_lock();
    if (otIp6IsEnabled(OtInstance_get()))
    {
        /* Only try to start Thread if we could bring up the interface */
        if (otThreadSetEnabled(OtInstance_get(), true) == OT_ERROR_NONE)
        {
            status = true;
            /* Register the network interface state change callback */
            otSetStateChangedCallback(OtInstance_get(), handleNetifStateChanged,
                                      (void *)OtInstance_get());
        }
    }
    OtRtosApi_unlock();
    return status;
}

/* Documented in otstack.h */
bool OtStack_setupInterfaceAndNetwork(void)
{
    bool status = false;
    OtRtosApi_lock();
    if (otIp6SetEnabled(OtInstance_get(), true) == OT_ERROR_NONE)
    {
        // Only try to start Thread if we could bring up the interface
        if (otThreadSetEnabled(OtInstance_get(), true) != OT_ERROR_NONE)
        {
            /* Bring the interface down if Thread failed to start */
            otIp6SetEnabled(OtInstance_get(), false);
        }
        else
        {
            status = true;
            /* Register the network interface state change callback */
            otSetStateChangedCallback(OtInstance_get(), handleNetifStateChanged,
                                      (void *)OtInstance_get());
        }
    }
    OtRtosApi_unlock();
    return status;
}

/**
 * Documented in task_config.h.
 */
void OtStack_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    /* create api gate */
    OtRtosApi_init();

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_OT_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc = pthread_attr_setstack(&pAttrs, (void *)OtStack_stack,
                                 TASK_CONFIG_OT_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, OtStack_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

    (void) retc;
}

/**
 * Callback from OpenThread stack to indicate tasklets are pending processing.
 */
void otTaskletsSignalPending(otInstance *aInstance)
{
    (void)aInstance;
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_tasklets;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}

/**
 * Callback from the alarm module indicating need for processing.
 */
void platformAlarmSignal()
{
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_alarm;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}

/**
 * Callback from the alarm module indicating need for processing.
 */
void platformAlarmMicroSignal()
{
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_alarmu;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}

/**
 * Callback from the alarm module indicating need for processing.
 */
void platformUartSignal(uintptr_t arg)
{
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_uart;
    msg.arg = arg;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}

/**
 * Callback from the spi module indicating need for processing.
 */
void platformSpiSignal()
{
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_spi;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}

/**
 * Callback from the radio module indicating need for processing.
 */
void platformRadioSignal(uintptr_t arg)
{
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_radio;
    msg.arg = arg;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}

/**
 * Callback from the random generator module indicating need for
 * processing.
 */
void platformRandomSignal(void)
{
    struct OtStack_procQueueMsg msg;
    int                         ret;
    msg.cmd = OtStack_procQueueCmd_random;
    ret = mq_send(OtStack_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);

    (void) ret;
}


/**
 * Documented in otsupport/otinstance.h.
 */
otInstance *OtInstance_get(void)
{
    otInstance *ret;

    if (NULL != OtStack_instance)
    {
        ret = OtStack_instance;
    }
    else
    {
        /* lock and unlock the API to make sure that the stack is initialized
         *  before the caller tries to use this pointer.
         */
        OtRtosApi_lock();
        ret = OtStack_instance;
        OtRtosApi_unlock();
    }

    return ret;
}

/**
 * Main processing thread for OpenThread Stack.
 */
void *OtStack_task(void *arg0)
{
    struct mq_attr attr;
    mqd_t          procQueueLoopDesc;

    attr.mq_curmsgs = 0;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = OTSTACK_PROC_QUEUE_MAX_MSG;
    attr.mq_msgsize = sizeof(struct OtStack_procQueueMsg);

    /* Open The processing queue in non-blocking write mode for the notify
     * callback functions
     */
    OtStack_procQueueDesc = mq_open(OtStack_procQueueName,
                                    (O_WRONLY | O_NONBLOCK | O_CREAT),
                                    0, &attr);

    /* Open the processing queue in blocking read mode for the process loop */
    procQueueLoopDesc = mq_open(OtStack_procQueueName, O_RDONLY, 0, NULL);

    /* Initialize the platform */
    otSysInit(0, NULL);

    OtStack_instance = otInstanceInitSingle();
    assert(OtStack_instance);

    /* Set the SysCfg params if the dataset has not been commissioned. */
    if (!otDatasetIsCommissioned(OtInstance_get()))
    {
        TIOP_init(OtInstance_get());
    }

    /* allow the application to lock the API */
    OtRtosApi_unlock();

    while (1)
    {
        struct OtStack_procQueueMsg msg;
        ssize_t ret;

        ret = mq_receive(procQueueLoopDesc, (char *)&msg, sizeof(msg), NULL);
        /* priorities are ignored */
        if (ret < 0 || ret != sizeof(msg))
        {
            /* there was an error on receive or we did not receive a full message */
            continue;
        }

        OtRtosApi_lock();
        switch (msg.cmd)
        {
            case OtStack_procQueueCmd_alarm:
            {
                platformAlarmProcess(OtInstance_get());
                break;
            }

            case OtStack_procQueueCmd_radio:
            {
                platformRadioProcess(OtInstance_get(), msg.arg);
                break;
            }

            case OtStack_procQueueCmd_tasklets:
            {
                otTaskletsProcess(OtInstance_get());
                break;
            }

            case OtStack_procQueueCmd_uart:
            {
#if OPENTHREAD_CONFIG_NCP_UART_ENABLE || TIOP_ENABLE_UART
                platformUartProcess(msg.arg);
#endif
                break;
            }

            case OtStack_procQueueCmd_random:
            {
                platformRandomProcess();
                break;
            }

            case OtStack_procQueueCmd_alarmu:
            {
#if OPENTHREAD_CONFIG_ENABLE_PLATFORM_USEC_TIMER
                platformAlarmMicroProcess(OtInstance_get());
#endif
                break;
            }

            case OtStack_procQueueCmd_spi:
            {
#if OPENTHREAD_CONFIG_NCP_SPI_ENABLE
                platformSpiProcess();
#endif
                break;
            }

            default:
            {
                break;
            }
        }
        OtRtosApi_unlock();
    }
}

