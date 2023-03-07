/******************************************************************************

 @file  ble_dispatch_lite.h

 @brief ICall BLE Dispatcher LITE Definitions.

 Group: WCS, BTS
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

#ifndef BLE_DISPATCH_LITE_H
#define BLE_DISPATCH_LITE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */
#include "sm.h"
#include "gap.h"
#include "gatt.h"
#include "gatt_uuid.h"

#include "gattservapp.h"
#include "gapbondmgr.h"

#include "icall_ble_apimsg.h"

/*********************************************************************
 * CONSTANTS
 */
extern const uint32_t bleAPItable[];

/*******************************************************************************
 * TYPEDEFS
 */


/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*
 * Task Initialization for the task
 */
extern void ble_dispatch_liteInit(uint8_t taskId);

/*
 * Task Event Processor for the task
 */
extern uint16 ble_dispatch_liteProcess(uint8_t taskId, uint16_t events);

/*
 * BLE direct API message parser
 */
extern void icall_liteMsgParser(void * msg);

/*
 * Error handler
 */
extern void icall_liteErrorFunction(void);

/*
 * Return revision of the stack
*/
extern uint8 buildRevision(ICall_BuildRevision *pBuildRev);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* !BLE_DISPATCH_LITE_H */
