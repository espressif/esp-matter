/******************************************************************************

 @file tiop_ui.c

 @brief TI-OpenThread CUI Functions

 Group: CMCU, LPC
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2021, Texas Instruments Incorporated
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

#include "tiop_ui.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tiop_config.h"
#include "otsupport/otinstance.h"
#include "otsupport/otrtosapi.h"
#include <openthread/thread.h>
#include "ti_drivers_config.h"
#include "tiop_app_defs.h"

#if !TIOP_POWER_MEASUREMENT
static LED_Handle redLedHandle;
static LED_Handle greenLedHandle;
#endif /* !TIOP_POWER_MEASUREMENT */

static CUI_clientHandle_t clientHandle;

static uint32_t deviceInfoLine;
static uint32_t nwkInfoLine1;
static uint32_t nwkInfoLine2;
static uint32_t nwkInfoLine3;
#ifdef USE_DMM
static uint32_t nwkInfoLine4;
#endif
static uint32_t deviceRoleLine;
static uint32_t connStatusLine;
static uint32_t appInfoLine;

static otPanId          panid;
static uint8_t          display_channel;
static otShortAddress   shortAddr;
static otExtAddress     extAddress;
static otExtendedPanId  extPanId;
static otMasterKey      masterkey;
#ifdef USE_DMM
static otChannelMask channelMask;
#endif
static const char *     nwkName;

CUI_SUB_MENU(appMenuConfig, "<     CONFIG     >", 2, appMainMenu)
    CUI_MENU_ITEM_INT_ACTION("<     PAN ID     >", (CUI_pFnIntercept_t) uiActionProcessConfigurePanId)
    CUI_MENU_ITEM_INT_ACTION("<     CHANNEL    >", (CUI_pFnIntercept_t) uiActionProcessConfigureChannel)
CUI_SUB_MENU_END

CUI_SUB_MENU(appMenuNwk, "<      NWK       >", 2, appMainMenu)
    CUI_MENU_ITEM_ACTION("<      JOIN      >", uiActionJoin)
    CUI_MENU_ITEM_ACTION("<     ATTACH     >", uiActionAttach)
CUI_SUB_MENU_END

CUI_SUB_MENU(appMenuApp, "<    APP MENU    >", SUB_APP_MENUS, appMainMenu)
    CUI_APP_MENU
CUI_SUB_MENU_END

CUI_MAIN_MENU(appMainMenu, APP_TITLE_STR, SAMPLE_APP_MENUS, (CUI_pFnClientMenuUpdate_t) processMenuUpdateFn)
    CUI_MENU_ITEM_SUBMENU(appMenuConfig)
    CUI_MENU_ITEM_SUBMENU(appMenuNwk)
    CUI_MENU_ITEM_SUBMENU(appMenuApp)
    CUI_MENU_ITEM_INT_ACTION("<     RESET      >", (CUI_pFnIntercept_t) tiopCUIReset)
    CUI_MENU_ITEM_INT_ACTION("<   NWK RESET    >", (CUI_pFnIntercept_t) tiopCUINwkReset)
CUI_MAIN_MENU_END

/**
 * documented in tiop_ui.h
 */
void tiopCUIInit(char* appString, Button_Handle* rightButtonHandle)
{
    CUI_params_t cuiParams;
    Button_Params bparams;
    Button_Handle btnHandle;

#if BOARD_DISPLAY_USE_UART
    LED_Params ledParams;
#endif /* BOARD_DISPLAY_USE_UART */

    CUI_paramsInit(&cuiParams);
#if !TIOP_CUI
    cuiParams.manageUart = false;
#endif /* TIOP_CUI */
    CUI_init(&cuiParams);
    CUI_clientParams_t clientParams;
    CUI_clientParamsInit(&clientParams);
#ifdef USE_DMM
    clientParams.maxStatusLines = 8;
#else
    clientParams.maxStatusLines = 7;
#endif
    strncpy(clientParams.clientName, "Thread CUI application", MAX_CLIENT_NAME_LEN);

    clientHandle = CUI_clientOpen(&clientParams);
    if (clientHandle == 0U)
    {
        // handle failure
    }

    Button_Params_init(&bparams);
    bparams.buttonEventMask = Button_EV_CLICKED;
    // Open Right button without appCallBack
    btnHandle = Button_open(CONFIG_BTN_RIGHT, &bparams);

    // Read button state
    if (!GPIO_read(((Button_HWAttrs*)btnHandle->hwAttrs)->gpioIndex))
    {
        OtRtosApi_lock();
        otInstanceFactoryReset(OtInstance_get());
        OtRtosApi_unlock();
    }

    // Set button callback
    Button_setCallback(btnHandle, processKeyChangeCB);

    // Pass the button handle back to the caller
    *rightButtonHandle = btnHandle;

#if TIOP_OAD
    btnHandle = Button_open(CONFIG_BTN_LEFT, &bparams);
    // Read button state
    if (!GPIO_read(((Button_HWAttrs*)btnHandle->hwAttrs)->gpioIndex))
    {
        TIOP_OAD_invalidate_image_header();
    }
    Button_close(btnHandle);
#endif /* TIOP_OAD */

#if BOARD_DISPLAY_USE_UART
    LED_Params_init(&ledParams);
#endif /* BOARD_DISPLAY_USE_UART */

#if !TIOP_POWER_MEASUREMENT
    greenLedHandle = LED_open(CONFIG_LED_GREEN, &ledParams);
    redLedHandle = LED_open(CONFIG_LED_RED, &ledParams);
#endif /* !TIOP_POWER_MEASUREMENT */

#if TIOP_CUI
    CUI_statusLineResourceRequest(clientHandle, "Device Info", false, &deviceInfoLine);
    CUI_statusLineResourceRequest(clientHandle, "   Nwk Info", false, &nwkInfoLine1);
    CUI_statusLineResourceRequest(clientHandle, "   Nwk Info", false, &nwkInfoLine2);
    CUI_statusLineResourceRequest(clientHandle, "   Nwk Info", false, &nwkInfoLine3);
#ifdef USE_DMM
    CUI_statusLineResourceRequest(clientHandle, "   Nwk Info", false, &nwkInfoLine4);
#endif
    CUI_statusLineResourceRequest(clientHandle, "  Role Info", false, &deviceRoleLine);
    CUI_statusLineResourceRequest(clientHandle, "  Conn Info", false, &connStatusLine);
    CUI_statusLineResourceRequest(clientHandle, "   APP Info", false, &appInfoLine);

    OtRtosApi_lock();
    otLinkGetFactoryAssignedIeeeEui64(OtInstance_get(), &extAddress);
    panid = otLinkGetPanId(OtInstance_get());
    display_channel = otLinkGetChannel(OtInstance_get());
    shortAddr = otLinkGetShortAddress(OtInstance_get());
#ifdef USE_DMM
    channelMask = otLinkGetSupportedChannelMask(OtInstance_get());
#endif
    OtRtosApi_unlock();

    nwkName = TIOP_CONFIG_NETWORK_NAME;
    /* Set master key using a byte-swap assignment */
    for (int i = 0; i < OT_MASTER_KEY_SIZE/2; i++)
    {
        masterkey.m8[OT_MASTER_KEY_SIZE - i - 1] =
                ((TIOP_CONFIG_MASTER_KEY_L >> (8*i)) & 0xFF);
    }
    for (int i = OT_MASTER_KEY_SIZE/2; i < OT_MASTER_KEY_SIZE; i++)
    {
        masterkey.m8[OT_MASTER_KEY_SIZE - i - 1] =
                ((TIOP_CONFIG_MASTER_KEY_U >> (8*(i-(OT_MASTER_KEY_SIZE/2)))) & 0xFF);
    }
    /* Set extended PAN ID using a byte-swap assignment */
    for (int i = 0; i < OT_EXT_PAN_ID_SIZE; i++)
    {
        extPanId.m8[OT_EXT_PAN_ID_SIZE - i - 1] =
                ((TIOP_CONFIG_EXT_PAN_ID >> (8*i)) & 0xFF);
    }

    //Update Device Info Line
    tiopCUIUpdateDeviceInfo();
    //Update Nwk Info Line1
    tiopCUIUpdateNwkName(nwkName);
    //Update Nwk Info Line 2
    tiopCUIUpdateMasterkey(masterkey);
    //Update Nwk Info Line 3
    tiopCUIUpdatePANID(panid);
#ifdef USE_DMM
    //Update Nwk Info Line 4
    tiopCUIUpdateChannelMask(channelMask);
#endif
    //Update Conn Info Line
    tiopCUIUpdateConnStatus(CUI_conn_disabled);
    //Update App Info Line
    tiopCUIUpdateApp(appString);

    CUI_registerMenu(clientHandle, &appMainMenu);
#endif /* TIOP_CUI */

    //Update Role Info Line
    tiopCUIUpdateRole(OT_DEVICE_ROLE_DISABLED);
}


/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateDeviceInfo(void)
{

    CUI_statusLinePrintf(
        clientHandle, deviceInfoLine,
        "[" CUI_COLOR_RED "EUI64" CUI_COLOR_RESET "] 0x%02x%02x%02x%02x%02x%02x%02x%02x  [" CUI_COLOR_MAGENTA "PSKD" CUI_COLOR_RESET"] %s",
        extAddress.m8[0], extAddress.m8[1], extAddress.m8[2],
        extAddress.m8[3], extAddress.m8[4], extAddress.m8[5],
        extAddress.m8[6], extAddress.m8[7], TIOP_CONFIG_PSKD);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateNwkName(const char* name)
{
    nwkName = name;
    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine1,
        "[" CUI_COLOR_MAGENTA "Network Name" CUI_COLOR_RESET "] %s  [" CUI_COLOR_MAGENTA "ExtPanID" CUI_COLOR_RESET "] 0x%02x%02x%02x%02x%02x%02x%02x%02x",
        nwkName,
        extPanId.m8[0], extPanId.m8[1], extPanId.m8[2],
        extPanId.m8[3], extPanId.m8[4], extPanId.m8[5],
        extPanId.m8[6], extPanId.m8[7]);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateExtPANID(otExtendedPanId extPan)
{
    extPanId = extPan;
    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine1,
        "[" CUI_COLOR_MAGENTA "Network Name" CUI_COLOR_RESET "] %s  [" CUI_COLOR_MAGENTA "ExtPanID" CUI_COLOR_RESET "] 0x%02x%02x%02x%02x%02x%02x%02x%02x",
        nwkName,
        extPanId.m8[0], extPanId.m8[1], extPanId.m8[2],
        extPanId.m8[3], extPanId.m8[4], extPanId.m8[5],
        extPanId.m8[6], extPanId.m8[7]);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateMasterkey(otMasterKey key)
{
    masterkey = key;
    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine2,
        "[" CUI_COLOR_MAGENTA "Masterkey" CUI_COLOR_RESET"] 0x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
        masterkey.m8[0], masterkey.m8[1], masterkey.m8[2],
        masterkey.m8[3], masterkey.m8[4], masterkey.m8[5],
        masterkey.m8[6], masterkey.m8[7], masterkey.m8[8],
        masterkey.m8[9], masterkey.m8[10], masterkey.m8[11],
        masterkey.m8[12], masterkey.m8[13], masterkey.m8[14], masterkey.m8[15]);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdatePANID(otPanId pan)
{
    panid = pan;
    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine3,
        "[" CUI_COLOR_YELLOW "PAN ID" CUI_COLOR_RESET "] 0x%04x  [" CUI_COLOR_YELLOW "Channel" CUI_COLOR_RESET "] %d  [" CUI_COLOR_YELLOW "Short Addr" CUI_COLOR_RESET "] 0x%x",
        (uint16_t) pan, display_channel, shortAddr);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateChannel(uint8_t ch)
{
    /* channel value is within range for IEEE 802.15.4-2006 page 0 */
    display_channel = ch;

    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine3,
        "[" CUI_COLOR_YELLOW "PAN ID" CUI_COLOR_RESET "] 0x%04x  [" CUI_COLOR_YELLOW "Channel" CUI_COLOR_RESET "] %d  [" CUI_COLOR_YELLOW "Short Addr" CUI_COLOR_RESET "] 0x%x",
        (uint16_t) panid, ch, shortAddr);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateShortAddr(otShortAddress s)
{
    shortAddr = s;
    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine3,
        "[" CUI_COLOR_YELLOW "PAN ID" CUI_COLOR_RESET "] 0x%04x  [" CUI_COLOR_YELLOW "Channel" CUI_COLOR_RESET "] %d  [" CUI_COLOR_YELLOW "Short Addr" CUI_COLOR_RESET "] 0x%x",
        (uint16_t) panid, display_channel, s);
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateRole(otDeviceRole role)
{
    switch (role)
    {
        case OT_DEVICE_ROLE_DISABLED:
        {
#if TIOP_CUI
            CUI_statusLinePrintf(
                clientHandle, deviceRoleLine,
                "["CUI_COLOR_RED "Device Role" CUI_COLOR_RESET "] Disabled");
#endif /* TIOP_CUI */

#if !TIOP_POWER_MEASUREMENT
            LED_setOff(greenLedHandle);
            LED_setOff(redLedHandle);
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }
        case OT_DEVICE_ROLE_DETACHED:
        {
#if TIOP_CUI
            CUI_statusLinePrintf(
                clientHandle, deviceRoleLine,
                "["CUI_COLOR_RED "Device Role" CUI_COLOR_RESET "] Detached");
#endif /* TIOP_CUI */

#if !TIOP_POWER_MEASUREMENT
            LED_setOff(greenLedHandle);
            LED_setOn(redLedHandle, LED_BRIGHTNESS_MAX);
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }
        case OT_DEVICE_ROLE_CHILD:
        {
#if TIOP_CUI
            CUI_statusLinePrintf(
                clientHandle, deviceRoleLine,
                "["CUI_COLOR_RED "Device Role" CUI_COLOR_RESET "] Child");
#endif /* TIOP_CUI */

#if !TIOP_POWER_MEASUREMENT
            LED_setOn(greenLedHandle, LED_BRIGHTNESS_MAX);
            LED_setOff(redLedHandle);
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }
        case OT_DEVICE_ROLE_ROUTER:
        {
#if TIOP_CUI
            CUI_statusLinePrintf(
                clientHandle, deviceRoleLine,
                "["CUI_COLOR_RED "Device Role" CUI_COLOR_RESET "] Router");
#endif /* TIOP_CUI */

#if !TIOP_POWER_MEASUREMENT
            LED_setOn(greenLedHandle, LED_BRIGHTNESS_MAX);
            LED_setOff(redLedHandle);
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }
        case OT_DEVICE_ROLE_LEADER:
        {
#if TIOP_CUI
            CUI_statusLinePrintf(
                clientHandle, deviceRoleLine,
                "["CUI_COLOR_RED "Device Role" CUI_COLOR_RESET "] Leader");
#endif /* TIOP_CUI */

#if !TIOP_POWER_MEASUREMENT
            LED_setOn(greenLedHandle, LED_BRIGHTNESS_MAX);
            LED_setOn(redLedHandle, LED_BRIGHTNESS_MAX);
#endif /* !TIOP_POWER_MEASUREMENT */
            break;
        }
    }
}

#ifdef USE_DMM
void tiopCUIUpdateChannelMask(otChannelMask mask){
    channelMask = mask;
    CUI_statusLinePrintf(
        clientHandle, nwkInfoLine4,
        "[" CUI_COLOR_YELLOW "Channel Mask" CUI_COLOR_RESET "] 0x%08x",
        (uint32_t)mask);
}
#endif

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateConnStatus(cuiConn status)
{
    switch(status)
    {
        case CUI_conn_disabled:
        {
            CUI_statusLinePrintf(
                clientHandle, connStatusLine,
                "[" CUI_COLOR_RED "Status" CUI_COLOR_RESET "] Disabled");
            break;
        }
        case CUI_conn_joining:
        {
            CUI_statusLinePrintf(
                clientHandle, connStatusLine,
                "[" CUI_COLOR_RED "Status" CUI_COLOR_RESET "] Joining Nwk ...");
            break;
        }
        case CUI_conn_joined:
        {
            CUI_statusLinePrintf(
                clientHandle, connStatusLine,
                "[" CUI_COLOR_RED "Status" CUI_COLOR_RESET "] Nwk Joined");
            break;
        }
        case CUI_conn_join_fail:
        {
            CUI_statusLinePrintf(
                clientHandle, connStatusLine,
                "[" CUI_COLOR_RED "Status" CUI_COLOR_RESET "] Join Failure");
            break;
        }
    }
}

/**
 * documented in tiop_ui.h
 */
void tiopCUIUpdateApp(char* appInfo)
{
    CUI_statusLinePrintf(clientHandle, appInfoLine, appInfo);
}

/**
 * documented in tiop_ui.h
 */
void uiActionProcessConfigurePanId(const char _input,
                                   char* _pLines[3],
                                   CUI_cursorInfo_t* _pCurInfo)
{
    /* variables are static to preserved across calling this function */
    static CUI_cursorInfo_t cursor = {0, 6};
    static uint16_t panId = TIOP_CONFIG_PAN_ID;
    const char tmpInput[2] = {_input, '\0'};

    switch (_input) {

        case CUI_ITEM_INTERCEPT_START:
        {
            OtRtosApi_lock();
            panId = (uint16_t)otLinkGetPanId(OtInstance_get());
            OtRtosApi_unlock();
            break;
        }
        // Submit the final modified value
        case CUI_ITEM_INTERCEPT_STOP:
        {
            otError error;

            OtRtosApi_lock();
            error = otLinkSetPanId(OtInstance_get(), (otPanId)panId);
            OtRtosApi_unlock();

            if (OT_ERROR_NONE == error)
            {
                tiopCUIUpdatePANID((otPanId)panId);
            }
            // Reset the local cursor info
            cursor.col = 6;
            break;
        }
        // Show the value of this screen w/o making changes
        case CUI_ITEM_PREVIEW:
        {
            break;
        }
        // Move the cursor to the left
        case CUI_INPUT_LEFT:
        {
            if (6 != cursor.col)
            {
                cursor.col--;
            }
            break;
        }
        // Move the cursor to the right
        case CUI_INPUT_RIGHT:
        {
            if (9 != cursor.col)
            {
                cursor.col++;
            }
            break;
        }
        // Increment the digit
        case CUI_INPUT_UP:
        {
            uint8_t shift = 4 * (9 - cursor.col);
            uint8_t digit = (panId >> shift) & 0xF;
            digit = (digit + 1 + 16) % 16;

            panId &= ~((uint32_t)0xF << shift);
            panId |= (uint32_t)digit << shift;
            break;
        }
        // Decrement the digit
        case CUI_INPUT_DOWN:
        {
            uint8_t shift = 4 * (9 - cursor.col);
            uint8_t digit = (panId >> shift) & 0xF;
            digit = (digit - 1 + 16) % 16;

            panId &= ~((uint32_t)0xF << shift);
            panId |= (uint32_t)digit << shift;
            break;
        }
        case CUI_INPUT_EXECUTE:
        {
            break;
        }
        default:
        {
            // is it a hex number
            if(CUI_IS_INPUT_HEX(_input))
            {
                /* multiply by 4 because you're working with binary numbers,
                and half a byte of hex = 4 bits. Calculate the bit shift
                based on the end of the line - the current column, to
                modify specific values.  */
                uint8_t shift = 4 * (9 - cursor.col);

                // convert from ascii to hex
                uint8_t digit = strtol(tmpInput, NULL, 16);

                // first clear the specific hex half byte, in the desired spot
                panId &= ~((uint32_t)0xF << shift);
                // then set the digit you have typed in.
                panId |= (uint32_t)digit << shift;
                if (9 != cursor.col)
                {
                    cursor.col++;
                }
            }
        }
    }

    if (panId == 0xFFFF)
    {
        strcpy(_pLines[0], "    0xFFFF (any)");
    }
    else
    {
        snprintf(_pLines[0], 16, "    0x%04x", panId);
    }

    if (_input != CUI_ITEM_PREVIEW) {
        strcpy(_pLines[2], "     PAN ID");
        _pCurInfo->row = 1;
        _pCurInfo->col = cursor.col+1;
    }
}

/**
 * documented in tiop_ui.h
 */
void uiActionProcessConfigureChannel(const char _input,
                                     char* _pLines[3],
                                     CUI_cursorInfo_t* _pCurInfo)
{
    /* variables are static to preserved across calling this function */
    static CUI_cursorInfo_t cursor = {0, 4};
    /* chArr is static so the menu is common across button presses */
    static char chArr[3] = {0};

    switch (_input) {
        // Show the value of this screen without making changes
        case CUI_ITEM_PREVIEW:
        // Start editing the value
        case CUI_ITEM_INTERCEPT_START:
        {
            snprintf(chArr, sizeof(chArr), "%02d", display_channel);
            break;
        }
        // Submit the final modified value
        case CUI_ITEM_INTERCEPT_STOP:
        {
            uint8_t ch = atoi(chArr);
            otError error;

            OtRtosApi_lock();
            error = otLinkSetChannel(OtInstance_get(), ch);
            OtRtosApi_unlock();

            if (OT_ERROR_NONE == error)
            {
                tiopCUIUpdateChannel(ch);
            }
            // Reset the local cursor info
            cursor.col = 4;
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
            if (5 != cursor.col)
            {
                cursor.col++;
            }
            break;

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
        strcpy(_pLines[2], "     CHANNEL");
        _pCurInfo->row = 1;
        _pCurInfo->col = cursor.col+1;
    }
}
