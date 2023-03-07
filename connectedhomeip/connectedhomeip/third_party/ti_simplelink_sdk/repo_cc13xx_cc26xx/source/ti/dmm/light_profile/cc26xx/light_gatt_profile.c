/******************************************************************************

 @file  light_gatt_profile.c

 @brief This file contains the 15.4 Light GATT profile implementation.


 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2010-2021, Texas Instruments Incorporated
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

#include "light_gatt_profile.h"

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
// Light GATT Profile Service UUID
CONST uint8 lightProfileServUUID[ATT_UUID_SIZE] =
{ 
  LIGHTPROFILE_SERV_UUID_BASE128(LIGHTPROFILE_SERV_UUID)
};

// Light On/Off Characteristic UUID
CONST uint8 lightProfileLightOnOffCharUUID[ATT_UUID_SIZE] =
{
  LIGHTPROFILE_CHAR_UUID_BASE128(LIGHTPROFILE_LIGHT_ONOFF_CHAR_UUID)
};

// Target Address Type Characteristic UUID
CONST uint8 lightProfileTargetAddrTypeCharUUID[ATT_UUID_SIZE] =
{
  LIGHTPROFILE_CHAR_UUID_BASE128(LIGHTPROFILE_TARGET_ADDR_TYPE_CHAR_UUID)
};

// Target Address Characteristic UUID
CONST uint8 lightProfileTargetAddrCharUUID[ATT_UUID_SIZE] =
{
  LIGHTPROFILE_CHAR_UUID_BASE128(LIGHTPROFILE_TARGET_ADDR_CHAR_UUID)
};

// Battery Level Characteristic UUID
CONST uint8 lightProfileBatteryLevelCharUUID[ATT_UUID_SIZE] =
{
  LIGHTPROFILE_CHAR_UUID_BASE128(LIGHTPROFILE_BAT_LEVEL_CHAR_UUID)
};

// Target Endpoint Level Characteristic UUID
CONST uint8 lightProfileTargetEndpointCharUUID[ATT_UUID_SIZE] =
{
  LIGHTPROFILE_CHAR_UUID_BASE128(LIGHTPROFILE_TARGET_ENDPOINT_CHAR_UUID)
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

static lightProfileCBs_t *lightProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

//  Light Profile Service attribute
static CONST gattAttrType_t lightProfileService = { ATT_UUID_SIZE, lightProfileServUUID };

// Light Profile Light On/Off Characteristic Properties
static uint8 lightProfileLightOnOffCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Light On/Off Characteristic Value
static uint8 lightProfileLightOnOffCharVal = 0;

// Light Profile Light On/Off Characteristic User Description
static uint8 lightProfileLightOnOffUserDesp[LIGHTPROFILE_MAX_DESCRIPTION_LEN] = "Light On/Off";

// Light Profile Target Address Type Characteristic Properties
static uint8 lightProfileTargetAddrTypeCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Light Profile Target Address Type Characteristic Value
static uint8 lightProfileTargetAddrTypeCharVal = 0;

// Light Profile Target Address Type Characteristic User Description
static uint8 lightProfileTargetAddrTypeUserDesp[LIGHTPROFILE_MAX_DESCRIPTION_LEN] = "Target Address Type";

// Light Profile Target Address Type Characteristic Properties
static uint8 lightProfileTargetAddrCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Light Profile Target Address Characteristic Value
static uint8 lightProfileTargetAddrCharVal[2] = {0};

// Light Profile Target Address Characteristic User Description
static uint8 lightProfileTargetAddrUserDesp[LIGHTPROFILE_MAX_DESCRIPTION_LEN] = "Target Address";

// Light Profile Battery Level Characteristic Properties
static uint8 lightProfileBatteryLevelCharProps = GATT_PROP_READ;

// Light Profile Battery Level  Characteristic Value
static uint8 lightProfileBatteryLevelCharVal = 0;

// Light Profile Battery Level Type Characteristic User Description
static uint8 lightProfileBatteryLevelUserDesc[LIGHTPROFILE_MAX_DESCRIPTION_LEN] = "Device Battery Level";

// Light Profile Target Endpoint Characteristic Properties
static uint8 lightProfileTargetEndpointCharProps = GATT_PROP_READ | GATT_PROP_WRITE;

// Light Profile Target Endpoint Characteristic Value
static uint8 lightProfileTargetEndpointCharVal = 0;

// Light Profile Target Endpoint Characteristic User Description
static uint8 lightProfileTargetEndpointUserDesc[LIGHTPROFILE_MAX_DESCRIPTION_LEN] = "Target Endpoint";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t lightProfileAttrTbl[] =
{
  // Light Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&lightProfileService     /* pValue */
  },
/*** Light On/Off Characteristic ***/
    // Light On/Off Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &lightProfileLightOnOffCharProps
    },
    // Light On/Off Characteristic Value
    {
      { ATT_UUID_SIZE, lightProfileLightOnOffCharUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      &lightProfileLightOnOffCharVal
    },
    // Light On/Off Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      lightProfileLightOnOffUserDesp
    },
/*** Target Address Type Characteristic ***/
    // Target Address Type Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &lightProfileTargetAddrTypeCharProps
    },
    // Target Address Type Characteristic Value
    {
      { ATT_UUID_SIZE, lightProfileTargetAddrTypeCharUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      &lightProfileTargetAddrTypeCharVal
    },
    // Target Address Type Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      lightProfileTargetAddrTypeUserDesp
    },
/*** Target Address Characteristic ***/
    // Target Address Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &lightProfileTargetAddrCharProps
    },
    // Target Address Characteristic Value
    {
      { ATT_UUID_SIZE, lightProfileTargetAddrCharUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      lightProfileTargetAddrCharVal
    },
    // Target Address Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      lightProfileTargetAddrUserDesp
    },
/*** Battery Level Characteristic ***/
    // Target Address Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &lightProfileBatteryLevelCharProps
    },
    // Battery Level Characteristic Value
    {
      { ATT_UUID_SIZE, lightProfileBatteryLevelCharUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      &lightProfileBatteryLevelCharVal
    },
    // Battery Level Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      lightProfileBatteryLevelUserDesc
    },
/*** Target Endpoint Characteristic ***/
    // Target Endpoint Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &lightProfileTargetEndpointCharProps
    },
    // Target Endpoint Characteristic Value
    {
      { ATT_UUID_SIZE, lightProfileTargetEndpointCharUUID },
      GATT_PERMIT_READ | GATT_PERMIT_WRITE,
      0,
      &lightProfileTargetEndpointCharVal
    },
    // Target Endpoint Characteristic User Description
    {
      { ATT_BT_UUID_SIZE, charUserDescUUID },
      GATT_PERMIT_READ,
      0,
      lightProfileTargetEndpointUserDesc
    },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t lightProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr, 
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t lightProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Light Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t lightProfileCBs =
{
  lightProfile_ReadAttrCB,  // Read callback function pointer
  lightProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      LightProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t LightProfile_AddService( uint32 services )
{
  uint8 status;

  if ( services & LIGHTPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( lightProfileAttrTbl, 
                                          GATT_NUM_ATTRS( lightProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &lightProfileCBs );
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
bStatus_t LightProfile_RegisterAppCBs( lightProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    lightProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      LightProfile_SetParameter
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
bStatus_t LightProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case LIGHTPROFILE_LIGHT_ONOFF_CHAR:
      if ( len == LIGHTPROFILE_LIGHT_ONOFF_LEN )
      {
        lightProfileLightOnOffCharVal = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    case LIGHTPROFILE_TARGET_ADDR_TYPE_CHAR:
      if ( len == LIGHTPROFILE_TARGET_ADDR_TYPE_LEN )
      {
        lightProfileTargetAddrTypeCharVal = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    case LIGHTPROFILE_TARGET_ADDR_CHAR:
      if ( len == LIGHTPROFILE_TARGET_ADDR_LEN )
      {
        VOID memcpy(lightProfileTargetAddrCharVal, value, LIGHTPROFILE_TARGET_ADDR_LEN);
      }
    case LIGHTPROFILE_BAT_LEVEL_CHAR:
      if ( len == LIGHTPROFILE_BAT_LEVEL_LEN )
      {
          lightProfileBatteryLevelCharVal = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
    case LIGHTPROFILE_TARGET_ENDPOINT_CHAR:
      if ( len == LIGHTPROFILE_TARGET_ENDPOINT_LEN )
      {
          lightProfileTargetEndpointCharVal = *((uint8*)value);
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
  
  return ( ret );
}

/*********************************************************************
 * @fn      LightProfile_GetParameter
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
bStatus_t LightProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case LIGHTPROFILE_LIGHT_ONOFF_CHAR:
      *((uint8*)value) = lightProfileLightOnOffCharVal;
      break;
    case LIGHTPROFILE_TARGET_ADDR_TYPE_CHAR:
      *((uint8*)value) = lightProfileTargetAddrTypeCharVal;
      break;
    case LIGHTPROFILE_TARGET_ADDR_CHAR:
      VOID memcpy(value, lightProfileTargetAddrCharVal, LIGHTPROFILE_TARGET_ADDR_LEN);
      break;
    case LIGHTPROFILE_BAT_LEVEL_CHAR:
      *((uint8*)value) = lightProfileBatteryLevelCharVal;
      break;
    case LIGHTPROFILE_TARGET_ENDPOINT_CHAR:
      *((uint8*)value) = lightProfileTargetEndpointCharVal;
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
static bStatus_t lightProfile_ReadAttrCB(uint16_t connHandle,
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
    if( (!memcmp(pAttr->type.uuid, lightProfileLightOnOffCharUUID, pAttr->type.len)) ||
        (!memcmp(pAttr->type.uuid, lightProfileTargetAddrTypeCharUUID, pAttr->type.len)) ||
        (!memcmp(pAttr->type.uuid, lightProfileBatteryLevelCharUUID, pAttr->type.len)) ||
        (!memcmp(pAttr->type.uuid, lightProfileTargetEndpointCharUUID, pAttr->type.len)))

    {
      // No need for "GATT_SERVICE_UUID" or "GATT_CLIENT_CHAR_CFG_UUID" cases;
      // gattserverapp handles those reads
      *pLen = LIGHTPROFILE_GENERIC_LEN;
      pValue[0] = *pAttr->pValue;
    }
    else if( !memcmp(pAttr->type.uuid, lightProfileTargetAddrCharUUID, pAttr->type.len))
    {
      *pLen = LIGHTPROFILE_TARGET_ADDR_LEN;
      VOID memcpy( pValue, pAttr->pValue, LIGHTPROFILE_TARGET_ADDR_LEN );
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
static bStatus_t lightProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;
  
  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
    if(!memcmp(pAttr->type.uuid, lightProfileLightOnOffCharUUID, pAttr->type.len))
    {
      //Validate the value
      // Make sure it's not a blob oper
      if ( offset == 0 )
      {
        if ( len != LIGHTPROFILE_LIGHT_ONOFF_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      //Write the value
      if ( status == SUCCESS )
      {
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        *pCurValue = pValue[0];

        notifyApp = LIGHTPROFILE_LIGHT_ONOFF_CHAR;
      }
    }
    else if(!memcmp(pAttr->type.uuid, lightProfileTargetAddrTypeCharUUID, pAttr->type.len))
    {
      //Validate the value
      // Make sure it's not a blob oper
      if ( offset == 0 )
      {
        if ( len != LIGHTPROFILE_TARGET_ADDR_TYPE_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      //Write the value
      if ( status == SUCCESS )
      {
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        *pCurValue = pValue[0];

        notifyApp = LIGHTPROFILE_TARGET_ADDR_TYPE_CHAR;
      }
    }
    else if(!memcmp(pAttr->type.uuid, lightProfileTargetAddrCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != LIGHTPROFILE_TARGET_ADDR_LEN )
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
        VOID memcpy( pAttr->pValue, pValue, LIGHTPROFILE_TARGET_ADDR_LEN );

        notifyApp = LIGHTPROFILE_TARGET_ADDR_CHAR;
      }
    }
    else if(!memcmp(pAttr->type.uuid, lightProfileTargetEndpointCharUUID, pAttr->type.len))
    {
      //Validate the value
      // Make sure it's not a blob oper
      if ( offset == 0 )
      {
        if ( len != LIGHTPROFILE_TARGET_ENDPOINT_LEN )
        {
          status = ATT_ERR_INVALID_VALUE_SIZE;
        }
      }
      else
      {
        status = ATT_ERR_ATTR_NOT_LONG;
      }

      //Write the value
      if ( status == SUCCESS )
      {
        uint8 *pCurValue = (uint8 *)pAttr->pValue;
        *pCurValue = pValue[0];

        notifyApp = LIGHTPROFILE_TARGET_ENDPOINT_CHAR;
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
  if ( (notifyApp != 0xFF ) && lightProfile_AppCBs && lightProfile_AppCBs->pfnLightProfileChange )
  {
    lightProfile_AppCBs->pfnLightProfileChange( notifyApp );  
  }
  
  return ( status );
}

/*********************************************************************
*********************************************************************/
