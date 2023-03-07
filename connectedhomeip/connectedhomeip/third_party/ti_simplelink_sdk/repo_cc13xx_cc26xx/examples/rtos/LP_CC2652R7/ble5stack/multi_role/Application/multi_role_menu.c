/******************************************************************************

@file  multi_role_menu.c

@brief This file contains the multi_role menu configuration for use
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

#include "board_key.h"
/* This Header file contains all BLE API and icall structure definition */
#include <icall_ble_api.h>

#include <menu/two_btn_menu.h>
#include "multi_role_menu.h"
#include "multi_role.h"
#include "ti_ble_config.h"

tbmMenuObj_t mrMenuSelectConn;
tbmMenuObj_t mrMenuMain;
tbmMenuObj_t mrMenuConnect;
tbmMenuObj_t mrMenuPerConn;
tbmMenuObj_t mrMenuGattWrite;
tbmMenuObj_t mrMenuPhy;
tbmMenuObj_t mrMenuAdvPhy;
tbmMenuObj_t mrMenuScanPhy;
tbmMenuObj_t mrMenuInitPhy;
tbmMenuObj_t mrMenuConnPhy;

/*
 * Items List Declarations
 */

MRMenu_ItemList_t MRMenu_connPhy[MRMENU_CONNPHY_SIZE] = 
{
    {"1 Mbps", HCI_PHY_1_MBPS},
    {"2 Mbps", HCI_PHY_2_MBPS},
    {"1 & 2 Mbps", HCI_PHY_1_MBPS | HCI_PHY_2_MBPS},
    {"Coded", HCI_PHY_CODED},
    {"1 & 2 Mbps, & Coded", HCI_PHY_1_MBPS | HCI_PHY_2_MBPS | HCI_PHY_CODED},
};


MRMenu_ItemList_t MRMenu_initPhy[MRMENU_INITPHY_SIZE] =
{
    {"1 Mbps", INIT_PHY_1M},
    {"Coded", INIT_PHY_CODED},
    {"1 & Coded", INIT_PHY_1M | INIT_PHY_CODED},
    {"1 & 2 Mbps", INIT_PHY_1M | INIT_PHY_2M},
    {"Coded & 2 Mbps", INIT_PHY_CODED  | INIT_PHY_2M},
    {"1 & 2 Mbps, & Coded", INIT_PHY_1M | INIT_PHY_2M | INIT_PHY_CODED},
};

MRMenu_ItemList_t MRMenu_scanPhy[MRMENU_SCANPHY_SIZE] = 
{
    {"1 Mbps", SCAN_PRIM_PHY_1M},
    {"Coded", SCAN_PRIM_PHY_CODED},
    {"1 Mbps & Coded", SCAN_PRIM_PHY_1M | SCAN_PRIM_PHY_CODED},
};

MRMenu_ItemList_t MRMenu_advPhy[MRMENU_ADVPHY_SIZE] = 
{
    {"Legacy 1 Mbps", MR_ADV_LEGACY_PHY_1_MBPS},
    {"Extended 1 Mbps", MR_ADV_EXT_PHY_1_MBPS},
    {"Extended Coded", MR_ADV_EXT_PHY_CODED},
};

/*
 * Menu Lists Initializations
 */


void multi_role_build_menu(void)
{
// Menu: SelectDev
// upper: Main
// NOTE: The number of items in this menu object shall be
//       equal to or greater than MAX_NUM_BLE_CONNS

  MENU_OBJ(mrMenuSelectConn, "Work with", MAX_NUM_BLE_CONNS, &mrMenuMain)
  MENU_ITEM_MULTIPLE_ACTIONS(mrMenuSelectConn, MAX_NUM_BLE_CONNS, NULL, multi_role_doSelectConn)
  MENU_OBJ_END

// Menu: Main
  MENU_OBJ(mrMenuMain, "Multi-Role", 7, NULL)
  MENU_ITEM_ACTION(mrMenuMain,0,"Discover Devices", multi_role_doDiscoverDevices)
  MENU_ITEM_ACTION(mrMenuMain,1,"Stop Discovering", multi_role_doStopDiscovering)
  MENU_ITEM_SUBMENU(mrMenuMain,2,&mrMenuConnect)
  MENU_ITEM_ACTION(mrMenuMain,3,"Cancel Connecting",multi_role_doCancelConnecting)
  MENU_ITEM_SUBMENU(mrMenuMain,4,&mrMenuSelectConn)
  MENU_ITEM_ACTION(mrMenuMain,5,"Advertise", multi_role_doAdvertise)
  MENU_ITEM_SUBMENU(mrMenuMain,6,&mrMenuPhy)
  MENU_OBJ_END


// Menu: Connect
// upper: Main
// NOTE: The number of items in this menu object shall be equal to
//       or greater than DEFAULT_MAX_SCAN_RES.
//       The number of items cannot exceed 27 which is the two-button menu's
//       constraint.
  MENU_OBJ(mrMenuConnect, "Connect", DEFAULT_MAX_SCAN_RES, &mrMenuMain)
  MENU_ITEM_MULTIPLE_ACTIONS(mrMenuConnect, DEFAULT_MAX_SCAN_RES, NULL, multi_role_doConnect)
  MENU_OBJ_END

// Menu: PerConnection
// upper: SelectDevice
  MENU_OBJ(mrMenuPerConn, NULL, 5, &mrMenuSelectConn)
  MENU_ITEM_ACTION(mrMenuPerConn, 0,"GATT Read",          multi_role_doGattRead)
  MENU_ITEM_SUBMENU(mrMenuPerConn, 1,&mrMenuGattWrite)
  MENU_ITEM_ACTION(mrMenuPerConn, 2,"Connection Update",  multi_role_doConnUpdate)
  MENU_ITEM_SUBMENU(mrMenuPerConn, 3,&mrMenuConnPhy)
  MENU_ITEM_ACTION(mrMenuPerConn, 4,"Disconnect",         multi_role_doDisconnect)
  MENU_OBJ_END

// Menu: GattWrite
// upper: PerConnection
  MENU_OBJ(mrMenuGattWrite, "GATT Write", 4, &mrMenuPerConn)
  MENU_ITEM_ACTION(mrMenuGattWrite,0,"Write 0x00", multi_role_doGattWrite)
  MENU_ITEM_ACTION(mrMenuGattWrite,1,"Write 0x55", multi_role_doGattWrite)
  MENU_ITEM_ACTION(mrMenuGattWrite,2,"Write 0xAA", multi_role_doGattWrite)
  MENU_ITEM_ACTION(mrMenuGattWrite,3,"Write 0xFF", multi_role_doGattWrite)
  MENU_OBJ_END

// Menu: Phy
// upper: Main
  MENU_OBJ(mrMenuPhy, "PHY Preferences", 3, &mrMenuMain)
  MENU_ITEM_SUBMENU(mrMenuPhy,0,&mrMenuAdvPhy)
  MENU_ITEM_SUBMENU(mrMenuPhy,1,&mrMenuScanPhy)
  MENU_ITEM_SUBMENU(mrMenuPhy,2,&mrMenuInitPhy)
  MENU_OBJ_END

// Menu: AdvPhy
// upper: Phy
  MENU_OBJ(mrMenuAdvPhy, "Advertising PHY", 3, &mrMenuPhy)
  MENU_ITEM_ACTION(mrMenuAdvPhy,0,MRMenu_advPhy[0].str, multi_role_doSetAdvPhy)
  MENU_ITEM_ACTION(mrMenuAdvPhy,1,MRMenu_advPhy[1].str, multi_role_doSetAdvPhy)
  MENU_ITEM_ACTION(mrMenuAdvPhy,2,MRMenu_advPhy[2].str, multi_role_doSetAdvPhy)
  MENU_OBJ_END

// Menu: ScanPhy
// upper: Phy
  MENU_OBJ(mrMenuScanPhy, "Scanning PHY", MRMENU_SCANPHY_SIZE, &mrMenuPhy)
  MENU_ITEM_ACTION(mrMenuScanPhy,0,MRMenu_scanPhy[0].str, multi_role_doSetScanPhy)
  MENU_ITEM_ACTION(mrMenuScanPhy,1,MRMenu_scanPhy[1].str, multi_role_doSetScanPhy)
  MENU_ITEM_ACTION(mrMenuScanPhy,2,MRMenu_scanPhy[2].str, multi_role_doSetScanPhy)
  MENU_OBJ_END


// Menu: InitPhy
// upper: Phy
  MENU_OBJ(mrMenuInitPhy, "Initialize PHY", MRMENU_INITPHY_SIZE, &mrMenuPhy)
  MENU_ITEM_ACTION(mrMenuInitPhy,0,MRMenu_initPhy[0].str, multi_role_doSetInitPhy)
  MENU_ITEM_ACTION(mrMenuInitPhy,1,MRMenu_initPhy[1].str, multi_role_doSetInitPhy)
  MENU_ITEM_ACTION(mrMenuInitPhy,2,MRMenu_initPhy[2].str, multi_role_doSetInitPhy)
  MENU_ITEM_ACTION(mrMenuInitPhy,3,MRMenu_initPhy[3].str, multi_role_doSetInitPhy)
  MENU_ITEM_ACTION(mrMenuInitPhy,4,MRMenu_initPhy[4].str, multi_role_doSetInitPhy)
  MENU_ITEM_ACTION(mrMenuInitPhy,5,MRMenu_initPhy[5].str, multi_role_doSetInitPhy)
  MENU_OBJ_END

// Menu: ConnPhy
// upper: Phy
  MENU_OBJ(mrMenuConnPhy, "PHY Preference", MRMENU_CONNPHY_SIZE, &mrMenuPhy)
  MENU_ITEM_ACTION(mrMenuConnPhy,0,MRMenu_connPhy[0].str, multi_role_doConnPhy)
  MENU_ITEM_ACTION(mrMenuConnPhy,1,MRMenu_connPhy[1].str, multi_role_doConnPhy)
  MENU_ITEM_ACTION(mrMenuConnPhy,2,MRMenu_connPhy[2].str, multi_role_doConnPhy)
  MENU_ITEM_ACTION(mrMenuConnPhy,3,MRMenu_connPhy[3].str, multi_role_doConnPhy)
  MENU_ITEM_ACTION(mrMenuConnPhy,4,MRMenu_connPhy[4].str, multi_role_doConnPhy)
  MENU_OBJ_END

}
