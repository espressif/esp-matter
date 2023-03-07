/******************************************************************************

 @file thermostat.c

 @brief Thermostat example application

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2020-2021, Texas Instruments Incorporated
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
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* OpenThread public API Header files */
#include <openthread/coap.h>
#include <openthread/link.h>
#include <openthread/platform/uart.h>
#include <openthread/thread.h>

/* grlib header defines `NDEBUG`, undefine here to avoid a compile warning */
#ifdef NDEBUG
#undef NDEBUG
#endif

/* TIRTOS specific driver header files */
#include <ti/grlib/grlib.h>

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

#ifdef USE_DMM
#ifdef BLE_START
#include "remote_display.h"
#include "provisioning_gatt_profile.h"
#endif
#include "ti_dmm_application_policy.h"
#endif

#include "thermostat.h"
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

/* integer value of character '0' for char to int conversion*/
const uint8_t zeroCharOffset = '0';

/* read attribute */
#define ATTR_READ     0x01
/* write attribute */
#define ATTR_WRITE    0x02
/* report attribute */
#define ATTR_REPORT   0x04
/* Number of attributes in thermostat application */
#define ATTR_COUNT  2
/* period for animation frames for LCD in milliseconds */
#define FRAME_PERIOD 230
/* Maximum number of characters for displayed temp including null terminator*/
#define TEMP_MAX_CHARS 3

/**
 * @brief Size of the message queue for `Thermostat_procQueue`
 *
 * There are 8/11 events that can be raised, it is unlikely that they will all be
 * raised at the same time. Add one buffer queue element.
 */
#if TIOP_CUI
#define THERMOSTAT_PROC_QUEUE_MAX_MSG       (12)
#else
#define THERMOSTAT_PROC_QUEUE_MAX_MSG       (9)
#endif /* TIOP_CUI */

/* coap attribute descriptor */
typedef struct
{
    const char*          uriPath; /* attribute URI */
    uint16_t             type; /* type of resource: read only or read write */
    char*                pValue;  /* pointer to value of attribute state */
    otCoapResource       *pAttrCoapResource; /* coap resource for this attr */
    otCoapRequestHandler pAttrHandlerCB;/* call back function for this attr */
} attrDesc_t;

/**
 * Thermostat Modes.
 */
typedef enum
{
  Thermostat_modeCooling,
  Thermostat_modeHeating,
} Thermostat_mode;

struct Thermostat_procQueueMsg
{
    Thermostat_evt evt;
};

/******************************************************************************
 Local variables
 *****************************************************************************/

/* POSIX message queue for passing events to the application processing loop */
const  char  Thermostat_procQueueName[] = "th_process";
static mqd_t Thermostat_procQueueDesc;
/* coap resources for the application */
static otCoapResource coapResourceTemp;
static otCoapResource coapResourceSetPt;

/* default coap attribute values of the application */
static char thermostatTemp[TEMP_MAX_CHARS] = "68";
static char thermostatSetPt[TEMP_MAX_CHARS] = "34";
static int setPoint = 34;

/* coap attribute discriptor for the application */
static attrDesc_t coapAttrs[ATTR_COUNT] = {
{
    .uriPath = THERMOSTAT_TEMP_URI,
    .type = (ATTR_READ|ATTR_WRITE),
    .pValue = thermostatTemp,
    .pAttrCoapResource = &coapResourceTemp
},
{
    .uriPath = THERMOSTAT_SET_PT_URI,
    .type = (ATTR_READ|ATTR_WRITE),
    .pValue = thermostatSetPt,
    .pAttrCoapResource = &coapResourceSetPt
}
};

static Button_Handle rightButtonHandle;

/* Holds the server setup state: 1 indicates CoAP server has been setup */
static bool serverSetup;

/* String variable for copying over app lines to CUI */
static char statusBuf[MAX_STATUS_LINE_VALUE_LEN];

#ifdef USE_DMM
static otPanId panId;
static otExtendedPanId extPanId;
static otChannelMask chanMask;
static uint8_t channel;
static otMasterKey masterKey;
static otNetworkName networkName;
static otShortAddress shortAddr;
static otExtAddress extAddress;
static otDeviceRole role;

static uint32_t deltaMask;
#endif /* USE_DMM */

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Thermostat processing thread. */
void *Thermostat_task(void *arg0);

#ifdef USE_DMM
static uint8_t Util_hiUint16(uint16_t a);
static uint8_t Util_loUint16(uint16_t a);
static uint16_t Util_buildUint16(uint8_t loByte, uint8_t hiByte);
static uint32_t Util_buildUint32(uint8_t byte0, uint8_t byte1, uint8_t byte2, uint8_t byte3);
static uint8_t Util_breakUint32(uint32_t var, int byteNum);

// Provisioning callback functions
static void provisionConnectCb(uint8_t connectType);
static void provisionDisconnectCb(void);
static void networkResetCb(uint8_t resetType);
static void setProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *const value, uint8_t len);
static void getProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *value, uint8_t len);
void updateNetworkParams(void);

RemoteDisplay_clientProvisioningCbs_t ot_ProvisioningCbs =
{
  setProvisioningCb,
  getProvisioningCb,
  provisionConnectCb,
  provisionDisconnectCb,
  networkResetCb
};
#endif /* USE_DMM */

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
static void coapHandleSetPt(void *aContext, otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapMessageGetCode(aMessage);
    otCoapType messageType = otCoapMessageGetType(aMessage);

    responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
    otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

    otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                         otCoapMessageGetTokenLength(aMessage));
    otCoapMessageSetPayloadMarker(responseMessage);

    if(OT_COAP_CODE_GET == messageCode)
    {

        error = otMessageAppend(responseMessage, thermostatSetPt,strlen((const char*)thermostatSetPt));
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

        if (read < TEMP_MAX_CHARS)
        {
            /* process message */
            strncpy(thermostatSetPt, data, TEMP_MAX_CHARS);
            Thermostat_postEvt(Thermostat_evtSetPtChange);
        }

        if (OT_COAP_TYPE_CONFIRMABLE == messageType)
        {
            error = otMessageAppend(responseMessage, thermostatSetPt,
                                    strlen((const char*)thermostatSetPt));
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
 * @brief Callback function registered with the Coap server.
 *        Processes the coap request from the clients.
 *
 * @param  aContext      A pointer to the context information.
 * @param  aMessage      A pointer to the message.
 * @param  aMessageInfo  A pointer to the message info.
 *
 * @return None
 */
static void coapHandleTemp(void *aContext, otMessage *aMessage,
                             const otMessageInfo *aMessageInfo)
{
    otError error = OT_ERROR_NONE;
    otMessage *responseMessage;
    otCoapCode responseCode = OT_COAP_CODE_CHANGED;
    otCoapCode messageCode = otCoapMessageGetCode(aMessage);
    otCoapType messageType = otCoapMessageGetType(aMessage);

    responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
    otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

    otCoapMessageInitResponse(responseMessage, aMessage, OT_COAP_TYPE_ACKNOWLEDGMENT, responseCode);
    otCoapMessageSetToken(responseMessage, otCoapMessageGetToken(aMessage),
                         otCoapMessageGetTokenLength(aMessage));
    otCoapMessageSetPayloadMarker(responseMessage);

    if(OT_COAP_CODE_GET == messageCode)
    {
        error = otMessageAppend(responseMessage, thermostatTemp,strlen((const char*)thermostatTemp));
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


        if (read < TEMP_MAX_CHARS)
        {
            /* process message */
            strncpy(thermostatTemp, data, TEMP_MAX_CHARS);
            Thermostat_postEvt(Thermostat_evtTempChange);
        }

        if (OT_COAP_TYPE_CONFIRMABLE == messageType)
        {
            error = otMessageAppend(responseMessage, thermostatTemp,
                                    strlen((const char*)thermostatTemp));
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
static otError setupCoapServer(otInstance *aInstance,
                                   const attrDesc_t *attr)
{
    otError error = OT_ERROR_NONE;

    OtRtosApi_lock();
    error = otCoapStart(aInstance, OT_DEFAULT_COAP_PORT);
    OtRtosApi_unlock();
    otEXPECT(OT_ERROR_NONE == error);

    if(attr->type & (ATTR_READ | ATTR_WRITE))
    {
        attr->pAttrCoapResource->mHandler = attr->pAttrHandlerCB;
        attr->pAttrCoapResource->mUriPath = (const char*)attr->uriPath;
        attr->pAttrCoapResource->mContext = aInstance;

        OtRtosApi_lock();
        error = otCoapAddResource(aInstance, attr->pAttrCoapResource);
        OtRtosApi_unlock();
        otEXPECT(OT_ERROR_NONE == error);
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
        Thermostat_postEvt(Thermostat_evtKeyRight);
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
#ifdef USE_DMM
    deltaMask = *(uint32_t *)aContext;
#else
    (void) aContext;
#endif

    switch (event)
    {
        case OT_STACK_EVENT_NWK_JOINED:
        {
            Thermostat_postEvt(Thermostat_evtNwkJoined);
            break;
        }

        case OT_STACK_EVENT_NWK_JOINED_FAILURE:
        {
            Thermostat_postEvt(Thermostat_evtNwkJoinFailure);
            break;
        }

        case OT_STACK_EVENT_NWK_DATA_CHANGED:
        {
            Thermostat_postEvt(Thermostat_evtNwkSetup);
            break;
        }

        case OT_STACK_EVENT_DEV_ROLE_CHANGED:
        {
            Thermostat_postEvt(Thermostat_evtDevRoleChanged);
            break;
        }
#ifdef USE_DMM
        case OT_STACK_EVENT_NWK_PARAMS_CHANGED:
        {
            Thermostat_postEvt(Thermostat_evtNwkParamUpdate);
            break;
        }
#endif
        default:
        {
            // do nothing
            break;
        }
    }
}

/**
 * @brief Processes the events.
 *
 * @return None
 */
static void processEvent(Thermostat_evt event)
{
    switch (event)
    {
        case Thermostat_evtTempChange:
        {
            /* perform activity related to the lock event. */
            DISPUTILS_SERIALPRINTF(0, 0, "Temperature Event received:");
            DISPUTILS_SERIALPRINTF(0, 0, thermostatTemp);

#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET " [" CUI_COLOR_RED "Set Point" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s", (char*)thermostatTemp, (char*)thermostatSetPt);
            tiopCUIUpdateApp((char*)statusBuf);
#endif /* TIOP_CUI */
            break;
        }

        case Thermostat_evtSetPtChange:
        {
            /* perform activity related to the un lock event */
            DISPUTILS_SERIALPRINTF(0, 0, "Setpoint Event received:");
            DISPUTILS_SERIALPRINTF(0, 0, thermostatSetPt);

            /* convert thermostat set point to an integer*/
            setPoint = atoi(thermostatSetPt);
#if TIOP_CUI
            snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET " [" CUI_COLOR_RED "Set Point" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s", (char*)thermostatTemp, (char*)thermostatSetPt);
            tiopCUIUpdateApp((char*)statusBuf);
#endif /* TIOP_CUI */
            break;
        }

        case Thermostat_evtNwkSetup:
        {
            if (false == serverSetup)
            {
                /* set callback functions */
                coapAttrs[0].pAttrHandlerCB = &coapHandleTemp;
                coapAttrs[1].pAttrHandlerCB = &coapHandleSetPt;
                /* register coap attributes */
                (void)setupCoapServer(OtInstance_get(), &coapAttrs[0]);
                (void)setupCoapServer(OtInstance_get(), &coapAttrs[1]);

                serverSetup = true;
            }
            break;
        }

        case Thermostat_evtKeyRight:
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

        case Thermostat_evtNwkJoined:
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

        case Thermostat_evtNwkJoinFailure:
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_join_fail);
#endif /* TIOP_CUI */
            DISPUTILS_SERIALPRINTF(1, 0, "Join Failure");
            DISPUTILS_LCDPRINTF(1, 0, "Join Failure");
            break;
        }

#if TIOP_CUI
        case Thermostat_evtProcessMenuUpdate:
        {
            CUI_processMenuUpdate();
            break;
        }
#endif /* TIOP_CUI */

        case Thermostat_evtNwkAttach:
        {
#if TIOP_CUI
            tiopCUIUpdateConnStatus(CUI_conn_joining);
#endif /* TIOP_CUI */
#ifdef USE_DMM
            DMMPolicy_updateStackState(DMMPolicy_StackRole_threadFtd, DMMPOLICY_THREAD_LINK_EST);
#endif /* USE_DMM */
            (void)OtStack_setupInterfaceAndNetwork();
            break;
        }

        case Thermostat_evtNwkJoin:
        {
            if ((!otDatasetIsCommissioned(OtInstance_get())) &&
                (OtStack_joinState() != OT_STACK_EVENT_NWK_JOIN_IN_PROGRESS))
            {
                DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
#if TIOP_CUI
                tiopCUIUpdateConnStatus(CUI_conn_joining);
#endif /* TIOP_CUI */
#ifdef USE_DMM
                DMMPolicy_updateStackState(DMMPolicy_StackRole_threadFtd, DMMPOLICY_THREAD_LINK_EST);
#endif /* USE_DMM */
                OtStack_joinConfiguredNetwork();
                break;
            }
        }

        case Thermostat_evtDevRoleChanged:
        {
            OtRtosApi_lock();
            role = otThreadGetDeviceRole(OtInstance_get());
            OtRtosApi_unlock();
#ifndef TIOP_POWER_MEASUREMENT
            /* This function handles LEDs as well as CUI updates. In future
             * it's role may be reduced and need to be folded into the
             * following `#if TIOP_CUI`.
             */
            tiopCUIUpdateRole(role);

            switch (role)
            {
                case OT_DEVICE_ROLE_DISABLED:
#ifdef TIOP_CUI
                    tiopCUIUpdateConnStatus(CUI_conn_disabled);
#endif /* TIOP_CUI */
#ifdef USE_DMM
                    DMMPolicy_updateStackState(DMMPolicy_StackRole_threadFtd, DMMPOLICY_THREAD_IDLE);
                    RemoteDisplay_updateProvProfData();
#endif /*USE_DMM */
#ifdef BLE_START
                    ProvisioningProfile_SetProvisionLock(false);
#endif /* BLE_START*/
                    break;

                case OT_DEVICE_ROLE_DETACHED:
#ifdef TIOP_CUI
                    tiopCUIUpdateConnStatus(CUI_conn_disabled);
#endif /* TIOP_CUI */
                    break;

                case OT_DEVICE_ROLE_CHILD:
                case OT_DEVICE_ROLE_ROUTER:
#ifdef USE_DMM
                    DMMPolicy_updateStackState(DMMPolicy_StackRole_threadFtd, DMMPOLICY_THREAD_DATA);
#endif /*USE_DMM */
#ifdef BLE_START
                    ProvisioningProfile_SetProvisionLock(true);
#endif
                    break;

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

#ifdef USE_DMM
                    DMMPolicy_updateStackState(DMMPolicy_StackRole_threadFtd, DMMPOLICY_THREAD_DATA);
#endif /*USE_DMM */
#ifdef BLE_START
                    ProvisioningProfile_SetProvisionLock(true);
#endif
                    break;
                }

                default:
                {
                    break;
                }
            }
#ifdef USE_DMM
        updateNetworkParams();
#endif /* USE_DMM */
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }

#ifdef USE_DMM
        case Thermostat_evtNwkParamUpdate:
        {
            updateNetworkParams();
            break;
        }
#endif /* USE_DMM */

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

/* Documented in thermostat.h */
void Thermostat_postEvt(Thermostat_evt event)
{
    struct Thermostat_procQueueMsg msg;
    int                            ret;
    msg.evt = event;
    ret = mq_send(Thermostat_procQueueDesc, (const char *)&msg, sizeof(msg), 0);
    assert(0 == ret);
}

/* Documented in graphicext.h */
void GraphicExt_requestProcess()
{
    Thermostat_postEvt(Thermostat_evtGraphicExt);
}

#if TIOP_CUI
/**
 * documented in tiop_ui.h
 */
void processMenuUpdateFn(void)
{
    Thermostat_postEvt(Thermostat_evtProcessMenuUpdate);
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
    Thermostat_postEvt(Thermostat_evtNwkAttach);
}

/**
 * documented in tiop_ui.h
 */
void uiActionJoin(const int32_t _itemEntry)
{
    Thermostat_postEvt(Thermostat_evtNwkJoin);
}

void thermostat_setPoint(const char _input, char* _pLines[3],
                                          CUI_cursorInfo_t* _pCurInfo)
{
    static CUI_cursorInfo_t cursor = {0, 4};
    /* chArr is static so the menu is common across button presses */
    static char chArr[3] = {0};

    switch (_input) {
        // Show the value of this screen w/o making changes
        case CUI_ITEM_PREVIEW:
        // Start editing the value
        case CUI_ITEM_INTERCEPT_START:
        {
            snprintf(chArr, sizeof(chArr), "%02d", setPoint);
            break;
        }
        // Submit the final modified value
        case CUI_ITEM_INTERCEPT_STOP:
        {
            strcpy((char *)thermostatSetPt, chArr);
            setPoint = atoi(chArr);
            // Reset the local cursor info
            cursor.col = 4;
            Thermostat_postEvt(Thermostat_evtSetPtChange);
            break;
        }
        // Move the cursor to the left
        case CUI_INPUT_LEFT:
        {
            if (4 != cursor.col)
            {
                cursor.col--;
            }
            break;
        }
        // Move the cursor to the right
        case CUI_INPUT_RIGHT:
        {
            if (5 != cursor.col)
            {
                cursor.col++;
            }
            break;
        }
        case CUI_INPUT_UP:
        {
            break;
        }

        case CUI_INPUT_DOWN:
        {
            break;
        }
        case CUI_INPUT_EXECUTE:
        {
            break;
        }
        default:
        {
            // is it a number
            if (CUI_IS_INPUT_NUM(_input))
            {
                if (cursor.col == 4)
                {
                    chArr[0] = _input;
                }
                else if (cursor.col == 5)
                {
                    chArr[1] = _input;
                }

                if (5 != cursor.col)
                {
                    cursor.col++;
                }
            }
        }
    }

    snprintf(_pLines[0], 16, "    %2s      ", chArr);

    if (_input != CUI_ITEM_PREVIEW) {
        strcpy(_pLines[2], "  SET POINT");
        _pCurInfo->row = 1;
        _pCurInfo->col = cursor.col+1;
    }
}
#endif /* TIOP_CUI */

#ifdef USE_DMM
/*!
 Get the high byte of a uint16_t variable

 */
uint8_t Util_hiUint16(uint16_t a)
{
    return((a >> 8) & 0xFF);
}

/*!
 Get the low byte of a uint16_t variable

 */
uint8_t Util_loUint16(uint16_t a)
{
    return((a) & 0xFF);
}

/*!
 Build a uint16_t out of 2 uint8_t variables

 */
uint16_t Util_buildUint16(uint8_t loByte, uint8_t hiByte)
{
    return((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)));
}


/*!
 Build a uint32_t out of 4 uint8_t variables

 */
uint32_t Util_buildUint32(uint8_t byte0, uint8_t byte1, uint8_t byte2,
                          uint8_t byte3)
{
    return((uint32_t)((uint32_t)((byte0) & 0x00FF) +
            ((uint32_t)((byte1) & 0x00FF) << 8) +
            ((uint32_t)((byte2) & 0x00FF) << 16) +
            ((uint32_t)((byte3) & 0x00FF) << 24)));
}

/*!
 Pull 1 uint8_t out of a uint32_t

 */
uint8_t Util_breakUint32(uint32_t var, int byteNum)
{
    return(uint8_t)((uint32_t)(((var) >> ((byteNum) * 8)) & 0x00FF));
}

static void provisionConnectCb(uint8_t connectType)
{
    if(connectType == PROVPROFILE_JOIN){
        uiActionJoin(0);
    }
    else if (connectType == PROVPROFILE_ATTACH){
        uiActionAttach(0);
    }
}

static void provisionDisconnectCb(void)
{
//    uiActionDetach(0);
}

static void networkResetCb(uint8_t resetType){
    if(resetType == PROVPROFILE_RESETDEVICE)
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
    }
    else if (resetType == PROVPROFILE_RESETNETWORK)
    {
        OtRtosApi_lock();
        otInstanceReset(OtInstance_get());
        OtRtosApi_unlock();
    }
}

/** @brief  Set provisioning callback functions
 *
 *  @param  ProvisionAttr_t  Remote display attribute value to set
 *  @param  value  pointer to data from remote dispaly application
 *  @param  len  length of data from remote display application
 */
static void setProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr,
                              void *const value, uint8_t len)
{
    uint8_t *byteArr = (uint8_t *)value;

    switch(provisioningAttr)
    {
    case ProvisionAttr_PanId:
    {
        OtRtosApi_lock();
        otLinkSetPanId(OtInstance_get(), Util_buildUint16(byteArr[1], byteArr[0]));
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_SensorChannelMask:
    {

        OtRtosApi_lock();

        otLinkSetSupportedChannelMask(OtInstance_get(), Util_buildUint32(byteArr[3], byteArr[2],
                                                                         byteArr[1], byteArr[0]));
        OtRtosApi_unlock();

        break;
    }
    case ProvisionAttr_ExtPanId:
    {
        extPanId.m8[0] = ((uint8_t *)value)[0];
        extPanId.m8[1] = ((uint8_t *)value)[1];
        extPanId.m8[2] = ((uint8_t *)value)[2];
        extPanId.m8[3] = ((uint8_t *)value)[3];
        extPanId.m8[4] = ((uint8_t *)value)[4];
        extPanId.m8[5] = ((uint8_t *)value)[5];
        extPanId.m8[6] = ((uint8_t *)value)[6];
        extPanId.m8[7] = ((uint8_t *)value)[7];
        OtRtosApi_lock();
        otThreadSetExtendedPanId(OtInstance_get(),&extPanId);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_Freq://channel
    {
        OtRtosApi_lock();
        otLinkSetChannel(OtInstance_get(),*(uint8_t *)value);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_NtwkName:
    {
        for(int i = 0; i< PROVPROFILE_NTWK_NAME_CHAR_LEN; i++)
            networkName.m8[i] = ((uint8_t *)value)[i];
        OtRtosApi_lock();
        otThreadSetNetworkName(OtInstance_get(),networkName.m8);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_NtwkKey:
    {
        masterKey.m8[0] = ((uint8_t *)value)[0];
        masterKey.m8[1] = ((uint8_t *)value)[1];
        masterKey.m8[2] = ((uint8_t *)value)[2];
        masterKey.m8[3] = ((uint8_t *)value)[3];
        masterKey.m8[4] = ((uint8_t *)value)[4];
        masterKey.m8[5] = ((uint8_t *)value)[5];
        masterKey.m8[6] = ((uint8_t *)value)[6];
        masterKey.m8[7] = ((uint8_t *)value)[7];
        OtRtosApi_lock();
        otThreadSetMasterKey(OtInstance_get(),&masterKey);
        OtRtosApi_unlock();
        break;
    }
    case ProvisionAttr_FFDAddr:
        //Not writeable
    case ProvisionAttr_ShortAddr:
        //not writable
    default:
        // Attribute not found
        break;
    }
}

/** @brief  Get provisioning callback functions
 *
 *  @param  ProvisionAttr_t  Remote display attribute value to set
 *
 *  @return  uint8_t  Current value of data present in 15.4 application
 */
static void getProvisioningCb(RemoteDisplay_ProvisionAttr_t provisioningAttr, void *value, uint8_t len)
{
    switch(provisioningAttr)
    {
        // The PAN ID and Channel mask are reversed in byte order below
        // to allow the Light Blue BLE phone application to parse this data properly.
        case ProvisionAttr_PanId:
        {
            ((uint8_t *)value)[0] = Util_hiUint16(panId);
            ((uint8_t *)value)[1] = Util_loUint16(panId);
            break;
        }
        case ProvisionAttr_SensorChannelMask:
        {
            ((uint8_t *)value)[0] = Util_breakUint32(chanMask, 3);
            ((uint8_t *)value)[1] = Util_breakUint32(chanMask, 2);
            ((uint8_t *)value)[2] = Util_breakUint32(chanMask, 1);
            ((uint8_t *)value)[3] = Util_breakUint32(chanMask, 0);
            break;
        }
        case ProvisionAttr_Freq://channel
        {
            *(uint8_t *)value = channel;
            break;
        }
        case ProvisionAttr_ExtPanId:
        {
            ((uint8_t *)value)[0] = extPanId.m8[0];
            ((uint8_t *)value)[1] = extPanId.m8[1];
            ((uint8_t *)value)[2] = extPanId.m8[2];
            ((uint8_t *)value)[3] = extPanId.m8[3];
            ((uint8_t *)value)[4] = extPanId.m8[4];
            ((uint8_t *)value)[5] = extPanId.m8[5];
            ((uint8_t *)value)[6] = extPanId.m8[6];
            ((uint8_t *)value)[7] = extPanId.m8[7];
            break;
        }
        case ProvisionAttr_FFDAddr://EUI64
        {
            ((uint8_t *)value)[0] = extAddress.m8[0];
            ((uint8_t *)value)[1] = extAddress.m8[1];
            ((uint8_t *)value)[2] = extAddress.m8[2];
            ((uint8_t *)value)[3] = extAddress.m8[3];
            ((uint8_t *)value)[4] = extAddress.m8[4];
            ((uint8_t *)value)[5] = extAddress.m8[5];
            ((uint8_t *)value)[6] = extAddress.m8[6];
            ((uint8_t *)value)[7] = extAddress.m8[7];
            break;
        }
        case ProvisionAttr_NtwkKey:
        {
            ((uint8_t *)value)[0] = masterKey.m8[0];
            ((uint8_t *)value)[1] = masterKey.m8[1];
            ((uint8_t *)value)[2] = masterKey.m8[2];
            ((uint8_t *)value)[3] = masterKey.m8[3];
            ((uint8_t *)value)[4] = masterKey.m8[4];
            ((uint8_t *)value)[5] = masterKey.m8[5];
            ((uint8_t *)value)[6] = masterKey.m8[6];
            ((uint8_t *)value)[7] = masterKey.m8[7];
            ((uint8_t *)value)[8] = masterKey.m8[8];
            ((uint8_t *)value)[9] = masterKey.m8[9];
            ((uint8_t *)value)[10] = masterKey.m8[10];
            ((uint8_t *)value)[11] = masterKey.m8[11];
            ((uint8_t *)value)[12] = masterKey.m8[12];
            ((uint8_t *)value)[13] = masterKey.m8[13];
            ((uint8_t *)value)[14] = masterKey.m8[14];
            ((uint8_t *)value)[15] = masterKey.m8[15];
            break;
        }
        case ProvisionAttr_NtwkName:
        {
            for(int i = 0; i< PROVISIONING_NTWK_NAME_LEN; i++)
                ((uint8_t *)value)[i] = networkName.m8[i];

            break;
        }
        case ProvisionAttr_PSKd:
        {
            for(int i = 0; i < strlen(TIOP_CONFIG_PSKD); i++)
                ((uint8_t *)value)[i] = TIOP_CONFIG_PSKD[i];
            break;
        }
        case ProvisionAttr_ShortAddr:
        {
            ((uint8_t *)value)[0] = Util_hiUint16(shortAddr);
            ((uint8_t *)value)[1] = Util_loUint16(shortAddr);
            break;
        }
        case ProvisionAttr_ProvState:
        {
            *(uint8_t *)value = role;
            break;
        }
        default:
            // Attribute not found
            break;
    }
}



/** @brief  Upon reading the message to update parameters, will update as appropriate
 *
 *  @param  void
 *
 *  @return  void
 */
void updateNetworkParams(void)
{
    OtRtosApi_lock();
    if(deltaMask & OT_CHANGED_THREAD_CHANNEL){
        channel = otLinkGetChannel(OtInstance_get());
        tiopCUIUpdateChannel(channel);
    }
    if(deltaMask & OT_CHANGED_THREAD_PANID ){
        panId = otLinkGetPanId(OtInstance_get());
        tiopCUIUpdatePANID(panId);
    }
    if(deltaMask & OT_CHANGED_THREAD_EXT_PANID ){
        extPanId = *(otThreadGetExtendedPanId(OtInstance_get()));
        tiopCUIUpdateExtPANID(extPanId);
    }
    if(deltaMask & OT_CHANGED_THREAD_NETWORK_NAME ){
        strncpy(networkName.m8,otThreadGetNetworkName(OtInstance_get()),sizeof(networkName.m8));
        tiopCUIUpdateNwkName(networkName.m8);
    }
    if(deltaMask & OT_CHANGED_MASTER_KEY ){
        masterKey = *(otThreadGetMasterKey(OtInstance_get()));
        tiopCUIUpdateMasterkey(masterKey);
    }
    if(deltaMask & OT_CHANGED_SUPPORTED_CHANNEL_MASK ){
        chanMask = otLinkGetSupportedChannelMask(OtInstance_get());
        tiopCUIUpdateChannelMask(chanMask);
    }
    if(deltaMask & (OT_CHANGED_THREAD_RLOC_ADDED | OT_CHANGED_THREAD_RLOC_REMOVED | OT_CHANGED_THREAD_LL_ADDR)){
        shortAddr = otLinkGetShortAddress(OtInstance_get());
        tiopCUIUpdateShortAddr(shortAddr);
    }

    OtRtosApi_unlock();
#ifdef BLE_START
    RemoteDisplay_updateProvProfData();
#endif
}
#endif /* USE_DMM */

/**
 * Documented in task_config.h.
 */
void Thermostat_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

    retc = pthread_attr_init(&pAttrs);
    assert(retc == 0);

    retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
    assert(retc == 0);

    priParam.sched_priority = TASK_CONFIG_THERMOSTAT_TASK_PRIORITY;
    retc = pthread_attr_setschedparam(&pAttrs, &priParam);
    assert(retc == 0);

    retc |= pthread_attr_setstacksize(&pAttrs, TASK_CONFIG_THERMOSTAT_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, Thermostat_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

#if defined(USE_DMM) && defined(BLE_START)
    RemoteDisplay_registerClientProvCbs(ot_ProvisioningCbs);
#endif /* defined(USE_DMM) && defined(BLE_START) */

    (void)retc;
}

/**
 *  Thermostat processing thread.
 */
void *Thermostat_task(void *arg0)
{
    struct mq_attr attr;
#if !TIOP_CONFIG_SET_NW_ID
    bool           commissioned;
#endif /* !TIOP_CONFIG_SET_NW_ID */
    mqd_t          procQueueLoopDesc;

    attr.mq_curmsgs = 0;
    attr.mq_flags   = 0;
    attr.mq_maxmsg  = THERMOSTAT_PROC_QUEUE_MAX_MSG;
    attr.mq_msgsize = sizeof(struct Thermostat_procQueueMsg);

    /* Open the processing queue in non-blocking mode for the notify
     * callback functions
     */
    Thermostat_procQueueDesc = mq_open(Thermostat_procQueueName,
                                       (O_WRONLY | O_NONBLOCK | O_CREAT),
                                       0, &attr);

    /* Open the processing queue in blocking read mode for the process loop */
    procQueueLoopDesc = mq_open(Thermostat_procQueueName, O_RDONLY, 0, NULL);

    OtStack_taskCreate();

    OtStack_registerCallback(processOtStackEvents);

    snprintf(statusBuf, sizeof(statusBuf), "[" CUI_COLOR_RED "Temperature" CUI_COLOR_RESET "] " CUI_COLOR_WHITE"%s" CUI_COLOR_RESET " [" CUI_COLOR_CYAN "Set Point" CUI_COLOR_RESET "] " CUI_COLOR_WHITE "%s", (char*)thermostatTemp, (char*)thermostatSetPt);
    tiopCUIInit((char*)statusBuf, &rightButtonHandle);

    DISPUTILS_SERIALPRINTF(0, 0, "Thermostat init!");

    /* Initialize the extended graphics library.
     *
     * Grabbing the lcd handle from disp_utils for this.
     */
#if BOARD_DISPLAY_USE_LCD
    GraphicExt_initAnimationModule(&lcdHandle, FRAME_PERIOD);
#endif

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

#ifdef USE_DMM
    //Initialization using default parameters
    OtRtosApi_lock();
    otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
    panId = otLinkGetPanId(OtInstance_get());
    channel = otLinkGetChannel(OtInstance_get());
    shortAddr = otLinkGetShortAddress(OtInstance_get());
    chanMask = otLinkGetSupportedChannelMask((OtInstance_get()));
    OtRtosApi_unlock();

    for(int i=0; i< sizeof(networkName.m8); i++){
        if(!TIOP_CONFIG_NETWORK_NAME[i])
            break;
        networkName.m8[i] = TIOP_CONFIG_NETWORK_NAME[i];
    }

    /* Set master key using a byte-swap assignment */
    for (int i = 0; i < OT_MASTER_KEY_SIZE/2; i++)
    {
        masterKey.m8[OT_MASTER_KEY_SIZE - i - 1] =
                ((TIOP_CONFIG_MASTER_KEY_L >> (8*i)) & 0xFF);
    }
    for (int i = OT_MASTER_KEY_SIZE/2; i < OT_MASTER_KEY_SIZE; i++)
    {
        masterKey.m8[OT_MASTER_KEY_SIZE - i - 1] =
                ((TIOP_CONFIG_MASTER_KEY_U >> (8*(i-(OT_MASTER_KEY_SIZE/2)))) & 0xFF);
    }
    /* Set extended PAN ID using a byte-swap assignment */
    for (int i = 0; i < OT_EXT_PAN_ID_SIZE; i++)
    {
        extPanId.m8[OT_EXT_PAN_ID_SIZE - i - 1] =
                ((TIOP_CONFIG_EXT_PAN_ID >> (8*i)) & 0xFF);
    }
#ifdef BLE_START
    RemoteDisplay_updateProvProfData();
#endif /* BLE_START */
    OtStack_setupStateChange();
#endif /* USE_DMM */

    /* process events */
    while (1)
    {
        struct Thermostat_procQueueMsg msg;
        ssize_t                        ret;

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
