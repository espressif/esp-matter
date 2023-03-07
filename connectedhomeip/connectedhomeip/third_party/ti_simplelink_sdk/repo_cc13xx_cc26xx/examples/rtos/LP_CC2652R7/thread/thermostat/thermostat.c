/******************************************************************************

 @file thermostat.c

 @brief Thermostat example application

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
#include <ti/display/Display.h>
#include <ti/display/DisplayExt.h>
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

#include "graphicext.h"
#include "images.h"
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

/* OpenThread Stack thread call stack */
static char stack[TASK_CONFIG_THERMOSTAT_TASK_STACK_SIZE];

/* coap resources for the application */
static otCoapResource coapResourceTemp;
static otCoapResource coapResourceSetPt;

/* default coap attribute values of the application */
static char thermostatTemp[TEMP_MAX_CHARS] = "68";
static int temperature = 68;
static char thermostatSetPt[TEMP_MAX_CHARS] = "34";
static int setPoint = 34;
static Thermostat_mode thermostatMode = Thermostat_modeCooling;

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

/******************************************************************************
 Function Prototype
 *****************************************************************************/

/*  Thermostat processing thread. */
void *Thermostat_task(void *arg0);

/******************************************************************************
 Local Functions
 *****************************************************************************/
/**
 * @brief Calculates the offset for the font sprite buffers based on a char.
 *
 * @param character character to map to font, any char from '0' to '9'.
 * @param charOffset offset for the font buffer.
 *
 * @return None
 */
static uint32_t getCharOffset(uint8_t character, int charOffset)
{
    return (charOffset * (character - zeroCharOffset));
}

/**
 * @brief update the temperature font buffers on the temperature digit sprites.
 *
 * @param temperatureStr current temperature in string form.
 *
 * @return None
 */
static void updateTemperatureLcd(char *temperatureStr)
{
    uint8_t tempLength = strlen(temperatureStr);
    char leftDigitChar = temperatureStr[0];
    char rightDigitChar = temperatureStr[1];
    uint32_t fontPixelOffset;

    if(tempLength == 1)
    {
        leftDigitChar = '0';
        rightDigitChar = temperatureStr[0];
    }

    fontPixelOffset = getCharOffset(leftDigitChar, TEMP_FONT_OFFSET);
    thermostatSpriteList[0].image->pPixel = (uint8_t*)(thermostatTempFontPix +
                                                        fontPixelOffset);
    fontPixelOffset = getCharOffset(rightDigitChar, TEMP_FONT_OFFSET);
    thermostatSpriteList[1].image->pPixel = (uint8_t*)(thermostatTempFontPix +
                                                        fontPixelOffset);

}

/**
 * @brief update the setpoint font buffers on the setpoint digit sprites.
 *
 * @param temperatureStr current setpoint in string form.
 *
 * @return None
 */
static void updateSetPtLcd(char *temperatureStr)
{
    uint8_t tempLength = strlen(temperatureStr);
    char leftDigitChar = temperatureStr[0];
    char rightDigitChar = temperatureStr[1];
    uint32_t fontPixelOffset;
    if(tempLength == 1)
    {
        leftDigitChar = '0';
        rightDigitChar = temperatureStr[0];
    }
    fontPixelOffset = getCharOffset(leftDigitChar, SETPT_FONT_OFFSET);
    thermostatSpriteList[2].image->pPixel = (uint8_t*)(thermostatSetPtFontPix +
                                                   fontPixelOffset);

    fontPixelOffset = getCharOffset(rightDigitChar, SETPT_FONT_OFFSET);
    thermostatSpriteList[3].image->pPixel = (uint8_t*)(thermostatSetPtFontPix +
                                                   fontPixelOffset);
}

/**
 * @brief update the mode of the thermostat by comparing the setpoint and temp.
 *
 * @return None
 */
static void updateThermostatMode(void)
{
    if(thermostatMode == Thermostat_modeHeating && temperature > setPoint)
    {
        thermostatMode = Thermostat_modeCooling;
        thermostatSpriteList[5].image->pPixel = (uint8_t*)(thermostatModePix);
        /* Update the background animation */
        GraphicExt_animateBackground((Graphics_Image*)thermostatBackgroundImage, 0,
                        COOL_ANIMATION_END, true);
    }
    else if(thermostatMode == Thermostat_modeCooling && temperature < setPoint)
    {
        thermostatMode = Thermostat_modeHeating;
        thermostatSpriteList[5].image->pPixel = (uint8_t*)(thermostatModePix +
                                                           MODE_ICON_OFFSET);
       GraphicExt_animateBackground((Graphics_Image*)thermostatBackgroundImage,
                                    COOL_ANIMATION_END,
                                    HEAT_ANIMATION_END, true);
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
static void coapHandleSetPt(void *aContext, otMessage *aMessage,
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
            responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
            otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

            otCoapMessageInitResponse(responseMessage, aMessage,
                                      OT_COAP_TYPE_ACKNOWLEDGMENT,
                                      OT_COAP_CODE_CHANGED);
            otCoapMessageSetToken(responseMessage,
                                  otCoapMessageGetToken(aMessage),
                                  otCoapMessageGetTokenLength(aMessage));
            otCoapMessageSetPayloadMarker(responseMessage);

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

        error = otMessageAppend(responseMessage, thermostatTemp,strlen((const char*)thermostatTemp));
        otEXPECT(OT_ERROR_NONE == error);

        error = otCoapSendResponse((otInstance*)aContext, responseMessage,
                                   aMessageInfo);
        otEXPECT(OT_ERROR_NONE == error);
    }
    else if (OT_COAP_CODE_POST == messageCode)
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
            responseMessage = otCoapNewMessage((otInstance*)aContext, NULL);
            otEXPECT_ACTION(responseMessage != NULL, error = OT_ERROR_NO_BUFS);

            otCoapMessageInitResponse(responseMessage, aMessage,
                                      OT_COAP_TYPE_ACKNOWLEDGMENT,
                                      OT_COAP_CODE_CHANGED);
            otCoapMessageSetToken(responseMessage,
                                  otCoapMessageGetToken(aMessage),
                                  otCoapMessageGetTokenLength(aMessage));
            otCoapMessageSetPayloadMarker(responseMessage);

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
        otCoapAddResource(aInstance, attr->pAttrCoapResource);
        OtRtosApi_unlock();
    }

exit:
    return error;
}

/**
 * @brief Display's the image on the LCD screen.
 *
 * @return None
 */
static void flushLcd(void)
{
    GraphicExt_drawSprites(thermostatSpriteList, SPRITE_COUNT, true);
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
    (void) aContext;

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

            /* convert thermostat temperature to an integer*/
            temperature = atoi(thermostatTemp);
            /* update the Lcd screen with new temperature value and mode */
            updateTemperatureLcd(thermostatTemp);
            updateThermostatMode();
            flushLcd();
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
            /* update the Lcd screen with new setpoint value and mode */
            updateSetPtLcd(thermostatSetPt);
            updateThermostatMode();
            flushLcd();
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

                /* Initialize the LCD screen */
                updateTemperatureLcd(thermostatTemp);
                updateSetPtLcd(thermostatSetPt);

                /* Flush the sprites to the LCD */
                flushLcd();
                GraphicExt_animateBackground((Graphics_Image*)thermostatBackgroundImage,
                                                    0,
                                                    COOL_ANIMATION_END, true);
                updateThermostatMode();
            }
            break;
        }

        case Thermostat_evtGraphicExt:
        {
            /* process the GraphicExt event */
            GraphicExt_processEvt();
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

        case Thermostat_evtNwkAttach:
        {
            DISPUTILS_LCDPRINTF(1, 0, "Joining Nwk ...");
            tiopCUIUpdateConnStatus(CUI_conn_joining);
            (void)OtStack_setupInterfaceAndNetwork();
            break;
        }

        case Thermostat_evtNwkJoin:
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

        case Thermostat_evtDevRoleChanged:
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

    retc = pthread_attr_setstack(&pAttrs, (void *)stack,
                                 TASK_CONFIG_THERMOSTAT_TASK_STACK_SIZE);
    assert(retc == 0);

    retc = pthread_create(&thread, &pAttrs, Thermostat_task, NULL);
    assert(retc == 0);

    retc = pthread_attr_destroy(&pAttrs);
    assert(retc == 0);

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

    /* Initialize display and open LCD and serial types of display. */
    Display_Params params;
    Display_Params_init(&params);
    params.lineClearMode = DISPLAY_CLEAR_BOTH;

    DispUtils_open();

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
