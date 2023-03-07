/******************************************************************************

 @file  osal_nv.c

 @brief This module contains the OSAL non-volatile memory functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2014-2022, Texas Instruments Incorporated
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

/******************************************************************************
 * INCLUDES
 */

#include "hal_board_cfg.h"
#include "osal_nv.h"
#include "ZComDef.h"
#include "nwk_globals.h"
#include "APSMEDE.h"

#include "zstackconfig.h"


/******************************************************************************
 * CONSTANTS
 */


/******************************************************************************
 * MACROS
 */


/******************************************************************************
 * TYPEDEFS
 */


/******************************************************************************
 * EXTERNAL VARIABLES
 */
extern zstack_Config_t *pZStackCfg;

/******************************************************************************
 * LOCAL VARIABLES
 */


/******************************************************************************
 * LOCAL FUNCTIONS
 */


/******************************************************************************
 * @fn      osal_nv_init
 *
 * @brief   Initialize NV service.
 *
 * @param   p - Not used.
 *
 * @return  none
 */
void osal_nv_init( void *p )
{
  if ( pZStackCfg && pZStackCfg->nvFps.initNV )
  {
    pZStackCfg->nvFps.initNV( NULL );
  }
}

/******************************************************************************
 * @fn      osal_nv_item_init_ex
 *
 * @brief   If the NV item does not already exist, it is created and
 *          initialized with the data passed to the function, if any.
 *          This function must be called before calling osal_nv_read() or
 *          osal_nv_write().
 *
 * @param   id  - Valid NV item Id.
 * @param   subId - Valid NV item sub Id.
 * @param   len - Item length.
 * @param  *buf - Pointer to item initalization data. Set to NULL if none.
 *
 * @return  NV_ITEM_UNINIT - Id did not exist and was created successfully.
 *          SUCCESS       - Id already existed, no action taken.
 *          NV_OPER_FAILED - Failure to find or create Id.
 */
uint8 osal_nv_item_init_ex( uint16 id, uint16 subId, uint16 len, void *buf )
{
  if ( pZStackCfg && pZStackCfg->nvFps.createItem )
  {
    uint32_t nvLen = 0;
    NVINTF_itemID_t nvId;

    nvId.systemID = NVINTF_SYSID_ZSTACK;
    nvId.itemID = (uint16_t)id;
    nvId.subID = (uint16_t)subId;

    if ( pZStackCfg->nvFps.getItemLen )
    {
      nvLen = pZStackCfg->nvFps.getItemLen( nvId );
    }

    if ( nvLen == len )
    {
      // Already exists and length is good
      return ( SUCCESS );
    }

    if ( pZStackCfg->nvFps.createItem( nvId, len, buf ) == NVINTF_FAILURE )
    {
      // Operation failed
      return ( NV_OPER_FAILED );
    }
  }

  // NV was created
  return ( NV_ITEM_UNINIT );
}

/******************************************************************************
 * @fn      osal_nv_item_init
 *
 * @brief   If the NV item does not already exist, it is created and
 *          initialized with the data passed to the function, if any.
 *          This function must be called before calling osal_nv_read() or
 *          osal_nv_write().
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Item length.
 * @param  *buf - Pointer to item initalization data. Set to NULL if none.
 *
 * @return  NV_ITEM_UNINIT - Id did not exist and was created successfully.
 *          SUCCESS       - Id already existed, no action taken.
 *          NV_OPER_FAILED - Failure to find or create Id.
 */
uint8 osal_nv_item_init( uint16 id, uint16 len, void *buf )
{
  // Legacy items use ZCD_NV_EX_LEGACY as the ID and the id as the sub-id.
  return ( osal_nv_item_init_ex( ZCD_NV_EX_LEGACY, id, len, buf ) );
}

/******************************************************************************
 * @fn      osal_nv_item_len_ex
 *
 * @brief   Get the data length of the item stored in NV memory.
 *
 * @param   id  - Valid NV item Id.
 * @param   subId - Valid NV item sub Id.
 *
 * @return  Item length, if found; zero otherwise.
 */
uint16 osal_nv_item_len_ex( uint16 id, uint16 subId  )
{
  uint16 nvLen = 0;

  if ( pZStackCfg && pZStackCfg->nvFps.getItemLen )
  {
    NVINTF_itemID_t nvId;

    nvId.systemID = NVINTF_SYSID_ZSTACK;
    nvId.itemID = (uint16_t)id;
    nvId.subID = (uint16_t)subId;

    nvLen = (uint16)pZStackCfg->nvFps.getItemLen( nvId );
  }
  return ( nvLen );
}

/******************************************************************************
 * @fn      osal_nv_item_len
 *
 * @brief   Get the data length of the item stored in NV memory.
 *
 * @param   id  - Valid NV item Id.
 *
 * @return  Item length, if found; zero otherwise.
 */
uint16 osal_nv_item_len( uint16 id )
{
  // Legacy items use ZCD_NV_EX_LEGACY as the ID and the id as the sub-id.
  return ( osal_nv_item_len_ex( ZCD_NV_EX_LEGACY, id ) );
}

/******************************************************************************
 * @fn      osal_nv_write_ex
 *
 * @brief   Write a data item to NV. Function can write an entire item to NV or
 *          an element of an item by indexing into the item with an offset.
 *
 * @param   id  - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   subId - Valid NV item sub Id.
 * @param   len - Length of data to write.
 * @param  *buf - Data to write.
 *
 * @return  SUCCESS if successful, NV_ITEM_UNINIT if item did not
 *          exist in NV and offset is non-zero, NV_OPER_FAILED if failure.
 */
uint8 osal_nv_write_ex( uint16 id, uint16 subId, uint16 ndx, uint16 len, void *buf )
{
  uint8 rtrn = SUCCESS;

  if ( pZStackCfg && pZStackCfg->nvFps.writeItemEx )
  {
    uint32 nvLen = 0;
    NVINTF_itemID_t nvId;

    nvId.systemID = NVINTF_SYSID_ZSTACK;
    nvId.itemID = (uint16_t)id;
    nvId.subID = (uint16_t)subId;

    if ( pZStackCfg->nvFps.getItemLen )
    {
      nvLen = pZStackCfg->nvFps.getItemLen( nvId );
    }

    if ( nvLen > 0 )
    {
      if ( pZStackCfg->nvFps.writeItemEx( nvId, ndx, len, buf ) == NVINTF_FAILURE )
      {
        rtrn = NV_OPER_FAILED;
      }
    }
    else
    {
      rtrn = NV_ITEM_UNINIT;
    }
  }

  return rtrn;
}

/******************************************************************************
 * @fn      osal_nv_write
 *
 * @brief   Write a data item to NV. Function can write an entire item to NV or
 *          an element of an item by indexing into the item with an offset.
 *
 * @param   id  - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   len - Length of data to write.
 * @param  *buf - Data to write.
 *
 * @return  SUCCESS if successful, NV_ITEM_UNINIT if item did not
 *          exist in NV and offset is non-zero, NV_OPER_FAILED if failure.
 */
uint8 osal_nv_write( uint16 id, uint16 ndx, uint16 len, void *buf )
{
  // Legacy items use ZCD_NV_EX_LEGACY as the ID and the id as the sub-id.
  return ( osal_nv_write_ex( ZCD_NV_EX_LEGACY, id, ndx, len, buf ) );
}

/******************************************************************************
 * @fn      osal_nv_read_ex
 *
 * @brief   Read data from NV. This function can be used to read an entire item from NV or
 *          an element of an item by indexing into the item with an offset.
 *          Read data is copied into *buf.
 *
 * @param   id     - Valid NV item Id.
 * @param   subId - Valid NV item sub Id.
 * @param   ndx - Index offset into item
 * @param   len    - Length of data to read.
 * @param   *buf  - Data is read into this buffer.
 *
 * @return  SUCCESS if NV data was copied to the parameter 'buf'.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8 osal_nv_read_ex( uint16 id, uint16 subId, uint16 ndx, uint16 len, void *buf )
{
  if ( pZStackCfg && pZStackCfg->nvFps.readItem )
  {
    NVINTF_itemID_t nvId;

    nvId.systemID = NVINTF_SYSID_ZSTACK;
    nvId.itemID = (uint16_t)id;
    nvId.subID = (uint16_t)subId;

    if ( pZStackCfg->nvFps.readItem( nvId, ndx, len, buf ) == NVINTF_FAILURE )
    {
      return ( NV_OPER_FAILED );
    }
    else
    {
      return ( SUCCESS );
    }
  }
  else
  {
    if ( id == ZCD_NV_EX_LEGACY )
    {
      // Dummy information
      if ( subId == ZCD_NV_NWK_ACTIVE_KEY_INFO && buf )
      {
        nwkKeyDesc *pKey = buf;
        pKey->keySeqNum = 0;
        if ( len > SEC_KEY_LEN )
        {
          osal_memcpy( pKey->key, defaultKey, SEC_KEY_LEN );
        }
      }
      else if ( subId == ZCD_NV_NWKKEY )
      {
        return ( NV_OPER_FAILED );
      }
      else if ( subId == ZCD_NV_TCLK_TABLE_START )
      {
        APSME_TCLinkKey_t *pPtr = (APSME_TCLinkKey_t *)buf;

        if ( len == SEC_KEY_LEN )
        {
          osal_memcpy( buf, defaultTCLinkKey, SEC_KEY_LEN );
        }
        else if ( len == sizeof( APSME_TCLinkKey_t ) )
        {
          osal_memset( pPtr->extAddr, 0xFF, Z_EXTADDR_LEN );
          osal_memcpy( pPtr->key, defaultTCLinkKey, SEC_KEY_LEN );
          pPtr->txFrmCntr = 0;
          pPtr->rxFrmCntr = 0;
        }
      }
    }
    return ( SUCCESS );
  }
}

/******************************************************************************
 * @fn      osal_nv_read
 *
 * @brief   Read data from NV. This function can be used to read an entire item from NV or
 *          an element of an item by indexing into the item with an offset.
 *          Read data is copied into *buf.
 *
 * @param   id     - Valid NV item Id.
 * @param   ndx - Index offset into item
 * @param   len    - Length of data to read.
 * @param   *buf  - Data is read into this buffer.
 *
 * @return  SUCCESS if NV data was copied to the parameter 'buf'.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8 osal_nv_read( uint16 id, uint16 ndx, uint16 len, void *buf )
{
  // Legacy items use ZCD_NV_EX_LEGACY as the ID and the id as the sub-id.
  return ( osal_nv_read_ex( ZCD_NV_EX_LEGACY, id, ndx, len, buf ) );
}

/******************************************************************************
 * @fn      osal_nv_delete_ex
 *
 * @brief   Delete item from NV. This function will fail if the length
 *          parameter does not match the length of the item in NV.
 *
 * @param   id  - Valid NV item Id.
 * @param   subId - Valid NV item sub Id.
 * @param   len - Length of item to delete.
 *
 * @return  SUCCESS if item was deleted,
 *          NV_ITEM_UNINIT if item did not exist in NV,
 *          NV_BAD_ITEM_LEN if length parameter not correct,
 *          NV_OPER_FAILED if attempted deletion failed.
 */
uint8 osal_nv_delete_ex( uint16 id, uint16 subId, uint16 len )
{
  uint8 ret = SUCCESS;

  if ( pZStackCfg && pZStackCfg->nvFps.deleteItem )
  {
    uint32 nvLen = 0;
    NVINTF_itemID_t nvId;

    nvId.systemID = NVINTF_SYSID_ZSTACK;
    nvId.itemID = (uint16_t)id;
    nvId.subID = (uint16_t)subId;

    if ( pZStackCfg->nvFps.getItemLen )
    {
      nvLen = pZStackCfg->nvFps.getItemLen( nvId );
    }

    if ( nvLen == 0 )
    {
      ret = NV_ITEM_UNINIT;
    }
    else if ( nvLen != len )
    {
      ret = NV_BAD_ITEM_LEN;
    }
    else if ( pZStackCfg->nvFps.deleteItem( nvId ) == NVINTF_FAILURE )
    {
      ret = NV_OPER_FAILED;
    }
  }

  return ( ret );
}

/******************************************************************************
 * @fn      osal_nv_delete
 *
 * @brief   Delete item from NV. This function will fail if the length
 *          parameter does not match the length of the item in NV.
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Length of item to delete.
 *
 * @return  SUCCESS if item was deleted,
 *          NV_ITEM_UNINIT if item did not exist in NV,
 *          NV_BAD_ITEM_LEN if length parameter not correct,
 *          NV_OPER_FAILED if attempted deletion failed.
 */
uint8 osal_nv_delete( uint16 id, uint16 len )
{
  // Legacy items use ZCD_NV_EX_LEGACY as the ID and the id as the sub-id.
  return ( osal_nv_delete_ex( ZCD_NV_EX_LEGACY, id, len ) );
}

/*********************************************************************
 */
