/******************************************************************************

 @file  sensor_gatt_profile.c

 @brief This file contains the 15.4 Sensor GATT profile implementation.


 Group: WCS, BTS
 Target Device: cc13x2_26x2

 ******************************************************************************
 
 Copyright (c) 2010-2020, Texas Instruments Incorporated
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

#include "ti_ble_config.h"

#include <string.h>
#include <icall.h>
#include"sensor_gatt_profile.h"
#include "util.h"
/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"


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
// Remote Display Profile Sensor Data  Characteristic Configuration Each client has its own
// instantiation of the Client Characteristic Configuration. Reads of the
// Client Characteristic Configuration only shows the configuration for
// that client and writes only affect the configuration of that client.
static gattCharCfg_t *SensorProfileTemperatureConfig;
// Sensor GATT Profile Service UUID
CONST uint8 SensorProfileServUUID[ATT_UUID_SIZE] =
{ 
  SENSORPROFILE_SERV_UUID_BASE128(SENSORPROFILE_SERV_UUID)
};


// Temperature Characteristic UUID
CONST uint8 SensorProfileTemperatureCharUUID[ATT_UUID_SIZE] =
{
  SENSORPROFILE_CHAR_UUID_BASE128(SENSORPROFILE_TEMPERATURE_CHAR_UUID)
};

// Report Interval Characteristic UUID
CONST uint8 SensorProfileReportIntervalCharUUID[ATT_UUID_SIZE] =
{
  SENSORPROFILE_CHAR_UUID_BASE128(SENSORPROFILE_REPORT_INTERVAL_CHAR_UUID)
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

static SensorProfileCBs_t *SensorProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

//  Sensor Profile Service attribute
static CONST gattAttrType_t SensorProfileService = { ATT_UUID_SIZE, SensorProfileServUUID };

// Sensor Profile Target Address Type Characteristic Properties
static uint8 SensorProfileTemperatureCharProps = GATT_PROP_READ| GATT_PROP_NOTIFY;

// Sensor Profile Target Address Characteristic Value
static uint8 SensorProfileTemperatureCharVal[2] = {0};

// Sensor Profile Target Address Characteristic User Description
static uint8 SensorProfileTemperatureUserDesp[SENSORPROFILE_MAX_DESCRIPTION_LEN] = "Temperature";

// Sensor Profile Target Address Type Characteristic Properties
static uint8 SensorProfileReportIntervalCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Sensor Profile Target Address Characteristic Value
static uint8 SensorProfileReportIntervalCharVal[4] = {0};

// Sensor Profile Target Address Characteristic User Description
static uint8 SensorProfileReportIntervalUserDesp[SENSORPROFILE_MAX_DESCRIPTION_LEN] = "Reporting Interval";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t SensorProfileAttrTbl[] =
{
  // Sensor Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&SensorProfileService     /* pValue */
  },

/*** Temperature Characteristic ***/
    // Temperature Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &SensorProfileTemperatureCharProps
    },
    // Temperature Characteristic Value
    {
      { ATT_UUID_SIZE, SensorProfileTemperatureCharUUID },
      GATT_PERMIT_READ,
      0,
      SensorProfileTemperatureCharVal
    },
    // Temperature Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      SensorProfileTemperatureUserDesp
    },
    // Sensor Data Characteristic configuration
    {
      { ATT_BT_UUID_SIZE, clientCharCfgUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      (uint8 *)&SensorProfileTemperatureConfig
    },
/*** Report Interval Characteristic ***/
    // Report Interval Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &SensorProfileReportIntervalCharProps
    },
    // Report Interval Characteristic Value
    {
      { ATT_UUID_SIZE, SensorProfileReportIntervalCharUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      SensorProfileReportIntervalCharVal
    },
    // Report Interval Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      SensorProfileReportIntervalUserDesp
    },

};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t SensorProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr, 
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t SensorProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Sensor Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t SensorProfileCBs =
{
  SensorProfile_ReadAttrCB,  // Read callback function pointer
  SensorProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      SENSORPROFILE_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t SensorProfile_AddService( uint32 services )
{
  uint8 status;
  // Allocate Client Characteristic Configuration tables
  SensorProfileTemperatureConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            MAX_NUM_BLE_CONNS );
  if ( SensorProfileTemperatureConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LL_CONNHANDLE_INVALID, SensorProfileTemperatureConfig );

  if ( services & SENSORPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( SensorProfileAttrTbl,
                                          GATT_NUM_ATTRS( SensorProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &SensorProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}

/*********************************************************************
 * @fn      SimpleProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t SensorProfile_RegisterAppCBs( SensorProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    SensorProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      SENSORPROFILE_SetParameter
 *
 * @brief   Set a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   len - length of data to write
 * @param   value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SensorProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SENSORPROFILE_TEMPERATURE_CHAR:
      if ( len == SENSORPROFILE_TEMPERATURE_LEN )
      {
        VOID memcpy(SensorProfileTemperatureCharVal, value, SENSORPROFILE_TEMPERATURE_LEN);

        GATTServApp_ProcessCharCfg( SensorProfileTemperatureConfig, SensorProfileTemperatureCharVal, FALSE,
                                    SensorProfileAttrTbl, GATT_NUM_ATTRS( SensorProfileAttrTbl ),
                                    INVALID_TASK_ID, SensorProfile_ReadAttrCB );
      }
      break;
    case SENSORPROFILE_REPORT_INTERVAL_CHAR:
      if ( len == SENSORPROFILE_REPORT_INTERVAL_LEN )
      {
        VOID memcpy(SensorProfileReportIntervalCharVal, value, SENSORPROFILE_REPORT_INTERVAL_LEN);
      }
      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn      SENSORPROFILE_GetParameter
 *
 * @brief   Get a Simple Profile parameter.
 *
 * @param   param - Profile parameter ID
 * @param   value - pointer to data to put.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate 
 *          data type (example: data type of uint16 will be cast to 
 *          uint16 pointer).
 *
 * @return  bStatus_t
 */
bStatus_t SensorProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SENSORPROFILE_TEMPERATURE_CHAR:
      VOID memcpy(value, SensorProfileTemperatureCharVal, SENSORPROFILE_TEMPERATURE_LEN);
      break;
    case SENSORPROFILE_REPORT_INTERVAL_CHAR:
      VOID memcpy(value, SensorProfileReportIntervalCharVal, SENSORPROFILE_REPORT_INTERVAL_LEN);
      break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          simpleProfile_ReadAttrCB
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
static bStatus_t SensorProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
  bStatus_t status = SUCCESS;
  
  // Make sure it's not a blob operation (no attributes in the profile are long)
  if ( offset > 0 )
  {
    return ( ATT_ERR_ATTR_NOT_LONG );
  }
 
  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
    if( !memcmp(pAttr->type.uuid, SensorProfileTemperatureCharUUID, pAttr->type.len))
    {
      *pLen = SENSORPROFILE_TEMPERATURE_LEN;
      VOID memcpy( pValue, pAttr->pValue, SENSORPROFILE_TEMPERATURE_LEN );
    }
    else if( !memcmp(pAttr->type.uuid, SensorProfileReportIntervalCharUUID, pAttr->type.len))
    {
      *pLen = SENSORPROFILE_REPORT_INTERVAL_LEN;
      VOID memcpy( pValue, pAttr->pValue, SENSORPROFILE_REPORT_INTERVAL_LEN );
    }
    else
    {
      // Should never get here! (other characteristics do not have read permissions)
      *pLen = 0;
      status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else
  {
    // 128-bit UUID
    *pLen = 0;
    status = ATT_ERR_INVALID_HANDLE;
  }

  return ( status );
}

/*********************************************************************
 * @fn      simpleProfile_WriteAttrCB
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
static bStatus_t SensorProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  if ( pAttr->type.len == ATT_UUID_SIZE )
  {

      //Do I need to have temperature rw
   if(!memcmp(pAttr->type.uuid, SensorProfileTemperatureCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != SENSORPROFILE_TEMPERATURE_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, SENSORPROFILE_TEMPERATURE_LEN );

        notifyApp = SENSORPROFILE_TEMPERATURE_CHAR;
      }
    }
   else if(!memcmp(pAttr->type.uuid, SensorProfileReportIntervalCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != SENSORPROFILE_REPORT_INTERVAL_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      // Write the value
      if ( status == SUCCESS )
      {
        VOID memcpy( pAttr->pValue, pValue, SENSORPROFILE_REPORT_INTERVAL_LEN );

        notifyApp = SENSORPROFILE_REPORT_INTERVAL_CHAR;
      }
    }
    else
    {
      // Should never get here! (characteristics 2 and 4 do not have write permissions)
      status = ATT_ERR_ATTR_NOT_FOUND;
    }
  }
  else if (( pAttr->type.len == ATT_BT_UUID_SIZE ) &&
          (BUILD_UINT16( pAttr->type.uuid[0], pAttr->type.uuid[1]) == GATT_CLIENT_CHAR_CFG_UUID))
  {
    status = GATTServApp_ProcessCCCWriteReq( connHandle, pAttr, pValue, len,
                                             offset, GATT_CLIENT_CFG_NOTIFY );
  }
  else
  {
    // unsupported length
    status = ATT_ERR_INVALID_HANDLE;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ( (notifyApp != 0xFF ) && SensorProfile_AppCBs && SensorProfile_AppCBs->pfnSensorProfileChange )
  {
    SensorProfile_AppCBs->pfnSensorProfileChange( notifyApp );
  }
  
  return ( status );
}

/*********************************************************************
*********************************************************************/
