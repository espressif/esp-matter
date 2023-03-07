/***************************************************************************//**
 * @file
 * @brief FIPS-180-2 compliant SHA-1 & SHA-256 implementation
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
 *  http://csrc.nist.gov/publications/fips/fips180-2/fips180-2.pdf
 */

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#include "em_device.h"

#if defined(SEMAILBOX_PRESENT)
#include "em_se.h"
#include "security/sha/btl_sha256.h"
#include "mbedtls/error.h"

int sha_x_process(SHA_Type_t algo,
                  uint8_t* state_in,
                  const unsigned char *blockdata,
                  uint8_t* state_out,
                  uint32_t num_blocks)
{
#if defined(_CMU_CLKEN1_SEMAILBOXHOST_MASK)
  CMU->CLKEN1_SET = CMU_CLKEN1_SEMAILBOXHOST;
#endif

  SE_Command_t command = SE_COMMAND_DEFAULT(SE_COMMAND_HASHUPDATE);
  SE_DataTransfer_t data_in = SE_DATATRANSFER_DEFAULT((void *)blockdata, 0);
  SE_DataTransfer_t iv_in = SE_DATATRANSFER_DEFAULT(state_in, 0);
  SE_DataTransfer_t iv_out = SE_DATATRANSFER_DEFAULT(state_out, 0);

  switch (algo) {
    case SHA256:
      command.command |= SE_COMMAND_OPTION_HASH_SHA256;
      // SHA256 block size is 64 bytes
      SE_addParameter(&command, 64 * num_blocks);
      data_in.length |= 64 * num_blocks;
      // SHA256 state size is 32 bytes
      iv_in.length |= 32;
      iv_out.length |= 32;
      break;
    default:
      return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
  }

  SE_addDataInput(&command, &iv_in);
  SE_addDataInput(&command, &data_in);
  SE_addDataOutput(&command, &iv_out);

  SE_executeCommand(&command);
  SE_Response_t res = SE_readCommandResponse();

  if (res == SE_RESPONSE_OK) {
    return 0;
  } else {
    return MBEDTLS_ERR_PLATFORM_HW_ACCEL_FAILED;
  }
}

#endif // #if defined(SEMAILBOX_PRESENT)
