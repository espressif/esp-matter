/******************************************************************************

 @file  ll_wl.h

 @brief This file contains the data structures and APIs for handling
        Bluetooth Low Energy White List structures using the CC26xx
        RF Core Firmware Specification.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*********************************************************************
 *
 * WARNING!!!
 *
 * THE API'S FOUND IN THIS FILE ARE FOR INTERNAL STACK USE ONLY!
 * FUNCTIONS SHOULD NOT BE CALLED DIRECTLY FROM APPLICATIONS, AND ANY
 * CALLS TO THESE FUNCTIONS FROM OUTSIDE OF THE STACK MAY RESULT IN
 * UNEXPECTED BEHAVIOR.
 *
 */

#ifndef WL_H
#define WL_H

/*******************************************************************************
 * INCLUDES
 */

#include "rf_hal.h"
#include "bcomdef.h"
#include "ll.h"
#include "ll_config.h"

/*******************************************************************************
 * MACROS
 */

// Advertising White List
// Note: Assumes wlEntryFlags = white list entry's flags.

#define CLR_WL_ENTRY( wlEntryFlags )                                           \
  (wlEntryFlags) = 0;

#define SET_WL_ENTRY_FREE( wlEntryFlags )                                      \
  (wlEntryFlags) &= ~BV(0)

#define SET_WL_ENTRY_BUSY( wlEntryFlags )                                      \
  (wlEntryFlags) |= BV(0)

#define IS_WL_ENTRY_FREE( wlEntryFlags )                                       \
  ((wlEntryFlags) & BV(0)) == 0

#define IS_WL_ENTRY_BUSY( wlEntryFlags )                                       \
  ((wlEntryFlags) & BV(0)) == 1

#define SET_WL_ENTRY_PUBLIC( wlEntryFlags )                                    \
  (wlEntryFlags) &= ~BV(1)

#define SET_WL_ENTRY_RANDOM( wlEntryFlags )                                    \
  (wlEntryFlags) |= BV(1)

#define GET_WL_ENTRY_ADDR_TYPE( wlEntryFlags )                                 \
  (((wlEntryFlags) & BV(1)) >> 1)

#define CLR_WL_ENTRY_IGNORE( wlEntryFlags )                                    \
  (wlEntryFlags) &= ~BV(2)

#define SET_WL_ENTRY_IGNORE( wlEntryFlags )                                    \
  (wlEntryFlags) |= BV(2)

#define SET_WL_ENTRY_PRIV_IGNORE( wlEntryFlags )                               \
  (wlEntryFlags) |= BV(4)

#define CLR_WL_ENTRY_PRIV_IGNORE( wlEntryFlags )                               \
  (wlEntryFlags) &= ~BV(4)

#define GET_WL_TABLE_POINTER( pWlEntry )                                       \
    ((wlTable_t *)((uint8 *)(pWlEntry) - sizeof(wlTable_t)))


/*******************************************************************************
 * CONSTANTS
 */

// API
#define BLE_IGNORE_WL_ENTRY            0
#define BLE_USE_WL_ENTRY               1

// Miscellaneous
#define BLE_BDADDR_SIZE                6
#define BLE_MAX_NUM_WL_ENTRIES         (wlSize)  // at 8 bytes per WL entry
#define BLE_NO_WL_MATCH_FOUND          0xFF

// White List Size
#define BLE_NUM_WL_ENTRIES             ((BLE_MAX_NUM_WL_ENTRIES) + (2 * (BLE_RESOLVING_LIST_SIZE)) + 1)

/*******************************************************************************
 * TYPEDEFS
 */

// BLE White List Flags
// | 7..5 |        4       |  3  |        2         |      1       |      0       |
// |  N/A | Privacy Ignore | N/A | WL Entry Ignored | Address Type | Entry In Use |
//
typedef uint8 wlFlgs_t;

// White List Entry
// Note: The layout of this structure can not be changed.
PACKED_TYPEDEF_STRUCT
{
  uint8    numEntries;                 // W:  number of white list entries
  wlFlgs_t wlFlags;                    // W:  white list flags (RW for bit 2)
  uint8    devAddr[BLE_BDADDR_SIZE];   // W:  BLE address
} wlEntry_t;

// White List Entry Table
// Note: The layout of this structure can be changed as long as pWlEntries is
//       aligned on a word boundary.
PACKED_TYPEDEF_STRUCT
{
  uint8     numWlEntries;
  uint8     numBusyWlEntries;
  uint16    reserve;
  wlEntry_t *pWlEntries;
} wlTable_t;

/*******************************************************************************
 * LOCAL VARIABLES
 */

/*******************************************************************************
 * GLOBAL VARIABLES
 */

extern wlTable_t *wlTable;
extern wlTable_t *wlTableScan;

/*******************************************************************************
 * GLOBAL ROUTINES
 */

extern void       WL_Init( wlTable_t * );

extern llStatus_t WL_Clear( wlTable_t * );

extern void       WL_ClearEntry( wlEntry_t * );

extern uint8      WL_GetSize( wlTable_t * );

extern uint8      WL_GetNumFreeEntries(  wlTable_t * );

extern uint8      WL_FindEntry( wlTable_t *, uint8 *, uint8 );

extern llStatus_t WL_AddEntry( wlTable_t *, uint8 *, uint8, uint8 );

extern llStatus_t WL_RemoveEntry( wlTable_t *, uint8 *, uint8 );

extern llStatus_t WL_SetWlIgnore( wlTable_t *, uint8 *, uint8 );

extern llStatus_t WL_ClearIgnoreList( wlTable_t * );

extern wlEntry_t *WL_Alloc( uint8 );

extern void       WL_Free( wlEntry_t * );

extern wlEntry_t *WL_Copy( wlEntry_t *, wlEntry_t * );

/*******************************************************************************
 */

#endif /* WL_H */


