/******************************************************************************

 @file  hum_service.c

 Humidity GATT Service implementation for LPSTK.

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
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "hum_service.h"
#include <ti/common/cc26xx/uartlog/UartLog.h>

/*********************************************************************
* GLOBAL VARIABLES
*/

// humidity Service UUID
CONST uint8_t humidityUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(HUMIDITY_SERV_UUID)
};
// data UUID
CONST uint8_t humidity_DataUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(HUMIDITY_DATA_UUID)
};
// configuration UUID
CONST uint8_t humidity_ConfigurationUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(HUMIDITY_CONFIGURATION_UUID)
};
// period UUID
CONST uint8_t humidity_PeriodUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(HUMIDITY_PERIOD_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static sensorServiceCBs_t *pProfileCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t humidityDecl = { ATT_UUID_SIZE, humidityUUID };

// Characteristic "Data" Properties (for declaration)
static uint8_t humidity_DataProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "Data" Value variable
static uint8_t humidity_DataVal[HUMIDITY_DATA_LEN] = {0};
// Characteristic "Data" CCCD
static gattCharCfg_t *humidity_DataConfig;
// Characteristic User Description: data
static uint8_t humidity_DataDescr[] = "Humidity Data";

// Characteristic "Configuration" Properties (for declaration)
static uint8_t humidity_ConfigurationProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Configuration" Value variable
static uint8_t humidity_ConfigurationVal[HUMIDITY_CONFIGURATION_LEN] = {0};
// Characteristic User Description: configuration
static uint8_t humidity_ConfigurationDescr[] = "Humidity Conf.";

// Characteristic "Period" Properties (for declaration)
static uint8_t humidity_PeriodProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "Period" Value variable
static uint8_t humidity_PeriodVal[HUMIDITY_PERIOD_LEN] = {0};
// Characteristic User Description: period
static uint8_t humidity_PeriodDescr[] = "Humidity Period";

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t humidityAttrTbl[] =
{
  // humidity Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&humidityDecl
  },
    // Data Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &humidity_DataProps
    },
      // Data Characteristic Value
      {
        { ATT_UUID_SIZE, humidity_DataUUID },
        GATT_PERMIT_READ,
        0,
        humidity_DataVal
      },
      // Data CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&humidity_DataConfig
      },
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        humidity_DataDescr
      },
    // Configuration Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &humidity_ConfigurationProps
    },
      // Configuration Characteristic Value
      {
        { ATT_UUID_SIZE, humidity_ConfigurationUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        humidity_ConfigurationVal
      },
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        humidity_ConfigurationDescr
      },
    // Period Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &humidity_PeriodProps
    },
      // Period Characteristic Value
      {
        { ATT_UUID_SIZE, humidity_PeriodUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        humidity_PeriodVal
      },
      // Characteristic User Description "Period"
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        humidity_PeriodDescr
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t humidity_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                      uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                      uint16_t maxLen, uint8_t method );

static bStatus_t humidity_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t len, uint16_t offset,
                                       uint8_t method );

/*********************************************************************
 * SERVICE CALLBACKS
 */

// Simple Profile Service Callbacks
CONST gattServiceCBs_t humidityCBs =
{
  humidity_ReadAttrCB,  // Read callback function pointer
  humidity_WriteAttrCB, // Write callback function pointer
  NULL                  // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 *  ======== Humidity_AddService ========
 */
extern bStatus_t Humidity_AddService()
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  humidity_DataConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( humidity_DataConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID, humidity_DataConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( humidityAttrTbl,
                                        GATT_NUM_ATTRS( humidityAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &humidityCBs );

  // Initialize non-zero values
  uint8_t period = SENSOR_DEFAULT_PERIOD / SENSOR_PERIOD_RESOLUTION;
  Humidity_SetParameter(HUMIDITY_PERIOD_ID, HUMIDITY_PERIOD_LEN, &period);

  if (status == SUCCESS)
  {
    Log_info0("Humidity Service initialized");
  }
  else
  {
    Log_error1("Humidity Service failed to enable with status 0x%x", status);
  }

  return ( status );
}

/*
 *  ======== Humidity_RegisterProfileCBs ========
 */
bStatus_t Humidity_RegisterProfileCBs( sensorServiceCBs_t *profileCallbacks )
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
 *  ======== Humidity_SetParameter ========
 */
bStatus_t Humidity_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case HUMIDITY_DATA_ID:
      if ( len == HUMIDITY_DATA_LEN )
      {
        memcpy(humidity_DataVal, value, len);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( humidity_DataConfig, (uint8_t *)&humidity_DataVal, FALSE,
                                    humidityAttrTbl, GATT_NUM_ATTRS( humidityAttrTbl ),
                                    INVALID_TASK_ID,  humidity_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case HUMIDITY_CONFIGURATION_ID:
      if ( len == HUMIDITY_CONFIGURATION_LEN )
      {
        memcpy(humidity_ConfigurationVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case HUMIDITY_PERIOD_ID:
      if ( len == HUMIDITY_PERIOD_LEN )
      {
        memcpy(humidity_PeriodVal, value, len);
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
  return ret;
}

/*
 *  ======== Humidity_GetParameter ========
 */
bStatus_t Humidity_GetParameter( uint8_t param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case HUMIDITY_CONFIGURATION_ID:
      memcpy(value, humidity_ConfigurationVal, HUMIDITY_CONFIGURATION_LEN);
      break;

    case HUMIDITY_PERIOD_ID:
      memcpy(value, humidity_PeriodVal, HUMIDITY_PERIOD_LEN);
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
static bStatus_t humidity_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the Data Characteristic Value
  if ( ! memcmp(pAttr->type.uuid, humidity_DataUUID, pAttr->type.len) )
  {
    if ( offset > HUMIDITY_DATA_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, HUMIDITY_DATA_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Configuration Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, humidity_ConfigurationUUID, pAttr->type.len) )
  {
    if ( offset > HUMIDITY_CONFIGURATION_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, HUMIDITY_CONFIGURATION_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
  // See if request is regarding the Period Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, humidity_PeriodUUID, pAttr->type.len) )
  {
    if ( offset > HUMIDITY_PERIOD_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, HUMIDITY_PERIOD_LEN - offset);  // Transmit as much as possible
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
static bStatus_t humidity_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;
  uint8_t   paramID = 0xFF;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);
  }
  // See if request is regarding the Configuration Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, humidity_ConfigurationUUID, pAttr->type.len) )
  {
    if ( offset + len > HUMIDITY_CONFIGURATION_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      if (*pValue > ST_CFG_SENSOR_ENABLE)
      {
        status = ATT_ERR_INVALID_VALUE;
      }
      else
      {
        // Copy pValue into the variable we point to from the attribute table.
        memcpy(pAttr->pValue + offset, pValue, len);

        // Only notify profile if entire expected value is written
        if ( offset + len == HUMIDITY_CONFIGURATION_LEN)
          paramID = HUMIDITY_CONFIGURATION_ID;
      }
    }
  }
  // See if request is regarding the Period Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, humidity_PeriodUUID, pAttr->type.len) )
  {
    if ( offset + len > HUMIDITY_PERIOD_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      // Copy pValue into the variable we point to from the attribute table.
      memcpy(pAttr->pValue + offset, pValue, len);

      // Only notify profile if entire expected value is written
      if ( offset + len == HUMIDITY_PERIOD_LEN)
        paramID = HUMIDITY_PERIOD_ID;
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
      pProfileCBs->pfnChangeCb(connHandle, HUMIDITY_SERV_UUID,
                               paramID, len, pValue);

  return status;
}
