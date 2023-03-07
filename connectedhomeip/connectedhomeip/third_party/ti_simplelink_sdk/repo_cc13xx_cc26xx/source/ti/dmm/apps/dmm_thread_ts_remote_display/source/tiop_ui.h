/******************************************************************************

 @file tiop_ui.h

 @brief TI-OpenThread CUI Functions

 Group: CMCU, LPC
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2017-2020, Texas Instruments Incorporated
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

#ifndef TIOP_UI_H_
#define TIOP_UI_H_

#include "cui.h"
#include <openthread/thread.h>

/**
 * Connection status
 */
typedef enum
{
    CUI_conn_disabled,
    CUI_conn_joining,
    CUI_conn_joined,
    CUI_conn_join_fail,
} cuiConn;

/**
 * @brief Initialize example CUI functionality
 *
 * @param rightButtonHandle a pointer to the button handle to be filled for the
 *        application
 * @param appString contains the application specific string to be output
 *
 * @return None
 */
extern void tiopCUIInit(char* appString,
                        Button_Handle* rightButtonHandle);

/**
 * @brief Update device role CUI output line
 *
 * @param role contains the role the device is in the network
 *
 * @return None
 */
extern void tiopCUIUpdateRole(otDeviceRole role);

/**
 * @brief Update device info CUI output line
 *
 * @return None
 */
extern void tiopCUIUpdateDeviceInfo(void);

/**
 * @brief Update panid CUI output
 *
 * @param pan contains the panid for the network
 *
 * @return None
 */
extern void tiopCUIUpdatePANID(otPanId pan);

/**
 * @brief Update channel CUI output
 *
 * @param ch contains the channel for the network
 *
 * @return None
 */
extern void tiopCUIUpdateChannel(uint8_t ch);

/**
 * @brief Update short address CUI output
 *
 * @param s contains the short address for the device
 *
 * @return None
 */
extern void tiopCUIUpdateShortAddr(otShortAddress s);

/**
 * @brief Update masterkey CUI output
 *
 * @param key contains the masterkey for the network
 *
 * @return None
 */
extern void tiopCUIUpdateMasterkey(otMasterKey key);

/**
 * @brief Update Extended panid CUI output
 *
 * @param extPan contains the extended pan id for the network
 *
 * @return None
 */
extern void tiopCUIUpdateExtPANID(otExtendedPanId extPan);

/**
 * @brief Update network name CUI output
 *
 * @param name contains the name of the network
 *
 * @note @ref name must be in persistent memory as the pointer is kept for
 * future screen refreshes
 *
 * @return None
 */
extern void tiopCUIUpdateNwkName(const char* name);

/**
 * @brief Update chanenl mask CUI output line
 *
 * @param mask contains the channel mask for network
 *
 * @return None
 */
#ifdef USE_DMM
extern void tiopCUIUpdateChannelMask(otChannelMask mask);
#endif
/**
 * @brief Update connection status CUI output line
 *
 * @param status contains the connection status for the device to the network
 *
 * @return None
 */
extern void tiopCUIUpdateConnStatus(cuiConn status);

/**
 * @brief Update Application specific CUI output line
 *
 * @param appInfo contains the application specific string to be output
 *
 * @return None
 */
extern void tiopCUIUpdateApp(char* appInfo);

/**
 * @brief Handle reading user input into the panid configuration in CUI
 *
 * @return None
 */
extern void uiActionProcessConfigurePanId(const char _input, char* _pLines[3],
                                          CUI_cursorInfo_t* _pCurInfo);

/**
 * @brief Handle reading user input into the channel configuration in CUI
 *
 * @return None
 */
extern void uiActionProcessConfigureChannel(const char _input,
                                            char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);


#ifdef USE_DMM
/**
 * @brief Handle reading user input into the report interval configuration in CUI
 *
 * @return None
 */
extern void uiActionProcessConfigureReportInterval(const char _input,
                                            char* _pLines[3], CUI_cursorInfo_t* _pCurInfo);
#endif

/**
 * @brief Handles attach menu option event for examples
 *
 * @return None
 */
extern void uiActionAttach(const int32_t _itemEntry);

/**
 * @brief Handles join menu option event for examples
 *
 * @return None
 */
extern void uiActionJoin(const int32_t _itemEntry);

/**
 * @brief Handles the key pressed events for examples
 *
 * @return None
 */
extern void processKeyChangeCB(Button_Handle _buttonHandle,
                               Button_EventMask _buttonEvents);

/**
 * @brief Handles CUI Menu update events for examples
 *
 * @return None
 */
extern void processMenuUpdateFn(void);

/**
 * @brief Reset the OpenThread Instance as well as clear out the previous
 *        network data. CUI function type that is called when the Nwk reset
 *        menu option is selected in the Example CUI menu.
 *
 * @return None
 */
extern void tiopCUINwkReset(const char _input, char* _pLines[3],
                            CUI_cursorInfo_t* _pCurInfo);

/**
 * @brief Reset the OpenThread Instance without clearing out previous network
 *        data. CUI function type that is called when the reset menu option is
 *        selected in the Example CUI menu.
 *
 * @return None
 */
extern void tiopCUIReset(const char _input, char* _pLines[3],
                         CUI_cursorInfo_t* _pCurInfo);


#endif /* TIOP_UI_H_ */
