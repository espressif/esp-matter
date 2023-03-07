/******************************************************************************

 @file  network_device_gatt_profile.c

 @brief This file contains the GATT network device profile used to provision a 15.4
        collector device.

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

#include "ti_ble_config.h"
#include "network_device_gatt_profile.h"

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
// Network Device GATT Profile Service UUID
CONST uint8 networkDeviceProfileServUUID[ATT_UUID_SIZE] =
{
  NETDEVPROFILE_SERV_UUID_BASE128(NETDEVPROFILE_SERV_UUID)
};

// Network Device characteristic UUID
CONST uint8 networkDeviceProfileDeviceCharUUID[ATT_UUID_SIZE] =
{
  NETDEVPROFILE_CHAR_UUID_BASE128(NETDEVPROFILE_NETWORK_DEVICE_CHAR_UUID)
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

static networkDeviceProfileCBs_t *networkDeviceProfile_AppCBs = NULL;

/*********************************************************************
 * Profile Attributes - variables
 */

//  Remote Display Profile Service attribute
static CONST gattAttrType_t networkDeviceProfileService = { ATT_UUID_SIZE, networkDeviceProfileServUUID };

// Network Device Profile Device Characteristic Properties
static uint8_t networkDeviceProfileDeviceCharProps = GATT_PROP_WRITE | GATT_PROP_NOTIFY;

// Sensor Network Device State Characteristic Value
static uint8_t networkDeviceProfileDeviceCharVal[NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN] = {0};

// Network Device Profile Device Characteristic Configuration
static gattCharCfg_t *networkDeviceProfileDeviceDataConfig;

// Network Device Profile Device Characteristic User Description
static uint8_t networkDeviceProfileDeviceDataUserDesp[NETDEVPROFILE_MAX_DESCRIPTION_LEN] = "Network Device";

/*********************************************************************
 * Profile Attributes - Table
 */

static gattAttribute_t networkDeviceProfileAttrTbl[] =
{
  // Network Device Profile Service
  { 
    { ATT_BT_UUID_SIZE, primaryServiceUUID }, /* type */
    GATT_PERMIT_READ,                         /* permissions */
    0,                                        /* handle */
    (uint8 *)&networkDeviceProfileService      /* pValue */
  },
      // Device Characteristic Declaration
      {
        { ATT_BT_UUID_SIZE, characterUUID },
        GATT_PERMIT_READ,
        0,
        &networkDeviceProfileDeviceCharProps
      },
      // Device Characteristic Value
      {
        { ATT_UUID_SIZE, networkDeviceProfileDeviceCharUUID },
        GATT_PERMIT_AUTHEN_WRITE,
        0,
        networkDeviceProfileDeviceCharVal
      },

      // Device Characteristic configuration
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&networkDeviceProfileDeviceDataConfig
      },

      // Device Characteristic User Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        networkDeviceProfileDeviceDataUserDesp
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t networkDeviceProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr, 
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method);
static bStatus_t networkDeviceProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method);

/*********************************************************************
 * PROFILE CALLBACKS
 */

// Remote Display Profile Service Callbacks
// Note: When an operation on a characteristic requires authorization and 
// pfnAuthorizeAttrCB is not defined for that characteristic's service, the 
// Stack will report a status of ATT_ERR_UNLIKELY to the client.  When an 
// operation on a characteristic requires authorization the Stack will call 
// pfnAuthorizeAttrCB to check a client's authorization prior to calling
// pfnReadAttrCB or pfnWriteAttrCB, so no checks for authorization need to be 
// made within these functions.
CONST gattServiceCBs_t networkDeviceProfileCBs =
{
  networkDeviceProfile_ReadAttrCB,  // Read callback function pointer
  networkDeviceProfile_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

/*********************************************************************
 * @fn      Provisioning_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   services - services to add. This is a bit map and can
 *                     contain more than one service.
 *
 * @return  Success or Failure
 */
bStatus_t NetworkDeviceProfile_AddService( uint32 services )
{
  uint8 status;
  // Allocate Client Characteristic Configuration tables
  networkDeviceProfileDeviceDataConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) *
                                                            MAX_NUM_BLE_CONNS );
  if ( networkDeviceProfileDeviceDataConfig == NULL )
  {     
    return ( bleMemAllocError );
  }
  
  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LL_CONNHANDLE_INVALID, networkDeviceProfileDeviceDataConfig );

  if ( services & NETDEVPROFILE_SERVICE )
  {
    // Register GATT attribute list and CBs with GATT Server App
    status = GATTServApp_RegisterService( networkDeviceProfileAttrTbl,
                                          GATT_NUM_ATTRS( networkDeviceProfileAttrTbl ),
                                          GATT_MAX_ENCRYPT_KEY_SIZE,
                                          &networkDeviceProfileCBs );
  }
  else
  {
    status = SUCCESS;
  }

  return ( status );
}

/*********************************************************************
 * @fn      NetworkDeviceProfile_RegisterAppCBs
 *
 * @brief   Registers the application callback function. Only call 
 *          this function once.
 *
 * @param   callbacks - pointer to application callbacks.
 *
 * @return  SUCCESS or bleAlreadyInRequestedMode
 */
bStatus_t NetworkDeviceProfile_RegisterAppCBs( networkDeviceProfileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    networkDeviceProfile_AppCBs = appCallbacks;
    
    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*********************************************************************
 * @fn      NetworkDeviceProfile_SetParameter
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
bStatus_t NetworkDeviceProfile_SetParameter( uint8 param, uint8 len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case NETDEVPROFILE_DEVICE_CHAR:
      if ( len == NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN )
      {
        VOID memcpy(networkDeviceProfileDeviceCharVal, value, NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN);

        // See if Notification has been enabled
        GATTServApp_ProcessCharCfg( networkDeviceProfileDeviceDataConfig, (uint8_t *)&networkDeviceProfileDeviceCharVal, FALSE,
                                    networkDeviceProfileAttrTbl, GATT_NUM_ATTRS( networkDeviceProfileAttrTbl ),
                                    INVALID_TASK_ID, networkDeviceProfile_ReadAttrCB );
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
 * @fn      NetworkDeviceProfile_GetParameter
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
bStatus_t NetworkDeviceProfile_GetParameter( uint8 param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case NETDEVPROFILE_DEVICE_CHAR:
        VOID memcpy(value, networkDeviceProfileDeviceCharVal, NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN);
        break;
    default:
      ret = INVALIDPARAMETER;
      break;
  }
  
  return ( ret );
}

/*********************************************************************
 * @fn          networkDeviceProfile_ReadAttrCB
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
static bStatus_t networkDeviceProfile_ReadAttrCB(uint16_t connHandle,
                                          gattAttribute_t *pAttr,
                                          uint8_t *pValue, uint16_t *pLen,
                                          uint16_t offset, uint16_t maxLen,
                                          uint8_t method)
{
    bStatus_t status = SUCCESS;

    // See if request is regarding the NetworkDeviceProfileDeviceCharVal Characteristic Value
    if (!memcmp(pAttr->type.uuid, networkDeviceProfileDeviceCharUUID,
                pAttr->type.len))
    {
        if (offset > NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN) // Prevent malicious ATT ReadBlob offsets.
        {
            status = ATT_ERR_INVALID_OFFSET;
        }
        else
        {
            *pLen = MIN(maxLen, NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN - offset); // Transmit as much as possible
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

/*********************************************************************
 * @fn      networkDeviceProfile_WriteAttrCB
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
static bStatus_t networkDeviceProfile_WriteAttrCB(uint16_t connHandle,
                                           gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t len,
                                           uint16_t offset, uint8_t method)
{
  bStatus_t status = SUCCESS;
  uint8 notifyApp = 0xFF;


  if ( pAttr->type.len == ATT_UUID_SIZE )
  {
    if(!memcmp(pAttr->type.uuid, networkDeviceProfileDeviceCharUUID, pAttr->type.len))
    {
      if ( offset == 0 )
      {
        if ( len != NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN )
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
        VOID memcpy( pAttr->pValue, pValue, NETDEVPROFILE_NETWORK_DEVICE_CHAR_LEN );

        notifyApp = NETDEVPROFILE_DEVICE_CHAR;
      }
    }
    else
    {
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
  if ( (notifyApp != 0xFF ) && networkDeviceProfile_AppCBs->pfnNetworkDeviceProfileChange )
  {
    networkDeviceProfile_AppCBs->pfnNetworkDeviceProfileChange( notifyApp );
  }

  return ( status );
}

/*********************************************************************
*********************************************************************/
