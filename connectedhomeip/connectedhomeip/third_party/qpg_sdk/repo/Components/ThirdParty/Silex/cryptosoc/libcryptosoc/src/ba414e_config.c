#if defined (GP_DIVERSITY_JUMPTABLES)
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief This file contains useful functions to configure registers of the BA4414E
 *          PointerReg, CommandReg, ControlReg, StatusReg
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */


#include "cryptolib_internal.h"
#include "ba414e_config.h"
#include <string.h>
#include <inttypes.h>
#include "sxregs.h"
#include "cryptodma.h"
#include "sx_math.h"
#include "sx_rng.h"

// #define PK_DEBUG_ENABLED

static BA414ERegs_t * const BA414E_REGS = (BA414ERegs_t *)ADDR_BA414E_REGS;

/** @brief: Function tells if a given Public Key is Busy or not (checking its status register)
 *  @return 1 when given pub key is busy, 0 otherwise
 */
#if WAIT_PK_WITH_REGISTER_POLLING
static CHECK_RESULT int ba414e_is_busy(void)
{
   uint32_t status = RD_REG32(BA414E_REGS->StatusReg);
   return ((status >> (BA414E_STS_BUSY_LSB)) & 0x1);
}
#endif


/** @brief This function starts the given PK
 *  @return The start bit of the CommandReg of the given BA414E struct has been set
 *  to appropriate value.
 */
STATIC_FUNC void ba414e_start(void){
   uint32_t DataReg = 0x1<<BA414E_CTL_START_LSB;
   WR_REG32(BA414E_REGS->ControlReg, DataReg );
}


/**
 * @brief Function is used to get the full contents of the BA414 status register
 * @return the contents oft he status register as uint32_t.
 */
STATIC_FUNC uint32_t ba414e_get_status(void)
{
    return RD_REG32(BA414E_REGS->StatusReg);
}


/**
 * @brief Function is used to wait for an interrupt, and read & return the status register
 * @return the contents of the status register as uint32_t.
 */
STATIC_FUNC uint32_t ba414e_wait_status(void)
{
   #if WAIT_PK_WITH_REGISTER_POLLING
      while(ba414e_is_busy());
   #else
      PK_WAITIRQ_FCT();
   #endif

   return (ba414e_get_status() & BA414E_STS_ERROR_MASK);
}

void ba414e_set_config(uint32_t PtrA, uint32_t PtrB, uint32_t PtrC, uint32_t PtrN)
{
   uint32_t DataReg = 0x00000000;
   DataReg = ((PtrC<<BA414E_PTR_OPPTRC_LSB) & BA414E_PTR_OPPTRC_MASK) |
             ((PtrB<<BA414E_PTR_OPPTRB_LSB) & BA414E_PTR_OPPTRB_MASK) |
             ((PtrA<<BA414E_PTR_OPPTRA_LSB) & BA414E_PTR_OPPTRA_MASK) |
             ((PtrN<<BA414E_PTR_OPPTRN_LSB) & BA414E_PTR_OPPTRN_MASK) ;

   WR_REG32(BA414E_REGS->PointerReg, DataReg);
}

void ba414e_set_command(uint32_t op, uint32_t operandsize, uint32_t swap, uint32_t curve_flags)
{
   uint32_t DataReg = 0x80000000;
   uint32_t NumberOfBytes;

   if (operandsize > 0) {
      NumberOfBytes = operandsize - 1;
   } else {
      NumberOfBytes = 0;
   }

   // Data ram is erased automatically after reset in PK engine.
   // Wait until erasing is finished before writing any data
   // (this routine is called before any data transfer)
   #if WAIT_PK_WITH_REGISTER_POLLING
      while(ba414e_is_busy());
   #else
      PK_WAITIRQ_FCT();
   #endif

   DataReg = DataReg | op;
   DataReg = DataReg | (NumberOfBytes  << BA414E_CMD_OPSIZE_LSB);
   #if PK_CM_ENABLED
      // Counter-Measures for the Public Key
      if (BA414E_IS_OP_WITH_SECRET_ECC(op)) {
         // ECC operation
         DataReg = DataReg | (BA414E_CMD_RANDPR(PK_CM_RANDPROJ_ECC));
         DataReg = DataReg | (BA414E_CMD_RANDKE(PK_CM_RANDKE_ECC));
      } else if (BA414E_IS_OP_WITH_SECRET_MOD(op)) {
         // Modular operations
         DataReg = DataReg | (BA414E_CMD_RANDPR(PK_CM_RANDPROJ_MOD));
         DataReg = DataReg | (BA414E_CMD_RANDKE(PK_CM_RANDKE_MOD));
      }
   #endif
   DataReg = DataReg | (BA414E_CMD_SWAP_MASK & (swap << BA414E_CMD_SWAP_LSB));
   DataReg = DataReg | curve_flags;


   WR_REG32(BA414E_REGS->CommandReg, DataReg );


   #if PK_CM_ENABLED
   // Copy random value in the CryptoRAM for the counter-measures
   if ((BA414E_IS_OP_WITH_SECRET_ECC(op) && (PK_CM_RANDKE_ECC || PK_CM_RANDPROJ_ECC)) ||
       (BA414E_IS_OP_WITH_SECRET_MOD(op) && (PK_CM_RANDKE_MOD || PK_CM_RANDPROJ_MOD))) {
      uint8_t pk_cm_rand[8] = { 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5, 0xb5 };
      block_t rnd_blk = block_t_convert(pk_cm_rand, PK_CM_RAND_SIZE);

      // Generate non-null random value
      do {
#if !CRYPTOLIB_TEST_ENABLED
         sx_rng_get_rand_blk(rnd_blk);
#endif //!CRYPTOLIB_TEST_ENABLED
      } while(!sx_math_array_is_not_null(rnd_blk.addr, PK_CM_RAND_SIZE));

      // Random has to be odd
      if (BA414E_IS_OP_WITH_SECRET_MOD(op) && PK_CM_RANDPROJ_MOD) {
         pk_cm_rand[PK_CM_RAND_SIZE-1] |= 1;
      }

      // Copy random to cryptoRAM (pad with 0 if smaller than operandsize)
      if (swap) {
         mem2CryptoRAM_rev(rnd_blk, roundup_32(operandsize), BA414E_MEMLOC_15);
      } else {
         mem2CryptoRAM(rnd_blk, roundup_32(operandsize), BA414E_MEMLOC_15);
      }
   }
   #endif
}


void mem2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset)
{
  if(src.len > size) {

    src.len = size;
  }

  if(!src.len || !size){

    return;
  }


   block_t dst;
   dst.flags   = BLOCK_S_INCR_ADDR;
   dst.len     = size;
   dst.addr    = BA414E_ADDR_MEMLOC(offset, src.len);
   memcpy_blk(dst, src, src.len);

   if(src.len < size) {
      dst.addr = BA414E_ADDR_MEMLOC(offset, size);
      MEMSET_ROM(dst.addr, 0, size - src.len);
   }

}

void point2CryptoRAM_rev(block_t src, uint32_t size, uint32_t offset)
{
   CRYPTOLIB_ASSERT((src.len >= size*2), "source length less then point size.");

#ifdef CRYPTOLIB_NO_CRYPTORAM_DMA

   CRYPTOLIB_ASSERT(!(src.flags & BLOCK_S_CONST_ADDR), "BLOCK_S_CONST_ADDR not supported by point2CryptoRAM_rev");
   MEMCPY_P(BA414E_ADDR_MEMLOC(offset, size), src.addr, size);
   MEMCPY_P(BA414E_ADDR_MEMLOC(offset + 1, size), src.addr + size, size);

#else //CRYPTOLIB_NO_CRYPTORAM_DMA

   struct dma_sg_descr_s desc_from;
   struct dma_sg_descr_s desc_to[2];
   block_t to[2];

   write_desc_blk(&desc_from, &src, DMA_SG_ENGINESELECT_BYPASS);
   //HACK: the source length can be bigger then the transfer size
   desc_from.length_irq = (2 * size) | (src.flags & BLOCK_S_FLAG_MASK_DMA_PROPS);
   set_last_desc(&desc_from);

   to[0].addr = BA414E_ADDR_MEMLOC(offset, size);
   to[0].len = size;
   to[0].flags = BLOCK_S_INCR_ADDR;
   write_desc_blk(&desc_to[0], &to[0], DMA_SG_ENGINESELECT_BYPASS);

   to[1].addr = BA414E_ADDR_MEMLOC(offset + 1, size);
   to[1].len = size;
   to[1].flags = BLOCK_S_INCR_ADDR;
   write_desc_blk(&desc_to[1], &to[1], DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_to[1]);

   cryptodma_run_sg(&desc_from, desc_to);

#endif //CRYPTOLIB_NO_CRYPTORAM_DMA
}

void CryptoRAM2point_rev(block_t dst, uint32_t size, uint32_t offset)
{
   CRYPTOLIB_ASSERT((dst.len >= size*2), "destination length smaller then point size.");

#ifdef CRYPTOLIB_NO_CRYPTORAM_DMA

   if (!(dst.flags & DMA_AXI_DESCR_DISCARD)) {
      MEMCPY_ROM(dst.addr, BA414E_ADDR_MEMLOC(offset, size), size);
      MEMCPY_ROM(dst.addr + size, BA414E_ADDR_MEMLOC(offset + 1, size), size);
   }

#else //CRYPTOLIB_NO_CRYPTORAM_DMA

   struct dma_sg_descr_s desc_from[2];
   struct dma_sg_descr_s desc_to;
   block_t from[2];

   from[0].addr = BA414E_ADDR_MEMLOC(offset, size);
   from[0].len = size;
   from[0].flags = BLOCK_S_INCR_ADDR;
   write_desc_blk(&desc_from[0], &from[0], DMA_SG_ENGINESELECT_BYPASS);

   from[1].addr = BA414E_ADDR_MEMLOC(offset + 1, size);
   from[1].len = size;
   from[1].flags = BLOCK_S_INCR_ADDR;
   write_desc_blk(&desc_from[1], &from[1], DMA_SG_ENGINESELECT_BYPASS);
   set_last_desc(&desc_from[1]);

   write_desc_blk(&desc_to, &dst, DMA_SG_ENGINESELECT_BYPASS);
   //HACK: the destination length can be bigger then the transfer size
   desc_to.length_irq = (2 * size) | (dst.flags & BLOCK_S_FLAG_MASK_DMA_PROPS);
   set_last_desc(&desc_to);

   cryptodma_run_sg(desc_from, &desc_to);

#endif //CRYPTOLIB_NO_CRYPTORAM_DMA
}

void CryptoRAM2mem_rev(block_t dst, uint32_t size, uint32_t offset)
{
   block_t src;

   src.addr    = BA414E_ADDR_MEMLOC(offset, size);
   src.len     = size;
   src.flags   = BLOCK_S_INCR_ADDR;
   memcpy_blk(dst, src, size);

}

void mem2CryptoRAM(block_t src, uint32_t size, uint32_t offset)
{
   block_t dst;

   dst.addr    = BA414E_ADDR_MEMLOC(offset - 1, 0);
   dst.len     = size;
   dst.flags   = BLOCK_S_INCR_ADDR;
   memcpy_blk(dst, src, size);

}

void CryptoRAM2mem(block_t dst, uint32_t size, uint32_t offset)
{
   block_t src;

   src.addr    = BA414E_ADDR_MEMLOC(offset - 1, 0);
   src.len     = size;
   src.flags   = BLOCK_S_INCR_ADDR;
   memcpy_blk(dst, src, size);

}

uint32_t ba414e_start_wait_status(void)
{
   uint32_t status;
   ba414e_start();
   status = ba414e_wait_status();
   if (status&~(BA414E_STS_SINV_MASK|BA414E_STS_PRIM_MASK|BA414E_STS_NINV_MASK)) {
      CRYPTOLIB_PRINTF("BA414e: error status: %08"PRIX32"\n", status);
   }
   return status;
}

uint32_t ba414e_load_curve(block_t curve, uint32_t size, uint32_t byte_swap, uint32_t gen)
{
   uint32_t i;

   block_t param;
   param.addr  = curve.addr;
   param.len   = curve.len/6;
   param.flags = curve.flags;

   /* Load ECC parameters */
   for (i=0; i < 6; i++) {
      if (gen || (i != 2 && i != 3)) {
         if (byte_swap)
            mem2CryptoRAM_rev(param, size, i);
         else
            mem2CryptoRAM(param, size, i);
      }

      if (!(param.flags & BLOCK_S_CONST_ADDR))
         param.addr += param.len;
   }
   return 0; // todo: check if other error can be detected? if not -> void
}

uint32_t ba414e_load_and_modN(uint8_t outloc, uint8_t nloc, uint32_t size, block_t in, block_t out, uint32_t flags)
{
   ba414e_set_command(BA414E_OPTYPE_MOD_RED_ODD, size, BA414E_BIGEND, flags);
   ba414e_set_config(outloc, outloc, outloc, nloc);
   mem2CryptoRAM_rev(in, size, outloc);
   if(ba414e_start_wait_status())
      return CRYPTOLIB_CRYPTO_ERR;

   CryptoRAM2mem_rev(out, size, outloc);

   return CRYPTOLIB_SUCCESS;
}
