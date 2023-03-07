/******************************************************************************

 @file  gattservapp_util.c

 @brief This file contains the GATT Server Application utility functions

 Group: WCS, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2013-2022, Texas Instruments Incorporated

 All rights reserved not granted herein.
 Limited License.

 Texas Instruments Incorporated grants a world-wide, royalty-free,
 non-exclusive license under copyrights and patents it now or hereafter
 owns or controls to make, have made, use, import, offer to sell and sell
 ("Utilize") this software subject to the terms herein. With respect to the
 foregoing patent license, such license is granted solely to the extent that
 any such patent is necessary to Utilize the software alone. The patent
 license shall not apply to any combinations which include this software,
 other than combinations with devices manufactured by or for TI ("TI
 Devices"). No hardware patent is licensed hereunder.

 Redistributions must preserve existing copyright notices and reproduce
 this license (including the above copyright notice and the disclaimer and
 (if applicable) source code license limitations below) in the documentation
 and/or other materials provided with the distribution.

 Redistribution and use in binary form, without modification, are permitted
 provided that the following conditions are met:

   * No reverse engineering, decompilation, or disassembly of this software
     is permitted with respect to any software provided in binary form.
   * Any redistribution and use are licensed by TI for use only with TI Devices.
   * Nothing shall obligate TI to provide you with source code for the software
     licensed and provided to you in object code.

 If software source code is provided to you, modification and redistribution
 of the source code are permitted provided that the following conditions are
 met:

   * Any redistribution and use of the source code, including any resulting
     derivative works, are licensed by TI for use only with TI Devices.
   * Any redistribution and use of any object code compiled from the source
     code and any resulting derivative works, are licensed by TI for use
     only with TI Devices.

 Neither the name of Texas Instruments Incorporated nor the names of its
 suppliers may be used to endorse or promote products derived from this
 software without specific prior written permission.

 DISCLAIMER.

 THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************
 
 
 *****************************************************************************/

/*******************************************************************************
 * INCLUDES
 */
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

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

/*********************************************************************
 * LOCAL FUNCTIONS
 */

static gattCharCfg_t *gattServApp_FindCharCfgItem( uint16 connHandle,
                                                   gattCharCfg_t *charCfgTbl );
static bStatus_t gattServApp_SendNotiInd( uint16 connHandle, uint8 cccValue,
                                          uint8 authenticated, gattAttribute_t *pAttr,
                                          uint8 taskId, pfnGATTReadAttrCB_t pfnReadAttrCB );

/*********************************************************************
 * API FUNCTIONS
 */

/*********************************************************************
 * @fn      GATTServApp_InitCharCfg
 *
 * @brief   Initialize the client characteristic configuration table.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Reads/Writes of the Client
 *                Characteristic Configuration only only affect the
 *                configuration of that client.
 *
 * @param   connHandle - connection handle (0xFFFF for all connections).
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  none
 */
void GATTServApp_InitCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl )
{
  // Initialize Client Characteristic Configuration attributes
  if ( connHandle == LINKDB_CONNHANDLE_INVALID )
  {
    uint8 i;
    for ( i = 0; i < linkDBNumConns; i++ )
    {
      charCfgTbl[i].connHandle = LINKDB_CONNHANDLE_INVALID;
      charCfgTbl[i].value = GATT_CFG_NO_OPERATION;
    }
  }
  else
  {
    gattCharCfg_t *pItem = gattServApp_FindCharCfgItem( connHandle, charCfgTbl );
    if ( pItem != NULL )
    {
      pItem->connHandle = LINKDB_CONNHANDLE_INVALID;
      pItem->value = GATT_CFG_NO_OPERATION;
    }
  }
}

/*********************************************************************
 * @fn      GATTServApp_ProcessCharCfg
 *
 * @brief   Process Client Characteristic Configuration change.
 *
 * @param   charCfgTbl - characteristic configuration table.
 * @param   pValue - pointer to attribute value.
 * @param   authenticated - whether an authenticated link is required.
 * @param   attrTbl - attribute table.
 * @param   numAttrs - number of attributes in attribute table.
 * @param   taskId - task to be notified of confirmation.
 * @param   pfnReadAttrCB - read callback function pointer.
 *
 * @return  Success or Failure
 */
bStatus_t GATTServApp_ProcessCharCfg( gattCharCfg_t *charCfgTbl, uint8 *pValue,
                                      uint8 authenticated, gattAttribute_t *attrTbl,
                                      uint16 numAttrs, uint8 taskId,
                                      pfnGATTReadAttrCB_t pfnReadAttrCB )
{
  uint8 i;
  bStatus_t status = SUCCESS;

  // Verify input parameters
  if ( ( charCfgTbl == NULL ) || ( pValue == NULL ) ||
       ( attrTbl == NULL )    || ( pfnReadAttrCB == NULL ) )
  {
    return ( INVALIDPARAMETER );
  }

  for ( i = 0; i < linkDBNumConns; i++ )
  {
    gattCharCfg_t *pItem = &(charCfgTbl[i]);

    if ( ( pItem->connHandle != LINKDB_CONNHANDLE_INVALID ) &&
         ( pItem->value != GATT_CFG_NO_OPERATION ) )
    {
      gattAttribute_t *pAttr;

      // Find the characteristic value attribute
      pAttr = GATTServApp_FindAttr( attrTbl, numAttrs, pValue );
      if ( pAttr != NULL )
      {
        if ( pItem->value & GATT_CLIENT_CFG_NOTIFY )
        {
           status |= gattServApp_SendNotiInd( pItem->connHandle, GATT_CLIENT_CFG_NOTIFY,
                                              authenticated, pAttr, taskId, pfnReadAttrCB );
        }

        if ( pItem->value & GATT_CLIENT_CFG_INDICATE )
        {
           status |= gattServApp_SendNotiInd( pItem->connHandle, GATT_CLIENT_CFG_INDICATE,
                                              authenticated, pAttr, taskId, pfnReadAttrCB );
        }
      }
    }
  } // for

  return ( status );
}

/*********************************************************************
 * @fn          GATTServApp_FindAttr
 *
 * @brief       Find the attribute record within a service attribute
 *              table for a given attribute value pointer.
 *
 * @param       pAttrTbl - pointer to attribute table
 * @param       numAttrs - number of attributes in attribute table
 * @param       pValue - pointer to attribute value
 *
 * @return      Pointer to attribute record. NULL, if not found.
 */
gattAttribute_t *GATTServApp_FindAttr( gattAttribute_t *pAttrTbl,
                                       uint16 numAttrs, uint8 *pValue )
{
  uint16  i;
  for ( i = 0; i < numAttrs; i++ )
  {
    if ( pAttrTbl[i].pValue == pValue )
    {
      // Attribute record found
      return ( &(pAttrTbl[i]) );
    }
  }

  return ( (gattAttribute_t *)NULL );
}

/*********************************************************************
 * @fn      GATTServApp_ProcessCCCWriteReq
 *
 * @brief   Process the client characteristic configuration
 *          write request for a given client.
 *
 * @param   connHandle - connection message was received on
 * @param   pAttr - pointer to attribute
 * @param   pValue - pointer to data to be written
 * @param   len - length of data
 * @param   offset - offset of the first octet to be written
 * @param   validCfg - valid configuration
 *
 * @return  Success or Failure
 */
bStatus_t GATTServApp_ProcessCCCWriteReq( uint16 connHandle, gattAttribute_t *pAttr,
                                          uint8 *pValue, uint16 len, uint16 offset,
                                          uint16 validCfg )
{
  bStatus_t status = SUCCESS;

  // Validate the value
  if ( offset == 0 )
  {
    if ( len == 2 )
    {
      uint16 value = BUILD_UINT16( pValue[0], pValue[1] );

      // Validate characteristic configuration bit field
      if ( ( value & ~validCfg ) == 0 ) // indicate and/or notify
      {
        // Write the value if it's changed
        if ( GATTServApp_ReadCharCfg( connHandle,
                                      GATT_CCC_TBL(pAttr->pValue) ) != value )
        {
          status = GATTServApp_WriteCharCfg( connHandle,
                                             GATT_CCC_TBL(pAttr->pValue),
                                             value );
        }
      }
      else
      {
        status = ATT_ERR_INVALID_VALUE;
      }
    }
    else
    {
      status = ATT_ERR_INVALID_VALUE_SIZE;
    }
  }
  else
  {
    status = ATT_ERR_ATTR_NOT_LONG;
  }

  return ( status );
}

/*********************************************************************
 * @fn      GATTServApp_ReadCharCfg
 *
 * @brief   Read the client characteristic configuration for a given
 *          client.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Reads of the Client
 *                Characteristic Configuration only shows the configuration
 *                for that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 *
 * @return  attribute value
 */
uint16 GATTServApp_ReadCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl )
{
  gattCharCfg_t *pItem;

  pItem = gattServApp_FindCharCfgItem( connHandle, charCfgTbl );
  if ( pItem != NULL )
  {
    return ( (uint16)(pItem->value) );
  }

  return ( (uint16)GATT_CFG_NO_OPERATION );
}

/*********************************************************************
 * @fn      GATTServApp_WriteCharCfg
 *
 * @brief   Write the client characteristic configuration for a given
 *          client.
 *
 *          Note: Each client has its own instantiation of the Client
 *                Characteristic Configuration. Writes of the Client
 *                Characteristic Configuration only only affect the
 *                configuration of that client.
 *
 * @param   connHandle - connection handle.
 * @param   charCfgTbl - client characteristic configuration table.
 * @param   value - attribute new value.
 *
 * @return  Success or Failure
 */
uint8 GATTServApp_WriteCharCfg( uint16 connHandle, gattCharCfg_t *charCfgTbl,
                                uint16 value )
{
  gattCharCfg_t *pItem;

  pItem = gattServApp_FindCharCfgItem( connHandle, charCfgTbl );
  if ( pItem == NULL )
  {
    pItem = gattServApp_FindCharCfgItem( LINKDB_CONNHANDLE_INVALID, charCfgTbl );
    if ( pItem == NULL )
    {
      return ( ATT_ERR_INSUFFICIENT_RESOURCES );
    }

    pItem->connHandle = connHandle;
  }

  // Write the new value for this client
  pItem->value = value;

  return ( SUCCESS );
}

/*********************************************************************
 * @fn      gattServApp_FindCharCfgItem
 *
 * @brief   Find the characteristic configuration for a given client.
 *          Uses the connection handle to search the characteristic
 *          configuration table of a client.
 *
 * @param   connHandle - connection handle (0xFFFF for empty entry)
 * @param   charCfgTbl - characteristic configuration table.
 *
 * @return  pointer to the found item. NULL, otherwise.
 */
static gattCharCfg_t *gattServApp_FindCharCfgItem( uint16 connHandle,
                                                   gattCharCfg_t *charCfgTbl )
{
  uint8 i;

  for ( i = 0; i < linkDBNumConns; i++ )
  {
    if ( charCfgTbl[i].connHandle == connHandle )
    {
      // Entry found
      return ( &(charCfgTbl[i]) );
    }
  }

  return ( (gattCharCfg_t *)NULL );
}

 /*********************************************************************
 * @fn      gattServApp_SendNotiInd
 *
 * @brief   Send an ATT Notification/Indication.
 *
 * @param   connHandle - connection handle to use.
 * @param   cccValue - client characteristic configuration value.
 * @param   authenticated - whether an authenticated link is required.
 * @param   pAttr - pointer to attribute record.
 * @param   taskId - task to be notified of confirmation.
 * @param   pfnReadAttrCB - read callback function pointer.
 *
 * @return  Success or Failure
 */
static bStatus_t gattServApp_SendNotiInd( uint16 connHandle, uint8 cccValue,
                                          uint8 authenticated, gattAttribute_t *pAttr,
                                          uint8 taskId, pfnGATTReadAttrCB_t pfnReadAttrCB )
{
  attHandleValueNoti_t noti;
  uint16 len;
  bStatus_t status;

  // If the attribute value is longer than (ATT_MTU - 3) octets, then
  // only the first (ATT_MTU - 3) octets of this attributes value can
  // be sent in a notification.
  noti.pValue = (uint8 *)GATT_bm_alloc( connHandle, ATT_HANDLE_VALUE_NOTI,
                                        GATT_MAX_MTU, &len );
  if ( noti.pValue != NULL )
  {
    status = (*pfnReadAttrCB)( connHandle, pAttr, noti.pValue, &noti.len,
                               0, len, GATT_LOCAL_READ );
    if ( status == SUCCESS )
    {
      noti.handle = pAttr->handle;

      if ( cccValue & GATT_CLIENT_CFG_NOTIFY )
      {
        status = GATT_Notification( connHandle, &noti, authenticated );
      }
      else // GATT_CLIENT_CFG_INDICATE
      {
        status = GATT_Indication( connHandle, (attHandleValueInd_t *)&noti,
                                  authenticated, taskId );
      }
    }

    if ( status != SUCCESS )
    {
      GATT_bm_free( (gattMsg_t *)&noti, ATT_HANDLE_VALUE_NOTI );
    }
  }
  else
  {
    status = bleNoResources;
  }

  return ( status );
}


/****************************************************************************
****************************************************************************/
