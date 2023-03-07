#ifdef GP_DIVERSITY_JUMPTABLES
#define GP_DIVERSITY_ROM_CODE
#endif //def GP_DIVERSITY_JUMPTABLES

#include "global.h"

/**
 * @file
 * @brief Defines the procedures to make operations with
 *          the BA431 (N)DRNG
 * @copyright Copyright (c) 2016-2018 Silex Inside. All Rights reserved
 */

#include "cryptolib_internal.h"
#include "sx_rng.h"
#include "cryptolib_def.h"
#include "cryptodma.h"
#include "ba431_config.h"
#include "ba414e_config.h"
#include <string.h>
#include "silexCryptoSoc.h"

#ifndef UNITTESTS

STATIC_FUNC Bool sx_rng_enabled(void)
{
    return (ba431_read_controlreg() & BA431_CTRL_NDRNG_ENABLE) == true;
}
/**
* @brief Verify the conditioning function of the BA431 against test patterns.
* @return CRYPTOLIB_SUCCESS if successful CRYPTOLIB_CRYPTO_ERR otherwise
*/
STATIC_VAR_CONST uint32_t test_data[16]
#if defined(GP_DIVERSITY_ROM_CODE)
LINKER_SECTION(".rom")
#endif //defined(GP_DIVERSITY_ROM_CODE)
                                    = {0xE1BCC06B, 0x9199452A, 0x1A7434E1, 0x25199E7F,
                                       0x578A2DAE, 0x9CAC031E, 0xAC6FB79E, 0x518EAF45,
                                       0x461CC830, 0x11E45CA3, 0x19C1FBE5, 0xEF520A1A,
                                       0x45249FF6, 0x179B4FDF, 0x7B412BAD, 0x10376CE6};
STATIC_FUNC uint32_t sx_rng_conditioning_test(void)
{
   const uint32_t known_answer[4]   = {0xA1CAF13F, 0x09AC1F68,0x30CA0E12, 0xA7E18675};
   const uint32_t test_key[4]       = {0x16157E2B, 0xA6D2AE28,0x8815F7AB, 0x3C4FCF09};

   uint32_t i;
   uint32_t error = 0;

   /*Soft reset*/
   ba431_softreset();

   /*Enable test mode*/
   ba431_set_nb128bitblocks(4);
   ba431_set_cond_test_mode();

   /*Write key*/
   ba431_write_conditioning_key(test_key);

   /*Write test data input*/
   for(i=0;i<sizeof(test_data)/4;i++)
      ba431_write_test_data(test_data[i]);

   /*Wait for conditioning test to complete --> wait for return data to appear in FIFO*/
   while(ba431_read_fifolevel()<4);

   /*Clear control register*/
   ba431_clear_ctrl();

   /*Compare results to known answer*/
   for(i=0;i<sizeof(known_answer)/4;i++){
      error |= ba431_read_fifovalue() ^ known_answer[i];
   }
   if (error)
      return CRYPTOLIB_CRYPTO_ERR;

   return CRYPTOLIB_SUCCESS;
}

/**
* @brief Poll for the end of the BA431 startup tests and check result.
* @return CRYPTOLIB_SUCCESS if successful CRYPTOLIB_CRYPTO_ERR otherwise
*/
STATIC_FUNC uint32_t sx_rng_wait_startup(void)
{
   uint32_t ba431_status;
   ba431_state_t ba431_state;

   /*Wait until startup is finished*/
   do{
      ba431_status = ba431_read_status();
      ba431_state = (ba431_state_t) (ba431_status & BA431_STAT_MASK_STATE);
   }while((ba431_state == BA431_STATE_RESET) || (ba431_state == BA431_STATE_STARTUP));

   /*Check startup test result*/
   if(ba431_status & BA431_STAT_MASK_STARTUP_FAIL){
      return CRYPTOLIB_CRYPTO_ERR;
   }

   return CRYPTOLIB_SUCCESS;
}


#if CRYPTOLIB_TEST_ENABLED
   static block_t random_blk = {(uint8_t *) ADDR_BA431_FIFO, 0, BLOCK_S_CONST_ADDR};
   static block_t *rng_source = &random_blk;

   void sx_rng_set_alternative_rng_source(block_t *rng_source_new)
   {
      rng_source = rng_source_new;
   }
   void sx_rng_restore_rng_source(void)
   {
      rng_source = &random_blk;
   }

   void sx_rng_get_rand_blk(block_t dest)
   {
      CRYPTOLIB_ASSERT((rng_source->len>=dest.len) || (rng_source->flags&BLOCK_S_CONST_ADDR), "Not enough data in source!");
#if AIS31_ENABLED
      if (ba431_read_status() & BA431_STAT_MASK_PREALM_INT) {
         CRYPTOLIB_PRINTF("Preliminary noise alarm detected.\n");
         ba431_softreset();
      }
#endif
      if(!sx_rng_enabled())
      {
      /* Re-init because chip could have gone to sleep */
        silexCryptoSoc_Init();

      }


      sx_rng_wait_startup();

      memcpy_blk(dest, *rng_source, dest.len);

      if(!(rng_source->flags&BLOCK_S_CONST_ADDR))
      {
         rng_source->addr += dest.len;
         rng_source->len  -= dest.len;
      }
   }

#else
   static const block_t random_blk = {(uint8_t *) ADDR_BA431_FIFO, 0, BLOCK_S_CONST_ADDR};
   void sx_rng_set_alternative_rng_source(block_t *rng_source_new)
   {
   }

   void sx_rng_restore_rng_source(void)
   {
   }

   void sx_rng_get_rand_blk(block_t dest)
   {
#if AIS31_ENABLED
      if (ba431_read_status() & BA431_STAT_MASK_PREALM_INT) {
         CRYPTOLIB_PRINTF("Preliminary noise alarm detected.\n");
         ba431_softreset();
      }
#endif
      if(!sx_rng_enabled())
      {
        /* Re-init because chip could have gone to sleep */
        silexCryptoSoc_Init();

      }

      sx_rng_wait_startup();

      memcpy_blk(dest, random_blk, dest.len);

   }
#endif


void sx_rng_get_rand(uint8_t * dst, uint32_t size)
{
    sx_rng_get_rand_blk(block_t_convert(dst, size));
}

#endif

/* This function implements one of the methods approved by FIPS 186-4 to
 * generate a random number k with 1 <= k < n.
 *
 * Get a random value rnd of the appropriate length.
 * If rnd > n -2
 *    Try another value
 * Else
 *    k = rnd + 1
 */
uint32_t sx_rng_get_rand_lt_n_blk(block_t dst, block_t n)
{
   CRYPTOLIB_ASSERT_NM(dst.flags == BLOCK_S_INCR_ADDR);
   CRYPTOLIB_ASSERT_NM(n.flags == BLOCK_S_INCR_ADDR);

   if (dst.len != n.len)
      return CRYPTOLIB_INVALID_PARAM;

   if (!(n.addr[n.len -1] & 0x01)) /*n must be odd*/
      return CRYPTOLIB_INVALID_PARAM;

   bool rnd_invalid;

   /* Check what is the most significant bit of n (note that only the highest
    * byte is taken into account!) and compute a mask that can be used to remove
    * the leading zeros. msb_mask will be used to indicate which bits of the
    * most significant byte of the generated random are to be taken into account.
    */
   uint8_t msb_mask;
   for (msb_mask = 0xFF; n.addr[0] & msb_mask; msb_mask <<= 1);

   do {
      /* Get a random value */
      sx_rng_get_rand_blk(dst);


      /* Check if rnd > n-2 (constant time comparison) */
      bool gt = false;
      bool eq = true;

      for (int i = 0; i < dst.len; i++) {
         uint32_t leftop = dst.addr[i];
         uint32_t rightop = n.addr[i];

         /* We rephrase rnd > n-2 as rnd >= n-1. Since n is odd, n-1 is obtained
          * by masking 1 bit.
          */
         if (i == dst.len - 1) {
            rightop &= 0xFE;
         }

         /* We use a trick to determine whether leftop >= rightop to avoid
          * possible time dependency in the implementations of ">", "<" and "==".
          * If leftop > rightop then (rightop - leftop) will be 0xFFFFFFxx.
          * If leftop <= rightop then (rightop - leftop) will be 0x000000xx.
          * By shifting out the xx, we can determine the relation between left
          * and right.
          *
          * A similar trick is used to determine whether leftop == rightop.
          * If leftop == rightop then (rightop ^ leftop) - 1 will be 0xFFFFFFFF.
          * If leftop != rightop then (rightop ^ leftop) - 1 will be 0x000000xx.
          *
          * By muxing eq with eq, we ensure that eq will be zero from the first
          * different byte onwards.
          * By muxing the leftop >= rightop check with eq, we ensure that it
          * only has an effect when executed on the first most significant byte
          * that is different between the arrays.
          */
         gt |= (bool) ((rightop - leftop) >> 8) & eq;
         eq &= (bool) (((rightop ^ leftop) - 1) >> 8);
      }

      rnd_invalid = gt | eq;

   } while (rnd_invalid);

   /* Compute k = rnd + 1 (constant time increment) */
   uint32_t carry = 1;
   /* The LSB are at the end of the array so start there. */
   for (int i = dst.len - 1; i >= 0; i--) {
      uint32_t byte = dst.addr[i];
      uint32_t sum = byte + carry;
      dst.addr[i] = (uint8_t) (sum & 0xFF);
      carry = sum >> 8; /* Will carry "1" over to the next byte if the current
                         * byte was 0xFF. */
   }

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_rng_init(uint32_t cond_test_en, uint32_t clk_div)
{
   uint32_t status = 0;
   uint32_t key[4];

   if(cond_test_en) {
      /*Conditioning function test*/
      status = sx_rng_conditioning_test();
      if(status){
         sx_rng_set_startup_failed(true);
         return CRYPTOLIB_CRYPTO_ERR;
      }
   }

   /*Soft reset*/
   ba431_softreset();

   /*Program powerdown and clock settings*/
   ba431_write_fifo_wakeup_threshold(RNG_FIFO_WAKEUP_LVL);
   if (RNG_OFF_TIMER_VAL<0) {
      ba431_force_run();
   } else {
      ba431_write_switch_off_timer(RNG_OFF_TIMER_VAL);
   }
   ba431_write_clk_div(clk_div);
   ba431_write_init_wait(RNG_INIT_WAIT_VAL);

   /*Configure conditioning test*/
   ba431_set_nb128bitblocks(RNG_NB_128BIT_BLOCKS);

   /*Enable NDRNG*/
   ba431_enable_ndrng();

   /*Check startup tests*/
   status = sx_rng_wait_startup();
   if (status) {
      sx_rng_set_startup_failed(true);
      return CRYPTOLIB_CRYPTO_ERR;
   }

   /*Program random key for the conditioning function*/
   key[0] = ba431_read_fifovalue();
   key[1] = ba431_read_fifovalue();
   key[2] = ba431_read_fifovalue();
   key[3] = ba431_read_fifovalue();
   ba431_write_conditioning_key(key);

   /*Soft reset to flush FIFO*/
   ba431_softreset();

   /*Enable interrupts for health tests (repetition and adaptive proportion tests) & AIS31 test failures */
   ba431_enable_health_test_irq();
#if AIS31_ENABLED
   ba431_enable_AIS31_test_irq();
#endif

   return CRYPTOLIB_SUCCESS;
}

uint32_t sx_rng_get_word(void)
{
   uint32_t rand = 0;
   sx_rng_get_rand((uint8_t*)&rand, sizeof(rand));
   return rand;
}

