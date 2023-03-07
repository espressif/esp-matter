/******************************************************************************

 @file  oad_reset_service.c

 @brief This is the TI proprietary OAD Reset Service.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2017-2022, Texas Instruments Incorporated
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

/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <icall.h>

#include "util.h"
#include "hal_board.h"

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"


#include "oad_reset_service.h"
#include "oad.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// OAD Reset GATT Profile Service UUID
static const uint8_t resetServUUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128(OAD_RESET_SERVICE_UUID)
};

static const uint8_t resetCharUUID[ATT_UUID_SIZE] =
{
    TI_BASE_UUID_128(OAD_RESET_CHAR_UUID)
};

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

static oadResetWriteCB_t oadResetWriteCB = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Simple Profile Service attribute
static CONST gattAttrType_t resetProfileService = { ATT_UUID_SIZE, resetServUUID };

// Simple Profile Characteristic 1 Properties
static uint8_t resetChar1Props = GATT_PROP_WRITE_NO_RSP | GATT_PROP_WRITE;

// Characteristic 1 Value
static uint8_t resetCharVal = 0;

// Simple Profile Characteristic 1 User Description
static uint8_t resetChar1UserDesc[] = "OAD Start/Reset";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t resetServiceAttrTbl[] =
{
    // Simple Profile Service
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
        GATT_PERMIT_READ,                         /* permissions */
        0,                                        /* handle */
        (uint8_t *)&resetProfileService            /* pValue */
    },

        // OAD Reset Characteristic Declaration
        {
            { ATT_BT_UUID_SIZE, characterUUID },
            GATT_PERMIT_READ,
            0,
            &resetChar1Props
        },

            // OAD Reset Characteristic Value
            {
                { ATT_UUID_SIZE, resetCharUUID },
                OAD_WRITE_PERMIT,
                0,
                &resetCharVal
            },

            // OAD Reset User Description
            {
                { ATT_BT_UUID_SIZE, charUserDescUUID },
                GATT_PERMIT_READ,
                0,
                resetChar1UserDesc
            }
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t ResetReadAttrCB(uint16_t connHandle,
                                 gattAttribute_t *pAttr,
                                 uint8_t *pValue, uint16_t *pLen,
                                 uint16_t offset, uint16_t maxLen,
                                 uint8_t method);

static bStatus_t ResetWriteAttrCB(uint16_t connHandle,
                                    gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// OAD Reset Service Callbacks
// Note: When an operation on a characteristic requires authorization and
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an
// operation on a characteristic requires authorization the Stack will call
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be
// made within these functions.
CONST gattServiceCBs_t resetServiceCBs =
{
    ResetReadAttrCB,  // Read callback function pointer
    ResetWriteAttrCB, // Write callback function pointer
    NULL              // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Reset_addService
 *
 * @brief   Initializes the OAD Reset service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   none
 *
 * @return  Success or Failure
 */
uint8_t Reset_addService(oadUsrAppCBs_t *pfnOadCBs)
{
    uint8_t status = OAD_SUCCESS;

    if(pfnOadCBs != NULL)
    {
        // Register a write callback function.
        oadResetWriteCB = pfnOadCBs->pfnOadWrite;

        // Register GATT attribute list and CBs with GATT Server App
        status = GATTServApp_RegisterService(resetServiceAttrTbl,
                                             GATT_NUM_ATTRS(resetServiceAttrTbl),
                                             GATT_MAX_ENCRYPT_KEY_SIZE,
                                             &resetServiceCBs);
    }
    else
    {
        status =  OAD_NO_RESOURCES;
    }

    return (status);
}

/*********************************************************************
 * @fn          ResetReadAttrCB
 *
 * @brief       Read an attribute.
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return      ATT_ERR_INVALID_HANDLE, reads are not allowed.
 */
static bStatus_t ResetReadAttrCB(uint16_t connHandle,
                                 gattAttribute_t *pAttr,
                                 uint8_t *pValue, uint16_t *pLen,
                                 uint16_t offset, uint16_t maxLen,
                                 uint8_t method)
{
    return (ATT_ERR_INVALID_HANDLE);
}

/*********************************************************************
 * @fn      ResetWriteAttrCB
 *
 * @brief   Validate attribute data prior to a write operation
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   method - type of write message
 *
 * @return  SUCCESS, blePending or Failure
 */
static bStatus_t ResetWriteAttrCB(uint16_t connHandle,
                                    gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len,
                                    uint16_t offset, uint8_t method)
{
    bStatus_t status = SUCCESS;

    //Set the BIM var to jump back to user applictaion by default
    uint32_t bim_var = 0x00000001;

    if ( pAttr->type.len == ATT_UUID_SIZE )
    {
        // 128-bit UUID
        if (!memcmp(pAttr->type.uuid, resetCharUUID, ATT_UUID_SIZE))
        {
            if(pValue[0] == OAD_RESET_CMD_START_OAD)
            {
                // Set the BIM variable to jump to persistent application
                bim_var = 0x00000001;
            }
            else
            {
                // Set BIM variable to jump back to user application
                bim_var = 0x00000101;
            }

            // Notify Application
            if (oadResetWriteCB != NULL)
            {
                (*oadResetWriteCB)(connHandle, bim_var);
            }
        }
        else
        {
            status = ATT_ERR_INVALID_HANDLE;
        }
    }

    return (status);
}

/*********************************************************************
*********************************************************************/
