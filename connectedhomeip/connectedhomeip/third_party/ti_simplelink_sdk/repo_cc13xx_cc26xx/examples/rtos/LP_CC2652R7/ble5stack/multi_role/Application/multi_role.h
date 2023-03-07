/******************************************************************************
 * @file  multi_role.h
 *
 * @description Definitions and prototypes for the multi_role example
 *
 *
 Copyright (c) 2013-2016, Texas Instruments Incorporated
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
 *
 *****************************************************************************/

#ifndef MULTIROLE_H
#define MULTIROLE_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
*  EXTERNAL VARIABLES
*/

/*********************************************************************
 * CONSTANTS
 */

// Maximum number of scan responses.
// Note: this value cannot be greater than the number of items reserved in
// mrMenuConnect (See multi_role_menu.c)
// This cannot exceed 27 (two-button menu's constraint)
#define DEFAULT_MAX_SCAN_RES                  15

// advertising PHY menu items
#define MR_ADV_LEGACY_PHY_1_MBPS    0
#define MR_ADV_EXT_PHY_1_MBPS       1
#define MR_ADV_EXT_PHY_CODED        2
  
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */
/*
 * Task creation function for the Simple Peripheral.
 */
extern void multi_role_createTask(void);

/* Action for Menu: Enable Scanning */
bool multi_role_doDiscoverDevices(uint8_t index);

/* Action for Menu: Disable Scanning */
bool multi_role_doStopDiscovering(uint8_t index);

/* Actions for Menu: Init - Connect */
bool multi_role_doConnect(uint8 index);

/* Action for Menu: Cancel Connecting */
bool multi_role_doCancelConnecting(uint8_t index);

/* Action for Menu: Select Connection */
bool multi_role_doSelectConn(uint8_t index);

bool multi_role_doAdvertise(uint8_t index);

/* Action for Menu: GATT Read */
bool multi_role_doGattRead(uint8_t index);

/* Action for Menu: GATT Write */
bool multi_role_doGattWrite(uint8_t index);

/* Actions for Menu: Init - Conn Update */
bool multi_role_doConnUpdate(uint8_t index);

/* Actions for Menu: Init - Disconnect */
bool multi_role_doDisconnect(uint8 index);

/* Actions for Menu: Init - Advertise */
bool mr_doAdvertise(uint8 index);

/* Action for Menu: Set Scanning PHY */
bool multi_role_doSetScanPhy(uint8_t index);

/* Action for Menu: Set Advertising PHY */
bool multi_role_doSetAdvPhy(uint8_t index);

/* Action for Menu: Set Initialize PHY */
bool multi_role_doSetInitPhy(uint8_t index);

/* Action for Menu: Set Connection PHY */
bool multi_role_doConnPhy(uint8_t index);


/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif /* MULTIROLE_H */
