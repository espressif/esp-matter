/******************************************************************************

@file  simple_peripheral_oad_offchip_menu.c

@brief This file contains the SBP OAD off-chip menu configuration for use
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

#include "two_btn_menu.h"
#include "simple_peripheral_oad_offchip_menu.h"
#include "simple_peripheral_oad_offchip.h"
#ifdef OAD_DEBUG
#include "oad_efl_dbg.h"
#endif

#include "ti_ble_config.h"

/* Main Menu Object */
tbmMenuObj_t spMenuMain;
tbmMenuObj_t spMenuSelectConn;
tbmMenuObj_t spMenuPerConn;
tbmMenuObj_t spMenuConnPhy;

#ifdef OAD_DEBUG
tbmMenuObj_t spMenuOADDebug;
tbmMenuObj_t spMenuEraseImg;
tbmMenuObj_t spMenuDisplayImgInfo;
#endif

/*
 * Menu Lists Initializations
 */

void SimplePeripheral_buildMenu(void)
{
/* Menu: Main
     2 submenus, no upper */
#ifdef OAD_DEBUG
  MENU_OBJ(spMenuMain, NULL, 2, NULL)
  MENU_ITEM_SUBMENU(spMenuMain,0,&spMenuOADDebug)
  MENU_ITEM_SUBMENU(spMenuMain,1,&spMenuSelectConn)
#else
  MENU_OBJ(spMenuMain, NULL, 1, NULL)
  MENU_ITEM_SUBMENU(spMenuMain,0,&spMenuSelectConn)
#endif
  MENU_OBJ_END

// Menu: SelectDev
// upper: Main
// NOTE: The number of items in this menu object shall be
//       equal to or greater than MAX_NUM_BLE_CONNS
  MENU_OBJ(spMenuSelectConn, "Work with", MAX_NUM_BLE_CONNS, &spMenuMain)
  MENU_ITEM_MULTIPLE_ACTIONS(spMenuSelectConn, MAX_NUM_BLE_CONNS, NULL, SimplePeripheral_doSelectConn)
  MENU_OBJ_END

// Menu: PerConnection
// upper: SelectDevice
  MENU_OBJ(spMenuPerConn, NULL, 1, &spMenuSelectConn)
  MENU_ITEM_SUBMENU(spMenuPerConn,0,&spMenuConnPhy)
  MENU_OBJ_END

// Menu: ConnPhy
// upper: Select Device
  MENU_OBJ(spMenuConnPhy, "Set Conn PHY Preference", 6, &spMenuPerConn)
  MENU_ITEM_ACTION(spMenuConnPhy,0,"1 Mbps",                 SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,1,"2 Mbps",                 SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,2,"1 & 2 Mbps",             SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,3,"Coded",                  SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,4,"1 & 2 Mbps, & Coded",    SimplePeripheral_doSetConnPhy)
  MENU_ITEM_ACTION(spMenuConnPhy,5,"Auto PHY change",        SimplePeripheral_doSetConnPhy)
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
#endif /* ifdef OAD_DEBUG */
}
