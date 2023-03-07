/***************************************************************************//**
 * @file
 * @brief Generic source for random number functions.
 *******************************************************************************
 * # License
 * <b>Copyright 2018 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * The licensor of this software is Silicon Laboratories Inc. Your use of this
 * software is governed by the terms of Silicon Labs Master Software License
 * Agreement (MSLA) available at
 * www.silabs.com/about-us/legal/master-software-license-agreement. This
 * software is distributed to you in Source Code format and is governed by the
 * sections of the MSLA applicable to Source Code.
 *
 ******************************************************************************/
#include <stdint.h>
#include "random.h"

////////////////////////
uint16_t seed0 = 0xbeef;
uint16_t seed1 = 0xface;
#ifdef RNG_SELF_TEST
uint16_t lastval = 0xffff;
#endif

void halStackSeedRandom(uint32_t seed)
{
  seed0 = (uint16_t) seed;
  if (seed0 == 0) {
    seed0 = 0xbeef;
  }
  seed1 = (uint16_t) (seed >> 16);
  if (seed1 == 0) {
    seed1 = 0xface;
  }
}

/*
 *    the stdlib rand() takes about 2.25ms @3.68MHz on the AVR
 *    hell- you can run an software implimentation AES cipher block in 1.5ms!
 *    obviously an LFSR method is much faster (12us on a 4MHz 128AVR actually).
 *    in the FPGA we did the first poly you see here but on a 32bit seed,
 *    taking only the msb word, but we quickly noticed a sequential nature
 *    in the output. so at each random number generation we actually ran the
 *    LFSR 16 times. we can avoid that but still remove the sequential nature
 *    and get a good looking spread by running two LFSRs and XORing them.
 */

// ** NOTE: we might consider replacing the pseudo rng generation by
// LFSR with something like SCG, LCG, or ACG - if the time trade off
// is acceptable.

static uint16_t shift(uint16_t *val, uint16_t taps)
{
  uint16_t newVal = *val;

  if ((newVal & 0x8000) != 0U) {
    newVal ^= taps;
  }
  *val = newVal << 1;
  return newVal;
}

uint16_t halCommonGetRandom(void)
{
  uint16_t val;
  val = (shift(&seed0, 0x0062) ^ shift(&seed1, 0x100B));

  // See Task EFM32ESS-869
  #ifdef RNG_SELF_TEST
  while (lastval == val) {
    val = (shift(&seed0, 0x0062) ^ shift(&seed1, 0x100B));
  }
  lastval = val;
  #endif

  return val;
}
