/******************************************************************************

 @file  osal_snv_octp.c

 @brief This module contains the OSAL simple non-volatile memory functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2015-2022, Texas Instruments Incorporated
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

#include "hal_types.h"
#include "comdef.h"
#include "osal_snv.h"
#include "osal_snv_octp.h"

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * LOCAL VARIABLES
 */

// Pointers to NV functions
static NVINTF_nvFuncts_t *pOsalSnvOctpNvFcns = NULL;

// User of this module
static uint8_t osalSnvOctpSystemId;

/*********************************************************************
 * LOCAL FUNCTIONS
 */

/*********************************************************************
 * @fn      osal_snv_register
 *
 * @brief   Client registration for the NV service.
 *
 * @param   pFptrs - Pointer to a set of NV API function pointers
 * @param   systemId - System that is going to be using this service
 *
 * @return  NV_INTF_SUCCESS if registration succeeds. NV_INTF_FAIL otherwise.
 */
uint8_t osal_snv_register(NVINTF_nvFuncts_t *pFptrs, uint8_t systemId)
{
  uint8_t status;

  // Store parameters
  pOsalSnvOctpNvFcns = pFptrs;
  osalSnvOctpSystemId = systemId;

  // Make sure we have valid function pointers
  if (pOsalSnvOctpNvFcns->initNV &&
      pOsalSnvOctpNvFcns->writeItem &&
      pOsalSnvOctpNvFcns->writeItemEx &&
      pOsalSnvOctpNvFcns->readItem &&
      pOsalSnvOctpNvFcns->compactNV)
  {
    status = NVINTF_SUCCESS;
  }
  else
  {
    status = NVINTF_FAILURE;
  }

  return status;
}

/*********************************************************************
 * @fn      osal_snv_init
 *
 * @brief   Initialize NV service.
 *
 * @return  SUCCESS if initialization succeeds. FAILURE, otherwise.
 */
uint8_t osal_snv_init(void)
{
  return pOsalSnvOctpNvFcns->initNV(NULL);
}

/*********************************************************************
 * @fn      osal_snv_write
 *
 * @brief   Write a data item to NV.
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Length of data to write.
 * @param   *pBuf - Data to write.
 *
 * @return  SUCCESS if successful, NV_OPER_FAILED if failed.
 */
uint8_t osal_snv_write(osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  uint8_t status;
  NVINTF_itemID_t nvId;

  nvId.systemID = osalSnvOctpSystemId;
  nvId.itemID = (uint16_t)id;
  nvId.subID = 0;

  status = pOsalSnvOctpNvFcns->writeItem(nvId, len, pBuf);

  return status;
}

/*********************************************************************
 * @fn      osal_snv_ext_write
 *
 * @brief   Write a data item to NV.
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Length of data to write.
 * @param   *pBuf - Data to write.
 *
 * @return  SUCCESS if successful, NV_OPER_FAILED if failed.
 */
uint8_t osal_snv_ext_write(osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  uint8_t status;
  NVINTF_itemID_t nvId;

  nvId.systemID = osalSnvOctpSystemId;
  nvId.itemID = (uint16_t)id;
  nvId.subID = 0;

  status = pOsalSnvOctpNvFcns->writeItemEx(nvId, 0x00, len, pBuf);

  return status;
}

/*********************************************************************
 * @fn      osal_snv_read
 *
 * @brief   Read data from NV.
 *
 * @param   id  - Valid NV item Id.
 * @param   len - Length of data to read.
 * @param   *pBuf - Data is read into this buffer.
 *
 * @return  SUCCESS if successful.
 *          Otherwise, NV_OPER_FAILED for failure.
 */
uint8_t osal_snv_read(osalSnvId_t id, osalSnvLen_t len, void *pBuf)
{
  NVINTF_itemID_t nvId;

  nvId.systemID = osalSnvOctpSystemId;
  nvId.itemID = (uint16_t)id;
  nvId.subID = 0;

  return pOsalSnvOctpNvFcns->readItem(nvId, 0, len, pBuf);
}

/*********************************************************************
 * @fn      osal_snv_compact
 *
 * @brief   Compacts NV if its usage has reached a specific threshold.
 *
 * @param   threshold - compaction threshold
 *
 * @return  SUCCESS if successful,
 *          NV_OPER_FAILED if failed, or
 *          INVALIDPARAMETER if threshold invalid.
 */
uint8_t osal_snv_compact(uint8_t threshold)
{
  uint8_t status;

  if ((threshold < OSAL_NV_MIN_COMPACT_THRESHOLD) ||
      (threshold > OSAL_NV_MAX_COMPACT_THRESHOLD))
  {
    status = NVINTF_BADPARAM;
  }
  else
  {
    status = pOsalSnvOctpNvFcns->compactNV(threshold);
  }

  return status;
}

/*********************************************************************
*********************************************************************/
