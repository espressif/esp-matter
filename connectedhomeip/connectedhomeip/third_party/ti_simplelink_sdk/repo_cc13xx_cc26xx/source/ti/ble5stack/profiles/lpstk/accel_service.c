/******************************************************************************

 @file  accel_service.c

 Accelerometer GATT Service implementation for LPSTK.

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
#include <icall.h>
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "accel_service.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
 * GLOBAL VARIABLES
 */

// Accelerometer Service UUID
CONST uint8 accel_servUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(ACCEL_SERVICE_UUID)
};
// Accelerometer Enabler UUID
CONST uint8 accel_enablerUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(ACCEL_ENABLER_UUID)
};
// Accelerometer Range UUID
CONST uint8 accel_rangeUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(ACCEL_RANGE_UUID)
};
// Accelerometer X-Axis Data UUID
CONST uint8 accel_xUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(ACCEL_X_UUID)
};
// Accelerometer Y-Axis Data UUID
CONST uint8 accel_yUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(ACCEL_Y_UUID)
};
// Accelerometer Z-Axis Data UUID
CONST uint8 accel_zUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(ACCEL_Z_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static sensorServiceCBs_t *pProfileCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

// Accelerometer Service attribute
static CONST gattAttrType_t accelDecl = { ATT_UUID_SIZE, accel_servUUID };

// Enabler Characteristic Properties
static uint8 accelEnabledCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Enabler Characteristic Value
static uint8 accelEnabled = FALSE;

// Enabler Characteristic user description
static uint8 accelEnabledUserDesc[14] = "Accel Enable";

// Range Characteristic Properties
static uint8 accelRangeCharProps = GATT_PROP_READ;

// Range Characteristic Value
static uint16 accelRange = ACCEL_RANGE_2G;

// Range Characteristic user description
static uint8 accelRangeUserDesc[13] = "Accel Range";

// Accel Coordinate Characteristic Properties
static uint8 accelXCharProps = GATT_PROP_NOTIFY;
static uint8 accelYCharProps = GATT_PROP_NOTIFY;
static uint8 accelZCharProps = GATT_PROP_NOTIFY;

// Accel Coordinate Characteristics
static int16 accelXCoordinates = 0;
static int16 accelYCoordinates = 0;
static int16 accelZCoordinates = 0;

/* Client Characteristic configuration. Each client has its own instantiation
 * of the Client Characteristic Configuration. Reads of the Client Characteristic
 * Configuration only shows the configuration for that client and writes only
 * affect the configuration of that client.
 */

// Accel Coordinate Characteristic Configs
static gattCharCfg_t *accelXConfigCoordinates;
static gattCharCfg_t *accelYConfigCoordinates;
static gattCharCfg_t *accelZConfigCoordinates;

// Accel Coordinate Characteristic user descriptions
static uint8 accelXCharUserDesc[20] = "Accel X-Coordinate";
static uint8 accelYCharUserDesc[20] = "Accel Y-Coordinate";
static uint8 accelZCharUserDesc[20] = "Accel Z-Coordinate";

/*********************************************************************
 * Profile Attributes - Table
 */
static gattAttribute_t accelAttrTbl[] =
{
  // Accelerometer Service
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&accelDecl                       /* pValue */
  },

    // Accel Enabler Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &accelEnabledCharProps
    },

      // Accelerometer Enable Characteristic Value
      {
        { ATT_UUID_SIZE, accel_enablerUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        &accelEnabled
      },

      // Accelerometer Enable User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        (uint8*)&accelEnabledUserDesc
      },

    // Accel Range Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &accelRangeCharProps
    },

      // Accelerometer Range Char Value
      {
        { ATT_UUID_SIZE, accel_rangeUUID },
        GATT_PERMIT_READ,
        0,
        (uint8*)&accelRange
      },

      // Accelerometer Range User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        accelRangeUserDesc
      },

    // X-Coordinate Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &accelXCharProps
    },

      // X-Coordinate Characteristic Value
      {
        { ATT_UUID_SIZE, accel_xUUID },
        0,
        0,
        (uint8 *)&accelXCoordinates
      },

      // X-Coordinate Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&accelXConfigCoordinates
      },

      // X-Coordinate Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        accelXCharUserDesc
      },

   // Y-Coordinate Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &accelYCharProps
    },

      // Y-Coordinate Characteristic Value
      {
        { ATT_UUID_SIZE, accel_yUUID },
        0,
        0,
        (uint8 *)&accelYCoordinates
      },

      // Y-Coordinate Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&accelYConfigCoordinates
      },

      // Y-Coordinate Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        accelYCharUserDesc
      },

   // Z-Coordinate Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &accelZCharProps
    },

      // Z-Coordinate Characteristic Value
      {
        { ATT_UUID_SIZE, accel_zUUID },
        0,
        0,
        (uint8 *)&accelZCoordinates
      },

      // Z-Coordinate Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&accelZConfigCoordinates
      },

      // Z-Coordinate Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        accelZCharUserDesc
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t accel_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                  uint8_t *pValue, uint16_t *pLen,
                                  uint16_t offset, uint16_t maxLen,
                                  uint8_t method);

static bStatus_t accel_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t len,
                                   uint16_t offset, uint8_t method);

/*********************************************************************
 * SERVICE CALLBACKS
 */

/**
 * Accelerometer Service Callbacks
 *
 * Note: When an operation on a characteristic requires authorization and
 * pfnAuthorizeAttrCB is not defined for that characteristic's service, the
 * Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an
 * operation on a characteristic requires authorization the Stack will call
 * pfnAuthorizeAttrCB to check a client's authorization prior to calling
 * pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be
 * made within these functions.
 */
CONST gattServiceCBs_t  accelCBs =
{
  accel_ReadAttrCB,  // Read callback function pointer
  accel_WriteAttrCB, // Write callback function pointer
  NULL               // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*
 *  ======== Accel_AddService ========
 */
bStatus_t Accel_AddService()
{
  uint8 status = SUCCESS;
  size_t allocSize = sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS;

  // Allocate Client Characteristic Configuration tables
  accelXConfigCoordinates = (gattCharCfg_t *)ICall_malloc(allocSize);
  if ((status == SUCCESS) && (accelXConfigCoordinates == NULL))
  {
    status = bleMemAllocError;
  }

  accelYConfigCoordinates = (gattCharCfg_t *)ICall_malloc(allocSize);
  if ((status == SUCCESS) && (accelYConfigCoordinates == NULL))
  {
    // Free already allocated data
    ICall_free(accelXConfigCoordinates);

    status = bleMemAllocError;
  }

  accelZConfigCoordinates = (gattCharCfg_t *)ICall_malloc(allocSize);
  if ((status == SUCCESS) && (accelZConfigCoordinates == NULL))
  {
    // Free already allocated data
    ICall_free(accelXConfigCoordinates);
    ICall_free(accelYConfigCoordinates);

    status = bleMemAllocError;
  }

  if (status == SUCCESS)
  {
    // Initialize Client Characteristic Configuration attributes
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, accelXConfigCoordinates);
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, accelYConfigCoordinates);
    GATTServApp_InitCharCfg(LINKDB_CONNHANDLE_INVALID, accelZConfigCoordinates);

    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService(accelAttrTbl,
                                        GATT_NUM_ATTRS(accelAttrTbl),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &accelCBs);
  }

  if (status == SUCCESS)
  {
    Log_info0("Accelerometer Service initialized");
  }
  else
  {
    Log_error1("Accelerometer Service failed to enable with status 0x%x", status);
  }

  return (status);
}

/*
 *  ======== Accel_RegisterProfileCBs ========
 */
bStatus_t Accel_RegisterProfileCBs(sensorServiceCBs_t *profileCallbacks)
{
  if (profileCallbacks)
  {
    pProfileCBs = profileCallbacks;

    return (SUCCESS);
  }
  else
  {
    return (bleAlreadyInRequestedMode);
  }
}

/*
 *  ======== Accel_SetParameter ========
 */
bStatus_t Accel_SetParameter(uint8 param, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;

  switch (param)
  {
    case ACCEL_ENABLER_ID:
      if (len == ACCEL_ENBLER_LENGTH)
      {
        accelEnabled = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_RANGE_ID:
      if ((len == ACCEL_RANGE_LENGTH) && ((*((uint8*)value)) <= ACCEL_RANGE_8G))
      {
        accelRange = *((uint16*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_X_ID:
      if (len == ACCEL_X_LENGTH)
      {
        accelXCoordinates = *((int16*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg(accelXConfigCoordinates,
                                   (uint8 *)&accelXCoordinates, FALSE,
                                   accelAttrTbl, GATT_NUM_ATTRS(accelAttrTbl),
                                   INVALID_TASK_ID, accel_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_Y_ID:
      if (len == ACCEL_Y_LENGTH)
      {
        accelYCoordinates = *((int16*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg(accelYConfigCoordinates,
                                   (uint8 *)&accelYCoordinates, FALSE,
                                   accelAttrTbl, GATT_NUM_ATTRS(accelAttrTbl),
                                   INVALID_TASK_ID, accel_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case ACCEL_Z_ID:
      if (len == ACCEL_Z_LENGTH)
      {
        accelZCoordinates = *((int16*)value);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg(accelZConfigCoordinates,
                                   (uint8 *)&accelZCoordinates, FALSE,
                                   accelAttrTbl, GATT_NUM_ATTRS(accelAttrTbl),
                                   INVALID_TASK_ID, accel_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
}

/*
 *  ======== Accel_GetParameter ========
 */
bStatus_t Accel_GetParameter(uint8 param, void *value)
{
  bStatus_t ret = SUCCESS;
  switch (param)
  {
    case ACCEL_ENABLER_ID:
      *((uint8*)value) = accelEnabled;
      break;

    case ACCEL_RANGE_ID:
      *((uint16*)value) = accelRange;
      break;

    case ACCEL_X_ID:
      *((int16*)value) = accelXCoordinates;
      break;

    case ACCEL_Y_ID:
      *((int16*)value) = accelYCoordinates;
      break;

    case ACCEL_Z_ID:
      *((int16*)value) = accelZCoordinates;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
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
static bStatus_t accel_ReadAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                  uint8_t *pValue, uint16_t *pLen,
                                  uint16_t offset, uint16_t maxLen,
                                  uint8_t method)
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the Range Characteristic Value
  if ( ! memcmp(pAttr->type.uuid, accel_rangeUUID, pAttr->type.len) )
  {
    if ( offset > ACCEL_RANGE_LENGTH )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ACCEL_RANGE_LENGTH - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Enabler Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, accel_enablerUUID, pAttr->type.len) )
  {
    if ( offset > ACCEL_ENBLER_LENGTH )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ACCEL_ENBLER_LENGTH - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the X Length Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, accel_xUUID, pAttr->type.len) )
  {
    if ( offset > ACCEL_X_LENGTH )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ACCEL_X_LENGTH - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Y Length Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, accel_yUUID, pAttr->type.len) )
  {
    if ( offset > ACCEL_Y_LENGTH )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ACCEL_Y_LENGTH - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the X Length Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, accel_zUUID, pAttr->type.len) )
  {
    if ( offset > ACCEL_Z_LENGTH )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, ACCEL_Z_LENGTH - offset);  // Transmit as much as possible
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
static bStatus_t accel_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                   uint8_t *pValue, uint16_t len,
                                   uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);
  }

  // See if request is regarding the Enabler Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, accel_enablerUUID, pAttr->type.len) )
  {
    if ( offset + len > ACCEL_ENBLER_LENGTH )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else if (pValue[0] != FALSE && pValue[0] != TRUE)
    {
      status = ATT_ERR_INVALID_VALUE;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify profile if entire expected value is written
      if ( offset + len == ACCEL_ENBLER_LENGTH)
        paramID = ACCEL_ENABLER_ID;
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
      pProfileCBs->pfnChangeCb(connHandle, ACCEL_SERVICE_UUID,
                               paramID, len, pValue);

  return (status);
}
