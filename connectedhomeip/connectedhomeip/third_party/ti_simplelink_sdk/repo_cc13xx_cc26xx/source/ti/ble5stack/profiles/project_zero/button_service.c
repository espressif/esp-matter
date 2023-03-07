/******************************************************************************

   @file  button_service.c

   @brief   This file contains the implementation of the service.

   Group: WCS, BTS
   Target Device: cc13xx_cc26xx

 ******************************************************************************
   
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

//#include <xdc/runtime/Log.h> // Comment this in to use xdc.runtime.Log
#include <ti/common/cc26xx/uartlog/UartLog.h>  // Comment out if using xdc Log

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "button_service.h"

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

// Button_Service Service UUID
CONST uint8_t ButtonServiceUUID[ATT_UUID_SIZE] =
{
    BUTTON_SERVICE_SERV_UUID_BASE128(BUTTON_SERVICE_SERV_UUID)
};

// BUTTON0 UUID
CONST uint8_t bs_BUTTON0UUID[ATT_UUID_SIZE] =
{
    BS_BUTTON0_UUID_BASE128(BS_BUTTON0_UUID)
};

// BUTTON1 UUID
CONST uint8_t bs_BUTTON1UUID[ATT_UUID_SIZE] =
{
    BS_BUTTON1_UUID_BASE128(BS_BUTTON1_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static ButtonServiceCBs_t *pAppCBs = NULL;
static uint8_t bs_icall_rsp_task_id = INVALID_TASK_ID;

/*********************************************************************
 * Profile Attributes - variables
 */

// Service declaration
static CONST gattAttrType_t ButtonServiceDecl =
{ ATT_UUID_SIZE, ButtonServiceUUID };

// Characteristic "BUTTON0" Properties (for declaration)
static uint8_t bs_BUTTON0Props = GATT_PROP_NOTIFY | GATT_PROP_READ;

// Characteristic "BUTTON0" Value variable
static uint8_t bs_BUTTON0Val[BS_BUTTON0_LEN] = {0};

// Length of data in characteristic "BUTTON0" Value variable, initialized to minimal size.
static uint16_t bs_BUTTON0ValLen = BS_BUTTON0_LEN_MIN;

// Characteristic "BUTTON0" Client Characteristic Configuration Descriptor
static gattCharCfg_t *bs_BUTTON0Config;

// Characteristic "BUTTON1" Properties (for declaration)
static uint8_t bs_BUTTON1Props = GATT_PROP_NOTIFY | GATT_PROP_READ;

// Characteristic "BUTTON1" Value variable
static uint8_t bs_BUTTON1Val[BS_BUTTON1_LEN] = {0};

// Length of data in characteristic "BUTTON1" Value variable, initialized to minimal size.
static uint16_t bs_BUTTON1ValLen = BS_BUTTON1_LEN_MIN;

// Characteristic "BUTTON1" Client Characteristic Configuration Descriptor
static gattCharCfg_t *bs_BUTTON1Config;

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t Button_ServiceAttrTbl[] =
{
    // Button_Service Service Declaration
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0,
        (uint8_t *)&ButtonServiceDecl
    },
    // BUTTON0 Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &bs_BUTTON0Props
    },
    // BUTTON0 Characteristic Value
    {
        { ATT_UUID_SIZE, bs_BUTTON0UUID },
        GATT_PERMIT_READ,
        0,
        bs_BUTTON0Val
    },
    // BUTTON0 CCCD
    {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&bs_BUTTON0Config
    },
    // BUTTON1 Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &bs_BUTTON1Props
    },
    // BUTTON1 Characteristic Value
    {
        { ATT_UUID_SIZE, bs_BUTTON1UUID },
        GATT_PERMIT_READ,
        0,
        bs_BUTTON1Val
    },
    // BUTTON1 CCCD
    {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8_t *)&bs_BUTTON1Config
    },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t Button_Service_ReadAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue,
                                           uint16_t *pLen,
                                           uint16_t offset,
                                           uint16_t maxLen,
                                           uint8_t method);
static bStatus_t Button_Service_WriteAttrCB(uint16_t connHandle,
                                            gattAttribute_t *pAttr,
                                            uint8_t *pValue,
                                            uint16_t len,
                                            uint16_t offset,
                                            uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t Button_ServiceCBs =
{
    Button_Service_ReadAttrCB, // Read callback function pointer
    Button_Service_WriteAttrCB, // Write callback function pointer
    NULL                     // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 * ButtonService_AddService- Initializes the ButtonService service by registering
 *          GATT attributes with the GATT server.
 *
 *    rspTaskId - The ICall Task Id that should receive responses for Indications.
 */
extern bStatus_t ButtonService_AddService(uint8_t rspTaskId)
{
    uint8_t status;

    // Allocate Client Characteristic Configuration table
    bs_BUTTON0Config = (gattCharCfg_t *)ICall_malloc(
        sizeof(gattCharCfg_t) * linkDBNumConns);
    if(bs_BUTTON0Config == NULL)
    {
        return(bleMemAllocError);
    }

    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, bs_BUTTON0Config);
    // Allocate Client Characteristic Configuration table
    bs_BUTTON1Config = (gattCharCfg_t *)ICall_malloc(
        sizeof(gattCharCfg_t) * linkDBNumConns);
    if(bs_BUTTON1Config == NULL)
    {
        return(bleMemAllocError);
    }

    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, bs_BUTTON1Config);
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(Button_ServiceAttrTbl,
                                         GATT_NUM_ATTRS(Button_ServiceAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &Button_ServiceCBs);
    Log_info1("Registered service, %d attributes",
              GATT_NUM_ATTRS(Button_ServiceAttrTbl));
    bs_icall_rsp_task_id = rspTaskId;

    return(status);
}

/*
 * ButtonService_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t ButtonService_RegisterAppCBs(ButtonServiceCBs_t *appCallbacks)
{
    if(appCallbacks)
    {
        pAppCBs = appCallbacks;
        Log_info1("Registered callbacks to application. Struct %p",
                  (uintptr_t)appCallbacks);
        return(SUCCESS);
    }
    else
    {
        Log_warning0("Null pointer given for app callbacks.");
        return(FAILURE);
    }
}

/*
 * ButtonService_SetParameter - Set a ButtonService parameter.
 *
 *    param - Profile parameter ID
 *    len   - length of data to write
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t ButtonService_SetParameter(uint8_t param, uint16_t len, void *value)
{
    bStatus_t ret = SUCCESS;
    uint8_t  *pAttrVal;
    uint16_t *pValLen;
    uint16_t valMinLen;
    uint16_t valMaxLen;
    uint8_t sendNotiInd = FALSE;
    gattCharCfg_t *attrConfig;
    uint8_t needAuth;

    switch(param)
    {
    case BS_BUTTON0_ID:
        pAttrVal = bs_BUTTON0Val;
        pValLen = &bs_BUTTON0ValLen;
        valMinLen = BS_BUTTON0_LEN_MIN;
        valMaxLen = BS_BUTTON0_LEN;
        sendNotiInd = TRUE;
        attrConfig = bs_BUTTON0Config;
        needAuth = FALSE;  // Change if authenticated link is required for sending.
        Log_info2("SetParameter : %s len: %d", (uintptr_t)"BUTTON0", len);
        break;

    case BS_BUTTON1_ID:
        pAttrVal = bs_BUTTON1Val;
        pValLen = &bs_BUTTON1ValLen;
        valMinLen = BS_BUTTON1_LEN_MIN;
        valMaxLen = BS_BUTTON1_LEN;
        sendNotiInd = TRUE;
        attrConfig = bs_BUTTON1Config;
        needAuth = FALSE;  // Change if authenticated link is required for sending.
        Log_info2("SetParameter : %s len: %d", (uintptr_t)"BUTTON1", len);
        break;

    default:
        Log_error1("SetParameter: Parameter #%d not valid.", param);
        return(INVALIDPARAMETER);
    }

    // Check bounds, update value and send notification or indication if possible.
    if(len <= valMaxLen && len >= valMinLen)
    {
        memcpy(pAttrVal, value, len);
        *pValLen = len; // Update length for read and get.

        if(sendNotiInd)
        {
            Log_info2("Trying to send noti/ind: connHandle %x, %s",
                      attrConfig[0].connHandle,
                      (uintptr_t)((attrConfig[0].value ==
                                   0) ? "\x1b[33mNoti/ind disabled\x1b[0m" :
                                  (attrConfig[0].value ==
                                   1) ? "Notification enabled" :
                                  "Indication enabled"));
            // Try to send notification.
            GATTServApp_ProcessCharCfg(attrConfig, pAttrVal, needAuth,
                                       Button_ServiceAttrTbl,
                                       GATT_NUM_ATTRS(
                                           Button_ServiceAttrTbl),
                                       bs_icall_rsp_task_id,
                                       Button_Service_ReadAttrCB);
        }
    }
    else
    {
        Log_error3("Length outside bounds: Len: %d MinLen: %d MaxLen: %d.", len,
                   valMinLen,
                   valMaxLen);
        ret = bleInvalidRange;
    }

    return(ret);
}

/*
 * ButtonService_GetParameter - Get a ButtonService parameter.
 *
 *    param - Profile parameter ID
 *    len   - pointer to a variable that contains the maximum length that can be written to *value.
              After the call, this value will contain the actual returned length.
 *    value - pointer to data to write.  This is dependent on
 *            the parameter ID and may be cast to the appropriate
 *            data type (example: data type of uint16_t will be cast to
 *            uint16_t pointer).
 */
bStatus_t ButtonService_GetParameter(uint8_t param, uint16_t *len, void *value)
{
    bStatus_t ret = SUCCESS;
    switch(param)
    {
    default:
        Log_error1("GetParameter: Parameter #%d not valid.", param);
        ret = INVALIDPARAMETER;
        break;
    }
    return(ret);
}

/*********************************************************************
 * @internal
 * @fn          Button_Service_findCharParamId
 *
 * @brief       Find the logical param id of an attribute in the service's attr table.
 *
 *              Works only for Characteristic Value attributes and
 *              Client Characteristic Configuration Descriptor attributes.
 *
 * @param       pAttr - pointer to attribute
 *
 * @return      uint8_t paramID (ref button_service.h) or 0xFF if not found.
 */
static uint8_t Button_Service_findCharParamId(gattAttribute_t *pAttr)
{
    // Is this a Client Characteristic Configuration Descriptor?
    if(ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID ==
       *(uint16_t *)pAttr->type.uuid)
    {
        return(Button_Service_findCharParamId(pAttr - 1)); // Assume the value attribute precedes CCCD and recurse
    }
    // Is this attribute in "BUTTON0"?
    else if(ATT_UUID_SIZE == pAttr->type.len &&
            !memcmp(pAttr->type.uuid, bs_BUTTON0UUID, pAttr->type.len))
    {
        return(BS_BUTTON0_ID);
    }
    // Is this attribute in "BUTTON1"?
    else if(ATT_UUID_SIZE == pAttr->type.len &&
            !memcmp(pAttr->type.uuid, bs_BUTTON1UUID, pAttr->type.len))
    {
        return(BS_BUTTON1_ID);
    }
    else
    {
        return(0xFF); // Not found. Return invalid.
    }
}

/*********************************************************************
 * @fn          Button_Service_ReadAttrCB
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
 * @return      SUCCESS, blePending or Failure
 */
static bStatus_t Button_Service_ReadAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen,
                                           uint16_t offset,
                                           uint16_t maxLen,
                                           uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint16_t valueLen;
    uint8_t paramID = 0xFF;

    // Find settings for the characteristic to be read.
    paramID = Button_Service_findCharParamId(pAttr);
    switch(paramID)
    {
    case BS_BUTTON0_ID:
        valueLen = bs_BUTTON0ValLen;

        Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                  (uintptr_t)"BUTTON0",
                  connHandle,
                  offset,
                  method);
        /* Other considerations for BUTTON0 can be inserted here */
        break;

    case BS_BUTTON1_ID:
        valueLen = bs_BUTTON1ValLen;

        Log_info4("ReadAttrCB : %s connHandle: %d offset: %d method: 0x%02x",
                  (uintptr_t)"BUTTON1",
                  connHandle,
                  offset,
                  method);
        /* Other considerations for BUTTON1 can be inserted here */
        break;

    default:
        Log_error0("Attribute was not found.");
        return(ATT_ERR_ATTR_NOT_FOUND);
    }
    // Check bounds and return the value
    if(offset > valueLen)   // Prevent malicious ATT ReadBlob offsets.
    {
        Log_error0("An invalid offset was requested.");
        status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
        *pLen = MIN(maxLen, valueLen - offset); // Transmit as much as possible
        memcpy(pValue, pAttr->pValue + offset, *pLen);
    }

    return(status);
}

/*********************************************************************
 * @fn      Button_Service_WriteAttrCB
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
static bStatus_t Button_Service_WriteAttrCB(uint16_t connHandle,
                                            gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len,
                                            uint16_t offset,
                                            uint8_t method)
{
    bStatus_t status = SUCCESS;
    uint8_t paramID = 0xFF;

    // See if request is regarding a Client Characterisic Configuration
    if(ATT_BT_UUID_SIZE == pAttr->type.len && GATT_CLIENT_CHAR_CFG_UUID ==
       *(uint16_t *)pAttr->type.uuid)
    {
        Log_info3("WriteAttrCB (CCCD): param: %d connHandle: %d %s",
                  Button_Service_findCharParamId(pAttr),
                  connHandle,
                  (uintptr_t)(method ==
                              GATT_LOCAL_WRITE ? "- restoring bonded state" :
                              "- OTA write"));

        // Allow notification and indication, but do not check if really allowed per CCCD.
        status = GATTServApp_ProcessCCCWriteReq(
            connHandle, pAttr, pValue, len,
            offset,
            GATT_CLIENT_CFG_NOTIFY |
            GATT_CLIENT_CFG_INDICATE);
        if(SUCCESS == status && pAppCBs && pAppCBs->pfnCfgChangeCb)
        {
            pAppCBs->pfnCfgChangeCb(connHandle,
                                    Button_Service_findCharParamId(
                                        pAttr), len, pValue);
        }

        return(status);
    }

    // Find settings for the characteristic to be written.
    paramID = Button_Service_findCharParamId(pAttr);
    switch(paramID)
    {
    default:
        Log_error0("Attribute was not found.");
        return(ATT_ERR_ATTR_NOT_FOUND);
    }
}
