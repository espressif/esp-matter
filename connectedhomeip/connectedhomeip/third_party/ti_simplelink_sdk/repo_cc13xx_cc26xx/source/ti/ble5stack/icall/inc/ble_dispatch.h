/******************************************************************************

 @file  ble_dispatch.h

 @brief ICall BLE Dispatcher Definitions.

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

#ifndef ICALLBLE_H
#define ICALLBLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */
// Dispatch Subgroup IDs (0x00-0x80)
#define DISPATCH_GENERAL                      0x00 // General
#define DISPATCH_GAP_PROFILE                  0x01 // GAP Profile
#define DISPATCH_GATT_PROFILE                 0x02 // GATT Profile
#define DISPATCH_GAP_GATT_SERV                0x03 // GAP GATT Server (GGS)
#define DISPATCH_GATT_SERV_APP                0x04 // GATT Server App (GSA)

// Common Command IDs reserved for each profile (0x00-0x0F)
#define DISPATCH_PROFILE_ADD_SERVICE          0x00 // Add service
#define DISPATCH_PROFILE_DEL_SERVICE          0x01 // Delete service
#define DISPATCH_PROFILE_REG_SERVICE          0x02 // Register service
#define DISPATCH_PROFILE_DEREG_SERVICE        0x03 // Deregister service
#define DISPATCH_PROFILE_REG_CB               0x04 // Register app callbacks
#define DISPATCH_PROFILE_GET_PARAM            0x05 // Get Parameter
#define DISPATCH_PROFILE_SET_PARAM            0x06 // Set Parameter
#define DISPATCH_PROFILE_DELAYED_RSP          0x07 // Send Delayed Read Response message

// GAP Profile Command IDs (0x10-0xFF)
#define DISPATCH_GAP_REG_FOR_MSG              0x10 // Register for HCI messages
#define DISPATCH_GAP_LINKDB_STATE             0x11 // Link DB State
#define DISPATCH_GAP_LINKDB_NUM_CONNS         0x12 // Link DB Num Conns
#define DISPATCH_GAP_BOND_PASSCODE_RSP        0x13 // Bond Mgr Passcode Response
#define DISPATCH_GAP_BOND_LINK_EST            0x14 // Bond Mgr Link Established
#define DISPATCH_GAP_BOND_LINK_TERM           0x15 // Bond Mgr Link Terminated
#define DISPATCH_GAP_BOND_SLAVE_REQ_SEC       0x16 // Bond Mgr Slave Requested Security
#define DISPATCH_GAP_BOND_FIND_ADDR           0x17 // Bond Mgr Resolve Address
#define DISPATCH_GAP_LINKDB_NUM_ACTIVE        0x18 // Link DB Num Active
#define DISPATCH_GAP_LINKDB_GET_INFO          0x19 // Link DB Get Info

// GATT Profile Command IDs (0x10-0xFF)
#define DISPATCH_GATT_REG_FOR_MSG             0x10 // Register for GATT events/messages
#define DISPATCH_GATT_INIT_CLIENT             0x11 // Initialize GATT Client
#define DISPATCH_GATT_REG_4_IND               0x12 // Register for Indication/Notification
#define DISPATCH_GATT_HTA_FLOW_CTRL           0x13 // Host To App Flow Control
#define DISPATCH_GATT_APP_COMPL_MSG           0x14 // App Completed Message
#define DISPATCH_GATT_SEND_RSP                0x15 // GATT Send Response message

// GATT Server App (GSA) Command IDs (0x10-0xFF)
#define DISPATCH_GSA_ADD_QUAL_SERVICE         0x10 // GSA Add Qualification Services
#define DISPATCH_GSA_ADD_TEST_SERVICE         0x11 // GSA Add Test Services
#define DISPATCH_GSA_SERVICE_CHANGE_IND       0x12 // GSA Service Change Indication

// ICall Dispatcher General Command IDs (0x10-0xFF)
#define DISPATCH_GENERAL_REG_NPI              0x10 // Register NPI task with stack
#define DISPATCH_GENERAL_REG_L2CAP_FC         0x11 // Register Task with L2CAP to receive Flow Control Events

/*** Build Revision Command ***/

// Stack Info field bitmaps
#define BLDREV_STK_IAR_PROJ                   0x01 // IAR used to build stack project
#define BLDREV_STK_CCS_PROJ                   0x02 // CCS used to build stack project
#define BLDREV_STK_IAR_LIB                    0x10 // IAR used to build stack library
#define BLDREV_STK_ROM_BLD                    0x80 // ROM build

// Controller Info field bitmaps
#define BLDREV_CTRL_PING_CFG                  0x10 // Ping included
#define BLDREV_CTRL_SLV_FEAT_EXCHG_CFG        0x20 // Slave Feature Exchange included
#define BLDREV_CTRL_CONN_PARAM_REQ_CFG        0x40 // Connection Parameter Request included

// Host Info field bitmaps
#define BLDREV_HOST_GAP_BOND_MGR              0x10 // GAP Bond Manager included
#define BLDREV_HOST_L2CAP_CO_CHANNELS         0x20 // L2CAP CO Channels included

// BM Message Types by layer/module
#define BM_MSG_GATT                           1 // GATT layer
#define BM_MSG_L2CAP                          2 // L2CAP layer
#define BM_MSG_GENERIC                        3 // Lowest layer

/*******************************************************************************
 * TYPEDEFS
 */

// BM allocator and de-allocator function pointer types
typedef void* (*pfnBMAlloc_t)(uint8_t type, uint16_t size, uint16_t connHandle,
                              uint8_t opcode, uint16_t *pSizeAlloc);
typedef void  (*pfnBMFree_t)(uint8_t type, void *pMsg, uint8_t opcode);

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the task
 */
extern void bleDispatch_Init(uint8_t task_id);

/*
 * Task Event Processor for the task
 */
extern uint16_t bleDispatch_ProcessEvent(uint8_t task_id, uint16_t events);

/*
 * Implementation of the BM allocator functionality.
 */
extern void *bleDispatch_BMAlloc(uint8_t type, uint16_t size,
                                 uint16_t connHandle, uint8_t opcode,
                                 uint16_t *pSizeAlloc);
/*
 * Implementation of the BM de-allocator functionality.
 */
extern void bleDispatch_BMFree(uint8_t type, void *pBuf, uint8_t opcode);

/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* ICALLBLE_H */
