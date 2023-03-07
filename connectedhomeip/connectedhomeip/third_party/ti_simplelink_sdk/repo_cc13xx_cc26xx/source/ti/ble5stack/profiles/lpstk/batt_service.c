/******************************************************************************

 @file  batt_service.c

 Battery GATT Service implementation for LPSTK

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
#include <ti/common/cc26xx/uartlog/UartLog.h>
#include <icall.h>
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"
#include "sensor_common.h"
#include "batt_service.h"
#include "gatt_profile_uuid.h"

/*********************************************************************
 * CONSTANTS
 */

#define BATTERY_LEVEL_FULL      100

/*********************************************************************
* GLOBAL VARIABLES
*/

// battery Service UUID
CONST uint8_t batteryUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BATT_SERV_UUID)
};
// data UUID
CONST uint8_t battery_levelUUID[ATT_UUID_SIZE] =
{
  TI_BASE_UUID_128(BATT_LEVEL_UUID)
};
/*********************************************************************
* LOCAL VARIABLES
*/

static sensorServiceCBs_t *pProfileCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t batteryDecl = { ATT_UUID_SIZE, batteryUUID };

// Characteristic "Data" Properties (for declaration)
static uint8_t battery_LevelProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "Data" Value variable
static uint8_t battery_LevelVal[BATTERY_LEVEL_LEN] = {0};
// Characteristic "Data" CCCD
static gattCharCfg_t *battery_LevelConfig;
// Characteristic User Description: data
static uint8_t battery_LevelDescr[] = "Battery Data";

/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t batteryAttrTbl[] =
{
  // battery Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&batteryDecl
  },
    // Data Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &battery_LevelProps
    },
      // Data Characteristic Value
      {
        { ATT_UUID_SIZE, battery_levelUUID },
        GATT_PERMIT_READ,
        0,
        battery_LevelVal
      },
      // Data CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&battery_LevelConfig
      },
      // Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        battery_LevelDescr
      }
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t battery_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );

static bStatus_t battery_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * SERVICE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t batteryCBs =
{
  battery_ReadAttrCB,  // Read callback function pointer
  battery_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 *  ======== Battery_AddService ========
 */
extern bStatus_t Battery_AddService()
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  battery_LevelConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * linkDBNumConns );
  if ( battery_LevelConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LINKDB_CONNHANDLE_INVALID, battery_LevelConfig );
  // Register GATT attribute list and CBs with GATT Server App
  status = GATTServApp_RegisterService( batteryAttrTbl,
                                        GATT_NUM_ATTRS( batteryAttrTbl ),
                                        GATT_MAX_ENCRYPT_KEY_SIZE,
                                        &batteryCBs );

  // Initialize non-zero values
  uint8_t level = BATTERY_LEVEL_FULL;
  memcpy(battery_LevelVal, &level, BATTERY_LEVEL_LEN);

  Log_info0("Battery Service Initialized");

  return ( status );
}

/*
 *  ======== Battery_RegisterProfileCBs ========
 */
bStatus_t Battery_RegisterProfileCBs( sensorServiceCBs_t *profileCallbacks )
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
 *  ======== Battery_SetParameter ========
 */
bStatus_t Battery_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case BATTERY_LEVEL_ID:
      if ( len == BATTERY_LEVEL_LEN )
      {
        memcpy(battery_LevelVal, value, len);

        // Try to send notification.
        ret = GATTServApp_ProcessCharCfg( battery_LevelConfig, (uint8_t *)&battery_LevelVal, FALSE,
                                    batteryAttrTbl, GATT_NUM_ATTRS( batteryAttrTbl ),
                                    INVALID_TASK_ID,  battery_ReadAttrCB);
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
 *  ======== Battery_GetParameter ========
 */
bStatus_t Battery_GetParameter( uint8_t param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
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
 * @Read an attribute
 *
 * @warning Called from BLE Stack context
 *
 * @param       connHandle - connection message was received on
 * @param       pAttr - pointer to attribute
 * @param       pValue - pointer to data to be read
 * @param       pLen - length of data to be read
 * @param       offset - offset of the first octet to be read
 * @param       maxLen - maximum length of data to be read
 * @param       method - type of read message
 *
 * @return SUCCESS
 * @return ATT_ERR_ATTR_NOT_FOUND invalid attribute
 * @return ATT_ERR_INVALID_OFFSET leng is greater than attribute length
 */
static bStatus_t battery_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the Data Characteristic Value
  if ( ! memcmp(pAttr->type.uuid, battery_levelUUID, pAttr->type.len) )
  {
    if ( offset > BATTERY_LEVEL_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, BATTERY_LEVEL_LEN - offset);  // Transmit as much as possible
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
static bStatus_t battery_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t len, uint16_t offset,
                                        uint8_t method )
{
  bStatus_t status  = SUCCESS;

  // See if request is regarding a Client Characterisic Configuration
  if ( ! memcmp(pAttr->type.uuid, clientCharCfgUUID, pAttr->type.len) )
  {
    // Allow only notifications.
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY);

    // Let the profile know that CCCD has been updated
    if ( pProfileCBs && pProfileCBs->pfnCfgChangeCb )
    {
      pProfileCBs->pfnCfgChangeCb(connHandle, BATT_SERV_UUID, BATTERY_LEVEL_ID,
                                  len, pValue);
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
