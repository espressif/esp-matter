/*
 * Copyright (c) 2017, 2020-2021, Qorvo Inc
 *
 *
 * This software is owned by Qorvo Inc
 * and protected under applicable copyright laws.
 * It is delivered under the terms of the license
 * and is intended and supplied for use solely and
 * exclusively with products manufactured by
 * Qorvo Inc.
 *
 *
 * THIS SOFTWARE IS PROVIDED IN AN "AS IS"
 * CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT
 * LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * QORVO INC. SHALL NOT, IN ANY
 * CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
 * INCIDENTAL OR CONSEQUENTIAL DAMAGES,
 * FOR ANY REASON WHATSOEVER.
 *
 *
 * $Header$
 * $Change$
 * $DateTime$
 *
 */

/*****************************************************************************
 *                    Includes Definitions
 *****************************************************************************/

#define GP_COMPONENT_ID GP_COMPONENT_ID_SILEXCRYPTOSOC

// General includes
#include "gpJumpTables.h"
#include "gpAssert.h"

// Crypto lib includes
#include <stddef.h>
#include "cryptolib_types.h"
#include "cryptolib_def.h"
#include "cryptodma.h"

/*****************************************************************************
 *                    NRT ROM patch fix version numbers
 *****************************************************************************/
/* Rom versions where patch for the corresponding function is already included
 * while building ROM image (so no patch required when application is built with
 * the specified ROM version)
 */
#if   defined(GP_DIVERSITY_GPHAL_K8E)
#define ROMVERSION_FIXFORPATCH_CRYPTO_GENERATE_CCM_HEADER 2
#define ROMVERSION_FIXFORPATCH_CRYPTO_BA414E_SET_CONFIG 2
#endif

/*****************************************************************************
 *                    ROM Function prototypes
 *****************************************************************************/
uint32_t generate_ccm_header_orgrom(block_t nonce, uint32_t aad_len, uint32_t data_len,uint32_t tag_len, block_t *header);

/*****************************************************************************
 *                    Macro Definitions
 *****************************************************************************/


/*****************************************************************************
 *                    Patched Function(s)
 *****************************************************************************/

#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_GENERATE_CCM_HEADER)
/**
 * @brief block cipher generate CCM header
 *    Create the header of CCM data based on lengths, nonce & aad2
 *
 * @param nonce      The Nonce
 * @param aad_len    The length of additional data to authenticate
 * @param data_len   The length of the data (plaintext or cipher) in bytes
 * @param tag_len    The length of the MAC in bytes
 * @param header     Pointer to the output header block
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
#define BLK_CIPHER_MAC_SIZE 16

uint32_t generate_ccm_header_patched(block_t nonce, uint32_t aad_len, uint32_t data_len,
      uint32_t tag_len, block_t *header)
{
   uint32_t flags;
   uint32_t m, l;
   uint32_t i;

   /* RFC3610 paragraph 2.2 defines the formatting of the first block.
    * Thee first block contains:
    *  byte  [0]           the flags byte (see below)
    *  bytes [1,1+nonce.len]   nonce
    *  bytes [2+nonce.len, 16] message length
    *
    *  The flags byte has the following bit fields:
    *    [6:7] = 0
    *    [3:5] = authentication tag size, encoded as (tag_len-2)/2
    *              only multiples of 2 between 2 and 16 are allowed.
    *    [0:2] = length field size minus 1. Is the same as (15 - nonce.len - 1)
    *         between 2 and 8.
    **/

   /* Verify input parameters
    *
    * Variant of CCM with disabled authenticity is supported: CCM*
    * CCM* MAC length is one of {0,4,6,8,10,12,14,16}, see
    * https://urldefense.com/v3/__https://tools.ietf.org/html/rfc3610__;!!LUcoR2A!eZMPhKpqTNe2yX12-vo9CceiOLDrcLCIq4Uc2AzL7rhdW-u1B1MopGSANaoZnl7CS-biww$  for CCM and
    * B.4 Specification of generic CCM* mode of operation from
    * https://urldefense.com/v3/__http://ecee.colorado.edu/ *liue/teaching/comm_standards/2015S_zigbee/802.15.4-2011.pdf__;fg!!LUcoR2A!eZMPhKpqTNe2yX12-vo9CceiOLDrcLCIq4Uc2AzL7rhdW-u1B1MopGSANaoZnl6DuYGoiw$
    * for CCM*
    */
   if ((tag_len & 1) || (tag_len == 2) || (tag_len > BLK_CIPHER_MAC_SIZE))
      return CRYPTOLIB_INVALID_PARAM;

   /* The conditions below translate into (l > 8) || (l < 2) which match the
    * specification "The parameter L can take on the values from 2 to 8"
    */
   if ((nonce.len < 7) || (nonce.len > 13))
      return CRYPTOLIB_INVALID_PARAM;

   flags = (aad_len > 0) ? (1 << 6) : 0;
   /* Authentication tag size encoding. A tag_len = 0 is accepted by CCM* and
    * that will be encoded as a 0. */
   m = (tag_len > 0) ? (tag_len-2)/2 : 0;
   flags |= (m & 0x7) << 3;
   l = 15 - nonce.len;
   flags |= ((l-1) & 0x7);
   header->addr[0] = flags;

   /* If l >= 4, data_len will always fit as it is an uint32_t, we need to
    * check only in case l < 4: */
   if (l < 4U && data_len >= (1U << (l * 8))) {
      /* message too long to encode the size in the CCM header */
      return CRYPTOLIB_INVALID_PARAM;
   }

   memcpy_blkIn(&header->addr[1], nonce, nonce.len);

   /* append message length in big endian format encoded l octets*/
   memset(&(header->addr[1 + nonce.len]), 0, l);
   for (i = 0; i < sizeof(data_len); i++) {
      if (data_len >= (1U << (i * 8)))
         header->addr[15 - i] = (data_len >> (i * 8)) & 0xff;
      else
         break;
   }

   /* if there's additional authentication data (or aad2),
    * encode the size:
    *
    * 0 < aad_len < 0xFF00     => 2 bytes in big endian format.
    * 0xFF00 < aad_len < 2^32  => 0xff, 0xfe, and four bytes in big endian format.
    * eSecure currently does not support sizes bigger than 2^32.
    */
   if (aad_len > 0) {
      if (aad_len < 0xFF00) {
         header->addr[16] = aad_len >> 8;
         header->addr[17] = aad_len & 0xff;
         header->len = 18;
      } else {
         // TODO: Test following case
         header->addr[16] = 0xFF;
         header->addr[17] = 0xFE;
         header->addr[18] = aad_len >> 24;
         header->addr[19] = (aad_len >> 16) & 0xff;
         header->addr[20] = (aad_len >> 8) & 0xff;
         header->addr[21] = aad_len & 0xff;
         header->len = 22;
      }
   } else {
      header->len = 16;
   }

   return CRYPTOLIB_SUCCESS;
}
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_GENERATE_CCM_HEADER)

#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_BA414E_SET_CONFIG)
typedef void (*ba414e_set_config_ptr_t)(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN);

void ba414e_set_config_patched(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN)
{
    ba414e_set_config_ptr_t p_ba414e_set_config;
    UInt8 romVersion = gpJumpTables_GetRomVersion();

    //adding +1 to function pointer for Thumb instruction
    switch (romVersion)
    {
#if   defined(GP_DIVERSITY_GPHAL_K8E)
        case 1: {
            p_ba414e_set_config = (ba414e_set_config_ptr_t)(0x480df1e + 1);
            break;
        }
#endif // defined(GP_DIVERSITY_GPHAL_K8E)
        default: {
            GP_ASSERT_DEV_INT(0);
            return;
        }
    }

    // Call ROM function
    p_ba414e_set_config(PtrA, PtrB, PtrC, PtrN);
}
#endif //(GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_BA414E_SET_CONFIG)

/*****************************************************************************
 *                    Patch Function Multiplexer(s)
 *****************************************************************************/

#if defined(GP_ROM_PATCHED_generate_ccm_header)
uint32_t generate_ccm_header(block_t nonce, uint32_t aad_len, uint32_t data_len,uint32_t tag_len, block_t *header)
{
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_GENERATE_CCM_HEADER)
    if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_CRYPTO_GENERATE_CCM_HEADER)
    {
        return generate_ccm_header_patched(nonce, aad_len, data_len, tag_len, header);
    }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_GENERATE_CCM_HEADER)
    return generate_ccm_header_orgrom(nonce, aad_len, data_len, tag_len, header);
}
#endif //GP_ROM_PATCHED_generate_ccm_header

#if defined(GP_ROM_PATCHED_ba414e_set_config)
#if (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_BA414E_SET_CONFIG)
void ba414e_set_config(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN)
{
    if(gpJumpTables_GetRomVersion() < ROMVERSION_FIXFORPATCH_CRYPTO_BA414E_SET_CONFIG)
    {
        ba414e_set_config_patched(PtrA, PtrB, PtrC, PtrN);
    }
    // Not in any ROM table
    //ba414e_set_config_orgrom(PtrA, PtrB, PtrC, PtrN);
 }
#endif // (GPJUMPTABLES_MIN_ROMVERSION < ROMVERSION_FIXFORPATCH_CRYPTO_BA414E_SET_CONFIG)
#endif // GP_ROM_PATCHED_ba414e_set_config
