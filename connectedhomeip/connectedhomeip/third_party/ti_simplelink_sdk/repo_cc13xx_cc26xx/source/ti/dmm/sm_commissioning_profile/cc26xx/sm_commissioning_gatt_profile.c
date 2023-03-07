/**********************************************************************************************
 * Filename:       sm_commissioning_gatt_profile.c
 *
 * Description:    This file contains the implementation of the service.
 *
 * Copyright (c) 2015-2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *************************************************************************************************/


/*********************************************************************
 * INCLUDES
 */
#include <string.h>

#include <icall.h>

/* This Header file contains all BLE API and icall structure definition */
#include "icall_ble_api.h"

#include "sm_commissioning_gatt_profile.h"
#include "ti_ble_config.h"

#include "remote_display.h"

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

// sm_commissioning_gatt_profile Service UUID
CONST uint8_t sm_commissioning_profileUUID[ATT_UUID_SIZE] =
{
  SMCOMMISSIONING_PROFILE_SERV_UUID_BASE128(SMCOMMISSIONING_PROFILE_SERV_UUID)
};

// smCommissionStateChar UUID
CONST uint8_t sm_commissioning_StateCharUUID[ATT_UUID_SIZE] =
{
  SMCOMMISSIONING_PROFILE_CHAR_UUID_BASE128(SMCOMMISSIONING_PROFILE_STATECHAR_UUID)
};
#ifdef DMM_SENSOR
// smSetAuthModeChar UUID
CONST uint8_t sm_commissioning_SetAuthModeCharUUID[ATT_UUID_SIZE] =
{
  SMCOMMISSIONING_PROFILE_CHAR_UUID_BASE128(SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_UUID)
};
#endif
// smSetPasskeyChar UUID
CONST uint8_t sm_commissioning_SetPasskeyCharUUID[ATT_UUID_SIZE] =
{
  SMCOMMISSIONING_PROFILE_CHAR_UUID_BASE128(SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_UUID)
};

/*********************************************************************
 * LOCAL VARIABLES
 */

static sm_commissioning_profileCBs_t *pAppCBs = NULL;

/*********************************************************************
* Profile Attributes - variables
*/

// Service declaration
static CONST gattAttrType_t sm_commissioning_profileDecl = { ATT_UUID_SIZE, sm_commissioning_profileUUID };

// Characteristic "sm_commissioning_StateChar" Properties (for declaration)
static uint8_t sm_commissioning_StateCharProps = GATT_PROP_READ | GATT_PROP_NOTIFY;
// Characteristic "sm_commissioning_StateChar" Value variable
static uint8_t sm_commissioning_StateCharVal = 0;
// Characteristic "sm_commissioning_StateChar" CCCD
static gattCharCfg_t *sm_commissioning_StateCharConfig;
// SM Commissioning Profile State Characteristic User Description
static uint8 sm_commissioning_StateCharDesp[SMCOMMISSIONING_MAX_DESCRIPTION_LEN] = "SM Commission State";

#ifdef DMM_SENSOR
// Characteristic "sm_commissioning_SetAuthModeChar" Properties (for declaration)
static uint8_t sm_commissioning_SetAuthModeCharProps = GATT_PROP_READ | GATT_PROP_WRITE;
// Characteristic "sm_commissioning_SetAuthModeChar" Value variable
static uint8_t sm_commissioning_SetAuthModeCharVal[SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN] = {0};
// SM Commissioning Profile Set Auth Mode Characteristic User Description
static uint8 sm_commissioning_SetAuthModeCharDesp[SMCOMMISSIONING_MAX_DESCRIPTION_LEN] = "Set Auth Mode";
#endif

// Characteristic "sm_commissioning_SetPasskeyChar" Properties (for declaration)
static uint8_t sm_commissioning_SetPasskeyCharProps = GATT_PROP_WRITE;
// Characteristic "sm_commissioning_SetPasskeyChar" Value variable
static uint8_t sm_commissioning_SetPasskeyCharVal[SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_LEN] = {0, 0, 0, 0};
// Characteristic "sm_commissioning_SetPasskeyChar" CCCD
static gattCharCfg_t *sm_commissioning_SetPasskeyCharConfig;
// SM Commissioning Profile Set Passkey Characteristic User Description
static uint8 sm_commissioning_SetPasskeyCharDesp[SMCOMMISSIONING_MAX_DESCRIPTION_LEN] = "Set Passkey";


/*********************************************************************
* Profile Attributes - Table
*/

static gattAttribute_t sm_commissioning_profileAttrTbl[] =
{
  // sm_commissioning_gatt_profile Service Declaration
  {
    { ATT_BT_UUID_SIZE, primaryServiceUUID },
    GATT_PERMIT_READ,
    0,
    (uint8_t *)&sm_commissioning_profileDecl
  },
    // SmCommissionStateChar Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &sm_commissioning_StateCharProps
    },
      // SmCommissionStateChar Characteristic Value
      {
        { ATT_UUID_SIZE, sm_commissioning_StateCharUUID },
        GATT_PERMIT_AUTHEN_READ,
        0,
        &sm_commissioning_StateCharVal
      },
      // SmCommissionStateChar CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ | GATT_PERMIT_WRITE,
        0,
        (uint8 *)&sm_commissioning_StateCharConfig
      },
      // SmCommissionStateChar Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        sm_commissioning_StateCharDesp
      },
#ifdef DMM_SENSOR
    // SmSetAuthModeChar Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &sm_commissioning_SetAuthModeCharProps
    },
      // SmSetAuthModeChar Characteristic Value
      {
        { ATT_UUID_SIZE, sm_commissioning_SetAuthModeCharUUID },
        GATT_PERMIT_AUTHEN_READ | GATT_PERMIT_AUTHEN_WRITE,
        0,
        sm_commissioning_SetAuthModeCharVal
      },
      // SmCommissionAuthModeChar Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        sm_commissioning_SetAuthModeCharDesp
      },
#endif
    // SmSetPasskeyChar Characteristic Declaration
    {
      { ATT_BT_UUID_SIZE, characterUUID },
      GATT_PERMIT_READ,
      0,
      &sm_commissioning_SetPasskeyCharProps
    },
      // SmSetPasskeyChar Characteristic Value
      {
        { ATT_UUID_SIZE, sm_commissioning_SetPasskeyCharUUID },
        GATT_PERMIT_AUTHEN_WRITE,
        0,
        sm_commissioning_SetPasskeyCharVal
      },
      // SmSetPasskeyChar CCCD
      {
        { ATT_BT_UUID_SIZE, clientCharCfgUUID },
        GATT_PERMIT_READ,
        0,
        (uint8 *)&sm_commissioning_SetPasskeyCharConfig
      },
      // SmCommissionSetPasskeyChar Description
      {
        { ATT_BT_UUID_SIZE, charUserDescUUID },
        GATT_PERMIT_READ,
        0,
        sm_commissioning_SetPasskeyCharDesp
      },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */
static bStatus_t sm_commissioning_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                           uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                           uint16_t maxLen, uint8_t method );
static bStatus_t sm_commissioning_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                            uint8_t *pValue, uint16_t len, uint16_t offset,
                                            uint8_t method );

/*********************************************************************
 * PROFILE CALLBACKS
 */
// Simple Profile Service Callbacks
CONST gattServiceCBs_t sm_commissioning_gatt_profileCBs =
{
  sm_commissioning_ReadAttrCB,  // Read callback function pointer
  sm_commissioning_WriteAttrCB, // Write callback function pointer
  NULL                       // Authorization callback function pointer
};

/*********************************************************************
* PUBLIC FUNCTIONS
*/

/*
 * SmCommissioning_AddService- Initializes the Sm_commissioning_gatt_profile service by registering
 *          GATT attributes with the GATT server.
 *
 */
bStatus_t SmCommissioning_AddService( uint32 services )
{
  uint8_t status;

  // Allocate Client Characteristic Configuration table
  sm_commissioning_StateCharConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS );
  if ( sm_commissioning_StateCharConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  // Initialize Client Characteristic Configuration attributes
  GATTServApp_InitCharCfg( LL_CONNHANDLE_INVALID, sm_commissioning_StateCharConfig );

  // Allocate Client Characteristic Configuration table
  sm_commissioning_SetPasskeyCharConfig = (gattCharCfg_t *)ICall_malloc( sizeof(gattCharCfg_t) * MAX_NUM_BLE_CONNS );
  if ( sm_commissioning_SetPasskeyCharConfig == NULL )
  {
    return ( bleMemAllocError );
  }

  if ( services & SMCOMMPROFILE_SERVICE )
  {
      // Initialize Client Characteristic Configuration attributes
      GATTServApp_InitCharCfg( LL_CONNHANDLE_INVALID, sm_commissioning_SetPasskeyCharConfig );
      // Register GATT attribute list and CBs with GATT Server App
      status = GATTServApp_RegisterService( sm_commissioning_profileAttrTbl,
                                            GATT_NUM_ATTRS( sm_commissioning_profileAttrTbl ),
                                            GATT_MAX_ENCRYPT_KEY_SIZE,
                                            &sm_commissioning_gatt_profileCBs );
  }
  else{
      status = SUCCESS;
  }

  return ( status );
}

/*
 * SmCommissioning_RegisterAppCBs - Registers the application callback function.
 *                    Only call this function once.
 *
 *    appCallbacks - pointer to application callbacks.
 */
bStatus_t SmCommissioning_RegisterAppCBs( sm_commissioning_profileCBs_t *appCallbacks )
{
  if ( appCallbacks )
  {
    pAppCBs = appCallbacks;

    return ( SUCCESS );
  }
  else
  {
    return ( bleAlreadyInRequestedMode );
  }
}

/*
 * SmCommissioning_SetParameter - Set a Sm_commissioning_gatt_profile parameter.
 *
 *    param - Profile parameter ID
 *    len - length of data to right
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t SmCommissioning_SetParameter( uint8_t param, uint16_t len, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SMCOMMISSIONING_PROFILE_STATECHAR_ID:
      if ( len == SMCOMMISSIONING_PROFILE_STATECHAR_LEN )
      {
        sm_commissioning_StateCharVal = *((uint8*)value);

        // Try to send notification.
        GATTServApp_ProcessCharCfg( sm_commissioning_StateCharConfig, &sm_commissioning_StateCharVal, FALSE,
                                    sm_commissioning_profileAttrTbl, GATT_NUM_ATTRS( sm_commissioning_profileAttrTbl ),
                                    INVALID_TASK_ID,  sm_commissioning_ReadAttrCB);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

#ifdef DMM_SENSOR
    case SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_ID:
      if ( len == SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN )
      {
        memcpy(sm_commissioning_SetAuthModeCharVal, value, len);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;
#endif

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*
 * SmCommissioning_GetParameter - Get a Sm_commissioning_gatt_profile parameter.
 *
 *    param - Profile parameter ID
 *    value - pointer to data to write.  This is dependent on
 *          the parameter ID and WILL be cast to the appropriate
 *          data type (example: data type of uint16 will be cast to
 *          uint16 pointer).
 */
bStatus_t SmCommissioning_GetParameter( uint8_t param, void *value )
{
  bStatus_t ret = SUCCESS;
  switch ( param )
  {
    case SMCOMMISSIONING_PROFILE_STATECHAR_ID:
        *((uint8*)value) = sm_commissioning_StateCharVal;
      break;

#ifdef DMM_SENSOR
    case SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_ID:
      memcpy(value, sm_commissioning_SetAuthModeCharVal, SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN);
      break;
#endif

    case SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_ID:
      memcpy(value, sm_commissioning_SetPasskeyCharVal, SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_LEN);
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }
  return ret;
}


/*********************************************************************
 * @fn          sm_commissioning_ReadAttrCB
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
static bStatus_t sm_commissioning_ReadAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
                                       uint8_t *pValue, uint16_t *pLen, uint16_t offset,
                                       uint16_t maxLen, uint8_t method )
{
  bStatus_t status = SUCCESS;

  // See if request is regarding the SmCommissionStateChar Characteristic Value
  if ( ! memcmp(pAttr->type.uuid, sm_commissioning_StateCharUUID, pAttr->type.len) )
  {
    if ( offset > SMCOMMISSIONING_PROFILE_STATECHAR_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, SMCOMMISSIONING_PROFILE_STATECHAR_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }

#ifdef DMM_SENSOR
  // See if request is regarding the SmSetAuthModeChar Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, sm_commissioning_SetAuthModeCharUUID, pAttr->type.len) )
  {
    if ( offset > SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN )  // Prevent malicious ATT ReadBlob offsets.
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      *pLen = MIN(maxLen, SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN - offset);  // Transmit as much as possible
      memcpy(pValue, pAttr->pValue + offset, *pLen);
    }
  }
#endif

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
 * @fn      sm_commissioning_WriteAttrCB
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
static bStatus_t sm_commissioning_WriteAttrCB( uint16_t connHandle, gattAttribute_t *pAttr,
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
  // See if request is regarding the SmCommissionStateChar Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, sm_commissioning_StateCharUUID, pAttr->type.len) )
  {
    if ( offset + len > SMCOMMISSIONING_PROFILE_STATECHAR_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      if (RemoteDisplay_checkSmAccessAllowed(connHandle))
      {
        // Copy pValue into the variable we point to from the attribute table.
        memcpy(pAttr->pValue + offset, pValue, len);

        // Only notify application if entire expected value is written
        if ( offset + len == SMCOMMISSIONING_PROFILE_STATECHAR_LEN)
          paramID = SMCOMMISSIONING_PROFILE_STATECHAR_ID;
      }
      else
      {
        status = ATT_ERR_INSUFFICIENT_AUTHOR;
      }
    }
  }

#ifdef DMM_SENSOR
  // See if request is regarding the SmSetAuthModeChar Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, sm_commissioning_SetAuthModeCharUUID, pAttr->type.len) )
  {
    if ( offset + len > SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      if (RemoteDisplay_checkSmAccessAllowed(connHandle))
      {
        // Copy pValue into the variable we point to from the attribute table.
        memcpy(pAttr->pValue + offset, pValue, len);

        // Only notify application if entire expected value is written
        if ( offset + len == SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_LEN)
          paramID = SMCOMMISSIONING_PROFILE_SETAUTHMODECHAR_ID;
      }
      else
      {
        status = ATT_ERR_INSUFFICIENT_AUTHOR;
      }
    }
  }
#endif

  // See if request is regarding the SmSetPasskeyChar Characteristic Value
  else if ( ! memcmp(pAttr->type.uuid, sm_commissioning_SetPasskeyCharUUID, pAttr->type.len) )
  {
    if ( offset + len > SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_LEN )
    {
      status = ATT_ERR_INVALID_OFFSET;
    }
    else
    {
      if (RemoteDisplay_checkSmAccessAllowed(connHandle))
      {
        // Copy pValue into the variable we point to from the attribute table.
        memcpy(pAttr->pValue + offset, pValue, len);

        // Only notify application if entire expected value is written
        if ( offset + len == SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_LEN)
          paramID = SMCOMMISSIONING_PROFILE_SETPASSKEYCHAR_ID;
      }
      else
      {
        status = ATT_ERR_INSUFFICIENT_AUTHOR;
      }
    }
  }
  else
  {
    // If we get here, that means you've forgotten to add an if clause for a
    // characteristic value attribute in the attribute table that has WRITE permissions.
    status = ATT_ERR_ATTR_NOT_FOUND;
  }

  // Let the application know something changed (if it did) by using the
  // callback it registered earlier (if it did).
  if (paramID != 0xFF)
    if ( pAppCBs && pAppCBs->pfnSmCommissioningProfileChangeCb )
      pAppCBs->pfnSmCommissioningProfileChangeCb(paramID); // Call app function from stack task context.

  return status;
}
