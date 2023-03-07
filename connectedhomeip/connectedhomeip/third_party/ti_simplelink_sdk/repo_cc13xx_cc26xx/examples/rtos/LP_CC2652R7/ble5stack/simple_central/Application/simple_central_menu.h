/******************************************************************************

 @file  simple_central_menu.h

 @brief This file contains menu objects for simple_central.

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

#ifndef SIMPLE_CENTRAL_MENU_H
#define SIMPLE_CENTRAL_MENU_H

// Duplicate menu item indices.
#define SC_ITEM_NONE        TBM_ITEM_NONE
#define SC_ITEM_ALL         TBM_ITEM_ALL

// Note: The defines should be updated accordingly if there is any change
//       in the order of the items of the menu objects the items belong to.
#define SC_ITEM_SCANPHY     TBM_ITEM(0)  // "Set Scanning PHY"
#define SC_ITEM_STARTDISC   TBM_ITEM(1)  // "Discover Devices"
#define SC_ITEM_STOPDISC    TBM_ITEM(2)  // "Stop Discovering"
#define SC_ITEM_CONNECT     TBM_ITEM(3)  // "Connect To"
#define SC_ITEM_CANCELCONN  TBM_ITEM(4)  // "Cancel Connecting"
#define SC_ITEM_SELECTCONN  TBM_ITEM(5)  // "Work With"
#define SC_ITEM_AUTOCONNECT TBM_ITEM(6)  // "Auto Connect"

#define SC_ITEM_GATTREAD    TBM_ITEM(0)  // "GATT Read"
#define SC_ITEM_GATTWRITE   TBM_ITEM(1)  // "GATT Write"
#define SC_ITEM_STRTRSSI    TBM_ITEM(2)  // "Start RSSI Reading"
#define SC_ITEM_STOPRSSI    TBM_ITEM(3)  // "Stop RSSI Reading"
#define SC_ITEM_UPDATE      TBM_ITEM(4)  // "Connection Update"
#define SC_ITEM_CONNPHY     TBM_ITEM(5)  // "Set Connection PHY"
#define SC_ITEM_DISCONNECT  TBM_ITEM(6)  // "Disconnect"

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Menus Declarations
 */

/* Main Menu Object */
extern tbmMenuObj_t scMenuMain;
extern tbmMenuObj_t scMenuConnect;
extern tbmMenuObj_t scMenuScanPhy;
extern tbmMenuObj_t scMenuAutoConnect;
extern tbmMenuObj_t scMenuConnPhy;
extern tbmMenuObj_t scMenuSelectConn;
extern tbmMenuObj_t scMenuPerConn;
extern tbmMenuObj_t scMenuGattWrite;

void SimpleCentral_buildMenu(void);

/* Items of (Main) */
/* Action items are defined in simple_central_menu.c */

#ifdef __cplusplus
}
#endif

#endif /* SIMPLE_CENTRAL_MENU_H */

