/******************************************************************************

 @file  osal_snv_wrapper.c

 @brief This module defines the OSAL simple non-volatile memory functions as a
        wrapper to NVOCMP(NV On Chip multi Page)

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2018-2022, Texas Instruments Incorporated
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
#include "icall_user_config.h"
#include "osal_snv.h"
#include "nvintf.h"
#include "nvocmp.h"

#ifndef SYSTEM_ID
#define SYSTEM_ID NVINTF_SYSID_BLE
#endif

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE 0x2000
#endif

#if !defined(RTLS_PASSIVE) && !defined(RTLS_CONNECTION_MONITOR)
// Get nvintf function pointer struct from global driver table
extern const drvTblPtr_t driverTable;
#endif

NVINTF_nvFuncts_t nvFptrs;

// Convert a threshold percentage to bytes.
#define THRESHOLD2BYTES(x) ((FLASH_PAGE_SIZE) - (((FLASH_PAGE_SIZE)*(x))/100))

/*********************************************************************
 * @fn      osal_snv_init
 *
 * @brief   Initialize NV service.
 *
 * @param   none
 *
 * @return  NVINTF_SUCCESS or other nvintf error code.
 */
uint8 osal_snv_init( void )
{
#if !defined(RTLS_PASSIVE) && !defined(RTLS_CONNECTION_MONITOR)
  // Get function pointers from driver table
  nvFptrs = *((NVINTF_nvFuncts_t *)(driverTable.nvintfStructPtr));
#endif

  NVOCMP_loadApiPtrsMin(&nvFptrs);
  return nvFptrs.initNV(NULL);
}

/*********************************************************************
 * @fn      osal_snv_read
 *
 * @brief   Read data from NV.
 *
 * @param   id   - Valid NV item Id.
 * @param   len  - Length of data to read.
 * @param   *pBuf - Data is read into this buffer.
 *
 * @return  NVINTF_SUCCESS or other nvintf error code.
 */
uint8 osal_snv_read( osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  NVINTF_itemID_t nv_id;
  nv_id.itemID   = id;
  nv_id.subID    = 0;
  nv_id.systemID = SYSTEM_ID;

  return nvFptrs.readItem(nv_id, 0, len, pBuf);
}

/*********************************************************************
 * @fn      osal_snv_write
 *
 * @brief   Write a data item to NV.
 *
 * @param   id   - Valid NV item Id.
 * @param   len  - Length of data to write.
 * @param   *pBuf - Data to write.
 *
 * @return  NVINTF_SUCCESS or other nvintf error code.
 */
uint8 osal_snv_write( osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  NVINTF_itemID_t nv_id;
  nv_id.itemID   = id;
  nv_id.subID    = 0;
  nv_id.systemID = SYSTEM_ID;

  return nvFptrs.writeItem(nv_id, len, pBuf);
}

/*********************************************************************
 * @fn      osal_snv_compact
 *
 * @brief   Compacts NV if its usage has reached a specific threshold.
 *
 * @param   threshold - compaction threshold.
 *
 * @return  NVINTF_SUCCESS if successful,
 *          NVINTF_FAILURE if failed, or
 *          NVINTF_BADPARAM if threshold invalid.
 */
uint8 osal_snv_compact( uint8 threshold )
{
  // convert percentage to approximate byte threshold.
  if (threshold <= 100)
  {
    return nvFptrs.compactNV(THRESHOLD2BYTES(threshold));
  }

  return NVINTF_BADPARAM;
}
