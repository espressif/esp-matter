

#include "global.h"

/**
 * @file
 * @brief defines dma configuration functions
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "cryptolib_def.h"
#include <stddef.h>
#include <string.h>
#include <inttypes.h>
#include "cryptodma.h"
#include "sxregs.h"

#ifndef BLK_MEMCPY_MIN_DMA_SIZE
#define BLK_MEMCPY_MIN_DMA_SIZE 0
#endif

//const block_t NULL_blk = {NULL, 0, BLOCK_S_INCR_ADDR | DMA_AXI_DESCR_DISCARD};

static dma_sg_regs32_t * const regs = (dma_sg_regs32_t *) ADDR_CRYPTOMASTER_REGS;

/**
* @brief Configure fetch and push operations in scatter-gather mode on internal DMA
* @param first_fetch_descriptor physical address of the first fetcher descriptor to be configured
* @param first_push_descriptor physical address of the first pusher descriptor to be configured
*/
STATIC_FUNC void cryptodma_config_sg(struct dma_sg_descr_s * first_fetch_descriptor, struct dma_sg_descr_s * first_push_descriptor)
{
   WR_REG32(regs->fetch_addr, (uint32_t) first_fetch_descriptor);
   WR_REG32(regs->push_addr,  (uint32_t) first_push_descriptor);
   WR_REG32(regs->config,      DMA_AXI_CONFIGREG_FETCHER_INDIRECT|DMA_AXI_CONFIGREG_PUSHER_INDIRECT);
}

/**
 * @brief Configure fetch and push operations in direct mode on internal DMA
 * @param src block_t to the source data to transfer
 * @param dest block_t to the destination location
 * @param length the length in bytes to transfer (from src to dest)
*/
STATIC_FUNC void cryptodma_config_direct(block_t dest, block_t src, uint32_t length)
{
   uint32_t transfer_len = dest.len<length ? dest.len : length;
   // if destination is a FIFO, size needs to be a multiple of 32-bits.
   if (dest.flags & BLOCK_S_CONST_ADDR)
      transfer_len = roundup_32(transfer_len);

   WR_REG32(regs->fetch_addr, (uint32_t) src.addr);
   WR_REG32(regs->fetch_len,  transfer_len | (src.flags & BLOCK_S_FLAG_MASK_DMA_PROPS) | DMA_AXI_DESCR_REALIGN);
   WR_REG32(regs->fetch_tag,  DMA_SG_ENGINESELECT_BYPASS);
   WR_REG32(regs->push_addr,  (uint32_t) dest.addr);
   WR_REG32(regs->push_len,   transfer_len | (dest.flags & BLOCK_S_FLAG_MASK_DMA_PROPS) | DMA_AXI_DESCR_REALIGN);
   WR_REG32(regs->config,     DMA_AXI_CONFIGREG_PUSHER_DIRECT | DMA_AXI_CONFIGREG_FETCHER_DIRECT);
}

/**
 * @brief Start internal DMA transfer
 */
STATIC_FUNC void cryptodma_start(void) {
   CRYPTOLIB_DMA_BARRIER();
   WR_REG32(regs->int_stat_clr, DMA_AXI_INTENSETREG_ALL_EN);
   WR_REG32(regs->int_en,       DMA_AXI_INTENSETREG_PUSHER_STOPPED_EN);
   WR_REG32(regs->start,        DMA_AXI_STARTREG_FETCH | DMA_AXI_STARTREG_PUSH);
}

/**
 * @brief Wait until internal DMA is done
 */
STATIC_FUNC void cryptodma_wait(void) {
   // Wait until DMA is done
   #if WAIT_CRYPTOMASTER_WITH_REGISTER_POLLING // polling
      while (RD_REG32(regs->status)&DMA_AXI_STATUSREG_MASK_PUSHER_BUSY);
   #else  // wait interrupt
      CRYPTOMASTER_WAITIRQ_FCT();
   #endif
}

/**
 * @brief Check cryptodma error flag
 * @return CRYPTOLIB_DMA_ERR if fifo's are not empty, CRYPTOLIB_SUCCESS otherwise
 */
STATIC_FUNC uint32_t cryptodma_check_bus_error(void) {
   if (RD_REG32(regs->int_stat_raw)&(DMA_AXI_RAWSTATREG_MASK_FETCHER_ERROR | DMA_AXI_RAWSTATREG_MASK_PUSHER_ERROR)) {
      CRYPTOLIB_PRINTF("CRYPTODMA bus error");
      return CRYPTOLIB_DMA_ERR;
   } else {
      return CRYPTOLIB_SUCCESS;
   }
}

/**
 * @brief Check cryptodma fifo status
 * @return CRYPTOLIB_DMA_ERR if bus error occured, CRYPTOLIB_SUCCESS otherwise
 */
STATIC_FUNC uint32_t cryptodma_check_fifo_empty(void) {
   uint32_t dma_status = RD_REG32(regs->status);
   if (dma_status & (DMA_AXI_STATUSREG_MASK_FIFOIN_NOT_EMPTY|DMA_AXI_STATUSREG_MASK_FIFOOUT_NDATA)) {
      CRYPTOLIB_PRINTF("CRYPTODMA fifo error %08"PRIx32"", dma_status);
      return CRYPTOLIB_DMA_ERR;
   } else {
      return CRYPTOLIB_SUCCESS;
   }
}

/**
 * @brief Check cryptodma status
 * Trigger a hardfault if any error occured
 */
STATIC_FUNC void cryptodma_check_status(void) {
   CRYPTOLIB_DMA_BARRIER();
   if (cryptodma_check_bus_error() | cryptodma_check_fifo_empty()) {
      TRIGGER_HARDFAULT_FCT();
   }
}

void cryptodma_reset(void) {
   WR_REG32(regs->config, DMA_AXI_CONFIGREG_SOFTRESET);
   WR_REG32(regs->config, 0);                                          // Clear soft-reset
   while (RD_REG32(regs->status) & DMA_AXI_STATUSREG_MASK_SOFT_RESET); // Wait for soft-reset deassertion
}


#ifndef MAP_DESCRIPTOR_HOOKS

/** @brief Map software descriptors and buffers to the hardware
 * @param first_fetch_descriptor DMA input descriptors list
 * @param first_push_descriptor DMA output descriptors list
 * @param mapped_in Pointer for the pointer to the mapped input descriptors list
 * @param mapped_out Pointer for the pointer to the mapped input descriptors list
 */
void map_descriptors(struct dma_sg_descr_s *first_fetch_descriptor,
   struct dma_sg_descr_s *first_push_descriptor,
   struct dma_sg_descr_s **mapped_in,
   struct dma_sg_descr_s **mapped_out)
{
   *mapped_in = first_fetch_descriptor;
   *mapped_out = first_push_descriptor;
}

/** @brief Unmap descriptors and buffers to the hardware
 * @param out_decs Output DMA descriptors list
 */
void unmap_descriptors(struct dma_sg_descr_s *out_descs)
{
}
#endif

void cryptodma_run_sg(struct dma_sg_descr_s * first_fetch_descriptor, struct dma_sg_descr_s * first_push_descriptor)
{
   struct dma_sg_descr_s *mapped_in, *mapped_out;

   map_descriptors(first_fetch_descriptor, first_push_descriptor, &mapped_in, &mapped_out);
   cryptodma_config_sg(mapped_in, mapped_out);
   cryptodma_start();
   cryptodma_wait();
   cryptodma_check_status();
   unmap_descriptors(first_push_descriptor);
}


struct dma_sg_descr_s* write_desc_blk(struct dma_sg_descr_s* d, const block_t* blk, uint32_t tag)
{
   if (blk->len) {
      struct dma_sg_descr_s * nextdescr = d+1;
      d->next_descr    = nextdescr;
      d->addr      = blk->addr;
      if(blk->addr == NULL)
         d->length_irq = blk->len | DMA_AXI_DESCR_DISCARD;
      else
         d->length_irq = blk->len | (blk->flags&BLOCK_S_FLAG_MASK_DMA_PROPS);
      d->tag          = tag;
      return nextdescr;
   } else {
      return d;
   }
}

void realign_desc(struct dma_sg_descr_s * d)
{
   d->length_irq |= DMA_AXI_DESCR_REALIGN;
}

void set_last_desc(struct dma_sg_descr_s * d)
{
   d->next_descr = DMA_AXI_DESCR_NEXT_STOP;
   d->tag |= DMA_SG_TAG_ISLAST;
   d->length_irq |= DMA_AXI_DESCR_REALIGN;
}

void memcpy_blk(block_t dest, block_t src, uint32_t length)
{
   if (dest.flags & DMA_AXI_DESCR_DISCARD) {
      return;
   }
   if (dest.len < length) {
      length = dest.len;
   }
   if (!(src.flags & BLOCK_S_CONST_ADDR) && (src.len < length)) {
      length = src.len;
   }
   if (!length)
      return;
   if (length >= BLK_MEMCPY_MIN_DMA_SIZE) {
      cryptodma_config_direct(dest, src, length);
      cryptodma_start();
      cryptodma_wait();
      cryptodma_check_status();
   } else {
      if (src.flags & BLOCK_S_CONST_ADDR) {
         for (uint32_t i = 0; i < length; i+=4) {
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-align"
#endif
            uint32_t v = *(volatile uint32_t*)src.addr;
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
            size_t len = (dest.len-i) < 4 ? (dest.len-i): 4;
            MEMCPY_ROM(dest.addr + i, &v, len);
         }
      } else {
         MEMCPY_ROM(dest.addr, src.addr, length);
      }
   }
}

void memcpy_blkOut(block_t dest, const volatile void * src, uint32_t length)
{
   block_t s = {(uint8_t*)src, length, BLOCK_S_INCR_ADDR};
   memcpy_blk(dest, s, length);
}

void memcpy_blkIn(volatile void * dest, block_t src, uint32_t length)
{
   block_t d = {(uint8_t*)dest, length, BLOCK_S_INCR_ADDR};
   memcpy_blk(d, src, length);
}

void memcpy_array(volatile void * dest, const volatile void * src, uint32_t length)
{
   MEMCPY_P((void*)dest, (const void*)src, length);
}

void* rom_memset(void *str, int c, size_t n)
{
    int32_t i;
    //for(i=0;i<n;i++)
    /* reversed the order. This is a trick to avoid that the compiler inserts a call to __aeabi_memset */
    for(i=n-1;i>=0;i--)
    {
        ((char*)str)[i] = c;
    }
    return str;
}

void* rom_memcpy(void *str1, const void *str2, size_t n)
{
    uint32_t i;
    for(i=0;i<n;i++)
    {
        ((char*)str1)[i] = ((char*)str2)[i];
    }
    return str1;
}

int rom_memcmp(const void *str1, const void *str2, size_t n)
{
    uint32_t i;
    for(i=0;i<n;i++)
    {
        if ( ((char*)str1)[i] < ((char*)str2)[i] )
        {
            return -1;
        }
        else if ( ((char*)str1)[i] > ((char*)str2)[i] )
        {
            return 1;
        }
    }
    return 0;

}
