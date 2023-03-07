/******************************************************************************

@file  simple_mesh_node_menu.c

@brief This file contains the mesh and simple peripheral menu configuration for use
with the CC2650 Bluetooth Low Energy Protocol Stack.

Group: WCS, BTS
Target Device: cc13xx_cc26xx

******************************************************************************

 Copyright (c) 2013-2022, Texas Instruments Incorporated
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

#include <bcomdef.h>
#include <ti/display/Display.h>
#include <stdio.h>

#include <menu/two_btn_menu.h>
#include "simple_mesh_node.h"
#include "simple_mesh_node_menu.h"

#ifdef OAD_DEBUG
#include "oad_efl_dbg.h"
#endif
#include "ti_ble_config.h"

/* Main Menu Object */
tbmMenuObj_t smMenuMain;
tbmMenuObj_t smMenuInit;
tbmMenuObj_t smMenuUUID;
tbmMenuObj_t smMenuButton;
tbmMenuObj_t smMenuBtnTarget;
tbmMenuObj_t smMenuLed;
tbmMenuObj_t smMenuLedTarget;
tbmMenuObj_t smMenuVndLargeMsg;
tbmMenuObj_t smMenuVndLargeMsgTarget;
tbmMenuObj_t smMenuGenericModels;
tbmMenuObj_t smMenuSensorModels;
tbmMenuObj_t smMenuGenOnOffModels;
tbmMenuObj_t smMenuGenLevelModels;
tbmMenuObj_t smMenuGenTransTimeModels;
tbmMenuObj_t smMenuGenPowerOnOffModels;
tbmMenuObj_t smMenuGenBatteryModels;
tbmMenuObj_t smMenuGenModelsTarget;
tbmMenuObj_t smMenuSensorModelsClient;
tbmMenuObj_t smMenuSensorModelsServer;
tbmMenuObj_t smMenuSensorModelsTarget;

#ifdef ADD_SIMPLE_PERIPHERAL
tbmMenuObj_t spMenuAutoConnect;
tbmMenuObj_t spMenuSelectConn;
tbmMenuObj_t spMenuPerConn;
tbmMenuObj_t spMenuConnPhy;
static void SimplePeripheralMenu_switchCb(tbmMenuObj_t* pMenuObjCurr, tbmMenuObj_t* pMenuObjNext);
#ifdef OAD_DEBUG
tbmMenuObj_t spMenuOADDebug;
tbmMenuObj_t spMenuEraseImg;
tbmMenuObj_t spMenuDisplayImgInfo;
#endif
#endif

/* Actions for mesh Menu */
bool SimpleMeshMenu_doInitMenu(uint8 index);
bool SimpleMeshMenu_doBtnTargetAddr(uint8 index);
bool SimpleMeshMenu_doLed(uint8 index);
bool SimpleMeshMenu_doLedTargetAddr(uint8 index);
bool SimpleMeshMenu_doButton(uint8 index);
bool SimpleMeshMenu_doLargeMsgTargetAddr(uint8_t index);
bool SimpleMeshMenu_doLargeMsg(uint8 index);
bool SimpleMeshMenu_doGenOnOffModels(uint8 index);
bool SimpleMeshMenu_doGenLevelModels(uint8 index);
bool SimpleMeshMenu_doGenTransTimeModels(uint8 index);
bool SimpleMeshMenu_doGenPowerOnOffModels(uint8 index);
bool SimpleMeshMenu_doGenBatteryModels(uint8 index);
bool SimpleMeshMenu_doSensorModelsClient(uint8_t index);
bool SimpleMeshMenu_doSensorModelsServer(uint8_t index);
bool SimpleMeshMenu_doGenModelsTargetAddr(uint8 index);
bool SimpleMeshMenu_doSensorModelsTargetAddr(uint8 index);
bool SimpleMeshMenu_doSetUUID(uint8_t index);
void SimpleMeshMenu_printUuid();

#ifdef ADD_SIMPLE_PERIPHERAL
/* Actions for simple peripheral Menu */
bool SimplePeripheralMenu_doSelectConn(uint8 index);
bool SimplePeripheralMenu_doAutoConnect(uint8_t index);
bool SimplePeripheralMenu_doSetConnPhy(uint8 index);
#endif
/*
 * Menu Lists Initializations
 */

void SimpleMeshMenu_build(void)
{
// Menu: Main
// upper: none
#ifdef ADD_SIMPLE_PERIPHERAL
#ifdef OAD_DEBUG
  MENU_OBJ(smMenuMain, NULL, 9, NULL)
#else
  MENU_OBJ(smMenuMain, NULL, 8, NULL)
#endif // OAD_DEBUG
#else
  MENU_OBJ(smMenuMain, NULL, 6, NULL)
#endif // ADD_SIMPLE_PERIPHERAL
  MENU_ITEM_SUBMENU(smMenuMain,0,&smMenuInit)
  MENU_ITEM_SUBMENU(smMenuMain,1,&smMenuButton)
  MENU_ITEM_SUBMENU(smMenuMain,2,&smMenuLed)
  MENU_ITEM_SUBMENU(smMenuMain,3,&smMenuVndLargeMsg)
  MENU_ITEM_SUBMENU(smMenuMain,4,&smMenuGenericModels)
  MENU_ITEM_SUBMENU(smMenuMain,5,&smMenuSensorModels)
#ifdef ADD_SIMPLE_PERIPHERAL
  MENU_ITEM_SUBMENU(smMenuMain,6,&spMenuSelectConn)
  MENU_ITEM_SUBMENU(smMenuMain,7,&spMenuAutoConnect)
#ifdef OAD_DEBUG
  MENU_ITEM_SUBMENU(spMenuMain,8,&spMenuOADDebug)
#endif
#endif // ADD_SIMPLE_PERIPHERAL

  MENU_OBJ_END

// Menu:  INIT
// upper: Main
#ifdef CONFIG_BT_SETTINGS
  MENU_OBJ(smMenuInit, "MESH INIT menu", 3, &smMenuMain)
  MENU_ITEM_SUBMENU(smMenuInit,0,&smMenuUUID)
#ifdef USE_STATIC_PROVISIONING
  MENU_ITEM_ACTION(smMenuInit,1,"MeshInit & static provisioning", SimpleMeshMenu_doInitMenu)
#else
  MENU_ITEM_ACTION(smMenuInit,1,"MeshInit unprovisioned beacon", SimpleMeshMenu_doInitMenu)
#endif // USE_STATIC_PROVISIONING
  MENU_ITEM_ACTION(smMenuInit,2,"MeshInit & try to load from NV", SimpleMeshMenu_doInitMenu)

#else
  MENU_OBJ(smMenuInit, "MESH INIT menu", 2, &smMenuMain)
  MENU_ITEM_SUBMENU(smMenuInit,0,&smMenuUUID)
#ifdef USE_STATIC_PROVISIONING
  MENU_ITEM_ACTION(smMenuInit,1,"MeshInit & static provisioning", SimpleMeshMenu_doInitMenu)
#else
  MENU_ITEM_ACTION(smMenuInit,1,"MeshInit unprovisioned beacon", SimpleMeshMenu_doInitMenu)
#endif // USE_STATIC_PROVISIONING
#endif
  MENU_OBJ_END

// Menu:  UUID
// upper: MESH INIT
  MENU_OBJ(smMenuUUID, "MESH set UUID prior to provisioning", 11, &smMenuInit)
  MENU_ITEM_ACTION(smMenuUUID,0,"Restore default UUID", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,1,"Set UUID to 0x1111111111111111a111111111111111", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,2,"Set UUID to 0x2222222222221222a222222222222222", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,3,"Set UUID to 0x3333333333331333a333333333333333", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,4,"Set UUID to 0x4444444444441444a444444444444444", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,5,"Set UUID to 0x5555555555551555a555555555555555", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,6,"Set UUID to 0x6666666666661666a666666666666666", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,7,"Set UUID to 0x7777777777771777a777777777777777", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,8,"Set UUID to 0x8888888888881888a888888888888888", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,9,"Set UUID to 0x9999999999991999a999999999999999", SimpleMeshMenu_doSetUUID)
  MENU_ITEM_ACTION(smMenuUUID,10,"Set UUID to 0xaaaaaaaaaaaa1aaaaaaaaaaaaaaaaaaa", SimpleMeshMenu_doSetUUID)
  MENU_OBJ_END

// Menu:  BUTTON
// upper: Main
  MENU_OBJ(smMenuButton, "MESH BUTTON menu", 4, &smMenuMain)
  MENU_ITEM_ACTION(smMenuButton,0,"Turn LED ON",    SimpleMeshMenu_doButton)
  MENU_ITEM_ACTION(smMenuButton,1,"Turn LED OFF",   SimpleMeshMenu_doButton)
  MENU_ITEM_ACTION(smMenuButton,2,"Get LED status", SimpleMeshMenu_doButton)
  MENU_ITEM_SUBMENU(smMenuButton,3,&smMenuBtnTarget)
  MENU_OBJ_END

// Menu:  BtnTarget
// upper: BUTTON
  MENU_OBJ(smMenuBtnTarget, "Set btn target addr", 11, &smMenuButton)
  MENU_ITEM_ACTION(smMenuBtnTarget,0,"Set target Addr Group", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,1,"Set target Addr 1", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,2,"Set target Addr 2", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,3,"Set target Addr 3", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,4,"Set target Addr 4", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,5,"Set target Addr 5", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,6,"Set target Addr 6", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,7,"Set target Addr 7", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,8,"Set target Addr 8", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,9,"Set target Addr 9", SimpleMeshMenu_doBtnTargetAddr)
  MENU_ITEM_ACTION(smMenuBtnTarget,10,"Set target Addr 10", SimpleMeshMenu_doBtnTargetAddr)
  MENU_OBJ_END

// Menu:  LED
// upper: Main
  MENU_OBJ(smMenuLed, "MESH LED menu", 2, &smMenuMain)
  MENU_ITEM_ACTION(smMenuLed,0,"Notify LED status",  SimpleMeshMenu_doLed)
  MENU_ITEM_SUBMENU(smMenuLed,1,&smMenuLedTarget)
  MENU_OBJ_END

// Menu:  LedTarget
// upper: LED
  MENU_OBJ(smMenuLedTarget, "Set led target addr", 11, &smMenuLed)
  MENU_ITEM_ACTION(smMenuLedTarget,0,"Set target Addr Group", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,1,"Set target Addr 1", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,2,"Set target Addr 2", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,3,"Set target Addr 3", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,4,"Set target Addr 4", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,5,"Set target Addr 5", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,6,"Set target Addr 6", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,7,"Set target Addr 7", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,8,"Set target Addr 8", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,9,"Set target Addr 9", SimpleMeshMenu_doLedTargetAddr)
  MENU_ITEM_ACTION(smMenuLedTarget,10,"Set target Addr 10", SimpleMeshMenu_doLedTargetAddr)
  MENU_OBJ_END

  // Menu:  VND Large MSG
  // upper: Main
  MENU_OBJ(smMenuVndLargeMsg, "MESH VND Large MSG menu", 6, &smMenuMain)
  MENU_ITEM_SUBMENU(smMenuVndLargeMsg,0,&smMenuVndLargeMsgTarget)
  MENU_ITEM_ACTION(smMenuVndLargeMsg,1,"Send 50 bytes of data", SimpleMeshMenu_doLargeMsg)
  MENU_ITEM_ACTION(smMenuVndLargeMsg,2,"Send 100 bytes of data", SimpleMeshMenu_doLargeMsg)
  MENU_ITEM_ACTION(smMenuVndLargeMsg,3,"Send 130 bytes of data", SimpleMeshMenu_doLargeMsg)
  MENU_ITEM_ACTION(smMenuVndLargeMsg,4,"Send 150 bytes of data", SimpleMeshMenu_doLargeMsg)
  MENU_ITEM_ACTION(smMenuVndLargeMsg,5,"Send 169 bytes of data", SimpleMeshMenu_doLargeMsg)
  MENU_OBJ_END

  // Menu:  VndLargeMsgTarget
  // upper: VND Large MSG
  MENU_OBJ(smMenuVndLargeMsgTarget, "Set VND large msg target addr", 11, &smMenuVndLargeMsg)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,0,"Set target Addr Group", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,1,"Set target Addr 1", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,2,"Set target Addr 2", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,3,"Set target Addr 3", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,4,"Set target Addr 4", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,5,"Set target Addr 5", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,6,"Set target Addr 6", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,7,"Set target Addr 7", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,8,"Set target Addr 8", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,9,"Set target Addr 9", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_ITEM_ACTION(smMenuVndLargeMsgTarget,10,"Set target Addr 10", SimpleMeshMenu_doLargeMsgTargetAddr)
  MENU_OBJ_END

  // Menu: Generic models
  // upper: Main
  MENU_OBJ(smMenuGenericModels, "GENERIC models menu", 6, &smMenuMain)
  MENU_ITEM_SUBMENU(smMenuGenericModels,0,&smMenuGenOnOffModels)
  MENU_ITEM_SUBMENU(smMenuGenericModels,1,&smMenuGenLevelModels)
  MENU_ITEM_SUBMENU(smMenuGenericModels,2,&smMenuGenTransTimeModels)
  MENU_ITEM_SUBMENU(smMenuGenericModels,3,&smMenuGenPowerOnOffModels)
  MENU_ITEM_SUBMENU(smMenuGenericModels,4,&smMenuGenBatteryModels)
  MENU_ITEM_SUBMENU(smMenuGenericModels,5,&smMenuGenModelsTarget)
  MENU_OBJ_END

// Menu:  GENERIC ONOFF MODELS
// upper: Main
  MENU_OBJ(smMenuGenOnOffModels, "GENERIC ONOFF models menu", 8, &smMenuGenericModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,0,"cli gen_OnOff_get",               SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,1,"cli gen_OnOff_set",               SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,2,"cli gen_OnOff_set_unack",         SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,3,"prepare onOff=0",                 SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,4,"prepare onOff=1",                 SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,5,"prepare onOff timing 0x00,0x0",   SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,6,"prepare onOff timing 0x54,0x5",   SimpleMeshMenu_doGenOnOffModels)
  MENU_ITEM_ACTION(smMenuGenOnOffModels,7,"prepare onOff timing without",    SimpleMeshMenu_doGenOnOffModels)
  MENU_OBJ_END

  // Menu:  GENERIC LEVEL MODELS
  // upper: Main
  MENU_OBJ(smMenuGenLevelModels, "GENERIC LEVEL models menu", 11, &smMenuGenericModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,0,"cli gen_level_get",              SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,1,"cli gen_level_set",              SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,2,"cli gen_level_set_unack",        SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,3,"cli gen_delta_set",              SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,4,"cli gen_delta_set_unack",        SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,5,"cli gen_move_set",               SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,6,"cli gen_move_set_unack",         SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,7,"prepare level=0x01FF",           SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,8,"prepare level timing 0xC0,0x0",  SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,9,"prepare level timing 0x54,0x5",  SimpleMeshMenu_doGenLevelModels)
  MENU_ITEM_ACTION(smMenuGenLevelModels,10,"prepare level timing without",  SimpleMeshMenu_doGenLevelModels)
  MENU_OBJ_END

  // Menu:  GENERIC TRANSITION TIME MODELS
  // upper: Main
  MENU_OBJ(smMenuGenTransTimeModels, "GENERIC TRANSITION TIME models menu", 5, &smMenuGenericModels)
  MENU_ITEM_ACTION(smMenuGenTransTimeModels,0,"cli gen_TransTime_get",        SimpleMeshMenu_doGenTransTimeModels)
  MENU_ITEM_ACTION(smMenuGenTransTimeModels,1,"cli gen_TransTime_set",        SimpleMeshMenu_doGenTransTimeModels)
  MENU_ITEM_ACTION(smMenuGenTransTimeModels,2,"cli gen_TransTime_set_unack",  SimpleMeshMenu_doGenTransTimeModels)
  MENU_ITEM_ACTION(smMenuGenTransTimeModels,3,"prepare TransTime=0xCF",       SimpleMeshMenu_doGenTransTimeModels)
  MENU_ITEM_ACTION(smMenuGenTransTimeModels,4,"prepare TransTime=0x8F",       SimpleMeshMenu_doGenTransTimeModels)
  MENU_OBJ_END

  // Menu:  GENERIC POWER ONOFF MODELS
  // upper: Main
  MENU_OBJ(smMenuGenPowerOnOffModels, "GENERIC POWER ONOFF models menu", 7, &smMenuGenericModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,0,"cli gen_Power_OnOff_set ON",             SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,1,"cli gen_Power_OnOff_set OFF",            SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,2,"cli gen_Power_OnOff_set Restore",        SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,3,"cli gen_Power_OnOff_get",                SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,4,"cli gen_Power_OnOff_set_unack ON",       SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,5,"cli gen_Power_OnOff_set_unack OFF",      SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_ITEM_ACTION(smMenuGenPowerOnOffModels,6,"cli gen_Power_OnOff_set_unack Restore",  SimpleMeshMenu_doGenPowerOnOffModels)
  MENU_OBJ_END

  // Menu:  GENERIC BATTERY MODELS
  // upper: Main
  MENU_OBJ(smMenuGenBatteryModels, "GENERIC BATTERY model menu", 1, &smMenuGenericModels)
  MENU_ITEM_ACTION(smMenuGenBatteryModels,0,"cli gen_Battery_get", SimpleMeshMenu_doGenBatteryModels)
  MENU_OBJ_END

  // Menu:  GenModelsTargetAddr
  // upper: Main
  MENU_OBJ(smMenuGenModelsTarget, "Set GENERIC models target addr", 11, &smMenuGenericModels)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,0,"Set target Addr Group", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,1,"Set target Addr 1", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,2,"Set target Addr 2", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,3,"Set target Addr 3", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,4,"Set target Addr 4", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,5,"Set target Addr 5", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,6,"Set target Addr 6", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,7,"Set target Addr 7", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,8,"Set target Addr 8", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,9,"Set target Addr 9", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuGenModelsTarget,10,"Set target Addr 10", SimpleMeshMenu_doGenModelsTargetAddr)
  MENU_OBJ_END

  // Menu: Sensor models
  // upper: Main
  MENU_OBJ(smMenuSensorModels, "SENSOR models menu", 3, &smMenuMain)
  MENU_ITEM_SUBMENU(smMenuSensorModels,0,&smMenuSensorModelsClient)
  MENU_ITEM_SUBMENU(smMenuSensorModels,1,&smMenuSensorModelsServer)
  MENU_ITEM_SUBMENU(smMenuSensorModels,2,&smMenuSensorModelsTarget)
  MENU_OBJ_END

  // Menu:  SENSOR MODELS
  // upper: Main
  MENU_OBJ(smMenuSensorModelsClient, "SENSOR models Client menu", 14, &smMenuSensorModels)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,0,"cli sensor_desc_get",        SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,1,"cli sensor_get",             SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,2,"cli sensor_Column_get",      SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,3,"cli sensor_Series_get",      SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,4,"cli set property ID tmpr8",  SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,5,"cli set property ID 0x42",   SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,6,"cli set property ID all",    SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,7,"cli sensor_settings_get",     SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,8,"cli sensor_cadence_get",      SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,9,"cli sensor_cadence_set",      SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,10,"cli sensor_cadence_set_unack",SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,11,"cli sensor_setting_get",      SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,12,"cli sensor_setting_set",      SimpleMeshMenu_doSensorModelsClient)
  MENU_ITEM_ACTION(smMenuSensorModelsClient,13,"cli sensor_setting_set_unack",SimpleMeshMenu_doSensorModelsClient)
  MENU_OBJ_END

  // Menu:  SENSOR MODELS
  // upper: Main
  MENU_OBJ(smMenuSensorModelsServer, "SENSOR models Server menu", 3, &smMenuSensorModels)
  MENU_ITEM_ACTION(smMenuSensorModelsServer,0,"srv simulate temperature 36 Celsius ",  SimpleMeshMenu_doSensorModelsServer)
  MENU_ITEM_ACTION(smMenuSensorModelsServer,1,"srv simulate temperature 40 Celsius ",  SimpleMeshMenu_doSensorModelsServer)
  MENU_ITEM_ACTION(smMenuSensorModelsServer,2,"srv sensor_status",                       SimpleMeshMenu_doSensorModelsServer)
  MENU_OBJ_END

  // Menu:  SensorModelsTargetAddr
  // upper: Main
  MENU_OBJ(smMenuSensorModelsTarget, "Set SENSOR models target addr", 11, &smMenuSensorModels)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,0,"Set target Addr Group", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,1,"Set target Addr 1", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,2,"Set target Addr 2", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,3,"Set target Addr 3", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,4,"Set target Addr 4", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,5,"Set target Addr 5", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,6,"Set target Addr 6", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,7,"Set target Addr 7", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,8,"Set target Addr 8", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,9,"Set target Addr 9", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_ITEM_ACTION(smMenuSensorModelsTarget,10,"Set target Addr 10", SimpleMeshMenu_doSensorModelsTargetAddr)
  MENU_OBJ_END


#ifndef ADD_SIMPLE_PERIPHERAL
  MENU_OBJ_END
}
#endif

#ifdef ADD_SIMPLE_PERIPHERAL
// Menu: SelectDev
// upper: Main
// NOTE: The number of items in this menu object shall be
//       equal to or greater than MAX_NUM_BLE_CONNS
  MENU_OBJ(spMenuSelectConn, "SP Work with", MAX_NUM_BLE_CONNS, &smMenuMain)
  MENU_ITEM_MULTIPLE_ACTIONS(spMenuSelectConn, MAX_NUM_BLE_CONNS, NULL, SimplePeripheralMenu_doSelectConn)
  MENU_OBJ_END

// Menu:  AutoConnect
// upper: Main
  MENU_OBJ(spMenuAutoConnect, "SP Set AutoConnect", 3, &smMenuMain)
  MENU_ITEM_ACTION(spMenuAutoConnect,0,"Disable", SimplePeripheralMenu_doAutoConnect)
  MENU_ITEM_ACTION(spMenuAutoConnect,1,"Group A", SimplePeripheralMenu_doAutoConnect)
  MENU_ITEM_ACTION(spMenuAutoConnect,2,"Group B", SimplePeripheralMenu_doAutoConnect)
  MENU_OBJ_END

// Menu: PerConnection
// upper: SelectDevice
  MENU_OBJ(spMenuPerConn, NULL, 1, &spMenuSelectConn)
  MENU_ITEM_SUBMENU(spMenuPerConn,0,&spMenuConnPhy)
  MENU_OBJ_END

// Menu: ConnPhy
// upper: Select Device
  MENU_OBJ(spMenuConnPhy, "Set Conn PHY Preference", 6, &spMenuPerConn)
  MENU_ITEM_ACTION(spMenuConnPhy,0,"1 Mbps",              SimplePeripheralMenu_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,1,"2 Mbps",              SimplePeripheralMenu_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,2,"1 & 2 Mbps",          SimplePeripheralMenu_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,3,"Coded",               SimplePeripheralMenu_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,4,"1 & 2 Mbps, & Coded", SimplePeripheralMenu_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,5,"Auto PHY change",     SimplePeripheralMenu_doSetConnPhy)
  MENU_OBJ_END

#ifdef OAD_DEBUG
// Menu: OAD Debug
// upper: Main
  MENU_OBJ(spMenuOADDebug, "OAD Debug", 5, &spMenuMain)
  MENU_ITEM_SUBMENU(spMenuOADDebug,0,&spMenuDisplayImgInfo)
  MENU_ITEM_SUBMENU(spMenuOADDebug,1,&spMenuEraseImg)
  MENU_ITEM_ACTION(spMenuOADDebug,2,"Display EFL Meta data list", OadEflDbg_displayMetaList)
  MENU_ITEM_ACTION(spMenuOADDebug,3,"Create Factory Image",  OadEflDbg_createFactoryImg)
  MENU_ITEM_ACTION(spMenuOADDebug,4,"Erase EFL All Images",  OadEflDbg_eraseAll)
  MENU_OBJ_END

  MENU_OBJ(spMenuEraseImg, "Erase Img", 5, &spMenuOADDebug)
  MENU_ITEM_ACTION(spMenuEraseImg,0,"Factory Img 0", OadEflDbg_eraseImg)
  MENU_ITEM_ACTION(spMenuEraseImg,1,"User Img 0", OadEflDbg_eraseImg)
  MENU_ITEM_ACTION(spMenuEraseImg,2,"User Img 1", OadEflDbg_eraseImg)
  MENU_ITEM_ACTION(spMenuEraseImg,3,"User Img 2", OadEflDbg_eraseImg)
  MENU_ITEM_ACTION(spMenuEraseImg,4,"User Img 3", OadEflDbg_eraseImg)
  MENU_OBJ_END

  MENU_OBJ(spMenuDisplayImgInfo, "Display Detailed metadata", 4, &spMenuOADDebug)
  MENU_ITEM_ACTION(spMenuDisplayImgInfo,0,"Metadata 0", OadEflDbg_displayMetadata)
  MENU_ITEM_ACTION(spMenuDisplayImgInfo,1,"Metadata 1", OadEflDbg_displayMetadata)
  MENU_ITEM_ACTION(spMenuDisplayImgInfo,2,"Metadata 2", OadEflDbg_displayMetadata)
  MENU_ITEM_ACTION(spMenuDisplayImgInfo,3,"Metadata 3", OadEflDbg_displayMetadata)
  MENU_OBJ_END
#endif // OAD_DEBUG
}
#endif // ADD_SIMPLE_PERIPHERAL

/*
 * Mesh Menu callbacks
 */

// ToDo - remove externs
extern uint16_t mesh_own_addr;
extern uint16_t mesh_btn_target_addr;
extern uint16_t mesh_large_msg_target_addr;
extern uint16_t mesh_led_target_addr;
extern uint16_t mesh_gen_models_target_addr;
extern uint16_t mesh_sensor_models_target_addr;
extern uint8_t menu_init_called;
extern uint8_t btn_status;
extern uint8_t led_status;

#define GROUP_ADDR 0xc000
#define PUBLISHER_ADDR  0x000f

// Locals
static void SimpleMeshMenu_switchCb(tbmMenuObj_t* pMenuObjCurr, tbmMenuObj_t* pMenuObjNext);
static Display_Handle dispHandle;

/*********************************************************************
 * @fn      SimpleMeshMenu_init
 *
 * @brief   init mesh two-button menu
 */
 void SimpleMeshMenu_init(Display_Handle dispHandleValue)
{
  dispHandle = dispHandleValue;

  // Create the menu
  SimpleMeshMenu_build();

  // Initialize Two-Button Menu module
  TBM_SET_TITLE(&smMenuMain, "Simple Mesh and Peripheral");
  tbm_setItemStatus(&smMenuMain, TBM_ITEM_ALL, TBM_ITEM_NONE);

  tbm_initTwoBtnMenu(dispHandle, &smMenuMain, 8, SimpleMeshMenu_switchCb);
  Display_printf(dispHandle, SMN_ROW_SEPARATOR_1, 0, "======MESH==========");

  Display_printf(dispHandle, SMN_ROW_INIT_ST, 0, "INIT: bt_mesh_init called=%d, OwnAddr=%d", menu_init_called, mesh_own_addr);

  SimpleMeshMenu_turnLed(0);
  SimpleMeshMenu_showBtn(0);
  SimpleMeshMenu_turnGenOnOff(0);
  SimpleMeshMenu_setSensorLine((uint8_t *)" ", 0);
#ifdef ADD_SIMPLE_PERIPHERAL
  Display_printf(dispHandle, SP_ROW_SEPARATOR_1, 0, "======PERIPHERAL====");
#endif
  MeshApp_saveDefaultUuid();
  SimpleMeshMenu_printUuid();
}

/*********************************************************************
 * @fn      SimpleMeshMenu_switchCb
 *
 * @brief   Detect menu context switching
 *
 * @param   pMenuObjCurr - the current menu object
 * @param   pMenuObjNext - the menu object the context is about to switch to
 *
 * @return  none
 */
static void SimpleMeshMenu_switchCb(tbmMenuObj_t* pMenuObjCurr,
                                       tbmMenuObj_t* pMenuObjNext)
{
  if (pMenuObjNext == &smMenuMain)
  {
    // Now we are not in a specific connection's context

  }
  else if (pMenuObjNext == &smMenuButton)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: BTN TargetAddr=0x%x", mesh_btn_target_addr);
  }
  else if (pMenuObjNext == &smMenuLed)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: LED TargetAddr=0x%x", mesh_led_target_addr);
  }
  else if (pMenuObjNext == &smMenuVndLargeMsg)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: VND Large MSG TargetAddr=0x%x", mesh_large_msg_target_addr);
  }
  else if (pMenuObjNext == &smMenuGenOnOffModels)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: GENERIC ONOFF TargetAddr=0x%x", mesh_gen_models_target_addr);
  }
  else if (pMenuObjNext == &smMenuGenLevelModels)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: GENERIC LEVEL TargetAddr=0x%x", mesh_gen_models_target_addr);
  }
  else if (pMenuObjNext == &smMenuGenTransTimeModels)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: GENERIC TRANSMIT TIME TargetAddr=0x%x", mesh_gen_models_target_addr);
  }
  else if (pMenuObjNext == &smMenuGenPowerOnOffModels)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: GENERIC POWER ONOFF TargetAddr=0x%x", mesh_gen_models_target_addr);
  }
  else if (pMenuObjNext == &smMenuGenBatteryModels)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: GENERIC BATTERY TargetAddr=0x%x", mesh_gen_models_target_addr);
  }
  else if (pMenuObjNext == &smMenuSensorModelsClient)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: SENSOR CLIENT TargetAddr=0x%x", mesh_sensor_models_target_addr);
  }
  else if (pMenuObjNext == &smMenuSensorModelsServer)
  {
    Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: SENSOR SERVER TargetAddr=0x%x", mesh_sensor_models_target_addr);
  }
#ifdef ADD_SIMPLE_PERIPHERAL
  SimplePeripheralMenu_switchCb(pMenuObjCurr, pMenuObjNext);
#endif
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doInitMenu
 *
 * @brief   Set own address and init.
 *
 * @param   index - target addr
 *
 * @return  always true
 */
bool SimpleMeshMenu_doInitMenu(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status;
  if(index != 2)
  {
    Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doInitMenu %d", index);
  }

  if (index == 1)
  {
    if (menu_init_called != 1)
    {
      status = MeshApp_init(mesh_own_addr, UNPROVISIONED_BEACON);
      Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_init status=%d", status);
      Display_printf(dispHandle, SMN_ROW_INIT_ST, 0, "INIT: bt_mesh_init called=%d", menu_init_called);
    }
    else
    {
      Display_printf(dispHandle, SMN_ROW_INIT_ST, 0, "INIT: bt_mesh_init already initialized");
    }
  }
  else if (index == 2)
#ifdef CONFIG_BT_SETTINGS
  {
    if (menu_init_called != 1)
    {
      status = MeshApp_init(mesh_own_addr, LOAD_FROM_NV);
      Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_init status=%d", status);
      Display_printf(dispHandle, SMN_ROW_INIT_ST, 0, "INIT: bt_mesh_init called=%d", menu_init_called);
    }
    else
    {
      Display_printf(dispHandle, SMN_ROW_INIT_ST, 0, "INIT: bt_mesh_init already initialized");
    }
  }
  else if (index == 3)
#endif
  {
    //own address sub menu;
  }
  else
  {
    //do reset board on index 2 was removed;
  }
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuInit);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doBtnTargetAddr
 *
 * @brief   Set target address.
 *
 * @param   index - own address
 *
 * @return  always true
 */
bool SimpleMeshMenu_doBtnTargetAddr(uint8_t index)
{
  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doBtnTargetAddr %d", index);

  if (index == 0)
  {
    mesh_btn_target_addr = GROUP_ADDR;
  }
  else if (index >=1 && index <= 10)
  {
    mesh_btn_target_addr = index;
  }

  Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: BTN TargetAddr=%d", mesh_btn_target_addr);

  tbm_goTo(&smMenuButton);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doButton
 *
 * @brief   Button actions.
 *
 * @param   index - 0 : Set
 *                  1 : Get
 *                  2 : Status
 *
 * @return  always true
 */
bool SimpleMeshMenu_doButton(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doButton index=%d", index);
  if (index == 0) // turn LED on
  {
    status = MeshApp_button_pressed(1);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
    SimpleMeshMenu_showBtn(1);
  }
  else if (index == 1) // turn LED off
  {
    status = MeshApp_button_pressed(0);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
    SimpleMeshMenu_showBtn(0);
  }
  else if (index == 2) // Get LED status
  {
    status = MeshApp_get_status_pressed();
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
  }
  else // Set target address
  {
      status = 0;
  }
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuButton);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doLedTargetAddr
 *
 * @brief   Set target address.
 *
 * @param   index - own address
 *
 * @return  always true
 */
bool SimpleMeshMenu_doLedTargetAddr(uint8_t index)
{
  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doLedTargetAddr %d", index);

  if (index == 0)
  {
    mesh_led_target_addr = GROUP_ADDR;
  }
  else if (index >=1 && index <= 10)
  {
    mesh_led_target_addr = index;
  }

  Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: LED TargetAddr=%d", mesh_led_target_addr);

  tbm_goTo(&smMenuLed);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doLed
 *
 * @brief   Button actions.
 *
 * @param   index - 0 : GetStatus
 *                  1 : SendStatus
 *
 * @return  always true
 */
bool SimpleMeshMenu_doLed(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doLed index=%d", index);
  if (index == 0) // Notify status
  {
    status = MeshApp_notify_status_pressed(led_status);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
  }
  else if (index == 1) // Set target address
  {
  }
  else
  {
      Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: Error");
  }
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuLed);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doLargeMsgTargetAddr
 *
 * @brief   Set target address.
 *
 * @param   index - own address
 *
 * @return  always true
 */
bool SimpleMeshMenu_doLargeMsgTargetAddr(uint8_t index)
{
  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doLargeMsgTargetAddr %d", index);

  if (index == 0)
  {
    mesh_large_msg_target_addr = GROUP_ADDR;
  }
  else if (index >=1 && index <= 10)
  {
      mesh_large_msg_target_addr = index;
  }

  Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: VND Large MSG TargetAddr=%d", mesh_large_msg_target_addr);

  tbm_goTo(&smMenuVndLargeMsg);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doLargeMsg
 *
 * @brief   VND model large MSG sending.
 *
 * @param   index - 1 : 50 bytes msg
 *                  2 : 100 bytes msg
 *                  3 : 130 bytes msg
 *                  4 : 150 bytes msg
 *                  5 : 169 bytes msg
 *
 * @return  always true
 */
bool SimpleMeshMenu_doLargeMsg(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doButton index=%d", index);
  if (index == 1) // 50 bytes data
  {
    status = MeshApp_send_large_msg(50);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
  }
  else if (index == 2) // 100 bytes data
  {
    status = MeshApp_send_large_msg(100);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
    SimpleMeshMenu_showBtn(0);
  }
  else if (index == 3) // 130 bytes data
  {
    status = MeshApp_send_large_msg(130);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
  }
  else if (index == 4) // 150 bytes data
  {
    status = MeshApp_send_large_msg(150);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
  }
  else if (index == 5) // 169 bytes data
  {
    status = MeshApp_send_large_msg(169);
    Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: bt_mesh_model_send status=%d", status);
  }
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuVndLargeMsg);

  return (true);
}

void SimpleMeshMenu_turnLed(uint8_t on_off)
{
  if (on_off)
  {
    // turn on the LED
    Display_printf(dispHandle, SMN_ROW_LED, 0, "----LED ON----");
  }
  else
  {
    // turn off the LED
    Display_printf(dispHandle, SMN_ROW_LED, 0, "----LED OFF---");
  }
}

void SimpleMeshMenu_turnGenOnOff(uint8_t on_off)
{
  Display_printf(dispHandle, SMN_ROW_GEN_MODELS, 0, "----GENERIC MODELS---OnOff=%d", on_off);
}

void SimpleMeshMenu_setGenBatteryLineBytes(uint8_t level, uint32_t time_to_discharge, uint32_t time_to_charge, uint8_t flags)
{
    Display_printf(dispHandle, SMN_ROW_GEN_MODELS, 0, "----GENERIC MODELS--- Battery: 0x%x 0x%x 0x%x 0x%x",
                   level, time_to_discharge, time_to_charge, flags);
}

void SimpleMeshMenu_setSensorLine(uint8_t *string, uint16_t value)
{
  Display_printf(dispHandle, SMN_ROW_SENSOR_MODELS, 0, "----SENSOR MODELS--- %s 0x%x", string, value);
}

void SimpleMeshMenu_setSensorLineBytes(uint8_t *string, uint8_t *pBuf, int bufLen)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
    int i;
    uint8_t localBuf[32*2+2];

    for (i=0; i<bufLen && i<32; i++)
    {
        sprintf((char *)&localBuf[i*2], "%02x", pBuf[i]);
    }
    localBuf[i*2] = 0;
    Display_printf(dispHandle, SMN_ROW_SENSOR_MODELS, 0, "----SENSOR MODELS--- %s %s", string, &localBuf);
#endif
}

void SimpleMeshMenu_showBtn(uint8_t on_off)
{
  btn_status = on_off;
  if (on_off)
  {
    // turn on the LED
    Display_printf(dispHandle, SMN_ROW_BTN, 0, "----BTN ON----");
  }
  else
  {
    // turn off the LED
    Display_printf(dispHandle, SMN_ROW_BTN, 0, "----BTN OFF---");
  }
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doGenOnOffModels
 *
 * @brief   Generic OnOff models actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doGenOnOffModels(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doGenOnOffModels index=%d", index);

  if (index <= 7)
  {
      status = MeshApp_genModels_onOff_pressed(index);
  }

  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: generic onoff model bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuGenOnOffModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doGenLevelModels
 *
 * @brief   Generic level models actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doGenLevelModels(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doGenLevelModels index=%d", index);

  if (index <= 10)
  {
      status = MeshApp_genModels_level_pressed(index);
  }

  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: generic level model bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuGenLevelModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doGenPowerOnOffModels
 *
 * @brief   Generic Power OnOff models actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doGenPowerOnOffModels(uint8 index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doGenPowerOnOffModels index=%d", index);

  switch(index)
  {
      case 0: // gen_Power_OnOff_set on
          status = MeshApp_genModels_powerOnOff_pressed(1, 1);
          break;
      case 1: // gen_Power_OnOff_set off
          status = MeshApp_genModels_powerOnOff_pressed(1, 0);
          break;
      case 2: // gen_Power_OnOff_set restore
          status = MeshApp_genModels_powerOnOff_pressed(1, 2);
          break;
      case 3: // gen_Power_OnOff_get
          status = MeshApp_genModels_powerOnOff_pressed(0, 0);
          break;
      case 4: // gen_Power_OnOff_set_unack on
          status = MeshApp_genModels_powerOnOff_pressed(2, 1);
          break;
      case 5: // gen_Power_OnOff_set_unack off
          status = MeshApp_genModels_powerOnOff_pressed(2, 0);
          break;
      case 6: // gen_Power_OnOff_set_unack restore
          status = MeshApp_genModels_powerOnOff_pressed(2, 2);
          break;
      default:
          status = -1;
  }

  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: generic power onoff model bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuGenPowerOnOffModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doGenBatteryModels
 *
 * @brief   Generic Battery models actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doGenBatteryModels(uint8 index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doGenBatteryModels index=%d", index);

  if (index == 0)
  {
      status = MeshApp_genModels_battery_pressed(index);
  }

  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: generic battery model bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuGenBatteryModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doGenTransTimeModels
 *
 * @brief   General TransTime models actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doGenTransTimeModels(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doGenTransTimeModels index=%d", index);

  if (index <= 5)
  {
      status = MeshApp_genModels_transTime_pressed(index);
  }

  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: generic transition time model bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuGenTransTimeModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doGenModelsTargetAddr
 *
 * @brief   Set target address.
 *
 * @param   index - own address
 *
 * @return  always true
 */
bool SimpleMeshMenu_doGenModelsTargetAddr(uint8_t index)
{
  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doGenModelsTargetAddr %d", index);

  if (index == 0)
  {
    mesh_gen_models_target_addr = GROUP_ADDR;
  }
  else if (index >=1 && index <= 10)
  {
      mesh_gen_models_target_addr = index;
  }

  Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: Gen Models TargetAddr=%d", mesh_gen_models_target_addr);

  tbm_goTo(&smMenuGenericModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doSensorModelsClient
 *
 * @brief   Sensor models client actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doSensorModelsClient(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doSensorModelsClient index=%d", index);

  if (index <= 13)
  {
      status = MeshApp_sensorModelsClient_pressed(index, 0);
  }

  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: sensor model client bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuSensorModelsClient);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doSensorModelsServer
 *
 * @brief   Sensor models server actions.
 *
 * @param   index
 *
 * @return  always true
 */
bool SimpleMeshMenu_doSensorModelsServer(uint8_t index)
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
  int status = -1;

  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doSensorModelsServer index=%d", index);
  switch(index)
  {
      case 0: // set temperatue 36
          status = MeshApp_sensorModelsServer_pressed(0, 0);
          break;
      case 1: // set temperatue 40
          status = MeshApp_sensorModelsServer_pressed(1, 0);
          break;
      case 2: // server sensor_status - unsolicited
          status = MeshApp_sensorModelsServer_pressed(2, 0);
          break;
      default:
          status = -1;
  }
  Display_printf(dispHandle, SMN_ROW_CALL_ST, 0, "MESH: sensor model server bt_mesh_model_send status=%d", status);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */

  tbm_goTo(&smMenuSensorModelsServer);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doSensorModelsTargetAddr
 *
 * @brief   Set target address.
 *
 * @param   index - own address
 *
 * @return  always true
 */
bool SimpleMeshMenu_doSensorModelsTargetAddr(uint8_t index)
{
  Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MENU: doSensorModelsTargetAddr %d", index);

  if (index == 0)
  {
    mesh_sensor_models_target_addr = GROUP_ADDR;
  }
  else if (index >=1 && index <= 10)
  {
      mesh_sensor_models_target_addr = index;
  }

  Display_printf(dispHandle, SMN_ROW_TRGT_ST, 0, "TRGT: sensor models TargetAddr=%d", mesh_sensor_models_target_addr);

  tbm_goTo(&smMenuSensorModels);

  return (true);
}

/*********************************************************************
 * @fn      SimpleMeshMenu_printUuid
 *
 * @brief   Displays the device UUID in TBM
 *
 * @param   none
 *
 * @return  none
 */
void SimpleMeshMenu_printUuid()
{
#if !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL)
    int i;
    char uuidStr[UUID_MAX_LEN * 2 + 1];
    const uint8_t *uuid = MeshApp_getUuid();

    memset(uuidStr, 0, UUID_MAX_LEN * 2 + 1);
    for(i = 0; i < UUID_MAX_LEN; ++i)
    {
        char temp[3];
        sprintf(temp, "%02x", uuid[i]);
        strcat(uuidStr, temp);
    }
    Display_printf(dispHandle, SMN_ROW_UUID, 0, "MESH: Device UUID: 0x%s",uuidStr);
#endif /* !defined(Display_DISABLE_ALL) || (!Display_DISABLE_ALL) */
}

/*********************************************************************
 * @fn      SimpleMeshMenu_doSetUUID
 *
 * @brief   Set device UUID
 *
 * @param   index - the index of the chosen UUID
 *
 * @return  always true
 */
bool SimpleMeshMenu_doSetUUID(uint8_t index)
{
  if(menu_init_called > 0){
      Display_printf(dispHandle, SMN_ROW_MENU_ST, 0, "MESH: can't set UUID, bt_mesh_init already initialized");
      return (true);
  }

  MeshApp_setUuid(index);

  SimpleMeshMenu_printUuid(); // Replace the current displayed UUID with the new one

  tbm_goTo(&smMenuInit);

  return (true);
}

#ifdef ADD_SIMPLE_PERIPHERAL
/*
 * Menu callbacks
 */

#include <icall_ble_api.h>
#include <util.h>  

// Size of string-converted device address ("0xXXXXXXXXXXXX")
#define SP_ADDR_STR_SIZE     15

// Locals
static void SimplePeripheralMenu_switchCb(tbmMenuObj_t* pMenuObjCurr, tbmMenuObj_t* pMenuObjNext);
static Display_Handle dispHandle;


/*********************************************************************
 * @fn      SimplePeripheralMenu_switchCb
 *
 * @brief   Detect menu context switching
 *
 * @param   pMenuObjCurr - the current menu object
 * @param   pMenuObjNext - the menu object the context is about to switch to
 *
 * @return  none
 */
static void SimplePeripheralMenu_switchCb(tbmMenuObj_t* pMenuObjCurr,
                                       tbmMenuObj_t* pMenuObjNext)
{
  uint8_t NUMB_ACTIVE_CONNS;

  // interested in only the events of
  // entering scMenuConnect, spMenuSelectConn, and scMenuMain for now
  if (pMenuObjNext == &spMenuSelectConn)
  {
    static uint8_t* pAddrs;
    uint8_t* pAddrTemp;

    NUMB_ACTIVE_CONNS = linkDB_NumActive();

    if (pAddrs != NULL)
    {
      ICall_free(pAddrs);
    }

    // Allocate buffer to display addresses
    pAddrs = ICall_malloc(NUMB_ACTIVE_CONNS * SP_ADDR_STR_SIZE);

    if (pAddrs == NULL)
    {
      TBM_SET_NUM_ITEM(&spMenuSelectConn, 0);
    }
    else
    {
      uint8_t i;

      TBM_SET_NUM_ITEM(&spMenuSelectConn, MAX_NUM_BLE_CONNS);

      pAddrTemp = pAddrs;

      // Add active connection info to the menu object
      for (i = 0; i < MAX_NUM_BLE_CONNS; i++)
      {
        if (SimplePeripheral_getConnHandle(i) != LINKDB_CONNHANDLE_INVALID)
        {
          // Get the address from the connection handle
          linkDBInfo_t linkInfo;
          linkDB_GetInfo(SimplePeripheral_getConnHandle(i), &linkInfo);
          // This connection is active. Set the corresponding menu item with
          // the address of this connection and enable the item.
          memcpy(pAddrTemp, Util_convertBdAddr2Str(linkInfo.addr),
                 SP_ADDR_STR_SIZE);
          TBM_SET_ACTION_DESC(&spMenuSelectConn, i, pAddrTemp);
          tbm_setItemStatus(&spMenuSelectConn, (1 << i), SP_ITEM_NONE);
          pAddrTemp += SP_ADDR_STR_SIZE;
        }
        else
        {
          // This connection is not active. Disable the corresponding menu item.
          tbm_setItemStatus(&spMenuSelectConn, SP_ITEM_NONE, (1 << i));
        }
      }
    }
  }
  else if (pMenuObjNext == &smMenuMain)
  {
    // Now we are not in a specific connection's context

    // Clear connection-related message
    Display_clearLine(dispHandle, SP_ROW_CONNECTION);
  }
}

/*********************************************************************
 * @fn      SimplePeripheralMenu_doSelectConn
 *
 * @brief   Select a connection to communicate with
 *
 * @param   index - item index from the menu
 *
 * @return  always true
 */
bool SimplePeripheralMenu_doSelectConn(uint8_t index)
{
  SimplePeripheral_setConnIndex(index);
  // Set the menu title and go to this connection's context
  TBM_SET_TITLE(&spMenuPerConn, TBM_GET_ACTION_DESC(&spMenuSelectConn, index));

  // Clear non-connection-related message
  Display_clearLine(dispHandle, SP_ROW_CONNECTION);

  tbm_goTo(&spMenuPerConn);

  return (true);
}
/*********************************************************************
 * @fn      SimplePeripheralMenu_doAutoConnect
 *
 * @brief   Enable/Disable peripheral as AutoConnect node.
 *
 * @param   index - 0 : Disable AutoConnect
 *                  1 : Enable Group A
 *                  2 : Enable Group B
 *
 * @return  always true
 */
bool SimplePeripheralMenu_doAutoConnect(uint8_t index)
{
    if (index == 1)
    {
      SimplePeripheral_setAutoConnect(AUTOCONNECT_GROUP_A);
      Display_printf(dispHandle, SP_ROW_AC, 0, "AutoConnect enabled: Group A");
    }
    else if (index == 2)
    {
      SimplePeripheral_setAutoConnect(AUTOCONNECT_GROUP_B);
      Display_printf(dispHandle, SP_ROW_AC, 0, "AutoConnect enabled: Group B");
    }
    else
    {
      SimplePeripheral_setAutoConnect(AUTOCONNECT_DISABLE);
      Display_printf(dispHandle, SP_ROW_AC, 0, "AutoConnect disabled");
    }
    tbm_goTo(&spMenuMain);
    
    return (true);
}

/*********************************************************************
 * @fn      SimplePeripheralMenu_doSetConnPhy
 *
 * @brief   Set PHY preference.
 *
 * @param   index - 0: 1M PHY
 *                  1: 2M PHY
 *                  2: 1M + 2M PHY
 *                  3: CODED PHY (Long range)
 *                  4: 1M + 2M + CODED PHY
 *
 * @return  always true
 */
bool SimplePeripheralMenu_doSetConnPhy(uint8 index)
{
  bool status;

  status = SimplePeripheral_setConnPhy(index);
  
  return status;
}
#endif // ADD_SIMPLE_PERIPHERAL
