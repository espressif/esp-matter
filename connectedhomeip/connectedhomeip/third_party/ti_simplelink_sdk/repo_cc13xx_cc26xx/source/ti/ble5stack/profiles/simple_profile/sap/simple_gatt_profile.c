/******************************************************************************

 @file  simple_gatt_profile.c

 @brief This file contains the Simple GATT profile sample GATT service profile
        for use with the BLE sample application.

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2010-2022, Texas Instruments Incorporated
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
#include <stdint.h>

#include <ti/sap/snp.h>
#include <ti/sap/snp_rpc.h>
#include <ti/sap/sap.h>
#include "simple_gatt_profile.h"

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * CONSTANTS
 */

#define SP_DEFAULT_CCCD         0
#define SERVAPP_NUM_ATTR_SUPPORTED 5
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
// Simple GATT Profile Service UUID: 0xFFF0
uint8 simpleProfileServUUID[SNP_16BIT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_SERV_UUID), HI_UINT16(SIMPLEPROFILE_SERV_UUID)
};

// Characteristic 1 UUID: 0xFFF1
uint8 simpleProfilechar1UUID[SNP_16BIT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR1_UUID), HI_UINT16(SIMPLEPROFILE_CHAR1_UUID)
};

// Characteristic 2 UUID: 0xFFF2
uint8 simpleProfilechar2UUID[SNP_16BIT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR2_UUID), HI_UINT16(SIMPLEPROFILE_CHAR2_UUID)
};

// Characteristic 3 UUID: 0xFFF3
uint8 simpleProfilechar3UUID[SNP_16BIT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR3_UUID), HI_UINT16(SIMPLEPROFILE_CHAR3_UUID)
};

// Characteristic 4 UUID: 0xFFF4
uint8 simpleProfilechar4UUID[SNP_16BIT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR4_UUID), HI_UINT16(SIMPLEPROFILE_CHAR4_UUID)
};

// Characteristic 5 UUID: 0xFFF5
uint8 simpleProfilechar5UUID[SNP_16BIT_UUID_SIZE] =
{
  LO_UINT16(SIMPLEPROFILE_CHAR5_UUID), HI_UINT16(SIMPLEPROFILE_CHAR5_UUID)
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

static simpleProfileWrite_t simpleProfile_AppWriteCB = NULL;
static simpleProfileCCCD_t simpleProfile_AppCccdCB = NULL;

static uint8_t cccdFlag = SP_DEFAULT_CCCD;

static uint16_t connHandle = 0; // Assumes only one connection at a time

/*********************************************************************
 * Profile Attributes - variables
 */

SAP_Service_t simpleService;
SAP_CharHandle_t simpleServiceCharHandles[SERVAPP_NUM_ATTR_SUPPORTED];

static UUIDType_t simpleServiceUUID = {SNP_16BIT_UUID_SIZE, simpleProfileServUUID};

// Characteristic 1 Value
static uint8 simpleProfileChar1 = 0;

// Simple Profile Characteristic 1 User Description
static uint8 simpleProfileChar1UserDesp[17] = "Characteristic 1";

// Characteristic 2 Value
static uint8 simpleProfileChar2 = 0;

// Simple Profile Characteristic 2 User Description
static uint8 simpleProfileChar2UserDesp[17] = "Characteristic 2";

// Characteristic 3 Value
static uint8 simpleProfileChar3 = 0;

// Simple Profile Characteristic 3 User Description
static uint8 simpleProfileChar3UserDesp[17] = "Characteristic 3";

// Characteristic 4 Value
static uint8 simpleProfileChar4 = 0;

// Simple Profile Characteristic 4 User Description
static uint8 simpleProfileChar4UserDesp[17] = "Characteristic 4";

// Characteristic 5 Value
static uint8 simpleProfileChar5 = 0;

// Simple Profile Characteristic 4 User Description
static uint8 simpleProfileChar5UserDesp[17] = "Characteristic 5";

/*********************************************************************
 * Profile Attributes - Table
 */
SAP_UserDescAttr_t char1UserDesc = {SNP_GATT_PERMIT_READ, 17, 17, simpleProfileChar1UserDesp};
SAP_UserDescAttr_t char2UserDesc = {SNP_GATT_PERMIT_READ, 17, 17, simpleProfileChar2UserDesp};
SAP_UserDescAttr_t char3UserDesc = {SNP_GATT_PERMIT_READ, 17, 17, simpleProfileChar3UserDesp};
SAP_UserDescAttr_t char4UserDesc = {SNP_GATT_PERMIT_READ, 17, 17, simpleProfileChar4UserDesp};
SAP_UserDescAttr_t char5UserDesc = {SNP_GATT_PERMIT_READ, 17, 17, simpleProfileChar5UserDesp};

SAP_UserCCCDAttr_t char4CCCD = {SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE};

static SAP_Char_t simpleProfileAttrTbl[SERVAPP_NUM_ATTR_SUPPORTED] =
{
    // Characteristic 1 Value Declaration
    {
      { SNP_16BIT_UUID_SIZE, simpleProfilechar1UUID }, /* UUID             */
      SNP_GATT_PROP_READ | SNP_GATT_PROP_WRITE,        /* Properties       */
      SNP_GATT_PERMIT_READ | SNP_GATT_PERMIT_WRITE,    /* Permissions      */
      &char1UserDesc,                                  /* User Description */
      NULL,                                            /* CCCD             */
      NULL,                                            /* Format           */
      NULL,                                            /* Short UUID       */
      NULL                                             /* Long UUID        */
    },

    // Characteristic 2 Value Declaration
    {
      { SNP_16BIT_UUID_SIZE, simpleProfilechar2UUID }, /* UUID             */
      SNP_GATT_PROP_READ,                              /* Properties       */
      SNP_GATT_PERMIT_READ,                            /* Permissions      */
      &char2UserDesc                                   /* User Description */
                                                       /* CCCD             */
                                                       /* Format           */
    },

    // Characteristic 3 Value Declaration
    {
      { SNP_16BIT_UUID_SIZE, simpleProfilechar3UUID }, /* UUID             */
      SNP_GATT_PROP_WRITE,                             /* Properties       */
      SNP_GATT_PERMIT_WRITE,                           /* Permissions      */
      &char3UserDesc                                   /* User Description */
                                                       /* CCCD             */
                                                       /* Format           */
    },

    // Characteristic 4 Value Declaration
    {
      { SNP_16BIT_UUID_SIZE, simpleProfilechar4UUID }, /* UUID             */
      SNP_GATT_PROP_NOTIFICATION,                      /* Properties       */
      0,                                               /* Permissions      */
      &char4UserDesc,                                  /* User Description */
      &char4CCCD                                       /* CCCD             */
                                                       /* Format           */
    },

    // Characteristic 5 Value Declaration
    {
      { SNP_16BIT_UUID_SIZE, simpleProfilechar5UUID }, /* UUID             */
      SNP_GATT_PROP_READ,                              /* Properties       */
      SNP_GATT_PERMIT_ENCRYPT_READ,                    /* Permissions      */
      &char5UserDesc                                   /* User Description */
                                                       /* CCCD             */
                                                       /* Format           */
    },
};

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static bStatus_t simpleProfile_ReadAttrCB(void *context,
                                   uint16_t connectionHandle,
                                   uint16_t charHdl, uint16_t offset,
                                   uint16_t size, uint16_t * len,
                                   uint8_t *pData);

bStatus_t simpleProfile_WriteAttrCB(void *context,
                                    uint16_t connectionHandle,
                                    uint16_t charHdl, uint16_t len,
                                    uint8_t *pData);

bStatus_t simpleProfile_CCCDIndCB(void *context,
                                  uint16_t connectionHandle,
                                  uint16_t cccdHdl, uint8_t type,
                                  uint16_t value);

/*********************************************************************
 * PROFILE CALLBACKS
 */

/*********************************************************************
 * PUBLIC FUNCTIONS
 */

static void simpleProfile_processSNPEventCB(uint16_t event,
                                            snpEventParam_t *param);

static uint16_t getCharIDFromHdl(uint16_t charHdl);
static uint16_t getHdlFromCharID(uint16_t charID);

/*********************************************************************
 * @fn      SimpleProfile_AddService
 *
 * @brief   Initializes the Simple Profile service by registering
 *          GATT attributes with the GATT server.
 *
 * @param   none
 *
 * @return  Success or Failure
 */
bStatus_t SimpleProfile_AddService(void)
{
  uint8_t status;

  // Register to recieve Connection Termination Events
  status = SAP_registerEventCB(simpleProfile_processSNPEventCB,
                               SNP_CONN_TERM_EVT);

  if (status == SNP_SUCCESS)
  {
    // reads through table, adding attributes to the NP.
    simpleService.serviceUUID       = simpleServiceUUID;
    simpleService.serviceType       = SNP_PRIMARY_SERVICE;
    simpleService.charTableLen      = SERVAPP_NUM_ATTR_SUPPORTED;
    simpleService.charTable         = simpleProfileAttrTbl;
    simpleService.context           = NULL;
    simpleService.charReadCallback  = simpleProfile_ReadAttrCB;
    simpleService.charWriteCallback = simpleProfile_WriteAttrCB;
    simpleService.cccdIndCallback   = simpleProfile_CCCDIndCB;
    simpleService.charAttrHandles   = simpleServiceCharHandles;

    // Service is setup, register with GATT server on the SNP.
    status = SAP_registerService(&simpleService);
  }

  return status;
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
bStatus_t SimpleProfile_RegisterAppCB(simpleProfileWrite_t writeCB,
                                      simpleProfileCCCD_t cccdCB)
{
  simpleProfile_AppWriteCB = writeCB;
  simpleProfile_AppCccdCB = cccdCB;

  return (SUCCESS);
}

/*********************************************************************
 * @fn      SimpleProfile_SetParameter
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
bStatus_t SimpleProfile_SetParameter(uint8 param, uint8 len, void *value)
{
  bStatus_t ret = SUCCESS;
  switch (SP_ID_CHAR(param))
  {
    case SP_CHAR1:
      if (len == sizeof (uint8))
      {
        simpleProfileChar1 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SP_CHAR2:
      if (len == sizeof (uint8))
      {
        simpleProfileChar2 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SP_CHAR3:
      if (len == sizeof (uint8))
      {
        simpleProfileChar3 = *((uint8*)value);
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SP_CHAR4:
      if (len == sizeof (uint8))
      {
        snpNotifIndReq_t localReq;
        simpleProfileChar4 = *((uint8*)value);

        // Initialize Request
        localReq.connHandle = connHandle;
        localReq.attrHandle = getHdlFromCharID(SP_ID_CREATE(SP_CHAR4,SP_VALUE));
        localReq.pData = (uint8_t *)&simpleProfileChar4;
        localReq.authenticate = 0; // Default no authentication

        // Check for whether a notification or indication should be sent.
        // Both flags should never be allowed to be set by NWP
        if (cccdFlag & SNP_GATT_CLIENT_CFG_NOTIFY)
        {
          localReq.type = SNP_SEND_NOTIFICATION;
          SNP_RPC_sendNotifInd(&localReq, sizeof(simpleProfileChar4));
        }
        else if (cccdFlag & SNP_GATT_CLIENT_CFG_INDICATE)
        {
          localReq.type = SNP_SEND_INDICATION;
          SNP_RPC_sendNotifInd(&localReq, sizeof(simpleProfileChar4));
        }
      }
      else
      {
        ret = bleInvalidRange;
      }
      break;

    case SP_CHAR5:
      if (len == sizeof (uint8))
      {
        simpleProfileChar5 = *((uint8*)value);
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

/*********************************************************************
 * @fn      SimpleProfile_GetParameter
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
bStatus_t SimpleProfile_GetParameter(uint8 param, void *value)
{
  bStatus_t ret = SUCCESS;
  switch (SP_ID_CHAR(param))
  {
    case SP_CHAR1:
      *((uint8*)value) = simpleProfileChar1;
      break;

    case SP_CHAR2:
      *((uint8*)value) = simpleProfileChar2;
      break;

    case SP_CHAR3:
      *((uint8*)value) = simpleProfileChar3;
      break;

    case SP_CHAR4:
      *((uint8*)value) = simpleProfileChar4;
      break;

    case SP_CHAR5:
      *((uint8*)value) = simpleProfileChar5;
      break;

    default:
      ret = INVALIDPARAMETER;
      break;
  }

  return (ret);
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

bStatus_t simpleProfile_ReadAttrCB(void *context,
                                   uint16_t connectionHandle,
                                   uint16_t charHdl, uint16_t offset,
                                   uint16_t size, uint16_t * len,
                                   uint8_t *pData)
{
  // Get characteristic from handle
  uint8_t charID = getCharIDFromHdl(charHdl);
  uint8_t isValid = 0;

  // Update Conn Handle (assumes one connection)
  connHandle = connectionHandle;

  switch(SP_ID_CHAR(charID))
  {
    case SP_CHAR1:
      switch (SP_ID_CHARHTYPE(charID))
      {
        case SP_VALUE:
          *len = sizeof(simpleProfileChar1);
          memcpy(pData,&simpleProfileChar1,sizeof(simpleProfileChar1));
          isValid = 1;
          break;

        default:
          // Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD
          break;
      }
      break;
    case SP_CHAR2:
      switch (SP_ID_CHARHTYPE(charID))
      {
        case SP_VALUE:
          *len = sizeof(simpleProfileChar2);
          memcpy(pData,&simpleProfileChar2,sizeof(simpleProfileChar2));
          isValid = 1;
          break;

        default:
          // Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD
          break;
      }
      break;
    case SP_CHAR5:
      switch (SP_ID_CHARHTYPE(charID))
      {
        case SP_VALUE:
          *len = sizeof(simpleProfileChar5);
          memcpy(pData, &simpleProfileChar5, sizeof(simpleProfileChar5));
          isValid = 1;

        default:
          // Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD
          break;
      }
      break;

    default:
      // Should not receive SP_CHAR3 || SP_CHAR4 reads
      break;
  }

  if (isValid)
  {
	return (SNP_SUCCESS);
  }

  // Unable to find handle - set len to 0 and return error code
  *len = 0;
  return (SNP_UNKNOWN_ATTRIBUTE);
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
bStatus_t simpleProfile_WriteAttrCB(void *context,
                                    uint16_t connectionHandle,
                                    uint16_t charHdl, uint16_t len,
                                    uint8_t *pData)
{
  bStatus_t status = SNP_UNKNOWN_ATTRIBUTE;
  uint8 notifyApp = SP_UNKNOWN_CHAR;

  // Update Conn Handle (assumes one connection)
  connHandle = connectionHandle;

  // Get characteristic from handle
  uint8_t charID = getCharIDFromHdl(charHdl);

  switch(SP_ID_CHAR(charID))
  {
    case SP_CHAR1:
      switch (SP_ID_CHARHTYPE(charID))
      {
        case SP_VALUE:
          if (len == sizeof(simpleProfileChar1))
          {
            simpleProfileChar1 = pData[0];
            status = SNP_SUCCESS;
            notifyApp = SP_CHAR1_ID;
          }
          break;
        default:
          // Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD
          break;
      }
      break;
    case SP_CHAR3:
      switch (SP_ID_CHARHTYPE(charID))
      {
        case SP_VALUE:
          if (len == sizeof(simpleProfileChar3))
          {
            simpleProfileChar3 = pData[0];
            status = SNP_SUCCESS;
            notifyApp = SP_CHAR3_ID;
          }
          break;
        default:
          // Should not receive SP_USERDESC || SP_FORMAT || SP_CCCD
          break;
      }
      break;
    default:
      // Should not receive SP_CHAR2 || SP_CHAR4 || SP_CHAR5 writes
      break;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ((notifyApp != SP_UNKNOWN_CHAR) && simpleProfile_AppWriteCB)
  {
    simpleProfile_AppWriteCB(notifyApp);
  }

  return (status);
}

/*********************************************************************
 * @fn      simpleProfile_CCCDIndCB
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
bStatus_t simpleProfile_CCCDIndCB(void *context,
                                    uint16_t connectionHandle,
                                    uint16_t cccdHdl, uint8_t type,
                                    uint16_t value)
{
  bStatus_t status = SNP_UNKNOWN_ATTRIBUTE;
  uint8 notifyApp = SP_UNKNOWN_CHAR;

  // Update Conn Handle (assumes one connection)
  connHandle = connectionHandle;

  // Get characteristic from handle
  uint8_t charID = getCharIDFromHdl(cccdHdl);

  switch(SP_ID_CHAR(charID))
  {
    case SP_CHAR4:
      switch (SP_ID_CHARHTYPE(charID))
      {
        case SP_CCCD:
          // Set Global cccd Flag which will be used to to gate Indications
          // or Notifications when SetParameter() is called
          cccdFlag = value;
          notifyApp = charID;
          status = SNP_SUCCESS;
          break;
        default:
          // Should not receive SP_VALUE || SP_USERDESC || SP_FORMAT
          break;
      }
      break;
    default:
      // No other Characteristics have CCCB attributes
      break;
  }

  // If a characteristic value changed then callback function to notify application of change
  if ((notifyApp != SP_UNKNOWN_CHAR) && simpleProfile_AppCccdCB)
  {
    simpleProfile_AppCccdCB(notifyApp, value);
  }

  return (status);
}


/*********************************************************************
 * @fn      simpleProfile_processSNPEventCB
 *
 * @brief   This is a callback operating in the NPI task. It will be envoked
 *          whenever an event is received from the SNP that this profile has
 *          registered for
 *
 * @param   event  - event mask
 * @param   pValue - pointer event struct
 *
 * @return  status
 */
void simpleProfile_processSNPEventCB(uint16_t event, snpEventParam_t *param)
{
  switch(event)
  {
    case SNP_CONN_TERM_EVT:
      {
        cccdFlag = SP_DEFAULT_CCCD;
      }
      break;
    default:
      break;
  }
}

/*********************************************************************
/ Utility function for translating a characteristic handle to
/ a handle ID (handle ID is a characteristic number plus type)
/
*/

uint16_t getCharIDFromHdl(uint16_t charHdl)
{
  uint16_t i;

  // Find Characteristic based on Handle
  for (i = 0; i < SERVAPP_NUM_ATTR_SUPPORTED; i++)
  {
    // Check if it is the characteristic value handle or cccd handle
    if (simpleServiceCharHandles[i].valueHandle == charHdl)
    {
      return SP_ID_CREATE(i,SP_VALUE);
    }
    else if (simpleServiceCharHandles[i].userDescHandle == charHdl)
    {
      return SP_ID_CREATE(i,SP_USERDESC);
    }
    else if (simpleServiceCharHandles[i].cccdHandle == charHdl)
    {
      return SP_ID_CREATE(i,SP_CCCD);
    }
    else if (simpleServiceCharHandles[i].formatHandle == charHdl)
    {
      return SP_ID_CREATE(i,SP_FORMAT);
    }
  }

  return SP_UNKNOWN_CHAR;
}

/*********************************************************************
/ Utility function for translating a characteristic ID to a handle
/
*/

uint16_t getHdlFromCharID(uint16_t charID)
{
  SAP_CharHandle_t charstick;

  if (SP_ID_CHAR(charID) < SERVAPP_NUM_ATTR_SUPPORTED)
  {
     charstick = simpleServiceCharHandles[SP_ID_CHAR(charID)];

    switch (SP_ID_CHARHTYPE(charID))
    {
    case SP_VALUE:
      return charstick.valueHandle;
    case SP_USERDESC:
      return charstick.userDescHandle;
    case SP_CCCD:
      return charstick.cccdHandle;
    case SP_FORMAT:
      return charstick.formatHandle;
    default:
      break;
    }
  }

  return SNP_INVALID_HANDLE;
}


/*********************************************************************
*********************************************************************/
