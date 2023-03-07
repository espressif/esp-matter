/******************************************************************************

 @file  saddr.c

 @brief Zigbee and 802.15.4 device address utility functions.

 Group: WCS, LPC, BTS
 Target Device: cc13xx_cc26xx

 ******************************************************************************
 
 Copyright (c) 2005-2022, Texas Instruments Incorporated
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

/****************************************************************************
 * INCLUDES
 */
#include "hal_types.h"
#include "osal.h"
#include "saddr.h"

/****************************************************************************
 * @fn          sAddrCmp
 *
 * @brief       Compare two device addresses.
 *
 * input parameters
 *
 * @param       pAddr1        - Pointer to first address.
 * @param       pAddr2        - Pointer to second address.
 *
 * output parameters
 *
 * @return      TRUE if addresses are equal, FALSE otherwise
 */
bool sAddrCmp(const sAddr_t *pAddr1, const sAddr_t *pAddr2)
{
  if (pAddr1->addrMode != pAddr2->addrMode)
  {
    return FALSE;
  }
  else if (pAddr1->addrMode == SADDR_MODE_NONE)
  {
    return FALSE;
  }
  else if (pAddr1->addrMode == SADDR_MODE_SHORT)
  {
    return (bool) (pAddr1->addr.shortAddr == pAddr2->addr.shortAddr);
  }
  else if (pAddr1->addrMode == SADDR_MODE_EXT)
  {
    return (sAddrExtCmp(pAddr1->addr.extAddr, pAddr2->addr.extAddr));
  }
  else
  {
    return FALSE;
  }
}

/****************************************************************************
 * @fn          sAddrIden
 *
 * @brief       Check if two device addresses are identical.
 *
 *              This routine is virtually the same as sAddrCmp, which is used
 *              to determine if two different addresses are the same. However,
 *              this routine can be used to determine if an address is the
 *              same as a previously stored address. The key difference is in
 *              the former case, if the address mode is "none", then the
 *              assumption is that the two addresses can not be the same. But
 *              in the latter case, the address mode itself is being compared.
 *              So two addresses can be identical even if the address mode is
 *              "none", as long as the address mode of both addresses being
 *              compared is "none".
 *
 * input parameters
 *
 * @param       pAddr1        - Pointer to first address.
 * @param       pAddr2        - Pointer to second address.
 *
 * output parameters
 *
 * @return      TRUE if addresses are identical, FALSE otherwise
 */
bool sAddrIden(const sAddr_t *pAddr1, const sAddr_t *pAddr2)
{
  // first check if the address modes are the same
  if (pAddr1->addrMode != pAddr2->addrMode)
  {
    // no, so no point in comparing any further
    return FALSE;
  }
  // the address modes are the same; check if there is no address
  else if (pAddr1->addrMode == SADDR_MODE_NONE)
  {
    // no address, so no need to compare any further as both addresses have the
    // same address mode but no address, so they are identical
    return TRUE;
  }
  // there's an address; check if it is short
  else if (pAddr1->addrMode == SADDR_MODE_SHORT)
  {
    // compare short addresses
    return (bool) (pAddr1->addr.shortAddr == pAddr2->addr.shortAddr);
  }
  // there's an address; check if it is extended
  else if (pAddr1->addrMode == SADDR_MODE_EXT)
  {
    // compare extended addresses
    return (sAddrExtCmp(pAddr1->addr.extAddr, pAddr2->addr.extAddr));
  }
  else // unknown error
  {
    return FALSE;
  }
}

/****************************************************************************
 * @fn          sAddrCpy
 *
 * @brief       Copy a device address.
 *
 * input parameters
 *
 * @param       pSrc         - Pointer to address to copy.
 *
 * output parameters
 *
 * @param       pDest        - Pointer to address of copy.
 *
 * @return      None.
 */
void sAddrCpy(sAddr_t *pDest, const sAddr_t *pSrc)
{
  pDest->addrMode = pSrc->addrMode;

  if (pDest->addrMode == SADDR_MODE_EXT)
  {
    sAddrExtCpy(pDest->addr.extAddr, pSrc->addr.extAddr);
  }
  else
  {
    pDest->addr.shortAddr = pSrc->addr.shortAddr;
  }
}

/****************************************************************************
 * @fn          sAddrExtCmp
 *
 * @brief       Compare two extended addresses.
 *
 * input parameters
 *
 * @param       pAddr1        - Pointer to first address.
 * @param       pAddr2        - Pointer to second address.
 *
 * output parameters
 *
 * @return      TRUE if addresses are equal, FALSE otherwise
 */
bool sAddrExtCmp(const uint8 * pAddr1, const uint8 * pAddr2)
{
  uint8 i;

  for (i = SADDR_EXT_LEN; i != 0; i--)
  {
    if (*pAddr1++ != *pAddr2++)
    {
      return FALSE;
    }
  }
  return TRUE;
}

/****************************************************************************
 * @fn          sAddrExtCpy
 *
 * @brief       Copy an extended address.
 *
 * input parameters
 *
 * @param       pSrc         - Pointer to address to copy.
 *
 * output parameters
 *
 * @param       pDest        - Pointer to address of copy.
 *
 * @return      pDest + SADDR_EXT_LEN.
 */
void *sAddrExtCpy(uint8 * pDest, const uint8 * pSrc)
{
  return osal_memcpy(pDest, pSrc, SADDR_EXT_LEN);
}




