
#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA411E AES
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include <stddef.h>
#include "cryptolib_def.h" // cryptolib_def.h needs to be included before other header files.
#include "sx_aes.h"
#include "cryptodma.h"
#include "memcmp.h"
#include "ba411e_config.h"
#include "sx_math.h"
#include "sx_rng.h"
#include <stdbool.h>

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "gpJumpTables_DataTable.h"
#endif
#include "silexCryptoSoc_Defs.h"

#if AES_HW_KEYS_ENABLED
uint8_t aes_hw_key1;
uint8_t aes_hw_key2;
#endif //AES_HW_KEYS_ENABLED


/**
 * @brief AES set mode function
 *    Set the configuration of the mode register to the required value.
 *
 * @param aes_fct mode of operation for AES. See ::sx_aes_fct_t.
 * @param dir encrypt or decrypt. See ::sx_aes_mode_t.
 * @param ctx none, save or load context. See ::sx_aes_ctx_t.
 * @param key input key. Expect len to be 16, 24 or 32.
 * @param mode_out pointer to the mode register value. A 32-bit word will be written.
 */
STATIC_FUNC uint32_t sw_aes_setmode(sx_aes_fct_t aes_fct, sx_aes_mode_t dir, sx_aes_ctx_t ctx, block_t key, uint32_t *mode_out)
{
   *mode_out = 0;
   // AES mode of operation
    switch(aes_fct){
        case ECB:
            *mode_out  |= AES_MODEID_ECB;
            break;
        #if AES_CBC_ENABLED
        case CBC:
            *mode_out  |= AES_MODEID_CBC;
            break;
        #endif
        #if AES_CTR_ENABLED
        case CTR:
            *mode_out  |= AES_MODEID_CTR;
            break;
        #endif
        #if AES_CFB_ENABLED
        case CFB:
            *mode_out  |= AES_MODEID_CFB;
            break;
        #endif
        #if AES_OFB_ENABLED
        case OFB:
            *mode_out  |= AES_MODEID_OFB;
            break;
        #endif
        #if AES_CCM_ENABLED
        case CCM:
            *mode_out  |= AES_MODEID_CCM;
            break;
        #endif
        #if AES_GCM_ENABLED
        case GCM:
            *mode_out  |= AES_MODEID_GCM;
            break;
        #endif
        #if AES_XTS_ENABLED
        case XTS:
            *mode_out  |= AES_MODEID_XTS;
            break;
        #endif
        #if AES_CMA_ENABLED
        case CMAC:
            *mode_out  |= AES_MODEID_CMA;
            break;
        #endif
        default: // Should not arrive, checked in sx_aes_validate_input
            return CRYPTOLIB_UNSUPPORTED_ERR;
    }

    // context mode
    switch(ctx){
        case CTX_WHOLE:
            *mode_out  |= AES_MODEID_NO_CX;
            break;
#if AES_PARTIAL_ENABLED
        case CTX_BEGIN:
            *mode_out  |= AES_MODEID_CX_SAVE;
            break;
        case CTX_END:
            *mode_out  |= AES_MODEID_CX_LOAD;
            break;
        case CTX_MIDDLE:
            *mode_out  |= AES_MODEID_CX_SAVE;
            *mode_out  |= AES_MODEID_CX_LOAD;
            break;
#endif
        default: // Should not arrive, checked in sx_aes_validate_input
            return CRYPTOLIB_UNSUPPORTED_ERR;
    }

    // direction
    switch(dir){
        case ENC:
            *mode_out  |= AES_MODEID_ENCRYPT;
            break;
        case DEC:
            *mode_out  |= AES_MODEID_DECRYPT;
            break;
        default: // Should not arrive, checked in sx_aes_validate_input
            return CRYPTOLIB_UNSUPPORTED_ERR;
    }

    // keysize
    switch(key.len){
        case 16:
            *mode_out  |= AES_MODEID_AES128;
            break;
#if defined(GP_DIVERSITY_GPHAL_K8E)
/* 192-bit AES keys not supported on K8E */
#else
        case 24:
            *mode_out  |= AES_MODEID_AES192;
            break;
#endif
        case 32:
            *mode_out  |= AES_MODEID_AES256;
            break;
        default: // Should not arrive, checked in sx_aes_validate_input
            return CRYPTOLIB_UNSUPPORTED_ERR;
    }

#if AES_HW_KEYS_ENABLED
    // hardware keys
    if (key.addr==&aes_hw_key1) {
        *mode_out  |= AES_MODEID_KEY1;
    } else if (key.addr==&aes_hw_key2) {
        *mode_out  |= AES_MODEID_KEY2;
    }
#endif //AES_HW_KEYS_ENABLED
    return CRYPTOLIB_SUCCESS;
}

/**
 * @brief AES generate CCM header
 *    Create the header of CCM data based on lengths, nonce & aad2
 *
 * @param nonce      The Nonce
 * @param aad_len    The length of additional data to authentify
 * @param data_len   The length of the data (plaintext or cipher) in bytes
 * @param tag_len    The length of the MAC in bytes
 * @param header     Pointer to the output header block
 * @return CRYPTOLIB_SUCCESS if execution was successful
 */
#if AES_CCM_ENABLED
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

uint32_t generate_ccm_header(block_t nonce, uint32_t aad_len, uint32_t data_len,
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
   MEMSET_ROM(&(header->addr[1 + nonce.len]), 0, l);
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
#endif



/**
 * @brief Get aes padding length (realign on aes block size)
 * @param input_length input length
 * @return length of padding
 */
STATIC_FUNC uint32_t get_aes_pad_len(uint32_t input_length)
{
   return (16-input_length)&15;
}




/**
 * @brief Build descriptors and call cryptoDMA for AES operation
 * @param config value for cfg mode register
 * @param key AES key
 * @param xtskey XTS key.
 * @param iv initialization vector
 * @param datain input data (plaintext or ciphertext)
 * @param dataout output data (ciphertext or plaintext)
 * @param aad1 additional authenticated data part #1
 * @param aad2 additional authenticated data part #2
 * @param tag_in authentication tag input for ::CCM
 * @param tag_out authentication tag input for ::CCM, ::GCM & ::CMAC
 * @param ctx_ptr AES context output
 * @param lenAlenC_blk lenA|lenC block for ::GCM mode
 * @return CRYPTOLIB_SUCCESS when execution was successful
 */
STATIC_FUNC uint32_t sx_aes_build_descr(  block_t *config,
                              block_t *key,
                              block_t *xtskey,
                              block_t *iv,
                              block_t *datain,
                              block_t *dataout,
                              block_t *aad1,
                              block_t *aad2,
                              block_t *tag_in,
                              block_t *tag_out,
                              block_t *ctx_ptr,
                              block_t *lenAlenC_blk)
{
#if ( AES_PARTIAL_ENABLED || AES_CBC_ENABLED || AES_OFB_ENABLED || AES_CFB_ENABLED || AES_XTS_ENABLED || AES_CTR_ENABLED || AES_GCM_ENABLED )
#define AES_MAX_FETCH_DESCRIPTORS 12
#else
#define AES_MAX_FETCH_DESCRIPTORS 9
#endif

#if AES_PARTIAL_ENABLED
#define AES_MAX_PUSH_DESCRIPTORS 6
#else
#define AES_MAX_PUSH_DESCRIPTORS 5
#endif

   struct dma_sg_descr_s desc_to[AES_MAX_FETCH_DESCRIPTORS];
   struct dma_sg_descr_s desc_fr[AES_MAX_PUSH_DESCRIPTORS];
   struct dma_sg_descr_s *d;  // pointer to current descriptor
   block_t keyb = *key;
   // input padding
   block_t aad_zeropad      = block_t_convert(SILEX_CRYPTOSOC_GET_GLOBALS()->zeroes, get_aes_pad_len(aad1->len + aad2->len));
   block_t datain_zeropad   = block_t_convert(SILEX_CRYPTOSOC_GET_GLOBALS()->zeroes, get_aes_pad_len(datain->len));
   block_t tagin_zeropad    = block_t_convert(SILEX_CRYPTOSOC_GET_GLOBALS()->zeroes, get_aes_pad_len(tag_in->len));

   // output discards
   block_t aads_discard      = block_t_convert(NULL, aad1->len + aad2->len + aad_zeropad.len);
   block_t dataout_discard   = block_t_convert(NULL, get_aes_pad_len(dataout->len));
   block_t tagout_discard    = block_t_convert(NULL, get_aes_pad_len(tag_out->len));

   // no input provided, -> empty input = 1 block of zero padding (i.e. for CMAC)
   if (!datain->len && !tag_in->len && !lenAlenC_blk->len && !aad1->len &&  !aad2->len )
      datain_zeropad.len = 16;
#if AES_HW_KEYS_ENABLED
   //do not transfer hardware keys
   if(keyb.addr == &aes_hw_key1 || keyb.addr == &aes_hw_key2)
      keyb.len = 0;
#endif //AES_HW_KEYS_ENABLED

   // fetcher descriptors
   d = desc_to;
   d = write_desc_blk(d,  config,          DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_CFG));
   realign_desc(d-1);
   d = write_desc_blk(d, &keyb,            DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY));
   realign_desc(d-1);
#if ( AES_PARTIAL_ENABLED || AES_CBC_ENABLED || AES_OFB_ENABLED || AES_CFB_ENABLED || AES_XTS_ENABLED || AES_CTR_ENABLED )
   d = write_desc_blk(d,  iv,              DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_IV));
   realign_desc(d-1);
#endif
#if AES_XTS_ENABLED
   d = write_desc_blk(d,  xtskey,          DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_KEY2));
   realign_desc(d-1);
#endif
   d = write_desc_blk(d,  aad1,            DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESHEADER);
   d = write_desc_blk(d,  aad2,            DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESHEADER);
   d = write_desc_blk(d, &aad_zeropad,     DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESHEADER | DMA_SG_TAG_SETINVALIDBYTES(aad_zeropad.len));
   realign_desc(d-1);
   d = write_desc_blk(d,  datain,          DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESPAYLOAD);
   d = write_desc_blk(d, &datain_zeropad,  DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(datain_zeropad.len));
   d = write_desc_blk(d,  tag_in,          DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESPAYLOAD);
   d = write_desc_blk(d, &tagin_zeropad,   DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESPAYLOAD | DMA_SG_TAG_SETINVALIDBYTES(tagin_zeropad.len));
#if AES_GCM_EMABLED
   d = write_desc_blk(d,  lenAlenC_blk,    DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISDATA   | DMA_SG_TAG_DATATYPE_AESPAYLOAD);
#endif
   set_last_desc(d-1);

   // pusher descriptors
   d = desc_fr;
   d = write_desc_blk(d, &aads_discard, 0);
   d = write_desc_blk(d,  dataout, 0);
   d = write_desc_blk(d, &dataout_discard, 0);
   d = write_desc_blk(d,  tag_out, 0);
   d = write_desc_blk(d, &tagout_discard, 0);
#if AES_PARTIAL_ENABLED
   d = write_desc_blk(d,  ctx_ptr, 0);
#endif
   set_last_desc(d-1);

   // launch cryptodma
   cryptodma_run_sg(desc_to, desc_fr);

   return CRYPTOLIB_SUCCESS;
}


#if AES_MASK_ENABLED
void sx_aes_load_mask(uint32_t value)
{
   struct dma_sg_descr_s desc_to;
   struct dma_sg_descr_s desc_fr;

   // Fetcher descriptor to store random in AES
   desc_to.addr = &value;
   desc_to.next_descr = DMA_AXI_DESCR_NEXT_STOP;
   desc_to.length_irq = 4 | DMA_AXI_DESCR_REALIGN;
   desc_to.tag = DMA_SG_ENGINESELECT_BA411E | DMA_SG_TAG_ISCONFIG | DMA_SG_TAG_SETCFGOFFSET(AES_OFFSET_MASK);
   // Dummy pusher descriptor
   desc_fr.addr = NULL;
   desc_fr.next_descr = DMA_AXI_DESCR_NEXT_STOP;
   desc_fr.length_irq = 0 | DMA_AXI_DESCR_REALIGN;

   // RUN
   cryptodma_run_sg(&desc_to, &desc_fr);
}
#endif

/**
 * @brief Verify the AES key (and in case of ::XTS, the key2) length are valid.
 * @param fct mode of operation of AES. Used only for ::XTS.
 * @param len length of the key (first key in ::XTS)
 * @param xts_len length of the secondary key used in ::XTS
 * @return true if key(s) length is/are valid.
 */
STATIC_FUNC bool IsKeyLenValid(sx_aes_fct_t fct, size_t len, size_t xts_len)
{
   bool valid = len == 16 || len == 24 || len == 32;
   if (fct == XTS)
      return valid && len == xts_len;
   return valid && xts_len == 0;
}

/**
 * @brief Verify that the size of the IV or the context passed to the BA411 is valid
 * @param fct mode of operation of AES, it determines with the context the expected length
 * @param ctx current state for the context, it determines with the mode the expected length
 * @param len length of the IV for full message (::CTX_WHOLE) or for beginning of message (::CTX_BEGIN), context for oters cases (::CTX_MIDDLE or ::CTX_END)
 * @return true if IV/context length is valid.
 */
STATIC_FUNC bool IsIVContextLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len)
{
   // Use a context switching, save it and add it later is mathematically equal
   // to the IV mechanism. So, at first iteration, it is a "true" IV which is
   // injected in the AES. For the following iteration the context replaces the
   // IV but works exactly the same way
   switch (fct)
   {
   case ECB:
      return !len;
#if AES_CMA_ENABLED
   case CMAC:
      if (ctx == CTX_BEGIN || ctx == CTX_WHOLE)
         return !len;
      break;
#endif
   case CCM:
      if (ctx == CTX_BEGIN || ctx == CTX_WHOLE)
         return !len;
      return len == AES_CTX_xCM_SIZE;
#if AES_GCM_ENABLED
   case GCM:
      if (ctx == CTX_BEGIN || ctx == CTX_WHOLE)
         return len == AES_IV_GCM_SIZE;
      return len == AES_CTX_xCM_SIZE;
#endif
   default:
      break;
   }

   return len == AES_IV_SIZE;
}

/**
 * @brief Verify that the next-context size to read from BA411 is valid
 * @param fct mode of operation of AES, it determines with the context the expected length
 * @param ctx current state for the context, it determines with the mode the expected length
 * @param len length of the next context
 * @return true if next context length is valid.
 */
STATIC_FUNC bool IsNextContextLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len)
{
   if (fct == ECB || ctx == CTX_END || ctx == CTX_WHOLE)
      return !len;
   else if (fct == GCM || fct == CCM)
      return len == AES_CTX_xCM_SIZE;
   return len == AES_CTX_SIZE;
}

/**
 * @brief Verify that the Nonce size is valid
 * @param fct mode of operation of AES, it determines with the context the expected length
 * @param ctx current state for the context, it determines with the mode the expected length
 * @param len length of the Nonce
 * @return true if Nonce length is valid.
 */
STATIC_FUNC bool IsNonceLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len)
{
   if (fct == GCM && (ctx == CTX_END /*|| ctx == CTX_WHOLE*/))
      return len == 16;
   else if (fct == CCM)
      return len >= 7 && len <= 13;
   return !len;
}

/**
 * @brief Verify that the data (payload) size is valid
 * @param fct mode of operation of AES, it determines with the context the expected length
 * @param ctx current state for the context, it determines with the mode the expected length
 * @param len length of the payload
 * @return true if payload length is valid.
 */
STATIC_FUNC bool IsPayloadLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len)
{
   // Context check
   if (ctx == CTX_BEGIN || ctx == CTX_MIDDLE) {
      if (fct != GCM)
         return len && !(len % 16);
   }
   else if (!len) {
      if (ctx == CTX_END && fct != CTR && fct != CMAC && fct != GCM)
      return false;
      else if (ctx == CTX_WHOLE && fct != CCM && fct != GCM && fct != CMAC)
         return false;
   }

   // Mode check
   if (fct == ECB || fct == OFB || fct == CFB)
      return !(len % 16) && len;
   else if (fct == CBC || fct == XTS) {
      if (ctx == CTX_WHOLE)
         return len >= 16;
      return len;
   }

   return true;
}

/**
 * @brief Verify that the tag (MAC) size is valid
 * @param fct mode of operation of AES, it determines with the context the expected length
 * @param ctx current state for the context, it determines with the mode the expected length
 * @param len length of the payload
 * @return true if MAC length is valid.
 */
STATIC_FUNC bool IsTagLenValid(sx_aes_fct_t fct, sx_aes_ctx_t ctx, size_t len)
{
   if(fct == GCM || fct == CMAC) {
      if (len != 0 && (ctx == CTX_BEGIN || ctx == CTX_MIDDLE))
         return false;
      if (len != 16  && (ctx == CTX_END || ctx == CTX_WHOLE))
         return false;
   }

   //CCM tag E [0,4,6,8,10,12,14,16]
   // check tag is even and remove 2 from even serie
   else if (fct == CCM) {
      if ((len % 2 != 0) || (len == 2) /*|| (!len) */|| (len > 16))
         return false;
   } else if (len)
      return false;

   return true;
}

/**
 * @brief Verify the differents inputs of sx_aes_blk
 * @param dir specify the direction of aes, in encryption or in decryption
 * @param fct mode of operation of AES, it determines with the context the expected lengths
 * @param ctx current state for the context, it determines with the mode the expected lengths
 * @param key pointer to the AES key.
 * @param xtskey pointer to the XTS key.
 * @param iv pointer to initialization vector. Used for \p aes_fct ::CBC, ::CTR, ::CFB, ::OFB, ::XTS and ::GCM, mode of operation. 16 bytes will be read.
 * @param data_in pointer to input data (plaintext or ciphertext).
 * @param aad pointer to additional authenticated data. Used for \p aes_fct ::GCM mode of operation.
 * @param tag pointer to the authentication tag. Used for ::GCM mode of operation. 16 bytes will be written.
 * @param ctx_ptr pointer to the AES context (after operation)
 * @param nonce_len_blk pointer to the lenAlenC data (AES GCM context switching only) or the AES CCM Nonce
 * @return ::CRYPTOLIB_SUCCESS if inputs are all valid, otherwise ::CRYPTOLIB_INVALID_PARAM.
 */
STATIC_FUNC uint32_t sx_aes_validate_input(sx_aes_mode_t dir,
                  sx_aes_fct_t fct,
                  sx_aes_ctx_t ctx,
                  block_t key,
                  block_t xtskey,
                  block_t iv,
                  block_t data_in,
                  block_t aad,
                  block_t tag,
                  block_t ctx_ptr,
                  block_t nonce_len_blk)
{
   if (dir != ENC && dir != DEC)
      return CRYPTOLIB_INVALID_PARAM;

   if (!IsKeyLenValid(fct, key.len, xtskey.len))
      return CRYPTOLIB_INVALID_PARAM;

   if (!IsIVContextLenValid(fct, ctx, iv.len))
      return CRYPTOLIB_INVALID_PARAM;

   if (!IsNextContextLenValid(fct, ctx, ctx_ptr.len))
      return CRYPTOLIB_INVALID_PARAM;

   if (!IsNonceLenValid(fct, ctx, nonce_len_blk.len))
      return CRYPTOLIB_INVALID_PARAM;

   if (fct != GCM && fct != CCM && aad.len != 0)
      return CRYPTOLIB_INVALID_PARAM;

   if (!IsPayloadLenValid(fct, ctx, data_in.len))
      return CRYPTOLIB_INVALID_PARAM;

   if (!IsTagLenValid(fct, ctx, tag.len))
      return CRYPTOLIB_INVALID_PARAM;

   return CRYPTOLIB_SUCCESS;
}


uint32_t sx_aes_blk( sx_aes_fct_t aes_fct,
               sx_aes_mode_t dir,
               sx_aes_ctx_t ctx,
               block_t key,
               block_t xtskey,
               block_t iv,
               block_t data_in,
               block_t data_out,
               block_t aad,
               block_t tag,
               block_t ctx_ptr,
               block_t nonce_len_blk)
{
   /* Step 0: input validation */
   uint32_t ret = sx_aes_validate_input(dir, aes_fct, ctx, key, xtskey, iv, data_in,
         aad, tag, ctx_ptr, nonce_len_blk);
   if (ret)
      return ret;

   /* Step 1: get value of config register, will also check aes_fct, dir, ctx values and key length*/
   uint32_t config = 0;
   ret = sw_aes_setmode(aes_fct, dir, ctx, key, &config);
   if(ret != CRYPTOLIB_SUCCESS)
      return ret;

   /* Step 2: pre-processing for authenticated modes */
   // 2.1a Generate CCM header
   block_t header = sx_getNULL_blk();

   #if AES_CCM_ENABLED
   uint8_t header_generated[22]; // Maximum header size (excluding aad2 which will be transfered directly)
   if (aes_fct == CCM) {
      header = block_t_convert(header_generated, sizeof(header_generated));
      // Generate the header message
      ret = generate_ccm_header(nonce_len_blk, aad.len, data_in.len, tag.len, &header);
      if (ret != CRYPTOLIB_SUCCESS)
         return ret;
   }
   #endif

   // 2.1b Generate GCM lenAlenC block
   block_t lenAlenC_blk = sx_getNULL_blk();

#if AES_GCM_ENABLED
   uint8_t lenAlenC[16];
   if ((aes_fct == GCM) && (ctx == CTX_WHOLE || ctx == CTX_END)) {
      if (ctx == CTX_WHOLE) {
         // build lenAlenC block as big endian byte array
         sx_math_u64_to_u8array(aad.len<<3, &lenAlenC[0], 0);
         sx_math_u64_to_u8array(data_in.len<<3,   &lenAlenC[8], 0);
         lenAlenC_blk = block_t_convert(lenAlenC, 16);
      } else {
         // With context switching, lenAlenC is to be provided by the host just after the data
         lenAlenC_blk = nonce_len_blk;
      }
   }
#endif

   // 2.2 prepare tag input and output depending on the modes and context
   block_t tag_in  = sx_getNULL_blk();
   block_t tag_out = sx_getNULL_blk();
   block_t tag_ref = sx_getNULL_blk();
   uint8_t tag_generated[16];
   if( (ctx == CTX_WHOLE || ctx == CTX_END) && tag.len && (aes_fct == CCM || aes_fct == GCM || aes_fct == CMAC) ) {
      if(dir == ENC) {
         //encrypt -> output MAC
         tag_out = tag;

      } else {
         // decrypt -> store output MAC
         tag_out = block_t_convert(tag_generated, tag.len);
         if(aes_fct == CCM) {
            // for CCM, MAC should be input of the ba411e core, and it returns zeroes if it's valid
            tag_in = tag;
            tag_ref = block_t_convert(SILEX_CRYPTOSOC_GET_GLOBALS()->zeroes, tag.len);
         } else {
            // for others, MAC is an output, that should be compare with reference tag
            tag_ref = tag;
         }

      }
   }

   // 2.3 handle alignment for data out
   if(data_out.flags & DMA_AXI_DESCR_CONST_ADDR)
      data_out.len = roundup_32(data_out.len);

   if(tag_out.flags & DMA_AXI_DESCR_CONST_ADDR)
      tag_out.len = roundup_32(tag_out.len);

   /* Step 3: Build descriptors and call cryptoDMA */
   block_t  config_blk = block_t_convert(&config, sizeof(config));
   ret = sx_aes_build_descr(&config_blk, &key, &xtskey, &iv, &data_in, &data_out, &header, &aad, &tag_in, &tag_out, &ctx_ptr, &lenAlenC_blk);
   if (ret != CRYPTOLIB_SUCCESS)
      return ret;

   /* Step 4: post-processing of MAC if necessary (authenticated modes in decrypt mode) */
   if( tag_ref.len ) {
      uint8_t tag_cpy[16];
      memcpy_blkIn(tag_cpy, tag_ref, tag_ref.len);
      uint32_t error = memcmp_time_cst(tag_out.addr, tag_cpy, tag_ref.len);
      if (aes_fct == GCM && dir == DEC && tag.flags == BLOCK_S_INCR_ADDR)
        memcpy_blk(tag , tag_out , tag_out.len);
      if (error)
         return CRYPTOLIB_INVALID_SIGN_ERR;
   }

   return CRYPTOLIB_SUCCESS;
}
