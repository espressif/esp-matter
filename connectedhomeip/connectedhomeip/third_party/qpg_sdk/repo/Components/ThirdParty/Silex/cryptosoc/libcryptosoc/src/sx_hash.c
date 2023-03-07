#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA413 hash function
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "sx_hash.h"
#include <string.h>
#include <stddef.h>
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba413_config.h"


/* Internal functions */

#define OP_FULL_HASH 1
#define OP_FULL_HMAC 2
#define OP_PART_HASH 3

/**
 * @brief internal function for hash operation
 * @param hash_fct hash function to use. See ::sx_hash_fct_t.
 * @param extra_in input K0 for OP_FULL_HMAC, or state for OP_PART_HASH, unused for OP_FULL_HASH
 * @param operation_type define type of operation to perform
 * @param data_in array of input data to process
 * @param entries length of array \p data_in
 * @param data_out output digest or state
 * @return ::CRYPTOLIB_SUCCESS when execution was successful
 */
STATIC_FUNC uint32_t sx_hash_internal(sx_hash_fct_t hash_fct,
                                 block_t extra_in,
                                 uint8_t operation_type,
                                 block_t data_in[],
                                 const unsigned int entries,
                                 block_t data_out)
{
   uint32_t total_len;
   uint32_t ign_bytes;
   uint32_t extra_in_tag;
   struct ba413_regs_s info;
   struct dma_sg_descr_s desc_in[SX_HASH_ARRAY_MAX_ENTRIES + 2];
   struct dma_sg_descr_s desc_out[2];
   struct dma_sg_descr_s *current_desc;
   uint32_t genlen, outlen;

   CRYPTOLIB_ASSERT(entries <= SX_HASH_ARRAY_MAX_ENTRIES, "Too many entries in data array");

   switch (hash_fct) {
   #if MD5_ENABLED
   case e_MD5:
      info.config = BA413_CONF_MODE_MD5;
      break;
   #endif
   #if SHA1_ENABLED
   case e_SHA1:
      info.config = BA413_CONF_MODE_SHA1;
      break;
   #endif
   #if SHA224_ENABLED
   case e_SHA224:
      info.config = BA413_CONF_MODE_SHA224;
      break;
   #endif
   case e_SHA256:
      info.config = BA413_CONF_MODE_SHA256;
      break;
   case e_SHA384:
      info.config = BA413_CONF_MODE_SHA384;
      break;
   case e_SHA512:
      info.config = BA413_CONF_MODE_SHA512;
      break;
   default:
      return CRYPTOLIB_INVALID_PARAM;
   }

   switch (operation_type) {
   // complete hash operation: no need to load extra info, enable padding in
   // hardware, output will be digest
   case OP_FULL_HASH:
      info.config |= BA413_CONF_HWPAD | BA413_CONF_FINAL;
      extra_in.len = 0;
      extra_in_tag = DMA_SG_ENGINESELECT_BA413;
      genlen = sx_hash_get_digest_size(hash_fct);
      break;

   // complete hmac operation: need to load extra info K0, enable padding in
   // hardware, output will be digest
   case OP_FULL_HMAC:
      info.config |= BA413_CONF_HMAC | BA413_CONF_HWPAD | BA413_CONF_FINAL;
      extra_in_tag = DMA_SG_TAG_DATATYPE_HASHK0 | DMA_SG_ENGINESELECT_BA413 |
                     DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST;
      genlen = sx_hash_get_digest_size(hash_fct);
      break;

   // partial hash operation: need to load initial state, don't enable padding in
   // hardware, output will be state
   case OP_PART_HASH:
   default:
      extra_in_tag = DMA_SG_TAG_DATATYPE_HASHINIT | DMA_SG_ENGINESELECT_BA413 |
                     DMA_SG_TAG_ISDATA | DMA_SG_TAG_ISLAST;
      genlen = sx_hash_get_state_size(hash_fct);
   }

   // configuration (to register)
   desc_in[0].addr = &info;
   desc_in[0].next_descr = &desc_in[1];
   desc_in[0].length_irq = 4 | DMA_AXI_DESCR_REALIGN;
   desc_in[0].tag = DMA_SG_ENGINESELECT_BA413 | DMA_SG_TAG_ISCONFIG;

   // K0 or init value
   desc_in[1].addr = extra_in.addr;
   desc_in[1].next_descr = &desc_in[2];
   desc_in[1].length_irq = extra_in.len | (extra_in.flags & BLOCK_S_FLAG_MASK_DMA_PROPS) | DMA_AXI_DESCR_REALIGN;
   desc_in[1].tag = extra_in_tag;

   // datas
   uint32_t i = 0;
   total_len = 0;
   current_desc = &desc_in[1];
   for (i = 0; i < entries; i++) {
      if (data_in[i].len) {
         total_len += data_in[i].len;
         current_desc++;
         current_desc->addr = data_in[i].addr;
         current_desc->next_descr = (current_desc + 1);
         current_desc->length_irq = data_in[i].len | (data_in[i].flags & BLOCK_S_FLAG_MASK_DMA_PROPS);
         current_desc->tag = DMA_SG_ENGINESELECT_BA413 | DMA_SG_TAG_ISDATA |
                             DMA_SG_TAG_DATATYPE_HASHMSG;
      }
   }

   if (total_len == 0) {
      ign_bytes = 4;
      /* DMA_SG_TAG_ISDATA must be present for each part of data, even for
       * an empty message. That's why in case of HMAC without data (empty
       * message), we artificially build a descriptor with zero data length
       * and insert it before the last descriptor.
       * To summarize, for HMAC, there are 2 descriptors tagged with IS_DATA,
       * one for K0 and the second which could be without data associated in
       * case of empty message.
       */
      if (operation_type == OP_FULL_HMAC) {
         ++current_desc;
         current_desc->tag = DMA_SG_ENGINESELECT_BA413 | DMA_SG_TAG_ISDATA |
               DMA_SG_TAG_DATATYPE_HASHMSG;
         current_desc->length_irq = 0;
         current_desc->addr = NULL;
      }
   } else if (total_len & 0x3) // not word aligned
      ign_bytes = (4 - (total_len & 0x3));
   else
      ign_bytes = 0;

   // last descriptor
   current_desc->next_descr = DMA_AXI_DESCR_NEXT_STOP;
   current_desc->length_irq += ign_bytes;
   current_desc->length_irq |= DMA_AXI_DESCR_REALIGN;
   current_desc->tag |= DMA_SG_TAG_ISLAST | (ign_bytes << 8);

   // ouput digest or state
   outlen = SX_MIN(genlen, data_out.len);
   desc_out[0].addr = data_out.addr;
   desc_out[0].next_descr = &desc_out[1];
   desc_out[0].length_irq = outlen | (data_out.flags & BLOCK_S_FLAG_MASK_DMA_PROPS);

   // discard unused
   desc_out[1].addr = NULL;
   desc_out[1].next_descr = DMA_AXI_DESCR_NEXT_STOP;
   desc_out[1].length_irq = (genlen - outlen) | BLOCK_S_INCR_ADDR | DMA_AXI_DESCR_DISCARD | DMA_AXI_DESCR_REALIGN;

   // #if DEBUG_ENABLED
   //    SX_PRINTF("\ndescriptor @addr    : dataptr  nextptr  length   tag\n");
   //    for (i = 0; i < entries+2; i++)
   //       SX_PRINTF("desc_in [%d] %08X: %08X %08X %08X %08X\n", i, &desc_in[i],
   //                   desc_in[i].addr, desc_in[i].next_descr, desc_in[i].length_irq, desc_in[i].tag)
   //    for (i = 0; i < 2; i++)
   //       SX_PRINTF("desc_out[%d] %08X: %08X %08X %08X %08X\n", i, &desc_out[i],
   //                   desc_out[i].addr, desc_out[i].next_descr, desc_out[i].length_irq, desc_out[i].tag)
   // #endif

   // launch cryptodma operation
   cryptodma_run_sg(desc_in, desc_out);
   // if error occurs, hardfault should be trigger


   return CRYPTOLIB_SUCCESS;
}

/* Public functions: properties */
uint32_t sx_hash_get_digest_size(sx_hash_fct_t hash_fct)
{
   switch (hash_fct) {
   case e_MD5:
      return MD5_DIGESTSIZE;
   case e_SHA1:
      return SHA1_DIGESTSIZE;
   case e_SHA224:
      return SHA224_DIGESTSIZE;
   case e_SHA256:
      return SHA256_DIGESTSIZE;
   case e_SHA384:
      return SHA384_DIGESTSIZE;
   case e_SHA512:
      return SHA512_DIGESTSIZE;
   default:
      return 0;
   }
}

uint32_t sx_hash_get_block_size(sx_hash_fct_t hash_fct)
{
   switch (hash_fct) {
   case e_MD5:
      return MD5_BLOCKSIZE;
   case e_SHA1:
      return SHA1_BLOCKSIZE;
   case e_SHA224:
      return SHA224_BLOCKSIZE;
   case e_SHA256:
      return SHA256_BLOCKSIZE;
   case e_SHA384:
      return SHA384_BLOCKSIZE;
   case e_SHA512:
      return SHA512_BLOCKSIZE;
   default:
      return 0;
   }
}

uint32_t sx_hash_get_state_size(sx_hash_fct_t hash_fct)
{

   switch (hash_fct) {
   case e_MD5:
      return MD5_INITSIZE;
   case e_SHA1:
      return SHA1_INITSIZE;
   case e_SHA224:
      return SHA224_INITSIZE;
   case e_SHA256:
      return SHA256_INITSIZE;
   case e_SHA384:
      return SHA384_INITSIZE;
   case e_SHA512:
      return SHA512_INITSIZE;
   default:
      return 0;
   }
}

/* Public functions: process arrays of blocks */
uint32_t sx_hash_array_blk(sx_hash_fct_t hash_fct,
                           block_t data_in[],
                           const unsigned int entries,
                           block_t data_out)
{
   return sx_hash_internal(hash_fct, block_t_convert(NULL, 0), OP_FULL_HASH,
                           data_in, entries, data_out);
}

#if HMAC_ENABLED
uint32_t sx_hmac_array_blk(sx_hash_fct_t hash_fct,
                           block_t key,
                           block_t data_in[],
                           const unsigned int entries,
                           block_t data_out)
{
   uint32_t blocksize;
   uint32_t kblen;
   uint8_t K0[MAX_BLOCKSIZE];

   blocksize = sx_hash_get_block_size(hash_fct);

   // 1. compute K0
   if (key.len <= blocksize) {
      memcpy_blkIn(K0, key, key.len);
      kblen = key.len;
   } else {
      uint32_t status;
      kblen = sx_hash_get_digest_size(hash_fct);
      status = sx_hash_blk(hash_fct, key, block_t_convert(K0, kblen));
      if (status)
         return status;
   }
   // append zeros
   MEMSET(K0 + kblen, 0, blocksize - kblen);

   // 2. perform hmac
   return sx_hash_internal(hash_fct, block_t_convert(K0, blocksize), OP_FULL_HMAC,
                           data_in, entries, data_out);
}
#endif

/* Public functions: process  blocks */
uint32_t sx_hash_update_blk(sx_hash_fct_t hash_fct, block_t state, block_t data)
{
   return sx_hash_internal(hash_fct, state, OP_PART_HASH, &data, 1, state);
}

uint32_t sx_hash_blk(sx_hash_fct_t hash_fct, block_t data_in, block_t data_out)
{
   return sx_hash_array_blk(hash_fct, &data_in, 1, data_out);
}

#if HMAC_ENABLED
uint32_t sx_hmac_blk(sx_hash_fct_t hash_fct, block_t key, block_t data_in, block_t data_out)
{
   return sx_hmac_array_blk(hash_fct, key, &data_in, 1, data_out);
}
#endif
