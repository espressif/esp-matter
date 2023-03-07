/***************************************************************************//**
 * @file
 * @brief PSA Driver software GHASH support
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/

// -------------------------------------
// Includes

#include "psa/crypto.h"
#include "sli_psa_driver_common.h"

// Software GHASH is only required when requested on devices needing it
#if (defined(SEMAILBOX_PRESENT)   \
  || defined(CRYPTOACC_PRESENT) ) \
  && defined(SLI_PSA_SUPPORT_GCM_IV_CALCULATION)

#ifndef GET_UINT32_BE
#define GET_UINT32_BE(n, b, i)               \
  {                                          \
    (n) = ( (uint32_t) (b)[(i)] << 24)       \
          | ( (uint32_t) (b)[(i) + 1] << 16) \
          | ( (uint32_t) (b)[(i) + 2] <<  8) \
          | ( (uint32_t) (b)[(i) + 3]);      \
  }
#endif

#ifndef PUT_UINT32_BE
#define PUT_UINT32_BE(n, b, i)                    \
  {                                               \
    (b)[(i)] = (unsigned char) ( (n) >> 24);      \
    (b)[(i) + 1] = (unsigned char) ( (n) >> 16);  \
    (b)[(i) + 2] = (unsigned char) ( (n) >>  8);  \
    (b)[(i) + 3] = (unsigned char) ( (n)       ); \
  }
#endif

/**
 * \brief Initialize Galois field (2^128) multiplication table
 *
 * This function is used as part of a software-based GHASH (as defined in
 * AES-GCM) algorithm, and originates from the mbed TLS implementation in gcm.c
 *
 * It takes the in the 'H' value for the GHASH operation (which is a block of
 * zeroes encrypted using AES-ECB with the key to be used for GHASH/GCM), and
 * converts it into a multiplication table for later use by the multiplication
 * function.
 *
 * \param[in] Ek  'H' value for which to create the multiplication tables
 * \param[out] HL Lower multiplication table for 'H'
 * \param[out] HH Upper multiplication table for 'H'
 */
void sli_psa_software_ghash_setup(const uint8_t Ek[16],
                                  uint64_t HL[16],
                                  uint64_t HH[16])
{
  int i, j;
  uint64_t hi, lo;
  uint64_t vl, vh;

  /* pack Ek as two 64-bits ints, big-endian */
  GET_UINT32_BE(hi, Ek, 0);
  GET_UINT32_BE(lo, Ek, 4);
  vh = (uint64_t) hi << 32 | lo;

  GET_UINT32_BE(hi, Ek, 8);
  GET_UINT32_BE(lo, Ek, 12);
  vl = (uint64_t) hi << 32 | lo;

  /* 8 = 1000 corresponds to 1 in GF(2^128) */
  HL[8] = vl;
  HH[8] = vh;

  /* 0 corresponds to 0 in GF(2^128) */
  HH[0] = 0;
  HL[0] = 0;

  for ( i = 4; i > 0; i >>= 1 ) {
    uint32_t T = (vl & 1) * 0xe1000000U;
    vl  = (vh << 63) | (vl >> 1);
    vh  = (vh >> 1) ^ ( (uint64_t) T << 32);

    HL[i] = vl;
    HH[i] = vh;
  }

  for ( i = 2; i <= 8; i *= 2 ) {
    uint64_t *HiL = HL + i, *HiH = HH + i;
    vh = *HiH;
    vl = *HiL;
    for ( j = 1; j < i; j++ ) {
      HiH[j] = vh ^ HH[j];
      HiL[j] = vl ^ HL[j];
    }
  }
}

static const uint64_t last4[16] =
{
  0x0000, 0x1c20, 0x3840, 0x2460,
  0x7080, 0x6ca0, 0x48c0, 0x54e0,
  0xe100, 0xfd20, 0xd940, 0xc560,
  0x9180, 0x8da0, 0xa9c0, 0xb5e0
};

/**
 * \brief Galois field (2^128) multiplication operation
 *
 * This function is used as part of a software-based GHASH (as defined in
 * AES-GCM) algorithm, and originates from the mbed TLS implementation in gcm.c
 *
 * This function takes in a 128-bit scalar and multiplies it with H (Galois
 * field multiplication as defined in AES-GCM). H is not provided to this
 * function directly. Instead, multiplication tables for the specific H need to
 * be calculated first by \ref sli_psa_software_ghash_setup, and passed to this
 * function.
 *
 * \param[in]   HL      Lower multiplication table for 'H'
 * \param[in]   HH      Upper multiplication table for 'H'
 * \param[out]  output  Output buffer for the multiplication result
 * \param[in]   input   Input buffer for the scalar to multiply
 */
void sli_psa_software_ghash_multiply(const uint64_t HL[16],
                                     const uint64_t HH[16],
                                     uint8_t output[16],
                                     const uint8_t input[16])
{
  int i = 0;
  unsigned char lo, hi, rem;
  uint64_t zh, zl;

  lo = input[15] & 0xf;

  zh = HH[lo];
  zl = HL[lo];

  for ( i = 15; i >= 0; i-- ) {
    lo = input[i] & 0xf;
    hi = (input[i] >> 4) & 0xf;

    if ( i != 15 ) {
      rem = (unsigned char) zl & 0xf;
      zl = (zh << 60) | (zl >> 4);
      zh = (zh >> 4);
      zh ^= (uint64_t) last4[rem] << 48;
      zh ^= HH[lo];
      zl ^= HL[lo];
    }

    rem = (unsigned char) zl & 0xf;
    zl = (zh << 60) | (zl >> 4);
    zh = (zh >> 4);
    zh ^= (uint64_t) last4[rem] << 48;
    zh ^= HH[hi];
    zl ^= HL[hi];
  }

  PUT_UINT32_BE(zh >> 32, output, 0);
  PUT_UINT32_BE(zh, output, 4);
  PUT_UINT32_BE(zl >> 32, output, 8);
  PUT_UINT32_BE(zl, output, 12);
}

#endif // (SEMAILBOX_PRESENT || CRYPTOACC_PRESENT) && SLI_PSA_SUPPORT_GCM_IV_CALCULATION
