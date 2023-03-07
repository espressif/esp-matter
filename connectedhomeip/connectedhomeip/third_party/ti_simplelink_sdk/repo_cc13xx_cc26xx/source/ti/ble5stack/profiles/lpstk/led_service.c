/******************************************************************************

 @file  led_service.c

 This file contains LEd Service header file.

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
#include "led_service.h"

/*********************************************************************
 * GLOBAL VARIABLES
 */

// LED_Service Service UUID
CONST uint8_t LedServiceUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(LED_SERVICE_SERV_UUID)
};
// LED0 UUID
CONST uint8_t ls_LED0UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(LS_LED0_UUID)
};
// LED1 UUID
CONST uint8_t ls_LED1UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(LS_LED1_UUID)
};
// LED2 UUID
CONST uint8_t ls_LED2UUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(LS_LED2_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static sensorServiceCBs_t *pProfileCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Service declaration
static CONST gattAttrType_t LedServiceDecl = { ATT_UUID_SIZE, LedServiceUUID };

// Characteristic "LED0" Properties (for declaration)
static uint8_t ls_LED0Props = GATT_PROP_READ | GATT_PROP_WRITE |
                              GATT_PROP_WRITE_NO_RSP;
// Characteristic "LED0" Value variable
static uint8_t ls_LED0Val[LS_LED0_LEN] = {0};
// Characteristic User Description
static uint8_t ls_led0Descr[] = "LED 0";

// Characteristic "LED1" Properties (for declaration)
static uint8_t ls_LED1Props = GATT_PROP_READ | GATT_PROP_WRITE |
                              GATT_PROP_WRITE_NO_RSP;
// Characteristic "LED1" Value variable
static uint8_t ls_LED1Val[LS_LED1_LEN] = {0};
// Characteristic User Description
static uint8_t ls_led1Descr[] = "LED 1";

// Characteristic "LED2" Properties (for declaration)
static uint8_t ls_LED2Props = GATT_PROP_READ | GATT_PROP_WRITE |
                              GATT_PROP_WRITE_NO_RSP;
// Characteristic "LED2" Value variable
static uint8_t ls_LED2Val[LS_LED2_LEN] = {0};
// Characteristic User Description
static uint8_t ls_led2Descr[] = "LED 2";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t LED_ServiceAttrTbl[] =
{
    // LED_Service Service Declaration
    {
        { ATT_BT_UUID_SIZE, primaryServiceUUID },
        GATT_PERMIT_READ,
        0,
        (uint8_t *)&LedServiceDecl
    },
    // LED0 Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &ls_LED0Props
    },
        // LED0 Characteristic Value
        {
            { ATT_UUID_SIZE, ls_LED0UUID },
            GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
            0,
            ls_LED0Val
        },
        // Characteristic User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          ls_led0Descr
        },
    // LED1 Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &ls_LED1Props
    },
        // LED1 Characteristic Value
        {
            { ATT_UUID_SIZE, ls_LED1UUID },
            GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
            0,
            ls_LED1Val
        },
        // Characteristic User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          ls_led1Descr
        },
    // LED2 Characteristic Declaration
    {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &ls_LED2Props
    },
        // LED2 Characteristic Value
        {
            { ATT_UUID_SIZE, ls_LED2UUID },
            GATT_PERMIT_READ | GATT_PERMIT_WRITE | GATT_PERMIT_WRITE,
            0,
            ls_LED2Val
        },
        // Characteristic User Description
        {
          { ATT_BT_UUID_SIZE, charUserDescUUID },
          GATT_PERMIT_READ,
          0,
          ls_led2Descr
        },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t LED_Service_ReadAttrCB(uint16_t connHandle,
                                        gattAttribute_t *pAttr,
                                        uint8_t *pValue,
                                        uint16_t *pLen,
                                        uint16_t offset,
                                        uint16_t maxLen,
                                        uint8_t method);

static bStatus_t LED_Service_WriteAttrCB(uint16_t connHandle,
                                         gattAttribute_t *pAttr,
                                         uint8_t *pValue,
                                         uint16_t len,
                                         uint16_t offset,
                                         uint8_t method);

/*********************************************************************
 * SERVICE CALLBACKS
 */

// Simple Profile Service Callbacks
CONST gattServiceCBs_t LED_ServiceCBs =
{
    LED_Service_ReadAttrCB, // Read callback function pointer
    LED_Service_WriteAttrCB, // Write callback function pointer
    NULL                     // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== LedService_AddService ========
 */
extern bStatus_t LedService_AddService()
{
    uint8_t status;

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(LED_ServiceAttrTbl,
                                         GATT_NUM_ATTRS(LED_ServiceAttrTbl),
                                         GATT_MAX_ENCRYPT_KEY_SIZE,
                                         &LED_ServiceCBs);

  if (status == SUCCESS)
  {
    Log_info0("LED Service initialized");
  }
  else
  {
    Log_error1("LED Service failed to enable with status 0x%x", status);
  }

    return(status);
}

/*
 *  ======== LedService_RegisterProfileCBs ========
 */
bStatus_t LedService_RegisterProfileCBs(sensorServiceCBs_t *profileCallbacks)
{
  if ( profileCallbacks )
  {
    pProfileCBs = profileCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*
 *  ======== LedService_SetParameter ========
 */
bStatus_t LedService_SetParameter(uint8_t param, uint16_t len, void *value)
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case LS_LED0_ID:
      memcpy(value, ls_LED0Val, LS_LED0_LEN);
      break;

    case LS_LED1_ID:
      memcpy(value, ls_LED1Val, LS_LED1_LEN);
      break;

    case LS_LED2_ID:
      memcpy(value, ls_LED2Val, LS_LED2_LEN);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}

/*
 *  ======== LedService_GetParameter ========
 */
bStatus_t LedService_GetParameter(uint8_t param, uint16_t *len, void *value)
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case LS_LED0_ID:
      memcpy(value, ls_LED0Val, LS_LED0_LEN);
      break;

    case LS_LED1_ID:
      memcpy(value, ls_LED1Val, LS_LED1_LEN);
      break;

    case LS_LED2_ID:
      memcpy(value, ls_LED2Val, LS_LED2_LEN);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
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
static bStatus_t LED_Service_ReadAttrCB(uint16_t connHandle,
                                        gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t *pLen,
                                        uint16_t offset,
                                        uint16_t maxLen,
                                        uint8_t method)
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the Data Characteristic Value
  if ( ! memcmp(pAttr->type.uuid, ls_LED0UUID, pAttr->type.len) )
  {
    if ( offset > LS_LED0_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, LS_LED0_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Configuration Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, ls_LED1UUID, pAttr->type.len) )
  {
    if ( offset > LS_LED1_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, LS_LED1_LEN - offset);  // Transmit as much as possible
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
static bStatus_t LED_Service_WriteAttrCB(uint16_t connHandle,
                                         gattAttribute_t *pAttr,
                                         uint8_t *pValue, uint16_t len,
                                         uint16_t offset,
                                         uint8_t method)
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  if ( ! memcmp(pAttr->type.uuid, ls_LED0UUID, pAttr->type.len) )
  {
    if ( offset + len > LS_LED0_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      paramID = LS_LED0_ID;
    }
  }
  // See if request is regarding the Period Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, ls_LED1UUID, pAttr->type.len) )
  {
    if ( offset + len > LS_LED1_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      paramID = LS_LED1_ID;
    }
  }
  // See if request is regarding the Period Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, ls_LED2UUID, pAttr->type.len) )
  {
    if ( offset + len > LS_LED2_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      paramID = LS_LED2_ID;
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the profile know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    // Call profile function from stack task context.
    if ( pProfileCBs && pProfileCBs->pfnChangeCb )
      pProfileCBs->pfnChangeCb(connHandle, LED_SERVICE_SERV_UUID,
                               paramID, len, pValue);

  return status;
}
