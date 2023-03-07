/**
 * @file
 * @brief defines dma configuration functions
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#ifndef CRYPTODMA_H_
#define CRYPTODMA_H_

#if defined(GP_DIVERSITY_ROM_CODE)
#include "cryptodma_RomCode.h"
#else //defined(GP_DIVERSITY_ROM_CODE)

#include <stdint.h>
#include "sx_dma.h"
#include "cryptolib_types.h"

/**
* @brief Select which IP core the DMA will use. To set in descriptor ::dma_sg_descr_s.tag.
*/
enum dma_sg_EngineSelect_e
{
   DMA_SG_ENGINESELECT_BYPASS = 0x00, /**< direct bypass from input to output */
   DMA_SG_ENGINESELECT_BA411E = 0x01, /**< data flow through BA411E AES */
   DMA_SG_ENGINESELECT_BA412  = 0x02, /**< data flow through BA412 DES */
   DMA_SG_ENGINESELECT_BA413  = 0x03, /**< data flow through BA413 Hash */
   DMA_SG_ENGINESELECT_BA417  = 0x04  /**< data flow through BA417 ChaChaPoly */
};


/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, to direct data to parameters */
#define DMA_SG_TAG_ISCONFIG 0x00000010
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, to direct data to processing */
#define DMA_SG_TAG_ISDATA 0x00000000
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag,specifying data as last */
#define DMA_SG_TAG_ISLAST  0x00000020

/** @brief macro to set the offset in the configuration for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag */
#define DMA_SG_TAG_SETCFGOFFSET(a) ((((a)&0xFF)<<8))

/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, specifying data type to message */
#define DMA_SG_TAG_DATATYPE_HASHMSG     0x00000000
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, specifying data type to initialization state */
#define DMA_SG_TAG_DATATYPE_HASHINIT    0x00000040
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, specifying data type to HMAC key */
#define DMA_SG_TAG_DATATYPE_HASHK0      0x00000080

/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, specifying data type payload (will be encrypted/decrypted and authenticated) */
#define DMA_SG_TAG_DATATYPE_AESPAYLOAD    0x00000000
/** @brief value for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag, specifying data type header (will only be authenticated, not encrypted/decrypted) */
#define DMA_SG_TAG_DATATYPE_AESHEADER     0x00000040

/** @brief macro to set the amount of invalid bytes in for ::dma_sg_descr_s.tag or ::dma_sg_regs_s.fetch_tag */
#define DMA_SG_TAG_SETINVALIDBYTES(a) ((((a)&0x1F)<<8))


extern const block_t NULL_blk;

#if defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)
#include "silexCryptoSoc_CodeJumpTableFlash_Defs_cryptodma.h"
#include "silexCryptoSoc_CodeJumpTableRom_Defs_cryptodma.h"
#endif // defined(GP_DIVERSITY_JUMPTABLES) && defined(GP_DIVERSITY_ROM_CODE)

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_START */

/**
 * @brief Reset the internal DMA
 */
void cryptodma_reset(void);

/**
 * @brief Configure & start the Internal DMA in indirect mode, wait for an interrupt and check for errors
 * @param first_fetch_descriptor list of descriptors to fetch from
 * @param first_push_descriptor  list of descriptors to push to
 */
/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_START */
void cryptodma_run_sg(struct dma_sg_descr_s * first_fetch_descriptor, struct dma_sg_descr_s * first_push_descriptor);


void map_descriptors(struct dma_sg_descr_s *first_fetch_descriptor, struct dma_sg_descr_s *first_push_descriptor, struct dma_sg_descr_s **mapped_in, struct dma_sg_descr_s **mapped_out);

void unmap_descriptors(struct dma_sg_descr_s *out_descs);


/**
 * @brief Write descriptor to send data to IP core
 * @param d   address of previous descriptor
 * @param blk     data to be sent
 * @param tag     tag identifying data purpose
 * @return        address of current descriptor
 */
struct dma_sg_descr_s* write_desc_blk(struct dma_sg_descr_s* d, const block_t* blk, uint32_t tag);

/**
 * @brief Mark input descriptor as needing to be realigned
 * @param d address of descriptor
 */
void realign_desc(struct dma_sg_descr_s * d);

/**
 * @brief Mark input descriptor as last
 * @param d address of last descriptor
 */
void set_last_desc(struct dma_sg_descr_s * d);


/**
 * Copy length bytes from the src block_t to the dest block_t
 * @param src block_t to the source data to transfer
 * @param dest block_t to the destination location
 * @param length the length in bytes to transfer (from src to dest)
 */
void memcpy_blk(block_t dest, block_t src, uint32_t length);

/**
 * Copy length bytes from src to the block_t defined by dest
 * @param  dest   block_t to the destination location
 * @param  src    input buffer (pointer)
 * @param  length length of the data to copy
 */
void memcpy_blkOut(block_t dest, const volatile void * src, uint32_t length);

/**
 * Copy length bytes from the src block_t to the dest buffer
 * @param  dest   output buffer (pointer)
 * @param  src    input block_t
 * @param  length length of the data to copy
 */
void memcpy_blkIn(volatile void * dest, block_t src, uint32_t length);

/**
 * Copy length bytes from the src array to the dest array
 * @param  dest   output array (pointer)
 * @param  src    input array (pointer)
 * @param  length length of the data to copy
 */
void memcpy_array(volatile void * dest, const volatile void * src, uint32_t length);

#if defined(GP_DIVERSITY_JUMPTABLES)
void cryptodma_config_sg(struct dma_sg_descr_s * first_fetch_descriptor, struct dma_sg_descr_s * first_push_descriptor);
void cryptodma_config_direct(block_t dest, block_t src, uint32_t length);
void cryptodma_start(void);
void cryptodma_wait(void);
uint32_t cryptodma_check_bus_error(void);
uint32_t cryptodma_check_fifo_empty(void);
void cryptodma_check_status(void);
#endif // GP_DIVERSITY_JUMPTABLES

void* rom_memset(void *str, int c, size_t n);
void* rom_memcpy(void *str1, const void *str2, size_t n);
int   rom_memcmp(const void *str1, const void *str2, size_t n);

/* JUMPTABLE_ROM_FUNCTION_DEFINITIONS_END */

block_t sx_getNULL_blk(void);

/* JUMPTABLE_FLASH_FUNCTION_DEFINITIONS_END */

#define MEMCPY_ROM(dst, src, len)   rom_memcpy(dst, src, len)
#define MEMSET_ROM(dst, value, len) rom_memset(dst, value, len)
#define MEMCMP_ROM(dst, value, len) rom_memcmp(dst, value, len)

#endif //defined(GP_DIVERSITY_ROM_CODE)

#endif
