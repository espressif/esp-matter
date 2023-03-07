/******************************************************************************

 @file  simple_mesh_node_menu.h

 @brief This file contains menu objects for mesh and simple_peripheral.

 Group: WCS BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2016-2022, Texas Instruments Incorporated
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

#ifndef SIMPLE_MESH_NODE_MENU_H
#define SIMPLE_MESH_NODE_MENU_H

#ifdef __cplusplus
extern "C"
{
#endif

// Menu item indices.
#define SP_ITEM_NONE            TBM_ITEM_NONE
#define SP_ITEM_ALL             TBM_ITEM_ALL

#define SMN_ROW_SEPARATOR_1   (TBM_ROW_APP + 0)
#define SMN_ROW_MENU_ST       (TBM_ROW_APP + 1)
#define SMN_ROW_INIT_ST       (TBM_ROW_APP + 3)
#define SMN_ROW_TRGT_ST       (TBM_ROW_APP + 4)
#define SMN_ROW_CALL_ST       (TBM_ROW_APP + 5)
#define SMN_ROW_CBCK_ST       (TBM_ROW_APP + 6)
#define SMN_ROW_EVNT_ST       (TBM_ROW_APP + 7)
#define SMN_ROW_NTFY_ST       (TBM_ROW_APP + 8)
#define SMN_ROW_MESH_CB       (TBM_ROW_APP + 9)
#define SMN_ROW_BTN           (TBM_ROW_APP + 10)
#define SMN_ROW_LED           (TBM_ROW_APP + 11)
#define SMN_ROW_GEN_MODELS    (TBM_ROW_APP + 12)
#define SMN_ROW_SENSOR_MODELS (TBM_ROW_APP + 13)
#define SMN_ROW_UUID          (TBM_ROW_APP + 15)
#ifdef ADD_SIMPLE_PERIPHERAL
#define TBM_ROW_SP            (TBM_ROW_APP + 14)
// Row numbers for two-button menu
#define SP_ROW_SEPARATOR_1   (TBM_ROW_SP + 0)
#define SP_ROW_STATUS_1      (TBM_ROW_SP + 1)
#define SP_ROW_STATUS_2      (TBM_ROW_SP + 2)
#define SP_ROW_CONNECTION    (TBM_ROW_SP + 3)
#define SP_ROW_ADVSTATE      (TBM_ROW_SP + 4)
#define SP_ROW_RSSI          (TBM_ROW_SP + 5)
#define SP_ROW_IDA           (TBM_ROW_SP + 6)
#define SP_ROW_RPA           (TBM_ROW_SP + 7)
#define SP_ROW_DEBUG         (TBM_ROW_SP + 8)
#define SP_ROW_AC            (TBM_ROW_SP + 9)

// Menu item indices.
#define SP_ITEM_NONE            TBM_ITEM_NONE
#define SP_ITEM_ALL             TBM_ITEM_ALL

// Note: The defines should be updated accordingly if there is any change
//       in the order of the items of the menu objects the items belong to.
#define SP_ITEM_SELECT_CONN     TBM_ITEM(3)  // "Work with"
#define SP_ITEM_AUTOCONNECT     TBM_ITEM(4)  // "Auto Connect"
#ifdef OAD_DEBUG
#define SP_ITEM_SELECT_OAD_DBG  TBM_ITEM(5)  // "OAD Debug"
#endif
#endif // ADD_SIMPLE_PERIPHERAL

/*
 * Menus Declarations
 */

/* Main Menu Object */
extern tbmMenuObj_t smMenuMain;

/* Items of (Main) */
/* Action items are defined in application */

/*
 * Menus Declarations
 */

/* Main Menu Object */
extern tbmMenuObj_t smMenuMain;
extern tbmMenuObj_t smMenuInit;
extern tbmMenuObj_t smMenuOwnAddr;
extern tbmMenuObj_t smMenuButton;
extern tbmMenuObj_t smGenModels;
extern tbmMenuObj_t smMenuBtnTarget;
extern tbmMenuObj_t smMenuLed;
extern tbmMenuObj_t smMenuLedTarget;

#ifdef ADD_SIMPLE_PERIPHERAL
#define spMenuMain  smMenuMain
extern tbmMenuObj_t spMenuAutoConnect;
extern tbmMenuObj_t spMenuSelectConn;
extern tbmMenuObj_t spMenuPerConn;
extern tbmMenuObj_t spMenuConnPhy;
#endif

#ifdef OAD_DEBUG
extern tbmMenuObj_t spMenuOADDebug;
extern tbmMenuObj_t spMenuEraseImg;
extern tbmMenuObj_t spMenuDisplayImgInfo;
#endif

void SimpleMeshMenu_init(Display_Handle dispHandleValue);
void SimpleMeshMenu_turnLed(uint8_t on_off);
void SimpleMeshMenu_showBtn(uint8_t on_off);
void SimpleMeshMenu_turnGenOnOff(uint8_t on_off);
void SimpleMeshMenu_setSensorLine(uint8_t *string, uint16_t value);
void SimpleMeshMenu_setGenBatteryLineBytes(uint8_t level, uint32_t time_to_discharge, uint32_t time_to_charge, uint8_t flags);
void SimpleMeshMenu_setSensorLineBytes(uint8_t *string, uint8_t *pBuf, int bufLen);

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_MESH_NODE_MENU_H */
