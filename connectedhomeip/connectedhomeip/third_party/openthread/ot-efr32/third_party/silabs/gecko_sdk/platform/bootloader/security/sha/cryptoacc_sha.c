/***************************************************************************//**
 * @file
 * @brief FIPS-180-2 compliant SHA-1, SHA-224/256, SHA-384/512 implementation
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: APACHE-2.0
 *
 * This software is subject to an open source license and is distributed by
 * Silicon Laboratories Inc. pursuant to the terms of the Apache License,
 * Version 2.0 available at https://www.apache.org/licenses/LICENSE-2.0.
 * Such terms and conditions may be further supplemented by the Silicon Labs
 * Master Software License Agreement (MSLA) available at www.silabs.com and its
 * sections applicable to open source software.
 *
 ******************************************************************************/

/*
 *  The SHA-256 Secure Hash Standard was published by NIST in 2002.
 *
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if defined(CRYPTOACC_PRESENT)
#include "cryptoacc_management.h"
#include "sx_hash.h"
#include "sx_errors.h"
#include "security/sha/btl_sha256.h"
#include "mbedtls/error.h"
#include <string.h>

int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks)
{
  int status;
  uint32_t sx_ret;
  sx_hash_fct_t hash_fct;
  block_t data_in;
  block_t state;
  uint8_t statebuf[SHA256_DIGESTSIZE];

  switch (algo) {
    case SHA256:
      hash_fct    = e_SHA256;
      data_in.len = SHA256_BLOCKSIZE * num_blocks;
      state.len   = SHA256_STATESIZE;
      break;
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  data_in.addr    = (uint8_t*)blockdata;
  data_in.flags   = 0;
  memcpy(statebuf, state_in, state.len);
  state.addr   = (uint8_t*)statebuf;
  state.flags  = 0;

  status = cryptoacc_management_acquire();
  if (status != 0) {
    return status;
  }
  sx_ret = sx_hash_update_blk(hash_fct, state, data_in);
  cryptoacc_management_release();

  if (sx_ret == CRYPTOLIB_SUCCESS) {
    memcpy(state_out, statebuf, state.len);
    return 0;
  } else {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }
}

#endif // #if defined(CRYPTOACC_PRESENT)
