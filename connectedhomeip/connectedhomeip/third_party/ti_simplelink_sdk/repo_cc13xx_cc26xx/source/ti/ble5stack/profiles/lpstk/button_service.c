/******************************************************************************

 @file  button_service.c

 Button GATT Service implementation for LPSTK.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2009-2022, Texas Instruments Incorporated
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
#include <ti/common/cc26xx/uartlog/UartLog.h>  // Comment out if using xdc Log
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "button_service.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Button_Service Service UUID
CONST uint8_t ButtonServiceUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BUTTON_SERVICE_SERV_UUID)
};
// BUTTON0 UUID
CONST uint8_t bs_BUTTON0UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BS_BUTTON0_UUID)
};
// BUTTON1 UUID
CONST uint8_t bs_BUTTON1UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BS_BUTTON1_UUID)
};

/*********************************************************************
 * Profile Attributes - variables
 */

// Service declaration
static CONST gattAttrType_t ButtonServiceDecl = { ATT_UUID_SIZE, ButtonServiceUUID };

// Characteristic "BUTTON0" Properties (for declaration)
static uint8_t bs_BUTTON0Props = GATT_PROP_NOTIFY | GATT_PROP_READ;
// Characteristic "BUTTON0" Value variable
static uint8_t bs_BUTTON0Val[BS_BUTTON0_LEN] = {0};
// Characteristic "BUTTON0" Client Characteristic Configuration Descriptor
static gattCharCfg_t *bs_BUTTON0Config;
// Characteristic User Description
static uint8_t bs_button0Descr[] = "Button 0";

// Characteristic "BUTTON1" Properties (for declaration)
static uint8_t bs_BUTTON1Props = GATT_PROP_NOTIFY | GATT_PROP_READ;
// Characteristic "BUTTON1" Value variable
static uint8_t bs_BUTTON1Val[BS_BUTTON1_LEN] = {0};
// Characteristic "BUTTON1" Client Characteristic Configuration Descriptor
static gattCharCfg_t *bs_BUTTON1Config;
// Characteristic User Description
static uint8_t bs_button1Descr[] = "Button 1";

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
        // Characteristic User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          bs_button0Descr
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
        // Characteristic User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          bs_button1Descr
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
 * SERVICE CALLBACKS
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
 *  ======== ButtonService_AddService ========
 */
extern bStatus_t ButtonService_AddService()
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

  if (status == SUCCESS)
  {
    Log_info0("Button Service initialized");
  }
  else
  {
    Log_error1("Button Service failed to enable with status 0x%x", status);
  }

    return(status);
}

/*
 *  ======== ButtonService_SetParameter ========
 */
bStatus_t ButtonService_SetParameter(uint8_t param, uint16_t len, void *value)
{
    bStatus_t ret = SUCCESS;

    switch(param)
    {
    case BS_BUTTON0_ID:
      if ( len == BS_BUTTON0_LEN )
      {
        memcpy(bs_BUTTON0Val, value, len);

        // Try to send notification.
        ret = GATTServApp_ProcessCharCfg( bs_BUTTON0Config, (uint8_t *)&bs_BUTTON0Val, FALSE,
                                    Button_ServiceAttrTbl, GATT_NUM_ATTRS( Button_ServiceAttrTbl ),
                                    INVALID_TASK_ID,  Button_Service_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case BS_BUTTON1_ID:
      if ( len == BS_BUTTON1_LEN )
      {
        memcpy(bs_BUTTON1Val, value, len);

        // Try to send notification.
        ret = GATTServApp_ProcessCharCfg( bs_BUTTON1Config, (uint8_t *)&bs_BUTTON1Val, FALSE,
                                    Button_ServiceAttrTbl, GATT_NUM_ATTRS( Button_ServiceAttrTbl ),
                                    INVALID_TASK_ID,  Button_Service_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
        return(INVALIDPARAMETER);
    }

    return(ret);
}

/*********************************************************************
 * PRIVATE FUNCTIONS
 */

/**
 * Read an attribute.
 *
 * @warning Called from BLE Stack context
 *
 * @param connHandle - connection message was received on
 * @param pAttr      - pointer to attribute
 * @param pValue     - pointer to data to be read
 * @param pLen       - length of data to be read
 * @param offset     - offset of the first octet to be read
 * @param maxLen     - maximum length of data to be read
 * @param method     - type of read message
 *
 * @return SUCCESS
 * @return ATT_ERR_ATTR_NOT_FOUND invalid attribute
 * @return ATT_ERR_INVALID_OFFSET leng is greater than attribute length
 */
static bStatus_t Button_Service_ReadAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen,
                                           uint16_t offset,
                                           uint16_t maxLen,
                                           uint8_t method)
{
  bStatus_t status = SUCCESS;

  // See if request is regarding button 0
  if ( ! memcmp(pAttr->type.uuid, bs_BUTTON0UUID, pAttr->type.len) )
  {
    if ( offset > BS_BUTTON0_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BS_BUTTON0_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding button 1
  else if ( ! memcmp(pAttr->type.uuid, bs_BUTTON1UUID, pAttr->type.len) )
  {
    if ( offset > BS_BUTTON1_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BS_BUTTON1_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has READ permissions.
    *pLen = 0;
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}

/**
 * Validate attribute data prior to a write operation
 *
 * @warning Called from BLE Stack context
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr      - pointer to attribute
 * @param   pValue     - pointer to data to be written
 * @param   len        - length of data
 * @param   offset     - offset of the first octet to be written
 * @param   method     - type of write message
 *
 * @return SUCCESS
 * @return ATT_ERR_INVALID_OFFSET len is greater than attribute length
 * @return ATT_ERR_INVALID_VALUE attribute does not allow this value
 * @return ATT_ERR_ATTR_NOT_FOUND there is no write clause implemented for
 *         this attribute
 */
static bStatus_t Button_Service_WriteAttrCB(uint16_t connHandle,
                                            gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len,
                                            uint16_t offset,
                                            uint8_t method)
{
  bStatus_t status  = SUCCESS;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);

    if (status == SUCCESS)
    {
      Log_info0("Button Service notifications enabled");
    }
    else
    {
      Log_error1("Button Service notifications failed to enable with status 0x%x", status);
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  return status;
}
